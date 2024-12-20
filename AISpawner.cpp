// Fill out your copyright notice in the Description page of Project Settings.


#include "AISpawner.h"
#include "Components/CapsuleComponent.h"
#include "EnemyBase.h"
#include "Crazy6/DebugMacros.h"

AAISpawner::AAISpawner()
{
	// <======= Root Setting =======>
	mRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(mRoot);
}

void AAISpawner::BeginPlay()
{
	Super::BeginPlay();
}

AEnemyBase* AAISpawner::Spawn(bool bUseRandomSpawn /*= false*/, float fRandomRadius /*= 0.0f*/)
{
	// <========== Actor Spawn ===========>
	if (false == IsValid(mSpawnClass))
	{
		LOG_ERROR(TEXT("Spawn class is nullptr"));
		return nullptr;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FVector	SpawnLocation = GetActorLocation();
	FRotator SpawnRotator = GetActorRotation();

	if (true == bUseRandomSpawn)
	{
		FVector2D RandomCirle = FMath::RandPointInCircle(fRandomRadius);

		SpawnLocation.X += RandomCirle.X;
		SpawnLocation.Y += RandomCirle.Y;
	}

	AEnemyBase* pDefaultPawn = mSpawnClass->GetDefaultObject<AEnemyBase>();
	if (true == IsValid(pDefaultPawn))
	{
		SpawnLocation.Z += pDefaultPawn->GetCapsule()->GetScaledCapsuleHalfHeight();
	}

	AEnemyBase* pEnemy = GetWorld()->SpawnActor<AEnemyBase>(mSpawnClass, SpawnLocation, SpawnRotator);
	return pEnemy;
}