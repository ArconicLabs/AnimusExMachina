// Copyright ArconicLabs. All Rights Reserved.

// StateTree condition that evaluates whether the NPC has exceeded its
// leash radius from home. When LeashRadius is 0 the leash is disabled
// and the condition always returns false.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeConditionBase.h"
#include "AxMCondition_IsOutsideLeash.generated.h"

/**
 *  Instance data for the IsOutsideLeash condition.
 */
USTRUCT()
struct FAxMCondition_IsOutsideLeashInstanceData
{
	GENERATED_BODY()

	/** Current distance from home (bound from Perception output) */
	UPROPERTY(EditAnywhere, Category = "Input")
	float DistanceFromHome = 0.0f;

	/** Max allowed distance from home (bound from Config output, 0 = disabled) */
	UPROPERTY(EditAnywhere, Category = "Input")
	float LeashRadius = 0.0f;

	/** If true, the condition logic is inverted */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bInvert = false;
};

/**
 *  StateTree condition that returns true when the NPC has exceeded its leash radius.
 */
USTRUCT(meta = (DisplayName = "AxM Is Outside Leash", Category = "Animus Ex Machina|Conditions"))
struct ANIMUSEXMACHINA_API FAxMCondition_IsOutsideLeash : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FAxMCondition_IsOutsideLeashInstanceData;

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
