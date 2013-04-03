#ifndef	__MUHTTPDEMOAPP_H__
#define	__MUHTTPDEMOAPP_H__

#include <wx/wx.h>

/**
 *
 */

class muHTTPdemoApp : public wxAppConsole {
public:
	bool OnInit();
	int OnRun();
};

DECLARE_APP(muHTTPdemoApp)

#endif

