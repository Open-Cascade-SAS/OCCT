// File:	MDataStd_ExtStringArrayRetrievalDriver_1.cxx
// Created:	Fri Mar 28 11:27:04 2008
// Author:	Sergey ZARITCHNY
//		<sergey.zaritchny@opencascade.com>
// Copyright:   Open CASCADE SA 2008


#include <MDataStd_ExtStringArrayRetrievalDriver_1.ixx>
#include <CDM_MessageDriver.hxx>
#include <MDataStd.hxx>
#include <PCollection_HExtendedString.hxx>
#include <PDataStd_ExtStringArray.hxx>
#include <PDataStd_ExtStringArray_1.hxx>
#include <TDataStd_ExtStringArray.hxx>

//=======================================================================
//function : MDataStd_ExtStringArrayRetrievalDriver_1
//purpose  : Constructor
//=======================================================================

MDataStd_ExtStringArrayRetrievalDriver_1::MDataStd_ExtStringArrayRetrievalDriver_1
  ( const Handle(CDM_MessageDriver)& theMsgDriver )
 :MDF_ARDriver(theMsgDriver)
{}


//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDataStd_ExtStringArrayRetrievalDriver_1::VersionNumber() const
{ return 1; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDataStd_ExtStringArrayRetrievalDriver_1::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PDataStd_ExtStringArray_1);
  return sourceType;
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) MDataStd_ExtStringArrayRetrievalDriver_1::NewEmpty() const
{
  return new TDataStd_ExtStringArray ();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDataStd_ExtStringArrayRetrievalDriver_1::Paste
  ( const Handle(PDF_Attribute)& Source,
    const Handle(TDF_Attribute)& Target,
    const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PDataStd_ExtStringArray_1) S = 
    Handle(PDataStd_ExtStringArray_1)::DownCast (Source);
  Handle(TDataStd_ExtStringArray)   T = 
    Handle(TDataStd_ExtStringArray)::DownCast (Target);

  Standard_Integer i, lower = S->Lower(), upper = S->Upper();
  T->Init(lower, upper);
  
  for(i = lower; i<=upper; i++)
  {
    Handle(PCollection_HExtendedString) aPExtStr = S->Value( i );
    if ( !aPExtStr.IsNull() )
    T->SetValue( i, aPExtStr->Convert() );
  }

  T->SetDelta(S->GetDelta());
}

