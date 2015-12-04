// Created on: 2002-10-30
// Created by: Michael SAZONOV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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


#include <BinMDF_ReferenceDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Reference.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDF_ReferenceDriver,BinMDF_ADriver)

//=======================================================================
//function : BinMDF_ReferenceDriver
//purpose  : Constructor
//=======================================================================
BinMDF_ReferenceDriver::BinMDF_ReferenceDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TDF_Reference)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) BinMDF_ReferenceDriver::NewEmpty() const
{
  return new TDF_Reference();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================

Standard_Boolean BinMDF_ReferenceDriver::Paste
                (const BinObjMgt_Persistent&   theSource,
                 const Handle(TDF_Attribute)&  theTarget,
                 BinObjMgt_RRelocationTable&   ) const
{
  Handle(TDF_Reference) aRef = Handle(TDF_Reference)::DownCast(theTarget);

  TDF_Label tLab; // Null label.
  if (!theSource.GetLabel(aRef->Label().Data(), tLab))
    return Standard_False;

  // set referenced label
  aRef->Set(tLab);
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================

void BinMDF_ReferenceDriver::Paste (const Handle(TDF_Attribute)&  theSource,
                                    BinObjMgt_Persistent&         theTarget,
                                    BinObjMgt_SRelocationTable&   ) const
{
  Handle(TDF_Reference) aRef = Handle(TDF_Reference)::DownCast(theSource);
  if (!aRef.IsNull()) {
    const TDF_Label& lab = aRef->Label();
    const TDF_Label& refLab = aRef->Get();
    if (!lab.IsNull() && !refLab.IsNull()) {
      if (lab.IsDescendant(refLab.Root())) {   // Internal reference
        theTarget << refLab;
      }
    }
  }
}
