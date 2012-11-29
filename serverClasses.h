#ifndef __SERVERCLASSES_H__
#define __SERVERCLASSES_H__

#include <wx/arrstr.h>
#include <wx/hashmap.h>

/* forward declarations */
class serverPage;
class serverCatalog;
class wxSocketBase;

typedef bool (*PAGE_CALLBACK)(serverPage*);

namespace HTML {
    wxString BOLD(wxString sText);
    wxString ITALIC(wxString sText);
    wxString HEADING1(wxString sText);
    wxString HEADING2(wxString sText);
    wxString HEADING3(wxString sText);
    wxString CENTER(wxString sText);
    wxString LINK(wxString sText, wxString sURL);
};

/**
 *
 */

class serverPage {
public:
    serverPage();
    serverPage(wxString sPageName,
               PAGE_CALLBACK pCBFunc = 0,
               void* pPageData = 0);
    serverPage(const serverPage& copy);

    virtual ~serverPage();

    bool            LoadFromFile(wxString sFilename);
    bool            SaveToFile(wxString sFilename);

    void            SetPageName(wxString sName);
    wxString        GetPageName();

    void            Clear();

    void            SetTitle(wxString sTitle);
    void            AddToHead(wxString sLine);
    void            AddToBody(wxString sLine);

    void*           GetPageData();
    void            SetPageData(void* pData);

    PAGE_CALLBACK   GetCallback();
    void            SetCallback(PAGE_CALLBACK cbFunc);

#ifdef  _DEBUG
    void            Dump(FILE* fOut);
#endif

    serverPage&     operator +=(wxString sLine);

//    size_t          size();

    bool            Send(wxSocketBase* pSocket);



protected:
    friend class serverCatalog;

    wxString        HTML();         ///< Generate HTML from head & body sections.
    void            Update();       ///< Call callback function to regenerate page.

    wxString        m_sPageName;
    wxString        m_sMimeType;
    wxString        m_sPageTitle;

    wxArrayString   m_sHeadText;
    wxArrayString   m_sBodyText;

    void*           m_pPageData;
    PAGE_CALLBACK   m_cbFunc;
    size_t          m_size;
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
    serverPage*     GetPage(wxString sPageName);

    bool            GetPageArray(wxArrayString& sNameArray);

#ifdef  _DEBUG
    void            Dump(FILE* fOut);
#endif

protected:
    PAGE_HASH       m_pages;
};

#endif // __SERVERCLASSES_H__
