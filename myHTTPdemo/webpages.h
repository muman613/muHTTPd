#ifndef _WEBPAGES_H_
#define _WEBPAGES_H_

#include <wx/wx.h>
class serverPage;
class Request;
class myHTTPd;

wxString generate_table();
bool page3_stub(serverPage* pPage, Request* pRequest);
void add_serverpages(myHTTPd* pServer);

#endif // _WEBPAGES_H_
