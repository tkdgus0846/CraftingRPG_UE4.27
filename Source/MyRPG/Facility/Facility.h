
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyRPG/Interfaces/Interactable.h"
#include "Facility.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFacilityWorkingDelegate, float, RemainedTime);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFacilityWorkEndDelegate);

UCLASS()
class MYRPG_API AFacility : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	AFacility();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void Interact_Implementation() override;
	virtual void SetInteractWidgetVisibility_Implementation(bool bVisibility) final;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void WorkStart(float Duration);
	virtual void WorkStart_Implementation(float Duration);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void WorkEnd();
	virtual void WorkEnd_Implementation();

	void FinishFacilityInteract(bool bHideAllWindow);
	void HideFacilityWindow();

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable)
	FFacilityWorkingDelegate FacilityWorkingDelegate;
	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable)
	FFacilityWorkEndDelegate FacilityWorkEndDelegate;

protected:
	UFUNCTION(BlueprintPure, meta = (CommutativeAssociativeBinaryOperator = true))
	class UNiagaraComponent* InitInteractVFXComponents(class UNiagaraComponent* VFX1, class UNiagaraComponent* VFX2);

	UFUNCTION(BlueprintCallable)
	void ActivateInteractVFX();
	UFUNCTION(BlueprintCallable)
	void DeactivateInteractVFX();

	/** 무언가가 구체 컴포넌트에 들어설 때 호출 */
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	/** 무언가가 구체 컴포넌트를 나설 때 호출 */
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UPROPERTY()
	class APlayerCharacter* PlayerCharacter = nullptr;

	bool bIsPlayerUsing = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	FName FacilityName;

	UPROPERTY(VisibleDefaultsOnly)
	class UWidgetComponent* InteractWidget;

	UPROPERTY(VisibleDefaultsOnly)
	class USphereComponent* DetectSphere;

	UPROPERTY(VisibleDefaultsOnly)
	class USceneComponent* RootSceneComponent;

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	bool bIsWorking = false;

	UPROPERTY()
	float ElapsedTime = 0.f;

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	float RemainedTime = 0.f;

	UPROPERTY()
	TArray<class UNiagaraComponent*> InteractVFXComponents;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UDragableWindow> FacilityWindowClass;
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	class UDragableWindow* FacilityWindow = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	class UInventory* InputInventory;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	class UInventory* OutputInventory;

	UPROPERTY()
	class USoundCue* InteractSound;
	UPROPERTY(EditDefaultsOnly)
	class USoundCue* WorkSound;

	UPROPERTY()
	class UAudioComponent* CurAudioComponent;
	UFUNCTION()
	void FadeOutSound();

public:
	FORCEINLINE const bool IsPlayerUsing() const { return bIsPlayerUsing; }
};
