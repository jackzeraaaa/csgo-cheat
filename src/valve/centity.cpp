#include "../valve/centity.h"
#include "windows.h"
#include "../core/interfaces.h"
#include "../util/offsets.h"

CMatrix3x4* CEntity::GetBoneMatrix()
{
	CMatrix3x4 matrix[256];

	if (this->SetupBones(matrix, 128, 256, interfaces::globals->currentTime))
		return matrix;
	else
		return nullptr;
}

CVector CEntity::GetBonePosition(int bone)
{
	CMatrix3x4 matrix[256];

	if (this->SetupBones(matrix, 128, 256, 0.0f))
		return matrix[bone].at(3);
	else
		return { };
}