// File:	BinMPrsStd_AISPresentationDriver.cxx
// Created:	Mon May 17 10:20:29 2004
// Author:	Sergey ZARITCHNY <szy@opencascade.com>
// Copyright:	Open CasCade S.A. 2004


#include <BinMPrsStd_AISPresentationDriver.ixx>
#include <TPrsStd_AISPresentation.hxx>
#include <Graphic3d_NameOfMaterial.hxx>
#include <Quantity_NameOfColor.hxx>
#include <CDM_MessageDriver.hxx>
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
