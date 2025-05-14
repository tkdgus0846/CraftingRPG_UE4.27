#pragma once

UENUM(BlueprintType)
enum class EPostProcessAnimationType : uint8
{
	EPPA_None			UMETA(DisplayName = "None"),
	EPPA_Blur			UMETA(DisplayName = "Blur")
};