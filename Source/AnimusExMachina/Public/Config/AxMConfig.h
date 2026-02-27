// Copyright ArconicLabs. All Rights Reserved.

// Primary Data Asset that consolidates all NPC archetype tuning parameters
// into a single asset. Assign to an AI Controller to configure perception,
// combat, movement, suspicion, and search behavior.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AxMConfig.generated.h"

/**
 *  Data Asset defining a complete NPC archetype configuration.
 *  Assign one to an AxM AI Controller (or Blueprint subclass) to drive
 *  all tuning parameters from a single asset.
 */
UCLASS(BlueprintType)
class ANIMUSEXMACHINA_API UAxMConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	// --- Perception: Sense Toggles ---

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animus Ex Machina|Perception")
	bool bEnableSight = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animus Ex Machina|Perception")
	bool bEnableHearing = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animus Ex Machina|Perception")
	bool bEnableDamage = true;

	// --- Perception: Sight ---

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animus Ex Machina|Perception|Sight",
		meta = (EditCondition = "bEnableSight", Units = "cm"))
	float SightRadius = 1500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animus Ex Machina|Perception|Sight",
		meta = (EditCondition = "bEnableSight", Units = "cm"))
	float LoseSightRadius = 2000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animus Ex Machina|Perception|Sight",
		meta = (EditCondition = "bEnableSight", Units = "deg"))
	float PeripheralVisionAngle = 60.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animus Ex Machina|Perception|Sight",
		meta = (EditCondition = "bEnableSight", Units = "s"))
	float SightMaxAge = 5.0f;

	// --- Perception: Hearing ---

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animus Ex Machina|Perception|Hearing",
		meta = (EditCondition = "bEnableHearing", Units = "cm"))
	float HearingRange = 1500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animus Ex Machina|Perception|Hearing",
		meta = (EditCondition = "bEnableHearing", Units = "s"))
	float HearingMaxAge = 3.0f;

	// --- Suspicion ---

	/** Per-second decay rate when no stimuli arrive */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animus Ex Machina|Suspicion")
	float SuspicionDecayRate = 0.15f;

	/** Suspicion level that triggers the Investigate state */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animus Ex Machina|Suspicion")
	float InvestigateThreshold = 0.4f;

	/** Multiplier applied to hearing strength when accumulating suspicion */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animus Ex Machina|Suspicion")
	float SuspicionGainMultiplier = 1.0f;

	// --- Combat ---

	/** Max distance from home before the leash condition triggers (0 = disabled) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animus Ex Machina|Combat",
		meta = (ClampMin = "0", Units = "cm"))
	float LeashRadius = 0.0f;

	// --- Movement ---

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animus Ex Machina|Movement",
		meta = (ClampMin = "0", Units = "cm/s"))
	float PatrolMovementSpeed = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animus Ex Machina|Movement",
		meta = (ClampMin = "0", Units = "cm/s"))
	float InvestigateMovementSpeed = 300.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animus Ex Machina|Movement",
		meta = (ClampMin = "0", Units = "cm/s"))
	float CombatMovementSpeed = 450.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animus Ex Machina|Movement",
		meta = (ClampMin = "0", Units = "cm/s"))
	float SearchMovementSpeed = 350.0f;

	// --- Patrol ---

	/** How long to wait at each patrol waypoint before moving to the next */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animus Ex Machina|Patrol",
		meta = (ClampMin = "0", Units = "s"))
	float PatrolWaitDuration = 2.0f;

	// --- Search ---

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animus Ex Machina|Search",
		meta = (ClampMin = "0", Units = "cm"))
	float SearchRadius = 500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animus Ex Machina|Search",
		meta = (ClampMin = "0", Units = "s"))
	float SearchDuration = 10.0f;

	// --- Alert State Tags ---
	// Assign tags from your project's tag hierarchy. The plugin registers no tags.

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animus Ex Machina|Alert State")
	FGameplayTag PatrolTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animus Ex Machina|Alert State")
	FGameplayTag InvestigateTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animus Ex Machina|Alert State")
	FGameplayTag CombatTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animus Ex Machina|Alert State")
	FGameplayTag SearchTag;
};
