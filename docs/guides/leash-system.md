# Leash System

The leash system prevents NPCs from chasing targets indefinitely. When an NPC strays too far from its home location, the leash condition triggers and returns it to patrol.

## Building Blocks

The leash uses three components that you wire together in the StateTree:

| Component | Type | Source |
|---|---|---|
| `DistanceFromHome` | Float output | Perception Global Task |
| `LeashRadius` | Float output | Config Global Task (from Data Asset) |
| `Is Outside Leash` | Condition | Compares the two values |

## Configuration

Open your `UAxMConfig` Data Asset and set **Leash Radius**:

- **Leash Radius > 0** — NPC returns to patrol when it exceeds this distance from home
- **Leash Radius = 0** — Leash disabled, NPC chases indefinitely

The value is in centimeters (100 cm = 1 meter). A value of 2000 means the NPC won't chase more than 20 meters from its spawn point.

## Wiring the Condition

Add the **AxM Is Outside Leash** condition to transitions where you want the NPC to disengage.

### On Combat (Parent) → Patrol

| Field | Bind To |
|---|---|
| DistanceFromHome | `Perception.DistanceFromHome` |
| LeashRadius | `Config.LeashRadius` |

Place this transition **below** the `!HasTarget → Search` transition so losing sight takes priority over leash.

### On Search (Parent) → Patrol

Same bindings as above. Place **below** the `HasTarget → Combat` transition.

## How It Works

- `DistanceFromHome` is computed every tick by the Perception Global Task as the distance between the pawn's current position and its recorded home location (set at StateTree start)
- `LeashRadius` is read from the Config Data Asset once at StateTree start
- The `Is Outside Leash` condition returns `true` when `LeashRadius > 0 && DistanceFromHome > LeashRadius`
- When `LeashRadius == 0`, the condition always returns `false` (leash disabled)

## Invert Support

The condition supports `bInvert`. When inverted, it returns `true` when the NPC is *inside* the leash radius. This can be useful for "engage only near home" behavior.
