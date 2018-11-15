#include <wx/wx.h>
#include <wx/filename.h>
#include "muutils.h"
#include "dbgutils.h"


typedef struct _mimeLookupEntry {
    wxString*       extension;
    wxString*       mimetype;
    TYPE_CATEGORY   category;
} MIME_TABLE_ENTRY;

/* Image file extensions */
static wxString     sExtJpeg        = wxT("jpg");
static wxString     sExtPng         = wxT("png");
static wxString     sExtGif         = wxT("gif");
static wxString     sExtIco         = wxT("ico");

/* Text file extensions */
static wxString     sExtText        = wxT("txt");
static wxString     sExtHtml        = wxT("html");
static wxString     sExtCss         = wxT("css");
static wxString     sExtJava        = wxT("js");
static wxString     sExtXml         = wxT("xml");

/* Image MIME types */
static wxString     sMimeJpeg       = wxT("image/jpeg");
static wxString     sMimePng        = wxT("image/png");
static wxString     sMimeGif        = wxT("image/gif");
static wxString     sMimeIco        = wxT("image/vnd.microsoft.icon");

/* Text MIME types */
static wxString     sMimeText       = wxT("text/plain");
static wxString     sMimeHtml       = wxT("text/html");
static wxString     sMimeCss        = wxT("text/css");
static wxString     sMimeJava       = wxT("text/javascript"); // obsolete?
static wxString     sMimeXml        = wxT("text/xml");

MIME_TABLE_ENTRY mimeTable[] = {

    /* image types */

    { &sExtJpeg, &sMimeJpeg, CATEGORY_IMAGE, },
    { &sExtPng , &sMimePng,  CATEGORY_IMAGE, },
    { &sExtGif,  &sMimeGif,  CATEGORY_IMAGE, },
    { &sExtIco,  &sMimeIco,  CATEGORY_IMAGE, },

    /* text types */

    { &sExtText, &sMimeText, CATEGORY_TEXT, },
    { &sExtHtml, &sMimeHtml, CATEGORY_TEXT, },
    { &sExtCss,  &sMimeCss,  CATEGORY_TEXT, },
    { &sExtJava, &sMimeJava, CATEGORY_TEXT, },
    { &sExtXml,  &sMimeXml,  CATEGORY_TEXT, },

    { 0L, 0L, CATEGORY_UNKNOWN, },
};


/**
 *
 */

wxString GetMimeFromExtenstion(wxString sExtension)
{
    wxString sMimeType = sMimeText;

    D(debug(wxT("GetMimeFromExtension(%s)\n"), static_cast<const char *>(sExtension)));

    MIME_TABLE_ENTRY*       pEntry = mimeTable;

    while (pEntry->extension != 0) {
        if (sExtension.CmpNoCase( *pEntry->extension ) == 0) {
            D(debug(wxT("-- found extension... Type is %s\n"), static_cast<const char *>(*pEntry->mimetype)));
            sMimeType = *pEntry->mimetype;
            break;
        }
        pEntry++;
    }

    return sMimeType;
}

/**
 *
 */

bool        TestMimeType(wxString sType, TYPE_CATEGORY category)
{
    bool                bRes    = false;
    MIME_TABLE_ENTRY*   pEntry  = mimeTable;

    D(debug(wxT("TestMimeType(%s, %d)\n"), static_cast<const char *>(sType), (int) category));

    while (pEntry->extension != 0) {
        if (sType.CmpNoCase( *pEntry->mimetype ) == 0) {
            bRes = (category == pEntry->category)?true:false;
            break;
        }
        pEntry++;
    }

    return bRes;
}

/**
 *  Return just file filename without the complete path...
 */

wxString GetFilePart(wxString sFullName) {
    wxFileName  fname(sFullName);

    return fname.GetFullName();
}
