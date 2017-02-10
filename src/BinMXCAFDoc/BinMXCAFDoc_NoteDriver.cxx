// Created on: 2017-02-10
// Created by: Eugeny NIKONOV
// Copyright (c) 2005-2017 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <BinObjMgt_Persistent.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <BinMXCAFDoc_NoteDriver.hxx>
#include <XCAFDoc_Note.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMXCAFDoc_NoteDriver, BinMDF_ADriver)

//=======================================================================
//function :
//purpose  : 
//=======================================================================
BinMXCAFDoc_NoteDriver::BinMXCAFDoc_NoteDriver(const Handle(CDM_MessageDriver)& theMsgDriver,
                                               Standard_CString                 theName)
  : BinMDF_ADriver(theMsgDriver, theName)
{

}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Standard_Boolean BinMXCAFDoc_NoteDriver::Paste(const BinObjMgt_Persistent&  theSource,
                                               const Handle(TDF_Attribute)& theTarget,
                                               BinObjMgt_RRelocationTable&  /*theRelocTable*/) const
{
  Handle(XCAFDoc_Note) aNote = Handle(XCAFDoc_Note)::DownCast(theTarget);
  if (aNote.IsNull())
    return Standard_False;
  
  TCollection_ExtendedString aUserName, aTimeStamp;
  if (!(theSource >> aUserName >> aTimeStamp))
    return Standard_False;

  aNote->Set(aUserName, aTimeStamp);

  return Standard_True;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
void BinMXCAFDoc_NoteDriver::Paste(const Handle(TDF_Attribute)& theSource,
					                         BinObjMgt_Persistent&        theTarget,
					                         BinObjMgt_SRelocationTable&  /*theRelocTable*/) const
{
  Handle(XCAFDoc_Note) aNote = Handle(XCAFDoc_Note)::DownCast(theSource);
  if (!aNote.IsNull())
    theTarget << aNote->UserName() << aNote->TimeStamp();
}
