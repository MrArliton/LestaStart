
#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Components/ArrowComponent.h" 
#include "Curves/CurveFloat.h"
#include "TraceEstimatorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTraceEstimatorDelegateTwo, float, Value, AActor*, HitActor);

/** Create a trace and compare the distance to the actor using floating curves, otherwise just return the distance
 * Curse timeline "-1.0" - trace did not reach a actor, ">0.0" - Hit some actor   */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LESTASTART_API UTraceEstimatorComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UTraceEstimatorComponent();

	/** Call trace logic */
	float MakeTrace();
	UFUNCTION(BlueprintCallable, Category = "Corrector")
	float GetLastValue();

	/** The delegate is called every trace  */
	UPROPERTY(BlueprintAssignable, Category = "Corrector")
	FTraceEstimatorDelegateTwo OnEstimateTrace;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	/** If AutoTrace enabled - call "OnTrace" delegate and update "EstimatedValue" */
	UPROPERTY(EditDefaultsOnly, Category = "Corrector")
	bool bAutoTrace = false;
	
	/** The interval between traces */
	UPROPERTY(EditDefaultsOnly, Category = "Corrector|Trace", meta = (ClampMin = "0.0", Units = "s", EditCondition = "bAutoTrace"))
	float TraceInterval;
	/** Trace sphere radius */
	UPROPERTY(EditDefaultsOnly, Category = "Corrector|Trace", meta = (ClampMin = "0.0"))
	float TraceSphereRadius;
	/** Max distance for tracing */
	UPROPERTY(EditDefaultsOnly, Category = "Corrector|Trace", meta = (ClampMin = "0.0"))
	float MaxDistance;

	/** Collision channel for tracing */
	UPROPERTY(EditDefaultsOnly, Category = "Corrector|Trace")
	TEnumAsByte<ECollisionChannel> CollisionChannel;

	FCollisionQueryParams TraceParams;

	/** Curve - used for estimating value */
	UPROPERTY(EditDefaultsOnly, Category = "Corrector")
	bool bUseCurve = false;
	UPROPERTY(EditDefaultsOnly, Category = "Corrector", meta = (EditCondition = "bUseCurve"))
	UCurveFloat* EstimationCurve;

private:
	/** Evaluated last result */
	float EstimatedValue;
	/** Last hited actor */
	AActor* HitActor;
	/** Arrow indicate the direction of a component */
	UPROPERTY(VisibleDefaultsOnly, Category = "Editor") 
	UArrowComponent* ArrowComponent;
};

