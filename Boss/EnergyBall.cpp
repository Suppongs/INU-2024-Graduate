// Fill out your copyright notice in the Description page of Project Settings.


#include "EnergyBall.h"
#include "BossZero.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Components/SphereComponent.h"
#include "Crazy6/Global/ProfileInfo.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/ProjectileMovementComponent.h"

AEnergyBall::AEnergyBall()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// <======= HitBox Setting =======>
	mEnergyBallHitBox = CreateDefaultSubobject<USphereComponent>(TEXT("EnergyBallHitBox"));
	SetRootComponent(mEnergyBallHitBox);
	mEnergyBallHitBox->SetSphereRadius(100.f);
	mEnergyBallHitBox->SetCollisionProfileName(UE_COLLISION_PROFILE_ENEMYBULLET);

	mEnergyBallHitBox->SetCanEverAffectNavigation(false);

	// <======= Visual Setting =======>
	mEnergyBallVisual = CreateDefaultSubobject<UNiagaraComponent>(TEXT("EnergyBallVisual"));
	mEnergyBallVisual->SetupAttachment(mEnergyBallHitBox);
	mEnergyBallVisual->SetRelativeScale3D(FVector(1.0, 1.0, 1.0));

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>
			VisualNiagaraSystem(TEXT("/Script/Niagara.NiagaraSystem'/Game/Dev/AI/Enemy/Boss/EnergyBallFX/VFX/LightningShield/N_LightningShield.N_LightningShield'"));
	if (true == VisualNiagaraSystem.Succeeded())
	{
		mEnergyBallVisual->SetAsset(VisualNiagaraSystem.Object);
	}

	// <======= Movement Setting =======>
	mMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	mMovement->SetUpdatedComponent(mEnergyBallHitBox);
	mMovement->InitialSpeed = 5000.f;
	mMovement->ProjectileGravityScale = 10.f;

	// <======= EnergyBall Overlap Setting =======>
	mEnergyBallHitBox->OnComponentBeginOverlap.AddDynamic(this, &AEnergyBall::OnBegineOverlap_EnergyBall);
	mMovement->OnProjectileStop.AddDynamic(this, &AEnergyBall::OnProJectileStop_EnergyBall);
	
	OnEnergyBallLaunch.BindLambda([&]()
		{
			mOwner->LaunchEnergyBall(this);
		});

	// <======= FX Setting =======>
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>
			ExplosionNiagaraSystem(TEXT("/Script/Niagara.NiagaraSystem'/Game/Dev/AI/Enemy/Boss/EnergyBallFX/VFX/LightningExplosion/N_LightningBlastCustom.N_LightningBlastCustom'"));
	if (true == ExplosionNiagaraSystem.Succeeded())
	{
		mExplosionEffect = ExplosionNiagaraSystem.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<USoundBase>
		BallSpawnSoundBase(TEXT("/Script/Engine.SoundWave'/Game/Dev/AI/Enemy/Boss/Sound/EnergyBall_Launch.EnergyBall_Launch'"));
	if (BallSpawnSoundBase.Succeeded())
	{
		mSpawnSound = BallSpawnSoundBase.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<USoundBase>
		ExplosionSoundBase(TEXT("/Script/Engine.SoundWave'/Game/Dev/AI/Enemy/Boss/Sound/EnergyBall_Explosion.EnergyBall_Explosion'"));
	if (ExplosionSoundBase.Succeeded())
	{
		mExplosionSound = ExplosionSoundBase.Object;
	}
}

void AEnergyBall::BeginPlay()
{
	Super::BeginPlay();

	SetReplicateMovement(true);

	mAccumulatedRange = mDefaultRange;
	mPrevLocation = GetActorLocation();

	UGameplayStatics::PlaySound2D(GetWorld(), mSpawnSound, 1, 1, 0,
			nullptr, this, false);
}

void AEnergyBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Range Calculate
	mCurLocation = GetActorLocation();
	FVector Dir = mCurLocation - mPrevLocation;
	float Distance = Dir.Length();
	mAccumulatedRange -= Distance;
	if (mAccumulatedRange <= 0.f)
	{
		Destroy();
	}
	mPrevLocation = mCurLocation;

	//Launch Setting
	if (false == IsLaunched)
	{
		if (mMovement->Velocity.Z < 0.f)
		{
			mMovement->ProjectileGravityScale = 0.f;
			SetActorRotation(FRotator::ZeroRotator);
			OnEnergyBallLaunch.ExecuteIfBound();

			IsLaunched = true;
		}
	}
	
}

void AEnergyBall::OnBegineOverlap_EnergyBall(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FDamageEvent DmgEvent;
	OtherActor->TakeDamage(mOwner->GetAttack(), DmgEvent, mOwner->GetController(), this);

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), mExplosionEffect, this->GetActorLocation());

	UGameplayStatics::PlaySound2D(GetWorld(), mExplosionSound, 1, 1, 0,
			nullptr, this, false);

	mOwner->ShootingSuccessCount++;
	Destroy();
}

void AEnergyBall::OnProJectileStop_EnergyBall(const FHitResult& HitResult)
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), mExplosionEffect, HitResult.Location);
	
	UGameplayStatics::PlaySound2D(GetWorld(), mExplosionSound, 1, 1, 0,
			nullptr, this, false);

	Destroy();
}

