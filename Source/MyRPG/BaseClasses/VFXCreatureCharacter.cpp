// Fill out your copyright notice in the Description page of Project Settings.


#include "VFXCreatureCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraComponent.h"
#include "Materials/MaterialInstance.h"
#include <MyRPG/GameInstance/MyGameInstance.h>
#include "MyRPG/BaseClasses/Components/StateComponent.h"
#include "Components/WidgetComponent.h"
#include "MyRPG/UI/StateWidget.h"
#include <MyRPG/UI/LevelHPStateWidget.h>

AVFXCreatureCharacter::AVFXCreatureCharacter()
{
	FXComponent = CreateDefaultSubobject<UNiagaraComponent>("NiagaraFX");
	if (FXComponent)
	{
		if (GetMesh())
		{
			FXComponent->SetupAttachment(GetMesh());
		}
	}

	VFXMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("FXMesh");
	if (VFXMeshComponent)
	{
		VFXMeshComponent->SetupAttachment(RootComponent);
	}

	StateComponent = CreateDefaultSubobject<UStateComponent>("StateComponent");
}

void AVFXCreatureCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	OriginMeshComponents.Empty();
	FXMeshComponents.Empty();

	if (IsValid(GetMesh()) && IsValid(VFXMeshComponent))
	{
		VFXMeshComponent->SetSkeletalMesh(GetMesh()->SkeletalMesh);

		RegisterMesh(GetMesh(), true, false);
		RegisterMesh(VFXMeshComponent, false, true);
	}
	
}

void AVFXCreatureCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetFXMeshes_Visibility(false);

	if (IsValid(StateComponent))
	{
		GetDeadDelegate().AddDynamic(StateComponent, &UStateComponent::ResetState);
	}

	UWidgetComponent* WidgetComponent = GetLevelHPStateComponent();
	if (WidgetComponent)
	{
		ULevelHPStateWidget* LevelHPStateWidget = Cast<ULevelHPStateWidget>(GetLevelHPStateComponent()->GetWidget());
		if (IsValid(LevelHPStateWidget))
		{
			StateComponent->StateChangedDelegate.AddDynamic(LevelHPStateWidget, &ULevelHPStateWidget::ChangeState);
			LevelHPStateWidget->StateWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	
}

void AVFXCreatureCharacter::FXMesh_SetVectorParameter(const FName ParamName, const FVector Vector)
{
	for (auto Item : FXMeshComponents)
	{
		if (IsValid(Item) && IsValid(Item->SkeletalMesh))
			Item->SetVectorParameterValueOnMaterials(ParamName, Vector);
	}
}

void AVFXCreatureCharacter::FXMesh_SetScalarParameter(const FName ParamName, const float ParameterValue)
{
	for (auto Item : FXMeshComponents)
	{
		if (IsValid(Item) && IsValid(Item->SkeletalMesh))
			Item->SetScalarParameterValueOnMaterials(ParamName, ParameterValue);
	}
}

void AVFXCreatureCharacter::PlayVFX(const FName& Name, bool bIsOriginMeshVFX, const TArray<FPlayMaterialParameterAnimationInfo>& Param)
{
	for (auto Component : ((bIsOriginMeshVFX) ? OriginMeshComponents : FXMeshComponents))
	{
		PlayVFX_ByMesh(Name, bIsOriginMeshVFX, Param, Component);
	}

	if (bIsOriginMeshVFX == false)
		SetFXMeshes_Visibility(true);
}

void AVFXCreatureCharacter::EndVFX(const FName& Name, bool bIsOriginMeshVFX)
{
	if (bIsOriginMeshVFX == false)
		SetFXMeshes_Visibility(false);

	for (auto Component : ((bIsOriginMeshVFX) ? OriginMeshComponents : FXMeshComponents))
	{
		EndVFX_ByMesh(Name, bIsOriginMeshVFX, Component);
	}
}

void AVFXCreatureCharacter::ChangeState(EStateMaterialType State, float Duration)
{
	if (IsValid(StateComponent))
	{
		StateComponent->ChangeState(State, Duration);
	}
}

void AVFXCreatureCharacter::RegisterMesh(USkeletalMeshComponent* MeshComp, bool bOriginMesh, bool bSetMasterPose)
{
	if (!IsValid(MeshComp))
	{
		UE_LOG(LogTemp, Error, TEXT("VFX MeshComp Not Valid~!!"));
		return;
	}

	if (bOriginMesh)
	{
		OriginMeshComponents.Add(MeshComp);
	}
	else
	{
		FXMeshComponents.Add(MeshComp);
	}

	if (bSetMasterPose)
		MeshComp->SetMasterPoseComponent(GetMesh());

	//FXComponent->SetUserParametersToDefaultValues();
}

void AVFXCreatureCharacter::RemoveMesh(USkeletalMeshComponent* MeshComp, bool bOriginMesh)
{
	if (!IsValid(MeshComp))
	{
		UE_LOG(LogTemp, Error, TEXT("VFX MeshComp Not Valid~!!"));
		return;
	}

	if (bOriginMesh)
	{
		OriginMeshComponents.Remove(MeshComp);
	}
	else
	{
		FXMeshComponents.Remove(MeshComp);
	}
}

void AVFXCreatureCharacter::RemoveMeshByIndex(int32 MeshIndex, bool bOriginMesh)
{
	if (bOriginMesh)
	{
		OriginMeshComponents.RemoveAt(MeshIndex);
	}
	else
	{
		FXMeshComponents.RemoveAt(MeshIndex);
	}
}

void AVFXCreatureCharacter::PlayVFX_ByMesh(const FName& Name, bool bIsOriginMesh, const TArray<FPlayMaterialParameterAnimationInfo>& Param, USkeletalMeshComponent* Component)
{
	if (IsValid(Component) == false || IsValid(Component->SkeletalMesh) == false) return;

	UMyGameInstance* Instance = GetGameInstance<UMyGameInstance>();
	if (IsValid(Instance) == false) return;

	TArray<FPlayMaterialParameterAnimationInfo> PlayInfo;
	for (const auto& Info : Param)
	{
		if (bIsOriginMesh == false && IsValid(Info.Material) == false) return;

		if (Info.MaterialIndex == -1)
		{
			for (int Index = 0; Index < Component->GetMaterials().Num(); ++Index)
			{
				FPlayMaterialParameterAnimationInfo NewInfo = FPlayMaterialParameterAnimationInfo(Info);
				NewInfo.MaterialIndex = Index;
				PlayInfo.Add(NewInfo);
			}
		}
		else
		{
			PlayInfo.Add(FPlayMaterialParameterAnimationInfo(Info));
		}
	}

	Instance->Play_MaterialParamAnimation(Name, Component, PlayInfo);
}

void AVFXCreatureCharacter::EndVFX_ByMesh(const FName& Name, bool bIsOriginMesh, USkeletalMeshComponent* Component)
{
	if (IsValid(Component) == false || IsValid(Component->SkeletalMesh) == false) return;

	UMyGameInstance* Instance = GetGameInstance<UMyGameInstance>();
	if (IsValid(Instance) == false) return;

	Instance->Clear_MaterialParamAnimation(Component, Name);
	
	/*if (bIsOriginMesh == false && Param[0].bValueToDefaultWhenEndVFX == true)
		SetFXMeshes_Visibility(false);*/
	
}

USkeletalMeshComponent* AVFXCreatureCharacter::FindMesh(int32 MeshIndex, bool bOriginMesh)
{
	if (bOriginMesh)
	{
		if (OriginMeshComponents.Num() > MeshIndex && 0 <= MeshIndex)
			return OriginMeshComponents[MeshIndex];
	}
	else
	{
		if (FXMeshComponents.Num() > MeshIndex && 0 <= MeshIndex)
			return FXMeshComponents[MeshIndex];
	}
	return nullptr;
}

void AVFXCreatureCharacter::SetOriginMeshes_Visibility(bool bVisibility)
{
	for (auto Item : OriginMeshComponents)
	{
		Item->SetVisibility(bVisibility);
	}
}

void AVFXCreatureCharacter::SetFXMeshes_Visibility(bool bVisibility)
{
	for (auto Item : FXMeshComponents)
	{
		Item->SetVisibility(bVisibility);
	}
}
