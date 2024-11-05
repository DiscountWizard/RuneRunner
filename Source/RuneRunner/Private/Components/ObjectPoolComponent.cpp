// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/ObjectPoolComponent.h"
#include "Components/PooledActor.h"

// Sets default values for this component's properties
UObjectPoolComponent::UObjectPoolComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UObjectPoolComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializePool();
	// ...

}

void UObjectPoolComponent::InitializePool()
{
	FActorSpawnParameters SpawnInfo;
	UClass* ActorClass = PooledActorClass.LoadSynchronous();

	for (int i = 0; i < PoolSize; i++)
	{
		FVector OffsetSpawn = GetOwner()->GetActorLocation();
		OffsetSpawn.X -= 2000;
		APooledActor* NewObject = GetWorld()->SpawnActor<APooledActor>(ActorClass, GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation(), SpawnInfo);
		ObjectPool.Add(NewObject);
	}

}

APooledActor* UObjectPoolComponent::FindFirstAvailableActor()
{
	for (auto actor : ObjectPool)
	{
		if (!actor->InUse)
		{
			return actor;
		}
	}
	return nullptr;
}

APooledActor* UObjectPoolComponent::SpawnFromPool(FTransform SpawnTransform)
{
	APooledActor* FoundActor = FindFirstAvailableActor();
	if (FoundActor)
	{
		FoundActor->SetActorTransform(SpawnTransform);
		FoundActor->SetPoolActorInUse(true);
		return FoundActor;
	}

	return nullptr;
}

// Called every frame
void UObjectPoolComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

