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


#include <BinMPrsStd_AISPresentationDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <CDM_MessageDriver.hxx>
#include <Graphic3d_NameOfMaterial.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TPrsStd_AISPresentation.hxx>

//=======================================================================
//function : BinMDataStd_AISPresentationDriver
//purpose  : Constructor
//=======================================================================
BinMPrsStd_AISPresentationDriver::BinMPrsStd_AISPresentationDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TPrsStd_AISPresentation)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) BinMPrsStd_AISPresentationDriver::NewEmpty() const
{
  return new TPrsStd_AISPresentation();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================

Standard_Boolean BinMPrsStd_AISPresentationDriver::Paste
                                (const BinObjMgt_Persistent&  theSource,
                                 const Handle(TDF_Attribute)& theTarget,
                                 BinObjMgt_RRelocationTable&  ) const
{
  Handle(TPrsStd_AISPresentation) anAtt = Handle(TPrsStd_AISPresentation)::DownCast(theTarget);
  Standard_Integer aValue;
//Display status
  Standard_Boolean ok = theSource >> aValue;
  if (!ok) return ok;
  anAtt->SetDisplayed((Standard_Boolean)aValue);

//GUID
  Standard_GUID aGUID;
  ok = theSource >> aGUID;
  if (!ok) return ok;
  anAtt->SetDriverGUID(aGUID);

//Color
  ok = theSource >> aValue;
  if (!ok) return ok;
  if(aValue != -1)  anAtt->SetColor( (Quantity_NameOfColor)(aValue) );
  else anAtt->UnsetColor();

//Material
  ok = theSource >> aValue;
  if (!ok) return ok;
  if(aValue != -1)  anAtt->SetMaterial( (Graphic3d_NameOfMaterial)(aValue) );
  else anAtt->UnsetMaterial();

//Transparency
  Standard_Real aRValue;
  ok = theSource >> aRValue;
  if (!ok) return ok;
  if(aRValue != -1.)  anAtt->SetTransparency(aRValue);
  else anAtt->UnsetTransparency(); 

//Width
  ok = theSource >> aRValue;
  if (!ok) return ok;
  if(aRValue != -1.)  anAtt->SetWidth( aRValue );
  else anAtt->UnsetWidth(); 

//Mode
  ok = theSource >> aValue;
  if (!ok) return ok;
  if(aValue != -1)  anAtt->SetMode(aValue);
  else anAtt->UnsetMode();

  return ok;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================

void BinMPrsStd_AISPresentationDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                       BinObjMgt_Persistent&        theTarget,
                                       BinObjMgt_SRelocationTable&  ) const
{
  Handle(TPrsStd_AISPresentation) anAtt = Handle(TPrsStd_AISPresentation)::DownCast(theSource);
//1
  theTarget.PutBoolean(anAtt->IsDisplayed());//Bool
//2
  theTarget.PutGUID(anAtt->GetDriverGUID());//GUID
//3
  if(anAtt->HasOwnColor()) 
    theTarget.PutInteger((Standard_Integer)anAtt->Color());//Color
  else theTarget.PutInteger(-1);
//4
 if(anAtt->HasOwnMaterial())
   theTarget.PutInteger((Standard_Integer)anAtt->Material());
 else theTarget.PutInteger(-1);
//5
  if(anAtt->HasOwnTransparency())
    theTarget.PutReal(anAtt->Transparency()); //Real
  else theTarget.PutReal(-1.);
//6
  if(anAtt->HasOwnWidth())
    theTarget.PutReal(anAtt->Width());// Real
  else theTarget.PutReal(-1.);
//7
  if(anAtt->HasOwnMode())
    theTarget.PutInteger(anAtt->Mode()); //Int
  else theTarget.PutInteger(-1);
}
