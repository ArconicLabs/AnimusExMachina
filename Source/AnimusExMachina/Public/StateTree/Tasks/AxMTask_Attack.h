// Copyright ArconicLabs. All Rights Reserved.

// Placeholder attack task that simulates an attack with a configurable
// timer duration, then completes. Will be replaced with montage-driven
// combat logic in a later milestone.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "AxMTask_Attack.generated.h"

/**
 *  Instance data for the placeholder attack task.
 */
USTRUCT()
struct FAxMTask_AttackInstanceData
{
	GENERATED_BODY()

	/** Pawn performing the attack (for logging context) */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<APawn> Pawn;

	/** Duration of the simulated attack in seconds */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (ClampMin = "0.1", Units = "s"))
	float AttackDuration = 1.0f;

	/** Internal elapsed time tracker */
	float ElapsedTime = 0.0f;
};

/**
 *  Placeholder attack task. Runs for AttackDuration seconds then succeeds.
 */
USTRUCT(meta = (DisplayName = "AxM Attack (Placeholder)", Category = "AxM|Tasks"))
struct ANIMUSEXMACHINA_API FAxMTask_Attack : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FAxMTask_AttackInstanceData;

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
