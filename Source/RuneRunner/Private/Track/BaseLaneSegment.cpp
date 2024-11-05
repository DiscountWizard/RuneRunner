// Fill out your copyright notice in the Description page of Project Settings.

#include "Track/BaseLaneSegment.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "GameModes/RunnerLevelGM.h"

// Sets default values
ABaseLaneSegment::ABaseLaneSegment()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Root Arrow"));
	SetRootComponent(RootPoint);

	SegmentCollisionBox = CreateDefaultSubobject<UBoxComponent>(FName("Segment Collision Box"));
	SegmentCollisionBox->SetupAttachment(GetRootComponent(), FName(TEXT("Collision Box")));
	SegmentCollisionBox->SetBoxExtent(FVector(SegmentLength, SegmentWidth, 1.0f));
	SegmentCollisionBox->bHiddenInGame = false;
	/*SegmentCollisionBox->SetGenerateOverlapEvents(true);
	SegmentCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndProbe);
	SegmentCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap);*/

	BackAttachPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Back Attach Arrow"));
	BackAttachPoint->SetupAttachment(GetRootComponent(), FName(TEXT("Back Attach Point")));
	BackAttachPoint->SetHiddenInGame(false);
	BackAttachPoint->SetVisibility(true);
}

// Called when the game starts or when spawned
void ABaseLaneSegment::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseLaneSegment::OnConstruction(const FTransform& Transform)
{
	SegmentCollisionBox->SetBoxExtent(FVector(SegmentLength, SegmentWidth, 1.0f));

	BackAttachPoint->SetRelativeLocation(FVector(-SegmentLength, 0.0f, 0.0f));

}

// Called every frame
void ABaseLaneSegment::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	double DistanceFromStart = FVector::Distance(GetActorLocation(), StartingLocation);
	UE_LOG(LogTemp, Log, TEXT("Lane Index: %i, Distance From Origin: %f"), LaneIndex, DistanceFromStart);
	if (!SpawnedNewActor && DistanceFromStart > SegmentLength * 2.0f)
	{
		SpawnLaneDelegate.Broadcast(this);
		SpawnedNewActor = true;
	}
	AddActorWorldOffset(FVector(SegmentSpeed * DeltaTime, 0.f, 0.f), true);

}

void ABaseLaneSegment::SetPoolActorInUse(bool NewValue)
{
	Super::SetPoolActorInUse(NewValue);

	// Put logic that would normally go in BeginPlay() here. 
	if (NewValue)
	{
		//StartingLocation = GetActorLocation();
		auto GameModeRef = GetWorld()->GetAuthGameMode<ARunnerLevelGM>();
		SpawnLaneDelegate = GameModeRef->SpawnLaneSegmentDelegate;
		SpawnedNewActor = false;
	}
	else {

	}
}

void ABaseLaneSegment::SetSpeed(float NewSpeed)
{
	SegmentSpeed = NewSpeed;
}

void ABaseLaneSegment::SetLaneIndex(int32 NewIndex)
{
	LaneIndex = NewIndex;
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

FVector ABaseLaneSegment::GetBackAttachPointLocationInWorldSpace()
{
	return BackAttachPoint->GetComponentLocation();
}

