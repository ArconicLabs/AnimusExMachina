// Copyright ArconicLabs. All Rights Reserved.

// StateTree task that navigates the AI pawn to a target actor or location
// using AAIController::MoveTo, completing via the move completed delegate.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "AxMTask_MoveTo.generated.h"

class AAIController;

/**
 *  Instance data for the MoveTo task.
 */
USTRUCT()
struct FAxMTask_MoveToInstanceData
{
	GENERATED_BODY()

	/** AI Controller for navigation */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AAIController> Controller;

	/** Target location to navigate to (used if TargetActor is null) */
	UPROPERTY(EditAnywhere, Category = "Input")
	FVector TargetLocation = FVector::ZeroVector;

	/** Optional target actor to navigate toward (overrides TargetLocation) */
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<AActor> TargetActor;

	/** Acceptance radius for reaching the destination */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (ClampMin = "0", Units = "cm"))
	float AcceptanceRadius = 50.0f;
};

/**
 *  StateTree task that navigates the AI pawn to a target.
 *  Completes asynchronously when the destination is reached.
 */
USTRUCT(meta = (DisplayName = "AxM Move To", Category = "AxM|Tasks"))
struct ANIMUSEXMACHINA_API FAxMTask_MoveTo : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FAxMTask_MoveToInstanceData;

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
