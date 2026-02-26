// Copyright ArconicLabs. All Rights Reserved.

#include "StateTree/Tasks/AxMTask_Attack.h"
#include "AnimusExMachina.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeExecutionTypes.h"

EStateTreeRunStatus FAxMTask_Attack::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	if (Transition.ChangeType != EStateTreeStateChangeType::Changed)
	{
		return EStateTreeRunStatus::Running;
	}

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.ElapsedTime = 0.0f;

	if (InstanceData.Pawn)
	{
		UE_LOG(LogAxM, Log, TEXT("%s begins attack (duration: %.1fs)"),
			*InstanceData.Pawn->GetName(), InstanceData.AttackDuration);
	}

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FAxMTask_Attack::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.ElapsedTime += DeltaTime;

	if (InstanceData.ElapsedTime >= InstanceData.AttackDuration)
	{
		if (InstanceData.Pawn)
		{
			UE_LOG(LogAxM, Log, TEXT("%s attack landed"),
				*InstanceData.Pawn->GetName());
		}
		InstanceData.ElapsedTime = 0.0f;
		return EStateTreeRunStatus::Running;
	}

	return EStateTreeRunStatus::Running;
}

#if WITH_EDITOR
FText FAxMTask_Attack::GetDescription(
	const FGuid& ID,
	FStateTreeDataView InstanceDataView,
	const IStateTreeBindingLookup& BindingLookup,
	EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString(TEXT("<b>AxM Attack (Placeholder)</b>"));
}
#endif
