// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PooledActor.h"
#include "Track/LaneTypes.h"
#include "BaseLaneSegment.generated.h"

class UBoxComponent;
class UArrowComponent;

UCLASS()
class RUNERUNNER_API ABaseLaneSegment : public APooledActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties

	ABaseLaneSegment();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void SetPoolActorInUse(bool NewValue) override;

	void SetSpeed(float NewSpeed);
	float GetSegmentLength() const;
	FORCEINLINE ETrackType GetTrackType() { return LaneType; }

	FVector GetBackAttachPointLocationInWorldSpace();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(VisibleAnywhere, Category = "Lane Segment Config")
	UBoxComponent* SegmentCollisionBox;

	UPROPERTY(BlueprintReadOnly, Category = "Lane Segment Config")
	float SegmentSpeed = 1.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Lane Segment Config")
	float SegmentLength = 200.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Lane Segment Config")
	float SegmentWidth = 50.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Lane Segment Config")
	ETrackType LaneType = ETrackType::ETT_Standard;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Lane Information")
	UArrowComponent* BackAttachPoint;

private:


};
