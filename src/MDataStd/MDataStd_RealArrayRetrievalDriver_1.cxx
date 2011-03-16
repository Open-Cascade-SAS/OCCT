// File:	MDataStd_RealArrayRetrievalDriver_1.cxx
// Created:	Fri Mar 28 11:08:47 2008
// Author:	Sergey ZARITCHNY
//		<sergey.zaritchny@opencascade.com>
// Copyright:   Open CASCADE SA 2008


#include <MDataStd_RealArrayRetrievalDriver_1.ixx>

#include <PDataStd_RealArray.hxx>
#include <PDataStd_RealArray_1.hxx>
#include <TDataStd_RealArray.hxx>
#include <MDataStd.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : MDataStd_RealArrayRetrievalDriver_1
//purpose  : 
//=======================================================================

MDataStd_RealArrayRetrievalDriver_1::MDataStd_RealArrayRetrievalDriver_1
  (const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{}


//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDataStd_RealArrayRetrievalDriver_1::VersionNumber() const
{ return 1; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDataStd_RealArrayRetrievalDriver_1::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PDataStd_RealArray_1);
  return sourceType;
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) MDataStd_RealArrayRetrievalDriver_1::NewEmpty () const {

  return new TDataStd_RealArray ();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDataStd_RealArrayRetrievalDriver_1::Paste(const Handle(PDF_Attribute)& Source,
      const Handle(TDF_Attribute)& Target,
      const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PDataStd_RealArray_1) S = Handle(PDataStd_RealArray_1)::DownCast (Source);
  Handle(TDataStd_RealArray)   T = Handle(TDataStd_RealArray)::DownCast (Target);

  Standard_Integer i, lower = S->Lower() , upper = S->Upper();
  T->Init(lower, upper);
  
  for(i = lower; i<=upper; i++) T->SetValue( i, S->Value(i) );
  T->SetDelta(S->GetDelta());
}

