// Fill out your copyright notice in the Description page of Project Settings.


#include "BTS_UpdateMovePoint.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Crazy6/Actor/Enemy/EnemyBase.h"


void UBTS_UpdateMovePoint::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AEnemyBase* EnemyPawn = Cast<AEnemyBase>(OwnerComp.GetAIOwner()->GetPawn());
	if (false == IsValid(EnemyPawn))
	{
		return;
	}
	
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(EnemyPawn->GetWorld());
	if (false == IsValid(NavSystem))
	{
		return;
	}

	FNavLocation RandomLocation;
	bool IsFound = NavSystem->GetRandomReachablePointInRadius(EnemyPawn->GetActorLocation(), mRadius, RandomLocation);
	if (false == IsFound)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector("MovePoint", EnemyPawn->GetActorLocation());
		return;
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsVector("MovePoint", RandomLocation.Location);
}
