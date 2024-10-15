// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyManager.generated.h"

UCLASS()
class TPSPROJECT_API AEnemyManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// 랜덤한 시간 간경의 최소값
	UPROPERTY(EditAnywhere, Category = SpawnSetting)
	float MinTime = 1;
	// 랜덤한 시간 간격의 최대값
	UPROPERTY(EditAnywhere,Category = SpawnSetting)
	float MaxTime = 5;
	// 스폰할 위치 정보 배열
	UPROPERTY(EditAnywhere,Category = SpawnSetting)
	TArray<class AActor*> SpawnPoints;
	// AEnemy 타입의 블루프린트를 할당 받을 변수
	UPROPERTY(EditAnywhere,Category = SpawnSetting)
	TSubclassOf<class AEnemy>EnemyFactory;

	// 스폰 생성을 위한 타이머
	FTimerHandle SpawnTimerHandle;
	void CreateEnemy();

	// 스폰할 위치를 찾아 동적 할당
	void FindSpawnPoints();

};
