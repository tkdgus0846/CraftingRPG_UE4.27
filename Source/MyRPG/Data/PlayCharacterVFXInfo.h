// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyRPG/Types/CurveType.h"
#include "MyRPG/Types/CurveValueType.h"
#include "PlayCharacterVFXInfo.generated.h"

USTRUCT(BlueprintType)
struct FPlayCharacterVFXInfo_InNotify
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite , Meta = (ToolTip = "if Value == -1 , All Materials In Mesh Will be Applied"))
	int MaterialIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (EditCondition = "bIsOriginMeshVFX", EditConditionHides))
	bool bValueToDefaultWhenEndVFX = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (EditCondition = "bValueToDefaultWhenEndVFX && ParameterMode == ECurveValueType::ECVT_Scalar", EditConditionHides))
	float DefaultScalarValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (EditCondition = "bValueToDefaultWhenEndVFX && ParameterMode == ECurveValueType::ECVT_LinearColor", EditConditionHides))
	FLinearColor DefaultColorValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ParameterName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PercentPerDuration = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPlayReverse = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayThumbnail = true))
	class UCurveBase* Curve = nullptr;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (EditCondition = "ParameterMode == ECurveValueType::ECVT_Scalar", EditConditionHides))
	float ScaleValue = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (EditCondition = "ParameterMode == ECurveValueType::ECVT_Scalar", EditConditionHides))
	float ValueOffset = 0.f;

	UPROPERTY()
	bool bIsOriginMeshVFX;
	UPROPERTY()
	ECurveValueType ParameterMode = ECurveValueType::ECVT_None;
};

//USTRUCT(BlueprintType)
//struct FPlayCharacterVFXInfo_InVFXCharacter
//{
//	GENERATED_BODY()
//
//	FPlayCharacterVFXInfo_InVFXCharacter() {}
//	FPlayCharacterVFXInfo_InVFXCharacter(class UMaterialInterface* _Material, int _MaterialIndex, bool _bValueToDefaultWhenEndVFX, float _DefaultScalarValue, const FLinearColor _DefaultColorValue, FName _ParameterName, float _Duration, UCurveBase* _Curve, float _ScaleValue, bool _bPlayReverse, float _ValueOffset, ECurveValueType _ParameterMode) :
//		Material(_Material),
//		MaterialIndex(_MaterialIndex),
//		bValueToDefaultWhenEndVFX(_bValueToDefaultWhenEndVFX),
//		DefaultScalarValue(_DefaultScalarValue),
//		DefaultColorValue(_DefaultColorValue),
//		ParameterName(_ParameterName),
//		Duration(_Duration),
//		Curve(_Curve),
//		ScaleValue(_ScaleValue),
//		bPlayReverse(_bPlayReverse),
//		ValueOffset(_ValueOffset),
//		ParameterMode(_ParameterMode)
//	{}
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	class UMaterialInterface* Material = nullptr;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	int MaterialIndex;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	bool bValueToDefaultWhenEndVFX;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	float DefaultScalarValue;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	FLinearColor DefaultColorValue;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	FName ParameterName;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	float Duration;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	class UCurveBase* Curve;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	float ScaleValue = 1.f;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	bool bPlayReverse = false;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	float ValueOffset = 0.f;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	ECurveValueType ParameterMode = ECurveValueType::ECVT_None;
//};

/*
		이 아래 두개 구조체는 다른 헤더파일로 옮기자.
*/
USTRUCT(BlueprintType)
struct FPlayMaterialParameterAnimationInfo
{
	GENERATED_BODY()

	FPlayMaterialParameterAnimationInfo() {}
	FPlayMaterialParameterAnimationInfo(FName _ParameterName, ECurveValueType _ParameterMode, float _Duration, class UCurveBase* _Curve, bool _bValueToDefaultWhenEndVFX, float _DefaultScalarValue, const FLinearColor _DefaultColorValue, int _MaterialIndex = 0, float _ScaleValue = 1.f, bool _bPlayReverse = false, float _ValueOffset = 0.f, class UMaterialInterface* _Material = nullptr) :
		ParameterName(_ParameterName),
		ParameterMode(_ParameterMode),
		Duration(_Duration),
		Curve(_Curve),
		bValueToDefaultWhenEndVFX(_bValueToDefaultWhenEndVFX),
		DefaultScalarValue(_DefaultScalarValue),
		DefaultColorValue(_DefaultColorValue),
		MaterialIndex(_MaterialIndex),
		ScaleValue(_ScaleValue),
		bPlayReverse(_bPlayReverse),
		ValueOffset(_ValueOffset),
		Material(_Material)
	{}
	FPlayMaterialParameterAnimationInfo(FName _ParameterName, ECurveValueType _ParameterMode, float _Duration, class UCurveBase* _Curve, int _MaterialIndex = 0, float _ScaleValue = 1.f, bool _bPlayReverse = false, float _ValueOffset = 0.f, class UMaterialInterface* _Material = nullptr) :
		ParameterName(_ParameterName),
		ParameterMode(_ParameterMode),
		Duration(_Duration),
		Curve(_Curve),
		bValueToDefaultWhenEndVFX(false),
		DefaultScalarValue(0.f),
		DefaultColorValue(FLinearColor::Black),
		MaterialIndex(_MaterialIndex),
		ScaleValue(_ScaleValue),
		bPlayReverse(_bPlayReverse),
		ValueOffset(_ValueOffset),
		Material(_Material)
	{}
	FPlayMaterialParameterAnimationInfo(const FPlayMaterialParameterAnimationInfo& Other) :
		ParameterName(Other.ParameterName),
		ParameterMode(Other.ParameterMode),
		Duration(Other.Duration),
		Curve(Other.Curve),
		bValueToDefaultWhenEndVFX(Other.bValueToDefaultWhenEndVFX),
		DefaultScalarValue(Other.DefaultScalarValue),
		DefaultColorValue(Other.DefaultColorValue),
		MaterialIndex(Other.MaterialIndex),
		ScaleValue(Other.ScaleValue),
		bPlayReverse(Other.bPlayReverse),
		ValueOffset(Other.ValueOffset),
		Material(Other.Material)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ParameterName; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECurveValueType ParameterMode; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCurveBase* Curve = nullptr; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bValueToDefaultWhenEndVFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DefaultScalarValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor DefaultColorValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaterialIndex = 0; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ScaleValue = 1.f; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPlayReverse = false; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ValueOffset = 0.f; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMaterialInterface* Material = nullptr;
};

USTRUCT()
struct FMaterialParameterAnimationInfo
{
	GENERATED_BODY()

	FMaterialParameterAnimationInfo() {}
	FMaterialParameterAnimationInfo(class UMaterialInstanceDynamic* _Instance, FName _ParameterName, float _Duration, float _ScaleValue, bool _bPlayReverse, float _ValueOffset, class UCurveFloat* _CurveFloat, class UCurveLinearColor* _CurveLinearColor, ECurveValueType _ParameterMode, bool _bValueToDefaultWhenEndVFX, float _DefaultScalarValue, const FLinearColor _DefaultColorValue) :
		Instance(_Instance),
		ParameterName(_ParameterName),
		Duration(_Duration),
		ScaleValue(_ScaleValue),
		bPlayReverse(_bPlayReverse),
		ValueOffset(_ValueOffset),
		CurveFloat(_CurveFloat),
		CurveLinearColor(_CurveLinearColor),
		ParameterMode(_ParameterMode),
		bValueToDefaultWhenEndVFX(_bValueToDefaultWhenEndVFX),
		DefaultScalarValue(_DefaultScalarValue),
		DefaultColorValue(_DefaultColorValue)
	{
	}

	FMaterialParameterAnimationInfo(class UMaterialInstanceDynamic* _Instance, const FPlayMaterialParameterAnimationInfo& PlayInfo); 

	UPROPERTY()
	class UMaterialInstanceDynamic* Instance = nullptr;
	UPROPERTY()
	FName ParameterName;
	UPROPERTY()
	float Duration;
	UPROPERTY()
	float ScaleValue = 1.f;
	UPROPERTY()
	bool bPlayReverse = false;
	UPROPERTY()
	float ValueOffset = 0.f;                        

	/**/
	UPROPERTY()
	class UCurveFloat* CurveFloat = nullptr;
	UPROPERTY()
	class UCurveLinearColor* CurveLinearColor = nullptr;
	UPROPERTY()
	ECurveValueType ParameterMode = ECurveValueType::ECVT_None;

	UPROPERTY()
	bool bValueToDefaultWhenEndVFX;
	UPROPERTY()
	float DefaultScalarValue;
	UPROPERTY()
	FLinearColor DefaultColorValue;
};

USTRUCT()
struct FMaterialParameterAnimatorMap
{
	GENERATED_BODY()

public:
	FMaterialParameterAnimatorMap() {}
	FMaterialParameterAnimatorMap(const FName& Name, class AMaterialParameterAnimator* Animator)
	{
		Add(Name, Animator);
	}

	void Add(const FName& Name, class AMaterialParameterAnimator* Animator);
	void Clear(const FName& Name);
	void ClearAll();

	TWeakObjectPtr<class AMaterialParameterAnimator> GetAnimator(const FName& Name);
	

private:
	UPROPERTY()
	TMap<FName, TWeakObjectPtr<class AMaterialParameterAnimator>> AnimatorMap;
};