
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

	// bWasDropped = false이면 주울때 파티클 없음, true이면 주울때 파티클 있음
	UFUNCTION(BlueprintCallable)
	void LootItem(AItem* NewItem, bool bWasDropped = false);

	UFUNCTION(BlueprintCallable)
	AItem* GainItemByInventoryIndex(AItem* NewItem, int Index);

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable)
	FUpdateInvetoryDelegate OnUpdateInventory;

	/* 인벤토리 UI 업데이트를 호출하는 델리게이트.*/
	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable)
	FSelectItemDelegate OnSelectItem;
	/* 인벤토리 UI 업데이트를 호출하는 델리게이트.*/
	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable)
	FDeselectItemDelegate OnDeselectItem;

	// 맨 첫번째 줄에 있는 아이템을 고르게 하는 함수.
	void SelectItem(int Num);

	void SetNewItemArray(const TArray<struct FItemSaveData>& NewItemArray);
	void ClearItemArray();

	UFUNCTION(BlueprintCallable)
	TArray<FCheckRecipeInfo> CheckInventoryItemWithRecipe(TArray<FRecipeInfo> RecipeInfoArray);

	UFUNCTION(BlueprintCallable)
	void SpendItem(TSubclassOf<AItem> ItemClass, int Count);

protected:
	/* WasDropped가 true면 떨어져있던 아이템을 주울때의 플래그이다. */
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

	// -1 인경우 아무것도 선택한게 없다는뜻.
	int CurrentSelectedItemIndex = -1;

	/*현재 내가 고른놈이 NULL이 되는경우 자동으로 앞에것중에 찾아서 바꿔주는 함수*/ 
	void ChangeCurrentItemAndIndex();

	/* 1,2,3,... 숫자패드를 클릭하여 아이템을 선택할수 있는가 없는가를 체크*/
	bool CanSelectItem();

	/* 레시피 체크를 위한 임시 저장용 아이템*/
	UPROPERTY()
	TMap<TSubclassOf<AItem>, AItem*> ItemForCheckRecipeMap;

public:
	EWeaponType GetCurrentSelectedWeaponType() const; 
	class AWeapon* GetCurrentSelectedWeapon() const;
		
};
