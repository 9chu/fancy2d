////////////////////////////////////////////////////////////////////////////////
/// @file  fcyFunctor.h
/// @brief fancy调用封装
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../fcyRefObj.h"
#include "../fcyException.h"

/// @addtogroup fancy杂项
/// @{

template<typename T>
class fcyFunctor;

#define FCYMAKEFUNCTOR 0
#include "fcyFunctorDetail.h"
#undef FCYMAKEFUNCTOR

#define FCYMAKEFUNCTOR 1
#include "fcyFunctorDetail.h"
#undef FCYMAKEFUNCTOR

#define FCYMAKEFUNCTOR 2
#include "fcyFunctorDetail.h"
#undef FCYMAKEFUNCTOR

#define FCYMAKEFUNCTOR 3
#include "fcyFunctorDetail.h"
#undef FCYMAKEFUNCTOR

#define FCYMAKEFUNCTOR 4
#include "fcyFunctorDetail.h"
#undef FCYMAKEFUNCTOR

#define FCYMAKEFUNCTOR 5
#include "fcyFunctorDetail.h"
#undef FCYMAKEFUNCTOR

#define FCYMAKEFUNCTOR 6
#include "fcyFunctorDetail.h"
#undef FCYMAKEFUNCTOR

#define FCYMAKEFUNCTOR 7
#include "fcyFunctorDetail.h"
#undef FCYMAKEFUNCTOR

#define FCYMAKEFUNCTOR 8
#include "fcyFunctorDetail.h"
#undef FCYMAKEFUNCTOR
/// @}
