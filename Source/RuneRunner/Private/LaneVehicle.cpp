// Fill out your copyright notice in the Description page of Project Settings.


#include "LaneVehicle.h"
#include "Track/LaneManager.h"
#include "Track/BaseLaneSegment.h"
#include "GameModes/RunnerLevelGM.h"
#include "Components/ArrowComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ALaneVehicle::ALaneVehicle()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	UArrowComponent* Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("RootArrowComponent"));
	Arrow->SetHiddenInGame(false);
	Arrow->SetVisibility(true);
	SetRootComponent(Arrow);


	VehicleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VehicleMeshComponent"));
	VehicleMesh->SetupAttachment(GetRootComponent());

	static ConstructorHelpers::FObjectFinder<UCurveFloat> Curve(TEXT("/Game/Data/Curves/Crv_LaneSwitch"));
	check(Curve.Succeeded());
	FloatCurve_LaneChange = Curve.Object;

	static ConstructorHelpers::FObjectFinder<UCurveFloat> Curve2(TEXT("/Game/Data/Curves/Crv_Jump"));
	check(Curve2.Succeeded());
	FloatCurve_Jump = Curve2.Object;
}

// Called when the game starts or when spawned
void ALaneVehicle::BeginPlay()
{
	Super::BeginPlay();

	CurrentVehicleState = EVehicleState::EVS_NotActive;

	GameModeRef = GetWorld()->GetAuthGameMode<ARunnerLevelGM>();
	LaneManagerRef = GameModeRef->GetLaneManager();
	MaximumLanes = LaneManagerRef->GetNumberOfLanes();

	FOnTimelineFloat onTimelineCallback;
	FOnTimelineEventStatic onTimelineFinishedCallback;

	if (FloatCurve_LaneChange)
	{
		MoveLaneTimeline = NewObject<UTimelineComponent>(this, FName("TimelineAnimation"));
		MoveLaneTimeline->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		this->BlueprintCreatedComponents.Add(MoveLaneTimeline);

		MoveLaneTimeline->SetNetAddressable();    // This component has a stable name that can be referenced for replication

		MoveLaneTimeline->SetPropertySetObject(this); // Set which object the timeline should drive properties on
		MoveLaneTimeline->SetDirectionPropertyName(FName("TimelineDirection"));

		MoveLaneTimeline->SetLooping(false);
		MoveLaneTimeline->SetTimelineLength(1.0f);
		MoveLaneTimeline->SetPlayRate(2.5f);
		MoveLaneTimeline->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);

		MoveLaneTimeline->SetPlaybackPosition(0.0f, false);

		//Add the float curve to the timeline and connect it to your timelines's interpolation function
		onTimelineCallback.BindUFunction(this, FName{ TEXT("TimelineCallback") });
		onTimelineFinishedCallback.BindUFunction(this, FName{ TEXT("TimelineFinishedCallback") });
		MoveLaneTimeline->AddInterpFloat(FloatCurve_LaneChange, onTimelineCallback);
		MoveLaneTimeline->SetTimelineFinishedFunc(onTimelineFinishedCallback);

		MoveLaneTimeline->RegisterComponent();

		// Jump Timeline

		JumpTimeline = NewObject<UTimelineComponent>(this, FName("Jump Timeline"));
		JumpTimeline->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		this->BlueprintCreatedComponents.Add(JumpTimeline);

		JumpTimeline->SetNetAddressable();    // This component has a stable name that can be referenced for replication

		JumpTimeline->SetPropertySetObject(this); // Set which object the timeline should drive properties on
		JumpTimeline->SetDirectionPropertyName(FName("TimelineDirection"));

		JumpTimeline->SetLooping(false);
		JumpTimeline->SetTimelineLength(1.0f);
		JumpTimeline->SetPlayRate(1.0f);
		JumpTimeline->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);

		JumpTimeline->SetPlaybackPosition(0.0f, false);

		//Add the float curve to the timeline and connect it to your timelines's interpolation function
		onTimelineCallback.BindUFunction(this, FName{ TEXT("TimelineCallback") });
		onTimelineFinishedCallback.BindUFunction(this, FName{ TEXT("TimelineFinishedCallback") });
		JumpTimeline->AddInterpFloat(FloatCurve_Jump, onTimelineCallback);
		JumpTimeline->SetTimelineFinishedFunc(onTimelineFinishedCallback);

		JumpTimeline->RegisterComponent();
	}
}

void ALaneVehicle::MoveToLane(int NewLaneIndex)
{
	if (FMath::IsWithin(NewLaneIndex, 0, MaximumLanes) && (CurrentVehicleState == EVehicleState::EVS_InLane || CurrentVehicleState == EVehicleState::EVS_NotActive ))
	{

		CurrentVehicleState = EVehicleState::EVS_ChangingLane;

		int LaneDifference = LaneIndex - NewLaneIndex;
		RotateVehicleToSide(LaneDifference);

		TTuple<FVector, FVector> LineVectors = LaneManagerRef->GetLaneLineVectors(NewLaneIndex);
		auto LineDirection = LineVectors.Get<0>() - LineVectors.Get<1>();
		TargetLocation = UKismetMathLibrary::FindClosestPointOnLine(GetActorLocation(), LineVectors.Get<0>(), LineDirection);
		TargetLocation.Z += 150.0f;

		LaneChanged(TargetLocation);
		//SetActorLocation(TargetLocation);
		LaneIndex = NewLaneIndex;
		CurrentVehicleState = EVehicleState::EVS_InLane;

	}
}

void ALaneVehicle::VehicleJump()
{
	if (CurrentVehicleState == EVehicleState::EVS_InLane)
	{
		CurrentVehicleState = EVehicleState::EVS_Jumping;
		JumpHangTime = 0.0f;
		JumpStateChanged();
		//OldLocation = GetActorLocation();
		//TargetLocation = OldLocation + FVector(0.0f, 0.0f, 500.0f);
		//JumpTimeline->PlayFromStart();
	}
}

void ALaneVehicle::RotateVehicleToSide(int IndexChange)
{
	SideUpIndex = SideUpIndex + IndexChange;
	if (SideUpIndex < 0)
	{
		SideUpIndex += VehicleSidesMax;
	}
	else if (SideUpIndex >= VehicleSidesMax)
	{
		SideUpIndex -= VehicleSidesMax;
	}

	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, FString::Printf(TEXT("New Side Up Index val: %i"), SideUpIndex));
	SideChanged();
}

// Called on every tick in timeline.
void ALaneVehicle::TimelineCallback(float val)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("TimelineCallback val: %f"), val));

	//if (CurrentVehicleState == EVehicleState::EVS_ChangingLane)
	//{
	//	FVector DistanceToMove = TargetLocation - OldLocation;
	//	FVector IntermediaryLocation = OldLocation + (DistanceToMove * val);
	//	SetActorLocation(IntermediaryLocation);

	//	//FVector DistanceToMove = TargetLocation - GetActorLocation();
	//	//FVector TimelineAdjustedDistanceToMove = DistanceToMove * val;
	//	//SetActorLocation(GetActorLocation() + TimelineAdjustedDistanceToMove);

	//	FRotator TotalRotationToRotate = TargetRotation - OldRotation;
	//	FRotator IntermediaryRotation = OldRotation + (TotalRotationToRotate * val);
	//	VehicleMesh->SetRelativeRotationExact(IntermediaryRotation);
	//	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, FString::Printf(TEXT("Mesh Relative Rotation: %s"), *VehicleMesh->GetRelativeRotation().ToCompactString()));
	//} 
	//else if (CurrentVehicleState == EVehicleState::EVS_Jumping)
	//{
	//	FVector DistanceToMove = TargetLocation - OldLocation;
	//	FVector IntermediaryLocation = OldLocation + (DistanceToMove * val);
	//	SetActorLocation(IntermediaryLocation);
	//}
}

// Called when timeline finishes
void ALaneVehicle::TimelineFinishedCallback()
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, FString(TEXT("Timeline Complete")));
	CurrentVehicleState = EVehicleState::EVS_InLane;
}

// Called every frame
void ALaneVehicle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentVehicleState == EVehicleState::EVS_Jumping)
	{
		JumpHangTime += DeltaTime;

		if (JumpHangTime >= 1.0f)
		{
			CurrentVehicleState = EVehicleState::EVS_InLane;
			JumpHangTime = 0.0f;
			JumpStateChanged();
		}
	}



	// Timeline Stuff

	MoveLaneTimeline->TickComponent(DeltaTime, ELevelTick::LEVELTICK_TimeOnly, nullptr);

	// Lane Detection
	FHitResult Hit;

	FVector TraceStart = GetActorLocation();
	FVector TraceEnd = GetActorLocation() + (GetActorUpVector() * -1) * 1000.0f;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, TraceChannelProperty, QueryParams);

	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, Hit.bBlockingHit ? FColor::Blue : FColor::Red, false, -1.0f, 0, 10.0f);

	if (Hit.bBlockingHit && IsValid(Hit.GetActor()))
	{
		ABaseLaneSegment* LaneUnderneath = Cast<ABaseLaneSegment>(Hit.GetActor());
		if (LaneUnderneath)
		{
			CurrentlyOnTrackType = LaneUnderneath->GetTrackType();
		}
	}
	else
	{
	}


}

// Called to bind functionality to input
void ALaneVehicle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}




