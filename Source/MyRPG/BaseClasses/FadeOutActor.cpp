
#include "FadeOutActor.h"
#include "DestructibleComponent.h"

AFadeOutActor::AFadeOutActor()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AFadeOutActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFadeOutActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*for (int i = 0; i < StaticMeshes.Num(); i++)
	{
		if (IsValid(StaticMeshes[i]) == false || StaticMeshes[i] == nullptr) continue;

		if (FadeTimers[i].Value == FS_FadeOut)
		{
			FadeOutAlphaMesh(i, DeltaTime);
		}
		else if (FadeTimers[i].Value == FS_FadeIn)
		{
			FadeInAlphaMesh(i, DeltaTime);
		}

	}*/
}

void AFadeOutActor::FadeOut(UMeshComponent* Component)
{
	int Index = Meshes.Find(Component);
	if (Index == INDEX_NONE) return;

	FadeTimers[Index].Value = FS_FadeOut;
	GetWorld()->GetTimerManager().SetTimer(FadeTimers[Index].Key, this, &AFadeOutActor::FadeOutFunc, GetWorld()->DeltaTimeSeconds, true);
}

void AFadeOutActor::FadeIn(UMeshComponent* Component)
{
	int Index = Meshes.Find(Component);
	if (Index == INDEX_NONE) return;
	
	FadeTimers[Index].Value = FS_FadeIn;
	GetWorld()->GetTimerManager().SetTimer(FadeTimers[Index].Key, this, &AFadeOutActor::FadeInFunc, GetWorld()->DeltaTimeSeconds, true);
}

UMeshComponent* AFadeOutActor::InitMeshes(UMeshComponent* Mesh1, UMeshComponent* Mesh2)
{
	if (Meshes.Find(Mesh1) == -1)
	{
		Meshes.Add(Mesh1);
		FadeTimers.Add(TTuple<FTimerHandle, EFadeState>(FTimerHandle(), FS_None));
		MaterialInstances.Add(Mesh1->CreateDynamicMaterialInstance(0, Mesh1->GetMaterial(0)));
	}
	
	if (Mesh2 == nullptr) return Mesh1;

	Meshes.Add(Mesh2);
	FadeTimers.Add(TTuple<FTimerHandle, EFadeState>(FTimerHandle(), FS_None));
	MaterialInstances.Add(Mesh2->CreateDynamicMaterialInstance(0, Mesh2->GetMaterial(0)));

	return Mesh2;
}

void AFadeOutActor::FadeOutFunc()
{
	for (int i = 0; i < FadeTimers.Num(); ++i)
	{
		if (FadeTimers[i].Value == FS_FadeOut)
		{
			FadeOutAlphaMesh(i, GetWorld()->GetTimerManager().GetTimerRate(FadeTimers[i].Key));
		}
	}
}

void AFadeOutActor::FadeInFunc()
{
	for (int i = 0; i < FadeTimers.Num(); ++i)
	{
		if (FadeTimers[i].Value == FS_FadeIn)
		{
			FadeInAlphaMesh(i, GetWorld()->GetTimerManager().GetTimerRate(FadeTimers[i].Key));
		}
	}
}

void AFadeOutActor::FadeOutAlphaMesh(int Index, float Rate)
{
	auto Mesh = Meshes[Index];
	if (Mesh == nullptr || IsValid(Mesh) == false) return;
	
	float Alpha;
	MaterialInstances[Index]->GetScalarParameterValue(TEXT("FadeOutAlpha"), Alpha);

	Alpha -= Rate * FadeOutPlayRate;
	if (Alpha < FadeOutAlphaLimit) Alpha = FadeOutAlphaLimit;
	MaterialInstances[Index]->SetScalarParameterValue("FadeOutAlpha", Alpha);

	if (Alpha == FadeOutAlphaLimit)
	{
		GetWorld()->GetTimerManager().ClearTimer(FadeTimers[Index].Key);
		FadeTimers[Index].Value = FS_None;
	}
}

void AFadeOutActor::FadeInAlphaMesh(int Index, float Rate)
{
	auto Mesh = Meshes[Index];
	if (Mesh == nullptr || IsValid(Mesh) == false) return;

	float Alpha;
	MaterialInstances[Index]->GetScalarParameterValue(TEXT("FadeOutAlpha"), Alpha);

	Alpha += Rate * FadeInPlayRate;
	if (Alpha > FadeInAlphaLimit) Alpha = FadeInAlphaLimit;
	MaterialInstances[Index]->SetScalarParameterValue("FadeOutAlpha", Alpha);

	if (Alpha == FadeInAlphaLimit)
	{
		GetWorld()->GetTimerManager().ClearTimer(FadeTimers[Index].Key);
		FadeTimers[Index].Value = FS_None;
	}
}

