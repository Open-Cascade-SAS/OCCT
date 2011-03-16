// File:	MDataStd_IntegerArrayRetrievalDriver_1.cxx
// Created:	Thu Mar 27 19:02:22 2008
// Author:	Sergey ZARITCHNY
//		<sergey.zaritchny@opencascade.com>
// Copyright:   Open CASCADE SA 2008


#include <MDataStd_IntegerArrayRetrievalDriver_1.ixx>
#include <PDataStd_IntegerArray.hxx>
#include <PDataStd_IntegerArray_1.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <MDataStd.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : MDataStd_IntegerArrayRetrievalDriver_1
//purpose  : 
//=======================================================================

MDataStd_IntegerArrayRetrievalDriver_1::MDataStd_IntegerArrayRetrievalDriver_1(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{}


//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDataStd_IntegerArrayRetrievalDriver_1::VersionNumber() const
{ return 1; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDataStd_IntegerArrayRetrievalDriver_1::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PDataStd_IntegerArray_1);
  return sourceType;
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) MDataStd_IntegerArrayRetrievalDriver_1::NewEmpty () const {

  return new TDataStd_IntegerArray ();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDataStd_IntegerArrayRetrievalDriver_1::Paste(const Handle(PDF_Attribute)& Source,
      const Handle(TDF_Attribute)& Target,
      const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PDataStd_IntegerArray_1) S = Handle(PDataStd_IntegerArray_1)::DownCast (Source);
  Handle(TDataStd_IntegerArray) T = Handle(TDataStd_IntegerArray)::DownCast (Target);

  Standard_Integer i, lower = S->Lower() , upper = S->Upper();
  T->Init(lower, upper);
  
  for(i = lower; i<=upper; i++) T->SetValue( i, S->Value(i) );
  T->SetDelta(S->GetDelta());
}

