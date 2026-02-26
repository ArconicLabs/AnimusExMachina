// Copyright ArconicLabs. All Rights Reserved.

// StateTree condition that checks whether a bound bool property indicates
// the target is within attack range.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeConditionBase.h"
#include "AxMCondition_IsInAttackRange.generated.h"

/**
 *  Instance data for the IsInAttackRange condition.
 */
USTRUCT()
struct FAxMCondition_IsInAttackRangeInstanceData
{
	GENERATED_BODY()

	/** Whether the target is in attack range (bound from TargetTracking output) */
	UPROPERTY(EditAnywhere, Category = "Input")
	bool bIsInAttackRange = false;

	/** If true, the condition passes when NOT in attack range */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bInvert = false;
};

/**
 *  StateTree condition that returns the value of a bound IsInAttackRange bool.
 */
USTRUCT(meta = (DisplayName = "AxM Is In Attack Range", Category = "Animus Ex Machina|Conditions"))
struct ANIMUSEXMACHINA_API FAxMCondition_IsInAttackRange : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FAxMCondition_IsInAttackRangeInstanceData;

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView,
		const IStateTreeBindingLookup& BindingLookup,
		EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};
