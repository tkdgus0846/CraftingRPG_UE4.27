// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

UENUM()
enum class EInputMode : uint8
{
	EIM_GameAndUI,
	EIM_UIOnly,
	EIM_GameOnly
};

UCLASS()
class MYRPG_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMyPlayerController();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void CameraRotatePressed();
	void CameraRotateReleased();
	void CancelPressed();
	bool IsShowAnyDragableWindow();
	void InventoryOpenPressed();

	void MouseFixWhenRotate();
	void CalcMouseDirectionAngle();

	void ChangeFocusWidget(class UUserWidget* InWidget);

	// ���콺 Ŀ�� �ٲ��ְ� �οﺯ�� �ϳ� ����
	void DisableGameplayMouse();
	void EnableGameplayMouse();

	void CursorPosUpdate();

	UFUNCTION(BlueprintCallable)
	void PlayerInputSetting(bool bEnableInput);

	UFUNCTION(BlueprintCallable)
	void SetUIRendererState(bool bState);

protected:

private:
	/* ���� ���� ������*/
	bool bCameraRotate = false;
	bool bCalcMouseTransform = true;
	bool bEnableGameplayMouse = true;

	float MouseOriginX, MouseOriginY;
	float MouseDirectionAngle;

	/* UI ���� ������ */
	UPROPERTY()
	class APlayerHUD* PlayerHUD = nullptr;

public:
	FORCEINLINE bool IsCameraRotate() const { return bCameraRotate; }
	FORCEINLINE float GetMouseDirectionAngle() const { return MouseDirectionAngle; }
	FORCEINLINE bool IsEnabledGameplayMouse() const { return bEnableGameplayMouse; }
	
};
