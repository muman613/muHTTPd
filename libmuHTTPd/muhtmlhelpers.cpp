#include <wx/wx.h>
#include "muhtmlhelpers.h"

/**
 *  Helper functions in namespace HTML.
 */

static wxString generate_opt_string(HTML::HTMLOpts* opts) {
    wxString sOpts;

    if (opts != 0) {
        if (!opts->m_sID.IsEmpty())
            sOpts += wxT(" id=\"") + opts->m_sID + wxT("\"");
        if (!opts->m_sClass.IsEmpty())
            sOpts += wxT(" class=\"") + opts->m_sClass + wxT("\"");
        if (!opts->m_sStyle.IsEmpty())
            sOpts += wxT(" style=\"") + opts->m_sStyle + wxT("\"");
        if (!opts->m_sOnClick.IsEmpty())
            sOpts += wxT(" onclick=\"") + opts->m_sOnClick + wxT("\"");
    }

    return sOpts;
}

namespace HTML {
    void HTMLOpts::clear() {
        m_sID.Clear();
        m_sClass.Clear();
        m_sStyle.Clear();
        m_sOnClick.Clear();
    }

    wxString BOLD(wxString sText) {
        return wxT("<b>") + sText + wxT("</b>");
    }

    wxString ITALIC(wxString sText) {
        return wxT("<i>") + sText + wxT("</i>");
    }

    wxString HEADING1(wxString sText) {
        return wxT("<h1>") + sText + wxT("</h1>");
    }

    wxString HEADING2(wxString sText) {
        return wxT("<h2>") + sText + wxT("</h2>");
    }

    wxString HEADING3(wxString sText) {
        return wxT("<h3>") + sText + wxT("</h3>");
    }

    wxString CENTER(wxString sText) {
        return wxT("<center>") + sText + wxT("</center>");
    }

    wxString LINK(wxString sText, wxString sURL, HTMLOpts* opts) {
        wxString sOpts = generate_opt_string(opts);
        return wxT("<a") + sOpts + wxT(" href=\"") + sURL + wxT("\">") + sText + wxT("</a>");
    }

    wxString IMAGE(wxString sSrc, wxString sAlt, int width, int height) {
        wxString sHTML;

        sHTML = wxT("<img src =\"") + sSrc + wxT("\"");
        if (!sAlt.IsEmpty()) {
            sHTML += wxT(" alt=\"") + sAlt + wxT("\"");
        }
        if (width > 0) {
            sHTML += wxT(" width=\"") + wxString::Format(wxT("%d"), width) + wxT("\"");
        }
        if (height > 0) {
            sHTML += wxT(" height=\"") + wxString::Format(wxT("%d"), height) + wxT("\"");
        }
        sHTML += wxT(">");

        return sHTML;
    }

    wxString BR() {
        return wxT("<br />");
    }

    wxString HR() {
        return wxT("<hr />");
    }

    wxString P(wxString sPText, wxString sClass, wxString sId) {
        wxString sText;

        sText = wxT("<p");
        if (!sClass.IsEmpty()) {
            sText += wxT(" class=\"") + sClass + wxT("\"");
        }
        if (!sId.IsEmpty()) {
            sText += wxT(" id=\"") + sId + wxT("\"");
        }
        sText += sPText;
        sText += wxT("</p>");

        return sText;
    }

    wxString SELECT(wxString sName, const wxArrayString& sOptions, wxString sDefault) {
        wxString sResult;

        sResult = wxT("<select name=\"") + sName + wxT("\">");
        for (size_t i = 0 ; i < sOptions.Count() ; i++) {
            if (!sDefault.IsEmpty() && (sDefault == sOptions[i])) {
                sResult += wxT("<option selected>") + sOptions[i] + wxT("</option>");
            } else {
                sResult += wxT("<option>") + sOptions[i] + wxT("</option>");
            }
        }
        sResult += wxT("</select>");

        return sResult;
    }

    wxString FILEBOX(wxString sFilename, wxString sCaption) {
        wxString        sHTML;
        wxTextFile      file( sFilename );

        if (file.Open()) {
//            sHTML += wxT("<table style=\"background-color: #DDDDDD; border-style: double; width: 80%; margin-left: 10%; margin-right: 10%;\">\n");
            sHTML += wxT("<table style=\"background-color: #DDDDDD; border-style: double; width: 100%; \">\n");
            if (!sCaption.IsEmpty()) {
                sHTML += wxT("<caption><h3>") + sCaption + wxT("</h3></caption>\n");
            }
            sHTML += wxT("<tr><td><pre>\n");
            for (size_t x = 0 ; x < file.GetLineCount() ; x++) {
                wxString sTmp = file[x];
                sTmp.Replace(wxT("<"), wxT("&lt;"));
                sTmp.Replace(wxT(">"), wxT("&gt;"));
                sTmp.Replace(wxT("\t"), wxT("&nbsp;&nbsp;&nbsp;&nbsp;"));
                sHTML += sTmp + wxT("\n");
            }
            sHTML += wxT("</pre></td></tr>\n");
            sHTML += wxT("</table>\n");
        }

        return sHTML;
    }

    wxString DIV(wxString sDIVText, wxString sClass, wxString sId) {
        wxString sText;

        sText = wxT("<div");
        if (!sClass.IsEmpty()) {
            sText += wxT(" class=\"") + sClass + wxT("\"");
        }
        if (!sId.IsEmpty()) {
            sText += wxT(" id=\"") + sId + wxT("\"");
        }
        sText += sDIVText;
        sText += wxT("</div>");

        return sText;
    }

};  // end namespace

