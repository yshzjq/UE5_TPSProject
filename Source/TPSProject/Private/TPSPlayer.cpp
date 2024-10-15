// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputSubSystems.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Bullet.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnemyFSM.h"
#include "PlayerAnim.h"

// Sets default values
ATPSPlayer::ATPSPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Spring Arm�� �����ؼ� Root �� ���δ�.
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	// Spring Arm -> Target Arm Length : 400
	SpringArmComp->TargetArmLength = 400;
	// ��ġ (X=0.000000,Y=0.000000,Z=70.000000)
	SpringArmComp->SetRelativeLocation(FVector(0, 20, 80));

	// Camera�� �����ؼ� SpringArm�� ���δ�.
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);
	
	// Mesh�� 3D ������ �ε��ؼ� �־��ش�.
	// ������ ����̸� �̿��ؼ� ���̷�Ż �޽��� �ε��Ѵ�.
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/ParagonWraith/Characters/Heroes/Wraith/Meshes/Wraith.Wraith'"));

	//����, �ε尡 �����ߴٸ�
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		// �޽ø� ���� ������ ĳ������ ��ġ ����� ȸ���� �ߴ�.
		// ��ġ (X=0.000000,Y=0.000000,Z=-90.000000)
		// ȸ�� (Pitch=0.000000,Yaw=-90.000000,Roll=0.000000)
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));

		bUseControllerRotationYaw = true;
		SpringArmComp->bUsePawnControlRotation = true;
		CameraComp->bUsePawnControlRotation = false;
		GetCharacterMovement()->bOrientRotationToMovement = false;

		JumpMaxCount = 2;
		GetCharacterMovement()->AirControl = 1;

		// 
		GetMesh()->SetupAttachment(GetMesh(), TEXT("FirePositon"));

	}

}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();

	// �ʱ� �ӵ��� �ȱ�� ����
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	// ������ �� �� ���� ������ �����Ѵ�.
	CrosshairUI = CreateWidget(GetWorld(), CrosshairUIfactory);
	SniperUI = CreateWidget(GetWorld(), SniperUIfactory);
	// �Ϲ� ���� ��� CrosshairUI ȭ�鿡 ǥ��
	CrosshairUI->AddToViewport();

	// ī�޶� �޴��� �Ҵ�
	CameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
	

	auto pc = Cast<APlayerController>(Controller);
	if (pc)
	{
		auto subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());

		if (subsystem)
		{
			subsystem->AddMappingContext(IMC_TPS, 0);
		}
	}
	
}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Move();
}

// Called to bind functionality to input
void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// �Է� �Լ����� ��� ����

	auto PlayerInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (PlayerInput)
	{
		PlayerInput->BindAction(IA_Turn, ETriggerEvent::Triggered, this, &ATPSPlayer::Turn);
		PlayerInput->BindAction(IA_LookUp, ETriggerEvent::Triggered, this, &ATPSPlayer::LookUp);
		PlayerInput->BindAction(IA_PlayerMove, ETriggerEvent::Triggered, this, &ATPSPlayer::PlayerMove);
		PlayerInput->BindAction(IA_Jump, ETriggerEvent::Triggered, this, &ATPSPlayer::InputJump);
		// �޸��� �ִϸ��̼�
		PlayerInput->BindAction(IA_Run, ETriggerEvent::Started, this, &ATPSPlayer::InputRun);
		PlayerInput->BindAction(IA_Run, ETriggerEvent::Completed, this, &ATPSPlayer::InputRun);


		PlayerInput->BindAction(IA_Fire, ETriggerEvent::Started, this, &ATPSPlayer::InputFire);
		PlayerInput->BindAction(IA_RifleMode, ETriggerEvent::Started, this, &ATPSPlayer::ChangeToRifle);
		PlayerInput->BindAction(IA_SniperMode, ETriggerEvent::Started, this, &ATPSPlayer::ChangeToSniper);
		// �������� ���� ���
		PlayerInput->BindAction(IA_SniperAim, ETriggerEvent::Started, this, &ATPSPlayer::SniperAim);
		PlayerInput->BindAction(IA_SniperAim, ETriggerEvent::Completed, this, &ATPSPlayer::SniperAim);
		

	}
}

void ATPSPlayer::Turn(const FInputActionValue& inputValue)
{
	float value = inputValue.Get<float>();
	AddControllerYawInput(value);
}

void ATPSPlayer::LookUp(const FInputActionValue& inputValue)
{
	float value = inputValue.Get<float>();
	AddControllerPitchInput(value);
}

void ATPSPlayer::PlayerMove(const FInputActionValue& inputValue)
{
	FVector2D value = inputValue.Get<FVector2D>();
	//���� �Է� ó��
	Direction.X = value.X;
	//�¿� �Է� ó��
	Direction.Y = value.Y;
}

void ATPSPlayer::InputRun()
{
	auto Movement = GetCharacterMovement();
	// ���� , ���� �޸��� �����
	if (Movement->MaxWalkSpeed > WalkSpeed)
	{
		// �ȱ� �ӵ��� ��ȯ
		Movement->MaxWalkSpeed = WalkSpeed;
	}
	else
	{
		Movement->MaxWalkSpeed = RunSpeed;
	}
}

void ATPSPlayer::InputJump(const FInputActionValue& inputValue)
{
	Jump();
}

void ATPSPlayer::Move()
{
	Direction = FTransform(GetControlRotation()).TransformVector(Direction);
	// ��� � ���� : P (�̵��� ��ġ) = P0 + (���� ��ġ) + V(�ӵ�*����) * T (�ð�)
	//FVector P0 = GetActorLocation;
	//FVector VT = Direction * walkSpeed * DeltaTime;
	AddMovementInput(FVector(Direction.X, Direction.Y, 0).GetSafeNormal());
	Direction = FVector::ZeroVector;
}

void ATPSPlayer::InputFire(const FInputActionValue& inputValue)
{
	// ī�޶� ����ũ ���
	CameraManager->StartCameraShake(CameraShake);

	auto Anim = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
	Anim->PlayAttackAnim();

	//����� ó��
	// �Ѿ��� ������ Transform�� ������
	FTransform FirePosition = GetMesh()->GetSocketTransform(TEXT("FirePosition"));
	
	// �װ��� �Ѿ��� �߻�Ǵ� ����Ʈ�� ����

	
	
	//����� ó��
	if (bUsingRifle)
	{
		//������ ����϶� �Ϲ� �Ѿ��� ���.
		GetWorld()->SpawnActor<ABullet>(BulletFactory, FirePosition);
		
	}
	else if(bUsingSniper)
	{
		//�������� ����϶�

		//LineTrace�� ���� ��ġ
		FVector StartPosition = CameraComp->GetComponentLocation();
		// LineTrace�� ���� ��ġ
		FVector EndPosition = CameraComp->GetComponentLocation() + CameraComp->GetForwardVector() * 100000;
		// LineTrace �� �浹 ������ ���� ����
		FHitResult HitInfo;
		// �浹 �ɼ� ���� ����
		FCollisionQueryParams Params;
		// �ڱ� �ڽ�(�÷��̾�)�� �浹���� ����
		Params.AddIgnoredActor(this);
		// Channel ���͸� �̿��� LineTrace �浹 ����
		// �浹 ����, ���� ��ġ, ���� ��ġ, ���� ä��, �浹 �ɼ�
		bool bHit = GetWorld()->LineTraceSingleByChannel(HitInfo, StartPosition, EndPosition, ECC_Visibility, Params);

		// �浹 ó�� -> �Ѿ� ���� ȿ�� ���
		if (bHit)
		{
			//�Ѿ� ���� ȿ�� Ʈ������
			FTransform BulletTransform;
			// �ε��� ��ġ �Ҵ�
			BulletTransform.SetLocation(HitInfo.ImpactPoint);
			BulletTransform.SetRotation(UKismetMathLibrary::Conv_VectorToQuaternion(HitInfo.ImpactNormal));
			// �Ѿ� ���� ȿ�� ��ü ����
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletHitEffectFactory,BulletTransform);
			
		}

		// �ε��� ��ü�� ������Ʈ�� ������ ����Ǿ� �ִٸ� ����������.
		auto HitComp = HitInfo.GetComponent();
		// 1. ���� ������Ʈ�� ������ ����Ǿ� �ִٸ�
		if (HitComp && HitComp->IsSimulatingPhysics())
		{
			// 2. ������ ������ �ʿ�
			FVector Dir = (EndPosition - StartPosition).GetSafeNormal();
			// �������� ����
			FVector force = Dir * HitComp->GetMass() * 500000;
			// 3. �� �������� ���������� �ʹ�.
			HitComp->AddForceAtLocation(force, HitInfo.ImpactPoint);
		}

		// �ε��� ����� ������ �Ǵ�
		auto Enemy = HitInfo.GetActor()->GetDefaultSubobjectByName(TEXT("EnemyFSM"));

		if (Enemy)
		{
			auto EnemyFSM = Cast<UEnemyFSM>(Enemy);
			EnemyFSM->OnDamageProcess();
		}
	}
	
}

void ATPSPlayer::ChangeToRifle(const FInputActionValue& inputValue)
{
	// ����(Hand Gun)���
	bUsingRifle = true;
}

void ATPSPlayer::ChangeToSniper(const FInputActionValue& inputValue)
{
	// ���� ���
	bUsingRifle = false;
	
}

void ATPSPlayer::SniperAim(const FInputActionValue& inputValue)
{
	// �������� ��尡 �ƴ� ��� ó������ �ʴ´�.
	if (bUsingRifle)
	{
		return;
	}
	// Pressed(Started) �Է�ó��
	if(bSniperAim == false)
	{
		// �������� ���� ��� Ȱ��ȭ
		bSniperAim = true;
		// �������� ���� UI ���
		SniperUI->AddToViewport();
		CameraComp->SetFieldOfView(45.0f);
		CrosshairUI->RemoveFromParent();
	}
	else //Released(Completed) �Է� ó��
	{
		bSniperAim = false;
		SniperUI->RemoveFromViewport();
		CameraComp->SetFieldOfView(90.f);
		CrosshairUI->AddToViewport();
	}

}





	