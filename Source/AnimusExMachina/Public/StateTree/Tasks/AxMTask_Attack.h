// Copyright ArconicLabs. All Rights Reserved.

// StateTree task that plays an attack animation montage and completes
// when it ends. Falls back to a timer-based attack when no montage is
// assigned, allowing combat sub-StateTrees to bind their own montages
// per ability node.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "AxMTask_Attack.generated.h"

class AAIController;
class UAnimMontage;

/**
 *  Instance data for the attack task.
 */
USTRUCT()
struct FAxMTask_AttackInstanceData
{
	GENERATED_BODY()

	/** AI Controller (needed to get the pawn's AnimInstance) */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AAIController> Controller;

	/** Montage to play (bound from combat sub-StateTree ability nodes) */
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UAnimMontage> AttackMontage;

	/** Fallback duration when no montage is assigned */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (ClampMin = "0.1", Units = "s"))
	float AttackDuration = 1.0f;

	// --- Internal state ---
	float ElapsedTime = 0.0f;
	bool bUsingMontage = false;
};

/**
 *  Attack task — plays a montage or falls back to timer.
 */
USTRUCT(meta = (DisplayName = "AxM Attack", Category = "Animus Ex Machina|Tasks"))
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

	virtual void ExitState(FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView,
		const IStateTreeBindingLookup& BindingLookup,
		EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};
