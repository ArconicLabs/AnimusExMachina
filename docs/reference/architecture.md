# Architecture

The plugin follows a layered architecture. Perception is the senses, the StateTree is the brain, Tasks are the muscles, and the Data Asset is the DNA.

## Layer Overview

| Layer | Responsibility | Key Classes |
|---|---|---|
| **AI Controller** | Owns the StateTree and Perception components. Entry point. | `AAxMAIController` |
| **Perception** | Configures sight, hearing, damage senses. Sends events to StateTree via Global Tasks. | `UAISense_Sight`, `_Hearing`, `_Damage` |
| **StateTree** | Decision logic. Hierarchical states with event-driven transitions. | `ST_AxM_Master` |
| **Global Tasks** | Persistent tasks computing derived data (target tracking, suspicion, config). Expose bindable outputs. | `FAxMGlobalTask_*` |
| **State Tasks** | Leaf-level execution: MoveTo, Attack, Patrol, Search, LookAround. | `FAxMTask_*` |
| **Conditions** | Transition guards: HasTarget, IsSuspicious, IsOutsideLeash. | `FAxMCondition_*` |
| **Data Assets** | Per-archetype tuning. Designers create new assets, no code changes. | `UAxMConfig` |

## Data Flow

```
AIPerception (sight/hearing/damage events)
    ↓
AAxMAIController (caches per-sense results)
    ↓
FAxMGlobalTask_Perception (reads cache → outputs TargetActor, LastKnownLocation, etc.)
    ↓
FAxMGlobalTask_TargetTracking (computes distance, LOS)
FAxMGlobalTask_Suspicion (accumulates/decays suspicion from hearing)
FAxMGlobalTask_Config (reads UAxMConfig → outputs all tuning values)
    ↓
Conditions (HasTarget, IsSuspicious, IsOutsideLeash)
    ↓
State transitions → Tasks execute (MoveTo, Attack, Patrol, Search)
```

## Perception Model

The plugin uses a **delegate-driven perception** model with **modular senses**:

1. `AAxMAIController` binds `OnTargetPerceptionUpdated` and caches results per sense type
2. Each sense is independently toggleable (`bEnableSight`, `bEnableHearing`, `bEnableDamage`)
3. Global Tasks read from the cache during their StateTree `Tick`, avoiding direct perception queries
4. Hearing events use a pending-flag consume pattern for safe one-shot reads

## Extension Points

### Composable Sub-StateTrees

Key states expose **linked sub-StateTree slots**. Designers compose behavior visually without touching C++:

- **Combat** → Link a combat sub-StateTree for positioning and attack sequences
- **Patrol** → The Patrol task handles waypoint navigation; custom idle behaviors can be added as additional tasks

### Subclassing

Every Task, Global Task, and Condition can be subclassed in Blueprint or C++:

- Override `FAxMTask_Attack` to add custom combat logic
- Override `FAxMTask_SearchArea` to replace the search pattern
- Add new `FStateTreeConditionBase` subclasses for game-specific transition logic

### Custom Global Tasks

Add project-specific Global Tasks to the root state. Their output properties automatically become available for binding throughout the tree, including within linked sub-StateTrees.

### Event Delegates

The AI Controller broadcasts these delegates for external systems:

| Delegate | When |
|---|---|
| `OnTargetAcquired(AActor*)` | First sight/damage perception of a target |
| `OnTargetLost()` | Target lost from perception |
| `OnAlertStateChanged(OldTag, NewTag)` | Alert state transition |

## File Structure

```
Source/AnimusExMachina/
  Public/
    AnimusExMachina.h                          # Module header, LogAxM category
    AI/
      AxMAIController.h                        # AI Controller with modular senses
      AxMPatrolRoute.h                         # Spline-based patrol route actor
      AxMEnvQueryContext_SearchCenter.h         # EQS context for search area
    Config/
      AxMConfig.h                              # Primary Data Asset for tuning
    StateTree/
      Tasks/
        AxMGlobalTask_Perception.h             # Multi-sense target acquisition
        AxMGlobalTask_Suspicion.h              # Suspicion accumulation and decay
        AxMGlobalTask_TargetTracking.h         # Distance, LOS computation
        AxMGlobalTask_Config.h                 # Config value exposure
        AxMTask_MoveTo.h                       # Navigation via PathFollowing delegate
        AxMTask_FaceTarget.h                   # Focus-based rotation
        AxMTask_Attack.h                       # Montage-driven attack (timer fallback)
        AxMTask_Patrol.h                       # Spline waypoint navigation
        AxMTask_LookAround.h                   # Investigation area scanning
        AxMTask_SearchArea.h                   # EQS/NavMesh search pattern
      Conditions/
        AxMCondition_HasTarget.h               # TargetActor validity check
        AxMCondition_IsSuspicious.h            # Suspicion threshold check
        AxMCondition_IsOutsideLeash.h          # Leash distance check
  Private/
    (mirrors Public structure)
```
