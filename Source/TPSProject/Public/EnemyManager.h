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
	// ������ �ð� ������ �ּҰ�
	UPROPERTY(EditAnywhere, Category = SpawnSetting)
	float MinTime = 1;
	// ������ �ð� ������ �ִ밪
	UPROPERTY(EditAnywhere,Category = SpawnSetting)
	float MaxTime = 5;
	// ������ ��ġ ���� �迭
	UPROPERTY(EditAnywhere,Category = SpawnSetting)
	TArray<class AActor*> SpawnPoints;
	// AEnemy Ÿ���� ��������Ʈ�� �Ҵ� ���� ����
	UPROPERTY(EditAnywhere,Category = SpawnSetting)
	TSubclassOf<class AEnemy>EnemyFactory;

	// ���� ������ ���� Ÿ�̸�
	FTimerHandle SpawnTimerHandle;
	void CreateEnemy();

	// ������ ��ġ�� ã�� ���� �Ҵ�
	void FindSpawnPoints();

};