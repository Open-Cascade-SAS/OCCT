// File:      XmlMDataStd_NameDriver.cxx
// Created:   01.08.01 09:19:45
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2001
// History:

#include <XmlMDataStd_NameDriver.ixx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_RRelocationTable.hxx>
#include <XmlObjMgt_SRelocationTable.hxx>
#include <TDataStd_Name.hxx>

//=======================================================================
//function : XmlMDataStd_NameDriver
//purpose  : Constructor
//=======================================================================

XmlMDataStd_NameDriver::XmlMDataStd_NameDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty()
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_NameDriver::NewEmpty () const
{
  return (new TDataStd_Name());
}

//=======================================================================
//function : Paste()
//purpose  : 
//=======================================================================
Standard_Boolean XmlMDataStd_NameDriver::Paste
                               (const XmlObjMgt_Persistent&  theSource,
                                const Handle(TDF_Attribute)& theTarget,
                                XmlObjMgt_RRelocationTable&  ) const
{
  TCollection_ExtendedString aString;
  if (XmlObjMgt::GetExtendedString (theSource, aString))
  {
    Handle(TDataStd_Name)::DownCast(theTarget) -> Set (aString);
    return Standard_True;
  }
  WriteMessage("error retrieving ExtendedString for type TDataStd_Name");
  return Standard_False;
}

//=======================================================================
//function : Paste()
//purpose  : 
//=======================================================================
void XmlMDataStd_NameDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                    XmlObjMgt_Persistent&        theTarget,
                                    XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_Name) aName = Handle(TDataStd_Name)::DownCast(theSource);
  XmlObjMgt::SetExtendedString (theTarget, aName -> Get());
}
