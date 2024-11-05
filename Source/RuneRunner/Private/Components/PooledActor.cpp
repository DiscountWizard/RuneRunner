// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PooledActor.h"

// Sets default values for this component's properties
APooledActor::APooledActor()
{
	PrimaryActorTick.bCanEverTick = true;

	TimerDelegate.BindUFunction(this, FName("ReturnToPool"));

	// ...
}

void APooledActor::SetPoolActorInUse(bool NewValue)
{
	InUse = NewValue;
	SetActorEnableCollision(NewValue);
	SetActorHiddenInGame(!NewValue);
	SetActorTickEnabled(NewValue);

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);

	if (NewValue)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, LifeTime, true);
	}
}


// Called when the game starts
void APooledActor::BeginPlay()
{
	Super::BeginPlay();

	SetPoolActorInUse(false);

	// ...
	
}

void APooledActor::ReturnToPool()
{
	SetPoolActorInUse(false);
}


// Called every frame
void APooledActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ...
}

