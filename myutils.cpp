#include <wx/wx.h>
#include "myutils.h"
#include "dbgutils.h"

/**
 *
 */

wxString GetMimeFromExtenstion(wxString sExtension)
{
    wxString sMimeType;

    if (sExtension.CmpNoCase(wxT("jpg")) == 0) {
        sMimeType = wxT("image/jpeg");
    } else if (sExtension.CmpNoCase(wxT("png")) == 0) {
        sMimeType = wxT("image/png");
    } else if (sExtension.CmpNoCase(wxT("gif")) == 0) {
        sMimeType = wxT("image/gif");
    }

    return sMimeType;
}
