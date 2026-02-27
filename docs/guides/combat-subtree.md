# Combat Sub-StateTree

The master StateTree's **Combat** state references a linked sub-StateTree that defines the NPC's combat behavior. The master tree handles *when* to fight (perception, target acquisition, leash) while the combat sub-StateTree handles *how* to fight — both **positioning** (closing distance, finding cover, repositioning) and **abilities** (attacks, cooldowns, reloads).

## How It Works

```
ST_AxM_Master
  └─ Combat (Linked → ST_Combat_Melee or ST_Combat_Ranged)
       ├─ Positioning  → [MoveTo, FaceTarget]
       ├─ Attack       → [AxM Attack with montage]
       └─ Cooldown     → [Delay]
```

- The master tree enters Combat when `HasTarget == true`
- Execution transfers into the linked combat sub-StateTree
- The sub-StateTree runs autonomously — handling movement, attacks, cooldowns
- When the master tree exits Combat (target lost, leash exceeded), the sub-StateTree is interrupted
- The sub-StateTree does **not** need to check HasTarget — the master tree handles that

## Passing Data via Parameters

Linked sub-StateTrees inherit the parent's **schema context** — `AIController` and `Pawn` are available for binding automatically. However, Global Task outputs (`Perception.*`, `TargetTracking.*`) are **not** inherited. The sub-tree must define **Parameters** to receive this data from the master tree.

!!! warning "No Global Tasks in sub-trees"
    Do not add Global Tasks to a linked sub-StateTree that uses parameters. This is a known crash vector in UE 5.5. Keep all Global Tasks in the master tree's root state.

The sub-StateTree's Schema must use `StateTreeAIComponentSchema` (same as the master).

### Parameters for Combat Sub-Trees

Define these parameters on the combat sub-StateTree. The master tree's Combat Linked Asset state binds them from its own Global Task outputs.

| Parameter | Type | Bound From (Master Tree) | Use |
|---|---|---|---|
| `TargetActor` | `AActor*` | `Perception.TargetActor` | Navigation target for MoveTo, FaceTarget |
| `LastKnownLocation` | `FVector` | `Perception.LastKnownLocation` | Fallback navigation when target is occluded |
| `DistanceToTarget` | `float` | `TargetTracking.DistanceToTarget` | Range-based transitions |
| `HasLineOfSight` | `bool` | `TargetTracking.HasLineOfSight` | LOS checks for ranged attacks |

---

## Sample: Melee Combat (`ST_Combat_Melee`)

A melee NPC that closes distance, attacks, then pauses before re-engaging.

### State Hierarchy

```
Root
├── CloseDistance (default)    ← move toward target
├── ExecuteAttack              ← play attack montage
└── Cooldown                   ← brief pause
```

### Step 1: Create the Asset

1. Right-click in Content Browser → **StateTree → StateTree**
2. Name it `ST_Combat_Melee`
3. Set Schema to `StateTreeAIComponentSchema`
4. Open the **Parameters** panel and add:
    - `TargetActor` — Object Reference (`AActor`)
    - `LastKnownLocation` — Vector
    - `DistanceToTarget` — Float
    - `HasLineOfSight` — Bool
5. Do **not** add any Global Tasks to this sub-tree

### Step 2: CloseDistance (Default State)

Move toward the target until within striking range.

| Task | Field | Bind To |
|---|---|---|
| AxM Move To | Controller | `AIController` |
| | TargetActor | `Parameters.TargetActor` |
| | TargetLocation | `Parameters.LastKnownLocation` |
| | AcceptanceRadius | 50.0 |
| AxM Face Target | Controller | `AIController` |
| | TargetActor | `Parameters.TargetActor` |

**Transitions:**

| Priority | Trigger | Condition | Target |
|---|---|---|---|
| 1 | On Tick | `Parameters.DistanceToTarget` **<** `200.0` | → ExecuteAttack |
| 2 | On State Completed | — | → CloseDistance (self-transition) |

!!! tip "Distance threshold"
    The `200.0` value is your melee strike range. Use StateTree's built-in property comparison — bind `DistanceToTarget` from Parameters and compare it against a literal float. No custom condition node needed.

### Step 3: ExecuteAttack

Play the attack montage.

| Task | Field | Bind To |
|---|---|---|
| AxM Face Target | Controller | `AIController` |
| | TargetActor | `Parameters.TargetActor` |
| AxM Attack | Controller | `AIController` |
| | AttackMontage | `AM_MeleeAttack` (your montage asset) |
| | AttackDuration | 1.0 (timer fallback) |

**Transitions:**

| Priority | Trigger | Condition | Target |
|---|---|---|---|
| 1 | On State Completed | — | → Cooldown |

### Step 4: Cooldown

Brief pause between attacks. This creates a window the player can exploit.

| Task | Field | Value |
|---|---|---|
| StateTree Delay | Duration | 0.5 |

**Transitions:**

| Priority | Trigger | Condition | Target |
|---|---|---|---|
| 1 | On State Completed | — | → CloseDistance |

### Step 5: Link to Master Tree

1. Open `ST_AxM_Master`
2. Select **Combat**
3. Set state type to **Linked Asset**
4. Set **Linked StateTree** to `ST_Combat_Melee`
5. Bind the Linked Asset's parameters:
    - `TargetActor` ← `Perception.TargetActor`
    - `LastKnownLocation` ← `Perception.LastKnownLocation`
    - `DistanceToTarget` ← `TargetTracking.DistanceToTarget`
    - `HasLineOfSight` ← `TargetTracking.HasLineOfSight`

---

## Sample: Ranged Combat (`ST_Combat_Ranged`)

A ranged NPC that finds a firing position, shoots, then repositions.

### State Hierarchy

```
Root
├── FindPosition (default)    ← move to firing position
├── Shoot                      ← play ranged attack
└── Reposition                 ← move to new position
```

### Step 1: Create the Asset

1. Right-click in Content Browser → **StateTree → StateTree**
2. Name it `ST_Combat_Ranged`
3. Set Schema to `StateTreeAIComponentSchema`
4. Open the **Parameters** panel and add:
    - `TargetActor` — Object Reference (`AActor`)
    - `LastKnownLocation` — Vector
    - `DistanceToTarget` — Float
    - `HasLineOfSight` — Bool
5. Do **not** add any Global Tasks to this sub-tree

### Step 2: FindPosition (Default State)

Move toward the target but maintain distance. The NPC moves to a position within firing range.

| Task | Field | Bind To |
|---|---|---|
| AxM Move To | Controller | `AIController` |
| | TargetActor | `Parameters.TargetActor` |
| | TargetLocation | `Parameters.LastKnownLocation` |
| | AcceptanceRadius | 200.0 |
| AxM Face Target | Controller | `AIController` |
| | TargetActor | `Parameters.TargetActor` |

**Transitions:**

| Priority | Trigger | Condition | Target |
|---|---|---|---|
| 1 | On Tick | `Parameters.DistanceToTarget` **<** `1000.0` **AND** `Parameters.HasLineOfSight` **==** `true` | → Shoot |
| 2 | On State Completed | — | → FindPosition (self-transition) |

!!! tip "Firing range"
    The `1000.0` value is your max engagement distance. Bind both `DistanceToTarget` and `HasLineOfSight` from Parameters. The NPC only fires when it has a clear shot within range.

### Step 3: Shoot

Fire at the target.

| Task | Field | Bind To |
|---|---|---|
| AxM Face Target | Controller | `AIController` |
| | TargetActor | `Parameters.TargetActor` |
| AxM Attack | Controller | `AIController` |
| | AttackMontage | `AM_RangedAttack` (your ranged montage) |
| | AttackDuration | 1.5 (timer fallback) |

**Transitions:**

| Priority | Trigger | Condition | Target |
|---|---|---|---|
| 1 | On State Completed | — | → Reposition |

!!! note "Projectile spawning"
    The AxM Attack task handles montage playback. To spawn a projectile, add an **Anim Notify** on the montage at the fire frame. Your game code handles the projectile spawn via standard UE Anim Notify callbacks — the plugin doesn't need to know about it.

### Step 4: Reposition

Move to a new position after firing. This prevents the NPC from being a stationary target.

| Task | Field | Bind To |
|---|---|---|
| AxM Move To | Controller | `AIController` |
| | TargetActor | `Parameters.TargetActor` |
| | TargetLocation | `Parameters.LastKnownLocation` |
| | AcceptanceRadius | 100.0 |

**Transitions:**

| Priority | Trigger | Condition | Target |
|---|---|---|---|
| 1 | On State Completed | — | → FindPosition |

!!! tip "Advanced repositioning"
    For smarter repositioning, replace the basic MoveTo with an EQS query that finds cover points or flanking positions. Create a custom state task that runs an EQS query and navigates to the best result.

### Step 5: Link to Master Tree

1. Open `ST_AxM_Master`
2. Select **Combat**
3. Set state type to **Linked Asset**
4. Set **Linked StateTree** to `ST_Combat_Ranged`
5. Bind the Linked Asset's parameters:
    - `TargetActor` ← `Perception.TargetActor`
    - `LastKnownLocation` ← `Perception.LastKnownLocation`
    - `DistanceToTarget` ← `TargetTracking.DistanceToTarget`
    - `HasLineOfSight` ← `TargetTracking.HasLineOfSight`

---

## Variations

### Sequential Melee Combo

```
Root (Sequential selection)
├── LightAttack    ← first hit
├── LightAttack    ← second hit
├── HeavyAttack    ← finisher
└── Cooldown       ← recovery
```

Set the root's **Selection Behavior** to **Sequential**. Each state transitions to the next on completion, creating a predictable combo pattern the player can learn.

### Utility-Based Selection

```
Root
├── HeavyAttack    ← condition: Parameters.DistanceToTarget < 100
├── LightAttack    ← condition: Parameters.DistanceToTarget < 200
├── CloseDistance   ← condition: Parameters.DistanceToTarget >= 200
└── Cooldown       ← always available (lowest priority)
```

Use conditions on each state to evaluate which attack to use based on current range. Closer = heavier attacks.

### Mixed Melee + Ranged

```
Root
├── CloseDistance       ← when far from target
├── MeleeAttack        ← condition: Parameters.DistanceToTarget < 200
├── ThrowProjectile    ← condition: Parameters.DistanceToTarget > 500 && Parameters.HasLineOfSight
└── Cooldown
```

An NPC with both melee and ranged abilities selects based on distance.

---

## Key Points

- **Positioning in the sub-tree** — The combat sub-StateTree owns both movement and abilities. The master tree only decides *whether* to fight, not *how*.
- **Montage per state** — Each ability state sets its own `AttackMontage` on the AxM Attack task. Different attacks use different montages.
- **Timer fallback** — Leave `AttackMontage` empty to use the `AttackDuration` timer. Useful for prototyping before montage assets are ready.
- **Master owns exit transitions** — The sub-StateTree loops indefinitely. The master tree interrupts it when conditions change (target lost, leash exceeded).
- **One sub-tree per archetype** — Different NPC types reference different combat sub-StateTrees. A melee grunt uses `ST_Combat_Melee`, a ranged archer uses `ST_Combat_Ranged`.
- **DistanceToTarget for range** — Bind `Parameters.DistanceToTarget` in the sub-tree for range-based transitions. Compare against literal floats using StateTree's built-in property comparisons.
- **No Global Tasks in sub-trees** — Define Parameters instead. The master tree binds its Global Task outputs to the sub-tree's parameters on the Linked Asset state. Adding Global Tasks to a linked sub-tree that also uses parameters can cause editor crashes in UE 5.5.
- **Anim Notifies for effects** — For gameplay events during montages (damage application, VFX, sound, projectile spawning), use Anim Notifies on the montage asset. The AxM Attack task handles playback; your game code handles effects via standard UE Anim Notify callbacks.
