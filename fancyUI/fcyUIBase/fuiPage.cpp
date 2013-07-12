#include "fuiPage.h"

#include "fuiFactory.h"

#include <fcyOS/fcyDebug.h>

using namespace std;

////////////////////////////////////////////////////////////////////////////////

const F2DINPUTKEYCODE VKCodeToF2DKeyCodeTable[256] = 
{
	F2DINPUTKEYCODE_UNKNOWN,
	F2DINPUTKEYCODE_UNKNOWN,    // 1 VK_LBUTTON
	F2DINPUTKEYCODE_UNKNOWN,    // 2 VK_RBUTTON
	F2DINPUTKEYCODE_UNKNOWN,    // 3 VK_CANCEL
	F2DINPUTKEYCODE_UNKNOWN,    // 4 VK_MBUTTON
	F2DINPUTKEYCODE_UNKNOWN,    // 5 VK_XBUTTON1
	F2DINPUTKEYCODE_UNKNOWN,    // 6 VK_XBUTTON2
	F2DINPUTKEYCODE_UNKNOWN,    // 7
	F2DINPUTKEYCODE_BACK,       // 8 VK_BACK = Backspace
	F2DINPUTKEYCODE_TAB,        // 9 VK_TAB = Tab
	F2DINPUTKEYCODE_UNKNOWN,    // 10
	F2DINPUTKEYCODE_UNKNOWN,    // 11
	F2DINPUTKEYCODE_UNKNOWN,    // 12 VK_CLEAR = Clear
	F2DINPUTKEYCODE_RETURN,     // 13 VK_RETURN = Enter
	F2DINPUTKEYCODE_UNKNOWN,    // 14
	F2DINPUTKEYCODE_UNKNOWN,    // 15
	F2DINPUTKEYCODE_LSHIFT,     // 16 VK_SHIFT = Shift
	F2DINPUTKEYCODE_LCONTROL,   // 17 VK_CONTROL = Ctrl
	F2DINPUTKEYCODE_LMENU,      // 18 VK_MENU = Alt
	F2DINPUTKEYCODE_PAUSE,      // 19 VK_PAUSE = Pause
	F2DINPUTKEYCODE_CAPITAL,    // 20 VK_CAPITAL = Caps Lock
	F2DINPUTKEYCODE_KANA,       // 21 VK_KANA
	F2DINPUTKEYCODE_UNKNOWN,    // 22
	F2DINPUTKEYCODE_UNKNOWN,    // 23 VK_JUNJA
	F2DINPUTKEYCODE_UNKNOWN,    // 24 VK_FINAL
	F2DINPUTKEYCODE_KANJI,      // 25 VK_HANJA
	F2DINPUTKEYCODE_UNKNOWN,    // 26
	F2DINPUTKEYCODE_ESCAPE,     // 27 VK_ESCAPE = Esc
	F2DINPUTKEYCODE_CONVERT,    // 28 VK_CONVERT
	F2DINPUTKEYCODE_NOCONVERT,  // 29 VK_NONCONVERT
	F2DINPUTKEYCODE_UNKNOWN,    // 30 VK_ACCEPT
	F2DINPUTKEYCODE_UNKNOWN,    // 31 VK_MODECHANGE
	F2DINPUTKEYCODE_SPACE,      // 32 VK_SPACE = Space
	F2DINPUTKEYCODE_PRIOR,      // 33 VK_PRIOR = Page Up
	F2DINPUTKEYCODE_NEXT,       // 34 VK_NEXT = Page Down
	F2DINPUTKEYCODE_END,        // 35 VK_END = End
	F2DINPUTKEYCODE_HOME,       // 36 VK_HOME = Home
	F2DINPUTKEYCODE_LEFT,       // 37 VK_LEFT = Left Arrow
	F2DINPUTKEYCODE_UP,         // 38 VK_UP	= Up Arrow
	F2DINPUTKEYCODE_RIGHT,      // 39 VK_RIGHT = Right Arrow
	F2DINPUTKEYCODE_DOWN,       // 40 VK_DOWN = Down Arrow
	F2DINPUTKEYCODE_UNKNOWN,    // 41 VK_SELECT = Select
	F2DINPUTKEYCODE_UNKNOWN,    // 42 VK_PRINT = Print
	F2DINPUTKEYCODE_UNKNOWN,    // 43 VK_EXECUTE = Execute
	F2DINPUTKEYCODE_UNKNOWN,    // 44 VK_SNAPSHOT = Snapshot
	F2DINPUTKEYCODE_INSERT,     // 45 VK_INSERT = Insert
	F2DINPUTKEYCODE_DELETE,     // 46 VK_DELETE = Delete
	F2DINPUTKEYCODE_UNKNOWN,    // 47 VK_HELP = Help
	F2DINPUTKEYCODE_0,          // 48 0
	F2DINPUTKEYCODE_1,          // 49 1
	F2DINPUTKEYCODE_2,          // 50 2
	F2DINPUTKEYCODE_3,          // 51 3
	F2DINPUTKEYCODE_4,          // 52 4
	F2DINPUTKEYCODE_5,          // 53 5
	F2DINPUTKEYCODE_6,          // 54 6
	F2DINPUTKEYCODE_7,          // 55 7
	F2DINPUTKEYCODE_8,          // 56 8
	F2DINPUTKEYCODE_9,          // 57 9
	F2DINPUTKEYCODE_UNKNOWN,    // 58
	F2DINPUTKEYCODE_UNKNOWN,    // 59
	F2DINPUTKEYCODE_UNKNOWN,    // 60
	F2DINPUTKEYCODE_UNKNOWN,    // 61
	F2DINPUTKEYCODE_UNKNOWN,    // 62
	F2DINPUTKEYCODE_UNKNOWN,    // 63
	F2DINPUTKEYCODE_UNKNOWN,    // 64
	F2DINPUTKEYCODE_A,          // 65 A
	F2DINPUTKEYCODE_B,          // 66 B
	F2DINPUTKEYCODE_C,          // 67 C
	F2DINPUTKEYCODE_D,          // 68 D
	F2DINPUTKEYCODE_E,          // 69 E
	F2DINPUTKEYCODE_F,          // 70 F
	F2DINPUTKEYCODE_G,          // 71 G
	F2DINPUTKEYCODE_H,          // 72 H
	F2DINPUTKEYCODE_I,          // 73 I
	F2DINPUTKEYCODE_J,          // 74 J
	F2DINPUTKEYCODE_K,          // 75 K
	F2DINPUTKEYCODE_L,          // 76 L
	F2DINPUTKEYCODE_M,          // 77 M
	F2DINPUTKEYCODE_N,          // 78 N
	F2DINPUTKEYCODE_O,          // 79 O
	F2DINPUTKEYCODE_P,          // 80 P
	F2DINPUTKEYCODE_Q,          // 81 Q
	F2DINPUTKEYCODE_R,          // 82 R
	F2DINPUTKEYCODE_S,          // 83 S
	F2DINPUTKEYCODE_T,          // 84 T
	F2DINPUTKEYCODE_U,          // 85 U
	F2DINPUTKEYCODE_V,          // 86 V
	F2DINPUTKEYCODE_W,          // 87 W
	F2DINPUTKEYCODE_X,          // 88 X
	F2DINPUTKEYCODE_Y,          // 89 Y
	F2DINPUTKEYCODE_Z,          // 90 Z
	F2DINPUTKEYCODE_LWIN,       // 91 VK_LWIN
	F2DINPUTKEYCODE_RWIN,       // 92 VK_RWIN
	F2DINPUTKEYCODE_APPS,       // 93 VK_APPS
	F2DINPUTKEYCODE_UNKNOWN,    // 94
	F2DINPUTKEYCODE_SLEEP,      // 95 VK_SLEEP
	F2DINPUTKEYCODE_NUMPAD0,    // 96 VK_NUMPAD0 = 小键盘 0
	F2DINPUTKEYCODE_NUMPAD1,    // 97 VK_NUMPAD1 = 小键盘 1
	F2DINPUTKEYCODE_NUMPAD2,    // 98 VK_NUMPAD2 = 小键盘 2
	F2DINPUTKEYCODE_NUMPAD3,    // 99 VK_NUMPAD3 = 小键盘 3
	F2DINPUTKEYCODE_NUMPAD4,    // 100 VK_NUMPAD4 = 小键盘 4
	F2DINPUTKEYCODE_NUMPAD5,    // 101 VK_NUMPAD5 = 小键盘 5
	F2DINPUTKEYCODE_NUMPAD6,    // 102 VK_NUMPAD6 = 小键盘 6
	F2DINPUTKEYCODE_NUMPAD7,    // 103 VK_NUMPAD7 = 小键盘 7
	F2DINPUTKEYCODE_NUMPAD8,    // 104 VK_NUMPAD8 = 小键盘 8
	F2DINPUTKEYCODE_NUMPAD9,    // 105 VK_NUMPAD9 = 小键盘 9
	F2DINPUTKEYCODE_MULTIPLY,   // 106 VK_MULTIPLY = 小键盘 *
	F2DINPUTKEYCODE_ADD,        // 107 VK_ADD = 小键盘 +
	F2DINPUTKEYCODE_NUMPADENTER,// 108 VK_SEPARATOR = 小键盘 Enter
	F2DINPUTKEYCODE_SUBTRACT,   // 109 VK_SUBTRACT = 小键盘 -
	F2DINPUTKEYCODE_DECIMAL,    // 110 VK_DECIMAL = 小键盘 .
	F2DINPUTKEYCODE_DIVIDE,     // 111 VK_DIVIDE = 小键盘 /
	F2DINPUTKEYCODE_F1,         // 112 VK_F1 = F1
	F2DINPUTKEYCODE_F2,         // 113 VK_F2 = F2
	F2DINPUTKEYCODE_F3,         // 114 VK_F3 = F3
	F2DINPUTKEYCODE_F4,         // 115 VK_F4 = F4
	F2DINPUTKEYCODE_F5,         // 116 VK_F5 = F5
	F2DINPUTKEYCODE_F6,         // 117 VK_F6 = F6
	F2DINPUTKEYCODE_F7,         // 118 VK_F7 = F7
	F2DINPUTKEYCODE_F8,         // 119 VK_F8 = F8
	F2DINPUTKEYCODE_F9,         // 120 VK_F9 = F9
	F2DINPUTKEYCODE_F10,        // 121 VK_F10 = F10
	F2DINPUTKEYCODE_F11,        // 122 VK_F11 = F11
	F2DINPUTKEYCODE_F12,        // 123 VK_F12 = F12
	F2DINPUTKEYCODE_F13,        // 124 VK_F13
	F2DINPUTKEYCODE_F14,        // 125 VK_F14
	F2DINPUTKEYCODE_F15,        // 126 VK_F15
	F2DINPUTKEYCODE_UNKNOWN,    // 127 VK_F16
	F2DINPUTKEYCODE_UNKNOWN,    // 128 VK_F17
	F2DINPUTKEYCODE_UNKNOWN,    // 129 VK_F18
	F2DINPUTKEYCODE_UNKNOWN,    // 130 VK_F19
	F2DINPUTKEYCODE_UNKNOWN,    // 131 VK_F20
	F2DINPUTKEYCODE_UNKNOWN,    // 132 VK_F21
	F2DINPUTKEYCODE_UNKNOWN,    // 133 VK_F22
	F2DINPUTKEYCODE_UNKNOWN,    // 134 VK_F23
	F2DINPUTKEYCODE_UNKNOWN,    // 135 VK_F24
	F2DINPUTKEYCODE_UNKNOWN,    // 136
	F2DINPUTKEYCODE_UNKNOWN,    // 137
	F2DINPUTKEYCODE_UNKNOWN,    // 138
	F2DINPUTKEYCODE_UNKNOWN,    // 139
	F2DINPUTKEYCODE_UNKNOWN,    // 140
	F2DINPUTKEYCODE_UNKNOWN,    // 141
	F2DINPUTKEYCODE_UNKNOWN,    // 142
	F2DINPUTKEYCODE_UNKNOWN,    // 143
	F2DINPUTKEYCODE_NUMLOCK,    // 144 VK_NUMLOCK = Num Lock
	F2DINPUTKEYCODE_SCROLL,     // 145 VK_SCROLL = Scroll
	F2DINPUTKEYCODE_UNKNOWN,    // 146
	F2DINPUTKEYCODE_UNKNOWN,    // 147
	F2DINPUTKEYCODE_UNKNOWN,    // 148
	F2DINPUTKEYCODE_UNKNOWN,    // 149
	F2DINPUTKEYCODE_UNKNOWN,    // 150
	F2DINPUTKEYCODE_UNKNOWN,    // 151
	F2DINPUTKEYCODE_UNKNOWN,    // 152
	F2DINPUTKEYCODE_UNKNOWN,    // 153
	F2DINPUTKEYCODE_UNKNOWN,    // 154
	F2DINPUTKEYCODE_UNKNOWN,    // 155
	F2DINPUTKEYCODE_UNKNOWN,    // 156
	F2DINPUTKEYCODE_UNKNOWN,    // 157
	F2DINPUTKEYCODE_UNKNOWN,    // 158
	F2DINPUTKEYCODE_UNKNOWN,    // 159
	F2DINPUTKEYCODE_LSHIFT,     // 160 VK_LSHIFT
	F2DINPUTKEYCODE_RSHIFT,     // 161 VK_RSHIFT
	F2DINPUTKEYCODE_LCONTROL,   // 162 VK_LCONTROL
	F2DINPUTKEYCODE_RCONTROL,   // 163 VK_RCONTROL
	F2DINPUTKEYCODE_LMENU,      // 164 VK_LMENU
	F2DINPUTKEYCODE_RMENU,      // 165 VK_RMENU
	F2DINPUTKEYCODE_WEBBACK,    // 166 VK_BROWSER_BACK
	F2DINPUTKEYCODE_WEBFORWARD, // 167 VK_BROWSER_FORWARD
	F2DINPUTKEYCODE_WEBREFRESH, // 168 VK_BROWSER_REFRESH
	F2DINPUTKEYCODE_WEBSTOP,    // 169 VK_BROWSER_STOP
	F2DINPUTKEYCODE_WEBSEARCH,  // 170 VK_BROWSER_SEARCH
	F2DINPUTKEYCODE_WEBFAVORITES, // 171 VK_BROWSER_FAVORITES
	F2DINPUTKEYCODE_WEBHOME,    // 172 VK_BROWSER_HOME
	F2DINPUTKEYCODE_MUTE,       // 173 VK_VOLUME_MUTE
	F2DINPUTKEYCODE_VOLUMEDOWN, // 174 VK_VOLUME_DOWN
	F2DINPUTKEYCODE_VOLUMEUP,   // 175 VK_VOLUME_UP
	F2DINPUTKEYCODE_NEXTTRACK,  // 176 VK_MEDIA_NEXT_TRACK
	F2DINPUTKEYCODE_PREVTRACK,  // 177 VK_MEDIA_PREV_TRACK
	F2DINPUTKEYCODE_MEDIASTOP,  // 178 VK_MEDIA_STOP
	F2DINPUTKEYCODE_PLAYPAUSE,  // 179 VK_MEDIA_PLAY_PAUSE
	F2DINPUTKEYCODE_MAIL,       // 180 VK_LAUNCH_MAIL
	F2DINPUTKEYCODE_MEDIASELECT,// 181 VK_LAUNCH_MEDIA_SELECT
	F2DINPUTKEYCODE_UNKNOWN,    // 182 VK_LAUNCH_APP1
	F2DINPUTKEYCODE_UNKNOWN,    // 183 VK_LAUNCH_APP2
	F2DINPUTKEYCODE_UNKNOWN,    // 184
	F2DINPUTKEYCODE_UNKNOWN,    // 185
	F2DINPUTKEYCODE_SEMICOLON,  // 186 VK_OEM_1	= ; :
	F2DINPUTKEYCODE_EQUALS,     // 187 VK_OEM_PLUS = = +
	F2DINPUTKEYCODE_COMMA,      // 188 VK_OEM_COMMA = ,
	F2DINPUTKEYCODE_MINUS,      // 189 VK_OEM_MINUS = - _
	F2DINPUTKEYCODE_PERIOD,     // 190 VK_OEM_PERIOD = .
	F2DINPUTKEYCODE_SLASH,      // 191 VK_OEM_2 = / ?
	F2DINPUTKEYCODE_GRAVE,      // 192 VK_OEM_3 = ` ~
	F2DINPUTKEYCODE_UNKNOWN,    // 193
	F2DINPUTKEYCODE_UNKNOWN,    // 194
	F2DINPUTKEYCODE_UNKNOWN,    // 195
	F2DINPUTKEYCODE_UNKNOWN,    // 196
	F2DINPUTKEYCODE_UNKNOWN,    // 197
	F2DINPUTKEYCODE_UNKNOWN,    // 198
	F2DINPUTKEYCODE_UNKNOWN,    // 199
	F2DINPUTKEYCODE_UNKNOWN,    // 200
	F2DINPUTKEYCODE_UNKNOWN,    // 201
	F2DINPUTKEYCODE_UNKNOWN,    // 202
	F2DINPUTKEYCODE_UNKNOWN,    // 203
	F2DINPUTKEYCODE_UNKNOWN,    // 204
	F2DINPUTKEYCODE_UNKNOWN,    // 205
	F2DINPUTKEYCODE_UNKNOWN,    // 206
	F2DINPUTKEYCODE_UNKNOWN,    // 207
	F2DINPUTKEYCODE_UNKNOWN,    // 208
	F2DINPUTKEYCODE_UNKNOWN,    // 209
	F2DINPUTKEYCODE_UNKNOWN,    // 210
	F2DINPUTKEYCODE_UNKNOWN,    // 211
	F2DINPUTKEYCODE_UNKNOWN,    // 212
	F2DINPUTKEYCODE_UNKNOWN,    // 213
	F2DINPUTKEYCODE_UNKNOWN,    // 214
	F2DINPUTKEYCODE_UNKNOWN,    // 215
	F2DINPUTKEYCODE_UNKNOWN,    // 216
	F2DINPUTKEYCODE_UNKNOWN,    // 217
	F2DINPUTKEYCODE_UNKNOWN,    // 218
	F2DINPUTKEYCODE_LBRACKET,   // 219 VK_OEM_4 = [ {
	F2DINPUTKEYCODE_BACKSLASH,  // 220 VK_OEM_5 = \ |
	F2DINPUTKEYCODE_RBRACKET,   // 221 VK_OEM_6 = ] }
	F2DINPUTKEYCODE_APOSTROPHE, // 222 VK_OEM_7 = ' "
	F2DINPUTKEYCODE_UNKNOWN,    // 223 VK_OEM_8
	F2DINPUTKEYCODE_UNKNOWN,    // 224
	F2DINPUTKEYCODE_UNKNOWN,    // 225
	F2DINPUTKEYCODE_OEM_102,    // 226 VK_OEM_102
	F2DINPUTKEYCODE_UNKNOWN,    // 227
	F2DINPUTKEYCODE_UNKNOWN,    // 228
	F2DINPUTKEYCODE_UNKNOWN,    // 229 VK_PROCESSKEY
	F2DINPUTKEYCODE_UNKNOWN,    // 230
	F2DINPUTKEYCODE_UNKNOWN,    // 231 VK_PACKET
	F2DINPUTKEYCODE_UNKNOWN,    // 232
	F2DINPUTKEYCODE_UNKNOWN,    // 233
	F2DINPUTKEYCODE_UNKNOWN,    // 234
	F2DINPUTKEYCODE_UNKNOWN,    // 235
	F2DINPUTKEYCODE_UNKNOWN,    // 236
	F2DINPUTKEYCODE_UNKNOWN,    // 237
	F2DINPUTKEYCODE_UNKNOWN,    // 238
	F2DINPUTKEYCODE_UNKNOWN,    // 239
	F2DINPUTKEYCODE_UNKNOWN,    // 240
	F2DINPUTKEYCODE_UNKNOWN,    // 241
	F2DINPUTKEYCODE_UNKNOWN,    // 242
	F2DINPUTKEYCODE_UNKNOWN,    // 243
	F2DINPUTKEYCODE_UNKNOWN,    // 244
	F2DINPUTKEYCODE_UNKNOWN,    // 245
	F2DINPUTKEYCODE_UNKNOWN,    // 246 VK_ATTN
	F2DINPUTKEYCODE_UNKNOWN,    // 247 VK_CRSEL
	F2DINPUTKEYCODE_UNKNOWN,    // 248 VK_EXSEL
	F2DINPUTKEYCODE_UNKNOWN,    // 249 VK_EREOF
	F2DINPUTKEYCODE_UNKNOWN,    // 250 VK_PLAY
	F2DINPUTKEYCODE_UNKNOWN,    // 251 VK_ZOOM
	F2DINPUTKEYCODE_UNKNOWN,    // 252 VK_NONAME
	F2DINPUTKEYCODE_UNKNOWN,    // 253 VK_PA1	
	F2DINPUTKEYCODE_UNKNOWN,    // 254 VK_OEM_CLEAR
	F2DINPUTKEYCODE_UNKNOWN     // 255
};

F2DINPUTKEYCODE fuiPage::VKKeyToF2DKey(fuInt VKCode)
{
	return VKCodeToF2DKeyCodeTable[VKCode];
}

////////////////////////////////////////////////////////////////////////////////

fuiPage::fuiPage(const std::wstring& Name, f2dRenderer* pRenderer, f2dGraphics2D* pGraph)
	: fuiControl(this, Name), m_pGraphics(pRenderer, pGraph), m_bDebug(false),
	m_pControlAtMousePos(NULL), m_pLastMouseMoveControl(NULL), m_pLockMouseControl(NULL), m_pFocus(NULL)
{
	m_pDefaultStyle.DirectSet(new fuiStyle());

	m_Rect = fcyRect(0.f, 0.f, (float)pRenderer->GetDevice()->GetBufferWidth(), (float)pRenderer->GetDevice()->GetBufferHeight());

	RegisterEvent(L"OnGlobalMouseMove");
	RegisterEvent(L"OnGlobalMouseLDown");
	RegisterEvent(L"OnGlobalMouseLUp");
	RegisterEvent(L"OnGlobalMouseMDown");
	RegisterEvent(L"OnGlobalMouseMUp");
	RegisterEvent(L"OnGlobalMouseRDown");
	RegisterEvent(L"OnGlobalMouseRUp");

	RegisterEvent(L"OnIMEClosed");
	RegisterEvent(L"OnIMEActivated");
	RegisterEvent(L"OnIMEStartComposition");
	RegisterEvent(L"OnIMEEndComposition");
	RegisterEvent(L"OnIMEComposition");
	RegisterEvent(L"OnIMEOpenCandidate");
	RegisterEvent(L"OnIMECloseCandidate");
	RegisterEvent(L"OnIMEChangeCandidate");

	RegisterEvent(L"OnTextInputStart");
	RegisterEvent(L"OnTextInputPosChanged");
	RegisterEvent(L"OnTextInputEnd");
}

fuiPage::~fuiPage()
{
	// 清空子对象
	m_SubControlList.clear();
	
	// 检查泄露
	if(!m_ControlMap.empty())
	{
		fcyDebug::Trace(L"[ @ fuiPage::~fuiPage ] Detected object not properly release.\n");
		
#ifdef _DEBUG
		unordered_map<std::wstring, fuiControl*>::iterator i = m_ControlMap.begin();
		while(i != m_ControlMap.end())
		{
			char tTextBuffer[256];
			sprintf_s(tTextBuffer, "Unrelease UI control object at %x", i->second);
			fcyDebug::Trace("[ @ fuiPage::~fuiPage ] %s\n", tTextBuffer);

			++i;
		}
#endif
	}
}

void fuiPage::loadLayoutNode(fcyXmlNode* pNode, fuiControl* pParent)
{
	for(fuInt i = 0; i<pNode->GetNodeCount(); ++i)
	{
		fcyXmlNode* pSubNode = pNode->GetNode(i);

		// 检查名称
		if(!pSubNode->HasAttribute(L"Name"))
			throw fcyException("fuiPage::loadLayoutNode", "Property 'Name' is needed.");

		fcyRefPointer<fuiControl> pNew = NULL;

		wstring tName = pSubNode->GetAttribute(L"Name");

		// 控件覆写
		if(wcscmp(pSubNode->GetName(), L"override") == 0)
		{	
			pNew = FindControl(tName);
			if(*pNew == NULL)
				throw fcyException("fuiPage::loadLayoutNode", "Can not override unexist object.");
			if(pNew->GetParent() != pParent)
				throw fcyException("fuiPage::loadLayoutNode", "Can not override object with different parent.");
		}
		else
		{
			// 创建控件
			pNew = fuiFactory::GetInstace()->CreateControl(pSubNode->GetName(), this, tName);
		}

		// 设置父对象
		pNew->SetParent(pParent);

		// 枚举所有属性并设置
		fcyXmlNode::AttributeIterator tIter = pSubNode->GetFirstAttribute();
		while(tIter != pSubNode->GetLastAttribute())
		{
			if(wcscmp(tIter.GetName(), L"Name") != 0)
			{
				pNew->RawSetProperty(tIter.GetName(), tIter.GetContent());
			}

			++tIter;
		}

		// 触发外观变化事件
		pNew->ExecEvent(L"OnStyleChanged");

		// 递归设置子控件
		loadLayoutNode(pSubNode, pNew);
	}
}

void fuiPage::debugDraw(fuiGraphics* pGraph, fuiControl* pControl)
{
	pGraph->PushOffset(pControl->m_Rect.a);

	for(fuInt i = 0; i<pControl->m_SubControlList.size(); ++i)
	{
		fuiControl* p = pControl->m_SubControlList[i];

		debugDraw(pGraph, p);
		
		if(p == m_pControlAtMousePos)
		{
			// 绘制鼠标处控件
			pGraph->GetGeometry()->SetPenColor(0, 0xF0F0F000);
			pGraph->GetGeometry()->SetPenColor(1, 0xFFF0F000);
			pGraph->GetGeometry()->SetPenColor(2, 0xF0F0F000);

			pGraph->GetGeometry()->DrawRectangle(pGraph->GetGraphics(), p->GetRect());

			pGraph->GetGeometry()->SetPenColor(0, 0xF0FF0000);
			pGraph->GetGeometry()->SetPenColor(1, 0xFFFF0000);
			pGraph->GetGeometry()->SetPenColor(2, 0xF0FF0000);
		}
		else if(p == m_pFocus)
		{
			// 绘制焦点控件
			pGraph->GetGeometry()->SetPenColor(0, 0xF0F0F0F0);
			pGraph->GetGeometry()->SetPenColor(1, 0xFFF0F0F0);
			pGraph->GetGeometry()->SetPenColor(2, 0xF0F0F0F0);

			pGraph->GetGeometry()->DrawRectangle(pGraph->GetGraphics(), p->GetRect());

			pGraph->GetGeometry()->SetPenColor(0, 0xF0FF0000);
			pGraph->GetGeometry()->SetPenColor(1, 0xFFFF0000);
			pGraph->GetGeometry()->SetPenColor(2, 0xF0FF0000);
		}
		else
		{
			pGraph->GetGeometry()->DrawRectangle(pGraph->GetGraphics(), p->GetRect());
		}
	}

	pGraph->PopOffset();
}

fuiControl* fuiPage::getControlAtPos(fuiControl* pControl, const fcyVec2& Pos, fcyVec2& PosOut)
{
	if(pControl->GetMouseTrans())
		return NULL;

	fcyVec2 tPos = Pos - pControl->m_Rect.a;

	if(pControl->m_Rect.Contain(Pos) && pControl->HitTest(tPos))
	{
		vector<fcyRefPointer<fuiControl>>::reverse_iterator i = pControl->m_SubControlList.rbegin();

		while(i != pControl->m_SubControlList.rend())
		{
			fuiControl* p = *i;

			fuiControl* pRet = getControlAtPos(p, tPos, PosOut);
	
			if(pRet)
				return pRet;
			else
				++i;
		}

		PosOut = tPos;
		return pControl;
	}
	else
		return NULL;
}

void fuiPage::execStyleChanged(fuiControl* p)
{
	// 触发所有控件的OnStyleChanged事件
	for(fuInt i = 0; i<p->m_SubControlList.size(); ++i)
	{
		p->ExecEvent(L"OnStyleChanged");

		execStyleChanged(p->m_SubControlList[i]);
	}
}

void fuiPage::sendMouseMove(const fcyVec2& MousePos)
{
	m_MouseLastPos = MousePos;

	// 发送全局消息
	{
		fuiPositionEventArgs tArgs;
		tArgs.SetPos(MousePos);
		ExecEvent(L"OnGlobalMouseMove", &tArgs);
	}

	m_pControlAtMousePos = getControlAtPos(this, MousePos, m_ControlMousePos);

	if(m_pLockMouseControl) // 若鼠标事件被锁定
	{
		// 发送鼠标移动消息
		fuiPositionEventArgs tArgs;
		tArgs.SetPos(MousePos - m_ControlOrigin);
		m_pLastMouseMoveControl->ExecEvent(L"OnMouseMove", &tArgs);
	}
	else // 未被锁定
	{
		// 对象发生变化
		if(m_pLastMouseMoveControl != m_pControlAtMousePos)
		{
			// 发送鼠标移出消息
			if(m_pLastMouseMoveControl)
				m_pLastMouseMoveControl->ExecEvent(L"OnMouseLeave");

			m_pLastMouseMoveControl = m_pControlAtMousePos;
		}

		// 发送鼠标移动消息
		if(m_pLastMouseMoveControl)
		{
			fuiPositionEventArgs tArgs;
			tArgs.SetPos(m_ControlMousePos);
			m_pLastMouseMoveControl->ExecEvent(L"OnMouseMove", &tArgs);
		}

		// 计算当前控件原点
		m_ControlOrigin = MousePos - m_ControlMousePos;
	}
}

void fuiPage::sendMouseButtonDown(MOUSEBUTTON Button, fcyVec2* MousePos)
{
	// 发送全局消息
	{
		switch(Button)
		{
		case MOUSEBUTTON_L:
			ExecEvent(L"OnGlobalMouseLDown");
			break;
		case MOUSEBUTTON_M:
			ExecEvent(L"OnGlobalMouseMDown");
			break;
		case MOUSEBUTTON_R:
			ExecEvent(L"OnGlobalMouseRDown");
			break;
		}
	}

	if(MousePos)
		sendMouseMove(*MousePos);

	if(!m_pLockMouseControl) // 未锁定，则锁定鼠标消息
	{
		m_pLockMouseControl = m_pControlAtMousePos;
		m_MouseUnlockEvent = Button;

		// 处理焦点
		if(m_pFocus != m_pLockMouseControl)
		{
			if(m_pFocus)
			{
				m_pFocus->ExecEvent(L"OnLostFocus");
				sendSubControlLostFocusMsg(m_pFocus);
			}

			m_pFocus = m_pLockMouseControl;

			if(m_pFocus)
			{
				m_pFocus->ExecEvent(L"OnGetFocus");
				sendSubControlGetFocusMsg(m_pFocus);
			}
		}
	}
	
	// 直接向控件发送消息
	if(m_pLockMouseControl)
	{
		switch(Button)
		{
		case MOUSEBUTTON_L:
			m_pLockMouseControl->ExecEvent(L"OnMouseLDown");
			break;
		case MOUSEBUTTON_M:
			m_pLockMouseControl->ExecEvent(L"OnMouseMDown");
			break;
		case MOUSEBUTTON_R:
			m_pLockMouseControl->ExecEvent(L"OnMouseRDown");
			break;
		}
	}
}

void fuiPage::sendMouseButtonUp(MOUSEBUTTON Button, fcyVec2* MousePos)
{
	// 发送全局消息
	{
		switch(Button)
		{
		case MOUSEBUTTON_L:
			ExecEvent(L"OnGlobalMouseLDown");
			break;
		case MOUSEBUTTON_M:
			ExecEvent(L"OnGlobalMouseMDown");
			break;
		case MOUSEBUTTON_R:
			ExecEvent(L"OnGlobalMouseRDown");
			break;
		}
	}

	if(MousePos)
		sendMouseMove(*MousePos);

	if(m_pLockMouseControl)
	{
		switch(Button)
		{
		case MOUSEBUTTON_L:
			m_pLockMouseControl->ExecEvent(L"OnMouseLUp");
			break;
		case MOUSEBUTTON_M:
			m_pLockMouseControl->ExecEvent(L"OnMouseMUp");
			break;
		case MOUSEBUTTON_R:
			m_pLockMouseControl->ExecEvent(L"OnMouseRUp");
			break;
		}

		if(m_MouseUnlockEvent == Button) // 解锁
			m_pLockMouseControl = NULL;
	}
}

void fuiPage::RegisterControl(fuiControl* pControl)
{
	if(pControl == this)
		return;
	if(!pControl)
		throw fcyException("fuiPage::RegisterControl", "Param 'pControl' is null.");
	if(m_ControlMap.find(pControl->GetName()) != m_ControlMap.end())
		throw fcyException("fuiPage::RegisterControl", "Control name is already existed.");

	m_ControlMap[pControl->GetName()] = pControl;
}

void fuiPage::UnregisterControl(fuiControl* pControl)
{
	if(pControl == this)
		return;
	
	if(pControl == m_pControlAtMousePos)
		m_pControlAtMousePos = NULL;
	if(pControl == m_pLastMouseMoveControl)
		m_pLastMouseMoveControl = NULL;
	if(pControl == m_pLockMouseControl)
		m_pLockMouseControl = NULL;
	if(pControl == m_pFocus)
	{
		sendSubControlLostFocusMsg(pControl);
		m_pFocus = NULL;
	}

	if(!pControl)
		throw fcyException("fuiPage::RegisterControl", "Param 'pControl' is null.");

	unordered_map<std::wstring, fuiControl*>::iterator i = m_ControlMap.find(pControl->GetName());
	if(i == m_ControlMap.end())
		throw fcyException("fuiPage::RegisterControl", "Control name is not exist.");

	m_ControlMap.erase(i);
}

void fuiPage::LoadLayoutFromFile(fcyStream* pStream)
{
	if(!pStream)
		throw fcyException("fuiPage::LoadLayoutFromFile", "Param 'pStream' is null.");
	pStream->SetPosition(FCYSEEKORIGIN_BEG, 0);

	fcyXml tXml(pStream);
	fcyXmlNode* pXmlRoot = tXml.GetRoot();

	// 加载布局
	loadLayoutNode(pXmlRoot, this);
}

fuiControl* fuiPage::FindControl(const std::wstring& Name)
{
	unordered_map<std::wstring, fuiControl*>::iterator i = m_ControlMap.find(Name);
	if(i == m_ControlMap.end())
		return NULL;
	else
		return i->second;
}

fuiStyle* fuiPage::GetControlStyle()const
{
	return m_pDefaultStyle;
}

void fuiPage::SetControlStyle(fuiStyle* pStyle)
{
	m_pDefaultStyle = pStyle;

	execStyleChanged(this);
}

void fuiPage::SetFocusControl(fuiControl* p)
{
	if(p && p->GetParent() != this)
		throw fcyException("fuiPage::SetFocusControl", "Invalid control.");

	m_pLockMouseControl = NULL;
	if(m_pFocus)
	{
		m_pFocus->ExecEvent(L"OnLostFocus");
		sendSubControlLostFocusMsg(m_pFocus);
	}

	m_pFocus = p;

	if(m_pFocus)
	{
		m_pFocus->ExecEvent(L"OnGetFocus");
		sendSubControlGetFocusMsg(m_pFocus);
	}
}

void fuiPage::Update(fDouble ElapsedTime)
{
	fuiControl::Update(ElapsedTime);
}

void fuiPage::Render(fuiGraphics* pGraph)
{
	if(pGraph == NULL)
		pGraph = &m_pGraphics;

	// 绘制所有控件
	if(FCYFAILED(pGraph->Begin()))
		return;

	if(GetClip())
		pGraph->PushClipRect(m_Rect);
	pGraph->PushOffset(m_Rect.a);

	if(GetVisiable())
		fuiControl::Render(pGraph);

	if(GetClip())
		pGraph->PopClipRect();
	pGraph->PopOffset();

	pGraph->End();

	// 调试绘图
	if(m_bDebug)
	{
		if(FCYFAILED(pGraph->Begin()))
			return;

		pGraph->GetGeometry()->SetPenSize(2.f);

		// 绘制边框
		pGraph->GetGeometry()->SetPenColor(0, 0xF0FFFFFF);
		pGraph->GetGeometry()->SetPenColor(1, 0xFFFFFFFF);
		pGraph->GetGeometry()->SetPenColor(2, 0xF0FFFFFF);

		pGraph->GetGeometry()->DrawRectangle(pGraph->GetGraphics(), GetRect());

		// 绘制控件边框
		pGraph->GetGeometry()->SetPenColor(0, 0xF0FF0000);
		pGraph->GetGeometry()->SetPenColor(1, 0xFFFF0000);
		pGraph->GetGeometry()->SetPenColor(2, 0xF0FF0000);

		debugDraw(pGraph, this);

		pGraph->End();
	}
}

void fuiPage::DealF2DMsg(const f2dMsg& Msg)
{
	switch(Msg.Type)
	{
	case F2DMSG_WINDOW_ONMOUSEMOVE:
		sendMouseMove(fcyVec2((float)(fLong)Msg.Param1, (float)(fLong)Msg.Param2));
		break;
	case F2DMSG_WINDOW_ONMOUSELDOWN:
		sendMouseButtonDown(fuiPage::MOUSEBUTTON_L, &fcyVec2((float)(fLong)Msg.Param1, (float)(fLong)Msg.Param2));
		break;
	case F2DMSG_WINDOW_ONMOUSELUP:
		sendMouseButtonUp(fuiPage::MOUSEBUTTON_L, &fcyVec2((float)(fLong)Msg.Param1, (float)(fLong)Msg.Param2));
		break;
	case F2DMSG_WINDOW_ONMOUSERDOWN:
		sendMouseButtonDown(fuiPage::MOUSEBUTTON_R, &fcyVec2((float)(fLong)Msg.Param1, (float)(fLong)Msg.Param2));
		break;
	case F2DMSG_WINDOW_ONMOUSERUP:
		sendMouseButtonUp(fuiPage::MOUSEBUTTON_R, &fcyVec2((float)(fLong)Msg.Param1, (float)(fLong)Msg.Param2));
		break;
	case F2DMSG_WINDOW_ONMOUSEMDOWN:
		sendMouseButtonDown(fuiPage::MOUSEBUTTON_M, &fcyVec2((float)(fLong)Msg.Param1, (float)(fLong)Msg.Param2));
		break;
	case F2DMSG_WINDOW_ONMOUSEMUP:
		sendMouseButtonUp(fuiPage::MOUSEBUTTON_M, &fcyVec2((float)(fLong)Msg.Param1, (float)(fLong)Msg.Param2));
		break;
	case F2DMSG_WINDOW_ONKEYDOWN:
		if(m_pFocus)
		{
			fuiKeyEventArgs tArgs;
			tArgs.SetKeyCode(fuiPage::VKKeyToF2DKey((fuInt)Msg.Param1));
			m_pFocus->ExecEvent(L"OnKeyDown", &tArgs);
		}
		break;
	case F2DMSG_WINDOW_ONKEYUP:
		if(m_pFocus)
		{
			fuiKeyEventArgs tArgs;
			tArgs.SetKeyCode(fuiPage::VKKeyToF2DKey((fuInt)Msg.Param1));
			m_pFocus->ExecEvent(L"OnKeyUp", &tArgs);
		}
		break;
	case F2DMSG_WINDOW_ONMOUSEWHEEL:
		if(m_pFocus)
		{
			fuiPositionEventArgs tArgs;
			tArgs.SetPos(fcyVec2((float)*(fDouble*)&Msg.Param3, 0));
			m_pFocus->ExecEvent(L"OnMouseWheel", &tArgs);
		}
		break;
	case F2DMSG_WINDOW_ONCHARINPUT:
		if(m_pFocus)
		{
			fuiCharEventArgs tArgs;
			tArgs.SetChar((fCharW)Msg.Param1);
			m_pFocus->ExecEvent(L"OnCharInput", &tArgs);
		}
		break;
	case F2DMSG_IME_ONCLOSE:
		ExecEvent(L"OnIMEClosed");
		break;
	case F2DMSG_IME_ONACTIVATE:
		{
			fuiStringEventArgs tArgs;
			tArgs.SetString((fcStrW)Msg.Param1);
			ExecEvent(L"OnIMEActivated", &tArgs);
		}
		break;
	case F2DMSG_IME_ONOPENCANDIDATE:
		{
			fuiIMECandidateEventArgs tArgs;
			tArgs.SetList((f2dIMECandidateList*)Msg.Param1);
			ExecEvent(L"OnIMEOpenCandidate", &tArgs);
		}
		break;
	case F2DMSG_IME_ONCLOSECANDIDATE:
		{
			fuiIMECandidateEventArgs tArgs;
			tArgs.SetList((f2dIMECandidateList*)Msg.Param1);
			ExecEvent(L"OnIMECloseCandidate", &tArgs);
		}
		break;
	case F2DMSG_IME_ONCHANGECANDIDATE:
		{
			fuiIMECandidateEventArgs tArgs;
			tArgs.SetList((f2dIMECandidateList*)Msg.Param1);
			ExecEvent(L"OnIMEChangeCandidate", &tArgs);
		}
		break;
	case F2DMSG_IME_ONSTARTCOMPOSITION:
		ExecEvent(L"OnIMEStartComposition");
		break;
	case F2DMSG_IME_ONENDCOMPOSITION:
		ExecEvent(L"OnIMEEndComposition");
		break;
	case F2DMSG_IME_ONCOMPOSITION:
		{
			fuiStringEventArgs tArgs;
			tArgs.SetString((fcStrW)Msg.Param1);
			ExecEvent(L"OnIMEComposition", &tArgs);
		}
		break;
	}
}
