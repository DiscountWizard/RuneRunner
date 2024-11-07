// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerVehicle.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

APlayerVehicle::APlayerVehicle()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

}

void APlayerVehicle::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PlayerVehicleContext, 0);
		}
	}

	if (LaneManagerRef && LaneManagerRef->StaticCamera)
	{
		PlayerController->SetViewTargetWithBlend(LaneManagerRef->StaticCamera, 0.1f);
	}

	MoveToLane(1);
	SideUpIndex = 0;
	SideChanged();
}

void APlayerVehicle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerVehicle::MoveLeft(const FInputActionValue& Value)
{
	MoveDirection(-1);
}

void APlayerVehicle::MoveRight(const FInputActionValue& Value)
{
	MoveDirection(1);
}

void APlayerVehicle::Jump(const FInputActionValue& Value)
{
	VehicleJump();
}

void APlayerVehicle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveLeftAction, ETriggerEvent::Triggered, this, &APlayerVehicle::MoveLeft);
		EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &APlayerVehicle::MoveRight);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &APlayerVehicle::Jump);
	}
}

