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

    myStyleElement& operator += (myStyleAttribute newAttribute);
    myStyleElement& operator + (myStyleAttribute newAttribute);

    bool                    GetCSS(wxString cssElement);
    bool                    GetCSS(wxArrayString& cssElement);

    wxString                GetComment();
    void                    SetComment(wxString comment);

protected:
    size_t                  GetCount() const;
    const myStyleAttribute& operator [](size_t index);

    wxString                m_sCSSTag;          ///< Tag of element e.g. 'div'
    wxString                m_sCSSClass;        ///< Class of element e.g class="super"
    wxString                m_sCSSId;           ///< Id of element e.g. id="content"
    wxString                m_sCSSComment;      ///< Optional comment for element

    ArrayOfAttributes       m_attrArray;        ///< Array of attributes
};

WX_DECLARE_OBJARRAY( myStyleElement, ArrayOfStyles );


#endif // __MYSTYLESHEET_H__
