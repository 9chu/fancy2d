#include "f2dSoundSysAPI.h"

////////////////////////////////////////////////////////////////////////////////

f2dSoundSysAPI::f2dSoundSysAPI()
	: m_hModule(NULL),
	m_Entry_pDirectSoundCreate8(NULL)
{
	// 加载模块
	m_hModule = LoadLibrary(L"DSound.dll");
	if(!m_hModule)
		throw fcyWin32Exception("f2dSoundSysAPI::f2dSoundSysAPI", "LoadLibrary(DSound.dll) Failed.");

	// 获得入口
	m_Entry_pDirectSoundCreate8 = (pDLLEntry_DirectSoundCreate8)GetProcAddress(m_hModule, "DirectSoundCreate8");
	if(!m_Entry_pDirectSoundCreate8)
		throw fcyWin32Exception("f2dSoundSysAPI::f2dSoundSysAPI", "GetProcAddress(DirectSoundCreate8) Failed.");
}

f2dSoundSysAPI::~f2dSoundSysAPI()
{
}

HRESULT f2dSoundSysAPI::DLLEntry_DirectSoundCreate8(LPCGUID pcGuidDevice, LPDIRECTSOUND8 *ppDS8, LPUNKNOWN pUnkOuter)
{
	return m_Entry_pDirectSoundCreate8(pcGuidDevice, ppDS8, pUnkOuter);
}
