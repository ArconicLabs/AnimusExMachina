// Copyright ArconicLabs. All Rights Reserved.

// AI Controller that owns the StateTree AI component and AI Perception
// component (sight sense) for AxM NPCs. Caches perception results via
// delegate so Global Tasks can read them safely during StateTree ticks.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AxMAIController.generated.h"

class UStateTreeAIComponent;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
struct FAIStimulus;

UCLASS()
class ANIMUSEXMACHINA_API AAxMAIController : public AAIController
{
	GENERATED_BODY()

public:

	AAxMAIController();

	/** Returns the cached target actor from the most recent perception update */
	UFUNCTION(BlueprintCallable, Category = "AxM|Perception")
	AActor* GetCachedTargetActor() const { return CachedTargetActor; }

	/** Returns the last known location of the target */
	UFUNCTION(BlueprintCallable, Category = "AxM|Perception")
	FVector GetCachedLastKnownLocation() const { return CachedLastKnownLocation; }

	/** Returns the AI Perception component */
	UAIPerceptionComponent* GetAxMPerception() const { return PerceptionComp; }

protected:

	virtual void BeginPlay() override;

	/** Called when a perception target's stimulus changes */
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	/** StateTree AI logic component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AxM|Components",
		meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStateTreeAIComponent> StateTreeAI;

	/** AI Perception component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AxM|Components",
		meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAIPerceptionComponent> PerceptionComp;

	/** Sight sense configuration */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AxM|Perception",
		meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	/** Cached target from perception delegate */
	UPROPERTY(Transient)
	TObjectPtr<AActor> CachedTargetActor;

	/** Cached last known location from perception delegate */
	FVector CachedLastKnownLocation = FVector::ZeroVector;
};
