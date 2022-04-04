#pragma once
#include "globals.h"

// interfaces
#include "../valve/ibaseclientdll.h"
#include "../valve/icliententitylist.h"
#include "../valve/iclientmodeshared.h"
#include "../valve/ienginetrace.h"
#include "../valve/iglobalvars.h"
#include "../valve/iglowmanager.h"
#include "../valve/imaterialsystem.h"
#include "../valve/istudiorender.h"
#include "../valve/ivengineclient.h"
#include "../valve/ivmodelinfo.h"
#include "../valve/ipanel.h"
#include "../valve/engineconvar.h"
#include "../valve/cinput.h"
#include "../valve/surface.h"

// learn more about interfaces -> https://www.youtube.com/watch?v=C0wGdwnaArA
namespace interfaces
{
	// call once to capture all interface pointers
	void Setup() noexcept;

	// capture an individual interface by name & module
	template <typename Interface>
	Interface* Capture(const char* moduleName, const char* interfaceName) noexcept;

	// interface pointers
	inline IBaseClientDLL* client = nullptr;
	inline IClientEntityList* entityList = nullptr;
	inline IClientModeShared* clientMode = nullptr;
	inline IVEngineClient* engineTrace = nullptr;
	inline IEngineTraceClient* trace = nullptr;
	inline IGlobalVars* globals = nullptr;
	inline IGlowManager* glow = nullptr;
	inline IMaterialSystem* materialSystem = nullptr;
	inline IStudioRender* studioRender = nullptr;
	inline IVEngineClient* engine = nullptr;
	inline IVModelInfo* modelInfo = nullptr;
	inline IInputSystem* inputsystem = nullptr;
	inline EngineConVar* cvar = nullptr;
	inline ISurface* surface = nullptr;
	inline Panel* panel = nullptr;
	inline CInput* input = nullptr;
	//inline ViewSetup* overrideview = nullptr;
	inline IClientRenderable* iclientrenderable = nullptr;

	// other
	inline void* keyValuesSystem = nullptr;

}
