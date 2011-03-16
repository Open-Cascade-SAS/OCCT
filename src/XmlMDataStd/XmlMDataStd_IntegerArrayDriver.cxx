// File:        XmlMDataStd_IntegerArrayDriver.cxx
// Created:     Fri Aug 24 20:46:58 2001
// Author:      Alexnder GRIGORIEV
// Copyright:   Open Cascade 2001-2002
// History:   AGV 150202: Changed prototype XmlObjMgt::SetStringValue()

#include <XmlMDataStd_IntegerArrayDriver.ixx>
#include <TDataStd_IntegerArray.hxx>
#include <XmlObjMgt.hxx>
#include <XmlMDataStd.hxx>

IMPLEMENT_DOMSTRING (FirstIndexString, "first")
IMPLEMENT_DOMSTRING (LastIndexString,  "last")
IMPLEMENT_DOMSTRING (IsDeltaOn,        "delta")
//=======================================================================
//function : XmlMDataStd_IntegerArrayDriver
//purpose  : Constructor
//=======================================================================

XmlMDataStd_IntegerArrayDriver::XmlMDataStd_IntegerArrayDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_IntegerArrayDriver::NewEmpty() const
{
  return (new TDataStd_IntegerArray());
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMDataStd_IntegerArrayDriver::Paste
                                (const XmlObjMgt_Persistent&  theSource,
                                 const Handle(TDF_Attribute)& theTarget,
                                 XmlObjMgt_RRelocationTable&  ) const
{
  Standard_Integer aFirstInd, aLastInd, aValue, ind;
  const XmlObjMgt_Element& anElement = theSource;

  // Read the FirstIndex; if the attribute is absent initialize to 1
  XmlObjMgt_DOMString aFirstIndex= anElement.getAttribute(::FirstIndexString());
  if (aFirstIndex == NULL)
    aFirstInd = 1;
  else if (!aFirstIndex.GetInteger(aFirstInd)) {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the first index"
                                 " for IntegerArray attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  // Read the LastIndex; the attribute should be present
  if (!anElement.getAttribute(::LastIndexString()).GetInteger(aLastInd)) {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the last index"
                                 " for IntegerArray attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  Handle(TDataStd_IntegerArray) anIntArray =
    Handle(TDataStd_IntegerArray)::DownCast(theTarget);
  anIntArray->Init(aFirstInd, aLastInd);

  if(aFirstInd == aLastInd) {
    Standard_Integer anInteger;
    if(!XmlObjMgt::GetStringValue(anElement).GetInteger( anInteger)) {
      TCollection_ExtendedString aMessageString =
        TCollection_ExtendedString("Cannot retrieve integer member"
                                   " for IntegerArray attribute as \"");
      WriteMessage (aMessageString);
      return Standard_False;
    }
    anIntArray->SetValue(aFirstInd, anInteger);
    
  }
  else {
    // Warning: check implementation of XmlObjMgt_DOMString !! For LDOM this is OK
    Standard_CString aValueStr =
      Standard_CString(XmlObjMgt::GetStringValue(anElement).GetString());
    
    for (ind = aFirstInd; ind <= aLastInd; ind++)
    {
      if (!XmlObjMgt::GetInteger(aValueStr, aValue)) {
        TCollection_ExtendedString aMessageString =
          TCollection_ExtendedString("Cannot retrieve integer member"
                                     " for IntegerArray attribute as \"")
            + aValueStr + "\"";
        WriteMessage (aMessageString);
        return Standard_False;
      }
      anIntArray->SetValue(ind, aValue);
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
                                 " for IntegerArray attribute as \"")
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
  anIntArray->SetDelta(aDelta);
  
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataStd_IntegerArrayDriver::Paste
                                (const Handle(TDF_Attribute)& theSource,
                                 XmlObjMgt_Persistent&        theTarget,
                                 XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_IntegerArray) anIntArray =
    Handle(TDataStd_IntegerArray)::DownCast(theSource);

  Standard_Integer aL = anIntArray->Lower(), anU = anIntArray->Upper();
  TCollection_AsciiString aValueStr;

  if (aL != 1) theTarget.Element().setAttribute (::FirstIndexString(), aL);
  theTarget.Element().setAttribute(::LastIndexString(), anU);
  theTarget.Element().setAttribute(::IsDeltaOn(), anIntArray->GetDelta());

  Standard_Integer i = aL;
  while (1) {
    aValueStr += TCollection_AsciiString(anIntArray->Value(i));
    if (i >= anU) break;
    aValueStr += ' ';
    ++i;
  }
  // No occurrence of '&', '<' and other irregular XML characters
  XmlObjMgt::SetStringValue (theTarget, aValueStr.ToCString(), Standard_True);
}
