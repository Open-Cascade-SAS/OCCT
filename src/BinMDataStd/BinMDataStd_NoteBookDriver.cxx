// File:	BinMDataStd_NoteBookDriver.cxx
// Created:	Thu May 13 12:34:39 2004
// Author:	Sergey ZARITCHNY <szy@opencascade.com>
// Copyright:	Open CasCade S.A. 2004


#include <BinMDataStd_NoteBookDriver.ixx>
#include <CDM_MessageDriver.hxx>
#include <TDF_Attribute.hxx>
#include <TDataStd_NoteBook.hxx>
#include <BinMDF_ADriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <BinObjMgt_RRelocationTable.hxx>
#include <BinObjMgt_SRelocationTable.hxx>
//=======================================================================
//function : BinMDataStd_NoteBookDriver
//purpose  : 
//=======================================================================

BinMDataStd_NoteBookDriver::BinMDataStd_NoteBookDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TDataStd_NoteBook)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) BinMDataStd_NoteBookDriver::NewEmpty() const
{
  return new TDataStd_NoteBook();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================

Standard_Boolean BinMDataStd_NoteBookDriver::Paste
                                (const BinObjMgt_Persistent&,
                                 const Handle(TDF_Attribute)&,
                                 BinObjMgt_RRelocationTable& ) const
{return Standard_True;}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================

void BinMDataStd_NoteBookDriver::Paste (const Handle(TDF_Attribute)&,
                                       BinObjMgt_Persistent&,
                                       BinObjMgt_SRelocationTable&  ) const
{}


