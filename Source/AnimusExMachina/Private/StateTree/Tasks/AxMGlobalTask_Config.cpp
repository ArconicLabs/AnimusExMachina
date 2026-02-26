// Copyright ArconicLabs. All Rights Reserved.

#include "StateTree/Tasks/AxMGlobalTask_Config.h"
#include "AI/AxMAIController.h"
#include "AnimusExMachina.h"
#include "Config/AxMConfig.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FAxMGlobalTask_Config::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	const AAxMAIController* AxMController = Cast<AAxMAIController>(InstanceData.Controller);
	if (!AxMController)
	{
		UE_LOG(LogAxM, Warning, TEXT("Config: no AxM AI Controller bound"));
		return EStateTreeRunStatus::Running;
	}

	const UAxMConfig* ConfigAsset = AxMController->GetConfig();
	if (!ConfigAsset)
	{
		UE_LOG(LogAxM, Verbose, TEXT("Config: no Data Asset assigned — using defaults"));
		return EStateTreeRunStatus::Running;
	}

	// Copy all values from the Data Asset to bindable outputs
	InstanceData.EngagementRange = ConfigAsset->EngagementRange;
	InstanceData.LeashRadius = ConfigAsset->LeashRadius;
	InstanceData.PatrolWaitDuration = ConfigAsset->PatrolWaitDuration;
	InstanceData.PatrolMovementSpeed = ConfigAsset->PatrolMovementSpeed;
	InstanceData.InvestigateMovementSpeed = ConfigAsset->InvestigateMovementSpeed;
	InstanceData.CombatMovementSpeed = ConfigAsset->CombatMovementSpeed;
	InstanceData.SearchMovementSpeed = ConfigAsset->SearchMovementSpeed;
	InstanceData.SearchRadius = ConfigAsset->SearchRadius;
	InstanceData.SearchDuration = ConfigAsset->SearchDuration;
	InstanceData.SuspicionDecayRate = ConfigAsset->SuspicionDecayRate;
	InstanceData.InvestigateThreshold = ConfigAsset->InvestigateThreshold;
	InstanceData.SuspicionGainMultiplier = ConfigAsset->SuspicionGainMultiplier;
	InstanceData.PatrolTag = ConfigAsset->PatrolTag;
	InstanceData.InvestigateTag = ConfigAsset->InvestigateTag;
	InstanceData.CombatTag = ConfigAsset->CombatTag;
	InstanceData.SearchTag = ConfigAsset->SearchTag;

	return EStateTreeRunStatus::Running;
}

#if WITH_EDITOR
FText FAxMGlobalTask_Config::GetDescription(
	const FGuid& ID,
	FStateTreeDataView InstanceDataView,
	const IStateTreeBindingLookup& BindingLookup,
	EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString(TEXT("<b>AxM Config</b>"));
}
#endif
