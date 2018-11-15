#include <wx/wx.h>
#include <wx/thread.h>

#ifdef  _DEBUG

static wxMutex* dbgMutex = 0L;

/**
 *  Function which initializes the debug mutex. This function is automatically
 *  called (on init) when built for debug.
 */

__attribute__ ((__constructor__)) void init_debug() {
    dbgMutex = new wxMutex;
    return;
}

/**
 *  Function which initializes the debug mutex. This function is automatically
 *  called (on exit) when built for debug.
 */

__attribute__ ((__destructor__)) void close_debug() {
    if (dbgMutex != 0) {
        delete dbgMutex;
        dbgMutex = 0L;
    }
    return;
}

/**
 *
 */

void debug(const wxChar* sFMT, ...)
{
    static char dbgBuffer[8192];

    if (dbgMutex != 0) {
        va_list         args;
        wxMutexLocker   lock(*dbgMutex);

        va_start(args, sFMT);
        wxVsprintf(dbgBuffer, sFMT, args);
    #ifdef  __WXMSW__
        OutputDebugString(dbgBuffer);
    #else
        fprintf(stderr, "%s", dbgBuffer);
        fflush(stderr);
    #endif

        va_end(args);
    }

    return;
}

#endif  // _DEBUG
