// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RenderTargetFunctionLibrary.generated.h"

class UTextureRenderTarget2D;

UCLASS()
class MYRPG_API URenderTargetFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Custom")
	static UTextureRenderTarget2D* ExportCaptureSceneToRenderTarget2D(UWorld* World, const FString& RenderTargetPath, const FString& RenderTargetName, const FTransform& CaptureTransform);

	UFUNCTION(BlueprintCallable, Category = "Custom")
	static void ExportRenderTargetToHDR(UWorld* World, UTextureRenderTarget2D* RenderTarget, const FString& HDRFileName);

	UFUNCTION(BlueprintCallable, Category = "Custom")
	static void ExportHDRToRenderTarget(UWorld* World, const FString& HDRName, const FString& RenderTargetPath, const FString& RenderTargetName);
	
};
