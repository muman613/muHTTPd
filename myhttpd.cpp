/**
 *  @file       myhttpd.cpp
 *  @author     Michael A. Uman
 *  @date       December 2, 2012
 */

//#define OLDWAY  1

#include <wx/wx.h>
#include <wx/tokenzr.h>
#include <wx/uri.h>


#include "myhttpd.h"
#include "dbgutils.h"

//#define DUMP_RAW_REQUEST          1

/**
 *  Find a cookie by name.
 */

wxString Request::FindCookie(wxString sName) {
    wxString sResult = wxEmptyString;

    for (size_t x = 0 ; x < m_cookies.Count() ; x++) {
        if (m_cookies[x].name() == sName) {
            sResult = m_cookies[x].value();
            break;
        }
    }

    return sResult;
}

/**
 *  Find a request header by name.
 */

wxString Request::FindHeader(wxString sHeader) {
    wxString sResult = wxEmptyString;
    HEADER_MAP::iterator it;

    for (it = m_headers.begin() ; it != m_headers.end() ; it++) {
        if (it->first == sHeader) {
            sResult = it->second;
            break;
        }
    }

    return sResult;
}

/**
 *  Find a query value by name.
 */

wxString Request::FindQuery(wxString sName) {
    wxString sResult = wxEmptyString;

    for (size_t i = 0 ; i < m_queries.Count() ; i++) {
        if (m_queries[i].m_sId == sName) {
            sResult = m_queries[i].m_sValue;
            break;
        }
    }

    return sResult;
}

/**
 *  Return the referring URL.
 */

wxString Request::Referer()
{
    return FindHeader( wxT("Referer") );
}

/*----------------------------------------------------------------------------*/

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
    Clear();
}

/**
 *  Break request buffer up into seperate lines stored in wxArrayString.
 */

void myHTTPdThread::parse_request()
{
//    char*       pos;
    wxString    sLine, sQuery;
    int         nPos;

    D(debug("myHTTPdThread::parse_request()\n"));

    m_method.Clear();
    m_url.Clear();
    m_reqver.Clear();

#ifdef OLDWAY

    m_requestArray.Clear();

    char*       pos;

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
#endif

    D(debug("-- found %ld lines\n", m_requestArray.Count()));

    /* parse the HTTP request. */
    wxStringTokenizer tokens(m_requestArray[0], wxT(" "));

    m_method    = tokens.GetNextToken();
    m_url       = tokens.GetNextToken();
    m_reqver    = tokens.GetNextToken();

    /* find all '+' in url and replace with 'space' */
    m_url.Replace( wxT("+"), wxT(" ") );

    wxURI   newUri(m_url);

    m_url = newUri.BuildUnescapedURI();

    if ((nPos = m_url.Find('?')) != wxNOT_FOUND) {
        sQuery  = m_url.Mid(nPos + 1);
        m_url   = m_url.Mid(0, nPos);

        D(debug("-- query string = %s\n", sQuery.c_str()));

        wxStringTokenizer qryToke( sQuery, wxT("&") );

        while (qryToke.HasMoreTokens()) {
            wxString sPair = qryToke.GetNextToken();
            wxString sID, sVal;

            D(debug("found token %s\n", sPair.c_str()));

            if ((nPos = sPair.Find('=')) != wxNOT_FOUND) {
                sID  = sPair.Mid(0, nPos);
                sVal = sPair.Mid(nPos + 1);

                D(debug("query id %s val %s\n", sID.c_str(), sVal.c_str()));
                myQuery newQuery( sID, sVal );

                m_Request.m_queries.Add( newQuery );
            }
        }
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

            if (sHdrName.CmpNoCase(wxT("Cookie")) != 0) {
                D(debug("\tHeader [%s] Value [%s]\n", sHdrName.c_str(),
                                                      sHdrValue.c_str()));
                m_Request.m_headers[sHdrName] = sHdrValue;    // Store in the hash.
            } else {
                wxStringTokenizer ckeToke(sHdrValue, wxT("=;"));
                wxString sCookieID, sCookieVal;

                D(debug("-- handing cookie header!\n"));
                while (ckeToke.HasMoreTokens()) {
                    sCookieID = ckeToke.GetNextToken().Trim(false);
                    sCookieVal = ckeToke.GetNextToken().Trim(false);

                    D(debug("cookie id [%s] value [%s]\n", sCookieID.c_str(), sCookieVal.c_str()));

                    /* Add the cookie to the request cookie-array */
                    myCookie newCookie( sCookieID, sCookieVal );

                    m_Request.m_cookies.Add(newCookie);
                }
            }
        }
    }

    return;
}

/**
 *
 */

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

bool myHTTPdThread::receive_request(wxSocketBase* pSocket) {
    bool bRes = false;
    bool bDone = false;
    wxString sLine;

    D(debug("myHTTPdThread::receive_request(%p)\n", pSocket));

    m_requestArray.Clear();

    while (!bDone && pSocket->IsData()) {
        char ch;

        pSocket->Read(&ch, sizeof(ch));
//        D(debug("%c", ch));
        if (ch != '\n') {
            sLine += ch;
        } else {
            sLine = sLine.Trim(true);
            if (sLine.IsEmpty()) {
                bDone = true;
            } else {
//                D(debug("adding line [%s]\n", sLine.c_str()));
                m_requestArray.Add( sLine );
                sLine.Clear();
            }
        }
    }

    if (pSocket->IsConnected()) {
        parse_request();
        bRes = true;
    } else {
        bRes = false;
    }

    return bRes;
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

            Clear();

#ifndef OLDWAY

            if (receive_request( pSocket )) {
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
            } else {
                bDone = true;
            }

#else
            wxUint32 count;

            /* Get HTTP request */
            pSocket->Read(m_buf, m_bufSize);
            count = pSocket->LastCount();

            D(debug("-- received %d bytes from peer!\n", count));

#ifdef  DUMP_RAW_REQUEST
            D(debug("%s\n", m_buf));
#endif

            if (count > 0) {
                parse_request();

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
#endif

        }

        ::wxMicroSleep( 100 );
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

    D(debug("handle_get_method()\n"));

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
