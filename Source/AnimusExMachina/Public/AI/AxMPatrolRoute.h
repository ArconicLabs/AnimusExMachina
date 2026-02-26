// Copyright ArconicLabs. All Rights Reserved.

// Level-placed actor with a spline component defining a patrol route.
// Each spline control point represents a patrol waypoint. Designers
// add and arrange points visually in the level editor.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AxMPatrolRoute.generated.h"

class USplineComponent;

UCLASS()
class ANIMUSEXMACHINA_API AAxMPatrolRoute : public AActor
{
	GENERATED_BODY()

public:

	AAxMPatrolRoute();

	/** Returns world-space positions of all spline control points */
	UFUNCTION(BlueprintCallable, Category = "Animus Ex Machina|Patrol")
	TArray<FVector> GetPatrolPoints() const;

	/** Returns the number of patrol points */
	UFUNCTION(BlueprintCallable, Category = "Animus Ex Machina|Patrol")
	int32 GetNumPatrolPoints() const;

	/** Returns a specific patrol point by index (world space) */
	UFUNCTION(BlueprintCallable, Category = "Animus Ex Machina|Patrol")
	FVector GetPatrolPoint(int32 Index) const;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animus Ex Machina|Patrol")
	TObjectPtr<USplineComponent> SplineComp;
};
