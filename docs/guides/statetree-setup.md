# StateTree Setup

Step-by-step guide for building the master StateTree from scratch. If you're using the included `ST_AxM_Master`, this shows you how it's wired and how to customize it.

## Schema

Set the StateTree's **Schema** to `StateTreeAIComponentSchema`. This provides `AIController` and `Pawn` as context objects for binding.

---

## Global Tasks

Add all four to the root state's **Global Tasks** list:

### 1. AxM Perception (Global)

| Field | Bind To |
|---|---|
| Controller (Context) | `AIController` |
| TargetActor (Output) | — available for binding |
| LastKnownLocation (Output) | — available for binding |
| HomeLocation (Output) | — available for binding |
| DistanceFromHome (Output) | — available for binding |
| StimulusLocation (Output) | — available for binding |
| HearingStrength (Output) | — available for binding |

### 2. AxM Suspicion (Global)

| Field | Bind To |
|---|---|
| HearingStrength (Input) | `Perception.HearingStrength` |
| SuspicionDecayRate (Parameter) | 0.15 (or bind from Config) |
| InvestigateThreshold (Parameter) | 0.4 (or bind from Config) |
| SuspicionGainMultiplier (Parameter) | 1.0 (or bind from Config) |
| SuspicionLevel (Output) | — available for binding |
| bIsSuspicious (Output) | — available for binding |

### 3. AxM Config (Global)

| Field | Bind To |
|---|---|
| Controller (Context) | `AIController` |
| LeashRadius (Output) | — available for binding |
| PatrolWaitDuration (Output) | — available for binding |
| All movement speeds (Output) | — available for binding |
| All suspicion params (Output) | — available for binding |
| All alert state tags (Output) | — available for binding |

### 4. AxM Target Tracking (Global)

| Field | Bind To |
|---|---|
| Controller (Context) | `AIController` |
| TargetActor (Input) | `Perception.TargetActor` |
| DistanceToTarget (Output) | — available for binding |
| HasLineOfSight (Output) | — available for binding |

---

## State Hierarchy

```
Root: Alive
├── [Patrol]            ← default entry state
├── [Investigate]       ← parent state
│   ├── [GoToStimulus]  ← default child
│   └── [ScanArea]
├── [Search]            ← parent state
│   ├── [GoToLastKnown] ← default child
│   └── [SearchArea]
└── [Combat]            ← linked sub-StateTree
```

---

## Patrol State

**Tasks:**

| Task | Field | Bind To |
|---|---|---|
| AxM Patrol | Controller | `AIController` |
| | WaitDuration | `Config.PatrolWaitDuration` |
| | AcceptanceRadius | 50.0 |
| | bLoop | true |

!!! note
    If the AI Controller has no PatrolRoute assigned, the Patrol task succeeds immediately and falls through to transitions. Add a `StateTree Delay` task after AxM Patrol as a fallback idle.

**Transitions:**

| Priority | Trigger | Condition | Target |
|---|---|---|---|
| 1 | On Tick | **Has Target** (TargetActor ← `Perception.TargetActor`) | → Combat |
| 2 | On Tick | **Is Suspicious** (bIsSuspicious ← `Suspicion.bIsSuspicious`) | → Investigate |

---

## Investigate State (Parent)

**Transitions (apply to all children):**

| Priority | Trigger | Condition | Target |
|---|---|---|---|
| 1 | On Tick | **Has Target** | → Combat |
| 2 | On Tick | **Is Suspicious** (bInvert = true) | → Patrol |

### GoToStimulus (Default Child)

| Task | Field | Bind To |
|---|---|---|
| AxM Move To | Controller | `AIController` |
| | TargetActor | `Perception.TargetActor` |
| | TargetLocation | `Perception.StimulusLocation` |
| | AcceptanceRadius | 50.0 |

Transition: **On State Completed** → ScanArea

### ScanArea

| Task | Field | Bind To |
|---|---|---|
| AxM Look Around | Controller | `AIController` |
| | LookDuration | 3.0 |

Transition: **On State Completed** → ScanArea (self-transition, loops until parent exits)

---

## Combat State

Set the state type to **Linked Asset** and reference a [combat sub-StateTree](combat-subtree.md). The combat sub-StateTree owns both positioning (closing distance, finding cover) and abilities (attacks, cooldowns).

!!! tip "Prototyping without a sub-StateTree"
    For quick prototyping, you can use the AxM Attack task directly instead of linking a sub-StateTree. Add AxM Move To + AxM Face Target + AxM Attack as tasks, and set a self-transition on State Completed. Replace with a linked sub-StateTree when you're ready to build proper combat behavior.

**Transitions:**

| Priority | Trigger | Condition | Target |
|---|---|---|---|
| 1 | On Tick | **Has Target** (bInvert = true) | → Search |
| 2 | On Tick | **Is Outside Leash** (DistanceFromHome ← `Perception.DistanceFromHome`, LeashRadius ← `Config.LeashRadius`) | → Patrol |

---

## Search State (Parent)

**Transitions:**

| Priority | Trigger | Condition | Target |
|---|---|---|---|
| 1 | On Tick | **Has Target** | → Combat |
| 2 | On Tick | **Is Outside Leash** | → Patrol |

### GoToLastKnown (Default Child)

| Task | Field | Bind To |
|---|---|---|
| AxM Move To | Controller | `AIController` |
| | TargetActor | `Perception.TargetActor` |
| | TargetLocation | `Perception.LastKnownLocation` |
| | AcceptanceRadius | 50.0 |

Transition: **On State Completed** → SearchArea

### SearchArea

| Task | Field | Bind To |
|---|---|---|
| AxM Search Area | Controller | `AIController` |
| | SearchCenter | `Perception.LastKnownLocation` |
| | SearchRadius | 500.0 (or bind from Config) |
| | SearchDuration | 10.0 (or bind from Config) |
| | AcceptanceRadius | 50.0 |

Transition: **On State Completed** → Patrol

---

## Property Binding Summary

```
Perception.TargetActor ──────┬──→ TargetTracking.TargetActor
                             ├──→ HasTarget conditions
                             └──→ MoveTo.TargetActor (combat sub-tree, GoToLastKnown)

Perception.HearingStrength ──→ Suspicion.HearingStrength

Perception.StimulusLocation ──→ MoveTo.TargetLocation (GoToStimulus)

Perception.LastKnownLocation ─┬─→ MoveTo.TargetLocation (GoToLastKnown)
                              └──→ SearchArea.SearchCenter

Perception.DistanceFromHome ──→ IsOutsideLeash.DistanceFromHome

Config.LeashRadius ──→ IsOutsideLeash.LeashRadius
Config.PatrolWaitDuration ──→ Patrol.WaitDuration

Suspicion.bIsSuspicious ──→ IsSuspicious conditions

TargetTracking.DistanceToTarget ──→ Combat sub-StateTree (positioning decisions)
TargetTracking.HasLineOfSight ──→ Combat sub-StateTree (LOS checks)
```
