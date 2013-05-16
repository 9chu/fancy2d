#include "f2dInputMouseImpl.h"

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
