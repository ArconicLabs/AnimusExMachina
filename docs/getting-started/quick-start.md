# Quick Start

Get a working NPC in 5 steps.

## 1. Create the AI Controller Blueprint

1. Right-click in Content Browser → **Blueprint Class**
2. Search for parent: `AxMAIController`
3. Name it `BP_MyAIController`
4. Open it → find the **StateTreeAI** component → assign a StateTree asset

!!! tip
    The plugin includes a sample StateTree (`ST_AxM_Master`) you can use as a starting point. For a custom tree, see the [StateTree Setup](../guides/statetree-setup.md) guide.

## 2. Create the NPC Character Blueprint

1. Right-click → **Blueprint Class** → parent: **Character**
2. Name it `BP_MyNPC`
3. Configure:
    - **Mesh**: Assign a Skeletal Mesh
    - **Animation**: Set an Animation Blueprint
    - **Character Movement**: Enable **Use Acceleration for Paths**
    - **AI Controller Class**: Set to `BP_MyAIController`
    - **Auto Possess AI**: Set to `Placed in World`

## 3. (Optional) Create a Config Data Asset

1. Right-click → **Miscellaneous → Data Asset**
2. Select `AxMConfig` as the class
3. Name it `DA_MyArchetype`
4. Tune perception ranges, movement speeds, engagement range, leash radius, etc.
5. Assign it to your AI Controller Blueprint's **Config** property

!!! note
    Without a Config asset, the AI Controller uses its built-in default values. The Config asset lets you create distinct archetypes (cautious sniper, aggressive rusher) without touching code.

## 4. (Optional) Set Up a Patrol Route

1. Place an `AxMPatrolRoute` actor in your level
2. Edit the spline component to add waypoints
3. Select your NPC instance → set **Patrol Route** to the placed route

## 5. Place and Play

1. Drag your NPC Blueprint into the level
2. Position it on the NavMesh
3. Press **Play**

**Expected behavior:**

- NPC patrols between waypoints (if a route is assigned) or idles
- Walk into sight range → NPC chases you
- Stand still → NPC attacks when in engagement range
- Run away → NPC pursues, then searches your last known location
- Leave sight range during combat → NPC searches, then returns to patrol
