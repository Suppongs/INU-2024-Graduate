// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AISpawner.h"
#include "BeginPlayAISpawner.generated.h"

/**
 * 
 */
UCLASS()
class CRAZY6_API ABeginPlayAISpawner : public AAISpawner
{
	GENERATED_BODY()
	
public:
	void BeginPlay() override;
	
	
};
