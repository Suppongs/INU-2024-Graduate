// Fill out your copyright notice in the Description page of Project Settings.


#include "BTS_UpdateIsInAttackRange.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Crazy6/Actor/Enemy/EnemyBase.h"


void UBTS_UpdateIsInAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AEnemyBase* EnemyPawn = Cast<AEnemyBase>(OwnerComp.GetAIOwner()->GetPawn());
	if (false == IsValid(EnemyPawn))
	{
		return;
	}

	APawn* Target = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("Target"));
	if (false == IsValid(Target))
	{
		return;
	}

	const float fDistance = EnemyPawn->GetDistanceTo(Target);
	const float fAttackDistance = EnemyPawn->GetAttackRange();

	bool _IsInAttackRange = fDistance < fAttackDistance;
	OwnerComp.GetBlackboardComponent()->SetValueAsBool("IsInAttackRange", _IsInAttackRange);
}
