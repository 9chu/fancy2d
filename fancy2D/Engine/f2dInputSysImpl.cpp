#include "f2dInputSysImpl.h"

#include "f2dEngineImpl.h"

#include "fcyException.h"

////////////////////////////////////////////////////////////////////////////////

f2dInputSysAPI::f2dInputSysAPI()
	: m_hModule(NULL),
	m_Entry_pDirectInput8Create(NULL)
{
	// 加载模块
	m_hModule = LoadLibrary(L"DInput8.dll");
	if(!m_hModule)
		throw fcyWin32Exception("f2dInputSysAPI::f2dInputSysAPI", "LoadLibrary Failed.");

	// 获得入口
	m_Entry_pDirectInput8Create = (pDLLEntry_DirectInput8Create)GetProcAddress(m_hModule, "DirectInput8Create");
	if(!m_Entry_pDirectInput8Create)
		throw fcyWin32Exception("f2dInputSysAPI::f2dInputSysAPI", "GetProcAddress Failed.");
}

f2dInputSysAPI::~f2dInputSysAPI()
{
}

HRESULT f2dInputSysAPI::DLLEntry_DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter)
{
	return m_Entry_pDirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter);
}

////////////////////////////////////////////////////////////////////////////////

const fuInt f2dInputMouseImpl::BufferSize = 32;

const DIDATAFORMAT f2dInputMouseImpl::DIDF_Mouse = 
{
	24, 16, 2, 16, 7,
	(LPDIOBJECTDATAFORMAT)&f2dInputMouseImpl::DIODF_Mouse
};

const DIOBJECTDATAFORMAT f2dInputMouseImpl::DIODF_Mouse[7] = 
{
	{ &GUID_XAxis, 0, 0x00FFFF03, 0 },
	{ &GUID_YAxis, 4, 0x00FFFF03, 0 },
	{ &GUID_ZAxis, 8, 0x80FFFF03, 0 },
	{ 0, 12, 0x00FFFF0C, 0 },
	{ 0, 13, 0x00FFFF0C, 0 },  
	{ 0, 14, 0x80FFFF0C, 0 },
	{ 0, 15, 0x80FFFF0C, 0 }
};

f2dInputMouseImpl::f2dInputMouseImpl(IDirectInput8* pDev, HWND Win, const GUID& pGUID, fBool bGlobalFocus)
	: m_pDev(NULL), m_pListener(NULL),
	m_TotalOffsetX(0), m_TotalOffsetY(0), m_TotalOffsetZ(0)
{
	memset(m_BtnState, 0, sizeof(m_BtnState));

	HRESULT tHR = pDev->CreateDevice(pGUID, &m_pDev, NULL);
	if(FAILED(tHR))
		throw fcyWin32COMException("f2dInputMouseImpl::f2dInputMouseImpl", "CreateDevice Failed.", tHR);
	
	// 设置协作模式
	fuInt tFlag = DISCL_NONEXCLUSIVE;
	if(bGlobalFocus)
		tFlag |= DISCL_BACKGROUND;
	else
		tFlag |= DISCL_FOREGROUND;
	tHR = m_pDev->SetCooperativeLevel(Win, tFlag);
	if(FAILED(tHR))
	{
		FCYSAFEKILL(m_pDev);
		throw fcyWin32COMException("f2dInputMouseImpl::f2dInputMouseImpl", "SetCooperativeLevel Failed.", tHR);
	}

    // 设置数据格式
    tHR = m_pDev->SetDataFormat(&DIDF_Mouse);
	if(FAILED(tHR))
	{
		FCYSAFEKILL(m_pDev);
		throw fcyWin32COMException("f2dInputMouseImpl::f2dInputMouseImpl", "SetDataFormat Failed.", tHR);
	}

	// 设置缓冲区
	DIPROPDWORD tBufferProperty;

    tBufferProperty.diph.dwSize = sizeof(DIPROPDWORD);
    tBufferProperty.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    tBufferProperty.diph.dwObj = 0;
    tBufferProperty.diph.dwHow = DIPH_DEVICE;
    tBufferProperty.dwData = BufferSize;

    tHR = m_pDev->SetProperty(DIPROP_BUFFERSIZE, &tBufferProperty.diph);
	if(FAILED(tHR))
	{
		FCYSAFEKILL(m_pDev);
		throw fcyWin32COMException("f2dInputMouseImpl::f2dInputMouseImpl", "SetProperty Failed.", tHR);
	}

	// 获得设备
	tHR = m_pDev->Acquire();
}

f2dInputMouseImpl::~f2dInputMouseImpl()
{
	if(m_pDev)
		m_pDev->Unacquire();
	FCYSAFEKILL(m_pDev);
}

f2dInputMouse* f2dInputMouseImpl::ToMouse()
{
	return this;
}

f2dInputKeyboard* f2dInputMouseImpl::ToKeyboard()
{
	return NULL;
}

f2dInputJoystick* f2dInputMouseImpl::ToJoystick()
{
	return NULL;
}

fResult f2dInputMouseImpl::UpdateState()
{
	DIDEVICEOBJECTDATA tRawData[BufferSize];
	DWORD tSize = BufferSize;

	HRESULT tHR = m_pDev->GetDeviceData(
		sizeof(DIDEVICEOBJECTDATA),
		tRawData,
		&tSize,
		0);
	
	if(FAILED(tHR))
	{
		tHR = m_pDev->Acquire();
		
		if(FAILED(tHR))
			return FCYERR_INTERNALERR;
		else
		{
			tSize = BufferSize;
			tHR = m_pDev->GetDeviceData(
				sizeof(DIDEVICEOBJECTDATA),
				tRawData,
				&tSize,
				0);
			if(FAILED(tHR))
				return FCYERR_INTERNALERR;
		}
	}

	for(fuInt i = 0; i<tSize; i++)
	{
		switch(tRawData[i].dwOfs)
		{
		case DIMOFS_X:
			m_TotalOffsetX += tRawData[i].dwData;
			// 触发事件
			if(m_pListener)
				m_pListener->OnMouseMoveX(tRawData[i].dwData);
			break;
		case DIMOFS_Y:
			m_TotalOffsetY += tRawData[i].dwData;
			// 触发事件
			if(m_pListener)
				m_pListener->OnMouseMoveY(tRawData[i].dwData);
			break;
		case DIMOFS_Z:
			m_TotalOffsetZ += tRawData[i].dwData;
			// 触发事件
			if(m_pListener)
				m_pListener->OnMouseMoveZ(tRawData[i].dwData);
			break;
		case DIMOFS_BUTTON0:
			m_BtnState[0] = (LOBYTE(tRawData[i].dwData) & 0x80) != 0;
			// 触发事件
			if(m_pListener)
				if(m_BtnState[0])
					m_pListener->OnMouseLBtnDown();
				else
					m_pListener->OnMouseLBtnUp();
			break;
		case DIMOFS_BUTTON1:
			m_BtnState[1] = (LOBYTE(tRawData[i].dwData) & 0x80) != 0;
			// 触发事件
			if(m_pListener)
				if(m_BtnState[1])
					m_pListener->OnMouseRBtnDown();
				else
					m_pListener->OnMouseRBtnUp();
			break;
		case DIMOFS_BUTTON2:
			m_BtnState[2] = (LOBYTE(tRawData[i].dwData) & 0x80) != 0;
			// 触发事件
			if(m_pListener)
				if(m_BtnState[2])
					m_pListener->OnMouseMBtnDown();
				else
					m_pListener->OnMouseMBtnUp();
			break;
		case DIMOFS_BUTTON3:
			m_BtnState[3] = (LOBYTE(tRawData[i].dwData) & 0x80) != 0;
			// 触发事件
			if(m_pListener)
				if(m_BtnState[3])
					m_pListener->OnMouseAdditionBtnDown(0);
				else
					m_pListener->OnMouseAdditionBtnUp(0);
			break;
		case DIMOFS_BUTTON4:
			m_BtnState[4] = (LOBYTE(tRawData[i].dwData) & 0x80) != 0;
			// 触发事件
			if(m_pListener)
				if(m_BtnState[4])
					m_pListener->OnMouseAdditionBtnDown(1);
				else
					m_pListener->OnMouseAdditionBtnUp(1);
			break;
		case DIMOFS_BUTTON5:
			m_BtnState[5] = (LOBYTE(tRawData[i].dwData) & 0x80) != 0;
			// 触发事件
			if(m_pListener)
				if(m_BtnState[5])
					m_pListener->OnMouseAdditionBtnDown(2);
				else
					m_pListener->OnMouseAdditionBtnUp(2);
			break;
		case DIMOFS_BUTTON6:
			m_BtnState[6] = (LOBYTE(tRawData[i].dwData) & 0x80) != 0;
			// 触发事件
			if(m_pListener)
				if(m_BtnState[6])
					m_pListener->OnMouseAdditionBtnDown(3);
				else
					m_pListener->OnMouseAdditionBtnUp(3);
			break;
		case DIMOFS_BUTTON7:
			m_BtnState[7] = (LOBYTE(tRawData[i].dwData) & 0x80) != 0;
			// 触发事件
			if(m_pListener)
				if(m_BtnState[7])
					m_pListener->OnMouseAdditionBtnDown(4);
				else
					m_pListener->OnMouseAdditionBtnUp(4);
			break;
		}
	}

	return FCYERR_OK;
}

f2dInputMouseEventListener* f2dInputMouseImpl::GetListener()
{
	return m_pListener;
}

fResult f2dInputMouseImpl::SetListener(f2dInputMouseEventListener* pListener)
{
	m_pListener = pListener;
	return FCYERR_OK;
}

fBool f2dInputMouseImpl::IsLeftBtnDown()
{
	return m_BtnState[0];
}

fBool f2dInputMouseImpl::IsRightBtnDown()
{
	return m_BtnState[1];
}

fBool f2dInputMouseImpl::IsMiddleBtnDown()
{
	return m_BtnState[2];
}

fBool f2dInputMouseImpl::IsAdditionBtnDown(fuInt Index)
{
	if(Index + 3>=8)
		return false;
	return m_BtnState[3 + Index];
}

fInt f2dInputMouseImpl::GetOffsetX()
{
	int tTotalOffset = m_TotalOffsetX;
	m_TotalOffsetX = 0;
	return tTotalOffset;
}

fInt f2dInputMouseImpl::GetOffsetY()
{
	int tTotalOffset = m_TotalOffsetY;
	m_TotalOffsetY = 0;
	return tTotalOffset;
}

fInt f2dInputMouseImpl::GetOffsetZ()
{
	int tTotalOffset = m_TotalOffsetZ;
	m_TotalOffsetZ = 0;
	return tTotalOffset;
}

//////////////////////////////////////////////////////////////////////////////////////////

const fuInt f2dInputKeyboardImpl::BufferSize = 32;

const DIDATAFORMAT f2dInputKeyboardImpl::DIDF_Keyboard = 
{
	24, 16, 2, 256, 256,
	(LPDIOBJECTDATAFORMAT)&f2dInputKeyboardImpl::DIODF_Keyboard
};

const DIOBJECTDATAFORMAT f2dInputKeyboardImpl::DIODF_Keyboard[256] = 
{
	{ &GUID_Key, 0, 2147483660, 0 },
	{ &GUID_Key, 1, 2147483916, 0 },
	{ &GUID_Key, 2, 2147484172, 0 },
	{ &GUID_Key, 3, 2147484428, 0 },
	{ &GUID_Key, 4, 2147484684, 0 },
	{ &GUID_Key, 5, 2147484940, 0 },
	{ &GUID_Key, 6, 2147485196, 0 },
	{ &GUID_Key, 7, 2147485452, 0 },
	{ &GUID_Key, 8, 2147485708, 0 },
	{ &GUID_Key, 9, 2147485964, 0 },
	{ &GUID_Key, 10, 2147486220, 0 },
	{ &GUID_Key, 11, 2147486476, 0 },
	{ &GUID_Key, 12, 2147486732, 0 },
	{ &GUID_Key, 13, 2147486988, 0 },
	{ &GUID_Key, 14, 2147487244, 0 },
	{ &GUID_Key, 15, 2147487500, 0 },
	{ &GUID_Key, 16, 2147487756, 0 },
	{ &GUID_Key, 17, 2147488012, 0 },
	{ &GUID_Key, 18, 2147488268, 0 },
	{ &GUID_Key, 19, 2147488524, 0 },
	{ &GUID_Key, 20, 2147488780, 0 },
	{ &GUID_Key, 21, 2147489036, 0 },
	{ &GUID_Key, 22, 2147489292, 0 },
	{ &GUID_Key, 23, 2147489548, 0 },
	{ &GUID_Key, 24, 2147489804, 0 },
	{ &GUID_Key, 25, 2147490060, 0 },
	{ &GUID_Key, 26, 2147490316, 0 },
	{ &GUID_Key, 27, 2147490572, 0 },
	{ &GUID_Key, 28, 2147490828, 0 },
	{ &GUID_Key, 29, 2147491084, 0 },
	{ &GUID_Key, 30, 2147491340, 0 },
	{ &GUID_Key, 31, 2147491596, 0 },
	{ &GUID_Key, 32, 2147491852, 0 },
	{ &GUID_Key, 33, 2147492108, 0 },
	{ &GUID_Key, 34, 2147492364, 0 },
	{ &GUID_Key, 35, 2147492620, 0 },
	{ &GUID_Key, 36, 2147492876, 0 },
	{ &GUID_Key, 37, 2147493132, 0 },
	{ &GUID_Key, 38, 2147493388, 0 },
	{ &GUID_Key, 39, 2147493644, 0 },
	{ &GUID_Key, 40, 2147493900, 0 },
	{ &GUID_Key, 41, 2147494156, 0 },
	{ &GUID_Key, 42, 2147494412, 0 },
	{ &GUID_Key, 43, 2147494668, 0 },
	{ &GUID_Key, 44, 2147494924, 0 },
	{ &GUID_Key, 45, 2147495180, 0 },
	{ &GUID_Key, 46, 2147495436, 0 },
	{ &GUID_Key, 47, 2147495692, 0 },
	{ &GUID_Key, 48, 2147495948, 0 },
	{ &GUID_Key, 49, 2147496204, 0 },
	{ &GUID_Key, 50, 2147496460, 0 },
	{ &GUID_Key, 51, 2147496716, 0 },
	{ &GUID_Key, 52, 2147496972, 0 },
	{ &GUID_Key, 53, 2147497228, 0 },
	{ &GUID_Key, 54, 2147497484, 0 },
	{ &GUID_Key, 55, 2147497740, 0 },
	{ &GUID_Key, 56, 2147497996, 0 },
	{ &GUID_Key, 57, 2147498252, 0 },
	{ &GUID_Key, 58, 2147498508, 0 },
	{ &GUID_Key, 59, 2147498764, 0 },
	{ &GUID_Key, 60, 2147499020, 0 },
	{ &GUID_Key, 61, 2147499276, 0 },
	{ &GUID_Key, 62, 2147499532, 0 },
	{ &GUID_Key, 63, 2147499788, 0 },
	{ &GUID_Key, 64, 2147500044, 0 },
	{ &GUID_Key, 65, 2147500300, 0 },
	{ &GUID_Key, 66, 2147500556, 0 },
	{ &GUID_Key, 67, 2147500812, 0 },
	{ &GUID_Key, 68, 2147501068, 0 },
	{ &GUID_Key, 69, 2147501324, 0 },
	{ &GUID_Key, 70, 2147501580, 0 },
	{ &GUID_Key, 71, 2147501836, 0 },
	{ &GUID_Key, 72, 2147502092, 0 },
	{ &GUID_Key, 73, 2147502348, 0 },
	{ &GUID_Key, 74, 2147502604, 0 },
	{ &GUID_Key, 75, 2147502860, 0 },
	{ &GUID_Key, 76, 2147503116, 0 },
	{ &GUID_Key, 77, 2147503372, 0 },
	{ &GUID_Key, 78, 2147503628, 0 },
	{ &GUID_Key, 79, 2147503884, 0 },
	{ &GUID_Key, 80, 2147504140, 0 },
	{ &GUID_Key, 81, 2147504396, 0 },
	{ &GUID_Key, 82, 2147504652, 0 },
	{ &GUID_Key, 83, 2147504908, 0 },
	{ &GUID_Key, 84, 2147505164, 0 },
	{ &GUID_Key, 85, 2147505420, 0 },
	{ &GUID_Key, 86, 2147505676, 0 },
	{ &GUID_Key, 87, 2147505932, 0 },
	{ &GUID_Key, 88, 2147506188, 0 },
	{ &GUID_Key, 89, 2147506444, 0 },
	{ &GUID_Key, 90, 2147506700, 0 },
	{ &GUID_Key, 91, 2147506956, 0 },
	{ &GUID_Key, 92, 2147507212, 0 },
	{ &GUID_Key, 93, 2147507468, 0 },
	{ &GUID_Key, 94, 2147507724, 0 },
	{ &GUID_Key, 95, 2147507980, 0 },
	{ &GUID_Key, 96, 2147508236, 0 },
	{ &GUID_Key, 97, 2147508492, 0 },
	{ &GUID_Key, 98, 2147508748, 0 },
	{ &GUID_Key, 99, 2147509004, 0 },
	{ &GUID_Key, 100, 2147509260, 0 },
	{ &GUID_Key, 101, 2147509516, 0 },
	{ &GUID_Key, 102, 2147509772, 0 },
	{ &GUID_Key, 103, 2147510028, 0 },
	{ &GUID_Key, 104, 2147510284, 0 },
	{ &GUID_Key, 105, 2147510540, 0 },
	{ &GUID_Key, 106, 2147510796, 0 },
	{ &GUID_Key, 107, 2147511052, 0 },
	{ &GUID_Key, 108, 2147511308, 0 },
	{ &GUID_Key, 109, 2147511564, 0 },
	{ &GUID_Key, 110, 2147511820, 0 },
	{ &GUID_Key, 111, 2147512076, 0 },
	{ &GUID_Key, 112, 2147512332, 0 },
	{ &GUID_Key, 113, 2147512588, 0 },
	{ &GUID_Key, 114, 2147512844, 0 },
	{ &GUID_Key, 115, 2147513100, 0 },
	{ &GUID_Key, 116, 2147513356, 0 },
	{ &GUID_Key, 117, 2147513612, 0 },
	{ &GUID_Key, 118, 2147513868, 0 },
	{ &GUID_Key, 119, 2147514124, 0 },
	{ &GUID_Key, 120, 2147514380, 0 },
	{ &GUID_Key, 121, 2147514636, 0 },
	{ &GUID_Key, 122, 2147514892, 0 },
	{ &GUID_Key, 123, 2147515148, 0 },
	{ &GUID_Key, 124, 2147515404, 0 },
	{ &GUID_Key, 125, 2147515660, 0 },
	{ &GUID_Key, 126, 2147515916, 0 },
	{ &GUID_Key, 127, 2147516172, 0 },
	{ &GUID_Key, 128, 2147516428, 0 },
	{ &GUID_Key, 129, 2147516684, 0 },
	{ &GUID_Key, 130, 2147516940, 0 },
	{ &GUID_Key, 131, 2147517196, 0 },
	{ &GUID_Key, 132, 2147517452, 0 },
	{ &GUID_Key, 133, 2147517708, 0 },
	{ &GUID_Key, 134, 2147517964, 0 },
	{ &GUID_Key, 135, 2147518220, 0 },
	{ &GUID_Key, 136, 2147518476, 0 },
	{ &GUID_Key, 137, 2147518732, 0 },
	{ &GUID_Key, 138, 2147518988, 0 },
	{ &GUID_Key, 139, 2147519244, 0 },
	{ &GUID_Key, 140, 2147519500, 0 },
	{ &GUID_Key, 141, 2147519756, 0 },
	{ &GUID_Key, 142, 2147520012, 0 },
	{ &GUID_Key, 143, 2147520268, 0 },
	{ &GUID_Key, 144, 2147520524, 0 },
	{ &GUID_Key, 145, 2147520780, 0 },
	{ &GUID_Key, 146, 2147521036, 0 },
	{ &GUID_Key, 147, 2147521292, 0 },
	{ &GUID_Key, 148, 2147521548, 0 },
	{ &GUID_Key, 149, 2147521804, 0 },
	{ &GUID_Key, 150, 2147522060, 0 },
	{ &GUID_Key, 151, 2147522316, 0 },
	{ &GUID_Key, 152, 2147522572, 0 },
	{ &GUID_Key, 153, 2147522828, 0 },
	{ &GUID_Key, 154, 2147523084, 0 },
	{ &GUID_Key, 155, 2147523340, 0 },
	{ &GUID_Key, 156, 2147523596, 0 },
	{ &GUID_Key, 157, 2147523852, 0 },
	{ &GUID_Key, 158, 2147524108, 0 },
	{ &GUID_Key, 159, 2147524364, 0 },
	{ &GUID_Key, 160, 2147524620, 0 },
	{ &GUID_Key, 161, 2147524876, 0 },
	{ &GUID_Key, 162, 2147525132, 0 },
	{ &GUID_Key, 163, 2147525388, 0 },
	{ &GUID_Key, 164, 2147525644, 0 },
	{ &GUID_Key, 165, 2147525900, 0 },
	{ &GUID_Key, 166, 2147526156, 0 },
	{ &GUID_Key, 167, 2147526412, 0 },
	{ &GUID_Key, 168, 2147526668, 0 },
	{ &GUID_Key, 169, 2147526924, 0 },
	{ &GUID_Key, 170, 2147527180, 0 },
	{ &GUID_Key, 171, 2147527436, 0 },
	{ &GUID_Key, 172, 2147527692, 0 },
	{ &GUID_Key, 173, 2147527948, 0 },
	{ &GUID_Key, 174, 2147528204, 0 },
	{ &GUID_Key, 175, 2147528460, 0 },
	{ &GUID_Key, 176, 2147528716, 0 },
	{ &GUID_Key, 177, 2147528972, 0 },
	{ &GUID_Key, 178, 2147529228, 0 },
	{ &GUID_Key, 179, 2147529484, 0 },
	{ &GUID_Key, 180, 2147529740, 0 },
	{ &GUID_Key, 181, 2147529996, 0 },
	{ &GUID_Key, 182, 2147530252, 0 },
	{ &GUID_Key, 183, 2147530508, 0 },
	{ &GUID_Key, 184, 2147530764, 0 },
	{ &GUID_Key, 185, 2147531020, 0 },
	{ &GUID_Key, 186, 2147531276, 0 },
	{ &GUID_Key, 187, 2147531532, 0 },
	{ &GUID_Key, 188, 2147531788, 0 },
	{ &GUID_Key, 189, 2147532044, 0 },
	{ &GUID_Key, 190, 2147532300, 0 },
	{ &GUID_Key, 191, 2147532556, 0 },
	{ &GUID_Key, 192, 2147532812, 0 },
	{ &GUID_Key, 193, 2147533068, 0 },
	{ &GUID_Key, 194, 2147533324, 0 },
	{ &GUID_Key, 195, 2147533580, 0 },
	{ &GUID_Key, 196, 2147533836, 0 },
	{ &GUID_Key, 197, 2147534092, 0 },
	{ &GUID_Key, 198, 2147534348, 0 },
	{ &GUID_Key, 199, 2147534604, 0 },
	{ &GUID_Key, 200, 2147534860, 0 },
	{ &GUID_Key, 201, 2147535116, 0 },
	{ &GUID_Key, 202, 2147535372, 0 },
	{ &GUID_Key, 203, 2147535628, 0 },
	{ &GUID_Key, 204, 2147535884, 0 },
	{ &GUID_Key, 205, 2147536140, 0 },
	{ &GUID_Key, 206, 2147536396, 0 },
	{ &GUID_Key, 207, 2147536652, 0 },
	{ &GUID_Key, 208, 2147536908, 0 },
	{ &GUID_Key, 209, 2147537164, 0 },
	{ &GUID_Key, 210, 2147537420, 0 },
	{ &GUID_Key, 211, 2147537676, 0 },
	{ &GUID_Key, 212, 2147537932, 0 },
	{ &GUID_Key, 213, 2147538188, 0 },
	{ &GUID_Key, 214, 2147538444, 0 },
	{ &GUID_Key, 215, 2147538700, 0 },
	{ &GUID_Key, 216, 2147538956, 0 },
	{ &GUID_Key, 217, 2147539212, 0 },
	{ &GUID_Key, 218, 2147539468, 0 },
	{ &GUID_Key, 219, 2147539724, 0 },
	{ &GUID_Key, 220, 2147539980, 0 },
	{ &GUID_Key, 221, 2147540236, 0 },
	{ &GUID_Key, 222, 2147540492, 0 },
	{ &GUID_Key, 223, 2147540748, 0 },
	{ &GUID_Key, 224, 2147541004, 0 },
	{ &GUID_Key, 225, 2147541260, 0 },
	{ &GUID_Key, 226, 2147541516, 0 },
	{ &GUID_Key, 227, 2147541772, 0 },
	{ &GUID_Key, 228, 2147542028, 0 },
	{ &GUID_Key, 229, 2147542284, 0 },
	{ &GUID_Key, 230, 2147542540, 0 },
	{ &GUID_Key, 231, 2147542796, 0 },
	{ &GUID_Key, 232, 2147543052, 0 },
	{ &GUID_Key, 233, 2147543308, 0 },
	{ &GUID_Key, 234, 2147543564, 0 },
	{ &GUID_Key, 235, 2147543820, 0 },
	{ &GUID_Key, 236, 2147544076, 0 },
	{ &GUID_Key, 237, 2147544332, 0 },
	{ &GUID_Key, 238, 2147544588, 0 },
	{ &GUID_Key, 239, 2147544844, 0 },
	{ &GUID_Key, 240, 2147545100, 0 },
	{ &GUID_Key, 241, 2147545356, 0 },
	{ &GUID_Key, 242, 2147545612, 0 },
	{ &GUID_Key, 243, 2147545868, 0 },
	{ &GUID_Key, 244, 2147546124, 0 },
	{ &GUID_Key, 245, 2147546380, 0 },
	{ &GUID_Key, 246, 2147546636, 0 },
	{ &GUID_Key, 247, 2147546892, 0 },
	{ &GUID_Key, 248, 2147547148, 0 },
	{ &GUID_Key, 249, 2147547404, 0 },
	{ &GUID_Key, 250, 2147547660, 0 },
	{ &GUID_Key, 251, 2147547916, 0 },
	{ &GUID_Key, 252, 2147548172, 0 },
	{ &GUID_Key, 253, 2147548428, 0 },
	{ &GUID_Key, 254, 2147548684, 0 },
	{ &GUID_Key, 255, 2147548940, 0 }
};

f2dInputKeyboardImpl::f2dInputKeyboardImpl(IDirectInput8* pDev, HWND Win, const GUID& pGUID, fBool bGlobalFocus)
	: m_pDev(NULL), m_pListener(NULL)
{
	memset(m_ButtonState, 0, sizeof(m_ButtonState));

	HRESULT tHR = pDev->CreateDevice(pGUID, &m_pDev, NULL);
	if(FAILED(tHR))
		throw fcyWin32COMException("f2dInputKeyboardImpl::f2dInputKeyboardImpl", "CreateDevice Failed.", tHR);
	
	// 设置协作模式
	fuInt tFlag = DISCL_NONEXCLUSIVE;
	if(bGlobalFocus)
		tFlag |= DISCL_BACKGROUND;
	else
		tFlag |= DISCL_FOREGROUND;
	tHR = m_pDev->SetCooperativeLevel(Win, tFlag);
	if(FAILED(tHR))
	{
		FCYSAFEKILL(m_pDev);
		throw fcyWin32COMException("f2dInputKeyboardImpl::f2dInputKeyboardImpl", "SetCooperativeLevel Failed.", tHR);
	}

    // 设置数据格式
	tHR = m_pDev->SetDataFormat(&DIDF_Keyboard);
	if(FAILED(tHR))
	{
		FCYSAFEKILL(m_pDev);
		throw fcyWin32COMException("f2dInputKeyboardImpl::f2dInputKeyboardImpl", "SetDataFormat Failed.", tHR);
	}

	// 设置缓冲区
	DIPROPDWORD tBufferProperty;

    tBufferProperty.diph.dwSize = sizeof(DIPROPDWORD);
    tBufferProperty.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    tBufferProperty.diph.dwObj = 0;
    tBufferProperty.diph.dwHow = DIPH_DEVICE;
    tBufferProperty.dwData = BufferSize;

    tHR = m_pDev->SetProperty(DIPROP_BUFFERSIZE, &tBufferProperty.diph);
	if(FAILED(tHR))
	{
		FCYSAFEKILL(m_pDev);
		throw fcyWin32COMException("f2dInputKeyboardImpl::f2dInputKeyboardImpl", "SetProperty Failed.", tHR);
	}

	// 获得设备
	tHR = m_pDev->Acquire();
}

f2dInputKeyboardImpl::~f2dInputKeyboardImpl()
{
	if(m_pDev)
		m_pDev->Unacquire();
	FCYSAFEKILL(m_pDev);
}

f2dInputMouse* f2dInputKeyboardImpl::ToMouse()
{
	return NULL;
}

f2dInputKeyboard* f2dInputKeyboardImpl::ToKeyboard()
{
	return this;
}

f2dInputJoystick* f2dInputKeyboardImpl::ToJoystick()
{
	return NULL;
}

fResult f2dInputKeyboardImpl::UpdateState()
{
	DIDEVICEOBJECTDATA tRawData[BufferSize];
	DWORD tSize = BufferSize;

	HRESULT tHR = m_pDev->GetDeviceData(
		sizeof(DIDEVICEOBJECTDATA),
		tRawData,
		&tSize,
		0);
	
	if(FAILED(tHR))
	{
		tHR = m_pDev->Acquire();
		
		if(FAILED(tHR))
			return FCYERR_INTERNALERR;
		else
		{
			tSize = BufferSize;
			tHR = m_pDev->GetDeviceData(
				sizeof(DIDEVICEOBJECTDATA),
				tRawData,
				&tSize,
				0);
			if(FAILED(tHR))
				return FCYERR_INTERNALERR;
		}
	}

	for(fuInt i = 0; i<tSize; i++)
	{
		fuInt tKeyCode = tRawData[i].dwOfs;
		bool tKeyDown = (LOWORD(tRawData[i].dwData) & 0x80) != 0;
		if(tKeyCode>255)
			continue;
		m_ButtonState[tKeyCode] = tKeyDown;

		if(m_pListener)
			if(tKeyDown)
				m_pListener->OnKeyboardBtnDown((F2DINPUTKEYCODE)tKeyCode);
			else
				m_pListener->OnKeyboardBtnUp((F2DINPUTKEYCODE)tKeyCode);
	}

	return FCYERR_OK;
}

f2dInputKeyboardEventListener* f2dInputKeyboardImpl::GetListener()
{
	return m_pListener;
}

fResult f2dInputKeyboardImpl::SetListener(f2dInputKeyboardEventListener* pListener)
{
	m_pListener = pListener;
	return FCYERR_OK;
}

fBool f2dInputKeyboardImpl::IsKeyDown(F2DINPUTKEYCODE KeyCode)
{
	if(KeyCode>255)
		return false;
	return m_ButtonState[KeyCode];
}

//////////////////////////////////////////////////////////////////////////////////////////

const fuInt f2dInputJoystickImpl::BufferSize = 32;

const DIDATAFORMAT f2dInputJoystickImpl::DIDF_Joystick = 
{
	24, 16, 1, 80, 44,
	(LPDIOBJECTDATAFORMAT)&f2dInputJoystickImpl::DIODF_Joystick
};

const DIOBJECTDATAFORMAT f2dInputJoystickImpl::DIODF_Joystick[44] = 
{
	{ &GUID_XAxis, 0, 2164260611, 256 },
	{ &GUID_YAxis, 4, 2164260611, 256 },
	{ &GUID_ZAxis, 8, 2164260611, 256 },
	{ &GUID_RxAxis, 12, 2164260611, 256 },
	{ &GUID_RyAxis, 16, 2164260611, 256 },
	{ &GUID_RzAxis, 20, 2164260611, 256 },
	{ &GUID_Slider, 24, 2164260611, 256 },
	{ &GUID_Slider, 28, 2164260611, 256 },
	{ &GUID_POV, 32, 2164260624, 0 },
	{ &GUID_POV, 36, 2164260624, 0 },
	{ &GUID_POV, 40, 2164260624, 0 },
	{ &GUID_POV, 44, 2164260624, 0 },
	{ 00000000, 48, 2164260620, 0 },
	{ 00000000, 49, 2164260620, 0 },
	{ 00000000, 50, 2164260620, 0 },
	{ 00000000, 51, 2164260620, 0 },
	{ 00000000, 52, 2164260620, 0 },
	{ 00000000, 53, 2164260620, 0 },
	{ 00000000, 54, 2164260620, 0 },
	{ 00000000, 55, 2164260620, 0 },
	{ 00000000, 56, 2164260620, 0 },
	{ 00000000, 57, 2164260620, 0 },
	{ 00000000, 58, 2164260620, 0 },
	{ 00000000, 59, 2164260620, 0 },
	{ 00000000, 60, 2164260620, 0 },
	{ 00000000, 61, 2164260620, 0 },
	{ 00000000, 62, 2164260620, 0 },
	{ 00000000, 63, 2164260620, 0 },
	{ 00000000, 64, 2164260620, 0 },
	{ 00000000, 65, 2164260620, 0 },
	{ 00000000, 66, 2164260620, 0 },
	{ 00000000, 67, 2164260620, 0 },
	{ 00000000, 68, 2164260620, 0 },
	{ 00000000, 69, 2164260620, 0 },
	{ 00000000, 70, 2164260620, 0 },
	{ 00000000, 71, 2164260620, 0 },
	{ 00000000, 72, 2164260620, 0 },
	{ 00000000, 73, 2164260620, 0 },
	{ 00000000, 74, 2164260620, 0 },
	{ 00000000, 75, 2164260620, 0 },
	{ 00000000, 76, 2164260620, 0 },
	{ 00000000, 77, 2164260620, 0 },
	{ 00000000, 78, 2164260620, 0 },
	{ 00000000, 79, 2164260620, 0 }
};

f2dInputJoystickImpl::f2dInputJoystickImpl(IDirectInput8* pDev, HWND Win, const GUID& pGUID, bool bGlobalFocus)
	: m_pDev(NULL), m_pListener(NULL),
	m_lXHalf(0), m_lXHalfLen(0), m_lYHalf(0), m_lYHalfLen(0), m_lZHalf(0), m_lZHalfLen(0), 
	m_lRxHalf(0), m_lRxHalfLen(0), m_lRyHalf(0), m_lRyHalfLen(0), m_lRzHalf(0), m_lRzHalfLen(0),
	m_lX(0.f), m_lY(0.f), m_lZ(0.f), m_lRx(0.f), m_lRy(0.f), m_lRz(0.f)
{
	memset(m_ButtonDown, 0, sizeof(m_ButtonDown));
	memset(m_Slider, 0, sizeof(m_Slider));
	memset(m_POV, 0, sizeof(m_POV));

	HRESULT tHR = pDev->CreateDevice(pGUID, &m_pDev, NULL);
	if(FAILED(tHR))
		throw fcyWin32COMException("f2dInputJoystickImpl::f2dInputJoystickImpl", "CreateDevice Failed.", tHR);
	
	// 设置协作模式
	fuInt tFlag = DISCL_NONEXCLUSIVE;
	if(bGlobalFocus)
		tFlag |= DISCL_BACKGROUND;
	else
		tFlag |= DISCL_FOREGROUND;
	tHR = m_pDev->SetCooperativeLevel(Win, tFlag);
	if(FAILED(tHR))
	{
		FCYSAFEKILL(m_pDev);
		throw fcyWin32COMException("f2dInputJoystickImpl::f2dInputJoystickImpl", "SetCooperativeLevel Failed.", tHR);
	}

    // 设置数据格式
	tHR = m_pDev->SetDataFormat(&DIDF_Joystick);
	if(FAILED(tHR))
	{
		FCYSAFEKILL(m_pDev);
		throw fcyWin32COMException("f2dInputJoystickImpl::f2dInputJoystickImpl", "SetDataFormat Failed.", tHR);
	}
	
	// 设置缓冲区
	DIPROPDWORD tBufferProperty;

    tBufferProperty.diph.dwSize = sizeof(DIPROPDWORD);
    tBufferProperty.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    tBufferProperty.diph.dwObj = 0;
    tBufferProperty.diph.dwHow = DIPH_DEVICE;
    tBufferProperty.dwData = BufferSize;

    tHR = m_pDev->SetProperty(DIPROP_BUFFERSIZE, &tBufferProperty.diph);
	if(FAILED(tHR))
	{
		FCYSAFEKILL(m_pDev);
		throw fcyWin32COMException("f2dInputJoystickImpl::f2dInputJoystickImpl", "SetProperty Failed.", tHR);
	}

	initStates();

	// 获得设备
	tHR = m_pDev->Acquire();
}

f2dInputJoystickImpl::~f2dInputJoystickImpl()
{
	if(m_pDev)
		m_pDev->Unacquire();
	FCYSAFEKILL(m_pDev);
}

void f2dInputJoystickImpl::initStates()
{
	HRESULT tHR;
	DIPROPRANGE tAxisRangeDesc;
	tAxisRangeDesc.diph.dwSize= sizeof( tAxisRangeDesc );
	tAxisRangeDesc.diph.dwHeaderSize = sizeof( tAxisRangeDesc.diph );
	tAxisRangeDesc.diph.dwHow = DIPH_BYOFFSET;
	tAxisRangeDesc.lMin = -40000;
	tAxisRangeDesc.lMax = 40000;

	// --- 设置轴参考范围 ---
	// X轴
	tAxisRangeDesc.diph.dwObj = DIJOFS_X;
    tHR = m_pDev->SetProperty(DIPROP_RANGE, &tAxisRangeDesc.diph);
	// Y轴
	tAxisRangeDesc.diph.dwObj = DIJOFS_Y;
    tHR = m_pDev->SetProperty(DIPROP_RANGE, &tAxisRangeDesc.diph);
	// Z轴
	tAxisRangeDesc.diph.dwObj = DIJOFS_Z;
	tHR = m_pDev->SetProperty(DIPROP_RANGE, &tAxisRangeDesc.diph);
	// X轴旋转
	tAxisRangeDesc.diph.dwObj = DIJOFS_RX;
	tHR = m_pDev->SetProperty(DIPROP_RANGE, &tAxisRangeDesc.diph);
	// Y轴旋转
	tAxisRangeDesc.diph.dwObj = DIJOFS_RY;
	tHR = m_pDev->SetProperty(DIPROP_RANGE, &tAxisRangeDesc.diph);
	// Z轴旋转
	tAxisRangeDesc.diph.dwObj = DIJOFS_RZ;
	tHR = m_pDev->SetProperty(DIPROP_RANGE, &tAxisRangeDesc.diph);

	// --- 获得轴参考范围 ---
	// X轴
	tAxisRangeDesc.diph.dwObj = DIJOFS_X;
    tHR = m_pDev->GetProperty(DIPROP_RANGE, &tAxisRangeDesc.diph);
	if(SUCCEEDED(tHR))
	{
		m_lXHalf = (tAxisRangeDesc.lMin + tAxisRangeDesc.lMax)/2;
		m_lXHalfLen = (tAxisRangeDesc.lMax - tAxisRangeDesc.lMin)/2;
	}
	// Y轴
	tAxisRangeDesc.diph.dwObj = DIJOFS_Y;
    tHR = m_pDev->GetProperty(DIPROP_RANGE, &tAxisRangeDesc.diph);
	if(SUCCEEDED(tHR))
	{
		m_lYHalf = (tAxisRangeDesc.lMin + tAxisRangeDesc.lMax)/2;
		m_lYHalfLen = (tAxisRangeDesc.lMax - tAxisRangeDesc.lMin)/2;
	}
	// Z轴
	tAxisRangeDesc.diph.dwObj = DIJOFS_Z;
    tHR = m_pDev->GetProperty(DIPROP_RANGE, &tAxisRangeDesc.diph);
	if(SUCCEEDED(tHR))
	{
		m_lZHalf = (tAxisRangeDesc.lMin + tAxisRangeDesc.lMax)/2;
		m_lZHalfLen = (tAxisRangeDesc.lMax - tAxisRangeDesc.lMin)/2;
	}
	// X轴旋转
	tAxisRangeDesc.diph.dwObj = DIJOFS_RX;
    tHR = m_pDev->GetProperty(DIPROP_RANGE, &tAxisRangeDesc.diph);
	if(SUCCEEDED(tHR))
	{
		m_lRxHalf = (tAxisRangeDesc.lMin + tAxisRangeDesc.lMax)/2;
		m_lRxHalfLen = (tAxisRangeDesc.lMax - tAxisRangeDesc.lMin)/2;
	}
	// Y轴旋转
	tAxisRangeDesc.diph.dwObj = DIJOFS_RY;
    tHR = m_pDev->GetProperty(DIPROP_RANGE, &tAxisRangeDesc.diph);
	if(SUCCEEDED(tHR))
	{
		m_lRyHalf = (tAxisRangeDesc.lMin + tAxisRangeDesc.lMax)/2;
		m_lRyHalfLen = (tAxisRangeDesc.lMax - tAxisRangeDesc.lMin)/2;
	}
	// Z轴旋转
	tAxisRangeDesc.diph.dwObj = DIJOFS_RZ;
    tHR = m_pDev->GetProperty(DIPROP_RANGE, &tAxisRangeDesc.diph);
	if(SUCCEEDED(tHR))
	{
		m_lRzHalf = (tAxisRangeDesc.lMin + tAxisRangeDesc.lMax)/2;
		m_lRzHalfLen = (tAxisRangeDesc.lMax - tAxisRangeDesc.lMin)/2;
	}
}

f2dInputMouse* f2dInputJoystickImpl::ToMouse()
{
	return NULL;
}

f2dInputKeyboard* f2dInputJoystickImpl::ToKeyboard()
{
	return NULL;
}

f2dInputJoystick* f2dInputJoystickImpl::ToJoystick()
{
	return this;
}

fResult f2dInputJoystickImpl::UpdateState()
{
	DIDEVICEOBJECTDATA tRawData[BufferSize];
	DWORD tSize = BufferSize;

	HRESULT tHR = m_pDev->GetDeviceData(
		sizeof(DIDEVICEOBJECTDATA),
		tRawData,
		&tSize,
		0);
	
	if(FAILED(tHR))
	{
		tHR = m_pDev->Acquire();
		
		if(FAILED(tHR))
			return FCYERR_INTERNALERR;
		else
		{
			tSize = BufferSize;
			tHR = m_pDev->GetDeviceData(
				sizeof(DIDEVICEOBJECTDATA),
				tRawData,
				&tSize,
				0);
			if(FAILED(tHR))
				return FCYERR_INTERNALERR;
		}
	}

	for(fuInt i = 0; i<tSize; i++)
	{
		float tValue = 0.f;
		switch(tRawData[i].dwOfs)
		{
		case DIJOFS_X:
			tValue = ((int)tRawData[i].dwData - m_lXHalf) / (float)(m_lXHalfLen);
			m_lX = tValue;
			if(m_pListener)
				m_pListener->OnJoystickXAxisPosChange(tValue);
			break;
		case DIJOFS_Y:
			tValue = ((int)tRawData[i].dwData - m_lYHalf) / (float)(m_lYHalfLen);
			m_lY = tValue;
			if(m_pListener)
				m_pListener->OnJoystickYAxisPosChange(tValue);
			break;
		case DIJOFS_Z:
			tValue = ((int)tRawData[i].dwData - m_lZHalf) / (float)(m_lZHalfLen);
			m_lZ = tValue;
			if(m_pListener)
				m_pListener->OnJoystickZAxisPosChange(tValue);
			break;
		case DIJOFS_RX:
			tValue = ((int)tRawData[i].dwData - m_lRxHalf) / (float)(m_lRxHalfLen);
			m_lRx = tValue;
			if(m_pListener)
				m_pListener->OnJoystickXAxisRotationChange(tValue);
			break;
		case DIJOFS_RY:
			tValue = ((int)tRawData[i].dwData - m_lRyHalf) / (float)(m_lRyHalfLen);
			m_lRy = tValue;
			if(m_pListener)
				m_pListener->OnJoystickYAxisRotationChange(tValue);
			break;
		case DIJOFS_RZ:
			tValue = ((int)tRawData[i].dwData - m_lRzHalf) / (float)(m_lRzHalfLen);
			m_lRz = tValue;
			if(m_pListener)
				m_pListener->OnJoystickZAxisRotationChange(tValue);
			break;
		case DIJOFS_SLIDER(0):
			m_Slider[0] = tRawData[i].dwData;
			break;
		case DIJOFS_SLIDER(1):
			m_Slider[1] = tRawData[i].dwData;
			break;
		case DIJOFS_POV(0):
			m_POV[0] = tRawData[i].dwData;
			break;
		case DIJOFS_POV(1):
			m_POV[1] = tRawData[i].dwData;
			break;
		case DIJOFS_POV(2):
			m_POV[2] = tRawData[i].dwData;
			break;
		case DIJOFS_POV(3):
			m_POV[3] = tRawData[i].dwData;
			break;
		default:
			if(tRawData[i].dwOfs>=DIJOFS_BUTTON0 && tRawData[i].dwOfs<=DIJOFS_BUTTON31)
			{
				bool tDown = (LOWORD(tRawData[i].dwData)&0x80) != 0;
				m_ButtonDown[tRawData[i].dwOfs - DIJOFS_BUTTON0] = tDown;
				if(m_pListener)
					if(tDown)
						m_pListener->OnJoystickBtnDown(tRawData[i].dwOfs - DIJOFS_BUTTON0);
					else
						m_pListener->OnJoystickBtnUp(tRawData[i].dwOfs - DIJOFS_BUTTON0);
			}
		}
	}

	return FCYERR_OK;
}

f2dInputJoystickEventListener* f2dInputJoystickImpl::GetListener()
{
	return m_pListener;
}

fResult f2dInputJoystickImpl::SetListener(f2dInputJoystickEventListener* pListener)
{
	m_pListener = pListener;
	return FCYERR_OK;
}

fFloat f2dInputJoystickImpl::GetXPosition()
{
	return m_lX;
}

fFloat f2dInputJoystickImpl::GetYPosition()
{
	return m_lY;
}

fFloat f2dInputJoystickImpl::GetZPosition()
{
	return m_lZ;
}

fFloat f2dInputJoystickImpl::GetXRotation()
{
	return m_lRx;
}

fFloat f2dInputJoystickImpl::GetYRotation()
{
	return m_lRy;
}

fFloat f2dInputJoystickImpl::GetZRotation()
{
	return m_lRz;
}

fBool f2dInputJoystickImpl::IsButtonDown(fuInt Index)
{
	if(Index > 31)
		return false;

	return m_ButtonDown[Index];
}

fInt f2dInputJoystickImpl::GetSlider(fuInt Index)
{
	if(Index>1)
		return 0;
	return m_Slider[Index];
}

fuInt f2dInputJoystickImpl::GetPOV(fuInt Index)
{
	if(Index>3)
		return 0;
	return m_POV[Index];
}

fFloat f2dInputJoystickImpl::GetDeadZone()
{
	float tRet = 0.f;
	DIPROPDWORD tDeadZoneDesc;
	tDeadZoneDesc.diph.dwSize= sizeof( tDeadZoneDesc );
	tDeadZoneDesc.diph.dwHeaderSize = sizeof( tDeadZoneDesc.diph );
	tDeadZoneDesc.diph.dwHow = DIPH_BYOFFSET;
	tDeadZoneDesc.diph.dwObj = DIJOFS_X;
    HRESULT tHR = m_pDev->GetProperty(DIPROP_DEADZONE, &tDeadZoneDesc.diph);
	if(SUCCEEDED(tHR))
	{
		tRet = tDeadZoneDesc.dwData / 10000.f;
	}
	return tRet;
}

fResult f2dInputJoystickImpl::SetDeadZone(fFloat Percent)
{
	if(Percent<0.f || Percent>1.f)
		return FCYERR_INVAILDPARAM;
	DWORD tValue = (DWORD)(Percent * 10000);
	DIPROPDWORD tDeadZoneDesc;
	tDeadZoneDesc.diph.dwSize= sizeof( tDeadZoneDesc );
	tDeadZoneDesc.diph.dwHeaderSize = sizeof( tDeadZoneDesc.diph );
	tDeadZoneDesc.diph.dwHow = DIPH_BYOFFSET;
	tDeadZoneDesc.dwData = tValue;

	HRESULT tHR;

	tDeadZoneDesc.diph.dwObj = DIJOFS_X;
    tHR = m_pDev->SetProperty(DIPROP_DEADZONE, &tDeadZoneDesc.diph);

	tDeadZoneDesc.diph.dwObj = DIJOFS_Y;
    tHR = m_pDev->SetProperty(DIPROP_DEADZONE, &tDeadZoneDesc.diph);

	tDeadZoneDesc.diph.dwObj = DIJOFS_Z;
    tHR = m_pDev->SetProperty(DIPROP_DEADZONE, &tDeadZoneDesc.diph);

	tDeadZoneDesc.diph.dwObj = DIJOFS_RX;
    tHR = m_pDev->SetProperty(DIPROP_DEADZONE, &tDeadZoneDesc.diph);

	tDeadZoneDesc.diph.dwObj = DIJOFS_RY;
    tHR = m_pDev->SetProperty(DIPROP_DEADZONE, &tDeadZoneDesc.diph);

	tDeadZoneDesc.diph.dwObj = DIJOFS_RZ;
    tHR = m_pDev->SetProperty(DIPROP_DEADZONE, &tDeadZoneDesc.diph);

	return FCYERR_OK;
}

fFloat f2dInputJoystickImpl::GetSaturation()
{
	float tRet = 0.f;
	DIPROPDWORD tDeadZoneDesc;
	tDeadZoneDesc.diph.dwSize= sizeof( tDeadZoneDesc );
	tDeadZoneDesc.diph.dwHeaderSize = sizeof( tDeadZoneDesc.diph );
	tDeadZoneDesc.diph.dwHow = DIPH_BYOFFSET;
	tDeadZoneDesc.diph.dwObj = DIJOFS_X;
    HRESULT tHR = m_pDev->GetProperty(DIPROP_SATURATION, &tDeadZoneDesc.diph);
	if(SUCCEEDED(tHR))
	{
		tRet = tDeadZoneDesc.dwData / 10000.f;
	}
	return tRet;
}

fResult f2dInputJoystickImpl::SetSaturation(fFloat Percent)
{
	if(Percent<0.f || Percent>1.f)
		return FCYERR_INVAILDPARAM;
	DWORD tValue = (DWORD)(Percent * 10000);
	DIPROPDWORD tDeadZoneDesc;
	tDeadZoneDesc.diph.dwSize= sizeof( tDeadZoneDesc );
	tDeadZoneDesc.diph.dwHeaderSize = sizeof( tDeadZoneDesc.diph );
	tDeadZoneDesc.diph.dwHow = DIPH_BYOFFSET;
	tDeadZoneDesc.dwData = tValue;

	HRESULT tHR;

	tDeadZoneDesc.diph.dwObj = DIJOFS_X;
    tHR = m_pDev->SetProperty(DIPROP_SATURATION, &tDeadZoneDesc.diph);

	tDeadZoneDesc.diph.dwObj = DIJOFS_Y;
    tHR = m_pDev->SetProperty(DIPROP_SATURATION, &tDeadZoneDesc.diph);

	tDeadZoneDesc.diph.dwObj = DIJOFS_Z;
    tHR = m_pDev->SetProperty(DIPROP_SATURATION, &tDeadZoneDesc.diph);

	tDeadZoneDesc.diph.dwObj = DIJOFS_RX;
    tHR = m_pDev->SetProperty(DIPROP_SATURATION, &tDeadZoneDesc.diph);

	tDeadZoneDesc.diph.dwObj = DIJOFS_RY;
    tHR = m_pDev->SetProperty(DIPROP_SATURATION, &tDeadZoneDesc.diph);

	tDeadZoneDesc.diph.dwObj = DIJOFS_RZ;
    tHR = m_pDev->SetProperty(DIPROP_SATURATION, &tDeadZoneDesc.diph);

	return FCYERR_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////

BOOL PASCAL f2dInputSysImpl::enumMouse(LPCDIDEVICEINSTANCE pInfo, void* pThis)
{
	((f2dInputSysImpl*)pThis)->m_MouseList.push_back(*pInfo);

	return DIENUM_CONTINUE;
}

BOOL PASCAL f2dInputSysImpl::enumKeyboard(LPCDIDEVICEINSTANCE pInfo, void* pThis)
{
	((f2dInputSysImpl*)pThis)->m_KeyboardList.push_back(*pInfo);

	return DIENUM_CONTINUE;
}

BOOL PASCAL f2dInputSysImpl::enumGameCtrl(LPCDIDEVICEINSTANCE pInfo, void* pThis)
{
	((f2dInputSysImpl*)pThis)->m_GameCtrlList.push_back(*pInfo);

	return DIENUM_CONTINUE;
}

f2dInputSysImpl::f2dInputSysImpl(f2dEngineImpl* pEngine)
	: m_pEngine(pEngine), m_hWinHandle((HWND)pEngine->GetMainWindow()->GetHandle()), m_pDInput(NULL)
{
	HRESULT tHR = m_API.DLLEntry_DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION,
		IID_IDirectInput8, (VOID**)&m_pDInput, NULL );
	if(FAILED(tHR))
		throw fcyWin32COMException("f2dInputSysImpl::f2dInputSysImpl", "DirectInput8Create Failed.", tHR);

	enumDevice(); // 枚举所有设备
}

f2dInputSysImpl::~f2dInputSysImpl()
{
	FCYSAFEKILL(m_pDInput);
}

void f2dInputSysImpl::enumDevice()
{
	// 枚举所有鼠标
	m_pDInput->EnumDevices(DI8DEVCLASS_POINTER, enumMouse, this, 0);
	// 枚举所有键盘
	m_pDInput->EnumDevices(DI8DEVCLASS_KEYBOARD, enumKeyboard, this, 0);
	// 枚举所有手柄
	m_pDInput->EnumDevices(DI8DEVCLASS_GAMECTRL, enumGameCtrl, this, 0);
}

fuInt f2dInputSysImpl::GetDeviceCount(F2DINPUTDEVTYPE Type)
{
	switch(Type)
	{
	case F2DINPUTDEVTYPE_MOUSE:
		return m_MouseList.size();
	case F2DINPUTDEVTYPE_KEYBOARD:
		return m_KeyboardList.size();
	case F2DINPUTDEVTYPE_GAMECTRL:
		return m_GameCtrlList.size();
	}
	return 0;
}

fcStrW f2dInputSysImpl::GetDeviceName(F2DINPUTDEVTYPE Type, fuInt Index)
{
	switch(Type)
	{
	case F2DINPUTDEVTYPE_MOUSE:
		if(Index >= m_MouseList.size())
			return NULL;
		return m_MouseList[Index].tszInstanceName;
	case F2DINPUTDEVTYPE_KEYBOARD:
		if(Index >= m_KeyboardList.size())
			return NULL;
		return m_KeyboardList[Index].tszInstanceName;
	case F2DINPUTDEVTYPE_GAMECTRL:
		if(Index >= m_GameCtrlList.size())
			return NULL;
		return m_GameCtrlList[Index].tszInstanceName;
	}
	return NULL;
}

fcStrW f2dInputSysImpl::GetDeviceProductName(F2DINPUTDEVTYPE Type, fuInt Index)
{
	switch(Type)
	{
	case F2DINPUTDEVTYPE_MOUSE:
		if(Index >= m_MouseList.size())
			return NULL;
		return m_MouseList[Index].tszProductName;
	case F2DINPUTDEVTYPE_KEYBOARD:
		if(Index >= m_KeyboardList.size())
			return NULL;
		return m_KeyboardList[Index].tszProductName;
	case F2DINPUTDEVTYPE_GAMECTRL:
		if(Index >= m_GameCtrlList.size())
			return NULL;
		return m_GameCtrlList[Index].tszProductName;
	}
	return NULL;
}

fResult f2dInputSysImpl::CreateMouse(fInt DevIndex, fBool bGlobalFocus, f2dInputMouse** pOut)
{
	if(pOut)
		*pOut = NULL;
	else
		return FCYERR_INVAILDPARAM;

	GUID tGuid = GUID_SysMouse;
	
	if(DevIndex != -1)
	{
		if(DevIndex<0 || (fuInt)DevIndex>=m_MouseList.size())
			return FCYERR_INVAILDPARAM;

		tGuid = m_MouseList[DevIndex].guidInstance;
	}

	f2dInputMouse* pRet = NULL;
	try
	{
		pRet = new f2dInputMouseImpl(m_pDInput, m_hWinHandle, tGuid, bGlobalFocus);
	}
	catch(const fcyException& e)
	{
		m_pEngine->ThrowException(e);
		return FCYERR_INTERNALERR;
	}

	*pOut = pRet;

	return FCYERR_OK;
}

fResult f2dInputSysImpl::CreateKeyboard(fInt DevIndex, fBool bGlobalFocus, f2dInputKeyboard** pOut)
{
	if(pOut)
		*pOut = NULL;
	else
		return FCYERR_INVAILDPARAM;

	GUID tGuid = GUID_SysKeyboard;
	
	if(DevIndex != -1)
	{
		if(DevIndex<0 || (fuInt)DevIndex>=m_KeyboardList.size())
			return FCYERR_INVAILDPARAM;

		tGuid = m_KeyboardList[DevIndex].guidInstance;
	}

	f2dInputKeyboard* pRet = NULL;
	try
	{
		pRet = new f2dInputKeyboardImpl(m_pDInput, m_hWinHandle, tGuid, bGlobalFocus);
	}
	catch(const fcyException& e)
	{
		m_pEngine->ThrowException(e);
		return FCYERR_INTERNALERR;
	}

	*pOut = pRet;

	return FCYERR_OK;
}

fResult f2dInputSysImpl::CreateJoystick(fInt DevIndex, fBool bGlobalFocus, f2dInputJoystick** pOut)
{
	if(pOut)
		*pOut = NULL;
	else
		return FCYERR_INVAILDPARAM;
	if(DevIndex<0 || (fuInt)DevIndex>=m_GameCtrlList.size())
		return FCYERR_INVAILDPARAM;

	GUID tGuid = m_GameCtrlList[DevIndex].guidInstance;

	f2dInputJoystick* pRet = NULL;
	try
	{
		pRet = new f2dInputJoystickImpl(m_pDInput, m_hWinHandle, tGuid, bGlobalFocus);
	}
	catch(const fcyException& e)
	{
		m_pEngine->ThrowException(e);
		return FCYERR_INTERNALERR;
	}

	*pOut = pRet;

	return FCYERR_OK;
}
