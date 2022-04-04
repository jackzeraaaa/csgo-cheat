#pragma once
#include "../core/interfaces.h"
#include "../valve/ienginetrace.h"
#include "../valve/masks.h"
#include "../valve/cmatrix.h"

#include "../core/netvars.h"

#include "cclientclass.h"

class ViewSetup
{
public:
	int			x;
	int			unscaledX;
	int			y;
	int			unscaledY;
	int			width;
	int			unscaledWidth;
	int			height;
	int			unscaledHeight;
	bool		ortho;
	float		orthoLeft;
	float		orthoTop;
	float		orthoRight;
	float		orthoBottom;
	std::byte	pad0[0x7C];
	float		fov;
	float		viewModelFOV;
	CVector		origin;
	CVector		view;
	float		nearZ;
	float		farZ;
	float		nearViewmodelZ;
	float		farViewmodelZ;
	float		aspectRatio;
	float		nearBlurDepth;
	float		nearFocusDepth;
	float		farFocusDepth;
	float		farBlurDepth;
	float		nearBlurRadius;
	float		farBlurRadius;
	float		doFQuality;
	int			motionBlurMode;
	float		shutterTime;
	CVector		shutterOpenPosition;
	CVector		shutterOpenAngles;
	CVector		shutterClosePosition;
	CVector		shutterCloseAngles;
	float		offCenterTop;
	float		offCenterBottom;
	float		offCenterLeft;
	float		offCenterRight;
	bool		offCenter : 1;
	bool		renderToSubrectOfLargerScreen : 1;
	bool		bloomAndToneMapping : 1;
	bool		depthOfField : 1;
	bool		HDRTarget : 1;
	bool		drawWorldNormal : 1;
	bool		cullFontFaces : 1;
	bool		cacheFullSceneState : 1;
	bool		CSMView : 1;
};

class CEntity;
class IClientUnknown
{
public:
	constexpr CEntity* GetBaseEntity() noexcept
	{
		return memory::Call<CEntity*>(this, 7);
	}
};

class IClientRenderable
{
public:
	constexpr IClientUnknown* GetIClientUnknown() noexcept
	{
		return memory::Call<IClientUnknown*>(this, 0);
	}
};

class CModel;
class CEntity
{
public:
	enum EFlags : std::int32_t
	{
		FL_ONGROUND = (1 << 0),
		FL_DUCKING = (1 << 1),
		FL_ANIMDUCKING = (1 << 2),
		FL_WATERJUMP = (1 << 3),
		FL_ONTRAIN = (1 << 4),
		FL_INRAIN = (1 << 5),
		FL_FROZEN = (1 << 6),
		FL_ATCONTROLS = (1 << 7),
		FL_CLIENT = (1 << 8),
		FL_FAKECLIENT = (1 << 9),
		FL_INWATER = (1 << 10),
		FL_FLY = (1 << 11),
		FL_SWIM = (1 << 12),
		FL_CONVEYOR = (1 << 13),
		FL_NPC = (1 << 14),
		FL_GODMODE = (1 << 15),
		FL_NOTARGET = (1 << 16),
		FL_AIMTARGET = (1 << 17),
		FL_PARTIALGROUND = (1 << 18),
		FL_STATICPROP = (1 << 19),
		FL_GRAPHED = (1 << 20),
		FL_GRENADE = (1 << 21),
		FL_STEPMOVEMENT = (1 << 22),
		FL_DONTTOUCH = (1 << 23),
		FL_BASEVELOCITY = (1 << 24),
		FL_WORLDBRUSH = (1 << 25),
		FL_OBJECT = (1 << 26),
		FL_KILLME = (1 << 27),
		FL_ONFIRE = (1 << 28),
		FL_DISSOLVING = (1 << 29),
		FL_TRANSRAGDOLL = (1 << 30),
		FL_UNBLOCKABLE_BY_PLAYER = (1 << 31)
	};

public: // netvars
	NETVAR(GetFlags, "CBasePlayer->m_fFlags", std::int32_t);
	NETVAR(Spotted, "CBaseEntity->m_bSpotted", bool);
	NETVAR(Thirdperson, "CBasePlayer->m_iObserverMode", std::int16_t);
	NETVAR(FlashDuration, "CCSPlayer->m_flFlashDuration", float);
	NETVAR(IsScoped, "CCSPlayer->m_bIsScoped", bool);
	NETVAR(IsDefusing, "CCSPlayer->m_bIsDefusing", bool);
	NETVAR(HasGunGameImmunity, "CCSPlayer->m_bGunGameImmunity", bool);
	NETVAR(Account, "CCSPlayer->m_iAccount", int);
	NETVAR(InBombZone, "CCSPlayer->m_bInBombZone", bool);
	NETVAR(HasDefuser, "CCSPlayer->m_bHasDefuser", bool);
	NETVAR(HasHelmet, "CCSPlayer->m_bHasHelmet", bool);

public: // renderable virtual functions (+0x4)
	constexpr CModel* GetModel() noexcept
	{
		return memory::Call<CModel*>(this + 0x4, 8);
	}

	constexpr bool SetupBones(CMatrix3x4* out, std::int32_t max, std::int32_t mask, float currentTime) noexcept
	{
		return memory::Call<bool>(this + 0x4, 13, out, max, mask, currentTime);
	}

public: // networkable virtual functions (+0x8)
	constexpr CClientClass* GetClientClass() noexcept
	{
		return memory::Call<CClientClass*>(this + 0x8, 2);
	}

	constexpr bool IsDormant() noexcept
	{
		return memory::Call<bool>(this + 0x8, 9);
	}

	constexpr std::int32_t GetIndex() noexcept
	{
		return memory::Call<std::int32_t>(this + 0x8, 10);
	}

public: // entity virtual functions
	constexpr const CVector& GetAbsOrigin() noexcept
	{
		return memory::Call<const CVector&>(this, 10);
	}

	constexpr void GetEyePosition(CVector& eyePositionOut) noexcept {
		memory::Call<void, CVector&>(this, 285, eyePositionOut);
	}

	constexpr std::int32_t GetTeam() noexcept 
	{
		return memory::Call<std::int32_t>(this, 88);
	}

	constexpr std::int32_t GetHealth() noexcept
	{
		return memory::Call<std::int32_t>(this, 122);
	}

	constexpr bool IsAlive() noexcept 
	{
		return memory::Call<bool>(this, 156);
	}

	constexpr bool IsPlayer() noexcept 
	{
		return memory::Call<bool>(this, 158);
	}
	bool IsWeapon()
	{
		return memory::Call<bool>(this, 166);
	}
	constexpr void GetAimPunch(CVector& aimPunchOut) noexcept {
		memory::Call<void, CVector&>(this, 346, aimPunchOut);
	}

	void UpdateVisibilityAllEntities()
	{
		static uintptr_t* update_visibility_all_entities = nullptr;
		if (update_visibility_all_entities == nullptr) {
			//static auto relative_call = (std::uintptr_t*)(memory::Scan("client.dll", E8 ? ? ? ? 83 7D D8 00 7C 0F));

			//static auto offset = *(uintptr_t*)(relative_call + 0x1);
			//auto update_visibility_all_entities = (uintptr_t*)(relative_call + 5 + offset);

			static auto callInstruction = memory::PatternScan("client.dll", "E8 ? ? ? ? 83 7D D8 00 7C 0F"); // get the instruction address
			static auto relativeAddress = *(std::uintptr_t*)(callInstruction + 1); // read the rel32
			static auto nextInstruction = callInstruction + 5; // get the address of next instruction
			update_visibility_all_entities = (std::uintptr_t*)(nextInstruction + relativeAddress); // our function address will be nextInstruction + relativeAddress
		}
		else
			reinterpret_cast<void(__thiscall*)(void*)>(update_visibility_all_entities)(this);
	}


	CMatrix3x4* GetBoneMatrix();
	CVector GetBonePosition(int bone);

};
