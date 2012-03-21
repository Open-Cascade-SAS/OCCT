// Created on: 2008-12-10
// Created by: Pavel TELKOV
// Copyright (c) 2008-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <XmlMXCAFDoc_DimTolDriver.ixx>
#include <XmlObjMgt.hxx>
#include <XCAFDoc_DimTol.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfReal.hxx>

IMPLEMENT_DOMSTRING (NameIndexString,  "name")
IMPLEMENT_DOMSTRING (DescrIndexString, "descr")
IMPLEMENT_DOMSTRING (FirstIndexString, "first")
IMPLEMENT_DOMSTRING (LastIndexString,  "last")
IMPLEMENT_DOMSTRING (ValueIndexString, "values")

//=======================================================================
//function : XmlMXCAFDoc_DimTolDriver
//purpose  : Constructor
//=======================================================================
XmlMXCAFDoc_DimTolDriver::XmlMXCAFDoc_DimTolDriver
  (const Handle(CDM_MessageDriver)& theMsgDriver)
: XmlMDF_ADriver (theMsgDriver, "xcaf", "DimTol")
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMXCAFDoc_DimTolDriver::NewEmpty() const
{
  return (new XCAFDoc_DimTol());
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMXCAFDoc_DimTolDriver::Paste
                                (const XmlObjMgt_Persistent&  theSource,
                                 const Handle(TDF_Attribute)& theTarget,
                                 XmlObjMgt_RRelocationTable&  ) const
{
  Standard_Integer aKind;
  XmlObjMgt_DOMString anIntStr = XmlObjMgt::GetStringValue(theSource);

  if (anIntStr.GetInteger(aKind) == Standard_False) {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve DimTol attribute kind from \"")
        + anIntStr + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }
  
  const XmlObjMgt_Element& anElement = theSource;
  XmlObjMgt_DOMString aNameStr = anElement.getAttribute(::NameIndexString());
  XmlObjMgt_DOMString aDescrStr = anElement.getAttribute(::DescrIndexString());
  if ( aNameStr == NULL || aDescrStr == NULL ) {
    TCollection_ExtendedString aMessageString
      ("Cannot retrieve DimTol attribute name or description");
    WriteMessage (aMessageString);
    return Standard_False;
  }

  Handle(TCollection_HAsciiString) aName =
    new TCollection_HAsciiString( aNameStr.GetString() );
  Handle(TCollection_HAsciiString) aDescr =
    new TCollection_HAsciiString( aDescrStr.GetString() );
  
  Standard_Integer aFirstInd, aLastInd;
  XmlObjMgt_DOMString aFirstIndex = anElement.getAttribute(::FirstIndexString());
  if (aFirstIndex == NULL)
    aFirstInd = 1;
  else if (!aFirstIndex.GetInteger(aFirstInd)) {
    TCollection_ExtendedString aMessageString
      ("Cannot retrieve the DimTol first index for real array ");
    WriteMessage (aMessageString);
    return Standard_False;
  }
  XmlObjMgt_DOMString aLastIndex = anElement.getAttribute(::LastIndexString());
  if (aLastIndex == NULL)
    aLastInd = 0;
  else if (!aLastIndex.GetInteger(aLastInd)) {
    TCollection_ExtendedString aMessageString
      ("Cannot retrieve the DimTol last index for real array ");
    WriteMessage (aMessageString);
    return Standard_False;
  }
  
  const Standard_Integer aLength = aLastInd - aFirstInd + 1;
  Handle(TColStd_HArray1OfReal) aHArr;
  if ( aLength > 0 ) {
    // read real array
    Standard_Real aValue = 0.;
    const XmlObjMgt_DOMString& aString = anElement.getAttribute(::ValueIndexString());
    aHArr = new TColStd_HArray1OfReal( aFirstInd, aLastInd );
    Standard_CString aValueStr = Standard_CString(aString.GetString());
    for (Standard_Integer ind = aFirstInd; ind <= aLastInd; ind++)
    {
      if (!XmlObjMgt::GetReal(aValueStr, aValue)) {
        TCollection_ExtendedString aMessageString =
          TCollection_ExtendedString("Cannot retrieve real member"
                                     " for real array \"")
            + aValueStr + "\"";
        WriteMessage (aMessageString);
        return Standard_False;
      }
      aHArr->SetValue(ind, aValue);
    }
  }
  
  Handle(XCAFDoc_DimTol) anAtt = Handle(XCAFDoc_DimTol)::DownCast(theTarget);
  anAtt->Set(aKind, aHArr, aName, aDescr);

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMXCAFDoc_DimTolDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                       XmlObjMgt_Persistent&        theTarget,
                                       XmlObjMgt_SRelocationTable&  ) const
{
  Handle(XCAFDoc_DimTol) anAtt = Handle(XCAFDoc_DimTol)::DownCast(theSource);
  
  XmlObjMgt_DOMString aNameString, aDescrString;
  if ( !anAtt->GetName().IsNull() )
    aNameString = anAtt->GetName()->String().ToCString();
  if ( !anAtt->GetDescription().IsNull() )
    aDescrString = anAtt->GetDescription()->String().ToCString();

  XmlObjMgt::SetStringValue (theTarget, anAtt->GetKind());
  theTarget.Element().setAttribute(::NameIndexString(), aNameString);
  theTarget.Element().setAttribute(::DescrIndexString(),aDescrString);

  Handle(TColStd_HArray1OfReal) aHArr = anAtt->GetVal();
  Standard_Integer aFirstInd = 1, aLastInd = 0;
  if ( !aHArr.IsNull() )
  {
    aFirstInd = aHArr->Lower();
    aLastInd = aHArr->Upper();
  }
  theTarget.Element().setAttribute(::FirstIndexString(), aFirstInd);
  theTarget.Element().setAttribute(::LastIndexString(), aLastInd);
  if ( aLastInd >= aFirstInd )
  {
    TCollection_AsciiString aValueStr;
    for ( Standard_Integer i = aFirstInd; i <= aLastInd; i++ )
    {
      char aValueChar[256];
      sprintf(aValueChar, "%.15g", aHArr->Value(i));
      aValueStr += aValueChar;
      if ( i < aLastInd )
        aValueStr += ' ';
    }
    theTarget.Element().setAttribute(::ValueIndexString(), aValueStr.ToCString());
  }
}
