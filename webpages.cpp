#include <wx/wx.h>
#include "serverClasses.h"
#include "myhttpd.h"
#include "mytable.h"
#include "dbgutils.h"

wxString generate_table() {
    myTable     table(2, 2);
    wxString    sHTML;

    D(debug("test_table()\n"));
    table.set_border(2);
    table.set_row_class(1, wxT("even"));

    table.cell(0, 0) = myCell( wxT("Cell #1") );
    table.cell(0, 1) = myCell( wxT("Cell #2") );
    table.cell(1, 0) = myCell( wxT("User name :") );
    //table.dump();
    table.cell(0,0).Class( wxT("special") );

    sHTML = table.HTML();

    printf("%s", sHTML.c_str());

    return sHTML;
}

/**
 *  Generate the page on-the-fly...
 */

bool page3_stub(serverPage* pPage, Request* pRequest)
{
    D(debug("index_stub(%p)\n", pPage));

    pPage->Clear();
    pPage->AddToHead("<style> a { font-size: 18pt; }</style>");

    pPage->AddToBody(HTML::CENTER(HTML::HEADING1(wxT("Page 3"))));
    pPage->AddToBody(wxT("<table><tr><td>Start Time :</td><td>14:56:09 2012-11-26</td></tr></table>"));
    pPage->AddToBody(wxT("<br>") + HTML::LINK(wxT("Display current results"), wxT("page2.html"))); // <a href=\"page2.html\">Display current results</a>"));
    pPage->AddToBody(wxT("<br><a href=\"page3.html\">Display previous results</a>"));

    pPage->AddToBody( generate_table() );

    return true;
}

/**
 *  Add an image page from memory to the page catalog.
 */

bool add_image_page(myHTTPd* pServer, wxString pageName, wxString sMimeType,
                    void* pData, size_t length)
{
    serverPage*     page = new serverPage( pageName );

    page->SetImageData( pData, length );
    page->SetMimeType( sMimeType );

    pServer->AddPage( *page );

    delete page;

    return true;
}

#ifndef LOAD_FROM_FILE
    #include "image/html_02_00_jpg.h"
    #include "image/html_debuggerfe_ico.h"
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
    *page += HTML::IMAGE( wxT("image.jpg"), wxT("My Face"), 128, 128 );

    page->AddCookie( wxT("ID"), wxT("muman613"), wxT("Tue 04-Dec-2012 12:00:00 GMT") );
    page->AddCookie( wxT("FLAGS"), wxT("AEfnoYz"), wxT("Tue 04-Dec-2012 12:00:00 GMT") );

    page->SaveToFile("/tmp/index.html");

    page->Dump( stdout );

    pServer->AddPage( *page );

    delete page;

    page = new serverPage(wxT("/page1.html"));

    page->SetTitle( wxT("Page 1") );

    *page += HTML::CENTER(HTML::HEADING1(wxT("Page 1")));
    *page += wxT("<p>This page was generated!");
    page->SetRedirectTo( wxT("index.html"), 10);

    page->Dump( stdout );

    pServer->AddPage( *page );

    delete page;

    page = new serverPage(wxT("/page3.html"), page3_stub);

    pServer->AddPage( *page );

    delete page;

    /* Add all images here */
    add_image_page( pServer, wxT("/image.jpg"), wxT("image/jpeg"),
                    html_002_00_jpg, html_002_00_jpg_len );
    add_image_page( pServer, wxT("/favicon.ico"), wxT("image/ico"),
                    html_debuggerfe_ico, html_debuggerfe_ico_len );

    return;
}

