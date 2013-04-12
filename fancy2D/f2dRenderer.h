////////////////////////////////////////////////////////////////////////////////
/// @file   f2dRenderer.h
/// @brief  fancy2D引擎渲染器接口定义
/// @note   定义了f2D的渲染器接口
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "f2dInterface.h"

/// @addtogroup f2d渲染系统
/// @brief fancy2d引擎中的渲染系统，提供对2D图形的渲染支持。
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @brief   fancy2D纹理
/// @note    为简化操作，约定所有的纹理格式均为A8R8G8B8。此方法创建的RenderTarget不支持抗锯齿。
/// @warning 动态纹理和渲染目标不会创建Mipmaps链，大量使用这种纹理可能会拖慢渲染速度
////////////////////////////////////////////////////////////////////////////////
struct f2dTexture : public f2dInterface
{
	virtual fBool IsDynamic()=0;       ///< @brief 是否是动态纹理
	virtual fBool IsRenderTarget()=0;  ///< @brief 是否是渲染目标
	virtual fuInt GetWidth()=0;        ///< @brief 获得宽度
	virtual fuInt GetHeight()=0;       ///< @brief 获得高度

	/// @brief      锁定纹理
	/// @note       只有动态纹理可以被锁定。锁定操作和解锁操作配对，在未解锁前只能锁定一次。
	/// @param[in]  pLockRect 被锁定的区域，设为NULL则锁定整个纹理
	/// @param[in]  Discard   如果原始数据正在被使用则抛弃原始数据
	/// @param[out] Pitch     行对齐字节
	/// @param[out] pOut      返回的内存指针，自动计算到区域的首字节
	virtual fResult Lock(fcyRect* pLockRect, fBool Discard, fuInt* Pitch, fData* pOut)=0;

	/// @brief 解锁纹理
	virtual fResult Unlock()=0;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 混合运算符
////////////////////////////////////////////////////////////////////////////////
enum F2DBLENDOPERATOR
{
	F2DBLENDOPERATOR_ADD         = 1, ///< @brief 混合结果 = 源像素 + 目标像素
	F2DBLENDOPERATOR_SUBTRACT    = 2, ///< @brief 混合结果 = 源像素 - 目标像素
	F2DBLENDOPERATOR_REVSUBTRACT = 3, ///< @brief 混合结果 = 目标像素 - 源像素
	F2DBLENDOPERATOR_MIN         = 4, ///< @brief 混合结果 = MIN(源像素, 目标像素)
	F2DBLENDOPERATOR_MAX         = 5  ///< @brief 混合结果 = MAX(源像素, 目标像素)
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 混合因素
/// @note  颜色混合公式：Color = SrcColor * SrcBlend + DestColor * DestBlend
////////////////////////////////////////////////////////////////////////////////
enum F2DBLENDFACTOR
{
    F2DBLENDFACTOR_ZERO               = 1,   ///< @brief 混合因素(0, 0, 0, 0)
    F2DBLENDFACTOR_ONE                = 2,   ///< @brief 混合因素(1, 1, 1, 1)
    F2DBLENDFACTOR_SRCCOLOR           = 3,   ///< @brief 混合因素(Rs, Gs, Bs, As)
    F2DBLENDFACTOR_INVSRCCOLOR        = 4,   ///< @brief 混合因素(1 - Rs, 1 - Gs, 1 - Bs, 1 - As)
    F2DBLENDFACTOR_SRCALPHA           = 5,   ///< @brief 混合因素(As, As, As, As)
    F2DBLENDFACTOR_INVSRCALPHA        = 6,   ///< @brief 混合因素(1 - As, 1 - As, 1 - As, 1 - As)
    F2DBLENDFACTOR_DESTALPHA          = 7,   ///< @brief 混合因素(Ad, Ad, Ad, Ad)
    F2DBLENDFACTOR_INVDESTALPHA       = 8,   ///< @brief 混合因素(1 - Ad, 1 - Ad, 1 - Ad, 1 - Ad)
    F2DBLENDFACTOR_DESTCOLOR          = 9,   ///< @brief 混合因素(Rd, Gd, Bd, Ad)
    F2DBLENDFACTOR_INVDESTCOLOR       = 10,  ///< @brief 混合因素(1 - Rd, 1 - Gd, 1 - Bd, 1 - Ad)
    F2DBLENDFACTOR_SRCALPHASAT        = 11   ///< @brief 混合因素(f, f, f, 1)，其中f = min(As, 1 - Ad)
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 纹理颜色混合公式
////////////////////////////////////////////////////////////////////////////////
enum F2DTEXOPERATOR
{
	F2DTEXOPT_DISABLE                     = 1,  ///< @brief 关闭
	F2DTEXOPT_SELECTARG1                  = 2,  ///< @brief Srgba = Arg1
	F2DTEXOPT_SELECTARG2                  = 3,  ///< @brief Srgba = Arg2
	F2DTEXOPT_MODULATE                    = 4,  ///< @brief Srgba = Arg1 x Arg2
	F2DTEXOPT_MODULATE2X                  = 5,  ///< @brief Srgba = (Arg1 x Arg2)<<1
	F2DTEXOPT_MODULATE4X                  = 6,  ///< @brief Srgba = (Arg1 x Arg2)<<2
	F2DTEXOPT_ADD                         = 7,  ///< @brief Srgba = Arg1 + Arg2
	F2DTEXOPT_ADDSIGNED                   = 8,  ///< @brief Srgba = Arg1 + Arg2 - 0.5
	F2DTEXOPT_ADDSIGNED2X                 = 9,  ///< @brief Srgba = (Arg1 + Arg2 - 0.5)<<1
	F2DTEXOPT_SUBTRACT                    = 10, ///< @brief Srgba = Arg1 - Arg2
	F2DTEXOPT_ADDSMOOTH                   = 11, ///< @brief Srgba = Arg1 + Arg2 * (1 - Arg1)
	F2DTEXOPT_BLENDDIFFUSEALPHA           = 12, 
	F2DTEXOPT_BLENDTEXTUREALPHA           = 13,
	F2DTEXOPT_BLENDFACTORALPHA            = 14,
	F2DTEXOPT_BLENDTEXTUREALPHAPM         = 15,
	F2DTEXOPT_BLENDCURRENTALPHA           = 16,
	F2DTEXOPT_PREMODULATE                 = 17,
	F2DTEXOPT_MODULATEALPHA_ADDCOLOR      = 18,
	F2DTEXOPT_MODULATECOLOR_ADDALPHA      = 19,
	F2DTEXOPT_MODULATEINVALPHA_ADDCOLOR   = 20,
	F2DTEXOPT_MODULATEINVCOLOR_ADDALPHA   = 21,
	F2DTEXOPT_BUMPENVMAP                  = 22,
	F2DTEXOPT_BUMPENVMAPLUMINANCE         = 23,
	F2DTEXOPT_DOTPRODUCT3                 = 24,
	F2DTEXOPT_MULTIPLYADD                 = 25,
	F2DTEXOPT_LERP                        = 26
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 纹理颜色混合源
////////////////////////////////////////////////////////////////////////////////
enum F2DTEXARG
{
	F2DTEXARG_DIFFUSE  = 0,  ///< @brief 选择漫反射颜色
	F2DTEXARG_CURRENT  = 1,  ///< @brief 选择上一层颜色
	F2DTEXARG_TEXTURE  = 2,  ///< @brief 选择纹理颜色
	F2DTEXARG_TFACTOR  = 3,  ///< @brief TFACTOR
	F2DTEXARG_SPECULAR = 4   ///< @brief 选择镜面颜色
};

////////////////////////////////////////////////////////////////////////////////
/// @brief Graphics渲染状态
////////////////////////////////////////////////////////////////////////////////
enum F2DRENDERSTATE
{
	F2DRENDERSTATE_BLENDOPT     = 0,   ///< @brief 混合运算符（对应F2DBLENDOPERATOR）
	F2DRENDERSTATE_BLENDSRC     = 1,   ///< @brief 混合源因子（对应F2DBLENDFACTOR）
	F2DRENDERSTATE_BLENDDEST    = 2,   ///< @brief 混合目的因子（对应F2DBLENDFACTOR）
	F2DRENDERSTATE_TEXCOLOROPT  = 3,   ///< @brief 纹理颜色通道混合运算符（对应F2DTEXOPERATOR）
	F2DRENDERSTATE_TEXCOLORARG1 = 4,   ///< @brief 纹理颜色通道混合参数1（对应F2DTEXARG）
	F2DRENDERSTATE_TEXCOLORARG2 = 5,   ///< @brief 纹理颜色通道混合参数2（对应F2DTEXARG）
	F2DRENDERSTATE_TEXALPHAOPT  = 6,   ///< @brief 纹理透明通道混合运算符（对应F2DTEXOPERATOR）
	F2DRENDERSTATE_TEXALPHAARG1 = 7,   ///< @brief 纹理透明通道混合参数1（对应F2DTEXARG）
	F2DRENDERSTATE_TEXALPHAARG2 = 8,   ///< @brief 纹理透明通道混合参数2（对应F2DTEXARG）
	F2DRENDERSTATE_MASK         = 9    ///< @brief 常量掩码，无效值
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D二维图形渲染器顶点
////////////////////////////////////////////////////////////////////////////////
struct f2dGraphics2DVertex
{
	fFloat x;      ///< @brief x坐标
	fFloat y;      ///< @brief y坐标
	fFloat z;      ///< @brief z坐标
	fuInt color;   ///< @brief 漫反射颜色
	fFloat u;      ///< @brief 纹理u坐标
	fFloat v;      ///< @brief 纹理v坐标
};

////////////////////////////////////////////////////////////////////////////////
/// @brief   fancy2D二维图形渲染器
/// @note    封装二维图形的渲染过程
/// @warning Graphics2D不允许嵌套使用
////////////////////////////////////////////////////////////////////////////////
struct f2dGraphics2D : public f2dInterface
{
	// --- 渲染状态管理 ---
	/// @brief     获得混合状态设置
	/// @param[in] State 渲染状态值
	/// @return    依据参数不同返回不同枚举类型
	virtual fuInt GetRenderState(F2DRENDERSTATE State)=0;
	/// @brief     设置混合状态设置
	/// @param[in] State 渲染状态值
	/// @param[in] Value 参数值
	virtual void SetRenderState(F2DRENDERSTATE State, fuInt Value)=0;
	/// @brief      返回变换矩阵
	/// @param[out] pOut 输出矩阵
	virtual void GetProjTransform(fcyMatrix4* pOut)=0;
	/// @brief     设置变换矩阵
	/// @param[in] pTrans 变换矩阵
	virtual void SetProjTransform(const fcyMatrix4& pTrans)=0;
	/// @brief      返回变换矩阵
	/// @param[out] pOut 输出矩阵
	virtual void GetWorldTransform(fcyMatrix4* pOut)=0;
	/// @brief     设置变换矩阵
	/// @param[in] pTrans 变换矩阵
	virtual void SetWorldTransform(const fcyMatrix4& pTrans)=0;
	/// @brief 保存当前矩阵到堆栈
	virtual void PushWorldTransform()=0;
	/// @brief 弹出堆栈中保存的矩阵
	virtual void PopWorldTransform()=0;
	
	// --- 绘制控制 ---
	/// @brief   状态同步
	/// @note    将状态立即提交到设备中进行同步
	/// @warning 请勿过多依赖该操作
	virtual void SyncStates()=0;
	/// @brief   启动绘制
	/// @warning 请勿在Begin/End区块间对渲染设备进行任何状态上的切换
	virtual fResult Begin()=0;
	/// @brief 结束绘制
	virtual fResult End()=0;
	/// @brief 立即绘制所有图元
	virtual fResult Flush()=0;

	// --- 绘图操作 ---
	/// @brief     绘制四边形
	/// @param[in] pTex 纹理
	/// @param[in] v1   顶点1，左上角
	/// @param[in] v2   顶点2，右上角
	/// @param[in] v3   顶点3，右下角
	/// @param[in] v4   顶点4，左下角
	virtual fResult DrawQuad(
	  f2dTexture* pTex,
		const f2dGraphics2DVertex& v1, 
		const f2dGraphics2DVertex& v2, 
		const f2dGraphics2DVertex& v3, 
		const f2dGraphics2DVertex& v4
		)=0;
	/// @brief     绘制四边形
	/// @param[in] pTex 纹理
	/// @param[in] arr  四边形数组
	virtual fResult DrawQuad(f2dTexture* pTex, const f2dGraphics2DVertex* arr)=0;

	/// @brief     绘制原始图元
	/// @note      绘制2D图形时需要手动对顶点(x,y)偏移+0.5个像素
	/// @param[in] pTex       绑定的纹理
	/// @param[in] VertCount  顶点数量
	/// @param[in] IndexCount 索引数量
	/// @param[in] VertArr    顶点数组
	/// @param[in] IndexArr   索引数组
	virtual fResult DrawRaw(f2dTexture* pTex, fuInt VertCount, fuInt IndexCount, const f2dGraphics2DVertex* VertArr, const fuShort* IndexArr)=0;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D后期效果接口
////////////////////////////////////////////////////////////////////////////////
struct f2dPostEffect : public f2dInterface
{
	/// @brief     返回浮点变量值
	/// @param[in] ParamName 变量名
	/// @return    返回对应元素的值，失败返回0
	virtual fFloat GetFloat(fcStr ParamName)=0;

	/// @brief 返回浮点数组
	/// @param[in]  ParamName 变量名
	/// @param[out] Dest      目的数组
	/// @param[in]  Count     元素个数
	virtual fResult GetFloatArr(fcStr ParamName, fFloat* Dest, fuInt Count)=0;

	/// @brief 设置浮点变量值
	/// @param[in] ParamName 变量名
	/// @param[in] Value     元素值
	virtual fResult SetValue(fcStr ParamName, fFloat Value)=0;

	/// @brief 设置浮点数组元素
	/// @param[in] ParamName 变量名
	/// @param[in] Index     元素下标
	/// @param[in] Value     元素值
	virtual fResult SetValue(fcStr ParamName, fuInt Index, fFloat Value)=0;

	/// @brief 设置浮点数组
	/// @param[in] ParamName 变量名
	/// @param[in] Src       原始值
	/// @param[in] Count     元素数量
	virtual fResult SetFloatArr(fcStr ParamName, const fFloat* Src, fuInt Count)=0;

	/// @brief 设置纹理
	/// @param[in] ParamName 变量名
	/// @param[in] pTex      纹理指针
	virtual fResult SetTexture(fcStr ParamName, f2dTexture* pTex)=0;

	/// @brief 启动后期特效，收集画面
	virtual fResult Begin()=0;

	/// @brief 完成后期特效，进行特效渲染
	virtual fResult End()=0;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D渲染设备事件监听器
/// @note  当设备丢失或者重置时引发该事件
////////////////////////////////////////////////////////////////////////////////
struct f2dRenderDeviceEventListener
{
	/// @brief 设备丢失事件
	virtual void OnRenderDeviceLost(){}

	/// @brief 设备重置事件
	virtual void OnRenderDeviceReset(){}
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D抗锯齿等级
////////////////////////////////////////////////////////////////////////////////
enum F2DAALEVEL
{
	F2DAALEVEL_NONE = 0,     ///< @brief 关闭抗锯齿
	F2DAALEVEL_2    = 1,     ///< @brief 2x抗锯齿
	F2DAALEVEL_4    = 2,     ///< @brief 4x抗锯齿
	F2DAALEVEL_8    = 3,     ///< @brief 8x抗锯齿
	F2DAALEVEL_16   = 4      ///< @brief 16x抗锯齿
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D渲染设备
/// @note  负责渲染对象的创建和设备管理。
////////////////////////////////////////////////////////////////////////////////
struct f2dRenderDevice
{
	/// @brief   返回D3D设备指针
	/// @note    需要强转到IDirect3DDevice9
	/// @warning 如无特殊需要，应当避免使用该方法
	virtual void* GetHandle()=0;
	
	// --- 监听器 ---
	/// @brief     挂接一个消息监听器
	/// @param[in] Listener 要绑定的监听器
	virtual fResult AttachListener(f2dRenderDeviceEventListener* Listener)=0;
	
	/// @brief     移除一个消息监听器
	/// @param[in] Listener 要移除的监听器
	virtual fResult RemoveListener(f2dRenderDeviceEventListener* Listener)=0;

	// --- 纹理创建 ---
	/// @brief      创建一个动态纹理
	/// @note       动态纹理之中的数据会在设备丢失之后消失。
	/// @param[in]  Width  宽度
	/// @param[in]  Height 高度
	/// @param[out] pOut   输出的纹理指针
	virtual fResult CreateDynamicTexture(fuInt Width, fuInt Height, f2dTexture** pOut)=0;

	/// @brief      从数据流创建纹理
	/// @note       可以选择性创建动态/静态纹理
	/// @param[in]  pStream   数据流，将读取整个数据流
	/// @param[in]  Width     宽度，设为0将使用数据流中图像默认宽度
	/// @param[in]  Height    高度，设为0将使用数据流中图像默认高度
	/// @param[in]  IsDynamic 是否为动态纹理
	/// @param[in]  HasMipmap 创建Mipmap链，用于加快图像渲染，对动态纹理和渲染目标无效。推荐设为true
	/// @param[out] pOut      输出的纹理指针
	virtual fResult CreateTextureFromStream(f2dStream* pStream, fuInt Width, fuInt Height, fBool IsDynamic, fBool HasMipmap, f2dTexture** pOut)=0;

	/// @brief      创建一个渲染目标
	/// @note       渲染目标用于存放渲染数据，不可锁定
	/// @warning    引擎尚不支持创建Z缓冲区，故渲染目标大小应和后备缓冲区大小一致
	/// @param[in]  Width  宽度
	/// @param[in]  Height 高度
	/// @param[out] pOut   输出的纹理指针
	virtual fResult CreateRenderTarget(fuInt Width, fuInt Height, f2dTexture** pOut)=0;

	/// @brief      创建一个2D渲染器
	/// @param[in]  VertexBufferSize 内部顶点缓冲区大小，置为0则默认
	/// @param[in]  IndexBufferSize  内部索引缓冲区大小，置为0则默认
	/// @param[out] pOut             输出指针
	virtual fResult CreateGraphics2D(fuInt VertexBufferSize, fuInt IndexBufferSize, f2dGraphics2D** pOut)=0;

	/// @brief      创建一个后期效果
	/// @param[in]  pStream 效果代码(fx文件)，将读取整个流
	/// @param[out] pOut    输出的效果
	/// @param[out] pErrOut 错误信息，可空
	virtual fResult CreatePostEffect(f2dStream* pStream, f2dPostEffect** pOut, f2dStream** pErrOut)=0;

	// --- 绘图设置 ---
	/// @brief     清空Z缓冲和渲染目标
	/// @param[in] BackBufferColor 背景颜色
	/// @param[in] ZValue          Z深度值，无特殊情况应取1.0f
	virtual fResult Clear(const fcyColor& BackBufferColor = 0, fFloat ZValue = 1.0f)=0;

	/// @brief     清空渲染目标
	/// @param[in] BackBufferColor 背景颜色
	virtual fResult ClearColor(const fcyColor& BackBufferColor = 0)=0;

	/// @brief     清空Z缓冲
	/// @param[in] Value Z深度值，无特殊情况应取1.0f
	virtual fResult ClearZBuffer(fFloat Value=1.0f)=0;

	/// @brief 返回目前使用中的渲染目标
	/// @note  如果为默认后台缓冲区将返回NULL，不增加引用计数
	virtual f2dTexture* GetRenderTarget()=0;

	/// @brief 将一个渲染目标推入栈顶用于渲染
	/// @note  一轮渲染完成前要保证渲染目标栈置空
	virtual fResult PushRenderTarget(f2dTexture* pTarget)=0;

	/// @brief 将栈顶渲染目标弹出
	virtual fResult PopRenderTarget()=0;

	/// @brief 返回目前的裁剪矩形
	virtual fcyRect GetScissorRect()=0;

	/// @brief   设置裁剪矩形
	/// @warning 矩形应当小于RenderTarget大小
	/// @note    重新设置缓冲区大小时该值会被覆盖
	virtual fResult SetScissorRect(const fcyRect& pRect)=0;

	// --- 设备控制 ---
	/// @brief 返回显卡名称
	virtual fcStrW GetDeviceName()=0;

	/// @brief 返回缓冲区宽度
	virtual fuInt GetBufferWidth()=0;

	/// @brief 返回缓冲区高度
	virtual fuInt GetBufferHeight()=0;

	/// @brief 是否窗口化
	virtual fBool IsWindowed()=0;

	/// @brief 获得抗锯齿等级
	virtual F2DAALEVEL GetAALevel()=0;

	/// @brief 检查抗锯齿是否被支持
	/// @param[in] AALevel  要检测的抗锯齿等级
	/// @param[in] Windowed 是否窗口模式
	virtual fBool CheckMultiSample(F2DAALEVEL AALevel, fBool Windowed)=0;

	/// @brief     重设缓冲区
	/// @param[in] Width    缓冲区宽度
	/// @param[in] Height   缓冲区高度
	/// @param[in] Windowed 窗口化
	/// @param[in] VSync    垂直同步
	/// @param[in] AALevel  抗锯齿等级
	virtual fResult SetBufferSize(fuInt Width, fuInt Height, fBool Windowed, fBool VSync, F2DAALEVEL AALevel)=0;

	/// @brief     截屏
	/// @note      以PNG形式保存
	/// @param[in] pStream 输出的流
	virtual fResult SaveScreen(f2dStream* pStream)=0;

	/// @brief     保存纹理
	/// @note      以PNG形式保存
	/// @warning   静态纹理由于会被拉升到2次幂长宽，故本函数保存静态纹理意义不大
	/// @param[in] pStream 输出的流
	/// @param[in] pTex    要保存的纹理
	virtual fResult SaveTexture(f2dStream* pStream, f2dTexture* pTex)=0;

	/// @brief     调用UpdateLayeredWindow更新分层窗口
	/// @note      要求主窗口被设置为分层窗口
	/// @param[in] KeyColor 关键色
	/// @param[in] Alpha    透明度
	virtual fResult UpdateScreenToWindow(fcyColor KeyColor, fByte Alpha)=0;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D精灵翻转选项
////////////////////////////////////////////////////////////////////////////////
enum F2DSPRITEFLIP
{
	F2DSPRITEFLIP_NONE,  ///< @brief 无翻转
	F2DSPRITEFLIP_H,     ///< @brief 水平翻转
	F2DSPRITEFLIP_V,     ///< @brief 垂直翻转
	F2DSPRITEFLIP_HV     ///< @brief 水平垂直翻转
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D精灵
////////////////////////////////////////////////////////////////////////////////
struct f2dSprite : public f2dInterface
{
	/// @brief 获得精灵绑定的纹理
	virtual f2dTexture* GetTexture()=0;

	/// @brief     设置一个新的纹理
	/// @param[in] pTex 纹理
	virtual fResult SetTexture(f2dTexture* pTex)=0;

	/// @brief 返回对应纹理的矩形
	virtual fcyRect GetTexRect()=0;

	/// @brief 设置对应纹理的矩形
	virtual fResult SetTexRect(fcyRect Rect)=0;

	/// @brief 返回热点
	virtual fcyVec2 GetHotSpot()=0;

	/// @brief     设置热点
	/// @param[in] Point 热点
	virtual fResult SetHotSpot(fcyVec2 Point)=0;

	/// @brief 返回翻转方式
	virtual F2DSPRITEFLIP GetFlipType()=0;
	
	/// @brief     设置翻转方式
	/// @param[in] Type 翻转方式
	virtual fResult SetFlipType(F2DSPRITEFLIP Type)=0;

	/// @brief 返回Z值
	virtual fFloat GetZ()=0;

	/// @brief     设置Z值
	/// @param[in] Value 新的Z值
	virtual fResult SetZ(fFloat Value)=0;

	/// @brief     返回顶点的颜色
	/// @param[in] Index 顶点索引[0~3]，按照顺时针方向定位
	virtual fcyColor GetColor(fuInt Index)=0;

	/// @brief     设置所有顶点的颜色
	/// @param[in] Color 混合颜色
	virtual fResult SetColor(fcyColor Color)=0;

	/// @brief 设置顶点颜色
	/// @param[in] Index 顶点索引[0~3]，按照顺时针方向定位
	/// @param[in] Color 混合颜色
	virtual fResult SetColor(fuInt Index, fcyColor Color)=0;

	/// @brief 绘制精灵
	/// @param[in] pGraph 绘图对象指针
	/// @param[in] Dest   目标矩形
	virtual fResult Draw(f2dGraphics2D* pGraph, const fcyRect& Dest)=0;

	/// @brief 绘制精灵
	/// @param[in] pGraph 绘图对象指针
	/// @param[in] Dest   目标矩形
	/// @param[in] SubTex 子纹理区域[0~1, 0~1]
	virtual fResult Draw(f2dGraphics2D* pGraph, const fcyRect& Dest, const fcyRect& SubTex)=0;

	/// @brief 绘制精灵
	/// @param[in] pGraph 绘图对象指针
	/// @param[in] Center 精灵显示中心
	virtual fResult Draw(f2dGraphics2D* pGraph, const fcyVec2& Center)=0;

	/// @brief 绘制精灵
	/// @param[in] pGraph 绘图对象指针
	/// @param[in] Center 精灵显示中心
	/// @param[in] Scale  缩放
	virtual fResult Draw(f2dGraphics2D* pGraph, const fcyVec2& Center, const fcyVec2& Scale)=0;

	/// @brief 绘制精灵
	/// @param[in] pGraph 绘图对象指针
	/// @param[in] Center 精灵显示中心
	/// @param[in] Scale  缩放
	/// @param[in] SubTex 子纹理区域[0~1, 0~1]
	virtual fResult Draw(f2dGraphics2D* pGraph, const fcyVec2& Center, const fcyVec2& Scale, const fcyRect& SubTex)=0;

	/// @brief 绘制精灵
	/// @param[in] pGraph   绘图对象指针
	/// @param[in] Center   精灵显示中心
	/// @param[in] Scale    缩放
	/// @param[in] Rotation 顺时针旋转角度，弧度制。
	virtual fResult Draw(f2dGraphics2D* pGraph, const fcyVec2& Center, const fcyVec2& Scale, fFloat Rotation)=0;
	
	/// @brief 绘制精灵
	/// @param[in] pGraph   绘图对象指针
	/// @param[in] Center   精灵显示中心
	/// @param[in] Scale    缩放
	/// @param[in] Rotation 顺时针旋转角度，弧度制。
	/// @param[in] SubTex   子纹理区域[0~1, 0~1]
	virtual fResult Draw(f2dGraphics2D* pGraph, const fcyVec2& Center, const fcyVec2& Scale, fFloat Rotation, const fcyRect& SubTex)=0;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D字体渲染器
////////////////////////////////////////////////////////////////////////////////
struct f2dFontRenderer : public f2dInterface
{
	/// @brief 返回字体数
	virtual fuInt GetFaceCount()=0;

	/// @brief 返回行高
	virtual fFloat GetLineHeight()=0;

	/// @brief 返回内置缓冲纹理
	virtual f2dTexture* GetCache()=0;

	/// @brief 缓存文字
	virtual void Cache(fcStrW Text)=0;

	/// @brief 获得Z值
	virtual fFloat GetZ()=0;

	/// @brief     设置Z值
	/// @param[in] ZValue Z值
	virtual void SetZ(fFloat ZValue)=0;

	/// @brief     获得顶点颜色
	/// @param[in] Index 索引[0~3]
	virtual fcyColor GetColor(fuInt Index)=0;

	/// @brief     设置字体颜色
	/// @param[in] Color 字体颜色
	virtual void SetColor(const fcyColor& Color)=0;

	/// @brief     设置顶点颜色
	/// @param[in] Index 索引[0~3]
	/// @param[in] Color 颜色
	virtual void SetColor(fuInt Index, const fcyColor& Color)=0;

	/// @brief 获得翻转方式
	virtual F2DSPRITEFLIP GetFlipType()=0;

	/// @brief     设置翻转方式
	/// @param[in] Type 翻转方式
	virtual void SetFlipType(F2DSPRITEFLIP Type)=0;

	/// @brief 绘制文字
	/// @param[in] pGraph   渲染器
	/// @param[in] Text     文字
	/// @param[in] StartPos 绘制起始基准
	virtual fResult DrawTextW(f2dGraphics2D* pGraph, fcStrW Text, const fcyVec2& StartPos)=0;

	/// @brief 绘制文字
	/// @param[in]  pGraph   渲染器
	/// @param[in]  Text     文字
	/// @param[in]  Count    字数，设为-1则为全部文字
	/// @param[in]  StartPos 绘制起始基准
	/// @param[out] PosOut   绘制终止位置，可空
	virtual fResult DrawTextW(f2dGraphics2D* pGraph, fcStrW Text, fuInt Count, const fcyVec2& StartPos, fcyVec2* PosOut)=0;

	/// @brief 绘制文字
	/// @param[in]  pGraph   渲染器
	/// @param[in]  Text     文字
	/// @param[in]  Count    字数，设为-1则为全部文字
	/// @param[in]  Bias     倾斜角，0为不倾斜
	/// @param[in]  StartPos 绘制起始基准
	/// @param[out] PosOut   绘制终止位置，可空
	virtual fResult DrawTextW(f2dGraphics2D* pGraph, fcStrW Text, fuInt Count, fFloat Bias, const fcyVec2& StartPos, fcyVec2* PosOut)=0;

	/// @brief      在矩形中绘制文字
	/// @note       函数要求文字绘制在矩形区域中，对于超过边界会自动换行
	/// @warning    函数只会保证笔触位置在矩形区域中，不保证文字一定只渲染在范围内
	/// @param[in]  pGraph         渲染器
	/// @param[in]  Text           文字
	/// @param[in]  DestRect       绘制范围
	/// @param[out] pCountRendered 已绘制的文字数量，可以设为NULL
	virtual fResult DrawTextInRect(f2dGraphics2D* pGraph, fcStrW Text, const fcyRect& DestRect, fuInt* pCountRendered)=0;

	/// @brief      在矩形中绘制文字
	/// @note       函数要求文字绘制在矩形区域中，对于超过边界会自动换行
	/// @warning    函数只会保证笔触位置在矩形区域中，不保证文字一定只渲染在范围内
	/// @param[in]  pGraph         渲染器
	/// @param[in]  Text           文字
	/// @param[in]  Count          字数，设为-1则为全部文字
	/// @param[in]  DestRect       绘制范围
	/// @param[out] pCountRendered 已绘制的文字数量，可以设为NULL
	virtual fResult DrawTextInRect(f2dGraphics2D* pGraph, fcStrW Text, fuInt Count, const fcyRect& DestRect, fuInt* pCountRendered)=0;

	/// @brief     绘制高级文字
	/// @note      绘制使用@标记的高级风格文字
	/// @param[in] pGraph        渲染器
	/// @param[in] Text          文字
	/// @param[in] Count         字数，设为-1则为全部文字
	/// @param[in] StartPos      绘制起始基准
	/// @param[in] PosCache      位置缓存，可设为NULL
	/// @param[in] PosCacheCount 位置缓存数量
	virtual fResult DrawAdvancedText(f2dGraphics2D* pGraph, fcStrW Text, fuInt Count, const fcyVec2& StartPos, fcyVec2* PosCache, fuInt PosCacheCount)=0;

	/// @brief      限定区域绘制高级文字
	/// @note       绘制使用@标记的高级风格文字
	/// @param[in]  pGraph         渲染器
	/// @param[in]  Text           文字
	/// @param[in]  Count          字数，设为-1则为全部文字
	/// @param[in]  DestRect       目标矩形，保证笔触锁定在其中，越界时换行
	/// @param[in]  PosCache       位置缓存，可设为NULL
	/// @param[in]  PosCacheCount  位置缓存数量
	/// @param[out] pCountRendered 已渲染文字数量，可设为NULL
	virtual fResult DrawAdvancedTextInRect(f2dGraphics2D* pGraph, fcStrW Text, fuInt Count, const fcyRect& DestRect, fcyVec2* PosCache, fuInt PosCacheCount, fuInt* pCountRendered)=0;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D简单几何体渲染器
////////////////////////////////////////////////////////////////////////////////
struct f2dGeometryRenderer : public f2dInterface
{
	/// @brief 获得Z值
	virtual fFloat GetZ()=0;

	/// @brief     设置Z值
	/// @param[in] ZValue Z值
	virtual void SetZ(fFloat ZValue)=0;

	/// @brief     获得画笔的颜色
	/// @param[in] Index 索引[0~2]，0=外侧颜色，1=中间颜色，2=内侧颜色
	virtual fcyColor GetPenColor(fuInt Index)=0;

	/// @brief     设置画笔颜色
	/// @param[in] Index 索引[0~2]，0=外侧颜色，1=中间颜色，2=内侧颜色
	/// @param[in] Color 颜色值
	virtual void SetPenColor(fuInt Index, const fcyColor& Color)=0;

	/// @brief 获得画笔大小
	virtual fFloat GetPenSize()=0;

	/// @brief     设置画笔大小
	/// @param[in] Size 画笔大小
	virtual void SetPenSize(fFloat Size)=0;

	/// @brief     画线
	/// @param[in] pGraph 绘图指针
	/// @param[in] Begin  起点
	/// @param[in] End    终点
	virtual fResult DrawLine(f2dGraphics2D* pGraph, const fcyVec2& Begin, const fcyVec2& End)=0;

	/// @brief 绘制矩形
	/// @param[in] pGraph 绘图指针
	/// @param[in] Rect   矩形
	virtual fResult DrawRectangle(f2dGraphics2D* pGraph, const fcyRect& Rect)=0;

	/// @brief 绘制圆
	/// @param[in] pGraph 绘图指针
	/// @param[in] Center 中心
	/// @param[in] R      半径
	/// @param[in] Divide 细分
	virtual fResult DrawCircle(f2dGraphics2D* pGraph, const fcyVec2& Center, float R, fuInt Divide=50)=0;

	/// @brief 填充矩形
	/// @param[in] pGraph    绘图指针
	/// @param[in] Rect      填充区域
	/// @param[in] pColorArr 颜色数组[0~3]，顺时针四个角的颜色
	virtual fResult FillRectangle(f2dGraphics2D* pGraph, const fcyRect& Rect, const fcyColor* pColorArr)=0;

	/// @brief 填充圆
	/// @param[in] pGraph      绘图指针
	/// @param[in] Center      中心
	/// @param[in] R           半径
	/// @param[in] CenterColor 中心颜色
	/// @param[in] BoundColor  边界颜色
	/// @param[in] Divide      细分
	virtual fResult FillCircle(f2dGraphics2D* pGraph, const fcyVec2& Center, float R, const fcyColor& CenterColor, const fcyColor& BoundColor, fuInt Divide=50)=0;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D渲染器
////////////////////////////////////////////////////////////////////////////////
struct f2dRenderer
{
	virtual f2dRenderDevice* GetDevice()=0;  ///< @brief 获得渲染设备指针

	/// @brief      创建Sprite2D
	/// @param[in]  pTex 绑定的纹理
	/// @param[out] pOut 创建的Sprite
	virtual fResult CreateSprite2D(f2dTexture* pTex, f2dSprite** pOut)=0;

	/// @brief      创建Sprite2D
	/// @param[in]  pTex 绑定的纹理
	/// @param[in]  Org  原始纹理矩形
	/// @param[out] pOut 创建的Sprite
	virtual fResult CreateSprite2D(f2dTexture* pTex, const fcyRect& Org, f2dSprite** pOut)=0;

	/// @brief      创建Sprite2D
	/// @param[in]  pTex    绑定的纹理
	/// @param[in]  Org     原始纹理矩形
	/// @param[in]  HotSopt 热点
	/// @param[out] pOut    创建的Sprite
	virtual fResult CreateSprite2D(f2dTexture* pTex, const fcyRect& Org, const fcyVec2& HotSopt, f2dSprite** pOut)=0;

	/// @brief      创建文字渲染器
	/// @param[in]  pStream    字体文件流
	/// @param[in]  FontWidth  字体宽度，若设为0，则等于字体高度
	/// @param[in]  FontHeight 字体高度，若设为0，则等于字体宽度
	/// @param[in]  FaceIndex  字体索引，0总是一个有效值
	/// @param[out] pRenderer  输出的字体渲染器指针
	virtual fResult CreateFontRenderer(f2dStream* pStream, fuInt FontWidth, fuInt FontHeight, fuInt FaceIndex, f2dFontRenderer** pRenderer)=0;

	/// @brief 创建几何体渲染器
	virtual fResult CreateGeometryRenderer(f2dGeometryRenderer** pOut)=0;
};

/// @}
