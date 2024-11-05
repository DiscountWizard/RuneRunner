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

	UFUNCTION(BlueprintImplementableEvent)
	void LaneChanged(FVector NewVehicleLocation);


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

	UFUNCTION()
	void MoveToLane(int NewLaneIndex);

	UFUNCTION()
	void VehicleJump();

	UFUNCTION()
	void RotateVehicleToSide(int IndexChange);

	// Timeline Stuff
	UPROPERTY()
	UTimelineComponent* MoveLaneTimeline;

	UPROPERTY()
	UTimelineComponent* JumpTimeline;

	UPROPERTY()
	UCurveFloat* FloatCurve_LaneChange;

	UPROPERTY()
	UCurveFloat* FloatCurve_Jump;

	UFUNCTION()
	void TimelineCallback(float val);

	UFUNCTION()
	void TimelineFinishedCallback();

	UPROPERTY()
	TEnumAsByte<ETimelineDirection::Type> TimelineDirection;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Vehicle Information")
	EVehicleState CurrentVehicleState = EVehicleState::EVS_NotActive;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Vehicle Information")
	int SideUpIndex = 0; 

	ALaneManager* LaneManagerRef;
	int VehicleSidesMax = 4;
private:

	ARunnerLevelGM* GameModeRef;

	int MaximumLanes = 1;
	FVector TargetLocation;

	float JumpHangTime = 0.0f;

	
	


};
