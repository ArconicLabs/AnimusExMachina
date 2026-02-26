# Changelog

All notable changes to AnimusExMachina will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- **Patrol Route**: `AAxMPatrolRoute` — Level-placed actor with a `USplineComponent` defining patrol waypoints. Designers add/move spline control points visually in the editor. Provides `GetPatrolPoints`, `GetNumPatrolPoints`, and `GetPatrolPoint` helpers.
- **State Tasks**:
  - `FAxMTask_Patrol` — Navigates the NPC along an assigned patrol route. Delegate-driven waypoint following with configurable wait duration at each point. Supports loop and ping-pong modes. Succeeds immediately if no route is assigned.
- **Conditions**:
  - `FAxMCondition_IsOutsideLeash` — Returns true when the NPC has exceeded its leash radius from home. Inputs: `DistanceFromHome` (from Perception), `LeashRadius` (from Config). When `LeashRadius` is 0 the leash is disabled and the condition always returns false. Supports `bInvert`.
- **Config**: `PatrolWaitDuration` — how long to wait at each patrol waypoint. `LeashRadius` — max distance from home before leash triggers (0 = disabled).
- **AI Controller**: `PatrolRoute` property (`EditInstanceOnly`) — assign a patrol route per NPC instance in the level.

### Changed

- **AxMTask_Attack**: Replaced timer-only placeholder with montage-driven combat. Accepts `AttackMontage` input (bound from combat sub-StateTree ability nodes). Plays montage and completes via `OnMontageEnded` delegate. Falls back to timer-based behavior when no montage is assigned. Display name updated from "AxM Attack (Placeholder)" to "AxM Attack".
- **Perception Global Task**: Added `DistanceFromHome` output — computed each tick as the distance between the pawn's current position and its recorded home location.
- **Config Global Task**: Added `LeashRadius` and `PatrolWaitDuration` outputs — read from the Config Data Asset.

### Added

- **Data Asset**: `UAxMConfig` Primary Data Asset consolidating all archetype tuning parameters — perception ranges, suspicion thresholds, engagement range, movement speeds, search params, and alert state gameplay tags. Assign to an AI Controller to drive all behavior from a single asset.
- **Global Tasks**:
  - `FAxMGlobalTask_Config` — Reads `UAxMConfig` from the AI Controller and exposes all values as bindable outputs for the rest of the tree. Replaces inline parameter values with data-driven bindings.
- **State Tasks**:
  - `FAxMTask_AlertState` — Lightweight task placed in each top-level state that calls `SetAlertState` on the controller. Broadcasts `OnAlertStateChanged` and applies movement speed from Config.
- **AI Controller**:
  - `Config` property — optional `UAxMConfig` Data Asset that overrides loose perception properties when assigned.
  - `SetAlertState` / `GetAlertState` — tracks current alert state as a `FGameplayTag` and applies movement speed from Config.
  - Event delegates: `OnTargetAcquired`, `OnTargetLost`, `OnAlertStateChanged` — `BlueprintAssignable` multicast delegates for game code integration.
- **Conditions**:
  - `FAxMCondition_IsInEngagementRange` — Replaces `IsInAttackRange`. Passes through a bound `IsInEngagementRange` bool. Supports `bInvert`.
- Module dependency: `GameplayTags`.

### Changed

- **EngagementRange**: Renamed `AttackRange` to `EngagementRange` throughout (TargetTracking global task parameter and output, condition). Engagement range is archetype-level (when the NPC enters combat); per-attack ranges live in combat sub-StateTrees.
- **AI Controller**: Perception parameters now optionally read from `UAxMConfig` Data Asset in `PostInitializeComponents`. Loose UPROPERTY fields remain as fallback when no Config is assigned. Sight perception handler fires `OnTargetAcquired`/`OnTargetLost` delegates on target transitions.
- **Categories**: Renamed all UPROPERTY/USTRUCT categories from `AxM|...` to `Animus Ex Machina|...` to prevent Unreal editor camelCase splitting.
- **ST_AxM_Master**: Renamed from `ST_AxM_Sample` to reflect its role as the shared master StateTree template. Added explicit self-transitions on Pursue and Engage states so combat tasks re-execute continuously via the StateTree rather than internal looping.
- **AxMTask_MoveTo**: Simplified to a single-outcome task — issues one move request and reports Succeeded or Failed. `AlreadyAtGoal` returns `Succeeded` unconditionally. Delegate is a straightforward `FinishTask` reporter with no re-issuing. `ExitState` removes delegate before `StopMovement` to prevent cleanup-triggered callbacks. Continuous following is now the StateTree's responsibility via self-transitions. `MoveToActor` now falls back to `MoveToLocation` (bound `TargetLocation`) when the actor's position is unreachable (e.g. off-navmesh).
- **AxMTask_SearchArea**: Replaced Tick-based polling with fully delegate-driven approach. Move chaining uses `OnRequestFinished` delegate; search duration uses `FTimerHandle` instead of manual elapsed time accumulation. `Tick` override removed entirely.
- **AxMTask_Attack**: Returns `Succeeded` after `AttackDuration` expires. Continuous attacking is the StateTree's responsibility via self-transitions on the Engage state.
- **AxMTask_LookAround**: Returns `Succeeded` after `LookDuration` expires. Continuous scanning is the StateTree's responsibility via self-transitions on the ScanArea state.

### Added

- **AI Controller**: Modular sense system with per-subclass toggle flags (`bEnableSight`, `bEnableHearing`, `bEnableDamage`). Configurable sense parameters exposed as `EditDefaultsOnly` properties. Hearing and damage events cached with pending-flag consume pattern for safe Global Task reads.
  - `UAISenseConfig_Hearing` — Hearing sense with configurable range and max age.
  - `UAISenseConfig_Damage` — Damage sense for immediate target acquisition or investigation.
  - Sense configuration moved to `PostInitializeComponents` so Blueprint CDO overrides are applied.
- **Global Tasks**:
  - `FAxMGlobalTask_Suspicion` — Manages a suspicion level (0–1) driven by hearing events. Accumulates from `HearingStrength` input, decays per-second at configurable rate. Outputs `SuspicionLevel` and `bIsSuspicious` threshold flag for driving Patrol → Investigate transitions.
- **State Tasks**:
  - `FAxMTask_LookAround` — Rotates the AI pawn through three focal points (left, right, forward) over a configurable duration then succeeds. Used in Investigate state.
  - `FAxMTask_SearchArea` — Navigates the NPC to successive search points around a center location. Supports EQS-driven queries (assign a `QueryAsset`) or random NavMesh fallback. Delegate-driven move chaining with timer-based duration. Succeeds when `SearchDuration` expires.
- **Conditions**:
  - `FAxMCondition_IsSuspicious` — Passes through `bIsSuspicious` from the Suspicion global task. Supports `bInvert`.
- **EQS**:
  - `UAxMEnvQueryContext_SearchCenter` — EQS context providing the AI Controller's cached last known location as a point for search queries.

### Changed

- **AI Controller**: `AAxMAIController` now supports sight, hearing, and damage senses (previously sight only). All three default to enabled. Perception delegate dispatches by sense type.
- **Perception Global Task**: `FAxMGlobalTask_Perception` now outputs `StimulusLocation` and `HearingStrength` in addition to existing `TargetActor`, `LastKnownLocation`, and `HomeLocation`. Consumes hearing and damage events from the controller's pending cache.

### Previously Added (M1)

- **AI Controller**: `AAxMAIController` with `UStateTreeAIComponent` and `UAIPerceptionComponent` (sight sense). Caches perception results via `OnTargetPerceptionUpdated` delegate for event-driven target acquisition.
- **Global Tasks**:
  - `FAxMGlobalTask_Perception` — Reads cached perception data from the AI Controller. Outputs `TargetActor`, `LastKnownLocation`, and `HomeLocation`.
  - `FAxMGlobalTask_TargetTracking` — Computes `DistanceToTarget`, `HasLineOfSight`, and `IsInEngagementRange` from a bound `TargetActor`.
- **State Tasks**:
  - `FAxMTask_MoveTo` — Navigates to a target actor or location. Uses `PathFollowingComponent::OnRequestFinished` delegate for async completion.
  - `FAxMTask_FaceTarget` — Rotates the AI pawn to face a target actor via the AIController focus system.
  - `FAxMTask_Attack` — Placeholder attack task that runs for a configurable duration then succeeds.
- **Conditions**:
  - `FAxMCondition_HasTarget` — Returns true when a bound `TargetActor` is valid. Supports `bInvert`.
  - `FAxMCondition_IsInEngagementRange` — Passes through a bound `IsInEngagementRange` bool. Supports `bInvert`.
- **Content**:
  - `BP_AxM_AIController` — Sample AI Controller blueprint.
  - `ST_AxM_Master` — Pre-wired master StateTree template (Patrol → Pursue ↔ Engage with global perception and target tracking).
  - `BP_AxM_SampleNPC` — Sample NPC character with Animation Blueprint and movement configured for AI (Use Acceleration for Paths enabled).
  - `Lvl_AxM_SampleThirdPerson` — Test level with NavMesh for AI navigation.
- `LogAxM` log category for plugin-wide logging.
- Module dependencies: `AIModule`, `StateTreeModule`, `GameplayStateTreeModule`, `NavigationSystem`.
- Plugin dependencies: `StateTree`, `GameplayStateTree`.
