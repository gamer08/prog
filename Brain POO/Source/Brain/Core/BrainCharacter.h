// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "GameplayObjects/BrainInteractiveObject.h"
#include "SaveSystem/BrainSaveInterface.h"
#include "BrainCharacter.generated.h"


UCLASS(config=Game)
class ABrainCharacter : public ACharacter, public IBrainSaveInterface
{
	friend class ABrainPlayerController;
	GENERATED_BODY()

public:
	ABrainCharacter();

	// declaration du type de d�l�gate. Un d�l�gate = pointeur de fonction !
	typedef void(ABrainInteractiveObject::* ActionOnObjectFunction)(void);

	void Save(FBrainSaveData& saveData);

protected:

	void MoveForward(float value);

	void MoveSide(float value);

	void Turn(float value);

	void LookUp(float value);

	void Jump();

	void StopJumping();

	void PerformActionOnObject(int action);

private:

	// Array des d�l�gates de type fonction de Interactive Object
	TArray<ActionOnObjectFunction> _actionObjects;

	float _minPitch, _maxPitch;

	float _maxDistanceInteraction;

	UPROPERTY()
	ABrainInteractiveObject* _selectedObject;

	void Tick(float deltaTime) override;

	void BeginPlay() override;
	
	void InitActionOnObjectDelegate();

	void LimitPitch(FRotator& rotation, float minPitch, float maxPitch);
	
	ABrainInteractiveObject* CheckForInteractiveObjects();

	void Load();
};
