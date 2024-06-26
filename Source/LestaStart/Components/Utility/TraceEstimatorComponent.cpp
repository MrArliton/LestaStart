
#include "TraceEstimatorComponent.h"

UTraceEstimatorComponent::UTraceEstimatorComponent()
{
	// Default this component only local
	SetIsReplicatedByDefault(false);

	PrimaryComponentTick.bCanEverTick = true;

	TraceInterval = 0.1f;
	TraceSphereRadius = 4.0f;
	MaxDistance = 500.0f;
	CollisionChannel = ECC_Pawn;
}

void UTraceEstimatorComponent::BeginPlay()
{
	Super::BeginPlay();

	PrimaryComponentTick.TickInterval = TraceInterval;

	if(bTraceIgnoreOwner)
	{
		if(AActor* Owner = GetOwner())
		{
			TraceParams.AddIgnoredActor(Owner);
		}
	}
}


float UTraceEstimatorComponent::MakeTrace()
{
	UWorld* World = GetWorld();
	AActor* Owner = GetOwner();
	// Default distance is undefined = -1.0 
	float Distance = -1.0f;
	// Default last hited actor is nullptr
	HitActor = nullptr;

	if (IsValid(World) && IsValid(Owner))
	{
		const FVector StartPosition = Owner->GetActorLocation();
		// Calculated end of trace
		const FRotator Rotation = Owner->GetActorRotation();
		// Calculate angles in radians
		const float YawRadian = FMath::DegreesToRadians(Rotation.Yaw);		
		const float PitchRadian = FMath::DegreesToRadians(Rotation.Pitch);
		// Calculate normal direction
		const FVector Direction = FVector(FMath::Cos(YawRadian), FMath::Sin(YawRadian), FMath::Sin(PitchRadian));
		// Calculate end position
		const FVector EndPostion = StartPosition + Direction * MaxDistance;
		
		FHitResult HitResult;			
		World->SweepSingleByChannel(HitResult, StartPosition, EndPostion, FRotator::ZeroRotator.Quaternion(), CollisionChannel, FCollisionShape::MakeSphere(TraceSphereRadius), TraceParams);

		if (HitResult.bBlockingHit)
		{
			Distance = HitResult.Distance;
			HitActor = HitResult.GetActor();
		}
	}
	if(bUseCurve) // If use curve take value from it
	{
		if (IsValid(EstimationCurve))
		{
			EstimatedValue = EstimationCurve->GetFloatValue(Distance);			
		}
		else 
		{
			EstimatedValue = Distance;
			UE_LOG(LogTemp, Warning, TEXT("Estimation Curve is undefined, object: %s"), *GetName());
		}
	}
	else  // If use of curves disabled return distance to hit actor
	{
		EstimatedValue = Distance;
	}
	OnEstimateTrace.Broadcast(EstimatedValue, HitActor);
	return EstimatedValue;
}


float UTraceEstimatorComponent::GetLastEstimatedValue() const
{
	return EstimatedValue;
}


AActor* UTraceEstimatorComponent::GetHitActor() const
{
	return HitActor;
}

void UTraceEstimatorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	MakeTrace();
}


void UTraceEstimatorComponent::SetTraceInterval(float NewTraceInterval) 
{
	PrimaryComponentTick.TickInterval = NewTraceInterval;
	TraceInterval = NewTraceInterval;
}

float UTraceEstimatorComponent::GetTraceInterval() const 
{
	return TraceInterval;
}

