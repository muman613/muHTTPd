/**
 *  @file       main.cpp
 *  @author     Michael A. Uman
 *  @date       December 30, 2012
 *  @brief      Example application using the muHTTPd library.
 */

#include <wx/wx.h>
#include <wx/socket.h>
#include <wx/config.h>
#include <wx/filename.h>
#include <stdio.h>
#include <signal.h>
#include "muHTTPdemoApp.h"
#include "dbgutils.h"
#include "serverClasses.h"
#include "webpages.h"
#include "muhttpd.h"
#include "mutable.h"

bool bDone = false;

IMPLEMENT_APP(muHTTPdemoApp)

/**
 *  Signal handler for Control-C break.
 */

void    signal_sigint_handler(int signum) {
    D(debug("signal_sigint_handler(%d)\n", signum));

    bDone = true;

    return;
}

/**
 *  Init function ensures that 'data-dir' exists in the current directory.
 */

bool muHTTPdemoApp::OnInit() {
    wxString    sDataDir = wxT("data-dir");

    if (!wxFileName::DirExists(sDataDir)) {
        if (!wxFileName::Mkdir( sDataDir )) {
            fprintf(stderr, "ERROR: Unable to create data-directory!\n");
        }
    }

    return true;
}

/**
 *
 */

int muHTTPdemoApp::OnRun() {
    signal( SIGINT, signal_sigint_handler );

    muHTTPd         webServer(8080);

    webServer.SetLogFile("/tmp/muHTTPd.log");

    add_serverpages( &webServer );

    printf("Hit Control-C to abort server!\n");

    if (webServer.Start()) {
        while (!bDone) {
            ::wxSleep( 80 );
        }
    }

    webServer.Stop();

    signal( SIGINT, SIG_DFL );

    return 0L;
}
