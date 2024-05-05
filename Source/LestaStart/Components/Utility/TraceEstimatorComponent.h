
#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Components/ArrowComponent.h" 
#include "Curves/CurveFloat.h"
#include "TraceEstimatorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTraceEstimatorDelegateTwo, float, Value, AActor*, HitActor);

/**
 * Create a trace and compare the distance to the actor using floating curves, otherwise just return the distance
 * Curse timeline "-1.0" - trace did not reach a actor, ">=0.0" - Hit some actor 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LESTASTART_API UTraceEstimatorComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UTraceEstimatorComponent();

	/** Call trace logic */
	float MakeTrace();
	UFUNCTION(BlueprintCallable, Category = "Corrector")
	float GetLastEstimatedValue() const;
	UFUNCTION(BlueprintCallable, Category = "Corrector")
	AActor* GetHitActor() const;

	/** Set TraceInterval - TickInterval */
	void SetTraceInterval(float NewTraceInterval);
	/** Interval between traces */
	float GetTraceInterval() const;

	/** The delegate is called every trace  */
	UPROPERTY(BlueprintAssignable, Category = "Corrector")
	FTraceEstimatorDelegateTwo OnEstimateTrace;

	/** If AutoTrace enabled - call "OnTrace" delegate and update "EstimatedValue" */
	UPROPERTY(EditDefaultsOnly, Category = "Corrector")
	bool bAutoTrace = false;
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

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void BeginPlay() override;

protected:
	/** The interval between traces */
	UPROPERTY(EditDefaultsOnly, Category = "Corrector|Trace", meta = (ClampMin = "0.0", Units = "s", EditCondition = "bAutoTrace"))
	float TraceInterval;
	/** Add owner to ignore list  */
	UPROPERTY(EditDefaultsOnly, Category = "Corrector|Trace")
	bool bTraceIgnoreOwner = true;

private:
	/** Evaluated last result */
	float EstimatedValue = 0.0f;
	/** Last hited actor */
	AActor* HitActor;
};

