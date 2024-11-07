// Fill out your copyright notice in the Description page of Project Settings.

#include "Track/BaseLaneSegment.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Track/LaneManager.h"
#include "GameModes/RunnerLevelGM.h"

// Sets default values
ABaseLaneSegment::ABaseLaneSegment()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_StartPhysics;

	RootPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Root Arrow"));
	SetRootComponent(RootPoint);

	SegmentCollisionBox = CreateDefaultSubobject<UBoxComponent>(FName("Segment Collision Box"));
	SegmentCollisionBox->SetupAttachment(GetRootComponent(), FName(TEXT("Collision Box")));
	SegmentCollisionBox->SetBoxExtent(FVector(SegmentLength, SegmentWidth, 1.0f));
	SegmentCollisionBox->bHiddenInGame = false;
	/*SegmentCollisionBox->SetGenerateOverlapEvents(true);
	SegmentCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndProbe);
	SegmentCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap);*/

}

// Called when the game starts or when spawned
void ABaseLaneSegment::BeginPlay()
{
	Super::BeginPlay();
	
	auto GameModeRef = GetWorld()->GetAuthGameMode<ARunnerLevelGM>();
	GameModeRef->SegmentTrackIndexIncreasedDelegate.AddDynamic(this, &ABaseLaneSegment::IncreaseTrackIndex);
	LaneManagerRef = GameModeRef->LaneManager;
	UE_LOG(LogTemp, Log, TEXT("Delegate Should be set"));
}

void ABaseLaneSegment::OnConstruction(const FTransform& Transform)
{
	// "Extent" is length from center.
	SegmentCollisionBox->SetBoxExtent(FVector(SegmentLength / 2.0f, SegmentWidth / 2.0f, 1.0f));
}

// Called every frame
void ABaseLaneSegment::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (InUse)
	{
		if (LaneManagerRef)
		{
			if (TrackIndex >= LaneManagerRef->TrackPacers.Num())
			{
				SetPoolActorInUse(false);
			}
			else 
			{
				FVector PaceLoc = LaneManagerRef->TrackPacers[TrackIndex];
				PaceLoc.Y = GetActorLocation().Y;
				SetActorLocation(PaceLoc);
				//DrawDebugString(GetWorld(), GetActorLocation(), FString::Printf(TEXT("%s"), *PaceLoc.ToString()), nullptr, FColor::Black, 0.f, false, 1.0f);
			}
		}
	}

	//double DistanceFromStart = FVector::Distance(GetActorLocation(), StartingLocation);
	//UE_LOG(LogTemp, Log, TEXT("Lane Index: %i, Distance From Origin: %f"), LaneIndex, DistanceFromStart);
	//if (!SpawnedNewActor && DistanceFromStart > SegmentLength * 2.0f)
	//{
	//	SpawnLaneDelegate.Broadcast(this);
	//	SpawnedNewActor = true;
	//}
	//AddActorWorldOffset(FVector(SegmentSpeed * DeltaTime, 0.f, 0.f), true);

}

void ABaseLaneSegment::SetPoolActorInUse(bool NewValue)
{
	Super::SetPoolActorInUse(NewValue);

	// Put logic that would normally go in BeginPlay() here. 
	if (NewValue)
	{
		auto GameModeRef = GetWorld()->GetAuthGameMode<ARunnerLevelGM>();
		GameModeRef->SegmentTrackIndexIncreasedDelegate.AddDynamic(this, &ABaseLaneSegment::IncreaseTrackIndex);
		LaneManagerRef = GameModeRef->LaneManager;
		//StartingLocation = GetActorLocation();
		/*auto GameModeRef = GetWorld()->GetAuthGameMode<ARunnerLevelGM>();
		SpawnLaneDelegate = GameModeRef->SpawnLaneSegmentDelegate;
		SpawnedNewActor = false;*/
	}
	else {

	}
}

void ABaseLaneSegment::MoveLane(FVector MoveAmount)
{
	AddActorWorldOffset(MoveAmount);
}

void ABaseLaneSegment::SetSpeed(float NewSpeed)
{
	SegmentSpeed = NewSpeed;
}

void ABaseLaneSegment::SetLaneIndex(int32 NewIndex)
{
	LaneIndex = NewIndex;
}



void ABaseLaneSegment::SetTrackIndex(int32 NewIndex)
{
	TrackIndex = NewIndex;
}

void ABaseLaneSegment::IncreaseTrackIndex()
{
	//UE_LOG(LogTemp, Log, TEXT("Increased Track Index"));
	TrackIndex++;
}

int32 ABaseLaneSegment::GetLaneIndex()
{
	return LaneIndex;
}

void ABaseLaneSegment::SetSpawnPoint(FVector SpawnPoint)
{
	StartingLocation = SpawnPoint;
}

float ABaseLaneSegment::GetSegmentLength() const
{
	return SegmentLength;
}
