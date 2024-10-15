// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"


UCLASS()
class TPSPROJECT_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// �浹ü, �ܰ�, �߻�ü �̵�
	UPROPERTY(EditAnywhere)
	class USphereComponent* CollisionComp;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere)
	class UProjectileMovementComponent* MoveComp;

	//�Ѿ� ���� �Լ�
	void Die();

	//void SetTimer()
	//{
	//	FTimerHandle& InOutHandle; // ����� �˶� �ð�
	//	UserClass* InObj		  // �˸� ó���� ���� �ִ� ��ü
	//	typename FTimerDelegate::
	//}

};
