
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

	/* WasDropped�� true�� �������ִ� �������� �ֿﶧ�� �÷����̴�. */
	UFUNCTION(BlueprintCallable)
	void PushInInventory(bool bWasDropped = false, AActor* NewOwner = nullptr);
	/* �������� ����� ������ ���� �޼ҵ� */
	UFUNCTION(BlueprintCallable)
	virtual void PopFromInventory(const FVector& PopPos, float PopRadius = 100.f);
	/* Ret => ��ġ�� ���� ������*/
	UFUNCTION(BlueprintCallable)
	bool CombineItem(AItem* InItem);

	/* ��Ʈ������ hide/show */
	virtual void SetInteractWidgetVisibility_Implementation(bool bVisibility) override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;

	/** ���𰡰� ��ü ������Ʈ�� �� �� ȣ�� */
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** ���𰡰� ��ü ������Ʈ�� ���� �� ȣ�� */
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

	/* ��Ʈ ������ ���� �͵� */
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

	/* �����ۿ� ���� VFX �� */
	UPROPERTY()
	class UNiagaraSystem* DroppedFX;
	UPROPERTY()
	class UNiagaraSystem* DroppingFX;
	UPROPERTY()
	class UNiagaraSystem* LootFX;

	/* �����ۿ� ���� SFX �� */
	UPROPERTY()
	class USoundCue* DropSFX;
	UPROPERTY()
	class USoundCue* DropHitSFX;
	UPROPERTY()
	class USoundCue* LootSFX;

	/* �������� �������� �׼��� ����� ���� */
	UPROPERTY(VisibleAnywhere)
	class UActionComponent* ActionComponent;
	/* �׼��� �ϰ� �ɸ��� �ð� ���� */
	float DroppingAction(float ActionRadius);
	/* �������� �������� ���ڿ� ȣ��� �Լ� */
	void DropFinished();

	UPROPERTY()
	AActor* ItemOwner = nullptr;

	/* 
		���̾ư��� �Ķ���Ϳ� �����ϱ����� Ÿ�̸��Լ��� 
	*/

	/* �������� �ֿﶧ FX �Ķ���Ϳ� ������ �ֿ����� ��ġ �ѱ�� ������Ƽ */
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
