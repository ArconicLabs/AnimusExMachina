# AnimusExMachina

A drop-in StateTree-based AI plugin for Unreal Engine 5. Provides a complete NPC behavior loop — perception, target tracking, navigation, and combat — driven entirely by StateTree global tasks, state tasks, and conditions.

## Installation

Clone this repo into your project's `Plugins/` directory:

```bash
cd YourProject/Plugins
git clone https://github.com/ArconicLabs/AnimusExMachina.git
```

Then enable the plugin in your `.uproject`:

```json
{
  "Name": "AnimusExMachina",
  "Enabled": true
}
```

Rebuild your project and you're good to go.

## Architecture

The plugin uses a **delegate-driven perception** model for scalability with many concurrent agents:

1. `AAxMAIController` binds `OnTargetPerceptionUpdated` and **caches** perception results (event-driven, no per-frame queries)
2. `FAxMGlobalTask_Perception` reads from the cache during its StateTree `Tick` (safe for instance data writes)
3. `FAxMGlobalTask_TargetTracking` computes derived values (distance, line-of-sight, range checks)
4. State tasks (`MoveTo`, `FaceTarget`, `Attack`) and conditions (`HasTarget`, `IsInAttackRange`) drive transitions

```
AIController (event-driven perception cache)
    └── StateTree
        ├── Global: Perception → TargetActor, LastKnownLocation, HomeLocation
        ├── Global: TargetTracking → DistanceToTarget, HasLineOfSight, IsInAttackRange
        │
        ├── Patrol (default) ──[HasTarget]──→ Combat
        └── Combat ──[!HasTarget]──→ Patrol
            ├── Pursue (MoveTo + FaceTarget) ──[IsInAttackRange]──→ Engage
            └── Engage (FaceTarget + Attack) ──[!IsInAttackRange]──→ Pursue
```

## File Structure

```
Source/AnimusExMachina/
  Public/
    AnimusExMachina.h                                # Module header, LogAxM category
    AI/
      AxMAIController.h                              # AI Controller with StateTree + Perception
    StateTree/
      Tasks/
        AxMGlobalTask_Perception.h                   # Sight-driven target acquisition
        AxMGlobalTask_TargetTracking.h               # Distance, LOS, range computation
        AxMTask_MoveTo.h                             # Navigation via PathFollowing delegate
        AxMTask_FaceTarget.h                         # Focus-based rotation
        AxMTask_Attack.h                             # Placeholder timer-based attack
      Conditions/
        AxMCondition_HasTarget.h                     # TargetActor validity check
        AxMCondition_IsInAttackRange.h               # Range bool passthrough
  Private/
    AnimusExMachina.cpp                              # Module implementation
    AI/
      AxMAIController.cpp
    StateTree/
      Tasks/
        AxMGlobalTask_Perception.cpp
        AxMGlobalTask_TargetTracking.cpp
        AxMTask_MoveTo.cpp
        AxMTask_FaceTarget.cpp
        AxMTask_Attack.cpp
      Conditions/
        AxMCondition_HasTarget.cpp
        AxMCondition_IsInAttackRange.cpp
Content/
  BP_AxM_AIController.uasset                         # Sample AI Controller blueprint
  ST_AxM_Sample.uasset                               # Sample StateTree asset
```

## Key Classes

### AI Controller

| Class | Description |
|---|---|
| `AAxMAIController` | Owns `UStateTreeAIComponent` and `UAIPerceptionComponent` with sight sense. Caches target data via delegate. |

### StateTree Global Tasks

| Class | Outputs |
|---|---|
| `FAxMGlobalTask_Perception` | `TargetActor`, `LastKnownLocation`, `HomeLocation` |
| `FAxMGlobalTask_TargetTracking` | `DistanceToTarget`, `HasLineOfSight`, `IsInAttackRange` |

### StateTree State Tasks

| Class | Description |
|---|---|
| `FAxMTask_MoveTo` | Async navigation via `PathFollowingComponent::OnRequestFinished` |
| `FAxMTask_FaceTarget` | Rotates pawn to face target using AIController focus |
| `FAxMTask_Attack` | Placeholder timer-based attack (M3 will add montage support) |

### StateTree Conditions

| Class | Description |
|---|---|
| `FAxMCondition_HasTarget` | True when `TargetActor` is valid. Supports `bInvert`. |
| `FAxMCondition_IsInAttackRange` | Passes through bound bool from TargetTracking. Supports `bInvert`. |

## Quick Start

1. Create a Blueprint subclass of `AAxMAIController`
2. Assign a StateTree asset to the `StateTreeAI` component (or use the included `ST_AxM_Sample`)
3. Create an NPC Character Blueprint, set its **AI Controller Class** to your controller BP
4. Set **Auto Possess AI** to `Placed in World`
5. Place the NPC on a NavMesh and press Play

## Dependencies

- **Modules**: `AIModule`, `StateTreeModule`, `GameplayStateTreeModule`, `NavigationSystem`
- **Plugins**: `StateTree`, `GameplayStateTree`
- **Engine**: Unreal Engine 5.5+

## Sample Project

See [AxMSample](https://github.com/ArconicLabs/AxMSample) for a full example project using this plugin.

## License

Apache 2.0 — see [LICENSE](LICENSE) for details.
