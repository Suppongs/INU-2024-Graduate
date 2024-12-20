// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_UpdateMovePoint.generated.h"

/**
 * 
 */
UCLASS()
class CRAZY6_API UBTS_UpdateMovePoint : public UBTService
{
	GENERATED_BODY()
	
protected:
	void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere)
	float mRadius = 0.f;
	
	
};