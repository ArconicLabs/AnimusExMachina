// Copyright ArconicLabs. All Rights Reserved.

// StateTree task that rotates the AI pawn through a sequence of focal
// points (left, right, forward) to scan the surrounding area. Succeeds
// after LookDuration expires.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "AxMTask_LookAround.generated.h"

class AAIController;

USTRUCT()
struct FAxMTask_LookAroundInstanceData
{
	GENERATED_BODY()

	/** AI Controller for setting focal point */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AAIController> Controller;

	/** Total duration to spend scanning */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (ClampMin = "0.1", Units = "s"))
	float LookDuration = 3.0f;

	/** Accumulated time */
	float ElapsedTime = 0.0f;

	/** Current look direction index (0=left, 1=right, 2=forward) */
	int32 LookPhase = 0;

	/** Precomputed focal points set on EnterState */
	FVector FocalPoints[3];
};

USTRUCT(meta = (DisplayName = "AxM Look Around", Category = "AxM|Tasks"))
struct ANIMUSEXMACHINA_API FAxMTask_LookAround : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FAxMTask_LookAroundInstanceData;

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) const override;

	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context,
		const float DeltaTime) const override;

	virtual void ExitState(FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView,
		const IStateTreeBindingLookup& BindingLookup,
		EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};
