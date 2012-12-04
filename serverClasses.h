#ifndef __SERVERCLASSES_H__
#define __SERVERCLASSES_H__

#include <wx/arrstr.h>
#include <wx/hashmap.h>

//#include "myhttpd.h"

/* forward declarations */
class serverPage;
class serverCatalog;
class wxSocketBase;
class HEADER_MAP;

typedef bool (*PAGE_CALLBACK)(serverPage*);

#define     ADD_PAGE(server, name, stub)                                        \
    {                                                                           \
        serverPage* pNewPage = new serverPage( (name), (stub) );                \
        (server)->AddPage( *pNewPage );                                         \
        delete pNewPage;                                                        \
    }


/**
 *  Helper functions for HTML formatting.
 */

namespace HTML {
    wxString BOLD(wxString sText);
    wxString ITALIC(wxString sText);
    wxString HEADING1(wxString sText);
    wxString HEADING2(wxString sText);
    wxString HEADING3(wxString sText);
    wxString CENTER(wxString sText);
    wxString LINK(wxString sText, wxString sURL);
    wxString IMAGE(wxString sSrc, wxString sAlt = wxEmptyString, int width = 0, int height = 0);
};

/**
 *
 */

class myCookie {
public:
    myCookie(wxString sName, wxString sValue,
             wxString sExpireDate = wxEmptyString,
             wxString sPath       = wxEmptyString,
             wxString sDomain     = wxEmptyString,
             bool bSecure         = false);
    myCookie(const myCookie& copy);
    virtual ~myCookie();

    myCookie& operator =(const myCookie& copy);

    wxString        header();

    wxString        name();
    wxString        value();
    wxString        expire_date();
    wxString        path();
    wxString        domain();
    bool            secure();

protected:
    wxString        m_sName;
    wxString        m_sValue;
    wxString        m_sExpireDate;
    wxString        m_sPath;
    wxString        m_sDomain;
    bool            m_bSecure;
};

WX_DECLARE_OBJARRAY( myCookie, ArrayOfCookies );

/**
 *  Class encapsulates an HTML page.
 */

class serverPage {
public:
    serverPage();
    serverPage(wxString sPageName, PAGE_CALLBACK pCBFunc = 0);
    serverPage(const serverPage& copy);

    virtual ~serverPage();

    serverPage&     operator = (const serverPage& copy);

//    bool            LoadFromFile(wxString sFilename);
    bool            SaveToFile(wxString sFilename);

    void            SetPageName(wxString sName);
    wxString        GetPageName();

    void            Clear();

    void            SetTitle(wxString sTitle);
    void            SetMimeType(wxString sMimeType);

    void            SetRedirectTo(wxString sRedirect, int nSec = 2);
    void            SetRefreshTime(int nSec = 2);

    void            AddToHead(wxString sLine);
    void            AddToBody(wxString sLine);

    void*           GetPageData();
    void            SetPageData(void* pData);

    bool            SetImageFile(wxString sFilename);
    bool            SetImageData(void* pData, size_t length);

    PAGE_CALLBACK   GetCallback();
    void            SetCallback(PAGE_CALLBACK cbFunc);

#ifdef  _DEBUG
    void            Dump(FILE* fOut);
#endif

    serverPage&     operator +=(wxString sLine);

    bool            Send(wxSocketBase* pSocket);

    bool            AddCookie(wxString sName, wxString sValue,
                              wxString sExpireDate  = wxEmptyString,
                              wxString sPath        = wxEmptyString,
                              wxString sDomain      = wxEmptyString,
                              bool bSecure          = false);

protected:

    typedef enum {
        PAGE_HTML,
        PAGE_BINARY,
    } PAGE_TYPE;

    friend class serverCatalog;

    wxString        HTML();                         ///< Generate HTML from
                                                    ///<   head & body sections.
    void            Update(HEADER_MAP* pMap = 0);   ///< Call callback function
                                                    ///<   to regenerate page.

    wxString        m_sPageName;
    wxString        m_sMimeType;
    wxString        m_sPageTitle;

    wxString        m_sRedirect;
    int             m_nRedirectTime;

    wxArrayString   m_sHeadText;
    wxArrayString   m_sBodyText;

    void*           m_pPageData;

    void*           m_pBinaryData;
    size_t          m_nBinaryDataSize;

    PAGE_CALLBACK   m_cbFunc;
    size_t          m_size;

    PAGE_TYPE       m_type;

    HEADER_MAP*     m_pHeaders;
    ArrayOfCookies  m_cookies;
};

WX_DECLARE_STRING_HASH_MAP( serverPage, PAGE_HASH );

/**
 *
 */

class serverCatalog {
public:
    serverCatalog();
    virtual ~serverCatalog();

    void            AddPage(serverPage& newPage);
    serverPage*     GetPage(wxString sPageName, HEADER_MAP* pMap);

    bool            GetPageArray(wxArrayString& sNameArray);

#ifdef  _DEBUG
    void            Dump(FILE* fOut);
#endif

protected:
    PAGE_HASH       m_pages;
};

#endif // __SERVERCLASSES_H__
