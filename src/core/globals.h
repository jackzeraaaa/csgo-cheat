#pragma once
#include <cstdint>
class CEntity;
namespace globals
{
	inline CEntity* localPlayer = nullptr;
	// update the local player pointer
	void UpdateLocalPlayer() noexcept;

}

namespace features
{

	//gui features
	inline bool bhop = false;

	inline bool glow = false;
	inline float glowColorVisible[] = {1.f, 0.2f, 0.f, 1.f};
	inline float glowColorInvisible[] = { 0.f, 0.2f, 1.f, 1.f };

	inline bool radar = false;

	inline bool triggerbot = false;
	inline int triggerDelay = 0;

	inline bool chams = false;
	inline float chamsColorVisible[] = { 1.f, 0.f, 0.1f};
	inline float chamsColorInvisible[] = { 0.f, 1.f, 0.1f };

	inline bool thirdperson = false;
	inline int thirdpersonDistance = 150;

	inline bool noflash = false;

	inline bool fov = false;
	inline int customFov = 60;
	inline int customWorldFov = 90;

}