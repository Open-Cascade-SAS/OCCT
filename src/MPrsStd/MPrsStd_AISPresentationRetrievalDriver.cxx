// Created on: 1999-07-08
// Created by: Sergey RUIN
// Copyright (c) 1999-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <MPrsStd_AISPresentationRetrievalDriver.ixx>

#include <PPrsStd_AISPresentation.hxx>
#include <TPrsStd_AISPresentation.hxx>
#include <AIS_Drawer.hxx>
#include <TCollection_ExtendedString.hxx>
#include <PCollection_HExtendedString.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Graphic3d_NameOfMaterial.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : MPrsStd_AISPresentationRetrievalDriver
//purpose  : 
//=======================================================================

MPrsStd_AISPresentationRetrievalDriver::MPrsStd_AISPresentationRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{}

Standard_Integer MPrsStd_AISPresentationRetrievalDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MPrsStd_AISPresentationRetrievalDriver::SourceType() const
{ return STANDARD_TYPE(PPrsStd_AISPresentation); }

Handle(TDF_Attribute) MPrsStd_AISPresentationRetrievalDriver::NewEmpty() const
{ return new TPrsStd_AISPresentation; }

void MPrsStd_AISPresentationRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,
                                                   const Handle(TDF_Attribute)& Target,
                                                   const Handle(MDF_RRelocationTable)& /*RelocTable*/) const
{
  Handle(PPrsStd_AISPresentation) S = Handle(PPrsStd_AISPresentation)::DownCast (Source);
  Handle(TPrsStd_AISPresentation) T = Handle(TPrsStd_AISPresentation)::DownCast (Target);

  T->SetDisplayed( S->IsDisplayed() );  

  TCollection_ExtendedString extstr = (S->GetDriverGUID())->Convert();   
  Standard_GUID guid( extstr.ToExtString() );  
 
  T->SetDriverGUID( guid ); 

  if( S->Color() != -1 ) T->SetColor( (Quantity_NameOfColor)(S->Color()) );
  else T->UnsetColor();

  if( S->Material() != -1 ) T->SetMaterial( (Graphic3d_NameOfMaterial)(S->Material()) );
  else T->UnsetMaterial(); 

  if( S->Transparency() != -1. ) T->SetTransparency( S->Transparency() );
  else T->UnsetTransparency(); 

  if( S->Width() != -1. ) T->SetWidth( S->Width() );
  else T->UnsetWidth(); 

#ifdef DEB
  cout << "AISPresentationRetrievalDriver "  << "retrieved DriverGUID ==> ";
  guid.ShallowDump(cout);
  cout << endl;
#endif
}

