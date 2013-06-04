#ifndef _BMMLIB_H_
#define _BMMLIB_H_

#ifdef BLD_BMM
#define  BMMExport __declspec( dllexport )
#else
#define  BMMExport __declspec( dllimport )
#endif

#include "bitmap.h"
#endif
