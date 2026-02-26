// Copyright ArconicLabs. All Rights Reserved.

// StateTree condition that checks whether a bound bool property indicates
// the target is within engagement range.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeConditionBase.h"
#include "AxMCondition_IsInEngagementRange.generated.h"

/**
 *  Instance data for the IsInEngagementRange condition.
 */
USTRUCT()
struct FAxMCondition_IsInEngagementRangeInstanceData
{
	GENERATED_BODY()

	/** Whether the target is in engagement range (bound from TargetTracking output) */
	UPROPERTY(EditAnywhere, Category = "Input")
	bool bIsInEngagementRange = false;

	/** If true, the condition passes when NOT in engagement range */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bInvert = false;
};

/**
 *  StateTree condition that returns the value of a bound IsInEngagementRange bool.
 */
USTRUCT(meta = (DisplayName = "AxM Is In Engagement Range", Category = "Animus Ex Machina|Conditions"))
struct ANIMUSEXMACHINA_API FAxMCondition_IsInEngagementRange : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FAxMCondition_IsInEngagementRangeInstanceData;

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
