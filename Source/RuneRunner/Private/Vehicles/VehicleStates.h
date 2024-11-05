#pragma once

UENUM(BlueprintType)
enum class EVehicleState : uint8
{
	EVS_InLane UMETA(DisplayName = "In Lane"),
	EVS_ChangingLane UMETA(DisplayName = "Changing Lanes"),
	EVS_Jumping UMETA(DisplayName = "Jumping"),
	EVS_NotActive UMETA(DisplayName = "Not Active")
};
