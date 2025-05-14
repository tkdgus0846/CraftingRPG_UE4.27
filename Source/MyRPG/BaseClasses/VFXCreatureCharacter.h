// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CreatureCharacter.h"
#include "MyRPG/Data/PlayCharacterVFXInfo.h"
#include "MyRPG/Types/StateMaterialType.h"
#include "VFXCreatureCharacter.generated.h"

class USkeletalMeshComponent;
class UMaterialInstanceDynamic;

UCLASS()
class MYRPG_API AVFXCreatureCharacter : public ACreatureCharacter
{
	GENERATED_BODY()

public:
	AVFXCreatureCharacter();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

	// 머터리얼 파라미터 설정하는것은 일괄버젼, 개별버젼 이렇게 두개 준비하면 될거같다.
	UFUNCTION(BlueprintCallable)
	void FXMesh_SetVectorParameter(const FName ParamName, const FVector ParameterValue);

	UFUNCTION(BlueprintCallable)
	void FXMesh_SetScalarParameter(const FName ParamName, const float ParameterValue);

	UFUNCTION(BlueprintCallable)
	void PlayVFX(const FName& Name, bool bIsOriginMeshVFX, const TArray<FPlayMaterialParameterAnimationInfo>& Param);

	UFUNCTION(BlueprintCallable)
	void EndVFX(const FName& Name, bool bIsOriginMeshVFX);

	/*
		
	*/
	UFUNCTION(BlueprintCallable)
	void ChangeState(EStateMaterialType State, float Duration);

protected:
	/*
		bOriginMesh: 만약 VFXMesh면 false
		bSetMasterPose: 소켓에 Attachment 한게 아닐경우 true, 그러니까 애니메이션을 따라 움직이게 만들려면.
	*/
	UFUNCTION(BlueprintCallable)
	void RegisterMesh(USkeletalMeshComponent* MeshComp, bool bOriginMesh, bool bSetMasterPose);

	UFUNCTION(BlueprintCallable)
	void RemoveMesh(USkeletalMeshComponent* MeshComp, bool bOriginMesh);

	UFUNCTION(BlueprintCallable)
	void RemoveMeshByIndex(int32 MeshIndex, bool bOriginMesh);

	UPROPERTY(EditDefaultsOnly)
	class USkeletalMeshComponent* VFXMeshComponent;

private:
	void PlayVFX_ByMesh(const FName& Name, bool bIsOriginMesh, const TArray<FPlayMaterialParameterAnimationInfo>& Param, USkeletalMeshComponent* Mesh);
	void EndVFX_ByMesh(const FName& Name, bool bIsOriginMesh, USkeletalMeshComponent* Mesh);

	void SetOriginMeshes_Visibility(bool bVisibility);
	void SetFXMeshes_Visibility(bool bVisibility);

	UPROPERTY()
	TArray<USkeletalMeshComponent*> OriginMeshComponents;
	UPROPERTY()
	TArray<USkeletalMeshComponent*> FXMeshComponents;

	UPROPERTY(VisibleAnywhere)
	class UNiagaraComponent* FXComponent;

	USkeletalMeshComponent* FindMesh(int32 MeshIndex, bool bOriginMesh);


	/*
		상태 이상 효과에 대한 데미지나, VFX Material Animation을 하는 역할.
	*/
	UPROPERTY(EditDefaultsOnly)
	class UStateComponent* StateComponent;

public:

	
};
