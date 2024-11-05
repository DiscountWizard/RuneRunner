// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LaneVehicle.h"
#include "InputActionValue.h"
#include "Track/LaneManager.h"
#include "PlayerVehicle.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class RUNERUNNER_API APlayerVehicle : public ALaneVehicle
{
	GENERATED_BODY()

public:
	APlayerVehicle();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* PlayerVehicleContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MoveLeftAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MoveRightAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* JumpAction;

	void MoveLeft(const FInputActionValue& Value);
	void MoveRight(const FInputActionValue& Value);
	void Jump(const FInputActionValue& Value);

};
