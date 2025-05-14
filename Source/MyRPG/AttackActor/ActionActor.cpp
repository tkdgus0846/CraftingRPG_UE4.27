// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "MyRPG/BaseClasses/Components/ActionComponent.h"

AActionActor::AActionActor()
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("ProjectileMesh");
	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetupAttachment(RootScene);

		if (CollisionBox)
		{
			CollisionBox->SetupAttachment(StaticMeshComponent);
		}
	}

	ActionComponent = CreateDefaultSubobject<UActionComponent>("Action");
}

void AActionActor::BeginPlay()
{
	bAttachToOwner = false;
	bRotateInherit = false;
	bLocationInherit = false;

	Super::BeginPlay();
}

void AActionActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AActionActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	}
}

void AActionActor::AddAction(const FActionInfo& ActionInfo)
{
	if (ActionComponent)
	{
		ActionComponent->AddAction(ActionInfo);
	}
}

void AActionActor::PlayAction()
{
	if (ActionComponent)
	{
		//UE_LOG(LogTemp, Error, TEXT("%s"), *GetActorLocation().ToString());
		ActionComponent->PlayAction();
	}
}

void AActionActor::PauseAction()
{
	if (ActionComponent)
	{
		ActionComponent->PauseAction();
	}
}

void AActionActor::ResumeAction()
{
	if (ActionComponent)
	{
		ActionComponent->ResumeAction();
	}
}

void AActionActor::ResetAction()
{
	if (ActionComponent)
	{
		ActionComponent->ResetAction();
	}
}

void AActionActor::ClearAction()
{
	if (ActionComponent)
	{
		ActionComponent->ClearAction();
	}
}

const bool AActionActor::IsActionPlaying()
{
	
	if (ActionComponent)
	{
		return ActionComponent->IsActionPlaying();
	}
	return false;
}

void AActionActor::Destroy()
{
	if (ActionComponent)
	{
		ActionComponent->DestroyComponent();
	}
	Super::Destroy();
}
