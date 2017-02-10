// Created on: 2017-02-13
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
#include <BinMXCAFDoc_NoteCommentDriver.hxx>
#include <XCAFDoc_NoteComment.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMXCAFDoc_NoteCommentDriver, BinMXCAFDoc_NoteDriver)

//=======================================================================
//function :
//purpose  : 
//=======================================================================
BinMXCAFDoc_NoteCommentDriver::BinMXCAFDoc_NoteCommentDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
  : BinMXCAFDoc_NoteDriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_NoteComment)->Name())
{
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMXCAFDoc_NoteCommentDriver::NewEmpty() const
{
  return new XCAFDoc_NoteComment();
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Standard_Boolean BinMXCAFDoc_NoteCommentDriver::Paste(const BinObjMgt_Persistent&  theSource,
                                                      const Handle(TDF_Attribute)& theTarget,
                                                      BinObjMgt_RRelocationTable&  theRelocTable) const
{
  if (!BinMXCAFDoc_NoteDriver::Paste(theSource, theTarget, theRelocTable))
    return Standard_False;

  Handle(XCAFDoc_NoteComment) aNote = Handle(XCAFDoc_NoteComment)::DownCast(theTarget);
  if (aNote.IsNull())
    return Standard_False;

  TCollection_ExtendedString aComment;
  if (!(theSource >> aComment))
    return Standard_False;

  aNote->Set(aComment);

  return Standard_True;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
void BinMXCAFDoc_NoteCommentDriver::Paste(const Handle(TDF_Attribute)& theSource,
					                                BinObjMgt_Persistent&        theTarget,
					                                BinObjMgt_SRelocationTable&  theRelocTable) const
{
  BinMXCAFDoc_NoteDriver::Paste(theSource, theTarget, theRelocTable);

  Handle(XCAFDoc_NoteComment) aNote = Handle(XCAFDoc_NoteComment)::DownCast(theSource);
  if (!aNote.IsNull())
    theTarget << aNote->Comment();
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
BinMXCAFDoc_NoteCommentDriver::BinMXCAFDoc_NoteCommentDriver(const Handle(CDM_MessageDriver)& theMsgDriver,
                                                             Standard_CString                 theName)
  : BinMXCAFDoc_NoteDriver(theMsgDriver, theName)
{

}
