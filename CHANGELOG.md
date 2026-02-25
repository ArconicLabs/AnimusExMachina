# Changelog

All notable changes to AnimusExMachina will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

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
- **Content**: Sample `BP_AxM_AIController` blueprint and `ST_AxM_Sample` StateTree asset.
- `LogAxM` log category for plugin-wide logging.
- Module dependencies: `AIModule`, `StateTreeModule`, `GameplayStateTreeModule`, `NavigationSystem`.
- Plugin dependencies: `StateTree`, `GameplayStateTree`.
