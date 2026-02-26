// Copyright ArconicLabs. All Rights Reserved.

#include "StateTree/Tasks/AxMTask_AlertState.h"
#include "AI/AxMAIController.h"
#include "AnimusExMachina.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FAxMTask_AlertState::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	AAxMAIController* AxMController = Cast<AAxMAIController>(InstanceData.Controller);
	if (!AxMController)
	{
		UE_LOG(LogAxM, Warning, TEXT("AlertState: no AxM AI Controller bound"));
		return EStateTreeRunStatus::Running;
	}

	if (InstanceData.AlertStateTag.IsValid())
	{
		AxMController->SetAlertState(InstanceData.AlertStateTag);
	}

	return EStateTreeRunStatus::Running;
}

#if WITH_EDITOR
FText FAxMTask_AlertState::GetDescription(
	const FGuid& ID,
	FStateTreeDataView InstanceDataView,
	const IStateTreeBindingLookup& BindingLookup,
	EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString(TEXT("<b>AxM Alert State</b>"));
}
#endif
