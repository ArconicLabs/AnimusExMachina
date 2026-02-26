// Copyright ArconicLabs. All Rights Reserved.

// Global StateTree task that reads a UAxMConfig Data Asset from the AI
// Controller and exposes all archetype tuning values as bindable outputs
// for the rest of the tree.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StateTreeTaskBase.h"
#include "AxMGlobalTask_Config.generated.h"

class AAIController;

/**
 *  Instance data for the config global task.
 */
USTRUCT()
struct FAxMGlobalTask_ConfigInstanceData
{
	GENERATED_BODY()

	/** AI Controller (cast to AAxMAIController internally to read Config) */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AAIController> Controller;

	// --- Combat ---

	/** [Output] Distance at which the NPC enters combat engagement */
	UPROPERTY(EditAnywhere, Category = "Output")
	float EngagementRange = 200.0f;

	/** [Output] Max distance from home before leash triggers (0 = disabled) */
	UPROPERTY(EditAnywhere, Category = "Output")
	float LeashRadius = 0.0f;

	// --- Movement ---

	// --- Patrol ---

	/** [Output] How long to wait at each patrol waypoint */
	UPROPERTY(EditAnywhere, Category = "Output")
	float PatrolWaitDuration = 2.0f;

	// --- Movement ---

	/** [Output] Movement speed during Patrol state */
	UPROPERTY(EditAnywhere, Category = "Output")
	float PatrolMovementSpeed = 200.0f;

	/** [Output] Movement speed during Investigate state */
	UPROPERTY(EditAnywhere, Category = "Output")
	float InvestigateMovementSpeed = 300.0f;

	/** [Output] Movement speed during Combat state */
	UPROPERTY(EditAnywhere, Category = "Output")
	float CombatMovementSpeed = 450.0f;

	/** [Output] Movement speed during Search state */
	UPROPERTY(EditAnywhere, Category = "Output")
	float SearchMovementSpeed = 350.0f;

	// --- Search ---

	/** [Output] Radius for search area navigation */
	UPROPERTY(EditAnywhere, Category = "Output")
	float SearchRadius = 500.0f;

	/** [Output] Duration of the search behavior */
	UPROPERTY(EditAnywhere, Category = "Output")
	float SearchDuration = 10.0f;

	// --- Suspicion ---

	/** [Output] Per-second decay rate when no stimuli arrive */
	UPROPERTY(EditAnywhere, Category = "Output")
	float SuspicionDecayRate = 0.15f;

	/** [Output] Suspicion level that triggers the Investigate state */
	UPROPERTY(EditAnywhere, Category = "Output")
	float InvestigateThreshold = 0.4f;

	/** [Output] Multiplier applied to hearing strength when accumulating suspicion */
	UPROPERTY(EditAnywhere, Category = "Output")
	float SuspicionGainMultiplier = 1.0f;

	// --- Alert State Tags ---

	/** [Output] Tag assigned to the Patrol state */
	UPROPERTY(EditAnywhere, Category = "Output")
	FGameplayTag PatrolTag;

	/** [Output] Tag assigned to the Investigate state */
	UPROPERTY(EditAnywhere, Category = "Output")
	FGameplayTag InvestigateTag;

	/** [Output] Tag assigned to the Combat state */
	UPROPERTY(EditAnywhere, Category = "Output")
	FGameplayTag CombatTag;

	/** [Output] Tag assigned to the Search state */
	UPROPERTY(EditAnywhere, Category = "Output")
	FGameplayTag SearchTag;
};

/**
 *  Global StateTree task that reads a UAxMConfig Data Asset from the AI
 *  Controller and exposes values as bindable outputs. Add to the root
 *  state's Global Tasks list.
 */
USTRUCT(meta = (DisplayName = "AxM Config (Global)", Category = "Animus Ex Machina|Global"))
struct ANIMUSEXMACHINA_API FAxMGlobalTask_Config : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FAxMGlobalTask_ConfigInstanceData;

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView,
		const IStateTreeBindingLookup& BindingLookup,
		EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};
