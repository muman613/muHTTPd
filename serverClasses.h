/**
 *  @file       serverClasses.h
 *  @author     Michael A. Uman
 *  @date       December 5, 2012
 *  @brief      Classes used by myHTTPd to represent pages, cookies, & queries.
 */

#ifndef __SERVERCLASSES_H__
#define __SERVERCLASSES_H__

#include <wx/arrstr.h>
#include <wx/hashmap.h>
#include <wx/file.h>

/* forward declarations */
class serverPage;
class serverCatalog;
class wxSocketBase;
class HEADER_MAP;
class Request;
class wxFile;
class myHTTPd;

typedef bool (*PAGE_CALLBACK)(serverPage*, Request*);

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
    wxString IMAGE(wxString sSrc, wxString sAlt = wxEmptyString,
                   int width = 0, int height = 0);
    wxString SELECT(wxString sName, const wxArrayString& sOptions, wxString sDefault = wxEmptyString);
    wxString FILEBOX(wxString sFilename, wxString sCaption = wxEmptyString);
};

class myStyleAttribute {
public:
    myStyleAttribute();
    myStyleAttribute(const wxString& sStyleAttribute, const wxString& sStyleValue);
    myStyleAttribute(const myStyleAttribute& copy);
    virtual ~myStyleAttribute();

    myStyleAttribute& operator =(const myStyleAttribute& copy);

    wxString AttributeName() const;
    wxString AttributeValue() const;

protected:
    wxString        m_sAttributeName;
    wxString        m_sAttributeValue;
};

WX_DECLARE_OBJARRAY( myStyleAttribute, ArrayOfAttributes );

class myStyle {
public:
    myStyle();
    myStyle(const wxString& sStyleTag, const wxString& sStyleClass = wxEmptyString);
    myStyle(const myStyle& copy);
    virtual ~myStyle();

    myStyle& operator =(const myStyle& copy);

    myStyle& AddAttribute(const wxString& sAttributeName, const wxString& sAttributeValue);

    myStyle& operator += (myStyleAttribute& newAttribute);
    myStyle& operator + (myStyleAttribute& newAttribute);




    wxString                GetCSS();

protected:
    size_t  GetCount() const;
    const myStyleAttribute& operator [](size_t index);

    wxString                m_sCSSTag;
    wxString                m_sCSSClass;

    ArrayOfAttributes       m_attrArray;
};

WX_DECLARE_OBJARRAY( myStyle, ArrayOfStyles );

/**
 *  Class which encapsulates a multi-part mime attachment.
 */

class myAttachment {
public:
    myAttachment(const wxString dataBuffer);
    myAttachment(const myAttachment& copy);
    virtual ~myAttachment();

    wxString        name() const     { return m_sName; }
    wxString        fname() const    { return m_sFilename; }
    wxUint32        size() const     { return m_dataLen; }
    wxUint8*        data() const     { return m_pData; }
    wxString        type() const     { return m_sContentType; }
    wxString        string() const;

protected:

    bool            write_file();

    wxString        m_sName;
    wxString        m_sFilename;
    wxString        m_sContentType;
    wxFile          m_tmpFile;
    wxUint8*        m_pData;
    wxUint32        m_dataLen;
};

WX_DECLARE_OBJARRAY( myAttachment*, ArrayOfAttachPtr );

/**
 *  Cookie class represents a HTTP cookie object.
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
 *  Class used to store query name/value pairs.
 */

class myQuery {
public:
    myQuery(wxString sId, wxString sValue) : m_sId(sId), m_sValue(sValue) {}
    virtual ~myQuery() {}

    wxString m_sId;
    wxString m_sValue;
};

WX_DECLARE_OBJARRAY( myQuery, ArrayOfQueries );


/**
 *  Class encapsulates an HTML page.
 */

class serverPage {
public:
    enum _serverFlags {
        FLAG_PRINT = (1L << 0),
    };

    typedef enum _pageType {
        PAGE_HTML,
        PAGE_CSS,
        PAGE_JSCRIPT,
        PAGE_BINARY,
    } PAGE_TYPE;

    serverPage();
    serverPage(wxString sPageName, PAGE_CALLBACK pCBFunc = 0L);
    serverPage(wxString sPageName, PAGE_TYPE type, PAGE_CALLBACK pCBFunc = 0L);
    serverPage(const serverPage& copy);

    virtual ~serverPage();

    serverPage&     operator = (const serverPage& copy);

//    bool            LoadFromFile(wxString sFilename);
    bool            SaveToFile(wxString sFilename);

    void            SetPageName(wxString sName);
    wxString        GetPageName();

    void            Clear();

    void            SetTitle(wxString sTitle);
    wxString        GetTitle();

    void            SetMimeType(wxString sMimeType);
    wxString        GetMimeType();

    void            SetRedirectTo(wxString sRedirect, int nSec = 2);
    void            SetRefreshTime(int nSec = 2);

    void            AddToHead(wxString sLine);
    void            AddToScript(wxString sLine);
    void            AddToBody(wxString sLine);

    void            BodyFromString(const char* sBodyData);

    void*           GetPageData();
    void            SetPageData(void* pData);

    bool            SetImageFile(wxString sFilename);
    bool            SetImageData(void* pData, size_t length);

    PAGE_CALLBACK   GetCallback();
    void            SetCallback(PAGE_CALLBACK cbFunc);

    bool            LoadJScript(wxString sFilename);

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

    bool            AddCookie(wxString sName, wxString sValue,
                              wxTimeSpan& tSpan,
                              wxString    sPath     = wxEmptyString,
                              wxString    sDomain   = wxEmptyString,
                              bool        bSecure   = false);

    void            SetFlags(wxUint32 flags);
    void            ClearFlags(wxUint32 flags);

    void            server(myHTTPd* pServer) { m_server = pServer; }
    myHTTPd*        server() const { return m_server; }

    /* Functions to get/set the favorite icon name */
    void            SetFavIconName(wxString sIconName);
    wxString        GetFavIconName() const;

protected:
    friend class myHTTPdThread;
    friend class serverCatalog;

    wxString        CSS();                          ///< Generate CSS script
    wxString        HTML();                         ///< Generate HTML from
                                                    ///<   head & body sections.
    void            Update(Request* pRequest = 0);  ///< Call callback function
                                                    ///<   to regenerate page.

    wxString        m_sPageName;
    wxString        m_sMimeType;
    wxString        m_sPageTitle;

    wxString        m_sRedirect;
    int             m_nRedirectTime;

    wxArrayString   m_sJScriptText;
    wxArrayString   m_sHeadText;
    wxArrayString   m_sBodyText;

    void*           m_pPageData;

    void*           m_pBinaryData;
    size_t          m_nBinaryDataSize;

    PAGE_CALLBACK   m_cbFunc;
    size_t          m_size;

    PAGE_TYPE       m_type;
    wxUint32        m_flags;

//    HEADER_MAP*     m_pHeaders;
    ArrayOfCookies  m_cookies;

    myHTTPd*        m_server;

    wxString        m_sFavIconName;
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
    serverPage*     GetPage(wxString sPageName, Request* pRequest);
    bool            PageExists(wxString sPageName);

    bool            GetPageArray(wxArrayString& sNameArray);

#ifdef  _DEBUG
    void            Dump(FILE* fOut);
#endif

protected:
    PAGE_HASH       m_pages;
};

extern wxString sHTMLEol;

#endif // __SERVERCLASSES_H__
