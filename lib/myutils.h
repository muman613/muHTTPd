#ifndef __MYUTILS_H__
#define __MYUTILS_H__

#include <wx/wx.h>

typedef enum _typeCategory {
    CATEGORY_UNKNOWN = 0,
    CATEGORY_IMAGE,
    CATEGORY_TEXT,
    CATEGORY_BINARY,
} TYPE_CATEGORY;

wxString    GetMimeFromExtenstion(wxString sExtension);
wxString    GetExtensionFromMime(wxString sMime);
bool        TestMimeType(wxString sType, TYPE_CATEGORY category);


#endif // __MYUTILS_H__

