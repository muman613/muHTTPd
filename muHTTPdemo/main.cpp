/**
 *  @file       main.cpp
 *  @author     Michael A. Uman
 *  @date       April 4, 2013
 *  @brief      Example application using the muHTTPd library.
 */

#include <wx/wx.h>
#include <wx/socket.h>
#include <wx/config.h>
#include <wx/filename.h>
#include <wx/cmdline.h>
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
    D(debug(wxT("signal_sigint_handler(%d)\n"), signum));

    bDone = true;

    return;
}

/**
 *
 */

muHTTPdemoApp::muHTTPdemoApp()
:   m_nPort(DEFAULT_PORT)
{
    // ctor
}

muHTTPdemoApp::~muHTTPdemoApp()
{
    // dtor
}

/**
 *
 */

void muHTTPdemoApp::create_data_dir() {
    wxString    sDataDir = wxT("data-dir");

    if (!wxFileName::DirExists(sDataDir)) {
        if (!wxFileName::Mkdir( sDataDir )) {
            fprintf(stderr, "ERROR: Unable to create data-directory!\n");
        }
    }

    return;
}

/**
 *  Read configuration information from file.
 */

void muHTTPdemoApp::read_config_file() {
    wxConfig*       config = new wxConfig( wxT("muHTTPdemo") );

    if (config) {
        config->Read( wxT("port"), &m_nPort );

        delete config;
    }

    return;
}

bool muHTTPdemoApp::parse_commandline() {
    wxCmdLineParser     parser;
    int                 rc;

    parser.SetCmdLine( argc, argv );

    parser.AddSwitch( wxT("h"), wxT("help"), wxT("Display help"),          wxCMD_LINE_OPTION_HELP );
    parser.AddOption( wxT("p"), wxT("port"), wxT("Port to run server on"), wxCMD_LINE_VAL_NUMBER );

    rc = parser.Parse();

    if (rc == 0) {
        long lTmp;

        if (parser.Found( wxT("port"), &lTmp )) {
            m_nPort = (int)lTmp;
        }

    } else if (rc == -1) {
        return false;
    } else {
        /* error */
        return false;
    }

    return true;
}

/**
 *  Read parameters from config file, over-ride with commandline options.
 *
 *  Also must call create_data_dir() to create the 'data-dir' directory.
 */

bool muHTTPdemoApp::OnInit() {
    bool            bRes = false;

    read_config_file();
    bRes = parse_commandline();

    if (bRes)
        create_data_dir();

    return bRes;
}

/**
 *
 */

int muHTTPdemoApp::OnRun() {
    muHTTPd         webServer(m_nPort);

    wxSocketBase::Initialize();

    webServer.SetLogFile( wxT("/tmp/muHTTPd.log") );

    add_serverpages( &webServer );

    /* Install the Control-C handler */
    signal( SIGINT, signal_sigint_handler );
    printf("muHTTPd running on port %d\nHit Control-C to abort server!\n", m_nPort);

    /* Start the webserver, wait for user to press Control-C... */
    if (webServer.Start()) {
        while (!bDone) {
            ::wxSleep( 80 );
        }
    }

    webServer.Stop();

    signal( SIGINT, SIG_DFL );

    printf("\n");

    return 0L;
}
