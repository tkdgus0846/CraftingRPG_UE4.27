#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

// �������̵� �Ұ� �ƴ϶�� C++ �ڵ忡���� IInteractable �����θ� �̿��Ҽ� �ִ�.
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
