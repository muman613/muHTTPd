#ifndef __MYSTYLESHEET_H__
#define __MYSTYLESHEET_H__

#include <wx/arrstr.h>
#include <wx/hashmap.h>
#include <wx/file.h>

/**
 *
 */

class myStyleAttribute {
public:
    myStyleAttribute();
    myStyleAttribute(const wxString& sStyleAttribute, const wxString& sStyleValue);
    myStyleAttribute(const myStyleAttribute& copy);
    virtual ~myStyleAttribute();

    myStyleAttribute& operator =(const myStyleAttribute& copy);

    wxString        operator *() const;
    wxString        AttributeName() const;
    wxString        AttributeValue() const;

protected:
    wxString        m_sAttributeName;
    wxString        m_sAttributeValue;
};

WX_DECLARE_OBJARRAY( myStyleAttribute, ArrayOfAttributes );

/**
 * tag.class#id { attr : val ; }
 */

class myStyleElement {
public:
    myStyleElement(const wxString sStyleTag   = wxEmptyString,
                   const wxString sStyleClass = wxEmptyString,
                   const wxString sStyleId    = wxEmptyString);
    myStyleElement(const myStyleElement& copy);
    virtual ~myStyleElement();

    myStyleElement& operator =(const myStyleElement& copy);

    myStyleElement& AddAttribute(const wxString& sAttributeName, const wxString& sAttributeValue);

    myStyleElement& operator += (myStyleAttribute& newAttribute);
    myStyleElement& operator + (myStyleAttribute& newAttribute);

    wxString                GetCSS();

protected:
    size_t                  GetCount() const;
    const myStyleAttribute& operator [](size_t index);

    wxString                m_sCSSTag;
    wxString                m_sCSSClass;
    wxString                m_sCSSId;

    ArrayOfAttributes       m_attrArray;
};

WX_DECLARE_OBJARRAY( myStyleElement, ArrayOfStyles );


#endif // __MYSTYLESHEET_H__
