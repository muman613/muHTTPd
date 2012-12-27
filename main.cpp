#include <wx/wx.h>
#include <wx/socket.h>
#include <wx/config.h>
#include <stdio.h>
#include <signal.h>
#include "dbgutils.h"
#include "serverClasses.h"
#include "webpages.h"
#include "myhttpd.h"
#include "mytable.h"

bool bDone = false;

/**
 *  Signal handler for Control-C break.
 */

void    signal_sigint_handler(int signum)
{
    D(debug("signal_sigint_handler(%d)\n", signum));

    bDone = true;

    return;
}


/**
 *
 */

int main() {
    wxInitializer       wxInit;

    wxSocketBase::Initialize();

    signal( SIGINT, signal_sigint_handler );

    myHTTPd         webServer(8080);

    webServer.SetLogFile("/tmp/myHTTPd.log");

    add_serverpages( &webServer );

    if (webServer.Start()) {
        while (!bDone) {
            ::wxSleep( 80 );
        }
    }

    webServer.Stop();

    signal( SIGINT, SIG_DFL );

	return 0L;
}
