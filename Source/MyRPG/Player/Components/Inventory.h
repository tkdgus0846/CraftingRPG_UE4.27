
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyRPG/Item/Item.h"
#include "MyRPG/Types/WeaponType.h"
#include "Inventory.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUpdateInvetoryDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSelectItemDelegate, int, SelectNum);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDeselectItemDelegate, int, DeselectNum);

USTRUCT(BlueprintType)
struct MYRPG_API FCheckRecipeInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Count = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int RequireNum = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanCraftCondition = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* ItemPreviewTexture = nullptr;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYRPG_API UInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventory();
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	TArray<AItem*>& GetItemArr();

	UFUNCTION(BlueprintCallable)
	bool IsItemValid(int FindIndex);

	UFUNCTION(BlueprintCallable)
	void DiscardItem(int Index, bool bDestroy = false, FVector PopPosOffset = FVector::ZeroVector, float PopRadius = 100.f);

	UFUNCTION(BlueprintCallable)
	void ExchangeItem(int AIndex, int BIndex);

	UFUNCTION(BlueprintCallable)
	void ExchangeItemOtherInventory(int ThisIndex, UInventory* OtherInventory, int OtherIndex);

	//void LootItem();

	// bWasDropped = false�̸� �ֿﶧ ��ƼŬ ����, true�̸� �ֿﶧ ��ƼŬ ����
	UFUNCTION(BlueprintCallable)
	void LootItem(AItem* NewItem, bool bWasDropped = false);

	UFUNCTION(BlueprintCallable)
	AItem* GainItemByInventoryIndex(AItem* NewItem, int Index);

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable)
	FUpdateInvetoryDelegate OnUpdateInventory;

	/* �κ��丮 UI ������Ʈ�� ȣ���ϴ� ��������Ʈ.*/
	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable)
	FSelectItemDelegate OnSelectItem;
	/* �κ��丮 UI ������Ʈ�� ȣ���ϴ� ��������Ʈ.*/
	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable)
	FDeselectItemDelegate OnDeselectItem;

	// �� ù��° �ٿ� �ִ� �������� ���� �ϴ� �Լ�.
	void SelectItem(int Num);

	void SetNewItemArray(const TArray<struct FItemSaveData>& NewItemArray);
	void ClearItemArray();

	UFUNCTION(BlueprintCallable)
	TArray<FCheckRecipeInfo> CheckInventoryItemWithRecipe(TArray<FRecipeInfo> RecipeInfoArray);

	UFUNCTION(BlueprintCallable)
	void SpendItem(TSubclassOf<AItem> ItemClass, int Count);

protected:
	/* WasDropped�� true�� �������ִ� �������� �ֿﶧ�� �÷����̴�. */
	void PushItem(AItem* Item, uint32 Index, bool bWasDropped = false);
	uint32 FindItemIndex(AItem* Item);
	bool RemoveIndexFromMap(const FName& ItemName, uint32 Index);
	void AddIndexToMap(const FName& ItemName, uint32 Index);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	int InventorySize = 36;

	UPROPERTY()
	TArray<AItem*> ItemArray;
	TMap<FName, TArray<uint32>> IndexMap;

	UPROPERTY()
	class AWeapon* CurrentSelectedWeapon = nullptr;

	void ChangeCurrentSelectedWeapon(class AWeapon* NewWeapon);

	// -1 �ΰ�� �ƹ��͵� �����Ѱ� ���ٴ¶�.
	int CurrentSelectedItemIndex = -1;

	/*���� ���� ������ NULL�� �Ǵ°�� �ڵ����� �տ����߿� ã�Ƽ� �ٲ��ִ� �Լ�*/ 
	void ChangeCurrentItemAndIndex();

	/* 1,2,3,... �����е带 Ŭ���Ͽ� �������� �����Ҽ� �ִ°� ���°��� üũ*/
	bool CanSelectItem();

	/* ������ üũ�� ���� �ӽ� ����� ������*/
	UPROPERTY()
	TMap<TSubclassOf<AItem>, AItem*> ItemForCheckRecipeMap;

public:
	EWeaponType GetCurrentSelectedWeaponType() const; 
	class AWeapon* GetCurrentSelectedWeapon() const;
		
};
