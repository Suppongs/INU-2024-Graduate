// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AISpawner.generated.h"

class AEnemyBase;

UCLASS()
class CRAZY6_API AAISpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAISpawner();

public:
	void BeginPlay() override;

public:
	AEnemyBase* Spawn(bool bUseRandomSpawn = false, float fRandomRadius = 0.0f);

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> mRoot = nullptr;

	UPROPERTY(EditAnywhere, Category = "SpawnAI")
	TSubclassOf<AEnemyBase> mSpawnClass = nullptr;
	
};
