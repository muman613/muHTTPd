/**
 *  @file       main.cpp
 *  @author     Michael A. Uman
 *  @date       December 28, 2012
 *  @brief      This module contains code to test the CSS classes.
 */

#include <wx/wx.h>
#include <wx/socket.h>
#include <wx/config.h>
#include <stdio.h>
#include <signal.h>
#include "dbgutils.h"
#include "mystylesheet.h"

bool bDone = false;

void dumpArray(wxArrayString& sArray) {
    for (size_t i = 0 ; i < sArray.Count() ; i++) {
        printf("%s\n", sArray[i].c_str());
    }

    return;
}

void old_test_code()
{
    myStyleAttribute        attr(wxT("background"), wxT("blue"));

    printf("attr text [%s]\n", (*attr).c_str() );

//    myStyleElement          elem(wxT("div"), wxT("special"));
//    myStyleElement          elem(wxT("div"));
    myStyleElement          elem(wxEmptyString, wxT("special"));
//    myStyleElement          elem(wxEmptyString, wxEmptyString, wxT("content"));

    elem.SetComment( wxT("special class") );

//    elem += myStyleAttribute( wxT("background"), wxT("blue") );
    elem += attr;
    elem += myStyleAttribute( wxT("color"), wxT("white") );
    elem += myStyleAttribute( wxT("width"), wxT("100%") );

    wxArrayString   cssText;

    elem.GetCSS( cssText );

    printf("elem :\n");

//    for (size_t i = 0 ; i < cssText.Count() ; i++) {
//        printf("%s\n", cssText[i].c_str());
//    }

    dumpArray( cssText );

    return;
}

void test_style_sheet() {
    myStyleSheet        sheet;
    myStyleElement      elem;

    elem = myStyleElement(wxT("div"), wxEmptyString, wxT("sidebar"));

    elem.AddAttribute(wxT("background"), wxT("#efefef"));
    elem.AddAttribute(wxT("color"),      wxT("#222222"));

    sheet.AddStyle( elem );

    elem = myStyleElement(wxT("div"), wxEmptyString, wxT("content"));

    elem.AddAttribute(wxT("background"), wxT("#000080"));
    elem.AddAttribute(wxT("color"),      wxT("#ffffff"));

    sheet.AddStyle( elem );

    sheet.DumpToFile();

    return;
}

/**
 *
 */

int main() {
    wxInitializer       wxInit;

    wxSocketBase::Initialize();

    test_style_sheet();
    //elem += myStyleAttribute(wxT("color"), wxT("white"));

	return 0L;
}
