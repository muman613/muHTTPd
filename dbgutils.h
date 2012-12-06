#ifndef __DBGUTILS_H__
#define __DBGUTILS_H__

#include <wx/wx.h>
#include "stdarg.h"

/* kludge for linux debug logging */
//#ifndef _DEBUG
//    #define _DEBUG
//#endif

#ifdef  _DEBUG

#define D(x) (x)

extern void debug(const wxChar*, ...);

#else   // _DEBUG

#define D(x)    /* (x) */

#endif  // _DEBUG

#endif  // __DBGUTILS_H__

