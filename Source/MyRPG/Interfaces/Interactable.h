#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

// 오버라이딩 할게 아니라면 C++ 코드에서만 IInteractable 구현부를 이용할수 있다.
class MYRPG_API IInteractable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Interact();
	virtual void Interact_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetInteractWidgetVisibility(bool bVisibility);
	virtual void SetInteractWidgetVisibility_Implementation(bool bVisibility);
};
