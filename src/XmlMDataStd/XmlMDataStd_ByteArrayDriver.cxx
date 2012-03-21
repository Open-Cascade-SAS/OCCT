// Created on: 2007-05-29
// Created by: Vlad Romashko
// Copyright (c) 2007-2012 OPEN CASCADE SAS
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


#include <XmlMDataStd_ByteArrayDriver.ixx>
#include <TDataStd_ByteArray.hxx>
#include <TColStd_HArray1OfByte.hxx>
#include <XmlObjMgt.hxx>
#include <XmlMDataStd.hxx>

IMPLEMENT_DOMSTRING (FirstIndexString, "first")
IMPLEMENT_DOMSTRING (LastIndexString,  "last")
IMPLEMENT_DOMSTRING (IsDeltaOn,        "delta")
//=======================================================================
//function : XmlMDataStd_ByteArrayDriver
//purpose  : Constructor
//=======================================================================
XmlMDataStd_ByteArrayDriver::XmlMDataStd_ByteArrayDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
     : XmlMDF_ADriver (theMsgDriver, NULL)
{

}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_ByteArrayDriver::NewEmpty() const
{
  return new TDataStd_ByteArray();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMDataStd_ByteArrayDriver::Paste(const XmlObjMgt_Persistent&  theSource,
						    const Handle(TDF_Attribute)& theTarget,
						    XmlObjMgt_RRelocationTable&  ) const
{
  Standard_Integer aFirstInd, aLastInd, aValue;
  const XmlObjMgt_Element& anElement = theSource;

  // Read the FirstIndex; if the attribute is absent initialize to 1
  XmlObjMgt_DOMString aFirstIndex= anElement.getAttribute(::FirstIndexString());
  if (aFirstIndex == NULL)
    aFirstInd = 1;
  else if (!aFirstIndex.GetInteger(aFirstInd)) 
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the first index"
                                 " for ByteArray attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  // Read the LastIndex; the attribute should be present
  if (!anElement.getAttribute(::LastIndexString()).GetInteger(aLastInd)) 
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the last index"
                                 " for ByteArray attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  if (aFirstInd > aLastInd)
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("The last index is greater than the first index"
                                 " for ByteArray attribute \"");
    WriteMessage (aMessageString);
    return Standard_False;
  }


  Handle(TDataStd_ByteArray) aByteArray = Handle(TDataStd_ByteArray)::DownCast(theTarget);
  Handle(TColStd_HArray1OfByte) array = new TColStd_HArray1OfByte(aFirstInd, aLastInd);

  Standard_CString aValueStr = Standard_CString(XmlObjMgt::GetStringValue(anElement).GetString());
  Standard_Integer i = array->Lower(), upper = array->Upper();
  for (; i <= upper; i++)
  {
    if (!XmlObjMgt::GetInteger(aValueStr, aValue)) 
    {
      TCollection_ExtendedString aMessageString =
	TCollection_ExtendedString("Cannot retrieve integer member"
				   " for ByteArray attribute as \"")
	  + aValueStr + "\"";
      WriteMessage (aMessageString);
      return Standard_False;
    }
    array->SetValue(i, (Standard_Byte) aValue);
  }
  aByteArray->ChangeArray(array);
  
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
                                 " for ByteArray attribute as \"")
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
  aByteArray->SetDelta(aDelta);

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataStd_ByteArrayDriver::Paste(const Handle(TDF_Attribute)& theSource,
					XmlObjMgt_Persistent&        theTarget,
					XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_ByteArray) aByteArray = Handle(TDataStd_ByteArray)::DownCast(theSource);

  Standard_Integer aL = aByteArray->Lower();
  Standard_Integer anU = aByteArray->Upper();
  TCollection_AsciiString aValueStr;

  theTarget.Element().setAttribute(::FirstIndexString(), aL);
  theTarget.Element().setAttribute(::LastIndexString(), anU);
  theTarget.Element().setAttribute(::IsDeltaOn(),aByteArray->GetDelta());

  const Handle(TColStd_HArray1OfByte)& array = aByteArray->InternalArray();
  Standard_Integer lower = array->Lower(), i = lower, upper = array->Upper();
  for (; i <= upper; i++)
  {
    aValueStr += TCollection_AsciiString((Standard_Integer) array->Value(i));
    aValueStr += ' ';
  }
  XmlObjMgt::SetStringValue (theTarget, aValueStr.ToCString(), Standard_True);

}
