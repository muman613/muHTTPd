#ifndef _WEBPAGES_H_
#define _WEBPAGES_H_

#include <wx/wx.h>
class serverPage;
class Request;
class muHTTPd;

wxString generate_table();
bool page3_stub(serverPage* pPage, Request* pRequest);
void add_serverpages(muHTTPd* pServer);

#endif // _WEBPAGES_H_
