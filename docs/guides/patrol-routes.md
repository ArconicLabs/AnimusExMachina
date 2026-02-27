# Patrol Routes

Patrol routes define the path an NPC follows during its Patrol state. Routes are level-placed actors with a spline component — designers add and arrange waypoints visually in the editor.

## Creating a Patrol Route

1. In the level, right-click → **Place Actor** → search for `AxMPatrolRoute`
2. Place it near your NPC's starting position
3. Select the actor → select its **Spline Component**
4. Add points by Alt+dragging existing points, or right-click the spline → **Add Spline Point**
5. Arrange at least 2 points to define a path

## Assigning to an NPC

Select your NPC's AI Controller instance in the level → in Details, set **Patrol Route** to your placed route.

!!! note
    `PatrolRoute` is `EditInstanceOnly` — it's set per-NPC instance in the level, not on the Blueprint class. Multiple NPCs can share the same controller Blueprint but follow different routes.

## Wiring the Patrol Task

In the StateTree's Patrol state, add the **AxM Patrol** task:

| Field | Value |
|---|---|
| Controller | `AIController` (context) |
| WaitDuration | `Config.PatrolWaitDuration` (or a fixed value) |
| AcceptanceRadius | 50.0 |
| bLoop | true |

## Loop vs Ping-Pong

- **bLoop = true** — NPC loops: point 0 → 1 → 2 → 0 → 1 → ...
- **bLoop = false** — NPC ping-pongs: 0 → 1 → 2 → 1 → 0 → ...

## No Route Assigned

If the AI Controller's `PatrolRoute` is null, the Patrol task succeeds immediately. The StateTree falls through to transitions, which preserves idle behavior. Add a `StateTree Delay` task after the Patrol task as a fallback idle if desired.

## Wait Duration

`PatrolWaitDuration` is configurable in the `UAxMConfig` Data Asset (default 2.0 seconds). The NPC pauses at each waypoint for this duration before moving to the next.

## API

`AAxMPatrolRoute` exposes these BlueprintCallable functions:

| Function | Returns | Description |
|---|---|---|
| `GetPatrolPoints()` | `TArray<FVector>` | World-space positions of all spline control points |
| `GetNumPatrolPoints()` | `int32` | Number of patrol points |
| `GetPatrolPoint(Index)` | `FVector` | A specific point by index (world space) |
