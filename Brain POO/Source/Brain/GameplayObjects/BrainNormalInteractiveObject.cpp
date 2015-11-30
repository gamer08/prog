// Fill out your copyright notice in the Description page of Project Settings.

#include "Brain.h"
#include "BrainNormalInteractiveObject.h"
#include "Components/StaticMeshComponent.h"
#include "BrainGameInstance.h"
#include "Engine.h"

ABrainNormalInteractiveObject::ABrainNormalInteractiveObject()
{

	PrimaryActorTick.bCanEverTick = true;
}

void ABrainNormalInteractiveObject::BeginPlay()
{
	Super::BeginPlay();

	int8 flags = (_canBeRotate ? EAction::ROTATE : 0)
		| (_canBeTranslate ? EAction::TRANSLATE : 0)
		| (_canBeScale ? EAction::SCALE : 0)
		| (_canBeShear ? EAction::SHEAR : 0);
	_actions = FObjectAction(flags);

	// Init Rotate
	_currentRotation = GetActorRotation();
	_targetRotation = GetActorRotation();

	// Init Translate
	_currentTranslation = GetActorLocation();
	_targetTranslation = GetActorLocation();

	// Init Scale
	_targetScale = _initScale;
	_currentScale = _initScale;

	// Init Shear
	_currentShearFirstAxis = 0;
	_currentShearSecondAxis = 0;
	_targetShearFirstAxis = 0;
	_targetShearSecondAxis = 0;

	_cachedTransform = GetTransform();

	if (this->GetClass()->ImplementsInterface(UBrainSaveInterface::StaticClass()))
		Load();
}

void ABrainNormalInteractiveObject::PerformAction1()
{
	if (_canBeRotate)
		ChangeRotation(1);
}

void ABrainNormalInteractiveObject::PerformAction2()
{
	if (_canBeRotate)
		ChangeRotation(-1);
}

void ABrainNormalInteractiveObject::PerformAction3()
{
	if (_canBeTranslate)
		ChangePosition(1);
}

void ABrainNormalInteractiveObject::PerformAction4()
{
	if (_canBeTranslate)
		ChangePosition(-1);
}

void ABrainNormalInteractiveObject::PerformAction5()
{
	if (_canBeScale)
		ChangeScale(1);
}

void ABrainNormalInteractiveObject::PerformAction6()
{
	if (_canBeScale)
		ChangeScale(-1);
}

void ABrainNormalInteractiveObject::PerformAction7()
{
	if (_canBeShear)
		ChangeShear(1);
}

void ABrainNormalInteractiveObject::PerformAction8()
{
	if (_canBeShear)
		ChangeShear(-1);
}

void ABrainNormalInteractiveObject::ChangeRotation(int32 orientation)
{	
	_targetRotation = _currentRotation + (orientation * _rotationStep);
	_durationRotation = 0;
	_deltaRotation = _targetRotation - _currentRotation;

}

void ABrainNormalInteractiveObject::ChangePosition(int32 orientation)
{
	_targetTranslation = _currentTranslation + (orientation * _translationStep);
	_durationTranslation = 0;
	_deltaTranslation = _targetTranslation - _currentTranslation;
}

void ABrainNormalInteractiveObject::ChangeScale(int32 orientation)
{
	_targetScale = _currentScale + (orientation * _scaleStep);
	_durationScale = 0;

	if (_targetScale.Size() < _minScale.Size())
		_targetScale = _minScale;
	else if (_targetScale.Size() > _maxScale.Size())
		_targetScale = _maxScale;
}

void ABrainNormalInteractiveObject::ChangeShear(int32 orientation)
{
	_targetShearFirstAxis = _currentShearFirstAxis + (orientation * _firstAxisStep);
	_targetShearSecondAxis = _currentShearSecondAxis + (orientation * _secondAxisStep);
	_durationShear = 0;

	_deltaShearFirstAxis = _targetShearFirstAxis - _currentShearFirstAxis;
	_deltaShearSecondAxis = _targetShearSecondAxis - _currentShearSecondAxis;

}

FMatrix ABrainNormalInteractiveObject::Shear(float firstAxis, float secondAxis)
{
	FMatrix shearMatrix;
	switch (_shearPlan)
	{
		case ShearPlan::XY:
			shearMatrix = FMatrix(FVector(1, 0, 0),
									FVector(0, 1, 0),
									FVector(firstAxis, secondAxis, 1), 
									FVector(0, 0, 0));
			break;

		case ShearPlan::XZ:
			shearMatrix = FMatrix(FVector(1, 0, 0),
									FVector(firstAxis, 1, secondAxis), 
									FVector(0, 0, 1),
									FVector(0, 0, 0));
			break;

		case ShearPlan::YZ:
			shearMatrix = FMatrix(FVector(1, firstAxis, secondAxis),
									FVector(0, 1, 0),
									FVector(0, 0, 1),
									FVector(0, 0, 0));
			break;

		default:
			shearMatrix = FMatrix(FVector(1, 0, 0),
									FVector(0, 1, 0),
									FVector(0, 0, 1),
									FVector(0, 0, 0));
			break;
	}

	return shearMatrix;
}

void ABrainNormalInteractiveObject::Save(FBrainSaveData& saveData)
{
	FBrainNIOSaveData dataToSave;

	dataToSave._loadFromfile = true;
	dataToSave._location = GetActorLocation();
	dataToSave._rotation = GetActorRotation();
	dataToSave._scale = GetActorScale();
	dataToSave._shearFirstAxis = _currentShearFirstAxis;
	dataToSave._shearSecondAxis = _currentShearSecondAxis;

	saveData.AddDataToSave(GetName(),dataToSave);
}

void ABrainNormalInteractiveObject::Load()
{
	FString name = GetName();
	FBrainNIOSaveData savedData = Cast<UBrainGameInstance>(GetGameInstance())->GetSaveManager()->GetDataFromSave<FBrainNIOSaveData>(name);
	if (savedData._loadFromfile)
	{
		// Load Translation
		SetActorLocation(savedData._location);
		_currentTranslation = savedData._location;
		_targetTranslation = savedData._location;

		SetActorRotation(savedData._rotation);
		_currentRotation = savedData._rotation;
		_targetRotation = savedData._rotation;

		SetActorScale3D(savedData._scale);
		_currentScale = savedData._scale;
		_targetScale = savedData._scale;

		_cachedTransform = GetTransform();
		SetActorTransform(FTransform(Shear(savedData._shearFirstAxis, savedData._shearSecondAxis))*_cachedTransform);
	}
}

void ABrainNormalInteractiveObject::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (_canBeRotate)
	{
		_currentRotation += (_deltaRotation * (deltaTime / _animDuration));
		_durationRotation += deltaTime;
		if (_durationRotation > _animDuration)
		{
			_currentRotation = _targetRotation;
			_deltaRotation = FRotator(0, 0, 0); // Annulation du deltaRotation
		}

		SetActorRotation(_currentRotation);
	}

	if (_canBeTranslate)
	{
		_currentTranslation += (_deltaTranslation * deltaTime / _animDuration);
		_durationTranslation += deltaTime;
		if (_durationTranslation > _animDuration)
		{
			_currentTranslation = _targetTranslation;
			_deltaTranslation = FVector(0, 0, 0); // Annulation du deltaSize
		}

		SetActorLocation(_currentTranslation,true);
	}

	if (_canBeScale)
	{
		_currentScale += (_deltaScale * deltaTime / _animDuration);
		_durationScale += deltaTime;
		if (_durationScale > _animDuration)
		{
			_currentScale = _targetScale;
			_deltaScale = FVector(0,0,0); // Annulation du deltaSize
		}

		SetActorScale3D(_currentScale);
	}

	if (_canBeShear)
	{
		_currentShearFirstAxis += (_deltaShearFirstAxis * deltaTime / _animDuration);
		_currentShearSecondAxis += (_deltaShearSecondAxis * deltaTime / _animDuration);
		_durationShear += deltaTime;
		if (_durationShear > _animDuration)
		{
			_currentShearFirstAxis = _targetShearFirstAxis;
			_currentShearSecondAxis = _targetShearSecondAxis;
			_deltaShearFirstAxis = 0;
			_deltaShearSecondAxis = 0;
		}

		FTransform sTrans = FTransform(Shear(_currentShearFirstAxis, _currentShearSecondAxis));
		SetActorTransform(sTrans*_cachedTransform);
	}
}
