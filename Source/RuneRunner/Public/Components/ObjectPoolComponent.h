// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObjectPoolComponent.generated.h"

class APooledActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RUNERUNNER_API UObjectPoolComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UObjectPoolComponent();

	UFUNCTION()
	void InitializePool();

	virtual APooledActor* FindFirstAvailableActor();

	virtual APooledActor* SpawnFromPool(FTransform SpawnTransform);

	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "Object Pool")
	TSoftClassPtr<APooledActor> PooledActorClass;

	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "Object Pool")
	int PoolSize;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	

	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "Object Pool")
	TArray<APooledActor*> ObjectPool;


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
