// Created on: 2004-09-27
// Created by: Pavel TELKOV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <CDM_MessageDriver.hxx>
#include <LDOM_MemManager.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_ExtStringArray.hxx>
#include <TDF_Attribute.hxx>
#include <XmlMDataStd.hxx>
#include <XmlMDataStd_ExtStringArrayDriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Document.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMDataStd_ExtStringArrayDriver,XmlMDF_ADriver)
IMPLEMENT_DOMSTRING (FirstIndexString, "first")
IMPLEMENT_DOMSTRING (LastIndexString, "last")
IMPLEMENT_DOMSTRING (ExtString,       "string")
IMPLEMENT_DOMSTRING (IsDeltaOn,       "delta")
IMPLEMENT_DOMSTRING (Separator,       "separator")
IMPLEMENT_DOMSTRING (AttributeIDString, "extstrarrattguid")

// Searches for a symbol within an array of strings.
// Returns TRUE if the symbol is found.
static Standard_Boolean Contains(const Handle(TDataStd_ExtStringArray)& arr,
                                 const TCollection_ExtendedString& c)
{
  for (Standard_Integer i = arr->Lower(); i <= arr->Upper(); i++)
  {
    const TCollection_ExtendedString& value = arr->Value(i);
    if (value.Search(c) != -1)
    {
      return Standard_True;
    }
  }
  return Standard_False;
}

//=======================================================================
//function : XmlMDataStd_ExtStringArrayDriver
//purpose  : Constructor
//=======================================================================

XmlMDataStd_ExtStringArrayDriver::XmlMDataStd_ExtStringArrayDriver
                        ( const Handle(CDM_MessageDriver)& theMsgDriver )
: XmlMDF_ADriver( theMsgDriver, NULL )
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_ExtStringArrayDriver::NewEmpty() const
{
  return ( new TDataStd_ExtStringArray() );
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMDataStd_ExtStringArrayDriver::Paste
                                        (const XmlObjMgt_Persistent&  theSource,
                                         const Handle(TDF_Attribute)& theTarget,
                                         XmlObjMgt_RRelocationTable& ) const
{
  Standard_Integer aFirstInd, aLastInd, ind;
  TCollection_ExtendedString aValue;
  const XmlObjMgt_Element& anElement = theSource;

  // Read the FirstIndex; if the attribute is absent initialize to 1
  XmlObjMgt_DOMString aFirstIndex= anElement.getAttribute(::FirstIndexString());
  if (aFirstIndex == NULL)
    aFirstInd = 1;
  else if (!aFirstIndex.GetInteger(aFirstInd)) {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the first index"
                                 " for ExtStringArray attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  // Read LastIndex; the attribute should be present
  if (!anElement.getAttribute(::LastIndexString()).GetInteger(aLastInd)) {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the last index"
                                 " for ExtStringArray attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  // Read separator.
  TCollection_ExtendedString separator;
  XmlObjMgt_DOMString aSeparator = anElement.getAttribute(::Separator());
  if (aSeparator.Type() != XmlObjMgt_DOMString::LDOM_NULL)
    separator = aSeparator.GetString();

  Handle(TDataStd_ExtStringArray) aExtStringArray =
    Handle(TDataStd_ExtStringArray)::DownCast(theTarget);
  aExtStringArray->Init(aFirstInd, aLastInd);
  
  // Read string values.
  if ( !separator.Length() && anElement.hasChildNodes() )
  {
    // Read values written by <string>VALUE<\string> notion - as children of the attribute.
    LDOM_Node aCurNode = anElement.getFirstChild();
    LDOM_Element* aCurElement = (LDOM_Element*)&aCurNode;
    TCollection_ExtendedString aValueStr;
    for (ind = aFirstInd; ind <= aLastInd && *aCurElement != anElement.getLastChild(); ind++)
    {
      XmlObjMgt::GetExtendedString( *aCurElement, aValueStr );
      aExtStringArray->SetValue(ind, aValueStr);
      aCurNode = aCurElement->getNextSibling();
      aCurElement = (LDOM_Element*)&aCurNode;
    }
    XmlObjMgt::GetExtendedString( *aCurElement, aValueStr );
    aExtStringArray->SetValue( aLastInd, aValueStr );
  }
  else
  {
    TCollection_ExtendedString xstr;
    XmlObjMgt::GetExtendedString(anElement, xstr);
#ifdef _DEBUG
    TCollection_AsciiString cstr(xstr, '?');
#endif

    // Split strings by the separator.
    Standard_Integer isym(1); // index of symbol in xstr
    Standard_ExtCharacter xsep = separator.Value(1);
    for (ind = aFirstInd; ind <= aLastInd; ind++)
    {
      // Calculate length of the string-value.
      Standard_Integer iend = isym;
      while (iend < xstr.Length())
      {
        if (xstr.Value(iend) == xsep)
        {
          break;
        }
        iend++;
      }
      if (iend <= xstr.Length() && 
          xstr.Value(iend) != xsep)
      {
        iend++;
      }

      // Allocate the string-value.
      TCollection_ExtendedString xvalue(iend - isym, '\0');

      // Set string-value.
      for (Standard_Integer i = isym; i < iend; ++i)
      {
        const Standard_ExtCharacter x = xstr.Value(i);
        xvalue.SetValue(i - isym + 1, x);
      }
#ifdef _DEBUG
      TCollection_AsciiString cvalue(xvalue, '?');
#endif

      // Set value for the array.
      aExtStringArray->SetValue(ind, xvalue);

      // Next string-value.
      isym = iend + 1;
    }
  }

  // Read delta-flag.
  Standard_Boolean aDelta(Standard_False);
  
  if(XmlMDataStd::DocumentVersion() > 2) {
    Standard_Integer aDeltaValue;
    if (!anElement.getAttribute(::IsDeltaOn()).GetInteger(aDeltaValue)) 
      {
        TCollection_ExtendedString aMessageString =
          TCollection_ExtendedString("Cannot retrieve the isDelta value"
                                     " for IntegerArray attribute as \"")
                                     + aDeltaValue + "\"";
        WriteMessage (aMessageString);
        return Standard_False;
      } 
    else
      aDelta = aDeltaValue != 0;
  }
#ifdef OCCT_DEBUG
  else if(XmlMDataStd::DocumentVersion() == -1)
    cout << "Current DocVersion field is not initialized. "  <<endl;
#endif
  aExtStringArray->SetDelta(aDelta);

  // attribute id
  Standard_GUID aGUID;
  XmlObjMgt_DOMString aGUIDStr = anElement.getAttribute(::AttributeIDString());
  if (aGUIDStr.Type() == XmlObjMgt_DOMString::LDOM_NULL)
    aGUID = TDataStd_ExtStringArray::GetID(); //default case
  else
    aGUID = Standard_GUID(Standard_CString(aGUIDStr.GetString())); // user defined case

  aExtStringArray->SetID(aGUID);

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataStd_ExtStringArrayDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                         XmlObjMgt_Persistent&        theTarget,
                                         XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_ExtStringArray) aExtStringArray =
    Handle(TDataStd_ExtStringArray)::DownCast(theSource);

  Standard_Integer aL = aExtStringArray->Lower(), anU = aExtStringArray->Upper(), i;

  XmlObjMgt_Element& anElement = theTarget;

  if (aL != 1) anElement.setAttribute(::FirstIndexString(), aL);
  anElement.setAttribute(::LastIndexString(), anU);
  anElement.setAttribute(::IsDeltaOn(), aExtStringArray->GetDelta() ? 1 : 0);

  // Find a separator.
  Standard_Boolean found(Standard_True);
  // Preferrable symbols for the separator: - _ . : ^ ~
  // Don't use a space as a separator: XML low-level parser sometimes "eats" it.
  Standard_Character c = '-';
  static Standard_Character aPreferable[] = "-_.:^~";
  for (i = 0; found && aPreferable[i]; i++)
  {
    c = aPreferable[i];
    found = Contains(aExtStringArray, TCollection_ExtendedString(c));
  }
  // If all prefferable symbols exist in the array, 
  // try to use any other simple symbols.
  if (found)
  {
    c = '!';
    while (found && c < '~')
    {
      found = Standard_False;
#ifdef _DEBUG
      TCollection_AsciiString cseparator(c); // deb
#endif
      TCollection_ExtendedString separator(c);
      found = Contains(aExtStringArray, separator);
      if (found)
      {
        c++;
        // Skip forbidden symbols for XML.
        while (c < '~' && (c == '&' || c == '<'))
        {
          c++;
        }
      }
    }
  }
  
  if (found)
  {
      // There is no unique separator. Use child elements for storage of strings of the array.

      // store a set of elements with string in each of them
      XmlObjMgt_Document aDoc (anElement.getOwnerDocument());
      for ( i = aL; i <= anU; i++ )
      {
        const TCollection_ExtendedString& aValueStr = aExtStringArray->Value( i );
        XmlObjMgt_Element aCurTarget = aDoc.createElement( ::ExtString() );
        XmlObjMgt::SetExtendedString( aCurTarget, aValueStr );
        anElement.appendChild( aCurTarget );
      }
  }
  else
  {
      // Set the separator.
      TCollection_AsciiString csep(c);
      anElement.setAttribute(::Separator(), csep.ToCString());

      // Calculate length of the common string.
      Standard_Integer len(0);
      for (i = aL; i <= anU; i++)
      {
        const TCollection_ExtendedString& aValueStr = aExtStringArray->Value(i);
        len += aValueStr.Length();
        len++; // for separator or ending \0 symbol
      }
      if (!len)
        len++; // for end of line \0 symbol

      // Merge all strings of the array into one extended string separated by the "separator".
      Standard_Integer isym(1);
      TCollection_ExtendedString xstr(len, c);
      for (i = aL; i <= anU; i++)
      {
        const TCollection_ExtendedString& aValueStr = aExtStringArray->Value(i);
        for (Standard_Integer k = 1; k <= aValueStr.Length(); k++)
        {
          xstr.SetValue(isym++, aValueStr.Value(k));
        }
        xstr.SetValue(isym++, c);
      }
      if (xstr.SearchFromEnd(c) == isym - 1)
        isym--; // replace the last separator by '\0'
      xstr.SetValue(isym, '\0');
#ifdef _DEBUG
      TCollection_AsciiString cstr(xstr, '?'); // deb
#endif

      // Set UNICODE value.
      XmlObjMgt::SetExtendedString(theTarget, xstr);
  }
  if(aExtStringArray->ID() != TDataStd_ExtStringArray::GetID()) {
    //convert GUID
    Standard_Character aGuidStr [Standard_GUID_SIZE_ALLOC];
    Standard_PCharacter pGuidStr = aGuidStr;
    aExtStringArray->ID().ToCString (pGuidStr);
    theTarget.Element().setAttribute (::AttributeIDString(), aGuidStr);
  }
}
