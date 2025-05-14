
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyRPG/Interfaces/Interactable.h"
#include "MyRPG/Types/ItemRarity.h"
#include "Item.generated.h"

UCLASS()
class MYRPG_API AItem : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	AItem();

	/* WasDropped가 true면 떨어져있던 아이템을 주울때의 플래그이다. */
	UFUNCTION(BlueprintCallable)
	void PushInInventory(bool bWasDropped = false, AActor* NewOwner = nullptr);
	/* 아이템을 월드로 던질때 쓰는 메소드 */
	UFUNCTION(BlueprintCallable)
	virtual void PopFromInventory(const FVector& PopPos, float PopRadius = 100.f);
	/* Ret => 합치고 남은 아이템*/
	UFUNCTION(BlueprintCallable)
	bool CombineItem(AItem* InItem);

	/* 루트위젯을 hide/show */
	virtual void SetInteractWidgetVisibility_Implementation(bool bVisibility) override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;

	/** 무언가가 구체 컴포넌트에 들어설 때 호출 */
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** 무언가가 구체 컴포넌트를 나설 때 호출 */
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void BringDataTableDatas();

	/* Item Data Table Properties*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Meta = (AllowPrivateAccess = true))
	class UDataTable* DataTable;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Meta = (AllowPrivateAccess = true))
	FName RowName;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	FName ItemID;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	class UTexture2D* PreviewTexture;

	/* Item Actor Properties */
	UPROPERTY(VisibleDefaultsOnly)
	class UStaticMeshComponent* DroppedMeshComponent;

	UPROPERTY(VisibleDefaultsOnly)
	class USphereComponent* DetectSphere;

	UPROPERTY(VisibleDefaultsOnly)
	class USceneComponent* RootSceneComponent;

	UPROPERTY(VisibleDefaultsOnly)
	class UNiagaraComponent* FXComponent;

	/* 루트 위젯에 대한 것들 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	class UWidgetComponent* LootWidget;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	int ItemCount = 1;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	int ItemMaxStackCount = 1;

	bool bStackable = false;
	bool bConsumable = false;
	bool bInInventory = false;

private:
	void UpdateLootWidget();

	FVector OriginalLocation;
	void SetLocationToOriginal();

	/* 아이템에 관한 VFX 들 */
	UPROPERTY()
	class UNiagaraSystem* DroppedFX;
	UPROPERTY()
	class UNiagaraSystem* DroppingFX;
	UPROPERTY()
	class UNiagaraSystem* LootFX;

	/* 아이템에 관한 SFX 들 */
	UPROPERTY()
	class USoundCue* DropSFX;
	UPROPERTY()
	class USoundCue* DropHitSFX;
	UPROPERTY()
	class USoundCue* LootSFX;

	/* 아이템이 떨어지는 액션을 만들기 위함 */
	UPROPERTY(VisibleAnywhere)
	class UActionComponent* ActionComponent;
	/* 액션을 하고 걸리는 시간 리턴 */
	float DroppingAction(float ActionRadius);
	/* 아이템이 떨어지고 난뒤에 호출될 함수 */
	void DropFinished();

	UPROPERTY()
	AActor* ItemOwner = nullptr;

	/* 
		나이아가라 파라미터에 전달하기위한 타이머함수들 
	*/

	/* 아이템을 주울때 FX 파라미터에 아이템 주운사람의 위치 넘기는 프로퍼티 */
	void SendLootedPos();
	FTimerHandle SendLootedPosTimer;
	UPROPERTY()
	class UNiagaraComponent* LootFXComp = nullptr;

public:
	FORCEINLINE FName GetItemID() const { return ItemID; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsStackable() const { return bStackable;}

	FORCEINLINE bool IsConsumable() const { return bConsumable; }

	FORCEINLINE void SetItemCount(int Count) { ItemCount = Count; }
	FORCEINLINE uint32 GetItemCount() const { return ItemCount; }

	FORCEINLINE int GetItemMaxStackCount() const { return ItemMaxStackCount; }

	FORCEINLINE bool IsItemMaxStackCount() { return ItemMaxStackCount == ItemCount; }

	FORCEINLINE class UStaticMeshComponent* GetDroppedMeshComponent() const { return DroppedMeshComponent; }
	FORCEINLINE class UTexture2D* GetPreviewTexture() const { return PreviewTexture; }


};
