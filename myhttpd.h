#ifndef MYHTTPD_H
#define MYHTTPD_H

#include <wx/wx.h>
#include <wx/thread.h>
#include <wx/socket.h>
#include <wx/hashmap.h>
#include <wx/file.h>

#include "serverClasses.h"

class myHTTPd;
class myHTTPdThread;


WX_DECLARE_STRING_HASH_MAP( wxString, HEADER_MAP );

/**
 *  Class representing background thread of webserver.
 */

class myHTTPdThread : public wxThread {
public:
    myHTTPdThread(myHTTPd* parent, int portNum);
    virtual ~myHTTPdThread();

    ExitCode Entry();

protected:
    void                handle_connection(wxSocketBase* pSock);
    void                handle_get_method(wxSocketBase* pSock);

    void                ReturnError(wxSocketBase* pSocket, int code, char* description);

private:
    void                parse_buffer();

    static int          m_bufSize;
    wxUint8*            m_buf;
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

    myHTTPd*            m_pParent;

    HEADER_MAP          m_headers;
};

/**
 *
 */

class myHTTPd {
    public:
        myHTTPd(int portNum = 8080);
        virtual ~myHTTPd();

        bool            Start();
        bool            Stop();

        void            AddPage(serverPage& page);
        serverPage*     GetPage(wxString sPageName, HEADER_MAP* pMap);

        void            SetLogFile(wxString sLogFilename);

    protected:
        int             m_nPort;

        myHTTPdThread*  m_serverThread;
        serverCatalog   m_catalog;
        wxString        m_sLogFilename;
        wxFile*         m_pLogFile;

    private:
};

#endif // MYHTTPD_H
