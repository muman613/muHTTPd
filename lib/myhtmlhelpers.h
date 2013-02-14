#ifndef __MYHTMLHELPERS_H__
#define __MYHTMLHELPERS_H__

#include <wx/wx.h>
#include <wx/textfile.h>

/**
 *  Helper functions for HTML formatting.
 */

namespace HTML {
    class HTMLOpts {
    public:
        wxString    m_sID;
        wxString    m_sClass;
        wxString    m_sStyle;

        /* events */
        wxString    m_sOnClick;

        void clear();
    };

    wxString BOLD(wxString sText);
    wxString ITALIC(wxString sText);
    wxString HEADING1(wxString sText);
    wxString HEADING2(wxString sText);
    wxString HEADING3(wxString sText);
    wxString CENTER(wxString sText);
    wxString BR();
    wxString HR();
    wxString LINK(wxString sText, wxString sURL, HTMLOpts* opts = 0L);
    wxString IMAGE(wxString sSrc, wxString sAlt = wxEmptyString,
                   int width = 0, int height = 0);
    wxString SELECT(wxString sName,
                    const wxArrayString& sOptions,
                    wxString sDefault = wxEmptyString);
    wxString FILEBOX(wxString sFilename,
                     wxString sCaption = wxEmptyString);
    wxString P(wxString sPText,
               wxString sClass = wxEmptyString,
               wxString sId    = wxEmptyString);
    wxString DIV(wxString sDIVText,
                 wxString sClass = wxEmptyString,
                 wxString sId    = wxEmptyString);
};

#endif // __MYHTMLHELPERS_H__
