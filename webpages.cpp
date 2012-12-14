#include <wx/wx.h>
#include "serverClasses.h"
#include "myhttpd.h"
#include "mytable.h"
#include "dbgutils.h"

wxString generate_table() {
    myTable     table(2, 2);
    wxString    sHTML;

    D(debug("generate_table()\n"));

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

    pPage->AddToBody( wxT("<form action=\"submit.cgi\" enctype=\"multipart/form-data\" method=\"post\">\n") );
    pPage->AddToBody( wxT("<table>\n") );
    pPage->AddToBody( wxT("  <tr><td align=\"right\">First Name :</td><td><input type=\"text\" name=\"fname\"></td></tr>\n"));
    pPage->AddToBody( wxT("  <tr><td align=\"right\">Image :</td><td><INPUT type=\"file\" name=\"name_of_files\" accept=\"application/*\"></td></tr>\n"));
    pPage->AddToBody( wxT("</table>\n") );
    pPage->AddToBody( wxT("  <input type=\"submit\" value=\"Send\">\n"));
    pPage->AddToBody( wxT("</form>\n") );

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

/**
 *
 */

bool submit_stub(serverPage* pPage, Request* pRequest) {
    D(debug("submit_stub()\n"));
    wxString                sName = wxT("Unsupplied"),
                            sFilename = wxT("Unsupplied"),
                            sUploadType;

    const myAttachment* pThisAttach = 0L;

    if ((pThisAttach = pRequest->FindAttach( wxT("fname") )) != 0) {
        sName = pThisAttach->string();
    }

    if ((pThisAttach = pRequest->FindAttach( wxT("name_of_files") )) != 0) {
        sFilename = pThisAttach->fname();
        sUploadType = pThisAttach->type();
    }

    pPage->Clear();

    pPage->AddToBody(HTML::HEADING1(wxT("Submit Results")));
    pPage->AddToBody(wxT("<p>Supplied user name is ") + sName + wxT("</br>"));
    pPage->AddToBody(wxT("<p>Uploaded file name ") + sFilename + wxT(" of type ") + sUploadType + wxT("</br>"));

    if (sUploadType == wxT("image/jpeg")) {
        serverPage*     pNewPage = new serverPage(wxT("/") + sFilename);

        pNewPage->SetImageData( pThisAttach->data(), pThisAttach->size() );
        pNewPage->SetMimeType( pThisAttach->type() );
        pPage->server()->AddPage( *pNewPage );
        delete pNewPage;
    }
    return true;
}

#ifndef LOAD_FROM_FILE
    #include "image/html_02_00_jpg.h"
    #include "image/html_debuggerfe_ico.h"
#endif

void add_serverpages(myHTTPd* pServer)
{
    serverPage*     page;

    D(debug("add_serverpages()\n"));

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

#ifdef  _DEBUG
    page->Dump( stdout );
#endif

    pServer->AddPage( *page );

    delete page;

    page = new serverPage(wxT("/page1.html"));

    page->SetTitle( wxT("Page 1") );

    *page += HTML::CENTER(HTML::HEADING1(wxT("Page 1")));
    *page += wxT("<p>This page was generated!");
    page->SetRedirectTo( wxT("index.html"), 10);

#ifdef  _DEBUG
    page->Dump( stdout );
#endif

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

    ADD_PAGE( pServer, wxT("/submit.cgi"), submit_stub)

    return;
}

