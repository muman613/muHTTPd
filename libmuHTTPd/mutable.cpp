#include <wx/wx.h>
#include "mutable.h"
#include "dbgutils.h"

//#include <wx/arrimpl.cpp>
//WX_DEFINE_OBJARRAY( ArrayOfRows );
//WX_DEFINE_OBJARRAY( ArrayOfCells );

myTable::myTable(int rows, int cols)
:   m_rows(rows),
    m_cols(cols),
    m_border(0)
{
    //ctor
    D(debug("myTable::myTable(%d, %d)\n", rows, cols));

    m_cells = new myCell[rows * cols];
    m_rowClass = new wxString[rows];

}

myTable::myTable(const myTable& copy)
{
    //ctor
}

myTable::~myTable()
{
    //dtor
    delete [] m_cells;
    delete [] m_rowClass;
}

void myTable::set_border(int border)
{
    m_border = border;
}

myCell& myTable::cell(int row, int col)
{
    return m_cells[(row * m_rows) + col];
}

/**
 *
 */

wxString myTable::HTML()
{
    wxString sHTML;

    if (m_border > 0) {
        sHTML += wxT("<table border=\"") + wxString::Format(wxT("%d"), m_border) + wxT("\">\n");
    } else {
        sHTML += wxT("<table>\n");
    }

    for (int row = 0 ; row < m_rows ; row++) {
        if (m_rowClass[row].IsEmpty()) {
            sHTML += wxT("<tr>\n");
        } else {
            sHTML += wxT("<tr class=\"") + m_rowClass[row] + wxT("\">\n");
        }
        for (int col = 0 ; col < m_cols ; col++) {
            myCell&     thisCell = cell( row, col );
            if (thisCell.m_sClass.IsEmpty()) {
                sHTML += wxT("<td>");
            } else {
                sHTML += wxT("<td class=\"") + thisCell.m_sClass + wxT("\">");
            }
            sHTML += thisCell.m_sText;
            sHTML += wxT("</td>");
        }
        sHTML += wxT("\n</tr>\n");
    }
    sHTML += wxT("</table>\n");

    return sHTML;
}

void myTable::dump()
{
    for (int row = 0 ; row < m_rows ; row++) {
        for (int col = 0 ; col < m_cols ; col++) {
            myCell&     thisCell = cell(row, col);

            printf("row %d col %d : %s\n", row, col, thisCell.m_sText.c_str());
        }
    }
}

void myTable::set_row_class(int row, wxString sClass)
{
    m_rowClass[row] = sClass;
}

//myRow::myRow(int cols) {
//
//}
//
//myRow::myRow(const myRow& copy)
//{
//
//}
//
//myRow::~myRow()
//{
//
//}

myCell::myCell()
{
    // ctor
}

myCell::myCell(wxString sText, wxString sClass, wxString sStyle)
:   m_sText(sText),
    m_sClass(sClass),
    m_sStyle(sStyle)
{
    // ctor
}

myCell::myCell(const myCell& copy)
:   m_sText(copy.m_sText),
    m_sClass(copy.m_sClass),
    m_sStyle(copy.m_sStyle)
{
    // ctor
}

myCell::~myCell()
{
    // dtor
}

myCell& myCell::operator =(const myCell& copy)
{
    m_sText     = copy.m_sText;
    m_sClass    = copy.m_sClass;
    m_sStyle    = copy.m_sStyle;

    return *this;
}

myCell& myCell::Class(wxString sClass) {
    m_sClass = sClass;
    return *this;
}

myCell& myCell::Style(wxString sStyle) {
    m_sStyle = sStyle;
    return *this;
}

myCell& myCell::Text(wxString sText) {
    m_sText = sText;
    return *this;
}
