#ifndef __DBGUTILS_H__
#define __DBGUTILS_H__

#include <wx/wx.h>
//#include "varargs.h"
#include "stdarg.h"

/* kludge for linux debug logging */
//#ifndef _DEBUG
//    #define _DEBUG
//#endif

#ifdef  _DEBUG

#define D(x) (x)

inline void debug(const wxChar* sFMT, ...) {
    va_list  args;
//    static char dbgBuffer[8192];

    va_start(args, sFMT);
    //vsprintf(dbgBuffer, sFMT, args);

#ifdef  __WXMSW__
    OutputDebugString(dbgBuffer);
#else
    vfprintf(stderr, sFMT, args);
#endif

    va_end(args);

    return;
}

#else   // _DEBUG

#define D(x)    /* (x) */

inline void debug(const wxChar* sFMT, ...) {
}

#endif  // _DEBUG

#endif  // __DBGUTILS_H__

