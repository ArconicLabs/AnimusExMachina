// Copyright ArconicLabs. All Rights Reserved.

#include "StateTree/Tasks/AxMGlobalTask_Suspicion.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FAxMGlobalTask_Suspicion::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.SuspicionLevel = 0.0f;
	InstanceData.bIsSuspicious = false;
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FAxMGlobalTask_Suspicion::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (InstanceData.HearingStrength > 0.0f)
	{
		// accumulate suspicion from hearing events
		InstanceData.SuspicionLevel += InstanceData.HearingStrength
			* InstanceData.SuspicionGainMultiplier;
	}
	else
	{
		// decay suspicion when no stimuli arrive
		InstanceData.SuspicionLevel -= InstanceData.SuspicionDecayRate * DeltaTime;
	}

	InstanceData.SuspicionLevel = FMath::Clamp(InstanceData.SuspicionLevel, 0.0f, 1.0f);
	InstanceData.bIsSuspicious = InstanceData.SuspicionLevel > InstanceData.InvestigateThreshold;

	return EStateTreeRunStatus::Running;
}

#if WITH_EDITOR
FText FAxMGlobalTask_Suspicion::GetDescription(
	const FGuid& ID,
	FStateTreeDataView InstanceDataView,
	const IStateTreeBindingLookup& BindingLookup,
	EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString(TEXT("<b>AxM Suspicion</b>"));
}
#endif
