// Copyright ArconicLabs. All Rights Reserved.

// StateTree task that rotates the AI pawn to face a target actor using
// the AIController's focus system.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "AxMTask_FaceTarget.generated.h"

class AAIController;

/**
 *  Instance data for the FaceTarget task.
 */
USTRUCT()
struct FAxMTask_FaceTargetInstanceData
{
	GENERATED_BODY()

	/** AI Controller */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AAIController> Controller;

	/** Actor to face toward */
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<AActor> TargetActor;
};

/**
 *  StateTree task that faces the AI pawn toward a target actor.
 *  Sets focus on enter, clears on exit.
 */
USTRUCT(meta = (DisplayName = "AxM Face Target", Category = "Animus Ex Machina|Tasks"))
struct ANIMUSEXMACHINA_API FAxMTask_FaceTarget : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FAxMTask_FaceTargetInstanceData;

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) const override;

	virtual void ExitState(FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView,
		const IStateTreeBindingLookup& BindingLookup,
		EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};
