// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayCharacterVFX.h"
#include <MyRPG/BaseClasses/VFXCreatureCharacter.h>
#include "Curves/CurveLinearColor.h"

void UPlayCharacterVFX::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	// 변수초기화
	//InfoArray.Empty();
	TArray<FPlayMaterialParameterAnimationInfo> InfoArray;
	AccTime = 0.f;

	//Material = (bIsOriginMeshVFX) ? nullptr : Material;

	NotifyTotalDuration = TotalDuration;
	float Duration = TotalDuration + ExtendTime;

	for (FPlayCharacterVFXInfo_InNotify& Item : VFXInfo)
	{
		float ResultDuration = Item.PercentPerDuration * Duration;

		InfoArray.Add(
			MoveTempIfPossible
			(
				FPlayMaterialParameterAnimationInfo
				(
					Item.ParameterName,
					Item.ParameterMode,
					ResultDuration,
					Item.Curve,
					Item.bValueToDefaultWhenEndVFX,
					Item.DefaultScalarValue,
					Item.DefaultColorValue,
					Item.MaterialIndex,
					Item.ScaleValue,
					Item.bPlayReverse,
					Item.ValueOffset,
					Material
				)
			)
		);
	}

	AVFXCreatureCharacter* Character = Cast<AVFXCreatureCharacter>(MeshComp->GetOwner());
	if (IsValid(Character) == false) return;

	Character->PlayVFX(NotifyName, Mode == ESelectVFXMesh::ESVM_OriginMesh, InfoArray);
}

void UPlayCharacterVFX::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

	AccTime += FrameDeltaTime;
}

void UPlayCharacterVFX::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	AVFXCreatureCharacter* Character = Cast<AVFXCreatureCharacter>(MeshComp->GetOwner());
	if (IsValid(Character) == false) return;

	bool bNotifyFinishedCompletely = FMath::IsNearlyEqual(AccTime, NotifyTotalDuration, 0.05f);

	if (bNotifyFinishedCompletely)
	{
		if (ExtendTime > 0.f)
		{
			// Set Timer => Character->EndVFX
			FTimerDelegate TimerDel;
			TimerDel.BindUFunction(Character, "EndVFX", NotifyName, Mode == ESelectVFXMesh::ESVM_OriginMesh);
			FTimerHandle Timer;
			Character->GetWorld()->GetTimerManager().SetTimer(Timer, TimerDel, ExtendTime, false);
			
			return;
		}
	}
	Character->EndVFX(NotifyName, Mode == ESelectVFXMesh::ESVM_OriginMesh);
}

FString UPlayCharacterVFX::GetNotifyName_Implementation() const
{
	return NotifyName.ToString();
}

FLinearColor UPlayCharacterVFX::GetEditorColor()
{
	for (auto& Item : VFXInfo)
	{
		Item.bIsOriginMeshVFX = (Mode == ESelectVFXMesh::ESVM_OriginMesh);

		UCurveFloat* CurveFloat = Cast<UCurveFloat>(Item.Curve);
		UCurveLinearColor* CurveLinearColor = Cast<UCurveLinearColor>(Item.Curve);
		if (CurveFloat)
		{
			Item.ParameterMode = ECurveValueType::ECVT_Scalar;
		}
		else if (CurveLinearColor)
		{
			Item.ParameterMode = ECurveValueType::ECVT_LinearColor;
		}
		else
		{
			Item.ParameterMode = ECurveValueType::ECVT_None;
		}
	}

	// bIsOriginMeshVFX == true 면 개별 체크 가능하게 false면 밖에 하나
	if ((Mode == ESelectVFXMesh::ESVM_OriginMesh) == false)
	{
		for (auto& Item : VFXInfo)
		{
			Item.bValueToDefaultWhenEndVFX = bValueToDefaultWhenEndVFX;
		}
	}
	
	Material = (Mode == ESelectVFXMesh::ESVM_OriginMesh) ? nullptr : Material;
	return (Mode == ESelectVFXMesh::ESVM_OriginMesh) ? FLinearColor(FColor(87, 69, 253)) : FLinearColor(FColor(253, 69, 69));
}
