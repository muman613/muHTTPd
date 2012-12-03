#include <wx/wx.h>
#include <wx/arrstr.h>
#include <wx/textfile.h>
#include <wx/filename.h>
#include <wx/socket.h>
#include <wx/mimetype.h>
#include "serverClasses.h"
#include "dbgutils.h"

//#define ENABLE_DEBUG_SEND            1    // Enable header debugging

static wxString sHTMLEol = wxT("\r\n");

/**
 *
 */

serverPage::serverPage()
:   m_sMimeType(wxT("text/html")),
    m_nRedirectTime(0),
    m_pPageData(0L),
    m_pBinaryData(0L),
    m_nBinaryDataSize(0L),
    m_cbFunc(0L),
    m_type(serverPage::PAGE_HTML)
{
    // ctor
}

/**
 *
 */

serverPage::serverPage(const serverPage& copy)
:   m_sPageName(copy.m_sPageName),
    m_sMimeType(copy.m_sMimeType),
    m_sRedirect(copy.m_sRedirect),
    m_nRedirectTime(copy.m_nRedirectTime),
    m_sHeadText(copy.m_sHeadText),
    m_sBodyText(copy.m_sBodyText),
    m_pPageData(copy.m_pPageData),
    m_cbFunc(copy.m_cbFunc),
    m_size(copy.m_size),
    m_type(copy.m_type)
{
    // ctor
    /* deep copy the binary data if it exists */
    if (copy.m_pBinaryData != 0) {
        m_pBinaryData = (void *)malloc( copy.m_nBinaryDataSize );
//      D(debug("-- copying binary data from %p to %p!!!\n", copy.m_pBinaryData, m_pBinaryData));
        memcpy(m_pBinaryData, copy.m_pBinaryData, copy.m_nBinaryDataSize);
        m_nBinaryDataSize = copy.m_nBinaryDataSize;
    } else {
        m_pBinaryData       = 0L;
        m_nBinaryDataSize   = 0L;
    }
}

/**
 *
 */

serverPage::serverPage(wxString sPageName, PAGE_CALLBACK pCBFunc)
:   m_sPageName(sPageName),
    m_sMimeType(wxT("text/html")),
    m_nRedirectTime(0),
    m_pPageData(0L),
    m_pBinaryData(0L),
    m_nBinaryDataSize(0L),
    m_cbFunc(pCBFunc),
    m_type(serverPage::PAGE_HTML)
{
    // ctor
}

/**
 *
 */

serverPage::~serverPage()
{
    // dtor
//  D(debug("destructing %s page %s\n", (m_type == PAGE_HTML)?"HTML":"IMAGE", m_sPageName.c_str()));
    Clear();
}

/**
 *  Overloaded operator equal (assignment).
 */

serverPage&     serverPage::operator = (const serverPage& copy) {
//  D(debug("serverPage::operator =(...)\n"));

    m_sPageName         = copy.m_sPageName;
    m_sMimeType         = copy.m_sMimeType;
    m_sRedirect         = copy.m_sRedirect;
    m_nRedirectTime     = copy.m_nRedirectTime;
    m_sHeadText         = copy.m_sHeadText;
    m_sBodyText         = copy.m_sBodyText;
    m_pPageData         = copy.m_pPageData;
    m_cbFunc            = copy.m_cbFunc;
    m_size              = copy.m_size;
    m_type              = copy.m_type;

    if (copy.m_pBinaryData != 0) {
        m_pBinaryData = (void *)malloc( copy.m_nBinaryDataSize );
        D(debug("-- copying binary data from %p to %p!!!\n", copy.m_pBinaryData, m_pBinaryData));
        memcpy(m_pBinaryData, copy.m_pBinaryData, copy.m_nBinaryDataSize);
        m_nBinaryDataSize = copy.m_nBinaryDataSize;
    } else {
        m_pBinaryData       = 0L;
        m_nBinaryDataSize   = 0L;
    }

    return *this;
}


void* serverPage::GetPageData()
{
    return m_pPageData;
}

void serverPage::SetPageData(void* pData)
{
    m_pPageData = pData;
    return;
}

#if 0
bool serverPage::LoadFromFile(wxString sFilename)
{
    bool            bRes = false;
    wxTextFile      textFile;

    D(debug("serverPage::LoadFromFile(%s)\n", sFilename.c_str()));

    if (textFile.Open( sFilename )) {

        for (size_t x = 0 ; x < textFile.GetLineCount() ; x++) {
            m_sPageText.Add( textFile.GetLine( x ) );
        }

        bRes = true;

        textFile.Close();
    }

    return bRes;
}
#endif

/**
 *  Save the HTML output to a file.
 */

bool serverPage::SaveToFile(wxString sFilename)
{
    bool        bRes = false;
    wxFileName  fname(sFilename);
    wxString    sFullPath;
    wxFile      textFile;

    D(debug("serverPage::SaveToFile(%s)\n", sFilename.c_str()));

    if (fname.IsDir()) {
        fname.Assign( sFilename, m_sPageName );
    } else {
        fname.Assign( sFilename );
    }

    sFullPath = fname.GetFullPath();
    D(debug("-- saving to file %s\n", sFullPath.c_str()));

    if (textFile.Create( sFullPath.c_str(), true )) {
        wxString sHTML = HTML();

        textFile.Write( sHTML );
    }

    return bRes;
}

void     serverPage::SetPageName(wxString sPageName)
{
    m_sPageName = sPageName;
}

wxString serverPage::GetPageName()
{
    return m_sPageName;
}

void     serverPage::AddToHead(wxString sLine) {
    m_sHeadText.Add( sLine );
}

void    serverPage::AddToBody(wxString sLine) {
    m_sBodyText.Add( sLine );
}

/**
 *
 */

serverPage&     serverPage::operator +=(wxString sLine) {
    AddToBody(sLine);
    return *this;
}

/**
 *
 */

void serverPage::SetTitle(wxString sTitle)
{
    D(debug("serverPage::SetTitle(%s)\n", sTitle.c_str()));
    m_sPageTitle = sTitle;
}

/**
 *
 */

void serverPage::SetMimeType(wxString sMimeType)
{
    D(debug("serverPage::SetMimeType(%s)\n", sMimeType.c_str()));
    m_sMimeType = sMimeType;
}

/**
 *
 */

void  serverPage::SetRedirectTo(wxString sRedirect, int nSec)
{
    D(debug("serverPage::SetRedirectTo(%s, %n)\n", sRedirect.c_str(), nSec));

    m_sRedirect     = sRedirect;
    m_nRedirectTime = nSec;

    return;
}

/**
 *
 */

void serverPage::SetRefreshTime(int nSec) {
    m_sRedirect.Clear();
    m_nRedirectTime = nSec;
    return;
}

/**
 *  Restore page to 'pristine' state.
 */

void serverPage::Clear() {
//  D(debug("serverPage::Clear()\n"));

    m_sHeadText.Clear();
    m_sBodyText.Clear();
    m_sRedirect.Clear();

    m_nRedirectTime = 0;
    m_sMimeType     = wxT("text/html");
    m_type          = PAGE_HTML;

    if (m_pBinaryData != 0) {
//      D(debug("-- freeing binary data from page %s\n", m_sPageName.c_str()));
        free(m_pBinaryData);
        m_pBinaryData       = 0L;
        m_nBinaryDataSize   = 0L;
    }
}

/**
 *  Call the 'update' hook function.
 */

void serverPage::Update() {
    D(debug("serverPage::Update()\n"));

    if (m_cbFunc != 0) {
        D(debug("-- calling call-back function!\n"));
        (*m_cbFunc)(this);
    }

    return;
}

/**
 *  Get the callback function for the page.
 */

PAGE_CALLBACK serverPage::GetCallback()
{
    return m_cbFunc;
}

/**
 *  Set the callback function for the page.
 */

void serverPage::SetCallback(PAGE_CALLBACK cbFunc)
{
    D(debug("serverPage::SetCallback(%p)\n", cbFunc));
    m_cbFunc = cbFunc;
}

/**
 *  Send the page to the peer. Use wxSOCKET_WAITALL flag in order to ensure the
 *  entire packet is sent in one burst.
 */

bool serverPage::Send(wxSocketBase* pSocket)
{
    bool        bRes = false;
    char        buf[500];
    wxString    sHTML;

    D(debug("serverPage::Send(%p)\n", pSocket));

#ifdef  ENABLE_DEBUG_SEND
    fOut = fopen("/tmp/dump.txt", "w");
#endif

    pSocket->SaveState();
    pSocket->SetFlags(wxSOCKET_WAITALL);

    if (m_type == PAGE_HTML) {
        sHTML = HTML();
    }

    sprintf(buf, "HTTP/1.1 200 OK\r\nServer: myHTTPd-1.0.0\r\nContent-Type: %s\r\nContent-Length: %ld\r\nConnection: Keep-Alive\r\n\r\n",
            m_sMimeType.c_str(), m_size );
    pSocket->Write( buf, strlen(buf) );

#ifdef  ENABLE_DEBUG_SEND
    fwrite(buf, strlen(buf), 1, fOut);
#endif

    if (m_type == PAGE_HTML) {
        pSocket->Write( sHTML.c_str(), sHTML.size());
    } else {
        pSocket->Write( m_pBinaryData, m_size );
    }

    bRes = true;

    pSocket->RestoreState();

#ifdef  ENABLE_DEBUG_SEND
    fclose(fOut);
#endif

    return bRes;
}

/**
 *  Generate the actual HTML for the page from the HEAD and BODY sections.
 */

wxString serverPage::HTML() {
    wxString sHTMLText, sTmp;

    sHTMLText  = wxT("<!DOCTYPE html>") + sHTMLEol;
    sHTMLText += wxT("<html>") + sHTMLEol;

    /* generate the HEAD section */
    sHTMLText += wxT("<head>") + sHTMLEol;
    sHTMLText += wxT("\t<title>") + m_sPageTitle + wxT("</title>") + sHTMLEol;
    for (size_t x = 0 ; x < m_sHeadText.Count() ; x++) {
        sHTMLText += wxT("\t") + m_sHeadText[x] + sHTMLEol;
    }
    /* If redirect is set, perform redirection */
    if (!m_sRedirect.IsEmpty()) {
        sTmp = wxT("\t<meta http-equiv=\"refresh\" content=\"") +
                wxString::Format(wxT("%d"), m_nRedirectTime) +
                wxT(";url=") + m_sRedirect + wxT("\">") + sHTMLEol;
        sHTMLText += sTmp;
    } else if (m_nRedirectTime != 0) {
        sTmp = wxT("\t<meta http-equiv=\"refresh\" content=\"") +
                wxString::Format(wxT("%d"), m_nRedirectTime) + wxT("\">") +
                sHTMLEol;
        sHTMLText += sTmp;
    }

    sHTMLText += wxT("</head>") + sHTMLEol;

    /* generate the BODY section */

    sHTMLText += wxT("<body>") + sHTMLEol;
    for (size_t x = 0 ; x < m_sBodyText.Count() ; x++) {
        sHTMLText += wxT("\t") + m_sBodyText[x] + sHTMLEol;
    }
    sHTMLText += wxT("</body>") + sHTMLEol;

    sHTMLText += wxT("</html>") + sHTMLEol;

    /* calculate size */
    m_size = sHTMLText.size();

    return sHTMLText;
}

/**
 *  Load image from file. Attempt to identify MIME type by file extension.
 */

bool serverPage::SetImageFile(wxString sFilename) {
    bool        bRes = false;
    wxString    sContentType;
    wxFileName  fname(sFilename);
    wxFileType* pType;
    wxFile      pFile;

    D(debug("serverPage::SetImageFile(%s)\n", sFilename.c_str()));

    if (fname.FileExists()) {

        pType = wxTheMimeTypesManager->GetFileTypeFromExtension(fname.GetExt());

        if (pType->GetMimeType(&sContentType)) {
            D(debug("Mime type reported = %s\n", sContentType.c_str()));
            m_sMimeType = sContentType;
            bRes = true;
        }

        if (pFile.Open( sFilename )) {
            size_t fileSize = pFile.Length();

            m_pBinaryData = (void*)malloc( fileSize );
            D(debug("-- binary data @ %p\n", m_pBinaryData));
            pFile.Read( m_pBinaryData, fileSize );
            m_size = m_nBinaryDataSize = fileSize;
            D(debug("-- read %ld bytes from file %s\n", fileSize, sFilename.c_str()));
            pFile.Close();

            m_type = PAGE_BINARY;
        }

    }

    return bRes;
}

/**
 *  Set image data pointer and length.
 */

bool serverPage::SetImageData(void* pData, size_t length) {
    D(debug("serverPage::SetImageData(%p, %ld)\n", pData, length));

    /* if image data already exists, free it... */
    if (m_pBinaryData != 0) {
        free(m_pBinaryData);
        m_pBinaryData       = 0L;
        m_nBinaryDataSize   = 0L;
    }

    m_pBinaryData = (void*)malloc(length);
    memcpy(m_pBinaryData, pData, length);

    m_nBinaryDataSize   = length;
    m_size              = length;
    m_type              = PAGE_BINARY;

    return true;
}

#ifdef  _DEBUG
void serverPage::Dump(FILE* fOut)
{
    wxString sHTML = HTML();

    fprintf(fOut,"---------------------------------------------------------\n");

    fprintf(fOut, "Page name      : %s\n", m_sPageName.c_str());
    fprintf(fOut, "Page title     : %s\n", m_sPageTitle.c_str());
    fprintf(fOut, "MIME type      : %s\n", m_sMimeType.c_str());
    fprintf(fOut, "Content Length : %ld\n", m_size);

    if (!m_sRedirect.IsEmpty()) {
        fprintf(fOut, "Redirect URL   : %s\n", m_sRedirect.c_str());
        fprintf(fOut, "Redirect Time  : %d Seconds\n", m_nRedirectTime);
    } else if (m_nRedirectTime > 0) {
        fprintf(fOut, "Refresh Time   : %d Seconds\n", m_nRedirectTime);
    }

    fprintf(fOut, "\n");
    fprintf(fOut, "%s\n", sHTML.c_str());

    fprintf(fOut,"---------------------------------------------------------\n");
    return;
}
#endif   // _DEBUG

/*----------------------------------------------------------------------------*/

serverCatalog::serverCatalog()
{
    // ctor
    D(debug("serverCatalog::serverCatalog()\n"));
}

/**
 *
 */

serverCatalog::~serverCatalog()
{
    // dtor
    D(debug("serverCatalog::~serverCatalog()\n"));
}

/**
 *
 */

void serverCatalog::AddPage(serverPage& newPage)
{
    D(debug("serverCatalog::AddPage(...)\n"));
    D(debug("-- adding page name [%s]\n", newPage.GetPageName().c_str()));

    m_pages[newPage.GetPageName()] = newPage;

    return;
}

/**
 *
 */

serverPage*     serverCatalog::GetPage(wxString sPageName)
{
    if (m_pages.find(sPageName) != m_pages.end()) {
        serverPage* pPage = &m_pages[sPageName];
        pPage->Update();
        return pPage;
    }

    return (serverPage*)0L;
}

/**
 *
 */

bool serverCatalog::GetPageArray(wxArrayString& sNameArray)
{
    PAGE_HASH::iterator pIter;

    D(debug("serverCatalog::GetPageArray(...)\n"));

    sNameArray.Clear();

    for (pIter = m_pages.begin() ; pIter != m_pages.end() ; pIter++) {
        sNameArray.Add( pIter->first );
    }

    return true;
}

#ifdef _DEBUG

/**
 *  Dump debugging information to output file.
 */

void serverCatalog::Dump(FILE* fOut)
{
    assert(fOut != 0L);

    fprintf(fOut,"---------------------------------------------------------\n");
    fprintf(fOut,"There are %ld pages in the catalog...\n", m_pages.size());

    PAGE_HASH::iterator pIter;

    for (pIter = m_pages.begin() ; pIter != m_pages.end() ; pIter++) {
        pIter->second.Dump(fOut);
    }

    fprintf(fOut,"---------------------------------------------------------\n");

    return;
}
#endif // _DEBUG

/**
 *  Helper functions in namespace HTML.
 */

namespace HTML {
    wxString BOLD(wxString sText) {
        return wxT("<b>") + sText + wxT("</b>");
    }
    wxString ITALIC(wxString sText) {
        return wxT("<i>") + sText + wxT("</i>");
    }
    wxString HEADING1(wxString sText) {
        return wxT("<h1>") + sText + wxT("</h1>");
    }
    wxString HEADING2(wxString sText) {
        return wxT("<h2>") + sText + wxT("</h2>");
    }
    wxString HEADING3(wxString sText) {
        return wxT("<h3>") + sText + wxT("</h3>");
    }
    wxString CENTER(wxString sText) {
        return wxT("<center>") + sText + wxT("</center>");
    }
    wxString LINK(wxString sText, wxString sURL) {
        return wxT("<a href=\"") + sURL + wxT("\">") + sText + wxT("</a>");
    }
    wxString IMAGE(wxString sSrc, wxString sAlt, int width, int height) {
        wxString sHTML;

        sHTML = wxT("<img src =\"") + sSrc + wxT("\"");
        if (!sAlt.IsEmpty()) {
            sHTML += wxT(" alt=\"") + sAlt + wxT("\"");
        }
        if (width > 0) {
            sHTML += wxT(" width=\"") + wxString::Format(wxT("%d"), width) + wxT("\"");
        }
        if (height > 0) {
            sHTML += wxT(" height=\"") + wxString::Format(wxT("%d"), height) + wxT("\"");
        }
        sHTML += wxT(">");

        return sHTML;
    }
    wxString BR() {
        return wxT("<br>");
    }
    wxString P(wxString sText) {
        return wxT("<p />") + sText;
    }
};
