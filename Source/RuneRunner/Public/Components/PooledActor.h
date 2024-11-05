// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "PooledActor.generated.h"


UCLASS()
class RUNERUNNER_API APooledActor : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	APooledActor();

	UFUNCTION(BlueprintCallable)
	virtual void SetPoolActorInUse(bool NewValue);

	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(BlueprintReadWrite, Category = "Pooled Actor")
	bool InUse = false;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


	UPROPERTY(BlueprintReadOnly, Category = "Pooled Actor")
	float LifeTime = 600.0f;

	UFUNCTION()
	void ReturnToPool();

private:
	FTimerDelegate TimerDelegate;
	FTimerHandle TimerHandle;
		
};
