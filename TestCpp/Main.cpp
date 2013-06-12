// 如果需要引入fcyStream则必须在f2d之前引入
#include <fcyIO/fcyStream.h>
#include <fcyFile/fcyModelMeshFile.h>
#include <fcyFile/fcyModelMeshExporter.h>
#include <fcyOS/fcyDebug.h>
#include <fcyOS/fcyMultiThread.h>

// UI库
#include <fcyUIBase/fuiPage.h>
#include <fcyUIBase/fuiFactory.h>

// f2d头文件
#include <f2d.h>

#include <iostream>
#include <string>

using namespace std;

class MyUIControl : 
	public fuiControl
{
public:
	MyUIControl(fuiPage* pRootPage, const std::wstring& Name)
		: fuiControl(pRootPage, Name)
	{
	}
};

class MyApp :
	public f2dEngineEventListener
{
private:
	// 必要组件
	fcyRefPointer<f2dEngine> m_pEngine;
	f2dFileSys* m_pFileSys;
	f2dRenderer* m_pRenderer;
	f2dRenderDevice* m_pDev;
	f2dSoundSys* m_pSoundSys;
	f2dInputSys* m_pInputSys;
	f2dVideoSys* m_pVideoSys;

	// 渲染器
	fcyRefPointer<f2dGraphics2D> m_pGraph2D;
	fcyRefPointer<f2dGraphics3D> m_pGraph3D;

	// FPS绘制用
	fcyRefPointer<f2dFontProvider> m_FPSFontProvider;
	fcyRefPointer<f2dFontRenderer> m_FPSFontRenderer;

	// 键盘
	fcyRefPointer<f2dInputKeyboard> m_KeyBoard;

	// 测试用
	fuiResProviderImpl tProvider;
	fcyRefPointer<fuiPage> m_pRootUIPage;
private:  // 功能函数
	void showSelfInfo()
	{
		f2dCPUInfo tInfo;

		m_pEngine->GetCPUInfo(tInfo);

		cout<<"f2d Version"<<(F2DVERSION>>16)<<"."<<(F2DVERSION & 0x0000FFFF)<<"\n"<<endl;

		cout<<"[ CPU制造商 ] "<<tInfo.CPUString<<endl;
		cout<<"[  CPU品牌  ] "<<tInfo.CPUBrandString<<"\n"<<endl;

		cout<<"[ 显卡名称 ] "<<m_pDev->GetDeviceName()<<"\n"<<endl;

		for(fuInt i = 0; i<m_pDev->GetSupportResolutionCount(); i++)
		{
			fcyVec2 t = m_pDev->EnumSupportResolution(i);
			cout<<"[ 分辨率 ] "<<(int)t.x<<"x"<<(int)t.y;

			cout<<" ( 抗锯齿等级 ";
			for(int j = F2DAALEVEL_NONE; j != F2DAALEVEL_16; j++)
			{
				if(m_pDev->CheckMultiSample((F2DAALEVEL)j, true))
				{
					switch(j)
					{
					case F2DAALEVEL_NONE:
						cout<<"x0 ";
						break;
					case F2DAALEVEL_2:
						cout<<"x2 ";
						break;
					case F2DAALEVEL_4:
						cout<<"x4 ";
						break;
					case F2DAALEVEL_8:
						cout<<"x8 ";
						break;
					case F2DAALEVEL_16:
						cout<<"x16 ";
						break;
					}
				}
			}
			cout<<" )"<<endl;
		}

		cout<<endl;
	}
	void drawFPS(f2dFPSController* pFPS, const std::wstring& AdditionData = L"")
	{
		m_pDev->ClearZBuffer();
		m_pGraph2D->Begin();

		fCharW tBuffer[1024];
		swprintf_s(tBuffer, L"FPS %4.2lf",
			pFPS->GetFPS()
			);

		m_FPSFontRenderer->SetColor(0xFF000000);
		m_FPSFontRenderer->DrawTextW(m_pGraph2D, tBuffer, fcyVec2(5.f, 5.f) + fcyVec2(0.f, m_FPSFontProvider->GetAscender()));
		m_FPSFontRenderer->SetColor(0xFF00FF00);
		m_FPSFontRenderer->DrawTextW(m_pGraph2D, tBuffer, fcyVec2(4.f, 4.f) + fcyVec2(0.f, m_FPSFontProvider->GetAscender()));

		m_pGraph2D->End();
	}
protected: // 引擎消息
	fBool OnUpdate(fDouble ElapsedTime, f2dFPSController* pFPSController, f2dMsgPump* pMsgPump)
	{
		// 消息处理
		f2dMsg tMsg;
		while(FCYOK(pMsgPump->GetMsg(&tMsg)))
		{
			switch(tMsg.Type)
			{
			case F2DMSG_WINDOW_ONCLOSE:
				m_pEngine->Abort();
				break;

			case F2DMSG_WINDOW_ONMOUSEMOVE:
				m_pRootUIPage->SendMouseMove(fcyVec2((float)tMsg.Param1, (float)tMsg.Param2));
				break;
			case F2DMSG_WINDOW_ONMOUSELDOWN:
				m_pRootUIPage->SendMouseButtonDown(fuiPage::MOUSEBUTTON_L, &fcyVec2((float)tMsg.Param1, (float)tMsg.Param2));
				break;
			case F2DMSG_WINDOW_ONMOUSELUP:
				m_pRootUIPage->SendMouseButtonUp(fuiPage::MOUSEBUTTON_L, &fcyVec2((float)tMsg.Param1, (float)tMsg.Param2));
				break;
			case F2DMSG_WINDOW_ONMOUSERDOWN:
				m_pRootUIPage->SendMouseButtonDown(fuiPage::MOUSEBUTTON_R, &fcyVec2((float)tMsg.Param1, (float)tMsg.Param2));
				break;
			case F2DMSG_WINDOW_ONMOUSERUP:
				m_pRootUIPage->SendMouseButtonUp(fuiPage::MOUSEBUTTON_R, &fcyVec2((float)tMsg.Param1, (float)tMsg.Param2));
				break;
			case F2DMSG_WINDOW_ONMOUSEMDOWN:
				m_pRootUIPage->SendMouseButtonDown(fuiPage::MOUSEBUTTON_M, &fcyVec2((float)tMsg.Param1, (float)tMsg.Param2));
				break;
			case F2DMSG_WINDOW_ONMOUSEMUP:
				m_pRootUIPage->SendMouseButtonUp(fuiPage::MOUSEBUTTON_M, &fcyVec2((float)tMsg.Param1, (float)tMsg.Param2));
				break;
			case F2DMSG_WINDOW_ONKEYDOWN:
				m_pRootUIPage->SendKeyDown(fuiPage::VKKeyToF2DKey(tMsg.Param1));
				break;
			case F2DMSG_WINDOW_ONKEYUP:
				m_pRootUIPage->SendKeyUp(fuiPage::VKKeyToF2DKey(tMsg.Param1));
				break;
			case F2DMSG_WINDOW_ONMOUSEWHEEL:
				m_pRootUIPage->SendMouseWheel(*(fDouble*)&tMsg.Param3);
				break;
			case F2DMSG_WINDOW_ONCHARINPUT:
				m_pRootUIPage->SendCharInput(tMsg.Param1);
				break;
			}
		}

		// 更新UI
		m_pRootUIPage->Update(ElapsedTime);

		return true;
	}
	fBool OnRender(fDouble ElapsedTime, f2dFPSController* pFPSController)
	{
		// 作图
		m_pDev->Clear();

		m_pGraph2D->Begin();

		m_pGraph2D->End();

		fCharW tBuffer[256];
		swprintf_s(tBuffer, L"FPS %4.2lf", pFPSController->GetFPS());
		m_pRootUIPage->FindControl(L"FPSLabel")->RawSetProperty(L"Text", tBuffer);

		// 绘制UI
		m_pRootUIPage->Render();

		// 绘制FPS数据
		// drawFPS(pFPSController);

		return true; 
	}
public:
	MyApp()
	{
		struct : public f2dInitialErrListener {
			void OnErr(fuInt TimeTick, fcStr Src, fcStr Desc)
			{
				fChar pBuffer[1024];
				sprintf_s(pBuffer, "初始化失败！\n时间戳：%u\n问题来源：%s\n错误描述：%s", TimeTick, Src, Desc);
				MessageBoxA(0, pBuffer, "f2d 初始化失败", MB_ICONERROR);
			}
		} tErrCallBack;

		// 创建引擎
		if(FCYFAILED(CreateF2DEngineAndInit(
			F2DVERSION,
			fcyRect(50.f, 50.f, 640.f + 50.f, 480.f + 50.f), // 640x480
			L"无标题",               // 标题
			true,
			false,  // VSYNC
			F2DAALEVEL_2,
			this,
			&*m_pEngine,
			&tErrCallBack)))
		{ return; }

		// 获得组件
		m_pFileSys = m_pEngine->GetFileSys();
		m_pRenderer = m_pEngine->GetRenderer();
		m_pDev = m_pEngine->GetRenderer()->GetDevice();
		m_pSoundSys = m_pEngine->GetSoundSys();
		m_pInputSys = m_pEngine->GetInputSys();
		m_pVideoSys = m_pEngine->GetVideoSys();

		// 信息自举
		showSelfInfo();

		// 加载资源
		{
			// 映射本地文件夹DemoRes到节点Res
			m_pFileSys->LoadRealPath(L"Res", L"E:\\Projects\\fancy2D\\TestCpp\\DemoRes\\");

			// 创建渲染器
			m_pDev->CreateGraphics2D(0, 0, &m_pGraph2D);
			m_pDev->CreateGraphics3D(NULL, &m_pGraph3D);
			m_pGraph3D->SetWorldTransform(fcyMatrix4::GetScaleMatrix(0.8f));
			m_pGraph3D->SetProjTransform(fcyMatrix4::GetPespctiveLH(4.f/3.f, 3.14f/4.f, 0.1f, 1000.f));
			m_pGraph3D->SetViewTransform(fcyMatrix4::GetLookAtLH(fcyVec3(0.f,0.f,100.f), fcyVec3(), fcyVec3(0.f,1.f,0.f)));

			// 创建键盘
			m_pInputSys->CreateKeyboard(-1, false, &m_KeyBoard);

			// 加载FPS字体
			m_pRenderer->CreateFontFromFile(m_pFileSys->GetStream(L"Res\\Droid Sans Fallback.ttf"), 0, fcyVec2(16, 16), F2DFONTFLAG_NONE, &m_FPSFontProvider);
			m_pRenderer->CreateFontRenderer(m_FPSFontProvider, &m_FPSFontRenderer);

			// 测试
			// m_pDev->CreateTextureFromStream(m_pFileSys->GetStream(L"Res\\Loading.png"), 0, 0, false, true, &m_pTexture);
			// m_pRenderer->CreateSprite2D(m_pTexture, &m_pSprite);

			fuiFactory::GetInstace()->RegisterControlCreator<MyUIControl>(L"MyUIControl");

			tProvider = fuiResProviderImpl(m_pFileSys, m_pRenderer);

			try
			{
				m_pRootUIPage.DirectSet(new fuiPage(L"Main", m_pRenderer, m_pGraph2D));
				m_pRootUIPage->GetControlStyle()->LoadResFromFile(m_pFileSys->GetStream(L"Res\\UIStyle.xml"), &tProvider);
				m_pRootUIPage->LoadLayoutFromFile(m_pFileSys->GetStream(L"Res\\UILayout.xml"));
				m_pRootUIPage->SetDebugMode(true);

				m_pRootUIPage->FindControl(L"Label_0")->GetEvent(L"OnCharInput") +=
					[&](fuiControl* pControl, fuiEventArgs* pArgs)
					{
						fuiCharEventArgs* tPos = (fuiCharEventArgs*)pArgs->QueryEvent(fuiEventArgs::EVENTTYPE_CHAR);
						
						pControl->RawSetProperty(L"Text", pControl->RawGetProperty(L"Text") + tPos->GetChar());
					};

				m_pRootUIPage->FindControl(L"Label")->GetEvent(L"OnMouseMove") +=
					[&](fuiControl* pControl, fuiEventArgs* pArgs)
					{
						fuiPositionEventArgs* pPosArgs = (fuiPositionEventArgs*)pArgs->QueryEvent(fuiEventArgs::EVENTTYPE_POSITION);

						fCharW tBuffer[256];
						swprintf_s(tBuffer, L"X=%.2f Y=%.2f", pPosArgs->GetPos().x, pPosArgs->GetPos().y);

						pControl->RawSetProperty(L"Text", tBuffer);
					};
				m_pRootUIPage->FindControl(L"Label")->GetEvent(L"OnMouseLeave") +=
					[&](fuiControl* pControl, fuiEventArgs* pArgs)
					{
						pControl->RawSetProperty(L"Text", L"鼠标移出");
					};
				m_pRootUIPage->FindControl(L"Label")->GetEvent(L"OnMouseLDown") +=
					[&](fuiControl* pControl, fuiEventArgs* pArgs)
					{
						pControl->RawSetProperty(L"Text", L"鼠标左键按下");
					};
				m_pRootUIPage->FindControl(L"Label")->GetEvent(L"OnMouseLUp") +=
					[&](fuiControl* pControl, fuiEventArgs* pArgs)
					{
						pControl->RawSetProperty(L"Text", L"鼠标左键放开");
					};
				m_pRootUIPage->FindControl(L"Label")->GetEvent(L"OnMouseMDown") +=
					[&](fuiControl* pControl, fuiEventArgs* pArgs)
					{
						pControl->RawSetProperty(L"Text", L"鼠标中键按下");
					};
				m_pRootUIPage->FindControl(L"Label")->GetEvent(L"OnMouseMUp") +=
					[&](fuiControl* pControl, fuiEventArgs* pArgs)
					{
						pControl->RawSetProperty(L"Text", L"鼠标中键放开");
					};
				m_pRootUIPage->FindControl(L"Label")->GetEvent(L"OnMouseRDown") +=
					[&](fuiControl* pControl, fuiEventArgs* pArgs)
					{
						pControl->RawSetProperty(L"Text", L"鼠标右键按下");
					};
				m_pRootUIPage->FindControl(L"Label")->GetEvent(L"OnMouseRUp") +=
					[&](fuiControl* pControl, fuiEventArgs* pArgs)
					{
						pControl->RawSetProperty(L"Text", L"鼠标右键放开");
					};
				m_pRootUIPage->FindControl(L"Label")->GetEvent(L"OnGetFocus") +=
					[&](fuiControl* pControl, fuiEventArgs* pArgs)
					{
						pControl->RawSetProperty(L"Text", L"获得焦点");
					};
				m_pRootUIPage->FindControl(L"Label")->GetEvent(L"OnLostFocus") +=
					[&](fuiControl* pControl, fuiEventArgs* pArgs)
					{
						pControl->RawSetProperty(L"Text", L"失去焦点");
					};
			}
			catch(const fcyException& e)
			{
				fChar pBuffer[1024];
				sprintf_s(pBuffer, "初始化失败！\n时间戳：%u\n问题来源：%s\n错误描述：%s", e.GetTime(), e.GetSrc(), e.GetDesc());
				MessageBoxA(0, pBuffer, "f2d 初始化失败", MB_ICONERROR);

				return;
			}
		}

		// 显示窗口
		m_pEngine->GetMainWindow()->SetVisiable(true);

		// 启动
		m_pEngine->Run(F2DENGTHREADMODE_MULTITHREAD);
	}
	~MyApp()
	{
	}
};

//int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int)
int main()
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
	// _CrtSetBreakAlloc(5351);
#endif
	MyApp tApp;

	// system("pause");
	return 0;
}
