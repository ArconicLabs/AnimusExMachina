// Copyright ArconicLabs. All Rights Reserved.

#include "StateTree/Conditions/AxMCondition_IsInAttackRange.h"
#include "StateTreeExecutionContext.h"

bool FAxMCondition_IsInAttackRange::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	return InstanceData.bIsInAttackRange;
}

#if WITH_EDITOR
FText FAxMCondition_IsInAttackRange::GetDescription(
	const FGuid& ID,
	FStateTreeDataView InstanceDataView,
	const IStateTreeBindingLookup& BindingLookup,
	EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString(TEXT("<b>Is In Attack Range</b>"));
}
#endif
