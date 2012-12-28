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
    m_attrArray(copy.m_attrArray)
{

}

myStyleElement::~myStyleElement()
{

}

myStyleElement& myStyleElement::operator =(const myStyleElement& copy)
{
    m_sCSSTag = copy.m_sCSSTag;
    m_sCSSClass = copy.m_sCSSClass;
    m_attrArray = copy.m_attrArray;

    return *this;
}

myStyleElement& myStyleElement::AddAttribute(const wxString& sAttName, const wxString& sAttVal)
{
    m_attrArray.Add( myStyleAttribute( sAttName, sAttVal ) );
    return *this;
}

myStyleElement& myStyleElement::operator +(myStyleAttribute& newAtt)
{
    m_attrArray.Add( newAtt );
    return *this;
}

myStyleElement& myStyleElement::operator +=(myStyleAttribute& newAtt)
{
    m_attrArray.Add( newAtt );
    return *this;
}
