# Changelog

All notable changes to AnimusExMachina will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- `AAxMAIController` — AI Controller with `UStateTreeAIComponent` and `UAIPerceptionComponent` (sight sense). Caches perception results via `OnTargetPerceptionUpdated` delegate for event-driven target acquisition.
- `FAxMGlobalTask_Perception` — Global StateTree task that reads cached perception data from the AI Controller. Outputs `TargetActor`, `LastKnownLocation`, and `HomeLocation` for property binding.
- `FAxMGlobalTask_TargetTracking` — Global StateTree task that computes `DistanceToTarget`, `HasLineOfSight`, and `IsInAttackRange` from a bound `TargetActor` input.
- `FAxMTask_MoveTo` — State-level task that navigates to a target actor or location. Uses `PathFollowingComponent::OnRequestFinished` delegate for async completion without polling.
- `FAxMTask_FaceTarget` — State-level task that rotates the AI pawn to face a target actor via the AIController focus system.
- `FAxMTask_Attack` — Placeholder attack task that runs for a configurable duration then succeeds. Logs to `LogAxM`.
- `FAxMCondition_HasTarget` — Condition that returns true when a bound `TargetActor` is valid.
- `FAxMCondition_IsInAttackRange` — Condition that passes through a bound `IsInAttackRange` bool from the TargetTracking global task.
- `LogAxM` log category for plugin-wide logging.
- Module dependencies for `AIModule`, `StateTreeModule`, `GameplayStateTreeModule`, and `NavigationSystem`.
- Plugin dependencies on `StateTree` and `GameplayStateTree` plugins.
