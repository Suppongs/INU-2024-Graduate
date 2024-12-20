// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_Attack.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Crazy6/Actor/Enemy/EnemyBase.h"


EBTNodeResult::Type UBTT_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	
	AEnemyBase* EnemyPawn = Cast<AEnemyBase>(OwnerComp.GetAIOwner()->GetPawn());
	if (false == IsValid(EnemyPawn))
	{
		return EBTNodeResult::Failed;
	}

	FAIAttackEnd OnAttackEndDelegate;
	OnAttackEndDelegate.BindLambda([&]()
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Type::Succeeded);
		});
	
	EnemyPawn->SetAIState(EAIState::Attack);
	EnemyPawn->SetAttackEndDelegate(OnAttackEndDelegate);
	EnemyPawn->Attack();
	
	return EBTNodeResult::InProgress;
}
