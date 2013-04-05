#ifndef MYTABLE_H
#define MYTABLE_H

#include <wx/wx.h>

class myTable;
class myCell;

class myCell
{
    public:
        myCell();
        myCell(wxString sText,
               wxString sClass = wxEmptyString,
               wxString sStyle = wxEmptyString);
        myCell(const myCell& copy);
        virtual ~myCell();

        myCell& operator = (const myCell& copy);

        wxString    HTML();

        myCell&     Class(wxString sClass);
        myCell&     Style(wxString sStyle);
        myCell&     Text(wxString sText);

    protected:
        friend class myTable;

        wxString    m_sText;
        wxString    m_sClass;
        wxString    m_sStyle;
};

/**
 *
 */

class myTable
{
    public:
        myTable(int rows, int cols);
        myTable(const myTable& copy);
        virtual ~myTable();

        wxString    HTML();

        void        set_caption(wxString sCaption);

        void        set_row_class(int row, wxString sClass);

        void        set_border(int border);
        myCell&     cell(int row, int col);

        void        dump();

    protected:
        int         m_rows;
        int         m_cols;
        int         m_border;
        myCell*     m_cells;
        wxString*   m_rowClass;
        wxString    m_caption;
    private:
};

#endif // MYTABLE_H
