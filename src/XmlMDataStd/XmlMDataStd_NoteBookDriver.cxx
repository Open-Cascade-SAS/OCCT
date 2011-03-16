// File:        XmlMDataStd_NoteBookDriver.cxx
// Created:     Fri Aug 24 20:46:58 2001
// Author:      Alexnder GRIGORIEV
// Copyright:   Open Cascade 2001
// History:

#include <XmlMDataStd_NoteBookDriver.ixx>
#include <TDataStd_NoteBook.hxx>

//=======================================================================
//function : XmlMDataStd_NoteBookDriver
//purpose  : Constructor
//=======================================================================

XmlMDataStd_NoteBookDriver::XmlMDataStd_NoteBookDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

 Handle(TDF_Attribute) XmlMDataStd_NoteBookDriver::NewEmpty() const
{
  return (new TDataStd_NoteBook());
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

 Standard_Boolean XmlMDataStd_NoteBookDriver::Paste
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

 void XmlMDataStd_NoteBookDriver::Paste (const Handle(TDF_Attribute)&,
                                         XmlObjMgt_Persistent&,
                                         XmlObjMgt_SRelocationTable&  ) const
{
}
