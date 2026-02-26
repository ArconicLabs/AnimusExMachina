// Copyright ArconicLabs. All Rights Reserved.

// Lightweight debug task that draws a label above the NPC's head while
// the state is active. Add to any state for visual debugging in PIE.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "AxMTask_DebugState.generated.h"

class AAIController;

USTRUCT()
struct FAxMTask_DebugStateInstanceData
{
	GENERATED_BODY()

	/** AI Controller to get the pawn location */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AAIController> Controller;

	/** Label drawn above the NPC's head */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	FString StateLabel = TEXT("State");

	/** Text color */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	FColor LabelColor = FColor::White;

	/** Vertical offset above the pawn (cm) */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (Units = "cm"))
	float HeightOffset = 120.0f;
};

USTRUCT(meta = (DisplayName = "AxM Debug State", Category = "AxM|Debug"))
struct ANIMUSEXMACHINA_API FAxMTask_DebugState : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FAxMTask_DebugStateInstanceData;

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) const override;

	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context,
		const float DeltaTime) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView,
		const IStateTreeBindingLookup& BindingLookup,
		EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};
