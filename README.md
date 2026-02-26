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

The plugin uses a **delegate-driven perception** model with **modular senses** for scalability:

1. `AAxMAIController` binds `OnTargetPerceptionUpdated` and **caches** perception results per sense type (sight, hearing, damage — each toggleable)
2. `FAxMGlobalTask_Perception` reads from the cache during its StateTree `Tick`, exposing target, stimulus, and hearing data
3. `FAxMGlobalTask_Suspicion` accumulates and decays suspicion from hearing events
4. `FAxMGlobalTask_TargetTracking` computes derived values (distance, line-of-sight, range checks)
5. State tasks and conditions drive transitions between behavioral states

```
AIController (modular senses: sight, hearing, damage — each toggleable)
    └── StateTree
        ├── Global: Perception    → TargetActor, LastKnownLocation, StimulusLocation, HearingStrength
        ├── Global: Suspicion     → SuspicionLevel, bIsSuspicious
        ├── Global: TargetTracking → DistanceToTarget, HasLineOfSight, IsInAttackRange
        │
        ├── Patrol (default) ──[HasTarget]──→ Combat
        │                    ──[IsSuspicious]──→ Investigate
        ├── Investigate ──[HasTarget]──→ Combat
        │               ──[!IsSuspicious]──→ Patrol
        ├── Combat ──[!HasTarget]──→ Search
        │   ├── Pursue ──[IsInAttackRange]──→ Engage
        │   └── Engage ──[!IsInAttackRange]──→ Pursue
        └── Search ──[HasTarget]──→ Combat
                   ──[SearchComplete]──→ Patrol
```

## File Structure

```
Source/AnimusExMachina/
  Public/
    AnimusExMachina.h                                # Module header, LogAxM category
    AI/
      AxMAIController.h                              # AI Controller with modular senses
      AxMEnvQueryContext_SearchCenter.h               # EQS context for search area
    StateTree/
      Tasks/
        AxMGlobalTask_Perception.h                   # Multi-sense target acquisition
        AxMGlobalTask_Suspicion.h                    # Suspicion accumulation and decay
        AxMGlobalTask_TargetTracking.h               # Distance, LOS, range computation
        AxMTask_MoveTo.h                             # Navigation via PathFollowing delegate
        AxMTask_FaceTarget.h                         # Focus-based rotation
        AxMTask_Attack.h                             # Placeholder timer-based attack
        AxMTask_LookAround.h                         # Investigation area scanning
        AxMTask_SearchArea.h                         # EQS/NavMesh search pattern
        AxMTask_DebugState.h                         # In-viewport debug state label
      Conditions/
        AxMCondition_HasTarget.h                     # TargetActor validity check
        AxMCondition_IsInAttackRange.h               # Range bool passthrough
        AxMCondition_IsSuspicious.h                  # Suspicion threshold check
  Private/
    (mirrors Public structure)
Content/
  BP_AxM_AIController.uasset                         # Sample AI Controller blueprint
  BP_AxM_SampleNPC.uasset                            # Sample NPC character
  ST_AxM_Sample.uasset                               # Sample StateTree asset
  Lvl_AxM_SampleThirdPerson.umap                     # Sample test level
```

## Key Classes

### AI Controller

| Class | Description |
|---|---|
| `AAxMAIController` | Owns `UStateTreeAIComponent` and `UAIPerceptionComponent` with modular senses (sight, hearing, damage). Toggle flags and parameters exposed for Blueprint subclasses. |

### StateTree Global Tasks

| Class | Outputs |
|---|---|
| `FAxMGlobalTask_Perception` | `TargetActor`, `LastKnownLocation`, `HomeLocation`, `StimulusLocation`, `HearingStrength` |
| `FAxMGlobalTask_Suspicion` | `SuspicionLevel`, `bIsSuspicious` |
| `FAxMGlobalTask_TargetTracking` | `DistanceToTarget`, `HasLineOfSight`, `IsInAttackRange` |

### StateTree State Tasks

| Class | Description |
|---|---|
| `FAxMTask_MoveTo` | Async navigation via `PathFollowingComponent::OnRequestFinished` |
| `FAxMTask_FaceTarget` | Rotates pawn to face target using AIController focus |
| `FAxMTask_Attack` | Placeholder timer-based attack (M3 will add montage support) |
| `FAxMTask_LookAround` | Scans area by rotating through focal points over a duration |
| `FAxMTask_SearchArea` | EQS-driven or random NavMesh search pattern with duration limit |
| `FAxMTask_DebugState` | Draws a color-coded label above the NPC via `DrawDebugString` (dev builds only) |

### StateTree Conditions

| Class | Description |
|---|---|
| `FAxMCondition_HasTarget` | True when `TargetActor` is valid. Supports `bInvert`. |
| `FAxMCondition_IsInAttackRange` | Passes through bound bool from TargetTracking. Supports `bInvert`. |
| `FAxMCondition_IsSuspicious` | Passes through `bIsSuspicious` from Suspicion. Supports `bInvert`. |

### EQS

| Class | Description |
|---|---|
| `UAxMEnvQueryContext_SearchCenter` | Provides the cached last known location as a point context for search queries. |

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
