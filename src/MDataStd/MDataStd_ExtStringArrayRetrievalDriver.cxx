// File:      MDataStd_ExtStringArrayRetrievalDriver.cxx
// Created:   27.09.04 08:37:09
// Author:    Pavel TELKOV
// Copyright: Open CASCADE  2004


#include <MDataStd_ExtStringArrayRetrievalDriver.ixx>

#include <CDM_MessageDriver.hxx>
#include <MDataStd.hxx>
#include <PCollection_HExtendedString.hxx>
#include <PDataStd_ExtStringArray.hxx>
#include <TDataStd_ExtStringArray.hxx>

//=======================================================================
//function : MDataStd_ExtStringArrayRetrievalDriver
//purpose  : Constructor
//=======================================================================

MDataStd_ExtStringArrayRetrievalDriver::MDataStd_ExtStringArrayRetrievalDriver
  ( const Handle(CDM_MessageDriver)& theMsgDriver )
 :MDF_ARDriver(theMsgDriver)
{}


//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDataStd_ExtStringArrayRetrievalDriver::VersionNumber() const
{ return 0; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDataStd_ExtStringArrayRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PDataStd_ExtStringArray);
  return sourceType;
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) MDataStd_ExtStringArrayRetrievalDriver::NewEmpty() const
{
  return new TDataStd_ExtStringArray ();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDataStd_ExtStringArrayRetrievalDriver::Paste
  ( const Handle(PDF_Attribute)& Source,
    const Handle(TDF_Attribute)& Target,
    const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PDataStd_ExtStringArray) S = Handle(PDataStd_ExtStringArray)::DownCast (Source);
  Handle(TDataStd_ExtStringArray) T = Handle(TDataStd_ExtStringArray)::DownCast (Target);

  Standard_Integer i, lower = S->Lower() , upper = S->Upper();
  T->Init(lower, upper);
  
  for(i = lower; i<=upper; i++)
  {
    Handle(PCollection_HExtendedString) aPExtStr = S->Value( i );
    if ( !aPExtStr.IsNull() )
    T->SetValue( i, aPExtStr->Convert() );
  }
}

