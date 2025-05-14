
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadingScreen.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadingFadeInFinished);

UCLASS()
class MYRPG_API ULoadingScreen : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	void FadeIn();
	void FadeOut();

	void Finishloading();
	FOnLoadingFadeInFinished& GetOnLoadingFadeInFinished() {return OnLoadingFadeInFinished;}

private:
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Animations", Meta = (AllowPrivateAccess = true, BindWidgetAnim))
	class UWidgetAnimation* FadeInAnimation;
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Animations", Meta = (AllowPrivateAccess = true, BindWidgetAnim))
	class UWidgetAnimation* FadeOutAnimation;

	UFUNCTION()
	void FadeOutAnimationFinished();
	UFUNCTION()
	void FadeInAnimationFinished();

	bool bFadeInAnimationFinished = false;
	bool bLoadingFinished = false;

	FWidgetAnimationDynamicEvent FadeInFinishedEvent;
	FWidgetAnimationDynamicEvent FadeOutFinishedEvent;

	FOnLoadingFadeInFinished OnLoadingFadeInFinished;

	void AttachPlayerToGround();

	FTimerHandle AttachPlayerToGroundTimer;
};
