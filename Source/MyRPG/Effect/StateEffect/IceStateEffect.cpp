
#include "IceStateEffect.h"
#include "MyRPG/GameInstance/MyGameInstance.h"
#include "MyRPG/BaseClasses/VFXCreatureCharacter.h"
#include "MyRPG/Effect/ImpactEffect.h"

void AIceStateEffect::StartState()
{
	if (IsValid(Character) == false) return;
	if (IsValid(GameInstance) == false) return;

	TArray<FPlayMaterialParameterAnimationInfo> AnimationArray;
	AnimationArray.Add
	(
		FPlayMaterialParameterAnimationInfo
		(
			"FresnelExponent",
			ECurveValueType::ECVT_Scalar,
			StateDuration,
			GameInstance->GetCurveFloat(ECurveFloat::ECF_Default),
			-1,
			5.f,
			false,
			0.f,
			StateMaterial
		)
	);

	Character->PlayVFX(StateName, false, AnimationArray);
	ReserveEndMaterialAnim(StateDuration);
}