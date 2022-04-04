#pragma once
#include "interfaces.h"
#include "../gui.h"
#include "../valve/cmatrix.h"
#include "../valve/baseclientdll.h"

namespace hooks
{
	// call once to emplace all hooks
	void Setup() noexcept;

	// call once to restore all hooks
	void Destroy() noexcept;

	//d3dx9 stuff
	constexpr void* VirtualFunction(void* thisptr, size_t index)
	{
		return (*static_cast<void***>(thisptr))[index];
	}



	// bypass return address checks (thx osiris)
	using AllocKeyValuesMemoryFn = void* (__thiscall*)(void*, const std::int32_t) noexcept;
	inline AllocKeyValuesMemoryFn AllocKeyValuesMemoryOriginal;
	void* __stdcall AllocKeyValuesMemory(const std::int32_t size) noexcept;

	// example CreateMove hook
	//original createmove:::
	using CreateMoveFn = bool(__thiscall*)(IClientModeShared*, float, CUserCmd*) noexcept;
	inline CreateMoveFn CreateMoveOriginal = nullptr;
	//our hooked function:
	bool __stdcall CreateMove(float frameTime, CUserCmd* cmd) noexcept;

	// DrawModel hook

	using DrawModelFn = void(__thiscall*)(
		void*,
		void*,
		const CDrawModelInfo&,
		CMatrix3x4*,
		float*,
		float*,
		const CVector&,
		const std::int32_t
	) noexcept;

	// original function pointer
	inline DrawModelFn DrawModelOriginal = nullptr;

	//Hooked DrawModel definition
	void __stdcall DrawModel(
		void* results,
		const CDrawModelInfo& info,
		CMatrix3x4* bones,
		float* flexWeights,
		float* flexDelayedWeights,
		const CVector& modelOrigin,
		const std::int32_t flags
	) noexcept;

	// Do Post Screen Space hook
	using DoPostScreenSpaceEffectsFn = int(__thiscall*)(void*, const ViewSetup*);
	inline DoPostScreenSpaceEffectsFn DoPostScreenSpaceEffectsOriginal = nullptr;
	int __stdcall DoPostScreenSpaceEffects(const ViewSetup* view);

	//D3D9 hook
	using EndSceneFn = long(__thiscall*)(void*, IDirect3DDevice9*) noexcept;
	inline EndSceneFn EndSceneOriginal = nullptr;
	long __stdcall EndScene(IDirect3DDevice9* device) noexcept;

	//Reset hook
	using ResetFn = HRESULT(__thiscall*)(void*, IDirect3DDevice9*, D3DPRESENT_PARAMETERS*) noexcept;
	inline ResetFn ResetOriginal = nullptr;
	HRESULT __stdcall Reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) noexcept;

	//PaintTraverse hook
	using PaintTraverseFn = void(__thiscall*)(void*, unsigned int, bool, bool);
	inline PaintTraverseFn PaintTraverseOriginal = { nullptr };
	void __stdcall PaintTraverse(VPANEL panel, bool forceRepaint, bool allowForce);

	//FrameStageNotify hook
	using FrameStageNotifyFn = void(__thiscall*)(void*, ClientFrameStage);
	inline FrameStageNotifyFn FrameStageNotifyOriginal = { nullptr };
	void __stdcall FrameStageNotify(ClientFrameStage stage);

	//OverrideView hook
	using OverrideViewFn = void(__thiscall*)(void*, ViewSetup*);
	inline OverrideViewFn OverrideViewOriginal = { nullptr };
	void __stdcall OverrideView(ViewSetup* setup);

	//GetViewModelFov hook
	using GetViewModelFOVFn = float(__thiscall*)(void*);
	inline GetViewModelFOVFn GetViewModelFOVOriginal = { nullptr };
	float __stdcall GetViewModelFOV();
}
