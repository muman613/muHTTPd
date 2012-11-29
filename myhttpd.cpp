#include <wx/wx.h>
#include <wx/tokenzr.h>
#include "myhttpd.h"
#include "dbgutils.h"

//#define DUMP_RAW_REQUEST          1



int myHTTPdThread::m_bufSize = 5000;

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
    wxString    sLine;

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
    wxStringTokenizer tokens(m_requestArray[0], wxT(" "));

    m_method    = tokens.GetNextToken();
    m_url       = tokens.GetNextToken();
    m_reqver    = tokens.GetNextToken();

    if (m_url == wxT("/"))
        m_url = wxT("/index.html");

    return;
}

/**
 *
 */

void myHTTPdThread::handle_connection(wxSocketBase* pSocket)
{
    bool            bDone = false;
    wxString        sLocalHost, sLocalPort;
    wxString        sPeerHost, sPeerPort;
    wxSockAddress   *localInfo = 0L,
                    *peerInfo = 0L;

    D(debug("myHTTPdThread::handle_connection(%p)\n", pSocket));

    /* Determine connection peer */
    localInfo = new wxIPV4address;
    peerInfo  = new wxIPV4address;

    if ( pSocket->GetLocal(*localInfo) ) {
        sLocalPort.Printf(wxT("%d"), ((wxIPV4address*)localInfo)->Service());
        sLocalHost = ((wxIPV4address*)localInfo)->Hostname();
    } else {
        sLocalPort = wxEmptyString;
        sLocalHost = wxEmptyString;
    }

    if ( pSocket->GetPeer(*peerInfo) ) {
        sPeerPort.Printf(wxT("%d"), ((wxIPV4address*)peerInfo)->Service());
        sPeerHost = ((wxIPV4address*)peerInfo)->Hostname();
    } else {
        sPeerPort = wxEmptyString;
        sPeerHost = wxEmptyString;
    }

    D(debug("Connection received from %s:%s\n", sPeerHost.c_str(), sPeerPort.c_str()));

    memset(m_buf, 0, m_bufSize);

    while (!bDone && !TestDestroy()) {
        //char c;

        if (pSocket->WaitForRead(0, 1000)) {
            wxUint32 count;

            /* Get HTTP request */
            pSocket->Read(m_buf, m_bufSize);
            count = pSocket->LastCount();

            D(debug("-- received %d bytes from peer!\n", count));

#ifdef  DUMP_RAW_REQUEST
            D(debug("%s\n", m_buf));
#endif

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
 *
 */

void myHTTPdThread::handle_get_method(wxSocketBase* pSocket)
{
    serverPage* pPage = m_pParent->GetPage( m_url );

    if (pPage != 0L) {
        pPage->Send(pSocket);
    } else {
        ReturnError(pSocket, 404, (char *)"Not Found");
    }
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
    m_serverThread(0L)
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

serverPage* myHTTPd::GetPage(wxString sPageName)
{
    D(debug("myHTTPd::GetPage(%s)\n", sPageName.c_str()));
    return m_catalog.GetPage( sPageName );
}
