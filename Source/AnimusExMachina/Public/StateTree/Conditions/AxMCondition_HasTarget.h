// Copyright ArconicLabs. All Rights Reserved.

// StateTree condition that evaluates whether a target actor reference
// is valid (non-null).

#pragma once

#include "CoreMinimal.h"
#include "StateTreeConditionBase.h"
#include "AxMCondition_HasTarget.generated.h"

/**
 *  Instance data for the HasTarget condition.
 */
USTRUCT()
struct FAxMCondition_HasTargetInstanceData
{
	GENERATED_BODY()

	/** Target actor to check (bound from Perception output) */
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<AActor> TargetActor;

	/** If true, the condition passes when TargetActor is NOT valid */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bInvert = false;
};

/**
 *  StateTree condition that returns true when TargetActor is valid.
 */
USTRUCT(meta = (DisplayName = "AxM Has Target", Category = "Animus Ex Machina|Conditions"))
struct ANIMUSEXMACHINA_API FAxMCondition_HasTarget : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FAxMCondition_HasTargetInstanceData;

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
