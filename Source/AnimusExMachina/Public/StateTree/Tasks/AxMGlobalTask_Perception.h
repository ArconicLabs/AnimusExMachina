// Copyright ArconicLabs. All Rights Reserved.

// Global StateTree task that reads cached perception data from the AI
// Controller and exposes sight, hearing, and damage outputs as bindable
// properties for the rest of the tree.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "AxMGlobalTask_Perception.generated.h"

class AAIController;

/**
 *  Instance data for the perception global task.
 */
USTRUCT()
struct FAxMGlobalTask_PerceptionInstanceData
{
	GENERATED_BODY()

	/** AI Controller (cast to AAxMAIController internally to read cached perception) */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AAIController> Controller;

	/** [Output] The currently perceived target actor (null if none) */
	UPROPERTY(EditAnywhere, Category = "Output")
	TObjectPtr<AActor> TargetActor;

	/** [Output] Last known location of the target */
	UPROPERTY(EditAnywhere, Category = "Output")
	FVector LastKnownLocation = FVector::ZeroVector;

	/** [Output] The NPC's home/spawn location, recorded when the tree starts */
	UPROPERTY(EditAnywhere, Category = "Output")
	FVector HomeLocation = FVector::ZeroVector;

	/** [Output] Location of the most recent unconfirmed stimulus (hearing or unknown-source damage) */
	UPROPERTY(EditAnywhere, Category = "Output")
	FVector StimulusLocation = FVector::ZeroVector;

	/** [Output] Strength of the most recent hearing event (0 if none this tick) */
	UPROPERTY(EditAnywhere, Category = "Output")
	float HearingStrength = 0.0f;
};

/**
 *  Global StateTree task that monitors AI Perception via cached delegate
 *  results on the AI Controller. Writes TargetActor and LastKnownLocation
 *  for property binding throughout the tree.
 */
USTRUCT(meta = (DisplayName = "AxM Perception (Global)", Category = "Animus Ex Machina|Global"))
struct ANIMUSEXMACHINA_API FAxMGlobalTask_Perception : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FAxMGlobalTask_PerceptionInstanceData;

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) const override;

	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context,
		const float DeltaTime) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView,
		const IStateTreeBindingLookup& BindingLookup,
		EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};
