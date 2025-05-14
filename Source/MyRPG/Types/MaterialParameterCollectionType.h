#pragma once

UENUM(BlueprintType)
enum class EMaterialParameterCollectionType : uint8
{
	EMPC_None			UMETA(DisplayName = "None"),
	EMPC_PostProcess	UMETA(DisplayName = "PostProcess")
};