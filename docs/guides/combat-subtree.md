# Combat Sub-StateTree

The master StateTree's **Combat: Engage** state references a linked sub-StateTree that defines the NPC's attack behavior. The master tree handles *when* to fight (perception, range checks, transitions) while the combat sub-StateTree handles *how* to fight (ability selection, montages, cooldowns).

## How It Works

```
ST_AxM_Master
  └─ Combat: Engage
       └─ Linked → ST_Combat_Melee  (user-built, replaceable)
                    ├─ LightAttack  → [AxM Attack, montage = AM_LightAttack]
                    ├─ HeavyAttack  → [AxM Attack, montage = AM_HeavyAttack]
                    └─ Cooldown     → [Delay, 1.0s]
```

- The master tree enters Engage when `IsInEngagementRange == true`
- Execution transfers into the linked sub-StateTree
- The sub-StateTree runs autonomously — selecting abilities, playing montages
- When the master tree exits Engage (target lost, out of range, leash), the sub-StateTree is interrupted
- The sub-StateTree does **not** need to check HasTarget or IsInRange

## Context Inheritance

Linked sub-StateTrees automatically inherit the parent's context:

- `AIController` and `Pawn` are available for binding
- Global Task outputs (`Perception.*`, `Config.*`, `TargetTracking.*`) are accessible
- No explicit parameter-passing needed

The sub-StateTree's Schema must use `StateTreeAIComponentSchema` (same as the master).

---

## Building a Sample Combat Tree

### Step 1: Create the Asset

1. Right-click in Content Browser → **StateTree → StateTree**
2. Name it `ST_Combat_Melee`
3. Set Schema to `StateTreeAIComponentSchema`

### Step 2: State Hierarchy

```
Root
└── ChooseAttack         ← default entry (selector)
    ├── LightAttack
    ├── HeavyAttack
    └── Cooldown
```

Set `ChooseAttack`'s **Selection Behavior** to **Random**.

!!! tip "Selection Behavior"
    **Random** gives variety. **Sequential** gives predictable combos. **Utility** lets you weight attacks based on distance, health, etc.

### Step 3: LightAttack

| Task | Field | Value |
|---|---|---|
| AxM Attack | Controller | `AIController` |
| | AttackMontage | `AM_LightAttack` |
| | AttackDuration | 0.8 (fallback) |

Transition: **On State Completed** → ChooseAttack

### Step 4: HeavyAttack

| Task | Field | Value |
|---|---|---|
| AxM Attack | Controller | `AIController` |
| | AttackMontage | `AM_HeavyAttack` |
| | AttackDuration | 1.5 (fallback) |

Transition: **On State Completed** → ChooseAttack

### Step 5: Cooldown

| Task | Field | Value |
|---|---|---|
| StateTree Delay | Duration | 1.0 |

Transition: **On State Completed** → ChooseAttack

### Step 6: Link to Master Tree

1. Open `ST_AxM_Master`
2. Select **Combat: Engage**
3. Set state type to **Linked**
4. Set **Linked StateTree** to `ST_Combat_Melee`

---

## Variations

### Sequential Combo

```
ChooseAttack (Sequential)
├── LightAttack    ← first hit
├── LightAttack    ← second hit
├── HeavyAttack    ← finisher
└── Cooldown       ← recovery
```

A predictable 3-hit combo with a cooldown window the player can exploit.

### Ranged Combat

```
ChooseAttack (Random)
├── FireProjectile  → [custom task: spawn projectile]
├── Reload          → [AxM Attack with reload montage]
└── Reposition      → [AxM Move To with EQS cover point]
```

Ranged enemies can include movement between shots.

### Utility-Based Selection

```
ChooseAttack (Utility)
├── HeavyAttack    ← condition: DistanceToTarget < 100
├── LightAttack    ← condition: DistanceToTarget < 200
└── Cooldown       ← always available (lowest weight)
```

The NPC prefers heavy attacks at close range.

---

## Key Points

- **Montage per state** — Each ability sets its own `AttackMontage` on the AxM Attack task. The montage never passes to the master tree.
- **Timer fallback** — Leave `AttackMontage` empty to use the `AttackDuration` timer. Useful for prototyping before montage assets are ready.
- **Master owns transitions** — The sub-StateTree loops indefinitely. The master tree interrupts it when conditions change.
- **One sub-tree per archetype** — Different NPC types reference different combat sub-StateTrees. A melee grunt uses `ST_Combat_Melee`, a ranged archer uses `ST_Combat_Ranged`.
- **Anim Notifies** — For gameplay events during montages (damage application, VFX, sound), use Anim Notifies on the montage asset. The AxM Attack task handles playback; your game code handles effects via standard UE Anim Notify callbacks.
