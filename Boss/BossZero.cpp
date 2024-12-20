// Fill out your copyright notice in the Description page of Project Settings.


#include "BossZero.h"
#include "Beam.h"
#include "Bomb.h"
#include "BrainComponent.h"
#include "EnergyBall.h"
#include "NavigationSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Crazy6/Actor/TeamEnum.h"
#include "Crazy6/Actor/Enemy/Controller/BossController.h"
#include "Crazy6/Global/ProfileInfo.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/ProjectileMovementComponent.h"

ABossZero::ABossZero()
{
	bUseFabrik  = false;
	
	/* <=========== Mesh Setting ===========> */
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>
		MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/Dev/AI/Enemy/Boss/Mesh/SM_Sphere_Bot.SM_Sphere_Bot'"));
	if (true == MeshAsset.Succeeded())
	{
		mMesh->SetSkeletalMeshAsset(MeshAsset.Object);
		mMesh->SetRelativeLocation(FVector(0.0, 0.0, -300.0));
		mMesh->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));
		mMesh->SetRelativeScale3D(FVector(3.0, 3.0, 3.0));
		mCapsule->SetCapsuleHalfHeight(300.f);
		mCapsule->SetCapsuleRadius(300.f);
		mCapsule->SetCollisionProfileName(UE_COLLISION_PROFILE_ENEMY);
		mCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		static ConstructorHelpers::FClassFinder<UAnimInstance>
			AnimObject(TEXT("/Script/Engine.AnimBlueprint'/Game/Dev/AI/Enemy/Boss/ABP_Zero.ABP_Zero_C'"));
		if (true == AnimObject.Succeeded())
		{
			mMesh->SetAnimInstanceClass(AnimObject.Class);
		}
	}

	/* <=========== Key Setting ===========> */
	mEnemyKey = TEXT("BossZero");
	
	// <======= AI Setting =======>
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = ABossController::StaticClass();

	/* <=========== Boss HitBox Setting ===========> */
	mBossHitBox = CreateDefaultSubobject<USphereComponent>(TEXT("BossHitBox"));
	mBossHitBox->SetupAttachment(mMesh, TEXT("BossHitBox"));
	mBossHitBox->SetRelativeScale3D(FVector(0.01, 0.01, 0.01));
	mBossHitBox->SetSphereRadius(35.f);
	mBossHitBox->SetCollisionProfileName(UE_COLLISION_PROFILE_ENEMY);
	mBossHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	for (int i = 1; i <= 12; ++i)
	{
		FString ArmorSocketName = FString::Printf(TEXT("Leg%d_Armor"), i);
		mArmorHitBox = CreateDefaultSubobject<UBoxComponent>(*ArmorSocketName);
		mArmorHitBox->SetupAttachment(mMesh, *ArmorSocketName);
		mArmorHitBox->SetRelativeScale3D(FVector(0.01, 0.01, 0.01));
		mArmorHitBox->SetBoxExtent(FVector(40.0, 60.0, 10.0));
		mArmorHitBox->SetRelativeLocation(FVector(0.0, -0.665, -0.053));
		mArmorHitBox->SetCollisionProfileName(UE_COLLISION_PROFILE_BOSSARMOR);
		mArmorHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

	/* <=========== RollingAttack HitBox Setting ===========> */
	mRollingHitBox = CreateDefaultSubobject<USphereComponent>(TEXT("RollingHitBox"));
	mRollingHitBox->SetupAttachment(mCapsule);
	mRollingHitBox->SetSphereRadius(300.f);
	mRollingHitBox->SetCollisionProfileName(UE_COLLISION_PROFILE_ENEMYATTACK);
	mRollingHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/* <=========== SlamAttack HitBox Setting ===========> */
	mSlamHitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SlamHitBox"));
	mSlamHitBox->SetupAttachment(mCapsule);
	mSlamHitBox->SetBoxExtent(FVector(470.0, 470.0, 30.0));
	mSlamHitBox->SetRelativeLocation(FVector(0.0, 0.0, -270.0));
	mSlamHitBox->SetCollisionProfileName(UE_COLLISION_PROFILE_ENEMYATTACK);
	mSlamHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/* <=========== Attack OverLap Setting ===========> */
	mRollingHitBox->OnComponentBeginOverlap.AddDynamic(this,
		&ABossZero::OnBegineOverlap_Rolling);

	mSlamHitBox->OnComponentBeginOverlap.AddDynamic(this,
		&ABossZero::OnBegineOverlap_Slam);

	// <======= Attack Setting =======>
	mEnergyBallClass = AEnergyBall::StaticClass();
	
	mBeam = CreateDefaultSubobject<UBeam>(TEXT("Beam"));
	mBeam->SetupAttachment(mMesh, TEXT("Muzzle"));

	mBombClass = ABomb::StaticClass();

	// <======= FX Setting =======>
	static ConstructorHelpers::FObjectFinder<USoundBase>
		ChargeSoundBase(TEXT("/Script/Engine.SoundWave'/Game/Dev/AI/Enemy/Boss/Sound/Laser_Charge.Laser_Charge'"));
	if (ChargeSoundBase.Succeeded())
	{
		mLaserChargeSound = ChargeSoundBase.Object;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem>
		RollingHitParticle(TEXT("/Script/Engine.ParticleSystem'/Game/Dev/AI/Enemy/Humanoid/FX/P_Minion_Impact_Default.P_Minion_Impact_Default'"));
	if (RollingHitParticle.Succeeded())
	{
		mRollingHitParticle = RollingHitParticle.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase>
		RollingSoundBase(TEXT("/Script/Engine.SoundWave'/Game/Dev/AI/Enemy/Boss/Sound/Rolling.Rolling'"));
	if (RollingSoundBase.Succeeded())
	{
		mRollingSound = RollingSoundBase.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase>
		RollingHitSoundBase(TEXT("/Script/Engine.SoundWave'/Game/Dev/AI/Enemy/Boss/Sound/Rolling_Hit.Rolling_Hit'"));
	if (RollingHitSoundBase.Succeeded())
	{
		mRollingHitSound = RollingHitSoundBase.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>
			ExplosionNiagaraSystem(TEXT("/Script/Niagara.NiagaraSystem'/Game/Dev/AI/Enemy/Boss/SlamFX/VFX/EarthExplosion/N_EarthExplosion.N_EarthExplosion'"));
	if (true == ExplosionNiagaraSystem.Succeeded())
	{
		mSlamFX = ExplosionNiagaraSystem.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase>
		SlamSoundBase(TEXT("/Script/Engine.SoundWave'/Game/Dev/AI/Enemy/Boss/Sound/Slam.Slam'"));
	if (SlamSoundBase.Succeeded())
	{
		mSlamSound = SlamSoundBase.Object;
	}
}

void ABossZero::BeginPlay()
{
	Super::BeginPlay();

	mAnimInstance = Cast<UEnemyAnimTemplate>(mMesh->GetAnimInstance());
	if (true == IsValid(mAnimInstance))
	{
		mAnimInstance->SetAnimData(mEnemyKey);
	}

	GetWorld()->GetTimerManager().SetTimer(mPlayTimer, mPlayTime, false);
}

void ABossZero::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ABossController* pAIController = Cast<ABossController>(NewController);
	if (true == IsValid(pAIController))
	{
		pAIController->SetTeamID(ETeamID::ENEMY);
	}
}

void ABossZero::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Rolling Move logic
	if (true == IsRollingMove)
	{
		//Change Rotation
		FVector MovingDirection = mTarget->GetActorLocation() - GetActorLocation();
		MovingDirection.Z = 0.0;
		FRotator TargetRotation = MovingDirection.Rotation();
		FRotator CurrentRotation = GetActorRotation();
		FRotator NextRotation = FMath::RInterpConstantTo(CurrentRotation, TargetRotation, DeltaTime, 1000.f);
		SetActorRotation(NextRotation);

		//Check navmesh & move
		FVector NextLocation = GetActorLocation() + (GetActorForwardVector() * 15);
		NextLocation.Z -= mCapsule->GetScaledCapsuleHalfHeight();
		FNavLocation NavLocation;
		UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
		if (true == IsValid(NavSystem) &&
			true == NavSystem->ProjectPointToNavigation(NextLocation, NavLocation))
		{
			NavLocation.Location.Z += mCapsule->GetScaledCapsuleHalfHeight();
			mCapsule->SetRelativeLocation(NavLocation.Location);
		}
		else
		{
		}
	}

	//Slam Move logic
	if (true == IsSlamMove)
	{
		FNavLocation NavLocation;
		UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
		if (true == IsValid(NavSystem) &&
			true == NavSystem->ProjectPointToNavigation(mSlamDestination, NavLocation))
		{
			NavLocation.Location.Z += mCapsule->GetScaledCapsuleHalfHeight();
			FVector CurrentLocation = GetActorLocation();
			FVector NextLocation = FMath::VInterpTo(CurrentLocation, NavLocation.Location, DeltaTime, 3.f);
			mCapsule->SetRelativeLocation(NextLocation);
		}
		else
		{
		}
	}

	//Advanced Laser Move logic
	if (true == IsAdvancedLaserMove)
	{
		FVector TargetDirection = mTarget->GetActorLocation() - GetActorLocation();
		TargetDirection.Z = 0.0;
		FRotator TargetRotation = TargetDirection.Rotation();
		FRotator CurrentRotation = GetActorRotation();
		FRotator NextRotation = FMath::RInterpConstantTo(CurrentRotation, TargetRotation, DeltaTime, 500.f);
		SetActorRotation(NextRotation);
	}
}

void ABossZero::SetAIState(EAIState _eType)
{
	Super::SetAIState(_eType);

	switch (mState)
	{
		case EAIState::Idle:
			mServerAnimationType = EAIState::Idle;
		break;
		case EAIState::Walk:
			mServerAnimationType = EAIState::Walk;
		break;
		case EAIState::Run:
			mServerAnimationType = EAIState::Run;
		break;
		case EAIState::Attack:
			mServerAnimationType = EAIState::Attack;
		break;
		case EAIState::Death:
			mServerAnimationType = EAIState::Death;
		break;
		default:
			break;
	}

	if (true == IsValid(mAnimInstance))
	{
		mAnimInstance->ChangeAnimation(mServerAnimationType);
	}
}

void ABossZero::Attack()
{
	Super::Attack();

	SwitchAttackMode();
	OnRep_mServerAttackMode();

	//Play by AttackMode
	mAnimInstance->PlayAttackMontage();
	
}

void ABossZero::CallBack_Hit(float Damage)
{
	Super::CallBack_Hit(Damage);
}

void ABossZero::CallBack_HpZERO()
{
	Super::CallBack_HpZERO();

	SetAIState(EAIState::Death);
	
	ABossController* AIControl = GetController<ABossController>();
	if (true == IsValid(AIControl))
	{
		AIControl->GetBrainComponent()->StopLogic(TEXT("Death"));
	}

	float ClearTime = GetWorld()->GetTimerManager().GetTimerElapsed(mPlayTimer);
	GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Blue, FString::Printf(TEXT("***Clear Time : %fSec"), ClearTime));
}

void ABossZero::ResumeMontage()
{
	//if reference is nullptr, it will resume all active montage
	mAnimInstance->Montage_Resume(nullptr);
}

void ABossZero::Shooting()
{
	FVector SpawnLocation = mMesh->GetSocketLocation("Muzzle");
	FRotator SpawnRotation = mMesh->GetSocketRotation("Muzzle");
	AEnergyBall* EnergyBall = GetWorld()->SpawnActor<AEnergyBall>(mEnergyBallClass, SpawnLocation, SpawnRotation);
	EnergyBall->SetEnergyBallOwner(this);
}

void ABossZero::LaunchEnergyBall(AEnergyBall* EnergyBall)
{
	ABossController* BossController = GetController<ABossController>();
	// mTarget = BossController->GetTarget();
	// FVector TargetLocation = mTarget->GetActorLocation();
	FVector TargetLocation = BossController->GetBlackboardComponent()->GetValueAsVector("AttackPoint");
	
	FVector EnergyBallLocation = EnergyBall->GetActorLocation();

	FVector LaunchDirection = TargetLocation - EnergyBallLocation;
	
	EnergyBall->mMovement->SetVelocityInLocalSpace(LaunchDirection.GetSafeNormal() * mEnergyBallLaunchSpeed);
}

void ABossZero::Laser()
{
	mAnimInstance->Montage_Pause();

	mBeam->mBeamHead->Activate();
	//Bind Indicator_On_Finished & mBeamBody_On_Finished Function in UBeam Class
	mBeam->mBeamIndicator->Activate();
	
	UGameplayStatics::PlaySound2D(GetWorld(), mLaserChargeSound, 1, 1, 0,
			nullptr, this, false);
}

void ABossZero::Rolling()
{
	mCapsule->SetCanEverAffectNavigation(false);
	
	if (true == IsRollingMove) //When RollingMove end
	{
		mRollingHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		IsRollingMove = false;
	}
	else //When RollingMove start
	{
		mRollingHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		ABossController* BossController = GetController<ABossController>();
		mTarget = BossController->GetTarget();
		
		IsRollingMove = true;
		
		UGameplayStatics::PlaySound2D(GetWorld(), mRollingSound, 1, 1, 0,
			nullptr, this, false);
	}
}

void ABossZero::Slam()
{
	mCapsule->SetCanEverAffectNavigation(false);
	
	if (true == IsSlamMove) //When SlamMove end
	{
		mSlamHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		mSlamHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		IsSlamMove = false;

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), mSlamFX, mSlamHitBox->GetComponentLocation());
		UGameplayStatics::PlaySound2D(GetWorld(), mSlamSound, 1, 1, 0,
			nullptr, this, false);
	}
	else //When SlamMove start
	{
		ABossController* BossController = GetController<ABossController>();
		mTarget = BossController->GetTarget();
		ACharacter* TargetCharacter = Cast<ACharacter>(mTarget);
		// mSlamDestination = TargetCharacter->GetActorLocation();
		mSlamDestination = BossController->GetBlackboardComponent()->GetValueAsVector("AttackPoint");
		mSlamDestination.Z -= TargetCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		
		IsSlamMove = true;
	}
}

void ABossZero::Bomb()
{
	if (false == GetWorld()->GetTimerManager().IsTimerActive(mBombTimer)) //When Bomb start
	{
		GetWorld()->GetTimerManager().SetTimer(mBombTimer, this, &ABossZero::SpawnBomb, mBombTime, true);
	}
	else //When Bomb end
	{
		GetWorld()->GetTimerManager().ClearTimer(mBombTimer);
	}
}

void ABossZero::SpawnBomb()
{
	ABossController* BossController = GetController<ABossController>();
	mTarget = BossController->GetTarget();
	ACharacter* TargetCharacter = Cast<ACharacter>(mTarget);
	//FVector BombLocation = TargetCharacter->GetActorLocation();
	FVector BombLocation = BossController->GetBlackboardComponent()->GetValueAsVector("AttackPoint");
	BombLocation.Z -= TargetCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	ABomb* Bomb = GetWorld()->SpawnActor<ABomb>(mBombClass, BombLocation, FRotator::ZeroRotator);
	Bomb->SetBombOwner(this);
}

void ABossZero::AdvancedLaser()
{
	mAnimInstance->Montage_Pause();
	
	ABossController* BossController = GetController<ABossController>();
	mTarget = BossController->GetTarget();

	mBeam->mBeamHead->Activate();
	//Bind Indicator_On_Finished & mBeamBody_On_Finished Function in UBeam Class
	mBeam->mBeamIndicator->Activate();

	UGameplayStatics::PlaySound2D(GetWorld(), mLaserChargeSound, 1, 1, 0,
			nullptr, this, false);

	GetWorld()->GetTimerManager().SetTimer(mAdvancedLaserMoveTimer, [this]()
	{
		IsAdvancedLaserMove = false;
	}, mAdvancedLaserMovingTime, false);

	IsAdvancedLaserMove = true;
}

void ABossZero::SwitchAttackMode()
{
	int8 AttackModeIndex;
	ABossController* BossController = GetController<ABossController>();
	mTarget = BossController->GetTarget();
	float Distance = FVector::Dist(GetActorLocation(), mTarget->GetActorLocation());

	float CloseWeight = FMath::Lerp(100.0f, 0.0f, Distance / 5000.0f);
	float FarWeight = FMath::Lerp(0.0f, 100.0f, Distance / 5000.0f);
	float MidWeight = (Distance <= 2500.f) ? FMath::Lerp(0.0f, 100.0f, Distance / 2500.f) : FMath::Lerp(100.0f, 0.0f, (Distance - 2500.f) / 2500.f);
	float TotalWeight = CloseWeight + FarWeight + MidWeight;
	CloseProb = (CloseWeight / TotalWeight) * 100.0f;
	FarProb = (FarWeight / TotalWeight) * 100.0f;
	MidProb = (MidWeight / TotalWeight) * 100.0f;
	
	float RandomValue = FMath::RandRange(0.0f, 100.0f);
	if (RandomValue <= CloseProb)
	{
		AttackModeIndex = 2;
	}
	else if (RandomValue <= CloseProb + MidProb)
	{
		float ShootingSuccessRate = (ShootingAttemptCount > 0) ? static_cast<float>(ShootingSuccessCount) / ShootingAttemptCount : InitialHitRate;
		float SlamSuccessRate = (SlamAttemptCount > 0) ? static_cast<float>(SlamSuccessCount) / SlamAttemptCount : InitialHitRate;
		float BombSuccessRate = (BombAttemptCount > 0) ? static_cast<float>(BombSuccessCount) / BombAttemptCount : InitialHitRate;
		
		float ShootingHitRate = FMath::Lerp(InitialHitRate, ShootingSuccessRate, static_cast<float>(ShootingAttemptCount) / (ShootingAttemptCount + 10));
		float SlamHitRate =  FMath::Lerp(InitialHitRate, SlamSuccessRate, static_cast<float>(SlamAttemptCount) / (SlamAttemptCount + 10));
		float BombHitRate = FMath::Lerp(InitialHitRate, BombSuccessRate, static_cast<float>(BombAttemptCount) / (BombAttemptCount + 10));
		float TotalRate = ShootingHitRate + SlamHitRate + BombHitRate;

		ShootingProb = (ShootingHitRate / TotalRate) * 100.0f;
		SlamProb = (SlamHitRate / TotalRate) * 100.0f;
		BombProb = (BombHitRate / TotalRate) * 100.0f;

		float RandomValue2 = FMath::RandRange(0.0f, 100.0f);
		if (RandomValue2 <= ShootingProb)
		{
			AttackModeIndex = 0;
			++ShootingAttemptCount;
		}
		else if (RandomValue2 <= ShootingProb + SlamProb)
		{
			AttackModeIndex = 3;
			++SlamAttemptCount;
		}
		else
		{
			AttackModeIndex = 4;
			++BombAttemptCount;
		}
	}
	else
	{
		AttackModeIndex = 5;
	}
	
	mServerAttackMode = static_cast<EBossAttackMode>(AttackModeIndex);

	GEngine->ClearOnScreenDebugMessages();
	GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Red, FString::Printf(TEXT("Bomb Attack : %f%%"), BombProb*MidProb/100.f));
	GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Red, FString::Printf(TEXT("Slam Attack : %f%%"), SlamProb*MidProb/100.f));
	GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Red, FString::Printf(TEXT("Shooting Attack : %f%%"), ShootingProb*MidProb/100.f));
	GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Red, FString::Printf(TEXT("Laser Attack : %f%%"), FarProb));
	GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Red, FString::Printf(TEXT("Rolling Attack : %f%%"), CloseProb));
}

void ABossZero::OnBegineOverlap_Rolling(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FDamageEvent	DmgEvent;
	OtherActor->TakeDamage(GetAttack() / 10.f, DmgEvent, GetController(),this);

	ACharacter* Player = Cast<ACharacter>(OtherActor);
	if (true == IsValid(Player))
	{
		Player->LaunchCharacter((mCapsule->GetForwardVector() + mCapsule->GetUpVector()) * 900.0, true, true);
	}

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), mRollingHitParticle,
			OtherActor->GetActorLocation(), OtherActor->GetActorRotation(), true);
	UGameplayStatics::PlaySound2D(GetWorld(), mRollingHitSound, 1, 1, 0,
			nullptr, OtherActor, false);
}

void ABossZero::OnBegineOverlap_Slam(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FDamageEvent	DmgEvent;
	OtherActor->TakeDamage(GetAttack(), DmgEvent, GetController(),this);
	
	ACharacter* Player = Cast<ACharacter>(OtherActor);
	if (true == IsValid(Player))
	{
		Player->LaunchCharacter((mCapsule->GetForwardVector() + mCapsule->GetUpVector()) * 900.0, true, true);
	}

	SlamSuccessCount++;
}

void ABossZero::OnRep_mServerAttackMode()
{
	mAnimInstance->SetAttackMode(mServerAttackMode);
}
