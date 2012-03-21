// Created on: 1999-07-08
// Created by: Sergey RUIN
// Copyright (c) 1999-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



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

void MPrsStd_AISPresentationRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,const Handle(TDF_Attribute)& Target,const Handle(MDF_RRelocationTable)& RelocTable) const
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

