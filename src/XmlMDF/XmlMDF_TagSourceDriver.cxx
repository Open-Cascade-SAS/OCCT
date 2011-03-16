// File:      XmlMDF_TagSourceDriver.cxx
// Created:   29.08.01 18:04:51
// Author:    Julia DOROVSKIKH
// Copyright: Open Cascade 2001
// History:   AGV 150202: Changed prototype XmlObjMgt::SetStringValue()

#include <XmlMDF_TagSourceDriver.ixx>
#include <XmlObjMgt.hxx>
#include <TDF_TagSource.hxx>

//=======================================================================
//function : XmlMDF_TagSourceDriver
//purpose  : Constructor
//=======================================================================

XmlMDF_TagSourceDriver::XmlMDF_TagSourceDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDF_TagSourceDriver::NewEmpty() const
{
  return (new TDF_TagSource());
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMDF_TagSourceDriver::Paste 
                                (const XmlObjMgt_Persistent&  theSource,
                                 const Handle(TDF_Attribute)& theTarget,
                                 XmlObjMgt_RRelocationTable&  ) const
{
  Standard_Integer aTag;
  XmlObjMgt_DOMString aTagStr = XmlObjMgt::GetStringValue(theSource.Element());

  if (aTagStr.GetInteger(aTag) == Standard_False) {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString ("Cannot retrieve TagSource attribute from \"")
        + aTagStr + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  if (aTag < 0) {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString ("Invalid value of TagSource retrieved: ")
        + aTag;
    WriteMessage (aMessageString);
    return Standard_False;
  }

  Handle(TDF_TagSource) aT = Handle(TDF_TagSource)::DownCast (theTarget);
  aT->Set(aTag);

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDF_TagSourceDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                    XmlObjMgt_Persistent&        theTarget,
                                    XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDF_TagSource) aTag = Handle(TDF_TagSource)::DownCast(theSource);
  // No occurrence of '&', '<' and other irregular XML characters
  XmlObjMgt::SetStringValue (theTarget.Element(), aTag->Get(), Standard_True);
}

