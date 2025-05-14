#pragma once

UENUM(BlueprintType)
enum class ERSkillType : uint8
{
	ERT_None UMETA(DisplayName = "None"),

	ERT_IceQuake UMETA(DisplayName = "IceQuake"),
	
	ERT_MAX UMETA(DisplayName = "Default MAX")
};

UENUM(BlueprintType)
enum class ECSkillType : uint8
{
	ECT_None UMETA(DisplayName = "None"),

	ECT_Barrier UMETA(DisplayName = "Barrier"),

	ECT_MAX UMETA(DisplayName = "Default MAX")
};

UENUM(BlueprintType)
enum class ETSkillType : uint8
{
	ETT_None UMETA(DisplayName = "None"),

	ETT_MAX UMETA(DisplayName = "Default MAX")
};

UENUM(BlueprintType)
enum class EDashSkillType : uint8
{
	EDT_None UMETA(DisplayName = "None"),

	EDT_BasicDash UMETA(DisplayName = "BasicDash"),
	EDT_HorseJump UMETA(DisplayName = "HorseJump"),

	EDT_MAX UMETA(DisplayName = "Default MAX")
};
