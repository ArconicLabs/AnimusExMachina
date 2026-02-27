# Animus Ex Machina

A drop-in StateTree-based AI plugin for Unreal Engine 5. Provides a complete NPC behavior loop — perception, target tracking, navigation, and combat — driven entirely by StateTree global tasks, state tasks, and conditions.

## What It Does

Drop the plugin into your project, assign the AI Controller to any Character, and get working enemy AI with zero configuration. The plugin handles:

- **Patrol** — Spline-based patrol routes with configurable wait times
- **Investigate** — Hearing-driven suspicion with stimulus investigation
- **Combat** — Montage-driven attacks with composable combat sub-StateTrees
- **Search** — EQS-driven or random NavMesh search patterns
- **Leash** — Distance-based return-to-home system

## Why StateTree?

StateTree is UE5's modern alternative to Behavior Trees. Where a BT re-evaluates from the top every tick, a StateTree only recalculates when conditions change. AxM leverages this for AI that is easier to understand, debug, and extend.

| Dimension | Behavior Tree | StateTree |
|---|---|---|
| **Evaluation** | Top-down every tick | Event-driven transitions |
| **Readability** | Wide/deep tree with decorators | Flat state list with explicit transitions |
| **Data Flow** | Blackboard keys (stringly typed) | Typed properties with direct binding |
| **Debugging** | Node execution trace | Active state highlight + property watch |

## Quick Overview

```
AIController (modular senses: sight, hearing, damage)
    └── StateTree
        ├── Global: Perception     → TargetActor, LastKnownLocation, DistanceFromHome
        ├── Global: Suspicion      → SuspicionLevel, bIsSuspicious
        ├── Global: TargetTracking → DistanceToTarget, HasLineOfSight, IsInEngagementRange
        ├── Global: Config         → EngagementRange, LeashRadius, PatrolWaitDuration, ...
        │
        ├── Patrol ──[HasTarget]──→ Combat
        │           ──[IsSuspicious]──→ Investigate
        ├── Investigate ──[HasTarget]──→ Combat
        │                ──[!IsSuspicious]──→ Patrol
        ├── Combat ──[!HasTarget]──→ Search
        │          ──[IsOutsideLeash]──→ Patrol
        │   ├── Pursue ──[IsInRange]──→ Engage
        │   └── Engage ──[!IsInRange]──→ Pursue
        └── Search ──[HasTarget]──→ Combat
                   ──[IsOutsideLeash]──→ Patrol
                   ──[SearchComplete]──→ Patrol
```

## Design Principles

- **Data-driven** — All tuning lives in a `UAxMConfig` Data Asset. Create enemy archetypes by duplicating an asset, not modifying code.
- **Composable** — Key states expose sub-StateTree slots. Swap in custom attack sequences, search patterns, or patrol behaviors as standalone assets.
- **Extensible** — Every Task, Global Task, and Condition is designed to be subclassed in Blueprint or C++.
- **Educational** — Well-commented code and clear naming serve as a StateTree learning resource.

## Links

- [GitHub Repository](https://github.com/ArconicLabs/AnimusExMachina)
- [Sample Project](https://github.com/ArconicLabs/AxMSample)
- [Changelog](changelog.md)
