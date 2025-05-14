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

	// ���͸��� �Ķ���� �����ϴ°��� �ϰ�����, �������� �̷��� �ΰ� �غ��ϸ� �ɰŰ���.
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
		bOriginMesh: ���� VFXMesh�� false
		bSetMasterPose: ���Ͽ� Attachment �Ѱ� �ƴҰ�� true, �׷��ϱ� �ִϸ��̼��� ���� �����̰� �������.
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
		���� �̻� ȿ���� ���� ��������, VFX Material Animation�� �ϴ� ����.
	*/
	UPROPERTY(EditDefaultsOnly)
	class UStateComponent* StateComponent;

public:

	
};
