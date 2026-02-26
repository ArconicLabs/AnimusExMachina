// Copyright ArconicLabs. All Rights Reserved.

// AI Controller that owns the StateTree AI component and AI Perception
// component for AxM NPCs. Supports modular senses (sight, hearing, damage)
// with per-subclass toggle flags. Caches perception results via delegate
// so Global Tasks can read them safely during StateTree ticks.
// Optionally reads all tuning from a UAxMConfig Data Asset.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "AxMAIController.generated.h"

class UAxMConfig;
class UStateTreeAIComponent;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class UAISenseConfig_Damage;
struct FAIStimulus;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAxMOnTargetAcquired, AActor*, Target);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAxMOnTargetLost);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAxMOnAlertStateChanged, FGameplayTag, OldState, FGameplayTag, NewState);

UCLASS()
class ANIMUSEXMACHINA_API AAxMAIController : public AAIController
{
	GENERATED_BODY()

public:

	AAxMAIController();

	// --- Config ---

	/** Returns the assigned config Data Asset (may be null) */
	UFUNCTION(BlueprintCallable, Category = "Animus Ex Machina|Config")
	UAxMConfig* GetConfig() const { return Config; }

	// --- Alert State ---

	/** Sets the current alert state tag, broadcasts OnAlertStateChanged, and applies movement speed */
	UFUNCTION(BlueprintCallable, Category = "Animus Ex Machina|Alert State")
	void SetAlertState(const FGameplayTag& NewState);

	/** Returns the current alert state tag */
	UFUNCTION(BlueprintCallable, Category = "Animus Ex Machina|Alert State")
	FGameplayTag GetAlertState() const { return CurrentAlertState; }

	// --- Event Delegates ---

	UPROPERTY(BlueprintAssignable, Category = "Animus Ex Machina|Events")
	FAxMOnTargetAcquired OnTargetAcquired;

	UPROPERTY(BlueprintAssignable, Category = "Animus Ex Machina|Events")
	FAxMOnTargetLost OnTargetLost;

	UPROPERTY(BlueprintAssignable, Category = "Animus Ex Machina|Events")
	FAxMOnAlertStateChanged OnAlertStateChanged;

	// --- Cached perception getters (read by Global Tasks) ---

	/** Returns the cached target actor from the most recent sight or damage event */
	UFUNCTION(BlueprintCallable, Category = "Animus Ex Machina|Perception")
	AActor* GetCachedTargetActor() const { return CachedTargetActor; }

	/** Returns the last known location of the target */
	UFUNCTION(BlueprintCallable, Category = "Animus Ex Machina|Perception")
	FVector GetCachedLastKnownLocation() const { return CachedLastKnownLocation; }

	/** Returns the AI Perception component */
	UAIPerceptionComponent* GetAxMPerception() const { return PerceptionComp; }

	// --- Hearing event consumption (one-shot per tick) ---

	/** Returns true if a hearing stimulus arrived since the last consume */
	bool HasPendingHearingEvent() const { return bHearingEventPending; }

	/** Returns and clears the pending hearing event data */
	void ConsumeHearingEvent(float& OutStrength, FVector& OutLocation);

	// --- Damage event consumption (one-shot per tick) ---

	/** Returns true if a damage stimulus arrived since the last consume */
	bool HasPendingDamageEvent() const { return bDamageEventPending; }

	/** Returns and clears the pending damage event data */
	void ConsumeDamageEvent(AActor*& OutInstigator, FVector& OutLocation);

protected:

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

	/** Called when a perception target's stimulus changes */
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	// --- Config Data Asset ---

	/** Optional archetype config — overrides the loose perception properties below when set */
	UPROPERTY(EditDefaultsOnly, Category = "Animus Ex Machina|Config")
	TObjectPtr<UAxMConfig> Config;

	// --- Sense toggle flags (fallback when Config is null) ---

	UPROPERTY(EditDefaultsOnly, Category = "Animus Ex Machina|Perception")
	bool bEnableSight = true;

	UPROPERTY(EditDefaultsOnly, Category = "Animus Ex Machina|Perception")
	bool bEnableHearing = true;

	UPROPERTY(EditDefaultsOnly, Category = "Animus Ex Machina|Perception")
	bool bEnableDamage = true;

	// --- Configurable sense parameters (fallback when Config is null) ---

	UPROPERTY(EditDefaultsOnly, Category = "Animus Ex Machina|Perception|Sight",
		meta = (EditCondition = "bEnableSight", Units = "cm"))
	float SightRadius = 1500.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Animus Ex Machina|Perception|Sight",
		meta = (EditCondition = "bEnableSight", Units = "cm"))
	float LoseSightRadius = 2000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Animus Ex Machina|Perception|Sight",
		meta = (EditCondition = "bEnableSight", Units = "deg"))
	float PeripheralVisionAngle = 60.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Animus Ex Machina|Perception|Sight",
		meta = (EditCondition = "bEnableSight", Units = "s"))
	float SightMaxAge = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Animus Ex Machina|Perception|Hearing",
		meta = (EditCondition = "bEnableHearing", Units = "cm"))
	float HearingRange = 1500.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Animus Ex Machina|Perception|Hearing",
		meta = (EditCondition = "bEnableHearing", Units = "s"))
	float HearingMaxAge = 3.0f;

	// --- Components ---

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animus Ex Machina|Components",
		meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStateTreeAIComponent> StateTreeAI;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animus Ex Machina|Components",
		meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAIPerceptionComponent> PerceptionComp;

	// --- Sense configurations (created as subobjects, configured in PostInitializeComponents) ---

	UPROPERTY()
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	UPROPERTY()
	TObjectPtr<UAISenseConfig_Hearing> HearingConfig;

	UPROPERTY()
	TObjectPtr<UAISenseConfig_Damage> DamageConfig;

	// --- Alert state ---

	UPROPERTY(BlueprintReadOnly, Category = "Animus Ex Machina|Alert State")
	FGameplayTag CurrentAlertState;

	// --- Cached perception data (written by delegate, read by Global Tasks) ---

	UPROPERTY(Transient)
	TObjectPtr<AActor> CachedTargetActor;

	FVector CachedLastKnownLocation = FVector::ZeroVector;

	FVector CachedStimulusLocation = FVector::ZeroVector;
	float CachedHearingStrength = 0.0f;
	bool bHearingEventPending = false;

	UPROPERTY(Transient)
	TObjectPtr<AActor> CachedDamageInstigator;

	FVector CachedDamageLocation = FVector::ZeroVector;
	bool bDamageEventPending = false;

private:

	/** Applies movement speed from Config based on the given alert state tag */
	void ApplyMovementSpeedForState(const FGameplayTag& StateTag);
};
