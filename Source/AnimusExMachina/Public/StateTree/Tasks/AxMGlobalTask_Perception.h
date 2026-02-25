// Copyright ArconicLabs. All Rights Reserved.

// Global StateTree task that reads cached perception data from the AI
// Controller and exposes TargetActor, LastKnownLocation, and HomeLocation
// as bindable output properties for the rest of the tree.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "AxMGlobalTask_Perception.generated.h"

class AAxMAIController;

/**
 *  Instance data for the perception global task.
 */
USTRUCT()
struct FAxMGlobalTask_PerceptionInstanceData
{
	GENERATED_BODY()

	/** The AxM AI Controller that owns the perception component */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AAxMAIController> Controller;

	/** [Output] The currently perceived target actor (null if none) */
	UPROPERTY(EditAnywhere, Category = "Output")
	TObjectPtr<AActor> TargetActor;

	/** [Output] Last known location of the target */
	UPROPERTY(EditAnywhere, Category = "Output")
	FVector LastKnownLocation = FVector::ZeroVector;

	/** [Output] The NPC's home/spawn location, recorded when the tree starts */
	UPROPERTY(EditAnywhere, Category = "Output")
	FVector HomeLocation = FVector::ZeroVector;
};

/**
 *  Global StateTree task that monitors AI Perception via cached delegate
 *  results on the AI Controller. Writes TargetActor and LastKnownLocation
 *  for property binding throughout the tree.
 */
USTRUCT(meta = (DisplayName = "AxM Perception (Global)", Category = "AxM|Global"))
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
