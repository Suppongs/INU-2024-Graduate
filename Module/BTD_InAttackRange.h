// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_InAttackRange.generated.h"

/**
 * 
 */
UCLASS()
class CRAZY6_API UBTD_InAttackRange : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UBTD_InAttackRange();
	
public:
	bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
	
	
};
