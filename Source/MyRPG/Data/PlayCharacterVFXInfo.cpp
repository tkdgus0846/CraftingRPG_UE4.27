// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayCharacterVFXInfo.h"
#include "MyRPG/Effect/MaterialParameterAnimator.h"
#include "Curves/CurveLinearColor.h"

FMaterialParameterAnimationInfo::FMaterialParameterAnimationInfo(UMaterialInstanceDynamic* _Instance, const FPlayMaterialParameterAnimationInfo& PlayInfo) :
	Instance(_Instance),
	ParameterName(PlayInfo.ParameterName),
	Duration(PlayInfo.Duration),
	ScaleValue(PlayInfo.ScaleValue),
	bPlayReverse(PlayInfo.bPlayReverse),
	ValueOffset(PlayInfo.ValueOffset),
	ParameterMode(PlayInfo.ParameterMode),
	bValueToDefaultWhenEndVFX(PlayInfo.bValueToDefaultWhenEndVFX),
	DefaultScalarValue(PlayInfo.DefaultScalarValue),
	DefaultColorValue(PlayInfo.DefaultColorValue)
{
	if (ParameterMode == ECurveValueType::ECVT_LinearColor)
		CurveLinearColor = Cast<UCurveLinearColor>(PlayInfo.Curve);
	else if (ParameterMode == ECurveValueType::ECVT_Scalar)
		CurveFloat = Cast<UCurveFloat>(PlayInfo.Curve);
}

void FMaterialParameterAnimatorMap::Add(const FName& Name, AMaterialParameterAnimator* Animator)
{
	TWeakObjectPtr<AMaterialParameterAnimator>* Find = AnimatorMap.Find(Name);
	if (Find == nullptr)
	{
		AnimatorMap.Add(Name, Animator);
	}
	else
	{
		if (Find->IsValid() == false)
		{
			AnimatorMap.Add(Name, Animator);
		}
	}
}

void FMaterialParameterAnimatorMap::Clear(const FName& Name)
{
	TWeakObjectPtr<AMaterialParameterAnimator>* Find = AnimatorMap.Find(Name);
	if (Find == nullptr) return;

	if (Find->IsValid())
		(*Find)->ClearToDefaultValue();
}

void FMaterialParameterAnimatorMap::ClearAll()
{
	for (auto& Item : AnimatorMap)
	{
		if (Item.Value.IsValid())
			Item.Value->ClearToDefaultValue();
	}
}

TWeakObjectPtr<AMaterialParameterAnimator> FMaterialParameterAnimatorMap::GetAnimator(const FName& Name)
{
	TWeakObjectPtr<class AMaterialParameterAnimator>* Find = AnimatorMap.Find(Name);
	if (Find == nullptr)
		return nullptr;
	else
		return *Find;
}


