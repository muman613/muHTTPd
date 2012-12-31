/**
 *  @file       serverClasses.cpp
 *  @author     Michael A. Uman
 *  @date       December 5, 2012
 *  @brief      Classes used by myHTTPd to represent pages, cookies, & queries.
 */

#include <wx/wx.h>
#include <wx/arrstr.h>
#include <wx/textfile.h>
#include <wx/filename.h>
#include <wx/socket.h>
#include <wx/mimetype.h>
#include <wx/tokenzr.h>
#include "serverClasses.h"
#include "myutils.h"
#include "dbgutils.h"

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( ArrayOfCookies );
WX_DEFINE_OBJARRAY( ArrayOfQueries );
WX_DEFINE_OBJARRAY( ArrayOfAttachPtr );



static wxString gsDataPath = wxT("data-dir");


/**
 *
 */

myAttachment::myAttachment(wxString sData)
:   m_sContentType(wxT("text/html")),
    m_pData(0L),
    m_dataLen(0L)
{
    wxString sTmpFile;
    wxString        sHeader;
    wxArrayString   sHeadArray;
    wxString        sContentType;
    wxString        sTmp, sTmp2;

    size_t lastOffset;

#ifdef  ENABLE_FORM_DUMP
    /* Dump the file to temporary file */
    if (!(sTmpFile = wxFileName::CreateTempFileName(wxT("att"), &m_tmpFile)).IsEmpty()) {
        D(debug("Created temporary file %s\n", sTmpFile.c_str()));

        if (m_tmpFile.IsOpened()) {
            m_tmpFile.Write( sData.GetData(), sData.Length() );
            m_tmpFile.Close();
    }
}
#endif

    wxStringTokenizer       partToke( sData, wxT("\n"));

    while ((sHeader = partToke.GetNextToken()) != wxT("\r")) {
        //D(debug("-- header %s\n", sHeader.c_str()));

        sHeadArray.Add( sHeader );

        wxStringTokenizer hdrToke( sHeader, wxT(":") );

        sTmp = hdrToke.GetNextToken();
        //D(debug("-- sTMP %s\n", sTmp.c_str()));

        if (sTmp.CmpNoCase(wxT("content-disposition")) == 0) {
            wxString sFields = hdrToke.GetNextToken();
            //D(debug("-- found content disposition!\n"));

            sFields.Trim(true);
            sFields.Trim(false);

            //D(debug("fields = [%s]\n", sFields.c_str()));

            wxStringTokenizer attributes( sFields, wxT(";") );

            sTmp2 = attributes.GetNextToken();
            sTmp2.Trim(true);
            sTmp2.Trim(false);

//            D(debug("sTmp2 = [%s]\n", sTmp2.c_str()));

            if (sTmp2.CmpNoCase(wxT("form-data")) == 0) {
                while (attributes.HasMoreTokens()) {
                    wxString sAttName, sAttValue;
                    wxString subAttr = attributes.GetNextToken().Trim(false).Trim();

                    //D(debug("---- sub attribute = %s\n", subAttr.c_str()));

                    wxStringTokenizer subToke( subAttr, wxT("=") );

                    sAttName = subToke.GetNextToken().Trim(false).Trim();
                    sAttValue = subToke.GetNextToken().Trim(false).Trim();

                    D(debug("Attribute name [%s] value [%s]\n", sAttName.c_str(), sAttValue.c_str()));

                    if (sAttName.CmpNoCase(wxT("name")) == 0) {
                        m_sName = sAttValue;
                        m_sName.Replace(wxT("\""), wxEmptyString);
                    } else if (sAttName.CmpNoCase(wxT("filename")) == 0) {
                        m_sFilename = sAttValue;
                        m_sFilename.Replace(wxT("\""), wxEmptyString);
                    } else {
                        /* */
                    }
                }
            }
        } else if (sTmp.CmpNoCase(wxT("content-type"))  == 0) {
            m_sContentType = hdrToke.GetNextToken().Trim(false).Trim();
            D(debug("-- Found content type of %s\n", m_sContentType.c_str()));
        }

    }
    lastOffset = partToke.GetPosition();

    //D(debug("-- last offset @ %ld\n", lastOffset));

    sData = sData.Mid( lastOffset );

    m_pData     = (unsigned char*)malloc( sData.Length() );
    m_dataLen   = sData.Length();

    memcpy( m_pData, sData.GetData(), sData.Length() );

//  write_file();

    return;
}

/**
 *
 */

myAttachment::~myAttachment()
{
#ifdef  ENABLE_FORM_DUMP
//    if (m_tmpFile.IsOpened()) {
//        m_tmpFile.Close();
//    }
#endif

    if (m_pData) {
        free(m_pData);
        m_pData = 0L;
        m_dataLen = 0L;
    }
}

/**
 *
 */

bool myAttachment::write_file() {
    bool        bRes = false;
    wxFile      outFile;

    if (!m_sFilename.IsEmpty()) {
        wxString    sFullPath = gsDataPath + wxT("/") + m_sFilename;

        if (outFile.Open( sFullPath.c_str(), wxFile::write)) {
            outFile.Write( m_pData, m_dataLen );
            bRes = true;
    	}
	}

    return bRes;
}

/**
 *
 */

wxString myAttachment::string() const
{
    wxString sRes;

    sRes = wxString::From8BitData( (const char*)m_pData, m_dataLen );

    return sRes;
}

//void myAttachment::add_buffer(unsigned char* pBuffer, size_t len) {
//    if (m_tmpFile.IsOpened()) {
//        m_tmpFile.Write(pBuffer, len);
//    }
//}
/**
 *  Cookie class, used to store cookies.
 */

myCookie::myCookie( wxString sName, wxString sValue,  wxString sExpireDate,
                    wxString sPath, wxString sDomain, bool bSecure )
:   m_sName(sName), m_sValue(sValue),
    m_sExpireDate(sExpireDate), m_sPath(sPath),
    m_sDomain(sDomain), m_bSecure(bSecure)
{
    // ctor
}

myCookie::myCookie(const myCookie& copy)
:   m_sName(copy.m_sName), m_sValue(copy.m_sValue),
    m_sExpireDate(copy.m_sExpireDate), m_sPath(copy.m_sPath),
    m_sDomain(copy.m_sDomain), m_bSecure(copy.m_bSecure)
{
    // copy ctor
}

myCookie::~myCookie()
{
    // dtor
}

/**
 *  Return a string containing the cookie header.
 */

wxString myCookie::header() {
    wxString sHeader;

    sHeader = wxT("Set-Cookie: ") + m_sName + wxT("=") + m_sValue + wxT(";");
    if (!m_sExpireDate.IsEmpty()) {
        sHeader += wxT(" expires=") + m_sExpireDate + wxT(";");
    }
    if (!m_sPath.IsEmpty()) {
        sHeader += wxT(" path=") + m_sPath + wxT(";");
    }
    if (!m_sDomain.IsEmpty()) {
        sHeader += wxT(" domain=") + m_sDomain + wxT(";");
    }
    if (m_bSecure) {
        sHeader += wxT(" secure");
    }

    return sHeader;
}

wxString myCookie::name() {
    return m_sName;
}


wxString myCookie::value() {
    return m_sValue;
}

wxString myCookie::expire_date() {
    return m_sExpireDate;
}

wxString myCookie::path() {
    return m_sPath;
}

wxString myCookie::domain() {
    return m_sDomain;
}

bool myCookie::secure() {
    return m_bSecure;
}

//#define ENABLE_DEBUG_SEND            1    // Enable header debugging

wxString sHTMLEol = wxT("\r\n");
static wxString sServerID = wxT("myHTTPd-1.0.0");

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
    m_type(serverPage::PAGE_HTML),
    m_flags(0L),
    m_server(0L)
{
    // ctor
}

/**
 *
 */

serverPage::serverPage(const serverPage& copy)
:   m_sPageName(copy.m_sPageName),
    m_sMimeType(copy.m_sMimeType),
    m_sPageTitle(copy.m_sPageTitle),
    m_sRedirect(copy.m_sRedirect),
    m_nRedirectTime(copy.m_nRedirectTime),
    m_sJScriptText(copy.m_sJScriptText),
    m_sHeadText(copy.m_sHeadText),
    m_sBodyText(copy.m_sBodyText),
    m_pPageData(copy.m_pPageData),
    m_cbFunc(copy.m_cbFunc),
    m_size(copy.m_size),
    m_type(copy.m_type),
    m_flags(copy.m_flags),
    m_cookies(copy.m_cookies),
    m_server(copy.m_server),
    m_sFavIconName(copy.m_sFavIconName)
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
    m_type(serverPage::PAGE_HTML),
    m_flags(0L),
    m_server(0L)
{
    // ctor
}

/**
 *
 */

serverPage::serverPage(wxString sPageName, PAGE_TYPE type, PAGE_CALLBACK pCBFunc)
:   m_sPageName(sPageName),
    m_sMimeType(wxEmptyString),
    m_nRedirectTime(0),
    m_pPageData(0L),
    m_pBinaryData(0L),
    m_nBinaryDataSize(0L),
    m_cbFunc(pCBFunc),
    m_type(type),
    m_flags(0L),
    m_server(0L)
{
    // ctor
    switch (type) {
    case PAGE_HTML:
        m_sMimeType = wxT("text/html");
        break;
    case PAGE_CSS:
        m_sMimeType = wxT("text/css");
        break;
    case PAGE_JSCRIPT:
        m_sMimeType = wxT("text/javascript");
        break;
    case PAGE_BINARY:
        {
            wxFileName  sFileName(sPageName);
            wxString    sType = sFileName.GetExt();

            D(debug("-- looking for mime type for extension [%s]\n", sType.c_str()));

            m_sMimeType = GetMimeFromExtenstion( sType );

            D(debug("-- mime type is [%s]\n", m_sMimeType.c_str()));
        }
        break;
    }
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
    m_sPageTitle        = copy.m_sPageTitle;
    m_sRedirect         = copy.m_sRedirect;
    m_nRedirectTime     = copy.m_nRedirectTime;
    m_sHeadText         = copy.m_sHeadText;
    m_sJScriptText      = copy.m_sJScriptText;
    m_sBodyText         = copy.m_sBodyText;
    m_pPageData         = copy.m_pPageData;
    m_cbFunc            = copy.m_cbFunc;
    m_size              = copy.m_size;
    m_type              = copy.m_type;
    m_flags             = copy.m_flags;
    m_cookies           = copy.m_cookies;
    m_server            = copy.m_server;
    m_sFavIconName      = copy.m_sFavIconName;

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

/**
 *
 */

void     serverPage::SetPageName(wxString sPageName)
{
    m_sPageName = sPageName;
}

/**
 *
 */

wxString serverPage::GetPageName()
{
    return m_sPageName;
}

/**
 *
 */

bool serverPage::LoadJScript(wxString sScriptName) {
    bool            bRes = false;
    wxTextFile      file(sScriptName);

    if (file.Open()) {
        for (size_t i = 0 ; i < file.GetLineCount() ; i++) {
            AddToScript( file[i] );
        }
        bRes = true;
    }

    return bRes;
}
/**
 *
 */

void serverPage::AddToScript(wxString sLine) {
    m_sJScriptText.Add( sLine);
}

/**
 *
 */

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

wxString serverPage::GetTitle() {
    D(debug("serverPage::GetTitle()\n"));
    return m_sPageTitle;
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

wxString serverPage::GetMimeType() {
    D(debug("serverPage::GetMimeType()\n"));
    return m_sMimeType;
}

/**
 *  Function sets a redirect URL and a time.
 */

void  serverPage::SetRedirectTo(wxString sRedirect, int nSec)
{
    D(debug("serverPage::SetRedirectTo(%s, %d)\n", sRedirect.c_str(), nSec));

    m_sRedirect     = sRedirect;
    m_nRedirectTime = nSec;

    return;
}

/**
 *  Function sets a refresh time.
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
    m_sJScriptText.Clear();
    m_sBodyText.Clear();
    m_sRedirect.Clear();
    m_cookies.Clear();

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

void serverPage::Update(Request* pMap) {
    D(debug("serverPage::Update()\n"));

    if (m_cbFunc != 0) {
        D(debug("-- calling call-back function!\n"));
        (*m_cbFunc)(this, pMap);
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
    wxString    sHTTP;
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

    sHTTP =  wxT("HTTP/1.1 200 OK") + sHTMLEol;
    sHTTP += wxT("Server: ") + sServerID + sHTMLEol;
    sHTTP += wxT("Content-Type: ") + m_sMimeType + sHTMLEol;
    sHTTP += wxT("Expires: ") +
             wxDateTime::Now().Format( wxT("%a, %d %b %Y %T GMT") , wxDateTime::UTC ) +
             sHTMLEol;

#if 1
    sHTTP += wxT("Connection: Close") + sHTMLEol;
#else
    sHTTP += wxT("Connection: Keep-Alive") + sHTMLEol;
#endif

    /* Add cookies to header */
    if (!m_cookies.IsEmpty()) {
        D(debug("-- adding cookies to header!\n"));
        for (size_t x = 0 ; x < m_cookies.Count() ; x++) {
            sHTTP += m_cookies[x].header() + sHTMLEol;
        }
    }

    sHTTP += sHTMLEol;

    /* Write the HTTP header... */
    pSocket->Write( sHTTP.c_str(), sHTTP.Length() );

#ifdef  ENABLE_DEBUG_SEND
    fwrite(sHTTP.c_str(), sHTTP.Length(), 1, fOut);
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

    /* Add the favorite icon link if it exists. */
    if (!m_sFavIconName.IsEmpty()) {
        sTmp = wxString::Format(wxT("\t<link rel=\"icon\" href=\"%s\" type=\"image/x-icon\">"),
                                m_sFavIconName.c_str()) + sHTMLEol;
        sHTMLText += sTmp;
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

    /* Send any JScript code */

    if (!m_sJScriptText.IsEmpty()) {
        sHTMLText += wxT("<script type=\"text/javascript\">") + sHTMLEol;
        for (size_t x = 0 ; x < m_sJScriptText.Count(); x++) {
            sHTMLText += wxT("\t") + m_sJScriptText[x] + sHTMLEol;
        }
        sHTMLText += wxT("</script>") + sHTMLEol;
    }

    sHTMLText += wxT("</head>") + sHTMLEol;

    /* generate the BODY section */

    if (m_flags & FLAG_PRINT) {
        sHTMLText += wxT("<body onload=\"window.print()\">") + sHTMLEol;
    } else {
    	sHTMLText += wxT("<body>") + sHTMLEol;
    }
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

/**
 *  Add a cookie to the page.
 */

bool serverPage::AddCookie(wxString sName, wxString sValue, wxString sExpireDate,
                           wxString sPath, wxString sDomain, bool bSecure)
{
    myCookie    newCookie(sName, sValue, sExpireDate, sPath, sDomain, bSecure);

    D(debug("serverPage::AddCookie()\n"));

    m_cookies.Add(newCookie);

    return true;
}

/**
 *  Add a cookie to the page.
 */

bool serverPage::AddCookie(wxString sName, wxString sValue, wxTimeSpan& tSpan,
                           wxString sPath, wxString sDomain, bool bSecure)
{
    wxString    sExpireDate;
    wxDateTime  expire,
                now = wxDateTime::Now();

    D(debug("serverPage::AddCookie(bytimespan)\n"));

    expire = now + tSpan;
    sExpireDate = expire.Format( wxT("%a, %d %b %Y %T GMT") , wxDateTime::UTC );

    D(debug("-- expire date is %s\n", sExpireDate.c_str()));

    AddCookie(sName, sValue, sExpireDate, sPath, sDomain, bSecure);

    return true;
}


/**
 *
 */

void serverPage::SetFlags(wxUint32 flags) {
    m_flags |= flags;
}

/**
 *
 */

void serverPage::ClearFlags(wxUint32 flags) {
    m_flags &= ~flags;
}


void serverPage::SetFavIconName(wxString sIconName) {
    m_sFavIconName = sIconName;
    return;
}

wxString serverPage::GetFavIconName() const {
    return m_sFavIconName;
}


void serverPage::BodyFromString(const char* szBodyData)
{
    wxString sBodyData = szBodyData;
    wxStringTokenizer bodyToken( sBodyData, wxT("\n") );
    wxString sTmp;

    D(debug("serverPage::BodyFromString()\n"));

    sTmp = bodyToken.GetNextToken();
    if (!sTmp.IsEmpty()) {
        m_sBodyText.Clear();

        while (!sTmp.IsEmpty()) {
            m_sBodyText.Add( sTmp );
            sTmp = bodyToken.GetNextToken();
        }
    }

    return;
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

serverPage* serverCatalog::GetPage(wxString sPageName, Request* pRequest)
{
    if (m_pages.find(sPageName) != m_pages.end()) {
        serverPage* pPage = &m_pages[sPageName];
        pPage->Update(pRequest);
        return pPage;
    }

    return (serverPage*)0L;
}

bool serverCatalog::PageExists(wxString sPageName)
{
    bool bRes = false;

    D(debug("serverCatalog::PageExists(%s)\n", sPageName.c_str()));

    if (m_pages.find(sPageName) != m_pages.end()) {
        D(debug("-- found page!\n"));
        bRes = true;
    }

    return bRes;
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
