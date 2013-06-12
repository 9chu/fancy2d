#include "fuiProperty.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

// int

void fuiPropertyAccessorHelper<int>::DefaultGetter(std::wstring& PropData, int* Value)
{
	fCharW tBuffer[256];
	swprintf_s(tBuffer, L"%d", *Value);

	PropData = tBuffer;
}

void fuiPropertyAccessorHelper<int>::DefaultSetter(const std::wstring& PropData, int* Value)
{
	int tValue;

	if(1 != swscanf_s(PropData.c_str(), L"%d", &tValue))
		throw fcyException("fuiPropertyAccessorHelper<int>::DefaultSetter", "Property string is not correct.");

	*Value = tValue;
}

// bool

void fuiPropertyAccessorHelper<bool>::DefaultGetter(std::wstring& PropData, bool* Value)
{
	if(*Value == true)
		PropData = L"true";
	else
		PropData = L"false";
}

void fuiPropertyAccessorHelper<bool>::DefaultSetter(const std::wstring& PropData, bool* Value)
{
	bool tValue;

	if(_wcsicmp(PropData.c_str(), L"true") == 0)
		tValue = true;
	else if(_wcsicmp(PropData.c_str(), L"false") == 0)
		tValue = false;
	else if(_wcsicmp(PropData.c_str(), L"1") == 0)
		tValue = true;
	else if(_wcsicmp(PropData.c_str(), L"0") == 0)
		tValue = false;
	else
		throw fcyException("fuiPropertyAccessorHelper<bool>::DefaultSetter", "Property string is not correct.");

	*Value = tValue;
}

// float

void fuiPropertyAccessorHelper<float>::DefaultGetter(std::wstring& PropData, float* Value)
{
	fCharW tBuffer[256];
	swprintf_s(tBuffer, L"%f", *Value);

	PropData = tBuffer;
}

void fuiPropertyAccessorHelper<float>::DefaultSetter(const std::wstring& PropData, float* Value)
{
	float tValue;

	if(1 != swscanf_s(PropData.c_str(), L"%f", &tValue))
		throw fcyException("fuiPropertyAccessorHelper<float>::DefaultSetter", "Property string is not correct.");

	*Value = tValue;
}

// fcyVec2

void fuiPropertyAccessorHelper<fcyVec2>::DefaultGetter(std::wstring& PropData, fcyVec2* Value)
{
	fCharW tBuffer[256];
	swprintf_s(tBuffer, L"%f,%f", Value->x, Value->y);

	PropData = tBuffer;
}

void fuiPropertyAccessorHelper<fcyVec2>::DefaultSetter(const std::wstring& PropData, fcyVec2* Value)
{
	fcyVec2 tValue;

	if(2 != swscanf_s(PropData.c_str(), L"%f,%f", &tValue.x, &tValue.y))
		throw fcyException("fuiPropertyAccessorHelper<fcyVec2>::DefaultSetter", "Property string is not correct.");

	*Value = tValue;
}

// fcyVec3

void fuiPropertyAccessorHelper<fcyVec3>::DefaultGetter(std::wstring& PropData, fcyVec3* Value)
{
	fCharW tBuffer[256];
	swprintf_s(tBuffer, L"%f,%f,%f", Value->x, Value->y, Value->z);

	PropData = tBuffer;
}

void fuiPropertyAccessorHelper<fcyVec3>::DefaultSetter(const std::wstring& PropData, fcyVec3* Value)
{
	fcyVec3 tValue;

	if(3 != swscanf_s(PropData.c_str(), L"%f,%f,%f", &tValue.x, &tValue.y, &tValue.z))
		throw fcyException("fuiPropertyAccessorHelper<fcyVec3>::DefaultSetter", "Property string is not correct.");

	*Value = tValue;
}

// fcyVec4

void fuiPropertyAccessorHelper<fcyVec4>::DefaultGetter(std::wstring& PropData, fcyVec4* Value)
{
	fCharW tBuffer[256];
	swprintf_s(tBuffer, L"%f,%f,%f,%f", Value->x, Value->y, Value->z, Value->w);

	PropData = tBuffer;
}

void fuiPropertyAccessorHelper<fcyVec4>::DefaultSetter(const std::wstring& PropData, fcyVec4* Value)
{
	fcyVec4 tValue;

	if(4 != swscanf_s(PropData.c_str(), L"%f,%f,%f,%f", &tValue.x, &tValue.y, &tValue.z, &tValue.w))
		throw fcyException("fuiPropertyAccessorHelper<fcyVec4>::DefaultSetter", "Property string is not correct.");

	*Value = tValue;
}

// fcyRect

void fuiPropertyAccessorHelper<fcyRect>::DefaultGetter(std::wstring& PropData, fcyRect* Value)
{
	fCharW tBuffer[256];
	swprintf_s(tBuffer, L"%f,%f,%f,%f", Value->a.x, Value->a.y, Value->b.x, Value->b.y);

	PropData = tBuffer;
}

void fuiPropertyAccessorHelper<fcyRect>::DefaultSetter(const std::wstring& PropData, fcyRect* Value)
{
	fcyRect tValue;

	if(4 != swscanf_s(PropData.c_str(), L"%f,%f,%f,%f", &tValue.a.x, &tValue.a.y, &tValue.b.x, &tValue.b.y))
		throw fcyException("fuiPropertyAccessorHelper<fcyRect>::DefaultSetter", "Property string is not correct.");

	*Value = tValue;
}

// fcyColor

void fuiPropertyAccessorHelper<fcyColor>::DefaultGetter(std::wstring& PropData, fcyColor* Value)
{
	fCharW tBuffer[256];
	swprintf_s(tBuffer, L"%hhu,%hhu,%hhu,%hhu", Value->a, Value->r, Value->g, Value->b);

	PropData = tBuffer;
}

void fuiPropertyAccessorHelper<fcyColor>::DefaultSetter(const std::wstring& PropData, fcyColor* Value)
{
	int t[4];

	if(4 != swscanf_s(PropData.c_str(), L"%u,%u,%u,%u", &t[0], &t[1], &t[2], &t[3]))
		throw fcyException("fuiPropertyAccessorHelper<fcyColor>::DefaultSetter", "Property string is not correct.");

	*Value = fcyColor(t[0], t[1], t[2], t[3]);
}

// std::wstring

void fuiPropertyAccessorHelper<std::wstring>::DefaultGetter(std::wstring& PropData, std::wstring* Value)
{
	PropData = *Value;
}

void fuiPropertyAccessorHelper<std::wstring>::DefaultSetter(const std::wstring& PropData, std::wstring* Value)
{
	*Value = PropData;
}

////////////////////////////////////////////////////////////////////////////////

void fuiPropertySet::RegisterProperty(const std::wstring& Str, fuiProperty* pProp)
{
	m_PropList[Str] = pProp;
}

const std::wstring& fuiPropertySet::RawGetProperty(const std::wstring& PropName)const
{
	unordered_map<std::wstring, fuiProperty*>::const_iterator i = m_PropList.find(PropName);
	if(i == m_PropList.end())
		throw fcyException("fuiControl::RawGetProperty", "Property is not exsit.");
	else
		return (i->second)->Get();
}

void fuiPropertySet::RawSetProperty(const std::wstring& PropName, const std::wstring& Value)
{
	unordered_map<std::wstring, fuiProperty*>::iterator i = m_PropList.find(PropName);
	if(i == m_PropList.end())
		throw fcyException("fuiControl::RawGetProperty", "Property is not exsit.");
	else
		(i->second)->Set(Value);
}
