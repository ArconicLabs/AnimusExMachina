# Configuration

All NPC tuning is centralized in the `UAxMConfig` Primary Data Asset. Create one per enemy archetype and assign it to the AI Controller's `Config` property.

## Creating a Config Asset

1. Right-click in Content Browser → **Miscellaneous → Data Asset**
2. Select `AxMConfig` as the class
3. Name it descriptively (e.g. `DA_Grunt_Melee`, `DA_Archer_Ranged`)

## Parameters

### Perception: Sense Toggles

| Parameter | Type | Default | Description |
|---|---|---|---|
| `bEnableSight` | bool | true | Enable sight sense |
| `bEnableHearing` | bool | true | Enable hearing sense |
| `bEnableDamage` | bool | true | Enable damage sense |

### Perception: Sight

| Parameter | Type | Default | Units | Description |
|---|---|---|---|---|
| `SightRadius` | float | 1500.0 | cm | Detection range |
| `LoseSightRadius` | float | 2000.0 | cm | Range at which sight is lost (hysteresis) |
| `PeripheralVisionAngle` | float | 60.0 | deg | Half-angle of sight cone |
| `SightMaxAge` | float | 5.0 | s | Max age before stimulus expires |

### Perception: Hearing

| Parameter | Type | Default | Units | Description |
|---|---|---|---|---|
| `HearingRange` | float | 1500.0 | cm | Detection range |
| `HearingMaxAge` | float | 3.0 | s | Max age before stimulus expires |

### Suspicion

| Parameter | Type | Default | Description |
|---|---|---|---|
| `SuspicionDecayRate` | float | 0.15 | Per-second decay when no stimuli arrive |
| `InvestigateThreshold` | float | 0.4 | Level that triggers Investigate state |
| `SuspicionGainMultiplier` | float | 1.0 | Multiplier on hearing strength accumulation |

### Combat

| Parameter | Type | Default | Units | Description |
|---|---|---|---|---|
| `EngagementRange` | float | 200.0 | cm | Distance for entering combat engagement |
| `LeashRadius` | float | 0.0 | cm | Max chase distance from home (0 = disabled) |

### Patrol

| Parameter | Type | Default | Units | Description |
|---|---|---|---|---|
| `PatrolWaitDuration` | float | 2.0 | s | Wait time at each patrol waypoint |

### Movement

| Parameter | Type | Default | Units | Description |
|---|---|---|---|---|
| `PatrolMovementSpeed` | float | 200.0 | cm/s | Speed during Patrol state |
| `InvestigateMovementSpeed` | float | 300.0 | cm/s | Speed during Investigate state |
| `CombatMovementSpeed` | float | 450.0 | cm/s | Speed during Combat state |
| `SearchMovementSpeed` | float | 350.0 | cm/s | Speed during Search state |

### Search

| Parameter | Type | Default | Units | Description |
|---|---|---|---|---|
| `SearchRadius` | float | 500.0 | cm | Search area around last known location |
| `SearchDuration` | float | 10.0 | s | Time before giving up search |

### Alert State Tags

These are `FGameplayTag` fields — assign tags from your project's hierarchy. The plugin registers no tags itself.

| Parameter | Description |
|---|---|
| `PatrolTag` | Tag for the Patrol alert state |
| `InvestigateTag` | Tag for the Investigate alert state |
| `CombatTag` | Tag for the Combat alert state |
| `SearchTag` | Tag for the Search alert state |

## Example Archetypes

### Aggressive Melee Grunt

| Parameter | Value |
|---|---|
| SightRadius | 2000 |
| EngagementRange | 150 |
| CombatMovementSpeed | 600 |
| LeashRadius | 0 (no leash — chases forever) |
| PatrolWaitDuration | 1.0 |

### Cautious Ranged Archer

| Parameter | Value |
|---|---|
| SightRadius | 3000 |
| EngagementRange | 800 |
| CombatMovementSpeed | 300 |
| LeashRadius | 2500 |
| SearchDuration | 15.0 |

### Alert Guard

| Parameter | Value |
|---|---|
| SightRadius | 1500 |
| HearingRange | 2500 |
| SuspicionGainMultiplier | 2.0 |
| InvestigateThreshold | 0.2 |
| LeashRadius | 1500 |
| PatrolWaitDuration | 3.0 |

## Fallback Behavior

When no Config asset is assigned, the AI Controller uses its built-in default property values. The Config Global Task outputs remain at their struct defaults. This means the StateTree still functions — it just uses hardcoded values instead of data-driven ones.
