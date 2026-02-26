# Changelog

All notable changes to AnimusExMachina will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Changed

- **AxMTask_MoveTo**: Fully delegate-driven async completion with no Tick dependency. Continuous follow (TargetActor valid) re-issues `MoveToActor` only on non-success results (path aborted/blocked); on success the NPC has arrived and state transitions handle the next step. One-shot (no TargetActor) calls `FinishTask` on completion. Re-entrancy safe — `AlreadyAtGoal` fires `OnRequestFinished` synchronously with Success, which the delegate correctly skips. `ExitState` removes delegate before `StopMovement` to prevent cleanup-triggered callbacks.
- **AxMTask_SearchArea**: Replaced Tick-based polling with fully delegate-driven approach. Move chaining uses `OnRequestFinished` delegate; search duration uses `FTimerHandle` instead of manual elapsed time accumulation. `Tick` override removed entirely.
- **AxMTask_Attack**: Now loops its attack action (resets timer, continues Running) instead of completing after the duration. Prevents state completion from bubbling up to Patrol when the Engage state has no `On State Completed` handler.
- **AxMTask_LookAround**: Now cycles focal points continuously instead of completing after the duration. Prevents premature exit from ScanArea via completion bubbling.

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
  - `FAxMGlobalTask_TargetTracking` — Computes `DistanceToTarget`, `HasLineOfSight`, and `IsInAttackRange` from a bound `TargetActor`.
- **State Tasks**:
  - `FAxMTask_MoveTo` — Navigates to a target actor or location. Uses `PathFollowingComponent::OnRequestFinished` delegate for async completion.
  - `FAxMTask_FaceTarget` — Rotates the AI pawn to face a target actor via the AIController focus system.
  - `FAxMTask_Attack` — Placeholder attack task that runs for a configurable duration then succeeds.
- **Conditions**:
  - `FAxMCondition_HasTarget` — Returns true when a bound `TargetActor` is valid. Supports `bInvert`.
  - `FAxMCondition_IsInAttackRange` — Passes through a bound `IsInAttackRange` bool. Supports `bInvert`.
- **Content**:
  - `BP_AxM_AIController` — Sample AI Controller blueprint.
  - `ST_AxM_Sample` — Pre-wired StateTree asset (Patrol → Pursue ↔ Engage with global perception and target tracking).
  - `BP_AxM_SampleNPC` — Sample NPC character with Animation Blueprint and movement configured for AI (Use Acceleration for Paths enabled).
  - `Lvl_AxM_SampleThirdPerson` — Test level with NavMesh for AI navigation.
- `LogAxM` log category for plugin-wide logging.
- Module dependencies: `AIModule`, `StateTreeModule`, `GameplayStateTreeModule`, `NavigationSystem`.
- Plugin dependencies: `StateTree`, `GameplayStateTree`.
