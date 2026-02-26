// Copyright ArconicLabs. All Rights Reserved.

// Lightweight state task that notifies the AI Controller of a state
// transition by calling SetAlertState with a bound gameplay tag.
// Place as the first task in each top-level state.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StateTreeTaskBase.h"
#include "AxMTask_AlertState.generated.h"

class AAIController;

/**
 *  Instance data for the alert state task.
 */
USTRUCT()
struct FAxMTask_AlertStateInstanceData
{
	GENERATED_BODY()

	/** AI Controller (cast to AAxMAIController internally) */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AAIController> Controller;

	/** Alert state tag to apply when this state is entered (bound from Config Global Task output) */
	UPROPERTY(EditAnywhere, Category = "Input")
	FGameplayTag AlertStateTag;
};

/**
 *  Notifies the AI Controller when the NPC enters a new alert state.
 *  Calls SetAlertState on enter which broadcasts OnAlertStateChanged
 *  and applies movement speed from the Config Data Asset.
 */
USTRUCT(meta = (DisplayName = "AxM Alert State", Category = "Animus Ex Machina|Tasks"))
struct ANIMUSEXMACHINA_API FAxMTask_AlertState : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FAxMTask_AlertStateInstanceData;

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView,
		const IStateTreeBindingLookup& BindingLookup,
		EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};
