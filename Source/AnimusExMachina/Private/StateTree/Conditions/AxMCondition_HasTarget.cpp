// Copyright ArconicLabs. All Rights Reserved.

#include "StateTree/Conditions/AxMCondition_HasTarget.h"
#include "StateTreeExecutionContext.h"

bool FAxMCondition_HasTarget::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	return IsValid(InstanceData.TargetActor);
}

#if WITH_EDITOR
FText FAxMCondition_HasTarget::GetDescription(
	const FGuid& ID,
	FStateTreeDataView InstanceDataView,
	const IStateTreeBindingLookup& BindingLookup,
	EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString(TEXT("<b>Has Target</b>"));
}
#endif
