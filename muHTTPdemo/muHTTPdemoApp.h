/**
 *  @file       muHTTPdemoApp.h
 *  @author     Michael A. Uman
 *  @date       April 4, 2013
 *  @brief      Example application using the muHTTPd library.
 */

#ifndef	__MUHTTPDEMOAPP_H__
#define	__MUHTTPDEMOAPP_H__

#include <wx/wx.h>

/**
 *
 */

class muHTTPdemoApp : public wxAppConsole {
public:
    muHTTPdemoApp();
    virtual ~muHTTPdemoApp();

	bool    OnInit();
	int     OnRun();

protected:
    void    create_data_dir();
    void    read_config_file();
    bool    parse_commandline();

    int     m_nPort;    ///< TCP/IP port to run server on.
};

DECLARE_APP(muHTTPdemoApp)

#endif

