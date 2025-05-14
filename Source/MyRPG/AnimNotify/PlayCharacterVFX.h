// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "MyRPG/Data/PlayCharacterVFXInfo.h"
#include "PlayCharacterVFX.generated.h"

UENUM(BlueprintType)
enum class ESelectVFXMesh : uint8
{
	ESVM_OriginMesh UMETA(DisplayName = "Mesh"),
	ESVM_VFXMesh UMETA(DisplayName = "Overlay")
};

UCLASS()
class MYRPG_API UPlayCharacterVFX : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	virtual FString GetNotifyName_Implementation() const override;
	virtual FLinearColor GetEditorColor() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	FName NotifyName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	float ExtendTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	ESelectVFXMesh Mode;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true, EditCondition = "Mode == ESelectVFXMesh::ESVM_VFXMesh", EditConditionHides))
	class UMaterialInterface* Material = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true, EditCondition = "Mode == ESelectVFXMesh::ESVM_VFXMesh", EditConditionHides))
	bool bValueToDefaultWhenEndVFX = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	TArray<FPlayCharacterVFXInfo_InNotify> VFXInfo;

	/*UPROPERTY()
	TArray<FPlayMaterialParameterAnimationInfo> InfoArray;*/

	float AccTime = 0.f;
	float NotifyTotalDuration;
	
};
