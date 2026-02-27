# API Reference

## AI Controller

### AAxMAIController

The core AI Controller. Owns the StateTree and Perception components, caches perception results, and manages alert state.

**Properties (Config):**

| Property | Type | Description |
|---|---|---|
| `Config` | `UAxMConfig*` | Optional archetype config Data Asset |
| `PatrolRoute` | `AAxMPatrolRoute*` | Patrol route for this NPC instance (`EditInstanceOnly`) |

**Properties (Perception — fallback when Config is null):**

| Property | Type | Default |
|---|---|---|
| `bEnableSight` | bool | true |
| `bEnableHearing` | bool | true |
| `bEnableDamage` | bool | true |
| `SightRadius` | float | 1500.0 cm |
| `LoseSightRadius` | float | 2000.0 cm |
| `PeripheralVisionAngle` | float | 60.0 deg |
| `SightMaxAge` | float | 5.0 s |
| `HearingRange` | float | 1500.0 cm |
| `HearingMaxAge` | float | 3.0 s |

**Functions:**

| Function | Returns | Description |
|---|---|---|
| `GetConfig()` | `UAxMConfig*` | Returns the assigned config (may be null) |
| `GetPatrolRoute()` | `AAxMPatrolRoute*` | Returns the assigned patrol route (may be null) |
| `SetAlertState(Tag)` | void | Sets alert state, applies movement speed, broadcasts delegate |
| `GetAlertState()` | `FGameplayTag` | Returns current alert state tag |
| `GetCachedTargetActor()` | `AActor*` | Most recent sight/damage target |
| `GetCachedLastKnownLocation()` | `FVector` | Last known target position |

**Delegates:**

| Delegate | Signature | When |
|---|---|---|
| `OnTargetAcquired` | `(AActor* Target)` | Target first perceived |
| `OnTargetLost` | `()` | Target lost from perception |
| `OnAlertStateChanged` | `(FGameplayTag Old, FGameplayTag New)` | Alert state transition |

---

### AAxMPatrolRoute

Level-placed actor with a spline component defining patrol waypoints.

| Function | Returns | Description |
|---|---|---|
| `GetPatrolPoints()` | `TArray<FVector>` | World-space positions of all control points |
| `GetNumPatrolPoints()` | `int32` | Number of patrol points |
| `GetPatrolPoint(Index)` | `FVector` | Specific point by index |

---

## Global Tasks

### FAxMGlobalTask_Perception

Reads cached perception from the AI Controller each tick.

| Field | Category | Type | Description |
|---|---|---|---|
| Controller | Context | `AAIController*` | AI Controller to read from |
| TargetActor | Output | `AActor*` | Currently perceived target |
| LastKnownLocation | Output | `FVector` | Last known target position |
| HomeLocation | Output | `FVector` | NPC spawn location (set once) |
| DistanceFromHome | Output | `float` | Current distance from home |
| StimulusLocation | Output | `FVector` | Most recent hearing/damage stimulus |
| HearingStrength | Output | `float` | Strength of last hearing event |

### FAxMGlobalTask_Suspicion

Accumulates and decays suspicion from hearing events.

| Field | Category | Type | Description |
|---|---|---|---|
| HearingStrength | Input | `float` | Bind from Perception output |
| SuspicionDecayRate | Parameter | `float` | Per-second decay (default 0.15) |
| InvestigateThreshold | Parameter | `float` | Threshold for bIsSuspicious (default 0.4) |
| SuspicionGainMultiplier | Parameter | `float` | Multiplier on hearing gain (default 1.0) |
| SuspicionLevel | Output | `float` | Current suspicion (0.0–1.0) |
| bIsSuspicious | Output | `bool` | True when above threshold |

### FAxMGlobalTask_TargetTracking

Computes derived target data when a target exists.

| Field | Category | Type | Description |
|---|---|---|---|
| Controller | Context | `AAIController*` | AI Controller for LOS traces |
| TargetActor | Input | `AActor*` | Bind from Perception output |
| DistanceToTarget | Output | `float` | Current distance to target |
| HasLineOfSight | Output | `bool` | Clear LOS to target |

### FAxMGlobalTask_Config

Reads `UAxMConfig` from the AI Controller and exposes all values as outputs.

| Field | Category | Type | Description |
|---|---|---|---|
| Controller | Context | `AAIController*` | AI Controller to read Config from |
| LeashRadius | Output | `float` | Max distance from home (0 = disabled) |
| PatrolWaitDuration | Output | `float` | Wait time at each patrol waypoint |
| PatrolMovementSpeed | Output | `float` | Speed during Patrol |
| InvestigateMovementSpeed | Output | `float` | Speed during Investigate |
| CombatMovementSpeed | Output | `float` | Speed during Combat |
| SearchMovementSpeed | Output | `float` | Speed during Search |
| SearchRadius | Output | `float` | Search area radius |
| SearchDuration | Output | `float` | Search time limit |
| SuspicionDecayRate | Output | `float` | Suspicion decay rate |
| InvestigateThreshold | Output | `float` | Suspicion investigation threshold |
| SuspicionGainMultiplier | Output | `float` | Hearing gain multiplier |
| PatrolTag | Output | `FGameplayTag` | Patrol alert state tag |
| InvestigateTag | Output | `FGameplayTag` | Investigate alert state tag |
| CombatTag | Output | `FGameplayTag` | Combat alert state tag |
| SearchTag | Output | `FGameplayTag` | Search alert state tag |

---

## State Tasks

### FAxMTask_Attack

Plays an attack montage and completes via `OnMontageEnded`. Falls back to timer when no montage is assigned.

| Field | Category | Type | Description |
|---|---|---|---|
| Controller | Context | `AAIController*` | Needed to get AnimInstance |
| AttackMontage | Input | `UAnimMontage*` | Montage to play (null = timer fallback) |
| AttackDuration | Parameter | `float` | Fallback duration (default 1.0s) |

### FAxMTask_Patrol

Navigates between waypoints on the assigned patrol route. Delegate-driven.

| Field | Category | Type | Description |
|---|---|---|---|
| Controller | Context | `AAIController*` | AI Controller (reads PatrolRoute) |
| WaitDuration | Parameter | `float` | Wait at each waypoint (default 2.0s) |
| AcceptanceRadius | Parameter | `float` | Arrival threshold (default 50.0 cm) |
| bLoop | Parameter | `bool` | True = loop, false = ping-pong |

### FAxMTask_MoveTo

Navigates to a target actor or location via PathFollowingComponent delegate.

| Field | Category | Type | Description |
|---|---|---|---|
| Controller | Context | `AAIController*` | AI Controller for navigation |
| TargetActor | Input | `AActor*` | Actor to navigate toward (overrides location) |
| TargetLocation | Input | `FVector` | Fallback location if actor is null/unreachable |
| AcceptanceRadius | Parameter | `float` | Arrival threshold (default 50.0 cm) |

### FAxMTask_FaceTarget

Rotates the pawn to face a target actor using the AIController focus system.

| Field | Category | Type | Description |
|---|---|---|---|
| Controller | Context | `AAIController*` | AI Controller for focus |
| TargetActor | Input | `AActor*` | Actor to face |

### FAxMTask_LookAround

Rotates through focal points over a duration, then succeeds.

| Field | Category | Type | Description |
|---|---|---|---|
| Controller | Context | `AAIController*` | AI Controller for rotation |
| LookDuration | Parameter | `float` | Total scan duration (default 3.0s) |

### FAxMTask_SearchArea

Navigates to successive search points around a center. Supports EQS or random NavMesh.

| Field | Category | Type | Description |
|---|---|---|---|
| Controller | Context | `AAIController*` | AI Controller for navigation |
| SearchCenter | Input | `FVector` | Center of the search area |
| SearchRadius | Parameter | `float` | Radius for point generation (default 500 cm) |
| SearchDuration | Parameter | `float` | Duration before succeeding (default 10.0s) |
| QueryAsset | Parameter | `UEnvQuery*` | Optional EQS query (null = random NavMesh) |
| AcceptanceRadius | Parameter | `float` | Arrival threshold (default 50.0 cm) |

### FAxMTask_AlertState

Sets the alert state tag on the AI Controller when a state is entered.

| Field | Category | Type | Description |
|---|---|---|---|
| Controller | Context | `AAIController*` | AI Controller |
| AlertStateTag | Input | `FGameplayTag` | Tag to set on state entry |

---

## Conditions

### FAxMCondition_HasTarget

Returns true when `TargetActor` is valid (non-null).

| Field | Category | Type |
|---|---|---|
| TargetActor | Input | `AActor*` |
| bInvert | Parameter | `bool` |

### FAxMCondition_IsSuspicious

Passes through `bIsSuspicious` from Suspicion.

| Field | Category | Type |
|---|---|---|
| bIsSuspicious | Input | `bool` |
| bInvert | Parameter | `bool` |

### FAxMCondition_IsOutsideLeash

Returns true when the NPC has exceeded its leash radius from home.

| Field | Category | Type |
|---|---|---|
| DistanceFromHome | Input | `float` |
| LeashRadius | Input | `float` |
| bInvert | Parameter | `bool` |

When `LeashRadius == 0`, the condition always returns false (leash disabled).

---

## EQS

### UAxMEnvQueryContext_SearchCenter

EQS context providing the AI Controller's cached last known location as a query center point. Used with `FAxMTask_SearchArea` when a `QueryAsset` is assigned.
