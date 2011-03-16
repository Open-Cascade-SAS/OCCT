// File:	MPrsStd_AISPresentationRetrievalDriver.cxx
// Created:	Thu Jul  8 17:25:26 1999
// Author:	Sergey RUIN


#include <MPrsStd_AISPresentationRetrievalDriver_1.ixx>

#include <PPrsStd_AISPresentation_1.hxx>
#include <TPrsStd_AISPresentation.hxx>
#include <AIS_Drawer.hxx>
#include <TCollection_ExtendedString.hxx>
#include <PCollection_HExtendedString.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Graphic3d_NameOfMaterial.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : MPrsStd_AISPresentationRetrievalDriver_1
//purpose  : 
//=======================================================================

MPrsStd_AISPresentationRetrievalDriver_1::MPrsStd_AISPresentationRetrievalDriver_1(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{}

Standard_Integer MPrsStd_AISPresentationRetrievalDriver_1::VersionNumber() const
{ return 1; }

Handle(Standard_Type) MPrsStd_AISPresentationRetrievalDriver_1::SourceType() const
{ return STANDARD_TYPE(PPrsStd_AISPresentation_1); }

Handle(TDF_Attribute) MPrsStd_AISPresentationRetrievalDriver_1::NewEmpty() const
{ return new TPrsStd_AISPresentation; }

void MPrsStd_AISPresentationRetrievalDriver_1::Paste(const Handle(PDF_Attribute)& Source,const Handle(TDF_Attribute)& Target,const Handle(MDF_RRelocationTable)& RelocTable) const
{
  Handle(PPrsStd_AISPresentation_1) S = Handle(PPrsStd_AISPresentation_1)::DownCast (Source);
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

  T->SetMode(S->Mode());
  
#ifdef DEB
  cout << "AISPresentationRetrievalDriver "  << "retrieved DriverGUID ==> ";
  guid.ShallowDump(cout);
  cout << endl;
#endif
}


