/**
 *  @file       webpages.cpp
 *  @author     Michael A. Uman
 *  @date       April 1, 2013
 */

#include <wx/wx.h>
#include "serverClasses.h"
#include "muhttpd.h"
#include "mutable.h"
#include "muhtmlhelpers.h"
#include "muutils.h"
#include "dbgutils.h"

myStyleSheet    sheet;


/**
 *
 */

wxString generate_table() {
    myTable     table(4,2);
    wxString    sHTML;

    D(debug(wxT("generate_table()\n")));

    table.set_border(1);
    table.set_caption( wxT("Information Matrix") );
    //table.set_row_class(1, wxT("even"));

    table.cell(0, 0) = myCell( wxT("Field") );
    table.cell(0, 1) = myCell( wxT("Value") );
    table.cell(1, 0) = myCell( wxT("User name :") );
    table.cell(1, 1) = myCell( wxT("Michael Uman") );
    table.cell(2, 0) = myCell( wxT("Phone # :") );
    table.cell(2, 1) = myCell( wxT("(XXX) XXX-XXXX") );
    table.cell(3, 0) = myCell( wxT("User ID :") );
    table.cell(3, 1) = myCell( wxT("muman613") );
    //table.dump();
    //table.cell(0,0).Class( wxT("special") );

    sHTML = table.HTML();

    //printf("%s", sHTML.c_str());

    return sHTML;
}

bool table_stub(serverPage* pPage, muRequest* pRequest) {
    wxString    sHTML = generate_table();

    pPage->Clear();

    pPage->SetTitle( wxT("Test Table Class") );
    pPage->AddToBody(HTML::CENTER(HTML::HEADING1(wxT("Test Table Class"))));
    *pPage += sHTML;

    *pPage += HTML::HR() ;
    *pPage += HTML::LINK(wxT("Go Back To Home"), wxT("/index.html"));

    pPage->SetStyleSheet( sheet );

    return true;
}

/**
 *
 */

bool page2_stub(serverPage* pPage, muRequest* pRequest) {
    D(debug(wxT("page2_stub(%p, %p)\n"), pPage, pRequest));

    pPage->Clear();

    *pPage += HTML::HEADING1(wxT("Cookies Sent")) + HTML::BR();

    size_t numCookies = pRequest->m_cookies.Count();

    D(debug(wxT("found %d cookies!\n"), numCookies));

    myTable table(numCookies, 2);

    for (size_t i = 0 ; i < numCookies ; i++) {
        table.cell(i, 0) = pRequest->m_cookies[i].name();
        table.cell(i, 1) = pRequest->m_cookies[i].value();
    }

    *pPage += table.HTML();

    *pPage += HTML::HR() ;
    *pPage += HTML::LINK(wxT("Go Back To Home"), wxT("/index.html"));

    return true;
}

/**
 *  Generate the page on-the-fly...
 */

bool page3_stub(serverPage* pPage, muRequest* pRequest) {
    D(debug(wxT("index_stub(%p)\n"), pPage));

    pPage->Clear();
    //pPage->AddToHead("<style> a { font-size: 18pt; }</style>");

    pPage->AddToBody(HTML::CENTER(HTML::HEADING1(wxT("Page 3"))));

    pPage->AddToBody( wxT("<form action=\"submit.cgi\" enctype=\"multipart/form-data\" method=\"post\">\n") );
    pPage->AddToBody( wxT("<table>\n") );
    pPage->AddToBody( wxT("  <tr><td align=\"right\">Image Name :</td><td><input type=\"text\" name=\"fname\"></td></tr>\n"));
    pPage->AddToBody( wxT("  <tr><td align=\"right\">Image :</td><td><INPUT type=\"file\" name=\"name_of_files\" accept=\"image/*\"></td></tr>\n"));
    pPage->AddToBody( wxT("</table>\n") );
    pPage->AddToBody( wxT("  <input type=\"submit\" value=\"Send\">\n"));
    pPage->AddToBody( wxT("</form>\n") );
    pPage->AddToBody( HTML::HR() );
    pPage->AddToBody( HTML::LINK(wxT("Go Back To Home"), wxT("/index.html")) );
    return true;
}

/**
 *  Add an image page from memory to the page catalog.
 */

bool add_image_page(muHTTPd* pServer, wxString pageName, wxString sMimeType,
                    void* pData, size_t length) {
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

bool submit_stub(serverPage* pPage, muRequest* pRequest) {
    //D(debug("submit_stub()\n"));
    wxString                sName,
    sFilename,
    sUploadType;

    const myAttachment* pThisAttach = 0L;

    if ((pThisAttach = pRequest->FindAttach( wxT("fname") )) != 0) {
        sName = pThisAttach->string();
    }

    if ((pThisAttach = pRequest->FindAttach( wxT("name_of_files") )) != 0) {
        sFilename = pThisAttach->fname();
        sUploadType = pThisAttach->type();

        pThisAttach->write_file();
    }

    pPage->Clear();

    pPage->AddToBody(HTML::HEADING1(wxT("Submit Results")));

    if (!sName.IsEmpty()) {
        pPage->AddToBody(wxT("<p>User name is ") + sName + wxT("</br>"));
    }

    if (!sFilename.IsEmpty() && !sUploadType.IsEmpty()) {

        pPage->AddToBody(wxT("<p>Uploaded file name ") + sFilename + wxT(" of type ") + sUploadType + wxT("</br>"));

//        if (sUploadType == wxT("image/jpeg")) {
        if (TestMimeType( sUploadType, CATEGORY_IMAGE )) {
            serverPage*     pNewPage = new serverPage(wxT("/images/") + sFilename, serverPage::PAGE_BINARY);

            pNewPage->SetImageData( pThisAttach->data(), pThisAttach->size() );
            if (pPage->server()->AddPage( *pNewPage )) {
                pPage->AddToBody(HTML::IMAGE( wxT("/images/") + sFilename, wxEmptyString, 320, 240));
                pPage->AddToBody( HTML::BR() );
            }

            delete pNewPage;

        }
    } else {

    }
    pPage->AddToBody( HTML::HR() );
    pPage->AddToBody( HTML::LINK(wxT("Go Back To Home"), wxT("/index.html")) );

    return true;
}

/**
 *  HTTP Error 404 Page HTML
 */

const char* sz404Text = \
                        "<CENTER><DIV ALIGN=\"center\">\n"
                        "<PRE><CODE>\n"
                        " \n"
                        " \n"
                        " \n"
                        " \n"
                        "<TABLE BORDER=0 BGCOLOR=\"#C0C0C0\"><TR><TD><CODE><FONT COLOR=\"#330033\">ERR 404</FONT></CODE></TD></TR></TABLE>"
                        " \n"
                        " \n"
                        "FFFFFF0 FFFFFFF FF00000 FF00000 0FFFFF0 FF000FF FF000FF\n"
                        "FF000FF 000F000 FF00000 FF00000 FF000FF FF000FF 0FF0FF0\n"
                        "FFFFFF0 000F000 FF00000 FF00000 0FFF000 FF000FF 00FFF00\n"
                        "FFFFFF0 000F000 FF00000 FF00000 000FFF0 FF000FF 00FFF00\n"
                        "FF000FF 000F000 FF00000 FF00000 FF000FF FF000FF 0FF0FF0\n"
                        "FFFFFF0 FFFFFFF FFFFFFF FFFFFFF 0FFFFF0 0FFFFF0 FF000FF\n"
                        " \n"
                        " \n"
                        "You have attempted to access a non-existent page.\n"
                        "The current HTTP session will be terminated.\n"
                        " \n"
                        " \n"
                        "Press any key to continue<BLINK>_</BLINK>\n"
                        "</CODE></PRE>\n"
                        "</DIV></CENTER>\n";

/**
 *
 */

void add_404_page( muHTTPd* pServer) {
    serverPage* pNewPage;
    //D(debug("add_404_page()\n"));

    pNewPage = new serverPage(wxT("/404.html"));

    pNewPage->AddToHead("<style>\nbody { background: #0000cc; color: #ffffff; }\n</style>\n");
    pNewPage->SetTitle(wxT("404 File Not Found - Blue Screen of Shame"));
    pNewPage->BodyFromString( sz404Text );

    pServer->Set404Page( *pNewPage );

    delete pNewPage;
}

#ifndef LOAD_FROM_FILE
#include "image/html_02_00_jpg.h"
#include "image/html_debuggerfe_ico.h"
#endif

/**
 *
 */

void add_all_images(muHTTPd* pServer) {
    /* Add all images here */
    add_image_page( pServer, wxT("/images/image.jpg"), wxT("image/jpeg"),
                    html_002_00_jpg, html_002_00_jpg_len );
    add_image_page( pServer, wxT("/images/favicon.ico"), wxT("image/ico"),
                    html_debuggerfe_ico, html_debuggerfe_ico_len );
    add_image_page( pServer, wxT("/favicon.ico"), wxT("image/ico"),
                    html_debuggerfe_ico, html_debuggerfe_ico_len );
    return;
}

/**
 *
 */

void init_style_sheet(myStyleSheet& sheet) {
    myStyleElement  elem;

    elem = myStyleElement( wxT("body") );

    elem += myStyleAttribute( wxT("font-family") ,wxT("helvetica"));
    elem += myStyleAttribute( wxT("background"), wxT("#8fafaf"));

    sheet += elem;

    elem = myStyleElement( wxT("div"), wxEmptyString, wxT("menu") );

    elem += myStyleAttribute( wxT("font-size"), wxT("12pt") );
    elem += myStyleAttribute( wxT("background"), wxT("#f8f8f8") );

    sheet += elem;

    return;
}

/**
 *
 */

bool index_stub(serverPage* page, muRequest* pRequest) {
    HTML::HTMLOpts opts;

    //D(debug("index_stub()\n"));

    opts.m_sOnClick = wxT("return confirm_stop();");

    page->Clear();

    page->SetFavIconName( wxT("images/favicon.ico") );
    page->SetTitle( wxT("Index Page") );
    page->AddJavascriptLink( wxT("functions.js") );

    *page += HTML::CENTER(HTML::HEADING1(wxT("Main Menu")));
    *page += wxT("<div id=\"menu\">");
    *page += wxT("<ul>");
    *page += wxT("<li>") + HTML::LINK(wxT("Stop running test"), wxT("page1.html"), &opts) + wxT("</li>");
    *page += wxT("<li>") + HTML::LINK(wxT("Display cookies"), wxT("page2.html")) + wxT("</li>");
    *page += wxT("<li>") + HTML::LINK(wxT("Upload an image to the server"), wxT("page3.html")) + wxT("</li>");
    *page += wxT("<li>") + HTML::LINK(wxT("Test Table class"), wxT("table-test.html")) + wxT("</li>");
    *page += wxT("</ul>");
    *page += wxT("</div>");

    *page += HTML::IMAGE( wxT("images/image.jpg"), wxT("My Face"), 128, 128 );

    /* Set cookies to expire in 5 minutes */
    wxTimeSpan expireSpan = wxTimeSpan::Minutes( 5 );

    page->AddCookie( wxT("ID"), wxT("muman613"), expireSpan );
    page->AddCookie( wxT("FLAGS"), wxT("AEfnoYz"), expireSpan );

    page->SetStyleSheet( sheet );


    return true;
}

/**
 *
 */

void add_javascript_page(muHTTPd* pServer) {

    serverPage*     page = 0;

    page = new serverPage(wxT("/functions.js"), serverPage::PAGE_JSCRIPT);

    page->AddToScript( wxT("function confirm_start() {\n\tvar confirmed=confirm(\"Are you sure you want to START test? This destroys previous results!\");\n\treturn confirmed;\n}\n"));
    page->AddToScript( wxT("function confirm_stop() {\n\tvar confirmed=confirm(\"Are you sure you want to STOP test? Results are incomplete!\");\n\treturn confirmed;\n}\n"));

    pServer->AddPage( *page );

    delete page;

}

/**
 *
 */

void add_serverpages(muHTTPd* pServer) {
    serverPage*     page;

    //D(debug("add_serverpages()\n"));

    init_style_sheet( sheet );

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

    ADD_PAGE( pServer, wxT("/index.html"),      index_stub);
    ADD_PAGE( pServer, wxT("/page2.html"),      page2_stub);
    ADD_PAGE( pServer, wxT("/page3.html"),      page3_stub);
    ADD_PAGE( pServer, wxT("/submit.cgi"),      submit_stub)
    ADD_PAGE( pServer, wxT("/table-test.html"), table_stub);

    add_all_images( pServer );

    add_404_page( pServer );


    add_javascript_page( pServer );

    return;
}
