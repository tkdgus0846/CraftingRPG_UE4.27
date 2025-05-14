#pragma once

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	EIR_Normal UMETA(DisplayName = "Normal"),
	EIR_Rare UMETA(DisplayName = "Rare"),
	EIR_Legendary UMETA(DisplayName = "Legendary"),

	EIR_MAX UMETA(DisplayName = "Default MAX"),
};