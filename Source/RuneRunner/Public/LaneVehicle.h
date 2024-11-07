// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/TimelineComponent.h"
#include "Vehicles/VehicleStates.h"
#include "Track/LaneTypes.h"
#include "Kismet/GameplayStatics.h"
#include "LaneVehicle.generated.h"

class ARunnerLevelGM;
class ALaneManager;

UCLASS()
class RUNERUNNER_API ALaneVehicle : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ALaneVehicle();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE EVehicleState GetVehicleState() const { return CurrentVehicleState; }

	UFUNCTION(BlueprintImplementableEvent)
	void SideChanged();

	UFUNCTION(BlueprintImplementableEvent)
	void JumpStateChanged();

	UFUNCTION(BlueprintCallable)
	void JumpEnded();

	UFUNCTION(BlueprintImplementableEvent)
	void JumpEvent();

	UFUNCTION(BlueprintImplementableEvent)
	void LaneChanged(FVector NewVehicleLocation);

	UFUNCTION(BlueprintCallable)
	void LaneChangeComplete();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* VehicleMesh;

	UPROPERTY(BlueprintReadWrite, Category = "Vehicle Information")
	int LaneIndex = 0;

	TEnumAsByte<ECollisionChannel> TraceChannelProperty = ECollisionChannel::ECC_GameTraceChannel2;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Collision")
	ETrackType CurrentlyOnTrackType = ETrackType::ETT_Standard;

	void MoveDirection(int LaneChangeDir);

	UFUNCTION()
	bool MoveToLane(int NewLaneIndex);

	UFUNCTION()
	void VehicleJump();

	UFUNCTION()
	void RotateVehicleToSide(int IndexChange);

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Vehicle Information")
	EVehicleState CurrentVehicleState = EVehicleState::EVS_NotActive;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Vehicle Information")
	int SideUpIndex = 0; 

	ALaneManager* LaneManagerRef;
	int VehicleSidesMax = 4;

	int VehicleHealth = 5;
	bool LaneEffectApplied = false;
	FGuid LaneSegmentIdentifier;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Vehicle Information")
	float LaneChangeTime = 0.5f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Vehicle Information")
	EVehicleInputState VehicleInputState = EVehicleInputState::EVIS_Available;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Vehicle Information")
	float JumpHangTime = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Vehicle Information")
	float JumpHeight = 100.0f;
private:

	ARunnerLevelGM* GameModeRef;

	int MaximumLanes = 1;
	FVector TargetLocation;

	

	
	


};
