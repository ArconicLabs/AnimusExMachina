# Changelog

All notable changes to AnimusExMachina will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

!!! note
    This page mirrors [CHANGELOG.md](https://github.com/ArconicLabs/AnimusExMachina/blob/main/CHANGELOG.md) from the repository root.

## [Unreleased]

### Removed

- **EngagementRange**: Removed from Config, Config Global Task, and TargetTracking Global Task. Combat sub-StateTrees own their own positioning logic.
- **IsInEngagementRange condition**: Deleted. Combat sub-StateTrees use `DistanceToTarget` directly.

### Changed

- **Combat state**: Simplified from Pursue/Engage child states to a single linked sub-StateTree state. The combat sub-StateTree now owns both positioning and abilities.
- **TargetTracking Global Task**: Outputs reduced to `DistanceToTarget` and `HasLineOfSight`.

### Added (M4 — Combat, Patrol & Leash)

- **Patrol Route**: `AAxMPatrolRoute` — Level-placed actor with a `USplineComponent` defining patrol waypoints. Designers add/move spline control points visually in the editor. Provides `GetPatrolPoints`, `GetNumPatrolPoints`, and `GetPatrolPoint` helpers.
- **State Tasks**:
  - `FAxMTask_Patrol` — Navigates the NPC along an assigned patrol route. Delegate-driven waypoint following with configurable wait duration at each point. Supports loop and ping-pong modes. Succeeds immediately if no route is assigned.
- **Conditions**:
  - `FAxMCondition_IsOutsideLeash` — Returns true when the NPC has exceeded its leash radius from home. Inputs: `DistanceFromHome` (from Perception), `LeashRadius` (from Config). When `LeashRadius` is 0 the leash is disabled and the condition always returns false. Supports `bInvert`.
- **Config**: `PatrolWaitDuration` — how long to wait at each patrol waypoint. `LeashRadius` — max distance from home before leash triggers (0 = disabled).
- **AI Controller**: `PatrolRoute` property (`EditInstanceOnly`) — assign a patrol route per NPC instance in the level.

### Changed (M4)

- **AxMTask_Attack**: Replaced timer-only placeholder with montage-driven combat. Accepts `AttackMontage` input (bound from combat sub-StateTree ability nodes). Plays montage and completes via `OnMontageEnded` delegate. Falls back to timer-based behavior when no montage is assigned. Display name updated from "AxM Attack (Placeholder)" to "AxM Attack".
- **Perception Global Task**: Added `DistanceFromHome` output — computed each tick as the distance between the pawn's current position and its recorded home location.
- **Config Global Task**: Added `LeashRadius` and `PatrolWaitDuration` outputs — read from the Config Data Asset.

### Added (M3 — Data-Driven Config)

- **Data Asset**: `UAxMConfig` Primary Data Asset consolidating all archetype tuning parameters — perception ranges, suspicion thresholds, movement speeds, search params, and alert state gameplay tags.
- **Global Tasks**: `FAxMGlobalTask_Config` — Reads `UAxMConfig` from the AI Controller and exposes all values as bindable outputs.
- **State Tasks**: `FAxMTask_AlertState` — Sets alert state on the controller, applies movement speed from Config.
- **AI Controller**: `Config` property, `SetAlertState`/`GetAlertState`, event delegates (`OnTargetAcquired`, `OnTargetLost`, `OnAlertStateChanged`).
- Module dependency: `GameplayTags`.

### Changed (M3)

- Perception parameters optionally read from `UAxMConfig` Data Asset.
- Categories renamed from `AxM|...` to `Animus Ex Machina|...`.
- `ST_AxM_Master` renamed from `ST_AxM_Sample`. Combat is a single linked sub-StateTree state.
- `AxMTask_MoveTo` simplified to single-outcome with actor→location fallback.
- `AxMTask_SearchArea` converted to fully delegate-driven.

### Added (M2 — Full Perception + Search)

- **AI Controller**: Modular senses (hearing, damage) with toggle flags and pending-flag consume pattern.
- **Global Tasks**: `FAxMGlobalTask_Suspicion` — Hearing-driven suspicion with accumulation/decay.
- **State Tasks**: `FAxMTask_LookAround`, `FAxMTask_SearchArea` (EQS + NavMesh fallback).
- **Conditions**: `FAxMCondition_IsSuspicious`.
- **EQS**: `UAxMEnvQueryContext_SearchCenter`.

### Added (M1 — Core Loop)

- **AI Controller**: `AAxMAIController` with `UStateTreeAIComponent` and `UAIPerceptionComponent` (sight).
- **Global Tasks**: `FAxMGlobalTask_Perception`, `FAxMGlobalTask_TargetTracking`.
- **State Tasks**: `FAxMTask_MoveTo`, `FAxMTask_FaceTarget`, `FAxMTask_Attack` (placeholder).
- **Conditions**: `FAxMCondition_HasTarget`.
- **Content**: Sample AI Controller, NPC, master StateTree, test level.
- `LogAxM` log category. Module dependencies: `AIModule`, `StateTreeModule`, `GameplayStateTreeModule`, `NavigationSystem`.
