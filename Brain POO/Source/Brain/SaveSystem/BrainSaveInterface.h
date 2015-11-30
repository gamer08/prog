#pragma once
#include "BrainSaveData.h"
#include "BrainSaveInterface.generated.h"

UINTERFACE(MinimalAPI)
class UBrainSaveInterface : public UInterface
{
    GENERATED_UINTERFACE_BODY()
};

class IBrainSaveInterface
{    GENERATED_IINTERFACE_BODY()

protected:
	virtual void Load() = 0;

public:
	virtual void Save(FBrainSaveData& saveData) = 0;
};