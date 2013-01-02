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
#define ENABLE_DUMP               1

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

const myAttachment* Request::FindAttach(wxString sName) {
    D(debug("Request::FindAttach(%s)\n", sName.c_str()));

    for (size_t x = 0 ; x < m_attached.Count() ; x++) {
        if (m_attached[x]->name() == sName) {
            D(debug("-- attachment found!\n"));
            return m_attached[x];
        }
    }

    return (myAttachment*)0L;
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

    for (size_t hdr_line = 1 ; hdr_line < m_requestArray.Count() ; hdr_line++)
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

                D(debug("-- handling cookie header!\n"));
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

    for (size_t x = 0 ;x < m_Request.m_attached.Count() ; x++) {
        delete m_Request.m_attached[x];
    }
    m_Request.m_attached.Clear();

    return;
}

/**
 *
 */

bool myHTTPdThread::handle_attachment( wxString sData )
{
    myAttachment*   pNewAttachment;

    D(debug("handle_attachment()\n"));

    pNewAttachment = new myAttachment( sData );
    wxASSERT( pNewAttachment != 0L);

    m_Request.m_attached.Add( pNewAttachment );

    D(debug("--  attachment name [%s] of type [%s]\n",
            pNewAttachment->name().c_str(),
            pNewAttachment->type().c_str()));

    return true;
}

/**
 *
 */

bool myHTTPdThread::receive_request(wxSocketBase* pSocket) {
    bool bRes = false;
    bool bDone = false;
    wxString sLine;

    D(debug("myHTTPdThread::receive_request(%p)\n", pSocket));

    pSocket->SaveState();
    pSocket->SetFlags( wxSOCKET_NOWAIT );
    m_requestArray.Clear();

    while (!bDone && pSocket->IsData()) {
        char ch;

        pSocket->Read(&ch, sizeof(ch));

        if (pSocket->LastCount() == 1) {
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
        } else {
            D(debug("-- NO DATA!\n"));
            bDone = true;
            goto exitReceive;
        }
    }

    if (pSocket->IsConnected()) {
        parse_request();

        if (m_method == wxT("POST")) {
//            char ch;
            wxString    sCTHeader,
                        sContentType,
                        sBoundry,
                        sBoundryEnd;
            wxString sTmp;
            wxString    sLine, sData, sBuffer;

            sCTHeader = m_Request.FindHeader( wxT("Content-Type") );
            //D(debug("sCTHeader %s\n", sCTHeader.c_str()));

            if (!sCTHeader.IsEmpty()) {
                wxStringTokenizer   ctToke( sCTHeader, wxT(";") );

                sContentType    = ctToke.GetNextToken();
                //D(debug("-- found content type = %s\n", sContentType.c_str()));

                if (sContentType == wxT("multipart/form-data")) {
                    wxString sBoundString = ctToke.GetNextToken();
                    //D(debug("sBoundString = %s\n", sBoundString.c_str()));

                    wxStringTokenizer bToke( sBoundString, wxT("=") );
                    bToke.GetNextToken();
                    sTmp            = bToke.GetNextToken();
                    sBoundry        = wxT("--") + sTmp + wxT("\r\n");
                    sBoundryEnd     = wxT("--") + sTmp + wxT("--") + wxT("\r\n");
                }
            }

            D(debug("-- reading POST data...\n"));

            char tBuffer[1024];
            wxUint32 tBread = 0;

            while (pSocket->WaitForRead(4)) {
                wxUint32 nBread = 0;

                memset(tBuffer, 0, 1024);
                pSocket->Read(tBuffer, 1024);
                nBread = pSocket->LastCount();

                //D(debug("-- got %ld bytes!\n", nBread));
                sBuffer += wxString::From8BitData(tBuffer, nBread);
                tBread += nBread;
            }

            D(debug("--- tBread %d\n", tBread));
            D(debug("--- SBUFFER SIZE %d\n", sBuffer.Length()));

            size_t          beginPos = -1, endPos = -1, curPos = 0;
            wxString*       pSearchFor = &sBoundry;
            bDone = false;

            while (!bDone) {
                //D(debug("curPos %d\n", curPos));

                curPos = sBuffer.find( *pSearchFor, curPos );
                //D(debug("result curPos %d\n", curPos));

                if (curPos == wxString::npos) {
                    //D(debug("-- hit end of string!\n"));

                    if (pSearchFor == &sBoundry) {
                        //D(debug("-- searching for end boundry!\n"));
                        pSearchFor = &sBoundryEnd;
                        curPos = 0;
                        continue;
                    } else {
                        bDone = true;
                        continue;
                    }
                }

                if (beginPos == (size_t)-1) {
//                    D(debug("-- must be a begin position @ %d\n", curPos));
                    beginPos = curPos + pSearchFor->Length();
                    curPos += pSearchFor->Length();
                    continue;
                }
                if (endPos == (size_t)-1) {
//                    D(debug("-- must be an end position @ %d\n", curPos));
                    endPos = curPos;
                    curPos++;
                }
                wxString sData = sBuffer.Mid( beginPos, endPos - beginPos -2 );

                D(debug("Attachment size %d\n", sData.Length()));

                handle_attachment( sData );

//                D(debug("begin %d end %d\n", beginPos, endPos));
//                D(debug("data : [%s]\n", sData.c_str()));

                beginPos = endPos + pSearchFor->Length();

                endPos = -1;
            }

#ifdef  ENABLE_DUMP
            wxFile fDump; //(wxT("/tmp/dump.bin"));

            if (fDump.Open(wxT("/tmp/dump.bin"), wxFile::write)) {
                fDump.Write( sBuffer.GetData(), sBuffer.Length() );
                fDump.Close();
            }
#endif

            D(debug("\nOK!\n"));
        }

        bRes = true;
    } else {
        bRes = false;
    }

exitReceive:

    pSocket->RestoreState();

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

            if (receive_request( pSocket )) {
                D(debug("HTTP request [%s]\n", m_requestArray[0].c_str()));
                D(debug("method %s url %s version %s\n",
                        m_method.c_str(),
                        m_url.c_str(),
                        m_reqver.c_str()));

                m_pParent->LogMessage( myHTTPd::LOG_MSG,
                                       wxString::Format(wxT("%s %s %s from %s.%s"),
                                                        m_method.c_str(),
                                                        m_url.c_str(),
                                                        m_reqver.c_str(),
                                                        m_sPeerHost.c_str(),
                                                        m_sPeerPort.c_str()) );

                if (m_method == wxT("GET")) {
                    handle_get_method(pSocket);
                } else if (m_method == wxT("POST")) {
                    handle_post_method(pSocket);
                } else {
                    ReturnError(pSocket, 501, (char*)"Not Implemented");
                }
                bDone = true;
            } else {
                bDone = true;
            }
        }

        ::wxMicroSleep( 100 );
    }

//    pSocket->Close();

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

        m_pParent->LogMessage( myHTTPd::LOG_WARN,
                               wxString::Format(wxT("Request For Invalid Page %s"),
                                                m_url.c_str()) );
    }

    return;
}
/**
 *  Handle the HTTP 'POST' request.
 */

void myHTTPdThread::handle_post_method(wxSocketBase* pSocket)
{
    serverPage* pPage = m_pParent->GetPage( m_url, &m_Request );

    D(debug("handle_post_method()\n"));

    if (pPage != 0L) {
        pPage->Send(pSocket);
    } else {
        D(debug("-- requested invalid page %s\n", m_url.c_str()));
        ReturnError(pSocket, 404, (char *)"Not Found");

        m_pParent->LogMessage( myHTTPd::LOG_WARN,
                               wxString::Format(wxT("Request For Invalid Page %s"),
                                                m_url.c_str()) );
    }

    return;
}

/**
 *
 */

void myHTTPdThread::ReturnError(wxSocketBase* pSocket, int code, char* description)
{
    wxString sResponseHeader, sResponseHTML;
    serverPage* p404Page = 0L;

    D(debug("myHTTPdThread::ReturnError(%p, %d, %s)\n", pSocket, code, description));

    if ((code == 404) && ((p404Page = m_pParent->Get404Page()) != 0L)) {
        sResponseHTML = p404Page->HTML();

        sResponseHeader += wxString::Format(wxT("HTTP/1.1 %d %s"), code, description) + sHTMLEol;
        sResponseHeader += wxT("content-type: text/html") + sHTMLEol;
        sResponseHeader += wxString::Format(wxT("content-length: %ld"), sResponseHTML.Length()) + sHTMLEol;
        sResponseHeader += sHTMLEol;

        pSocket->Write(sResponseHeader, sResponseHeader.Length());
        pSocket->Write(sResponseHTML, sResponseHTML.Length());
    } else {
        char response[500];

        sprintf( response, "HTTP/1.1 %d %s\r\nserver: myHTTPd-1.0.0\r\n"
                           "content-type: text/plain\r\n"
                           "content-length: %ld\r\n\r\n%s",
                           code, description, strlen(description), description );
        pSocket->Write(response, strlen(response));
        D(debug("404 response string = %s\n", response));
    }

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

            pSocket->Close();
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
    m_pLogFile(0L),
    m_p404Page(0L)
{
    //ctor
}

/**
 *  myHTTPd destructor.
 */

myHTTPd::~myHTTPd()
{
    /* If a 404 page was allocated, delete it... */
    if (m_p404Page != 0L) {
        delete m_p404Page;
        m_p404Page = 0L;
    }

    CloseLogFile();
}


/**
 *  Set the port # for the HTTP server to use.
 *
 *  If the server is already running this function returns false.
 */

bool myHTTPd::SetPort(int portNum)
{
    bool bRes = false;

    D(debug("myHTTPd::SetPort(%d)\n", portNum));

    if (m_serverThread != 0L) {
        m_nPort = portNum;
        bRes = true;
    }

    return bRes;
}

/**
 *  Close the log file.
 */

void myHTTPd::CloseLogFile() {
    D(debug("myHTTPd::CloseLogFile()\n"));

    if (m_pLogFile) {
        if (m_pLogFile->IsOpened()) {
            m_pLogFile->Close();
            delete m_pLogFile;
            m_pLogFile = 0L;
        }
    }

    return;
}

/**
 *  Open the log file.
 *
 *  @param sLogFilename     Name of file to open as log file.
 *  @param bAppend          true if log messages should append to existing log.
 */

bool myHTTPd::SetLogFile(wxString sLogFilename, bool bAppend)
{
    bool                bRes    = false;
    wxFile::OpenMode    logMode = (bAppend == true)?wxFile::write_append:
                                                    wxFile::write;

    D(debug("myHTTPd::SetLogFile(%s)\n", sLogFilename.c_str()));

    CloseLogFile(); // Close any existing log file.

    m_pLogFile = new wxFile( sLogFilename, logMode );

    if (m_pLogFile->IsOpened()) {
        D(debug("-- logfile opened and ready to write!\n"));

        m_sLogFilename = sLogFilename;
        bRes = true;
    } else {
        D(debug("-- unable to open log file!\n"));

        delete m_pLogFile;
        m_pLogFile = 0L;
    }

    return bRes;
}

/**
 *  Send a message to the log file.
 */

bool myHTTPd::LogMessage(logType nType, wxString sMsg)
{
    bool bRes = false;
    if (m_pLogFile != 0L) {
        wxDateTime      now = wxDateTime::Now();
        wxString        sFullMsg;

        switch (nType) {
        case LOG_MSG:
            sFullMsg = wxString::Format( wxT("MESSAGE %s : %s\n"),
                                        now.Format().c_str(),
                                        sMsg.c_str() );
            break;
        case LOG_WARN:
            sFullMsg = wxString::Format( wxT("WARNING %s : %s\n"),
                                        now.Format().c_str(),
                                        sMsg.c_str() );
            break;
        case LOG_ERROR:
            sFullMsg = wxString::Format( wxT("ERROR   %s : %s\n"),
                                        now.Format().c_str(),
                                        sMsg.c_str() );
            break;
        }

        m_pLogFile->Write(sFullMsg);
        m_pLogFile->Flush();

        bRes = true;
    }
    return bRes;
}

/**
 *  Start the HTTP server running.
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
                LogMessage(LOG_MSG, wxT("Server Started"));
                m_serverThread = pNewThread;
                bRes = true;
            }
        }
    }

    return bRes;
}

/**
 *  Stop the server thread.
 */

bool myHTTPd::Stop()
{
    D(debug("myHTTPD::Stop()\n"));

    if (m_serverThread != 0L) {
        m_serverThread->Delete();
        m_serverThread->Wait();

        delete m_serverThread;
        m_serverThread = 0L;

        LogMessage(LOG_MSG, "Server Stopped");
    }

    return false;
}

/**
 *  Add a HTML page to the page catalog.
 *
 *  Page name must be unique, otherwise this function returns false.
 */

bool myHTTPd::AddPage(serverPage& page)
{
    bool bRes = false;

    D(debug("myHTTPd::AddPage(name=%s)\n", page.GetPageName().c_str()));

    if ( !PageExists( page.GetPageName() ) ) {
        page.server(this);              // set the server pointer...
        m_catalog.AddPage( page );
        bRes = true;
    }

    return bRes;
}

/**
 *
 */

serverPage* myHTTPd::GetPage(wxString sPageName, Request* pRequest)
{
    D(debug("myHTTPd::GetPage(%s)\n", sPageName.c_str()));
    return m_catalog.GetPage( sPageName, pRequest );
}

/**
 *  Determine if a page with name 'sPageName' is in the catalog.
 */

bool myHTTPd::PageExists(wxString sPageName) {
    D(debug("myHTTPd::PageExists(%s)\n", sPageName.c_str()));
    return m_catalog.PageExists( sPageName );
}

/**
 *  Set the 404 page to the contents of 'page'.
 */

void myHTTPd::Set404Page(serverPage& page) {
    D(debug("myHTTPd::Set404Page(...)\n"));

    if (m_p404Page != 0L) {
        delete m_p404Page;
        m_p404Page = 0L;
    }

    m_p404Page = new serverPage(page);

    return;
}

/**
 *  Return pointer to users 404 page.
 */

serverPage* myHTTPd::Get404Page() {
    return m_p404Page;
}
