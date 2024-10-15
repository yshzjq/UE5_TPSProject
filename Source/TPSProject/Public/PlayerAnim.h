// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnim.generated.h"

/**
 * 
 */
UCLASS()
class TPSPROJECT_API UPlayerAnim : public UAnimInstance
{
	GENERATED_BODY()
public:
	// �� ������ ���ŵǴ� �Լ�
	virtual void NativeUpdateAnimation(float Deltatime) override;

	UPROPERTY()
	class ATPSPlayer* Player;
	
public:
	// �̵� �ӵ�
	// �յڷ� �����̴� �ӷ�
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	float Speed = 0;
	// �¿�� �����̴� �ӷ�
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = PlayerAnim)
	float Direction;
	UPROPERTY(EditDefaultsOnly, BLueprintReadwrite,Category = PlayerAnim)
	bool IsInAir;

	UPROPERTY(EditDefaultsOnly,Category = PlayerAnim)
	class UAnimMontage* AttackAnimMontage;
	// ���� �ִϸ��̼� �Լ�
	void PlayAttackAnim();
};
