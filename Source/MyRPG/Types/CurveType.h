#pragma once

UENUM(BlueprintType)
enum class ECurveFloat : uint8
{
	ECF_None			UMETA(DisplayName = "None"),
	ECF_Default			UMETA(DisplayName = "Default"),
	ECF_EaseIn			UMETA(DisplayName = "EaseIn"),
	ECF_EaseOut			UMETA(DisplayName = "EaseOut"),
	ECF_EaseInOut		UMETA(DisplayName = "EaseInOut"),
	ECF_EaseOutIn		UMETA(DisplayName = "EaseOutIn"),
	ECF_Sine			UMETA(DisplayName = "Sine"),
	ECF_Cosine			UMETA(DisplayName = "Cosine"),

	ECF_EaseInSlowly	UMETA(DisplayName = "EaseInSlowly"),

	ECF_MAX				UMETA(DisplayName = "Default MAX")
};