// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_TurnToTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Crazy6/Actor/Enemy/EnemyBase.h"


EBTNodeResult::Type UBTT_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AEnemyBase* EnemyPawn = Cast<AEnemyBase>(OwnerComp.GetAIOwner()->GetPawn());
	if (false == IsValid(EnemyPawn))
	{
		return EBTNodeResult::Failed;
	}
	
	APawn* Target = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("Target"));
	if (false == IsValid(Target))
	{
		return EBTNodeResult::Failed;
	}

	FVector LookVector = Target->GetActorLocation() - EnemyPawn->GetActorLocation();
	LookVector.Z = 0.0f;
	FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
	EnemyPawn->SetActorRotation(TargetRot);

	return Result;
}
