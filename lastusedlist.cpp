#include <wx/wx.h>
#include <wx/config.h>
#include <assert.h>
#include "lastusedlist.h"
#include "dbgutils.h"

lastUsedList::lastUsedList()
{
    //ctor
    m_lastUsedArray.Add(wxT(""));
    m_lastUsedArray.Add(wxT(""));
    m_lastUsedArray.Add(wxT(""));
}

lastUsedList::~lastUsedList()
{
    //dtor
}

bool lastUsedList::Save(wxConfig* pWxConfig)
{
    D(debug("lastUsedList::Save(%p)\n", pWxConfig));

    assert(pWxConfig != 0L);

    pWxConfig->SetPath(wxT("HISTORY"));
    pWxConfig->Write(wxT("last"),   m_lastUsedArray[0]);
    pWxConfig->Write(wxT("later"),  m_lastUsedArray[1]);
    pWxConfig->Write(wxT("latest"), m_lastUsedArray[2]);

    return true;
}

bool lastUsedList::Load(wxConfig* pWxConfig)
{
    wxString sTmp;

    D(debug("lastUsedList::Load(%p)\n", pWxConfig));

    assert(pWxConfig != 0L);

    pWxConfig->SetPath(wxT("HISTORY"));
    pWxConfig->Read(wxT("latest"),  &m_lastUsedArray[2], wxT(""));
    pWxConfig->Read(wxT("later"),   &m_lastUsedArray[1], wxT(""));
    pWxConfig->Read(wxT("last"),    &m_lastUsedArray[0], wxT(""));

    return true;
}

void lastUsedList::Push(wxString sLastReport)
{
    D(debug("lastUsedList::Push(%s)\n", sLastReport.c_str()));

    m_lastUsedArray.Insert( sLastReport, 0 );
    if (m_lastUsedArray.size() > 3) {
        m_lastUsedArray.RemoveAt(3);
    }
}

wxString lastUsedList::Top()
{
    D(debug("lastUsedList::Top()\n"));

    if (m_lastUsedArray.size() > 0) {
        return m_lastUsedArray[0];
    } else
        return wxT("");
}

#ifdef  _DEBUG

void lastUsedList::Dump(FILE* fOut) {
    fprintf(fOut,"%ld items in array!\n", m_lastUsedArray.size());

    fprintf(fOut,"---------------------------------------------------------\n");
    fprintf(fOut, "Top    : %s\n", !m_lastUsedArray[0].IsEmpty()?m_lastUsedArray[0].c_str():wxT("EMPTY"));
    fprintf(fOut, "       : %s\n", !m_lastUsedArray[1].IsEmpty()?m_lastUsedArray[1].c_str():wxT("EMPTY"));
    fprintf(fOut, "Bottom : %s\n", !m_lastUsedArray[2].IsEmpty()?m_lastUsedArray[2].c_str():wxT("EMPTY"));
    fprintf(fOut,"---------------------------------------------------------\n");
    return;
}

#endif
