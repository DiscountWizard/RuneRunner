#pragma once

UENUM(BlueprintType)
enum class ETrackType : uint8
{
	ETT_Standard UMETA(DisplayName = "Standard"),
	ETT_Gap UMETA(DisplayName = "Gap"),
	ETT_Rough UMETA(DisplayName = "Rough"),
	ETT_Wall UMETA(DisplayName = "Wall")
};