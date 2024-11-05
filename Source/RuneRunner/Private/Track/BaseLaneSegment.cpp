// Fill out your copyright notice in the Description page of Project Settings.

#include "Track/BaseLaneSegment.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
ABaseLaneSegment::ABaseLaneSegment()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SegmentCollisionBox = CreateDefaultSubobject<UBoxComponent>(FName("Segment Collision Box"));
	SegmentCollisionBox->SetBoxExtent(FVector(SegmentLength, SegmentWidth, 1.0f));
	SegmentCollisionBox->bHiddenInGame = false;
	SegmentCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);
	SetRootComponent(SegmentCollisionBox);

	BackAttachPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("RootArrowComponent"));
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

	AddActorWorldOffset(FVector(SegmentSpeed * DeltaTime, 0.f, 0.f));

}

void ABaseLaneSegment::SetPoolActorInUse(bool NewValue)
{
	Super::SetPoolActorInUse(NewValue);

	// Put logic that would normally go in BeginPlay() here. 
	if (NewValue)
	{

	}
	else {

	}
}

void ABaseLaneSegment::SetSpeed(float NewSpeed)
{
	SegmentSpeed = NewSpeed;
}

float ABaseLaneSegment::GetSegmentLength() const
{
	return SegmentLength;
}

FVector ABaseLaneSegment::GetBackAttachPointLocationInWorldSpace()
{
	return BackAttachPoint->GetComponentLocation();
}

