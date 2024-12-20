// Fill out your copyright notice in the Description page of Project Settings.


#include "BTD_InAttackRange.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Crazy6/Actor/Enemy/EnemyBase.h"


UBTD_InAttackRange::UBTD_InAttackRange()
{
	
}

bool UBTD_InAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool Result = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
	
	AEnemyBase* EnemyPawn = Cast<AEnemyBase>(OwnerComp.GetAIOwner()->GetPawn());
	if (false == IsValid(EnemyPawn))
	{
		return false;
	}
	
	APawn* Target = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("Target"));
	if (false == IsValid(Target))
	{
		return false;
	}

	const float fAttackRange = EnemyPawn->GetAttackRange();
	const float fDistanceToTarget = EnemyPawn->GetDistanceTo(Target);
	bool _IsInAttackRange = fDistanceToTarget < fAttackRange;
	OwnerComp.GetBlackboardComponent()->SetValueAsBool("IsInAttackRange", _IsInAttackRange);
	
	Result = OwnerComp.GetBlackboardComponent()->GetValueAsBool("IsInAttackRange");
	
	return Result;
}
