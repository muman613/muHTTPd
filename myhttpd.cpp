/**
 *  @file       myhttpd.cpp
 *  @author     Michael A. Uman
 *  @date       December 2, 2012
 */

#include <wx/wx.h>
#include <wx/tokenzr.h>
#include "myhttpd.h"
#include "dbgutils.h"

//#define DUMP_RAW_REQUEST          1


int myHTTPdThread::m_bufSize = 5000;    // Maximum request size

/**
 *
 */

myHTTPdThread::myHTTPdThread(myHTTPd* parent, int portNum)
:   wxThread(wxTHREAD_JOINABLE),
    m_buf(0),
    m_portNum(portNum),
    m_pParent(parent)
{
    // ctor
    m_buf = new wxUint8[m_bufSize];
}

/**
 *
 */

myHTTPdThread::~myHTTPdThread()
{
    // dtor
    delete [] m_buf;
}

/**
 *  Break request buffer up into seperate lines stored in wxArrayString.
 */

void myHTTPdThread::parse_buffer()
{
    char*       pos;
    wxString    sLine, sQuery;
    int         nPos;

    D(debug("myHTTPdThread::parse_buffer()\n"));

    m_requestArray.Clear();
    m_method.Clear();
    m_url.Clear();
    m_reqver.Clear();

    pos = (char *)m_buf;

    while (*pos != 0) {
        if (*pos == '\n') {
            m_requestArray.Add( sLine );
            sLine.Clear();
        } else {
            if (*pos != '\r')
                sLine += *pos;
        }
        pos++;
    }

    D(debug("-- found %ld lines\n", m_requestArray.Count()));

    /* parse the HTTP request. */
    wxStringTokenizer tokens((m_requestArray[0] + wxT(" ")), wxT(" "));

    m_method    = tokens.GetNextToken();
    m_url       = tokens.GetNextToken();
    m_reqver    = tokens.GetNextToken();

    if ((nPos = m_url.Find('?')) != wxNOT_FOUND) {
        sQuery  = m_url.Mid(nPos + 1);
        m_url   = m_url.Mid(0, nPos);

        D(debug("-- query string = %s\n", sQuery.c_str()));
    }

    if (m_url == wxT("/"))
        m_url = wxT("/index.html");

    m_Request.m_headers.clear();

    for (size_t hdr_line = 1 ; hdr_line < m_requestArray.Count() - 1 ; hdr_line++)
    {
        wxString sHdrName, sHdrValue;
        int nPos = -1;

        sLine = m_requestArray[hdr_line];

        /* Find first ':' and parse header-name and value. */
        if ((nPos = sLine.Find(wxT(':'))) != wxNOT_FOUND) {
            sHdrName  = sLine.Mid(0, nPos);
            sHdrValue = sLine.Mid(nPos + 1).Trim(false);
            D(debug("\tHeader [%s] Value [%s]\n", sHdrName.c_str(),
                                                  sHdrValue.c_str()));
            m_Request.m_headers[sHdrName] = sHdrValue;    // Store in the hash.
        }
    }

    return;
}

void myHTTPdThread::Clear()
{
    D(debug("myHTTPdThread::Clear()\n"));

    m_requestArray.Clear();
    m_Request.m_cookies.Clear();
    m_Request.m_headers.clear();
    m_Request.m_queries.Clear();

    return;
}
/**
 *
 */

void myHTTPdThread::handle_connection(wxSocketBase* pSocket)
{
    bool            bDone = false;
    wxSockAddress   *localInfo = 0L,
                    *peerInfo = 0L;

    D(debug("myHTTPdThread::handle_connection(%p)\n", pSocket));

    /* Determine connection peer */
    localInfo = new wxIPV4address;
    peerInfo  = new wxIPV4address;

    if ( pSocket->GetLocal(*localInfo) ) {
        m_sLocalPort.Printf(wxT("%d"), ((wxIPV4address*)localInfo)->Service());
        m_sLocalHost = ((wxIPV4address*)localInfo)->Hostname();
    } else {
        m_sLocalPort = wxEmptyString;
        m_sLocalHost = wxEmptyString;
    }

    if ( pSocket->GetPeer(*peerInfo) ) {
        m_sPeerPort.Printf(wxT("%d"), ((wxIPV4address*)peerInfo)->Service());
        m_sPeerHost = ((wxIPV4address*)peerInfo)->Hostname();
    } else {
        m_sPeerPort = wxEmptyString;
        m_sPeerHost = wxEmptyString;
    }

    D(debug("Connection received from %s:%s\n", m_sPeerHost.c_str(), m_sPeerPort.c_str()));

    memset(m_buf, 0, m_bufSize);

    while (!bDone && !TestDestroy()) {
        //char c;

        if (pSocket->WaitForRead(0, 1000)) {
            wxUint32 count;

            Clear();

            /* Get HTTP request */
            pSocket->Read(m_buf, m_bufSize);
            count = pSocket->LastCount();

            D(debug("-- received %d bytes from peer!\n", count));

#ifdef  DUMP_RAW_REQUEST
            D(debug("%s\n", m_buf));
#endif

            if (count > 0) {
                parse_buffer();

                D(debug("HTTP request [%s]\n", m_requestArray[0].c_str()));
                D(debug("method %s url %s version %s\n",
                        m_method.c_str(),
                        m_url.c_str(),
                        m_reqver.c_str()));

                if (m_method == wxT("GET")) {
                    handle_get_method(pSocket);
                } else {
                    ReturnError(pSocket, 400, (char*)"Bad Request");
                }
            } else {
                ReturnError(pSocket, 400, (char*)"Bad Request");
            }
            bDone = true;
        }

        ::wxMicroSleep( 5000 );
    }

    pSocket->Close();

    delete peerInfo;
    delete localInfo;

    return;
}

/**
 *  Handle the HTTP 'GET' request.
 */

void myHTTPdThread::handle_get_method(wxSocketBase* pSocket)
{
    serverPage* pPage = m_pParent->GetPage( m_url, &m_Request );

    if (pPage != 0L) {
        pPage->Send(pSocket);
    } else {
        D(debug("-- requested invalid page %s\n", m_url.c_str()));
        ReturnError(pSocket, 404, (char *)"Not Found");
    }

    return;
}

/**
 *
 */

void myHTTPdThread::ReturnError(wxSocketBase* pSocket, int code, char* description)
{
	char response[500];
    D(debug("myHTTPdThread::ReturnError(%p, %d, %s)\n", pSocket, code, description));

	sprintf( response, "HTTP/1.1 %d %s\r\nserver: myHTTPd-1.0.0\r\n"
                       "content-type: text/plain\r\n"
                       "content-length: %ld\r\n\r\n%s",
                       code, description, strlen(description), description );
	pSocket->Write(response, strlen(response));

	return;
}

/**
 *
 */

wxThread::ExitCode myHTTPdThread::Entry()
{
    bool                bDone = false;
    wxIPV4address       addr;

    D(debug("myHTTPdThread::Entry()\n"));

    addr.AnyAddress();
    addr.Service(m_portNum);

    m_sockServer = new wxSocketServer( addr, wxSOCKET_REUSEADDR );
    assert(m_sockServer != 0L);

    if (!m_sockServer->IsOk()) {
        D(debug("ERROR: Unable to create server socket!\n"));
        return (ExitCode)-10;
    }

    while (!bDone) {

        if (TestDestroy()) {
            D(debug("-- got destroy message!\n"));
            bDone = true;
            continue;
        }

        if (m_sockServer->WaitForAccept(0, 1000)) {
            wxSocketBase*       pSocket = m_sockServer->Accept(false);

            D(debug("-- connection is waiting!!!\n"));

            handle_connection(pSocket);

            delete pSocket;
        }

        ::wxMicroSleep( 5000 );
    }

    m_sockServer->Close();

    delete m_sockServer;
    m_sockServer = 0L;

    D(debug("-- server thread exiting!\n"));

    return 0;
}

/*----------------------------------------------------------------------------*/

/**
 *
 */

myHTTPd::myHTTPd(int portNum)
:   m_nPort(portNum),
    m_serverThread(0L),
    m_sLogFilename(wxT("/tmp/myHTTPd.log")),
    m_pLogFile(0L)
{
    //ctor
}

/**
 *
 */

myHTTPd::~myHTTPd()
{
    //dtor
}

void myHTTPd::SetLogFile(wxString sLogFilename)
{
    m_sLogFilename = sLogFilename;
    return;
}

/**
 *
 */

bool myHTTPd::Start()
{
    bool            bRes        = false;
    myHTTPdThread*  pNewThread  = 0L;

    D(debug("myHTTPD::Start()\n"));

    pNewThread = new myHTTPdThread(this, m_nPort);

    if (pNewThread) {
        if (pNewThread->Create() == wxTHREAD_NO_ERROR) {
            m_pLogFile = new wxFile( m_sLogFilename, wxFile::write );

            if (pNewThread->Run() == wxTHREAD_NO_ERROR) {
                m_serverThread = pNewThread;
                bRes = true;
            }
        }
    }

    return bRes;
}

/**
 *
 */

bool myHTTPd::Stop()
{
    D(debug("myHTTPD::Stop()\n"));

    if (m_serverThread) {
        m_serverThread->Delete();
        m_serverThread->Wait();

        delete m_serverThread;
        m_serverThread = 0L;

        /* close log file */
        m_pLogFile->Close();
        delete m_pLogFile;
        m_pLogFile = 0L;
    }

    return false;
}

/**
 *
 */

void myHTTPd::AddPage(serverPage& page)
{
    m_catalog.AddPage( page );
}

/**
 *
 */

serverPage* myHTTPd::GetPage(wxString sPageName, Request* pRequest)
{
    D(debug("myHTTPd::GetPage(%s)\n", sPageName.c_str()));
    return m_catalog.GetPage( sPageName, pRequest );
}
