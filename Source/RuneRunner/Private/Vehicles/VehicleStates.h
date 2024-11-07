#pragma once

UENUM(BlueprintType)
enum class EVehicleState : uint8
{
	EVS_InLane UMETA(DisplayName = "In Lane"),
	EVS_ChangingLane UMETA(DisplayName = "Changing Lanes"),
	EVS_Jumping UMETA(DisplayName = "Jumping"),
	EVS_NotActive UMETA(DisplayName = "Not Active")
};

UENUM(BlueprintType)
enum class EVehicleInputState : uint8
{
	EVIS_Available UMETA(DisplayName = "Available to process Input"),
	EVIS_Queued UMETA(DisplayName = "Input Queued"),
	EVIS_Processing UMETA(DisplayName = "Input Currently Processing"),
	EVIS_Locked UMETA(DisplayName = "Input Locked")
};
