#pragma once

UENUM(BlueprintType)
enum class ELevelType : uint8
{
	ELT_None UMETA(DisplayName = "None"),
	ELT_MainLevel UMETA(DisplayName = "MainLevel"),
	ELT_FieldLevel UMETA(DisplayName = "FieldLevel"),
	ELT_PaladinBossLevel UMETA(DisplayName = "PaladinBossLevel")
};