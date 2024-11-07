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

}

void ALaneVehicle::JumpEnded()
{
	CurrentVehicleState = EVehicleState::EVS_InLane;
	VehicleInputState = EVehicleInputState::EVIS_Available;
}

// Called by BP on animation completion.
void ALaneVehicle::LaneChangeComplete()
{
	CurrentVehicleState = EVehicleState::EVS_InLane;
	VehicleInputState = EVehicleInputState::EVIS_Available;
}

// Called when the game starts or when spawned
void ALaneVehicle::BeginPlay()
{
	Super::BeginPlay();

	CurrentVehicleState = EVehicleState::EVS_NotActive;

	GameModeRef = GetWorld()->GetAuthGameMode<ARunnerLevelGM>();
	LaneManagerRef = GameModeRef->GetLaneManager();
	MaximumLanes = LaneManagerRef->GetNumberOfLanes();

}

void ALaneVehicle::MoveDirection(int LaneChangeDir)
{
	if (CurrentVehicleState == EVehicleState::EVS_InLane && VehicleInputState == EVehicleInputState::EVIS_Available)
	{
		if (MoveToLane(LaneIndex + LaneChangeDir))
		{
			VehicleInputState = EVehicleInputState::EVIS_Processing;
		}
	}
	else if (CurrentVehicleState == EVehicleState::EVS_Jumping && VehicleInputState == EVehicleInputState::EVIS_Processing)
	{
		RotateVehicleToSide(LaneChangeDir * -1);
		VehicleInputState = EVehicleInputState::EVIS_Processing;
	}
}

bool ALaneVehicle::MoveToLane(int NewLaneIndex)
{
	if (FMath::IsWithin(NewLaneIndex, 0, MaximumLanes) && 
		(CurrentVehicleState == EVehicleState::EVS_InLane || CurrentVehicleState == EVehicleState::EVS_NotActive ))
	{

		CurrentVehicleState = EVehicleState::EVS_ChangingLane;

		int LaneDifference = LaneIndex - NewLaneIndex;
		RotateVehicleToSide(LaneDifference);

		TTuple<FVector, FVector> LineVectors = LaneManagerRef->GetLaneLineVectors(NewLaneIndex);
		auto LineDirection = LineVectors.Get<0>() - LineVectors.Get<1>();
		TargetLocation = UKismetMathLibrary::FindClosestPointOnLine(GetActorLocation(), LineVectors.Get<0>(), LineDirection);
		TargetLocation.Z += 120.0f;

		LaneChanged(TargetLocation);
		LaneIndex = NewLaneIndex;
		return true;
	}
	else {
		return false;
	}
}

void ALaneVehicle::VehicleJump()
{
	if (VehicleInputState == EVehicleInputState::EVIS_Available)
	{
		CurrentVehicleState = EVehicleState::EVS_Jumping;
		VehicleInputState = EVehicleInputState::EVIS_Processing;
		JumpEvent();
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

	SideChanged();

}

// Called every frame
void ALaneVehicle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
			if (LaneSegmentIdentifier.IsValid())
			{
				if (LaneSegmentIdentifier != LaneUnderneath->GetActorGuid())
				{
					LaneSegmentIdentifier = LaneUnderneath->GetActorGuid();
					LaneEffectApplied = false;
					CurrentlyOnTrackType = LaneUnderneath->GetTrackType();
				}
			}
			else {
				LaneSegmentIdentifier = LaneUnderneath->GetActorGuid();
			}
		}
	}
	else
	{
	}

	if (!LaneEffectApplied)
	{
		switch (CurrentlyOnTrackType)
		{
			case ETrackType::ETT_Gap:
			case ETrackType::ETT_Wall:
			case ETrackType::ETT_Rough:
				VehicleHealth--;
				break;
			default:
				break;
		}
		LaneEffectApplied = true;
	}

	DrawDebugString(GetWorld(), GetActorLocation(), FString::Printf(TEXT("%i"), VehicleHealth), nullptr, FColor::Red, 0.0f, false, 1.0f);

}

// Called to bind functionality to input
void ALaneVehicle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}




