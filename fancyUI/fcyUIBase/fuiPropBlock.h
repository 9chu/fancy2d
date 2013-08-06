////////////////////////////////////////////////////////////////////////////////
/// @file  fuiPropBlock.h
/// @brief fancyUI 属性块
/// @note  用于保存控件属性
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <fcyRefObj.h>
#include <fcyException.h>

#include "fuiProperty.h"

class fuiControl;

/// @brief 属性块
/// @note  用于保存控件属性状态
class fuiPropBlock
{
	friend class fuiPropBlock;
protected:
	fuiControl* m_pControl;

	std::unordered_map<std::wstring, std::wstring> m_ControlProp;
	std::vector<fuiPropBlock> m_SubControlProp;
protected:
	void getAllProp(fuiPropBlock& Block, fuiControl* p);
	void setAllProp(fuiPropBlock& Block);
public:
	/// @brief 返回属性块所绑定的控件
	fuiControl* GetBindControl();
	/// @brief 重新获取所有属性
	void Get();
	/// @brief 设置所有属性
	void Set();
protected:
	fuiPropBlock();
public:
	/// @brief 构造并绑定到控件
	fuiPropBlock(fuiControl* pControl, fBool CaptureImmediate=true);
	fuiPropBlock(const fuiPropBlock& Org);
	~fuiPropBlock();
};
