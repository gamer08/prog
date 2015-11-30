#pragma once 

#include "Brain.h"
#include "BrainNIOSaveData.generated.h"

USTRUCT()
struct FBrainNIOSaveData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FVector _location;
	
	UPROPERTY()
	FRotator _rotation;

	UPROPERTY()
	FVector _scale;

	UPROPERTY()
	float _shearFirstAxis;

	UPROPERTY()
	float _shearSecondAxis;

	UPROPERTY()
	bool _loadFromfile;

	FBrainNIOSaveData()
	{
		_location = FVector();
		_rotation = FRotator();
		_scale = FVector();
		_shearFirstAxis = 0;
		_shearSecondAxis = 0;

		_loadFromfile = false;
	}
};

FORCEINLINE FArchive &operator << (FArchive &archive, FBrainNIOSaveData& data)
{
	archive << data._location;
	archive << data._rotation;
	archive << data._scale;
	archive << data._shearFirstAxis;
	archive << data._shearSecondAxis;
	archive << data._loadFromfile;

	return archive;
}