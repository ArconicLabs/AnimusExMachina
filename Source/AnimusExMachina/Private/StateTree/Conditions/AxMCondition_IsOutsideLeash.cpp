// Copyright ArconicLabs. All Rights Reserved.

#include "StateTree/Conditions/AxMCondition_IsOutsideLeash.h"
#include "StateTreeExecutionContext.h"

bool FAxMCondition_IsOutsideLeash::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// LeashRadius of 0 means leash is disabled — never trigger
	bool bOutside = false;
	if (InstanceData.LeashRadius > 0.0f)
	{
		bOutside = InstanceData.DistanceFromHome > InstanceData.LeashRadius;
	}

	return InstanceData.bInvert ? !bOutside : bOutside;
}

#if WITH_EDITOR
FText FAxMCondition_IsOutsideLeash::GetDescription(
	const FGuid& ID,
	FStateTreeDataView InstanceDataView,
	const IStateTreeBindingLookup& BindingLookup,
	EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString(TEXT("<b>Is Outside Leash</b>"));
}
#endif
