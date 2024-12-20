// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AISpawner.h"
#include "AISpawnerTrigger.generated.h"


// Not use Class (replace TriggerMissionObjective)
class UBoxComponent;

UCLASS()
class CRAZY6_API AAISpawnerTrigger : public AAISpawner
{
	GENERATED_BODY()
	
public:
	AAISpawnerTrigger();

public:
	void OnCollision();
	void OffCollision();

protected:
	void PostInitializeComponents();

protected:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> mTrigger = nullptr;

	
};
