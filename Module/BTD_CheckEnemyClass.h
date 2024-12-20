// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "Crazy6/Actor/Enemy/EnemyBase.h"
#include "BTD_CheckEnemyClass.generated.h"

class AEnemyBase;
/**
 * 
 */
UCLASS()
class CRAZY6_API UBTD_CheckEnemyClass : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AEnemyBase> mConfiguredEnemyClass;
	
};
