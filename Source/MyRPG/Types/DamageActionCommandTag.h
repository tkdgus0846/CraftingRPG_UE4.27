#pragma once

UENUM(BlueprintType)
enum class EDamageActionCommandTag : uint8
{
	EDACT_SpawnEffect	UMETA(DisplayName = "SpawnEffect"),
	EDACT_Damage		UMETA(DisplayName = "Damage"),
	EDACT_CrowdControl	UMETA(DisplayName = "CrowdControl"),
	EDACT_CameraShake	UMETA(DisplayName = "CameraShake"),
	EDACT_ChangeState	UMETA(DisplayName = "ChangeState"),

	EDACT_MAX			UMETA(DisplayName = "MAX")
};