#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_None UMETA(DisplayName = "None"),

	EWT_Unarmed UMETA(DisplayName = "Unarmed"),
	EWT_Bow UMETA(DisplayName = "Bow"),
	EWT_Mace UMETA(DisplayName = "Mace"),
	EWT_Axe UMETA(DisplayName = "Axe"),
	EWT_Slasher UMETA(DisplayName = "Slasher"),
	EWT_Sword UMETA(DisplayName = "Sword"),
	EWT_Horse UMETA(DisplayName = "Horse"),

	EWT_MAX UMETA(DisplayName = "Default MAX")
};