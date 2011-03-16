// File:        XmlMDataStd_DirectoryDriver.cxx
// Created:     Fri Aug 24 20:46:58 2001
// Author:      Alexnder GRIGORIEV
// Copyright:   Open Cascade 2001
// History:

#include <XmlMDataStd_DirectoryDriver.ixx>
#include <TDataStd_Directory.hxx>

//=======================================================================
//function : XmlMDataStd_DirectoryDriver
//purpose  : Constructor
//=======================================================================

XmlMDataStd_DirectoryDriver::XmlMDataStd_DirectoryDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

 Handle(TDF_Attribute) XmlMDataStd_DirectoryDriver::NewEmpty() const
{
  return (new TDataStd_Directory());
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

 Standard_Boolean XmlMDataStd_DirectoryDriver::Paste
                                        (const XmlObjMgt_Persistent&,
                                         const Handle(TDF_Attribute)&,
                                         XmlObjMgt_RRelocationTable& ) const
{
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

 void XmlMDataStd_DirectoryDriver::Paste (const Handle(TDF_Attribute)&,
                                          XmlObjMgt_Persistent&,
                                          XmlObjMgt_SRelocationTable&  ) const
{
}
