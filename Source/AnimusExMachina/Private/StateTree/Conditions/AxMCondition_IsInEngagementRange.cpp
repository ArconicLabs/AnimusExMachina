// Copyright ArconicLabs. All Rights Reserved.

#include "StateTree/Conditions/AxMCondition_IsInEngagementRange.h"
#include "StateTreeExecutionContext.h"

bool FAxMCondition_IsInEngagementRange::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	return InstanceData.bInvert ? !InstanceData.bIsInEngagementRange : InstanceData.bIsInEngagementRange;
}

#if WITH_EDITOR
FText FAxMCondition_IsInEngagementRange::GetDescription(
	const FGuid& ID,
	FStateTreeDataView InstanceDataView,
	const IStateTreeBindingLookup& BindingLookup,
	EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString(TEXT("<b>Is In Engagement Range</b>"));
}
#endif
