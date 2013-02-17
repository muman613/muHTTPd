/**
 *  @file       mystylesheet.cpp
 *  @author     Michael A. Uman
 *  @date       December 26, 2012
 *  @brief      Module contains classes used to represent a CSS stylesheet.
 */

#include <wx/wx.h>
#include <wx/arrstr.h>
#include <wx/hashmap.h>
#include <wx/file.h>
#include "mystylesheet.h"
#include "dbgutils.h"

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( ArrayOfAttributes );
WX_DEFINE_OBJARRAY( ArrayOfElementStyles );

/**
 *
 */

myStyleAttribute::myStyleAttribute()
{
    // ctor
}

/**
 *
 */

myStyleAttribute::myStyleAttribute(const wxString& sAttributeName, const wxString& sAttributeValue)
:   m_sAttributeName(sAttributeName),
    m_sAttributeValue(sAttributeValue)
{
    // ctor
}

/**
 *
 */

myStyleAttribute::myStyleAttribute(const myStyleAttribute& copy)
:   m_sAttributeName(copy.m_sAttributeName),
    m_sAttributeValue(copy.m_sAttributeValue)
{
    // ctor
}

/**
 *
 */

myStyleAttribute::~myStyleAttribute()
{
    // dtor
}

myStyleAttribute& myStyleAttribute::operator =(const myStyleAttribute& copy)
{
    m_sAttributeName = copy.m_sAttributeName;
    m_sAttributeValue = copy.m_sAttributeValue;

    return *this;
}

wxString myStyleAttribute::AttributeName() const {
    return m_sAttributeName;
}

wxString myStyleAttribute::AttributeValue() const {
    return m_sAttributeValue;
}

/**
 *  Operator '*' used to return the actual style in 'name : value ;' form.
 */

wxString myStyleAttribute::operator *() const {
    wxString sAttributeText = wxString::Format(wxT("%s : %s ;"),
                                               m_sAttributeName.c_str(),
                                               m_sAttributeValue.c_str());
    return sAttributeText;
}

/*----------------------------------------------------------------------------*/

myStyleElement::myStyleElement(const wxString sStyleTag,
                               const wxString sStyleClass,
                               const wxString sStyleId)
:   m_sCSSTag(sStyleTag),
    m_sCSSClass(sStyleClass),
    m_sCSSId(sStyleId)
{
    // ctor
}


/**
 *  Copy constructor.
 */

myStyleElement::myStyleElement(const myStyleElement& copy)
:   m_sCSSTag(copy.m_sCSSTag),
    m_sCSSClass(copy.m_sCSSClass),
    m_sCSSId(copy.m_sCSSId),
    m_sCSSComment(copy.m_sCSSComment),
    m_attrArray(copy.m_attrArray)
{
    // ctor
}

/**
 *  Copy operator.
 */

myStyleElement::~myStyleElement()
{
    // dtor
}

/**
 *  Copy operator.
 */

myStyleElement& myStyleElement::operator =(const myStyleElement& copy)
{
    m_sCSSTag       = copy.m_sCSSTag;
    m_sCSSClass     = copy.m_sCSSClass;
    m_sCSSId        = copy.m_sCSSId;
    m_sCSSComment   = copy.m_sCSSComment;
    m_attrArray     = copy.m_attrArray;

    return *this;
}

/**
 *  Clear all fields of the element.
 */

void myStyleElement::Clear()
{
    m_sCSSTag.Clear();
    m_sCSSClass.Clear();
    m_sCSSId.Clear();
    m_sCSSComment.Clear();
    m_attrArray.Clear();

    return;
}

myStyleElement& myStyleElement::AddAttribute(const wxString& sAttName, const wxString& sAttVal)
{
    m_attrArray.Add( myStyleAttribute( sAttName, sAttVal ) );
    return *this;
}

myStyleElement& myStyleElement::operator +(myStyleAttribute newAtt)
{
    m_attrArray.Add( newAtt );
    return *this;
}

myStyleElement& myStyleElement::operator +=(myStyleAttribute newAtt)
{
    m_attrArray.Add( newAtt );
    return *this;
}

/**
 *
 */

bool myStyleElement::GetCSS(wxArrayString& cssElement, bool bAppend) {
    wxString sText;

    if (!bAppend)
        cssElement.Clear();

    if (m_attrArray.Count() > 0) {
        if (!m_sCSSComment.IsEmpty()) {
            sText = wxT("/** ") + m_sCSSComment + wxT(" **/");
            cssElement.Add(sText);
        }

        sText = m_sCSSTag;

        if (!m_sCSSClass.IsEmpty()) {
            sText += wxT(".") + m_sCSSClass;
        }

        if (!m_sCSSId.IsEmpty()) {
            sText += wxT("#") + m_sCSSId;
        }

        sText += wxT(" {");

        cssElement.Add( sText );

        for (size_t index = 0 ; index < m_attrArray.Count() ; index++) {
            wxString sAttrPair = wxT("\t") + *(m_attrArray[index]);

            cssElement.Add( sAttrPair );
        }

        cssElement.Add(wxT("}"));
    }

    return (cssElement.Count() > 0)?true:false;
}

/**
 *
 */

wxString myStyleElement::GetComment() {
    return m_sCSSComment;
}

/**
 *
 */

void myStyleElement::SetComment(wxString comment) {
    m_sCSSComment = comment;
}

/**
 *  Compare this element against another element. Return true if equal.
 */

bool myStyleElement::operator ==(const myStyleElement& compare) {
    bool bRes = false;

    if ((m_sCSSTag   == compare.m_sCSSTag) &&
        (m_sCSSClass == compare.m_sCSSClass) &&
        (m_sCSSId    == compare.m_sCSSId))
    {
        bRes = true;
    }

    return bRes;
}
/*----------------------------------------------------------------------------*/

myStyleSheet::myStyleSheet()
{
    // ctor
}

myStyleSheet::~myStyleSheet()
{
    // dtor
}

bool myStyleSheet::AddStyle( myStyleElement& elem ) {
    m_styleArray.Add( elem );
    return true;
}

myStyleSheet& myStyleSheet::operator +=( myStyleElement& elem ) {
    m_styleArray.Add( elem );
    return *this;
}

myStyleSheet& myStyleSheet::operator +( myStyleElement& elem ) {
    m_styleArray.Add( elem );
    return *this;
}

/**
 *
 */

bool myStyleSheet::GetCSS(wxArrayString& cssSheet) {
    cssSheet.Clear();

    for (size_t i = 0 ; i < m_styleArray.Count() ; i++) {
        m_styleArray[i].GetCSS(cssSheet);
    }

    return (cssSheet.Count() > 0)?true:false;
}

bool myStyleSheet::DumpToFile(FILE* oFP) {
    bool            bRes = false;

    if (!IsEmpty()) {
        wxArrayString   cssSheet;

        if (GetCSS( cssSheet )) {
            for (size_t i = 0 ; i < cssSheet.Count() ; i++) {
                fprintf(oFP, "%s\n", cssSheet[i].c_str());
            }
            bRes = true;
        }
    }

    return bRes;
}

bool myStyleSheet::IsEmpty() const {
    return m_styleArray.IsEmpty();
}

void myStyleSheet::Clear() {
    m_styleArray.Clear();
}

myStyleSheet& myStyleSheet::operator = (const myStyleSheet& copy) {
    m_styleArray = copy.m_styleArray;
    return *this;
}
