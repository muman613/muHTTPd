#include <wx/wx.h>
#include <wx/socket.h>
#include <wx/config.h>
#include <stdio.h>
#include <signal.h>
#include "dbgutils.h"
#include "serverClasses.h"
#include "lastusedlist.h"
#include "myhttpd.h"


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


#if 1

bool page3_stub(serverPage* pPage)
{
    D(debug("index_stub(%p)\n", pPage));

    pPage->Clear();
    pPage->AddToHead("<style> a { font-size: 18pt; }</style>");

    pPage->AddToBody(HTML::CENTER(HTML::HEADING1(wxT("Page 3"))));
    pPage->AddToBody(wxT("<table><tr><td>Start Time :</td><td>14:56:09 2012-11-26</td></tr></table>"));
    pPage->AddToBody(wxT("<br>") + HTML::LINK(wxT("Display current results"), wxT("page2.html"))); // <a href=\"page2.html\">Display current results</a>"));
    pPage->AddToBody(wxT("<br><a href=\"page3.html\">Display previous results</a>"));

    return true;
}

#endif

void add_serverpages(myHTTPd* pServer)
{
    serverPage*     page;

    page = new serverPage(wxT("/index.html") /*, index_stub */);

    page->SetTitle( wxT("Index Page") );

    *page += HTML::CENTER(HTML::HEADING1(wxT("Main Menu")));
    *page += wxT("<p>") + HTML::LINK(wxT("Stop running test"), wxT("page1.html")) + wxT("<br>");
    *page += HTML::LINK(wxT("Display current results"), wxT("page2.html")) + wxT("<br>");
    *page += HTML::LINK(wxT("Display previous results"), wxT("page3.html")) + wxT("<br>");

    page->SaveToFile("/tmp/index.html");

    page->Dump( stdout );

    pServer->AddPage( *page );

    delete page;

    page = new serverPage(wxT("/page1.html"));

    page->SetTitle( wxT("Page 1") );

    *page += HTML::CENTER(HTML::HEADING1(wxT("Page 1")));
    *page += wxT("<p>This page was generated!");

    page->Dump( stdout );

    pServer->AddPage( *page );

    delete page;

#if 0
    page = new serverPage(wxT("/page2.html"));

    page->LoadFromFile(wxT("html/template.html"));

    pServer->AddPage( *page );

    delete page;
#endif

    page = new serverPage(wxT("/page3.html"), page3_stub);

    pServer->AddPage( *page );

    delete page;
}

int main() {
    wxInitializer       wxInit;

    wxSocketBase::Initialize();

    signal( SIGINT, signal_sigint_handler );

    myHTTPd         webServer(8080);

    add_serverpages( &webServer );

    if (webServer.Start()) {
        while (!bDone) {
            ::wxSleep( 80 );
        }
    }

    webServer.Stop();

    signal( SIGINT, SIG_DFL );
}
