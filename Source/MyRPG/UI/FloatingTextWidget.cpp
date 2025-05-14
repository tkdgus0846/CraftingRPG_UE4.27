// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingTextWidget.h"
#include <Animation/MovieScene2DTransformTrack.h>
#include <Animation/MovieScene2DTransformSection.h>
#include "Components/TextBlock.h"

void UFloatingTextWidget::NativeConstruct()
{
	Super::NativeConstruct();

	float RandX = FMath::FRandRange(RandXRange.X, RandXRange.Y);
	float RandY = FMath::FRandRange(RandYRange.X, RandYRange.Y);

	RandTargetTranslation = FVector2D(
		RandX, 
		RandY);

	FTimerDelegate TimerDele;
	TimerDele.BindUFunction(this, "LerpRenderTranslation", GetWorld()->GetDeltaSeconds());
	GetWorld()->GetTimerManager().SetTimer(LerpTranslationTimer, TimerDele, GetWorld()->GetDeltaSeconds(), false);

	PlayAnimationForward(FadeOutAnimation);
	
	
	/*SetAnimationTranslationRandom();*/
}

void UFloatingTextWidget::SetText(const FString& Text, const FLinearColor& Color)
{
	if (IsValid(FloatingText))
	{
		FloatingText->SetText(FText::FromString(Text));
		FloatingText->ColorAndOpacity = FSlateColor(Color);
	}
}

void UFloatingTextWidget::LerpRenderTranslation(float ElapsedTime)
{
	float LerpValue = ElapsedTime / AnimationTime;

	LerpValue = FMath::Clamp(LerpValue, 0.f, 1.f);
	float X = FMath::Lerp(0.f, RandTargetTranslation.X, LerpValue);
	float Y = FMath::Lerp(0.f, RandTargetTranslation.Y, LerpValue);

	FWidgetTransform Transform;
	Transform.Translation = FVector2D(X, Y);
	SetRenderTransform(Transform);

	if (LerpValue < 1.f)
	{
		FTimerDelegate TimerDele;
		TimerDele.BindUFunction(this, "LerpRenderTranslation", ElapsedTime + GetWorld()->GetDeltaSeconds());
		GetWorld()->GetTimerManager().SetTimer(LerpTranslationTimer, TimerDele, GetWorld()->GetDeltaSeconds(), false);
	}
	//else //
	//{
	//	RemoveFromParent();
	//}
}

//void UFloatingTextWidget::SetAnimationTranslationRandom()
//{
//	if (IsValid(FadeOutAnimation) == false || IsValid(FadeOutAnimation->MovieScene) == false)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Invalid WidgetAnimation or MovieScene"));
//		return;
//	}
//
//    for (const FMovieSceneBinding& Binding : FadeOutAnimation->MovieScene->GetBindings())
//    {
//        for (UMovieSceneTrack* Track : Binding.GetTracks())
//        {
//            UMovieScene2DTransformTrack* TransformTrack = Cast<UMovieScene2DTransformTrack>(Track);
//            if (IsValid(TransformTrack) == false)
//            {
//                continue;
//            }
//
//            const TArray<UMovieSceneSection*>& SectionArray = TransformTrack->GetAllSections();
//
//            UMovieScene2DTransformSection* TransformSection = (SectionArray.Num() >= 1) ? Cast<UMovieScene2DTransformSection>(SectionArray[SectionArray.Num() - 1]) : nullptr;
//            if (IsValid(TransformSection))
//            {
//                float RandX = FMath::FRandRange(RandXRange.X, RandXRange.Y);
//                float RandY = FMath::FRandRange(RandYRange.X, RandYRange.Y);
//
//                FMovieSceneFloatChannel;
//                TransformSection->Translation[0] = RandX; // -80~80 ?
//                TransformSection->Translation[1] = ; // -30~ -80 Á¤µµ?
//
//                return;
//            }
//        }
//    }
//}
