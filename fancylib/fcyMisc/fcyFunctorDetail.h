#ifdef FCYMAKEFUNCTOR

#ifndef FCYFUNCTOR_MAKEARG
#define FCYFUNCTOR_MAKEARG

#define FCYFUNCTOR_MAKEARG1_0(P) 
#define FCYFUNCTOR_MAKEARG1_1(P) P##1
#define FCYFUNCTOR_MAKEARG1_2(P) P##1, P##2
#define FCYFUNCTOR_MAKEARG1_3(P) P##1, P##2, P##3
#define FCYFUNCTOR_MAKEARG1_4(P) P##1, P##2, P##3, P##4
#define FCYFUNCTOR_MAKEARG1_5(P) P##1, P##2, P##3, P##4, P##5
#define FCYFUNCTOR_MAKEARG1_6(P) P##1, P##2, P##3, P##4, P##5, P##6
#define FCYFUNCTOR_MAKEARG1_7(P) P##1, P##2, P##3, P##4, P##5, P##6, P##7
#define FCYFUNCTOR_MAKEARG1_8(P) P##1, P##2, P##3, P##4, P##5, P##6, P##7, P##8

#define FCYFUNCTOR_MAKEARG2_0(P,p) 
#define FCYFUNCTOR_MAKEARG2_1(P,p) P##1 p##1
#define FCYFUNCTOR_MAKEARG2_2(P,p) P##1 p##1, P##2 p##2
#define FCYFUNCTOR_MAKEARG2_3(P,p) P##1 p##1, P##2 p##2, P##3 p##3
#define FCYFUNCTOR_MAKEARG2_4(P,p) P##1 p##1, P##2 p##2, P##3 p##3, P##4 p##4
#define FCYFUNCTOR_MAKEARG2_5(P,p) P##1 p##1, P##2 p##2, P##3 p##3, P##4 p##4, P##5 p##5
#define FCYFUNCTOR_MAKEARG2_6(P,p) P##1 p##1, P##2 p##2, P##3 p##3, P##4 p##4, P##5 p##5, P##6 p##6
#define FCYFUNCTOR_MAKEARG2_7(P,p) P##1 p##1, P##2 p##2, P##3 p##3, P##4 p##4, P##5 p##5, P##6 p##6, P##7 p##7
#define FCYFUNCTOR_MAKEARG2_8(P,p) P##1 p##1, P##2 p##2, P##3 p##3, P##4 p##4, P##5 p##5, P##6 p##6, P##7 p##7, P##8 p##8

#define FCYFUNCTOR_ADD(a,b) a##b

#define FCYFUNCTOR_MAKEARG1(cnt, P)    FCYFUNCTOR_ADD(FCYFUNCTOR_MAKEARG1_, cnt)(P)
#define FCYFUNCTOR_MAKEARG2(cnt, P, p) FCYFUNCTOR_ADD(FCYFUNCTOR_MAKEARG2_, cnt)(P, p)

#endif

#if FCYMAKEFUNCTOR == 0
#define FCYFUNCTOR_COMMA
#else
#define FCYFUNCTOR_COMMA ,
#endif

// typename R, typename P1, typename P2, typename P3...
template<typename R FCYFUNCTOR_COMMA FCYFUNCTOR_MAKEARG1(FCYMAKEFUNCTOR, typename P)>
class fcyFunctor<R( FCYFUNCTOR_MAKEARG1(FCYMAKEFUNCTOR, P) )>  // R(P1,P2,P3...)
{
private:
	class fcyFunctorCallBack :
		public fcyRefObj
	{
	public:
		virtual R Invoke( FCYFUNCTOR_MAKEARG1(FCYMAKEFUNCTOR, P) )=0;  // (P1,P2,P3...)
	protected:
		virtual ~fcyFunctorCallBack() {}
	};

	template<typename StaticFunc>
	class fcyFunctorCallBack_StaticFunc :
		public fcyRefObjImpl<fcyFunctorCallBack>
	{
	private:
		StaticFunc m_pFunc;
	public:
		R Invoke( FCYFUNCTOR_MAKEARG2(FCYMAKEFUNCTOR, P, p) )  // (P1 p1, P2 p2, P3 p3...)
		{
			return m_pFunc( FCYFUNCTOR_MAKEARG1(FCYMAKEFUNCTOR, p) ); // (p1, p2, p3...)
		}
	public:
		fcyFunctorCallBack_StaticFunc(StaticFunc Func)
			: m_pFunc(Func) {}
	};

	template<typename Class, typename MemFunc>
	class fcyFunctorCallBack_MemFunc :
		public fcyRefObjImpl<fcyFunctorCallBack>
	{
	private:
		Class* m_pClsPtr;
		MemFunc m_pClsMemFunc;
	public:
		R Invoke( FCYFUNCTOR_MAKEARG2(FCYMAKEFUNCTOR, P, p) ) // (P1 p1, P2 p2, P3 p3...)
		{
			return (m_pClsPtr->*m_pClsMemFunc)( FCYFUNCTOR_MAKEARG1(FCYMAKEFUNCTOR, p) ); // (p1, p2, p3...)
		}
	public:
		fcyFunctorCallBack_MemFunc(Class* Ptr, MemFunc Func)
			: m_pClsPtr(Ptr), m_pClsMemFunc(Func) {}
	};
private:
	fcyFunctorCallBack* m_pCallBack;
public:
	fcyFunctor& operator=(const fcyFunctor& Right)
	{
		if(this != &Right)
		{
			FCYSAFEKILL(m_pCallBack);

			m_pCallBack = Right.m_pCallBack;

			if(m_pCallBack)
				m_pCallBack->AddRef();
		}

		return *this;
	}

	bool operator==(const fcyFunctor& Right)
	{
		return (m_pCallBack == Right.m_pCallBack);
	}

	R operator() ( FCYFUNCTOR_MAKEARG2(FCYMAKEFUNCTOR, P, p) ) // (P1 p1, P2 p2, P3 p3...)
	{
		if(!m_pCallBack)
			throw fcyException("fcyFunctor::operator()", "Invalid pointer.");
		else
			return m_pCallBack->Invoke( FCYFUNCTOR_MAKEARG1(FCYMAKEFUNCTOR, p) ); // (p1, p2, p3...)
	}
	R operator() ( FCYFUNCTOR_MAKEARG2(FCYMAKEFUNCTOR, P, p) )const // (P1 p1, P2 p2, P3 p3...)
	{
		if(!m_pCallBack)
			throw fcyException("fcyFunctor::operator()const", "Invalid pointer.");
		else
			return m_pCallBack->Invoke( FCYFUNCTOR_MAKEARG1(FCYMAKEFUNCTOR, p) ); // (p1, p2, p3...)
	}
public:
	bool Empty()const
	{
		return (NULL == m_pCallBack);
	}
public:
	fcyFunctor()
		: m_pCallBack(NULL) { }

	template<typename StaticFunc>
	fcyFunctor(StaticFunc pFunc)
		: m_pCallBack(NULL)
	{
		m_pCallBack = new fcyFunctorCallBack_StaticFunc<StaticFunc>(pFunc);
	}

	template<typename Class, typename MemFunc>
	fcyFunctor(Class* Ptr, MemFunc pFunc)
		: m_pCallBack(NULL)
	{
		m_pCallBack = new fcyFunctorCallBack_MemFunc<Class, MemFunc>(Ptr, pFunc);
	}

	fcyFunctor(const fcyFunctor& Org)
		: m_pCallBack(Org.m_pCallBack)
	{
		if(m_pCallBack)
			m_pCallBack->AddRef();
	}

	~fcyFunctor()
	{
		FCYSAFEKILL(m_pCallBack);
	}
};

#undef FCYFUNCTOR_COMMA

#endif
