// File:        XmlMDataStd_IntegerDriver.cxx
// Created:     Fri Aug 24 20:46:58 2001
// Author:      Alexnder GRIGORIEV
// Copyright:   Open Cascade 2001
// History:

#include <XmlMDataStd_IntegerDriver.ixx>
#include <XmlObjMgt.hxx>
#include <TDataStd_Integer.hxx>

//=======================================================================
//function : XmlMDataStd_IntegerDriver
//purpose  : Constructor
//=======================================================================

XmlMDataStd_IntegerDriver::XmlMDataStd_IntegerDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_IntegerDriver::NewEmpty() const
{
  return (new TDataStd_Integer());
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMDataStd_IntegerDriver::Paste
                                (const XmlObjMgt_Persistent&  theSource,
                                 const Handle(TDF_Attribute)& theTarget,
                                 XmlObjMgt_RRelocationTable&  ) const
{
  Standard_Integer aValue;
  XmlObjMgt_DOMString anIntStr= XmlObjMgt::GetStringValue(theSource);

  if (anIntStr.GetInteger(aValue) == Standard_False) {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve Integer attribute from \"")
        + anIntStr + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  Handle(TDataStd_Integer) anInt= Handle(TDataStd_Integer)::DownCast(theTarget);
  anInt->Set(aValue);

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataStd_IntegerDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                       XmlObjMgt_Persistent&        theTarget,
                                       XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_Integer) anInt= Handle(TDataStd_Integer)::DownCast(theSource);
  XmlObjMgt::SetStringValue (theTarget, anInt->Get());
}
