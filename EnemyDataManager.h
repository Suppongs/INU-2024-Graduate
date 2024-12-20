// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyInfo.h"
#include "Crazy6/Utils/Singleton.h"

class CRAZY6_API CEnemyDataManager : public CSingleton<CEnemyDataManager>
{
public:
	CEnemyDataManager();
	~CEnemyDataManager();

public:
	const FEnemyAnimData* FindAnim(const FName& _Key);
	const FEnemyInfoTable* FindInfo(const FName& _Key);

private:
	TObjectPtr<UDataTable> mpAnimTable = nullptr;
	TObjectPtr<UDataTable> mpInfoTable = nullptr;
};
