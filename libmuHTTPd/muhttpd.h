/**
 *  @file       myhttpd.h
 *  @author     Michael A. Uman
 *  @date       December 3, 2012
 */


#ifndef MYHTTPD_H
#define MYHTTPD_H

#include <wx/wx.h>
#include <wx/thread.h>
#include <wx/socket.h>
#include <wx/hashmap.h>
#include <wx/file.h>

#include "serverClasses.h"

class muHTTPd;
class muHTTPdThread;

#define DEFAULT_PORT            (8080)

WX_DECLARE_STRING_HASH_MAP( wxString, HEADER_MAP );

/**
 *  Request class keeps the information about the request for the callback.
 */

class muRequest {
public:
    wxString            FindCookie(wxString sName);
    wxString            FindHeader(wxString sHeader);
    wxString            FindQuery(wxString sName);
    const myAttachment* FindAttach(wxString sName);
    wxString            FindReferer();
    wxString            FindUserAgent();
    wxString            FindHost();

    HEADER_MAP          m_headers;
    ArrayOfCookies      m_cookies;
    ArrayOfQueries      m_queries;
    ArrayOfAttachPtr    m_attached;
};

/**
 *  Class representing background thread of webserver.
 */

class muHTTPdThread : public wxThread {
public:
    muHTTPdThread(muHTTPd* parent, int portNum);
    virtual ~muHTTPdThread();

    ExitCode Entry();

protected:
    void                handle_connection(wxSocketBase* pSock);
    void                handle_get_method(wxSocketBase* pSock);
    void                handle_post_method(wxSocketBase* pSock);

    void                ReturnError(wxSocketBase* pSocket, int code, char* description);

    void                Clear();

private:
    void                parse_request();
    bool                receive_request(wxSocketBase* pSocket);
    bool                handle_attachment(wxString sData);

    int                 m_portNum;
    wxSocketServer*     m_sockServer;
    wxArrayString       m_requestArray;

    wxString            m_sLocalHost;
    wxString            m_sLocalPort;
    wxString            m_sPeerHost;
    wxString            m_sPeerPort;

    wxString            m_method;
    wxString            m_url;
    wxString            m_reqver;

    wxString            m_useragent;
    wxString            m_host;

    muHTTPd*            m_pParent;
    muRequest           m_Request;
};


/**
 *  The control class for the HTTP daemon.
 */

class muHTTPd {
    public:
        muHTTPd(int portNum = DEFAULT_PORT);
        virtual ~muHTTPd();

        bool            SetPort(int portNum = DEFAULT_PORT);

        bool            Start();
        bool            Stop();

        bool            AddPage(serverPage& page);
        serverPage*     GetPage(wxString sPageName, muRequest* pRequest);
        bool            PageExists(wxString sPageName);

        void            Set404Page(serverPage& page);
        serverPage*     Get404Page();

        enum logType {
            LOG_MSG,
            LOG_WARN,
            LOG_ERROR,
        };

        bool            SetLogFile(wxString sLogFilename, bool bAppend = false);
        bool            LogMessage(logType nType, wxString sMsg);
        void            CloseLogFile();

    protected:

        int             m_nPort;

        muHTTPdThread*  m_serverThread;
        serverCatalog   m_catalog;
        wxString        m_sLogFilename;     ///< Log filename.
        wxFile*         m_pLogFile;         ///< File used for logging.
        serverPage*     m_p404Page;         ///< User specified 404 page.

    private:
};

#endif // MYHTTPD_H
