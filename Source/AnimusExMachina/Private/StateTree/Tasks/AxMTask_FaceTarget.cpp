// Copyright ArconicLabs. All Rights Reserved.

#include "StateTree/Tasks/AxMTask_FaceTarget.h"
#include "AIController.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeExecutionTypes.h"

EStateTreeRunStatus FAxMTask_FaceTarget::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	if (Transition.ChangeType != EStateTreeStateChangeType::Changed)
	{
		return EStateTreeRunStatus::Running;
	}

	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (InstanceData.Controller && IsValid(InstanceData.TargetActor))
	{
		InstanceData.Controller->SetFocus(InstanceData.TargetActor);
	}

	return EStateTreeRunStatus::Running;
}

void FAxMTask_FaceTarget::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	if (Transition.ChangeType != EStateTreeStateChangeType::Changed)
	{
		return;
	}

	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (InstanceData.Controller)
	{
		InstanceData.Controller->ClearFocus(EAIFocusPriority::Gameplay);
	}
}

#if WITH_EDITOR
FText FAxMTask_FaceTarget::GetDescription(
	const FGuid& ID,
	FStateTreeDataView InstanceDataView,
	const IStateTreeBindingLookup& BindingLookup,
	EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString(TEXT("<b>AxM Face Target</b>"));
}
#endif
