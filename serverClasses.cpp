#include <wx/wx.h>
#include <wx/arrstr.h>
#include <wx/textfile.h>
#include <wx/filename.h>
#include <wx/socket.h>
#include "serverClasses.h"
#include "dbgutils.h"


static wxString sHTMLEol = wxT("\r\n");

serverPage::serverPage()
:   m_sMimeType(wxT("text/html")),
    m_pPageData(0L),
    m_cbFunc(0L)
{
    // ctor
}

serverPage::serverPage(const serverPage& copy)
:   m_sPageName(copy.m_sPageName),
    m_sMimeType(copy.m_sMimeType),
    m_sHeadText(copy.m_sHeadText),
    m_sBodyText(copy.m_sBodyText),
    m_pPageData(copy.m_pPageData),
    m_cbFunc(copy.m_cbFunc)
{
    // ctor
}

serverPage::serverPage(wxString sPageName, PAGE_CALLBACK pCBFunc, void* pPageData)
:   m_sPageName(sPageName),
    m_sMimeType(wxT("text/html")),
    m_pPageData(pPageData),
    m_cbFunc(pCBFunc)
{
    // ctor
}

serverPage::~serverPage()
{
    // dtor
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

serverPage&     serverPage::operator +=(wxString sLine) {
    AddToBody(sLine);
    return *this;
}

void serverPage::SetTitle(wxString sTitle)
{
    m_sPageTitle = sTitle;
}

void serverPage::Clear() {
    D(debug("serverPage::Clear()\n"));
    m_sHeadText.Clear();
    m_sBodyText.Clear();
}


void serverPage::Update() {
    D(debug("serverPage::Update()\n"));

    if (m_cbFunc != 0) {
        D(debug("-- calling call-back function!\n"));
        (*m_cbFunc)(this);
    }

    return;
}

PAGE_CALLBACK serverPage::GetCallback()
{
    return m_cbFunc;
}

void serverPage::SetCallback(PAGE_CALLBACK cbFunc)
{
    D(debug("serverPage::SetCallback(%p)\n", cbFunc));
    m_cbFunc = cbFunc;
}

/**
 *
 */

bool serverPage::Send(wxSocketBase* pSocket)
{
    bool        bRes = false;
    char        buf[500];
    wxString    sHTML;

    D(debug("serverPage::Send(%p)\n", pSocket));

    sHTML = HTML();

    sprintf(buf, "HTTP/1.1 200 OK\r\nserver: myHTTPd-1.0.0\r\ncontent-type: %s\r\ncontent-length: %ld\r\n\r\n",
            m_sMimeType.c_str(), m_size );
    pSocket->Write( buf, strlen(buf) );
    pSocket->Write( sHTML.c_str(), sHTML.size());

    bRes = true;

    return bRes;
}

/**
 *  Generate the actual HTML for the page from the HEAD and BODY sections.
 */

wxString serverPage::HTML() {
    wxString sHTMLText;

    sHTMLText  = wxT("<!DOCTYPE html>") + sHTMLEol;
    sHTMLText += wxT("<html>") + sHTMLEol;

    /* generate the HEAD section */
    sHTMLText += wxT("<head>") + sHTMLEol;
    sHTMLText += wxT("\t<title>") + m_sPageTitle + wxT("</title>") + sHTMLEol;
    for (size_t x = 0 ; x < m_sHeadText.Count() ; x++) {
        sHTMLText += wxT("\t") + m_sHeadText[x] + sHTMLEol;
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

#ifdef  _DEBUG
void serverPage::Dump(FILE* fOut)
{
    wxString sHTML = HTML();

    fprintf(fOut,"---------------------------------------------------------\n");

    fprintf(fOut, "Page name      : %s\n", m_sPageName.c_str());
    fprintf(fOut, "Page title     : %s\n", m_sPageTitle.c_str());
    fprintf(fOut, "MIME type      : %s\n", m_sMimeType.c_str());
    fprintf(fOut, "Content Length : %ld\n", m_size);
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
 *  Dump debugging information to console.
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
};
