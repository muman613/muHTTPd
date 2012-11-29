#ifndef LASTUSEDLIST_H
#define LASTUSEDLIST_H

#include <wx/wx.h>

class wxConfig;

class lastUsedList
{
    public:
        lastUsedList();
        virtual ~lastUsedList();

        bool                Save(wxConfig* pWxConfig);
        bool                Load(wxConfig* pWxConfig);

        void                Push(wxString sLastReport);
        wxString            Top();

#ifdef  _DEBUG
        void                Dump(FILE* fOut);
#endif

    protected:
    private:
        wxArrayString       m_lastUsedArray;
};

#endif // LASTUSEDLIST_H
