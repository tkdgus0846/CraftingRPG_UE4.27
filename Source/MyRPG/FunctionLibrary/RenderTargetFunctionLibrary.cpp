
#include "RenderTargetFunctionLibrary.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/SceneCapture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include <Kismet/KismetRenderingLibrary.h>

/*테스트용*/
#include "Components/Image.h"

UTextureRenderTarget2D* URenderTargetFunctionLibrary::ExportCaptureSceneToRenderTarget2D(UWorld* World, const FString& RenderTargetPath, const FString& RenderTargetName, const FTransform& CaptureTransform)
{
	if (IsValid(World) == false)
	{
		return nullptr;
	}

	ASceneCapture2D* SceneCaptureActor = World->SpawnActor<ASceneCapture2D>(ASceneCapture2D::StaticClass(), CaptureTransform);

	if (IsValid(SceneCaptureActor))
	{
		FString Path = RenderTargetPath + "/RT_" + RenderTargetName;

		UTextureRenderTarget2D* RenderTarget = Cast<UTextureRenderTarget2D>(StaticLoadObject(UTextureRenderTarget2D::StaticClass(), nullptr, *Path));

		if (IsValid(RenderTarget) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("RenderTarget NotFound!!"));
			return nullptr;
		}

		SceneCaptureActor->GetCaptureComponent2D()->TextureTarget = RenderTarget;
		SceneCaptureActor->GetCaptureComponent2D()->CaptureSource = ESceneCaptureSource::SCS_FinalColorHDR;
		SceneCaptureActor->GetCaptureComponent2D()->FOVAngle = 90.0f;
		SceneCaptureActor->GetCaptureComponent2D()->bCaptureEveryFrame = false;
		SceneCaptureActor->GetCaptureComponent2D()->bCaptureOnMovement = false;
		SceneCaptureActor->GetCaptureComponent2D()->CaptureScene();

		SceneCaptureActor->SetLifeSpan(0.1f);

		return RenderTarget;
	}
	return nullptr;
}

void URenderTargetFunctionLibrary::ExportRenderTargetToHDR(UWorld* World, UTextureRenderTarget2D* RenderTarget, const FString& HDRFileName)
{
	FString ExportImagePath = FPaths::ProjectSavedDir() + "/SaveGames";
	FString ExportImageFilename = HDRFileName + ".png"; // 원래는 HDR이였음

	UKismetRenderingLibrary::ExportRenderTarget(World, RenderTarget, ExportImagePath, ExportImageFilename);
}

void URenderTargetFunctionLibrary::ExportHDRToRenderTarget(UWorld* World, const FString& HDRName, const FString& RenderTargetPath, const FString& RenderTargetName)
{
	FString HDRFilePath = FPaths::ProjectSavedDir() + "/SaveGames/" + HDRName + TEXT(".png");// 원래는 HDR이였음

	UTexture2D* LoadedTexture = UKismetRenderingLibrary::ImportFileAsTexture2D(World, HDRFilePath);
	if (IsValid(LoadedTexture) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ImportFileAsTexture2D Failed......"));
		return;
	}
	UMaterial* Material = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), nullptr, TEXT("/Game/Blueprints/Material/UIMaterial/M_TextureToRenderTarget")));
	if (IsValid(Material) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ImportMaterial Failed......"));
		return;
	}

	UMaterialInstanceDynamic* MaterialInstanceDynamic = UMaterialInstanceDynamic::Create(Material, nullptr);
	if (IsValid(MaterialInstanceDynamic) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("CreateMaterialDynamic Failed......"));
		return;
	}

	MaterialInstanceDynamic->SetTextureParameterValue("Texture", LoadedTexture);

	FString Path = RenderTargetPath + "/RT_" + RenderTargetName;
	UTextureRenderTarget2D* RenderTarget = Cast<UTextureRenderTarget2D>(StaticLoadObject(UTextureRenderTarget2D::StaticClass(), nullptr, *Path));

	if (IsValid(RenderTarget) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("RenderTarget NotFound!!"));
		return;
	}

	UKismetRenderingLibrary::DrawMaterialToRenderTarget(World, RenderTarget, MaterialInstanceDynamic);
}
