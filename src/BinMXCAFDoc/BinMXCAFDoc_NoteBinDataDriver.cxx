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
#include <TColStd_HArray1OfByte.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <BinMXCAFDoc_NoteBinDataDriver.hxx>
#include <XCAFDoc_NoteBinData.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMXCAFDoc_NoteBinDataDriver, BinMXCAFDoc_NoteDriver)

//=======================================================================
//function :
//purpose  : 
//=======================================================================
BinMXCAFDoc_NoteBinDataDriver::BinMXCAFDoc_NoteBinDataDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
  : BinMXCAFDoc_NoteDriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_NoteBinData)->Name())
{
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMXCAFDoc_NoteBinDataDriver::NewEmpty() const
{
  return new XCAFDoc_NoteBinData();
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Standard_Boolean BinMXCAFDoc_NoteBinDataDriver::Paste(const BinObjMgt_Persistent&  theSource,
                                                      const Handle(TDF_Attribute)& theTarget,
                                                      BinObjMgt_RRelocationTable&  theRelocTable) const
{
  if (!BinMXCAFDoc_NoteDriver::Paste(theSource, theTarget, theRelocTable))
    return Standard_False;

  Handle(XCAFDoc_NoteBinData) aNote = Handle(XCAFDoc_NoteBinData)::DownCast(theTarget);
  if (aNote.IsNull())
    return Standard_False;

  TCollection_ExtendedString aTitle;
  TCollection_AsciiString aMIMEtype;
  Standard_Integer nbSize;
  if (!(theSource >> aTitle >> aMIMEtype >> nbSize))
    return Standard_False;

  Handle(TColStd_HArray1OfByte) aData;
  if (nbSize > 0)
  {
    aData.reset(new TColStd_HArray1OfByte(1, nbSize));
    theSource.GetByteArray(&aData->ChangeFirst(), nbSize);
  }

  aNote->Set(aTitle, aMIMEtype, aData);

  return Standard_True;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
void BinMXCAFDoc_NoteBinDataDriver::Paste(const Handle(TDF_Attribute)& theSource,
					                                BinObjMgt_Persistent&        theTarget,
					                                BinObjMgt_SRelocationTable&  theRelocTable) const
{
  BinMXCAFDoc_NoteDriver::Paste(theSource, theTarget, theRelocTable);

  Handle(XCAFDoc_NoteBinData) aNote = Handle(XCAFDoc_NoteBinData)::DownCast(theSource);
  if (!aNote.IsNull())
  {
    theTarget << aNote->Title() << aNote->MIMEtype() << aNote->Size();
    if (aNote->Size() > 0)
      theTarget.PutByteArray(&aNote->Data()->ChangeFirst(), aNote->Size());
  }
}
