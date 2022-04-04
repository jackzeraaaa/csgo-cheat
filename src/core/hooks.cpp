#include "hooks.h"
// include minhook for epic hookage
#include "../../ext/minhook/minhook.h"
#include "../util/hash.h"

#include <iostream>
#include <intrin.h>
#include "../../ext/imgui/imgui_impl_dx9.h"
#include "../../ext/imgui/imgui_impl_win32.h"
#include "../../ext/imgui/imgui.h"
#include "../valve/masks.h"
#include "../valve/qangle.h"


void hooks::Setup() noexcept
{
	MH_Initialize();

	// AllocKeyValuesMemory hook
	MH_CreateHook(
		memory::Get(interfaces::keyValuesSystem, 1),
		&AllocKeyValuesMemory,
		reinterpret_cast<void**>(&AllocKeyValuesMemoryOriginal)
	);

	// CreateMove hook
	MH_CreateHook(
		memory::Get(interfaces::clientMode, 24),
		&CreateMove,
		reinterpret_cast<void**>(&CreateMoveOriginal)
	);
	MH_CreateHook(
		memory::Get(interfaces::clientMode, 44),
		&DoPostScreenSpaceEffects,
		reinterpret_cast<void**>(&DoPostScreenSpaceEffectsOriginal)
	);
	MH_CreateHook(
		memory::Get(interfaces::studioRender, 29),
		&DrawModel,
		reinterpret_cast<void**>(&DrawModelOriginal)
	);

	MH_CreateHook(
		memory::Get(interfaces::clientMode, 18),
		&OverrideView,
		reinterpret_cast<void**>(&OverrideViewOriginal)
	);

	MH_CreateHook(
		memory::Get(interfaces::client, 37),
		&FrameStageNotify,
		reinterpret_cast<void**>(&FrameStageNotifyOriginal)
	);

	MH_CreateHook(
		memory::Get(interfaces::panel, 41),
		&PaintTraverse,
		reinterpret_cast<void**>(&PaintTraverseOriginal)
	);
	MH_CreateHook(
		memory::Get(interfaces::clientMode, 35),
		&GetViewModelFOV,
		reinterpret_cast<void**>(&GetViewModelFOVOriginal)
	);

	MH_CreateHook(
		VirtualFunction(gui::device, 42),
		&EndScene,
		reinterpret_cast<void**>(&EndSceneOriginal)
	);
	MH_CreateHook(
		VirtualFunction(gui::device, 16),
		&Reset,
		reinterpret_cast<void**>(&ResetOriginal)
	);
	MH_EnableHook(MH_ALL_HOOKS);
	gui::DestroyDirectX();
}

void hooks::Destroy() noexcept
{
	// restore hooks
	MH_DisableHook(MH_ALL_HOOKS);
	MH_RemoveHook(MH_ALL_HOOKS);

	// uninit minhook
	MH_Uninitialize();
}

void* __stdcall hooks::AllocKeyValuesMemory(const std::int32_t size) noexcept
{
	// if function is returning to speficied addresses, return nullptr to "bypass"
	if (const std::uint32_t address = reinterpret_cast<std::uint32_t>(_ReturnAddress());
		address == reinterpret_cast<std::uint32_t>(memory::allocKeyValuesEngine) ||
		address == reinterpret_cast<std::uint32_t>(memory::allocKeyValuesClient)) 
		return nullptr;

	// return original
	return AllocKeyValuesMemoryOriginal(interfaces::keyValuesSystem, size);
}
bool firstTime = true; //holds when trigger should shoot
std::int32_t tickCount; //holds tickcount var
bool __stdcall hooks::CreateMove(float frameTime, CUserCmd* cmd) noexcept
{
	// make sure this function is being called from CInput::CreateMove
	if (!cmd->commandNumber)
		return CreateMoveOriginal(interfaces::clientMode, frameTime, cmd);

	// this would be done anyway by returning true
	if (CreateMoveOriginal(interfaces::clientMode, frameTime, cmd))
		interfaces::engine->SetViewAngles(cmd->viewAngles);

	// get our local player here
	globals::UpdateLocalPlayer();
	//bhop
	if (features::bhop)
	{
		if (!(globals::localPlayer->GetFlags() & CEntity::FL_ONGROUND))
			cmd->buttons &= ~CUserCmd::IN_JUMP;
	}
	
	// trigger bot
	if (features::triggerbot)
	{
		if (!GetAsyncKeyState(VK_XBUTTON1))
			return false;
		if (!globals::localPlayer || !globals::localPlayer->IsAlive())
			return false;

		CVector eyePosition;
		globals::localPlayer->GetEyePosition(eyePosition);
		CVector aimPunch;
		globals::localPlayer->GetAimPunch(aimPunch);

		//calculate the destination of ray:
		const CVector dst = eyePosition + CVector{ cmd->viewAngles + aimPunch }.ToVector() * 1100.f;
		//trace the ray from eye->dst
		CTrace trace;
		interfaces::trace->TraceRay({ eyePosition, dst }, 0x46004009, globals::localPlayer, trace);
		if (!trace.entity || !trace.entity->IsPlayer())
			return false;
		if (!trace.entity->IsAlive() && trace.entity->GetTeam() == globals::localPlayer->GetTeam())
			return false;
		if (firstTime == true) {
			tickCount = interfaces::globals->tickCount;
			firstTime = false;
		}
		//local player shoots
		if (interfaces::globals->tickCount - tickCount >= features::triggerDelay) {
			cmd->buttons |= CUserCmd::IN_ATTACK;
			tickCount = interfaces::globals->tickCount;
			firstTime = true;
		}
	}

	return false;
}

void __stdcall hooks::DrawModel(
	void* results,
	const CDrawModelInfo& info,
	CMatrix3x4* bones,
	float* flexWeights,
	float* flexDelayedWeights,
	const CVector& modelOrigin,
	const std::int32_t flags
) noexcept {
	//Chams
	if (features::chams)
	{
		//tendo certeza se o local player e o player que vai ser renderizado é valido:
		//caso contrario *crash* :/
		if (globals::localPlayer && info.renderable) {
			CEntity* entity = info.renderable->GetIClientUnknown()->GetBaseEntity();
			if (entity && entity->IsPlayer() && entity->GetTeam() != globals::localPlayer->GetTeam()) {
				if (interfaces::studioRender->IsForcedMaterialOverride())
					return DrawModelOriginal(interfaces::studioRender, results, info, bones, flexWeights, flexDelayedWeights, modelOrigin, flags);
				//material para sobrepor
				//obs: Static para não precisarmos encontrar o material toda vez que a função for chamada
				//"materials do not change"
				static IMaterial* material = interfaces::materialSystem->FindMaterial("debug/debugambientcube");
				//float para guardar as cores do chams
				float hidden[3] = { features::chamsColorInvisible[0], features::chamsColorInvisible[1] , features::chamsColorInvisible[2] };
				float visible[3] = { features::chamsColorVisible[0], features::chamsColorVisible[1] , features::chamsColorVisible[2] };
				//alpha modulate
				interfaces::studioRender->SetAlphaModulation(1.f);

				//mostrar através das paredes
				material->SetMaterialVarFlag(IMaterial::IGNOREZ, true);
				interfaces::studioRender->SetColorModulation(hidden);
				interfaces::studioRender->ForcedMaterialOverride(material);
				DrawModelOriginal(interfaces::studioRender, results, info, bones, flexWeights, flexDelayedWeights, modelOrigin, flags);
				//mostrar partes visiveis
				material->SetMaterialVarFlag(IMaterial::IGNOREZ, false);
				interfaces::studioRender->SetColorModulation(visible);
				interfaces::studioRender->ForcedMaterialOverride(material);
				DrawModelOriginal(interfaces::studioRender, results, info, bones, flexWeights, flexDelayedWeights, modelOrigin, flags);

				//return hook
				return interfaces::studioRender->ForcedMaterialOverride(nullptr);
			}
		}
	}
	//glow
	static bool shouldGlow = false;
	if (features::glow) {
		shouldGlow = true;
		for (int i = 0; i < interfaces::glow->glowObjects.size; i++) {
			auto& glow = interfaces::glow->glowObjects[i];
			if (!glow.entity)
				continue;
			const auto clientClass = glow.entity->GetClientClass();
			if (glow.IsUnused() || !glow.entity->IsAlive() || glow.entity->GetTeam() == globals::localPlayer->GetTeam())
				continue;
			
			CVector localPosition;
			localPosition = globals::localPlayer->GetBonePosition(8);
			if (clientClass->classID == ClassID::CCSPlayer) {
				CVector enemyPosition;
				enemyPosition = glow.entity->GetBonePosition(8);

				//trace the ray from eye->dst
				CTrace visibleTrace;
				interfaces::trace->TraceRay({ localPosition, enemyPosition }, 0x46004009, globals::localPlayer, visibleTrace);//crashe

				if (visibleTrace.entity && visibleTrace.entity->IsPlayer())
				{
					if (glow.entity == visibleTrace.entity) {
						glow.SetColor(features::glowColorVisible[0], features::glowColorVisible[1], features::glowColorVisible[2], features::glowColorVisible[3]);
						continue;
					}
				}
				else
				{
					glow.SetColor(features::glowColorInvisible[0], features::glowColorInvisible[1], features::glowColorInvisible[2], features::glowColorInvisible[3]);
				}
			}
			if (clientClass->classID == ClassID::CPlantedC4)
				glow.SetColor(0.f, 0.f, 0.f);
		}
	}
	else { shouldGlow = false; }

	if(shouldGlow = false) 
	{
		for (int i = 0; i < interfaces::glow->glowObjects.size; i++) {
			auto& glow = interfaces::glow->glowObjects[i];
			if (!glow.entity)
				continue;
			const auto clientClass = glow.entity->GetClientClass();
			if (glow.IsUnused() || !glow.entity->IsAlive() || glow.entity->GetTeam() == globals::localPlayer->GetTeam())
				continue;
			if (clientClass->classID == ClassID::CCSPlayer)
				glow.SetColor(0.f, 0.f, 0.f, 0.f);
			if (clientClass->classID == ClassID::CPlantedC4)
				glow.SetColor(0.f, 0.f, 0.f);
		}
	}
	shouldGlow = true;
	
	DrawModelOriginal(interfaces::studioRender, results, info, bones, flexWeights, flexDelayedWeights, modelOrigin, flags);
}

int __stdcall hooks::DoPostScreenSpaceEffects(const ViewSetup* view)
{
	//static unsigned int client = reinterpret_cast<std::uintptr_t>(GetModuleHandle("client.dll")); not necessary for now
	if (globals::localPlayer) {
		if (&features::radar)
		{
			for (int i = 1; i <= interfaces::globals->maxClients; i++) {
				const auto entity = interfaces::entityList->GetEntityFromIndex(i);
				if (!entity)
					continue;
				if (entity->GetTeam() == globals::localPlayer->GetTeam())
					continue;
				entity->Spotted() = true;
			}
		}
	}

	return DoPostScreenSpaceEffectsOriginal(interfaces::clientMode, view);
}

long __stdcall hooks::EndScene(IDirect3DDevice9* device) noexcept
{
	static const auto returnAddress = _ReturnAddress();
	const auto result = EndSceneOriginal(device, device);
	
	//stop endscene getting called twice
	if (_ReturnAddress() == returnAddress)
		return result;

	if (!gui::setup)
		gui::SetupMenu(device);
	if (gui::open) {
		gui::Render();
	}

	return false;
}

HRESULT __stdcall hooks::Reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) noexcept
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	const auto result = ResetOriginal(device, device, params);
	ImGui_ImplDX9_CreateDeviceObjects();
	return result;
}

void __stdcall hooks::PaintTraverse(VPANEL panel, bool forceRepaint, bool allowForce)
{
	// Find a better way of doing this? This doesn't need to be called each frame...
	if (hash::RunTime(interfaces::panel->GetName(panel)) == hash::CompileTime("FocusOverlayPanel"))
	{
		if (features::noflash && globals::localPlayer)
			globals::localPlayer->FlashDuration() = 0;
		//interfaces::inputsystem->EnableInput(!gui::open); //vou deixar as pessoas mexerem no menu com o insert aberto, acho q é melhor assim ;D
		interfaces::panel->SetMouseInputEnabled(panel, gui::open);
	}

	PaintTraverseOriginal(interfaces::panel, panel, forceRepaint, allowForce);
}

void __stdcall hooks::FrameStageNotify(ClientFrameStage stage) {

	if (stage != ClientFrameStage::FRAME_RENDER_START)
		return FrameStageNotifyOriginal(interfaces::client, stage);
	if (!globals::localPlayer)
		return FrameStageNotifyOriginal(interfaces::client, stage);

	return FrameStageNotifyOriginal(interfaces::client, stage);
}

void __stdcall hooks::OverrideView(ViewSetup* setup) {

	if (!globals::localPlayer)
		return OverrideViewOriginal(interfaces::clientMode, setup);
	if(globals::localPlayer->IsScoped())
		return OverrideViewOriginal(interfaces::clientMode, setup);

	if (features::fov) {
		setup->fov = (float)features::customWorldFov;
	}
	else {
		setup->fov = 90;
	}
	
	//thirdperson
	if (features::thirdperson) {
		static CVector myangles;
		interfaces::engine->GetViewAngles(myangles);
		interfaces::input->m_fCameraInThirdPerson = true;
		interfaces::input->m_vecCameraOffset.x = myangles.x;
		interfaces::input->m_vecCameraOffset.y = myangles.y;
		interfaces::input->m_vecCameraOffset.z = static_cast<float>(features::thirdpersonDistance);
		globals::localPlayer->UpdateVisibilityAllEntities();
	}
	else {
		interfaces::input->m_fCameraInThirdPerson = false;
		interfaces::input->m_vecCameraOffset.z = 0.0f;
	}


	return OverrideViewOriginal(interfaces::clientMode, setup);
}

float __stdcall hooks::GetViewModelFOV() {
	if(!globals::localPlayer)
		return GetViewModelFOVOriginal(interfaces::clientMode);
	if(globals::localPlayer->IsScoped())
		return GetViewModelFOVOriginal(interfaces::clientMode);
	if (features::fov)
		return (float)features::customFov;
	return GetViewModelFOVOriginal(interfaces::clientMode);
}