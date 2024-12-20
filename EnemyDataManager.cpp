// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyDataManager.h"
#include "Crazy6/DebugMacros.h"


CEnemyDataManager::CEnemyDataManager()
{
	// Data Load from UDataTable in Editor Object
	static ConstructorHelpers::FObjectFinder<UDataTable>
		MonsterAnimTable(TEXT("/Script/Engine.DataTable'/Game/Dev/AI/Enemy/DT_EnemyAnimData.DT_EnemyAnimData'"));
	if (true == MonsterAnimTable.Succeeded())
	{
		mpAnimTable = MonsterAnimTable.Object;
		LOG_MSG(TEXT("Load Monster Anim Data Table Asset"));
	}
	else
	{
		LOG_ERROR(TEXT("Not load data table asset"));
	}

	static ConstructorHelpers::FObjectFinder<UDataTable>
		MonsterInfoTable(TEXT("/Script/Engine.DataTable'/Game/Dev/AI/Enemy/DT_EnemyInfoData.DT_EnemyInfoData'"));
	if (true == MonsterInfoTable.Succeeded())
	{
		mpInfoTable = MonsterInfoTable.Object;
		LOG_MSG(TEXT("Load Monster Info Data Table Asset"));
	}
	else
	{
		LOG_ERROR(TEXT("Not load data table asset"));
	}
}

CEnemyDataManager::~CEnemyDataManager()
{
}

const FEnemyAnimData* CEnemyDataManager::FindAnim(const FName& _Key)
{
	if (nullptr == mpAnimTable)
	{
		LOG_ERROR(TEXT("Not load data table asset"));
		return nullptr;
	}

	return mpAnimTable->FindRow<FEnemyAnimData>(_Key, TEXT("Not Found Table"));
}

const FEnemyInfoTable* CEnemyDataManager::FindInfo(const FName& _Key)
{
	if (nullptr == mpAnimTable)
	{
		LOG_ERROR(TEXT("Not load data table asset"));
		return nullptr;
	}

	return mpInfoTable->FindRow<FEnemyInfoTable>(_Key, TEXT("Not Found Table"));
}
