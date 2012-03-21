// Created on: 2001-08-24
// Created by: Alexnder GRIGORIEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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

//AGV 150202: Changed prototype XmlObjMgt::SetStringValue()

#define OCC6010 // vro 01.06.2004

# include <stdio.h>
#include <XmlMDataStd_RealArrayDriver.ixx>
#include <TDataStd_RealArray.hxx>
#include <XmlObjMgt.hxx>
#include <XmlMDataStd.hxx>

IMPLEMENT_DOMSTRING (FirstIndexString, "first")
IMPLEMENT_DOMSTRING (LastIndexString, "last")
IMPLEMENT_DOMSTRING (IsDeltaOn,       "delta")

//=======================================================================
//function : XmlMDataStd_RealArrayDriver
//purpose  : Constructor
//=======================================================================

XmlMDataStd_RealArrayDriver::XmlMDataStd_RealArrayDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_RealArrayDriver::NewEmpty() const
{
  return (new TDataStd_RealArray());
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMDataStd_RealArrayDriver::Paste
                                        (const XmlObjMgt_Persistent&  theSource,
                                         const Handle(TDF_Attribute)& theTarget,
                                         XmlObjMgt_RRelocationTable&  ) const
{
  Standard_Integer aFirstInd, aLastInd, ind;
  Standard_Real aValue;
  const XmlObjMgt_Element& anElement = theSource;

  // Read the FirstIndex; if the attribute is absent initialize to 1
  XmlObjMgt_DOMString aFirstIndex= anElement.getAttribute(::FirstIndexString());
  if (aFirstIndex == NULL)
    aFirstInd = 1;
  else if (!aFirstIndex.GetInteger(aFirstInd)) {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the first index"
                                 " for RealArray attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  // Read LastIndex; the attribute should be present
  if (!anElement.getAttribute(::LastIndexString()).GetInteger(aLastInd)) {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the last index"
                                 " for RealArray attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  Handle(TDataStd_RealArray) aRealArray =
    Handle(TDataStd_RealArray)::DownCast(theTarget);
  aRealArray->Init(aFirstInd, aLastInd);

  // Check the type of LDOMString
  const XmlObjMgt_DOMString& aString = XmlObjMgt::GetStringValue(anElement);
  if (aString.Type() == LDOMBasicString::LDOM_Integer) {
    if (aFirstInd == aLastInd) {
      Standard_Integer anIntValue;
      if (aString.GetInteger(anIntValue))
        aRealArray -> SetValue (aFirstInd, Standard_Real(anIntValue));
    } else {
      TCollection_ExtendedString aMessageString =
        TCollection_ExtendedString("Cannot retrieve array of real members"
                                   " for RealArray attribute from Integer \"")
        + aString + "\"";
      WriteMessage (aMessageString);
      return Standard_False;
    }
  } else {
    Standard_CString aValueStr = Standard_CString(aString.GetString());
    for (ind = aFirstInd; ind <= aLastInd; ind++)
    {
      if (!XmlObjMgt::GetReal(aValueStr, aValue)) {
        TCollection_ExtendedString aMessageString =
          TCollection_ExtendedString("Cannot retrieve real member"
                                     " for RealArray attribute as \"")
            + aValueStr + "\"";
        WriteMessage (aMessageString);
        return Standard_False;
      }
      aRealArray->SetValue(ind, aValue);
    }
  }
#ifdef DEB
  //cout << "CurDocVersion = " << XmlMDataStd::DocumentVersion() <<endl;
#endif
  Standard_Boolean aDelta(Standard_False);
  
  if(XmlMDataStd::DocumentVersion() > 2) {
    Standard_Integer aDeltaValue;
    if (!anElement.getAttribute(::IsDeltaOn()).GetInteger(aDeltaValue)) 
      {
	TCollection_ExtendedString aMessageString =
	  TCollection_ExtendedString("Cannot retrieve the isDelta value"
                                 " for RealArray attribute as \"")
        + aDeltaValue + "\"";
	WriteMessage (aMessageString);
	return Standard_False;
      } 
    else
      aDelta = (Standard_Boolean)aDeltaValue;
  }
#ifdef DEB
  else if(XmlMDataStd::DocumentVersion() == -1)
    cout << "Current DocVersion field is not initialized. "  <<endl;
#endif
  aRealArray->SetDelta(aDelta);

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataStd_RealArrayDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                         XmlObjMgt_Persistent&        theTarget,
                                         XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_RealArray) aRealArray =
    Handle(TDataStd_RealArray)::DownCast(theSource);

  Standard_Integer aL = aRealArray->Lower(), anU = aRealArray->Upper();
  TCollection_AsciiString aValueStr;

  if (aL != 1) theTarget.Element().setAttribute(::FirstIndexString(), aL);
  theTarget.Element().setAttribute(::LastIndexString(), anU);
  theTarget.Element().setAttribute(::IsDeltaOn(), aRealArray->GetDelta());

  Standard_Integer i = aL;
  while (1) {
#ifndef OCC6010
    aValueStr += TCollection_AsciiString(aRealArray->Value(i));
#else
    char aValueChar[32];
    sprintf(aValueChar, "%.15g", aRealArray->Value(i));
    aValueStr += aValueChar;
#endif
    if (i >= anU) break;
    aValueStr += ' ';
    ++i;
  }
  // No occurrence of '&', '<' and other irregular XML characters
  XmlObjMgt::SetStringValue (theTarget, aValueStr.ToCString(), Standard_True);
}
