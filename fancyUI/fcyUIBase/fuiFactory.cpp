#include "fuiFactory.h"

#include "../fcyUIControl/fuiLabel.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

fuiFactory::fuiFactory()
{
	RegisterControlCreator<fuiLabel>(L"fuiLabel");
}

fuiFactory::~fuiFactory()
{}

fcyRefPointer<fuiControl> fuiFactory::CreateControl(const std::wstring& ControlTypeName, fuiPage* pRootPage, const std::wstring& Name)
{
	unordered_map<std::wstring, lpCreateFunc>::iterator i = m_Creator.find(ControlTypeName);
	if(i == m_Creator.end())
		throw fcyException("fuiFactory::CreateControl", "Control type not found.");

	return i->second(pRootPage, Name);
}

////////////////////////////////////////////////////////////////////////////////

fuiResFactory::fuiResFactory()
{
	RegisterResCreator<fuiSprite>(L"Sprite");
	RegisterResCreator<fuiBorderSprite>(L"BorderSprite");
	RegisterResCreator<fuiFont>(L"Font");
}

fuiResFactory::~fuiResFactory()
{}

fcyRefPointer<fuiRes> fuiResFactory::CreateRes(const std::wstring& ResTypeName, const std::wstring& Name)
{
	unordered_map<std::wstring, lpCreateFunc>::iterator i = m_Creator.find(ResTypeName);
	if(i == m_Creator.end())
		throw fcyException("fuiResFactory::CreateControl", "Res type not found.");

	return i->second(Name);
}
