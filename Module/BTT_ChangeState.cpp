// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ChangeState.h"
#include "AIController.h"
#include "Crazy6/Actor/Enemy/EnemyBase.h"


EBTNodeResult::Type UBTT_ChangeState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AEnemyBase* EnemyPawn = Cast<AEnemyBase>(OwnerComp.GetAIOwner()->GetPawn());
	if (true == IsValid(EnemyPawn))
	{
		EnemyPawn->SetAIState(mConfiguredState);
	}

	return Result;
}
