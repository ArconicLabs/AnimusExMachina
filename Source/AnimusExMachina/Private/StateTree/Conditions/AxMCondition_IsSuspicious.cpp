// Copyright ArconicLabs. All Rights Reserved.

#include "StateTree/Conditions/AxMCondition_IsSuspicious.h"
#include "StateTreeExecutionContext.h"

bool FAxMCondition_IsSuspicious::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	return InstanceData.bInvert ? !InstanceData.bIsSuspicious : InstanceData.bIsSuspicious;
}

#if WITH_EDITOR
FText FAxMCondition_IsSuspicious::GetDescription(
	const FGuid& ID,
	FStateTreeDataView InstanceDataView,
	const IStateTreeBindingLookup& BindingLookup,
	EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString(TEXT("<b>AxM Is Suspicious</b>"));
}
#endif
