// Copyright ArconicLabs. All Rights Reserved.

// StateTree condition that checks whether the AI is suspicious, driven
// by the Suspicion global task's bIsSuspicious output. Supports bInvert.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeConditionBase.h"
#include "AxMCondition_IsSuspicious.generated.h"

USTRUCT()
struct FAxMCondition_IsSuspiciousInstanceData
{
	GENERATED_BODY()

	/** Bound from Suspicion global task output */
	UPROPERTY(EditAnywhere, Category = "Input")
	bool bIsSuspicious = false;

	/** When true, inverts the condition result */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bInvert = false;
};

USTRUCT(meta = (DisplayName = "AxM Is Suspicious", Category = "Animus Ex Machina|Conditions"))
struct ANIMUSEXMACHINA_API FAxMCondition_IsSuspicious : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FAxMCondition_IsSuspiciousInstanceData;

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
