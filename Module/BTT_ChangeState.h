// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Crazy6/Actor/Enemy/EnemyEnum.h"
#include "BTT_ChangeState.generated.h"

enum class EAIState : uint8;
/**
 * 
 */
UCLASS()
class CRAZY6_API UBTT_ChangeState : public UBTTaskNode
{
	GENERATED_BODY()
	
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAIState mConfiguredState = EAIState::Idle;
	
};
