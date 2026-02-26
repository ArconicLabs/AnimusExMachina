// Copyright ArconicLabs. All Rights Reserved.

#include "StateTree/Tasks/AxMTask_Attack.h"
#include "AIController.h"
#include "AnimusExMachina.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeAsyncExecutionContext.h"
#include "StateTreeExecutionTypes.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Character.h"

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
	InstanceData.bUsingMontage = false;

	// Try montage path
	if (InstanceData.AttackMontage && InstanceData.Controller)
	{
		APawn* Pawn = InstanceData.Controller->GetPawn();
		ACharacter* Character = Cast<ACharacter>(Pawn);
		UAnimInstance* AnimInstance = Character ? Character->GetMesh()->GetAnimInstance() : nullptr;

		if (AnimInstance)
		{
			const float MontageLength = AnimInstance->Montage_Play(InstanceData.AttackMontage);

			if (MontageLength > 0.0f)
			{
				InstanceData.bUsingMontage = true;

				FOnMontageEnded EndDelegate;
				EndDelegate.BindLambda(
					[WeakContext = Context.MakeWeakExecutionContext()](
						UAnimMontage* /*Montage*/, bool bInterrupted)
					{
						if (bInterrupted)
						{
							WeakContext.FinishTask(EStateTreeFinishTaskType::Failed);
						}
						else
						{
							WeakContext.FinishTask(EStateTreeFinishTaskType::Succeeded);
						}
					}
				);

				AnimInstance->Montage_SetEndDelegate(EndDelegate, InstanceData.AttackMontage);

				UE_LOG(LogAxM, Log, TEXT("%s begins montage attack (%s)"),
					Pawn ? *Pawn->GetName() : TEXT("Unknown"),
					*InstanceData.AttackMontage->GetName());

				return EStateTreeRunStatus::Running;
			}
		}
	}

	// Timer fallback
	APawn* Pawn = InstanceData.Controller ? InstanceData.Controller->GetPawn() : nullptr;
	UE_LOG(LogAxM, Log, TEXT("%s begins attack (duration: %.1fs)"),
		Pawn ? *Pawn->GetName() : TEXT("Unknown"),
		InstanceData.AttackDuration);

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FAxMTask_Attack::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// Montage path — delegate handles completion
	if (InstanceData.bUsingMontage)
	{
		return EStateTreeRunStatus::Running;
	}

	// Timer fallback
	InstanceData.ElapsedTime += DeltaTime;

	if (InstanceData.ElapsedTime >= InstanceData.AttackDuration)
	{
		APawn* Pawn = InstanceData.Controller ? InstanceData.Controller->GetPawn() : nullptr;
		if (Pawn)
		{
			UE_LOG(LogAxM, Log, TEXT("%s attack landed"), *Pawn->GetName());
		}
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;
}

void FAxMTask_Attack::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	if (Transition.ChangeType != EStateTreeStateChangeType::Changed)
	{
		return;
	}

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// Stop montage if still playing
	if (InstanceData.bUsingMontage && InstanceData.Controller)
	{
		APawn* Pawn = InstanceData.Controller->GetPawn();
		ACharacter* Character = Cast<ACharacter>(Pawn);
		UAnimInstance* AnimInstance = Character ? Character->GetMesh()->GetAnimInstance() : nullptr;

		if (AnimInstance && AnimInstance->Montage_IsPlaying(InstanceData.AttackMontage))
		{
			AnimInstance->Montage_Stop(0.25f, InstanceData.AttackMontage);
		}
	}

	InstanceData.ElapsedTime = 0.0f;
	InstanceData.bUsingMontage = false;
}

#if WITH_EDITOR
FText FAxMTask_Attack::GetDescription(
	const FGuid& ID,
	FStateTreeDataView InstanceDataView,
	const IStateTreeBindingLookup& BindingLookup,
	EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString(TEXT("<b>AxM Attack</b>"));
}
#endif
