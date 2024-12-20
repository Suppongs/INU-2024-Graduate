// Fill out your copyright notice in the Description page of Project Settings.


#include "AISpawnerTrigger.h"
#include "Components/BoxComponent.h"
#include "Crazy6/Global/ProfileInfo.h"

AAISpawnerTrigger::AAISpawnerTrigger()
{
	mTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	mTrigger->SetCollisionProfileName(UE_COLLISION_PROFILE_EVENTTRIGGER);
}

void AAISpawnerTrigger::OnCollision()
{
	mTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AAISpawnerTrigger::OffCollision()
{
	mTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AAISpawnerTrigger::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	mTrigger->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
}

void AAISpawnerTrigger::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	Spawn();
	Destroy();
}
