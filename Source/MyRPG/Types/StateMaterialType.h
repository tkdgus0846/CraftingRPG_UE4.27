#pragma once

UENUM(BlueprintType)
enum class EStateMaterialType : uint8
{
	ESMT_None			UMETA(DisplayName = "None"),
	ESMT_Ice			UMETA(DisplayName = "IceState"),
	ESMT_Block			UMETA(DisplayName = "Block"),
	ESMT_Channeling		UMETA(DisplayName = "Channeling")
};