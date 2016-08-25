// Created on: 2004-05-17
// Created by: Sergey ZARITCHNY
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

#include <BinMDataXtd_PresentationDriver.hxx>

#include <TDataXtd_Presentation.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <CDM_MessageDriver.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDataXtd_PresentationDriver,BinMDF_ADriver)

  //=======================================================================
//function : BinMDataStd_AISPresentationDriver
//purpose  : Constructor
//=======================================================================
BinMDataXtd_PresentationDriver::BinMDataXtd_PresentationDriver
                          (const Handle(CDM_MessageDriver)& theMsgDriver)
: BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(TDataXtd_Presentation)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) BinMDataXtd_PresentationDriver::NewEmpty() const
{
  return new TDataXtd_Presentation();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================

Standard_Boolean BinMDataXtd_PresentationDriver::Paste
                                  (const BinObjMgt_Persistent&  theSource,
                                   const Handle(TDF_Attribute)& theTarget,
                                   BinObjMgt_RRelocationTable&  /*theRT*/) const
{
  Standard_Boolean ok = Standard_False;
  Handle(TDataXtd_Presentation) anAttribute = Handle(TDataXtd_Presentation)::DownCast(theTarget);

  // Display status
  Standard_Integer aValue;
  ok = theSource >> aValue;
  if (!ok) return ok;
  anAttribute->SetDisplayed (aValue != 0);

  // GUID
  Standard_GUID aGUID;
  ok = theSource >> aGUID;
  if (!ok) return ok;
  anAttribute->SetDriverGUID(aGUID);

  // Color
  ok = theSource >> aValue;
  if (!ok) return ok;
  if ( aValue != -1 )
    anAttribute->SetColor((Quantity_NameOfColor)aValue);
  else
    anAttribute->UnsetColor();

  // Material
  ok = theSource >> aValue;
  if ( !ok ) return ok;
  if (aValue != -1)
    anAttribute->SetMaterialIndex(aValue);
  else
    anAttribute->UnsetMaterial();

  // Transparency
  Standard_Real aRValue;
  ok = theSource >> aRValue;
  if ( !ok ) return ok;
  if ( aRValue != -1. )
    anAttribute->SetTransparency(aRValue);
  else
    anAttribute->UnsetTransparency();

  // Width
  ok = theSource >> aRValue;
  if ( !ok ) return ok;
  if ( aRValue != -1. )
    anAttribute->SetWidth(aRValue);
  else
    anAttribute->UnsetWidth();

  // Mode
  ok = theSource >> aValue;
  if ( !ok ) return ok;
  if ( aValue != -1 )
    anAttribute->SetMode(aValue);
  else
    anAttribute->UnsetMode();

  return ok;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================

void BinMDataXtd_PresentationDriver::Paste(const Handle(TDF_Attribute)& theSource,
                                           BinObjMgt_Persistent&        theTarget,
                                           BinObjMgt_SRelocationTable&  /*theSRT*/) const
{
  Handle(TDataXtd_Presentation) anAttribute = Handle(TDataXtd_Presentation)::DownCast(theSource);

  // Display status
  theTarget.PutBoolean(anAttribute->IsDisplayed());

  // GUID
  theTarget.PutGUID(anAttribute->GetDriverGUID());

  // Color
  if (anAttribute->HasOwnColor())
    theTarget.PutInteger(anAttribute->Color());
  else
    theTarget.PutInteger(-1);

  // Material
  if (anAttribute->HasOwnMaterial())
    theTarget.PutInteger(anAttribute->MaterialIndex());
  else
    theTarget.PutInteger(-1);

  // Transparency
  if (anAttribute->HasOwnTransparency())
    theTarget.PutReal(anAttribute->Transparency());
  else
    theTarget.PutReal(-1.);

  // Width
  if (anAttribute->HasOwnWidth())
    theTarget.PutReal(anAttribute->Width());
  else
    theTarget.PutReal(-1.);

  // Mode
  if (anAttribute->HasOwnMode())
    theTarget.PutInteger(anAttribute->Mode());
  else
    theTarget.PutInteger(-1);
}
