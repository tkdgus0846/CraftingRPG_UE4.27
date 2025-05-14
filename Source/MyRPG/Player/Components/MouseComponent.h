
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MouseComponent.generated.h"


UENUM(BlueprintType)
enum class EAimPreview : uint8
{
	EAP_None UMETA(DisplayName = "None"),
	EAP_Circle UMETA(DisplayName = "Circle"),
	EAP_Line UMETA(DisplayName = "Line"),
	EAP_Sector UMETA(DisplayName = "Sector"),
	EAP_MAX UMETA(DisplayName = "MAX")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYRPG_API UMouseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMouseComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Pressed();
	void Released();

	void TurnOnAimPreview(EAimPreview AimType, FVector Size, float OffsetFromCenter = -1.f);
	void TurnOffAimPreview();

	void AimPreviewTraceMouse(bool bState);
	void RotateByAimPreview();
	void RotatePlayerByMouseDirection();

protected:

private:
	UPROPERTY()
	class APlayerCharacter* PlayerCharacter;
	UPROPERTY()
	class APlayerHUD* PlayerHUD;

	/* 스킬 에임 데칼 관련 프로퍼티들 */
	bool bUseAimPreview = false;
	bool bAimPreviewTraceMouse = true;

	void AimPreview();

	void AimCircle();
	void AimLine();
	void AimSector();

	FVector AimPreviewPos;
	FRotator AimPreviewRotation;
	float DecalOffsetFromCenter = -1.f;

	UPROPERTY(EditDefaultsOnly)
	TMap<EAimPreview, TSubclassOf<class ADecalActor>> AimPreviewClasses;
	TMap<EAimPreview, class ADecalActor*> AimPreviews;

	EAimPreview CurAimType = EAimPreview::EAP_None;

	UPROPERTY()
	class ADecalActor* CurAimActor = nullptr;

	/* */
	void TraceEnemy();
	FTimerHandle TraceEnemyTimer;
	void ShowEnemyNameLevelHP(bool bState, class AEnemy* Enemy);

public:
	FVector GetAimPreviewPos();

		
};
