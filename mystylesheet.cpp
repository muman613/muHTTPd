#include <wx/wx.h>
#include <wx/arrstr.h>
#include <wx/hashmap.h>
#include <wx/file.h>
#include "mystylesheet.h"
#include "dbgutils.h"

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( ArrayOfAttributes );
WX_DEFINE_OBJARRAY( ArrayOfStyles );


myStyleAttribute::myStyleAttribute()
{

}

myStyleAttribute::myStyleAttribute(const wxString& sAttributeName, const wxString& sAttributeValue)
:   m_sAttributeName(sAttributeName),
    m_sAttributeValue(sAttributeValue)
{

}

myStyleAttribute::myStyleAttribute(const myStyleAttribute& copy)
:   m_sAttributeName(copy.m_sAttributeName),
    m_sAttributeValue(copy.m_sAttributeValue)
{

}

myStyleAttribute::~myStyleAttribute()
{

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

wxString myStyleAttribute::operator *() const {
    wxString sAttributeText = wxString::Format(wxT("%s : %s ;"),
                                               m_sAttributeName.c_str(),
                                               m_sAttributeValue.c_str());
    return sAttributeText;
}

/*----------------------------------------------------------------------------*/

//myStyleElement::myStyleElement()
//{
//
//}

myStyleElement::myStyleElement(const wxString sStyleTag,
                               const wxString sStyleClass,
                               const wxString sStyleId)
:   m_sCSSTag(sStyleTag),
    m_sCSSClass(sStyleClass),
    m_sCSSId(sStyleId)
{

}

myStyleElement::myStyleElement(const myStyleElement& copy)
:   m_sCSSTag(copy.m_sCSSTag),
    m_sCSSClass(copy.m_sCSSClass),
    m_sCSSId(copy.m_sCSSId),
    m_sCSSComment(copy.m_sCSSComment),
    m_attrArray(copy.m_attrArray)
{
    // ctor
}

myStyleElement::~myStyleElement()
{
    // dtor
}

myStyleElement& myStyleElement::operator =(const myStyleElement& copy)
{
    m_sCSSTag       = copy.m_sCSSTag;
    m_sCSSClass     = copy.m_sCSSClass;
    m_sCSSId        = copy.m_sCSSId;
    m_sCSSComment   = copy.m_sCSSComment;
    m_attrArray     = copy.m_attrArray;

    return *this;
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

bool myStyleElement::GetCSS(wxArrayString& cssElement) {
    wxString sText;

    cssElement.Clear();

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

    return true;
}

wxString myStyleElement::GetComment() {
    return m_sCSSComment;
}

void myStyleElement::SetComment(wxString comment) {
    m_sCSSComment = comment;
}

