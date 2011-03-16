// File:      MDataStd_ExtStringArrayStorageDriver.cxx
// Created:   27.09.04 08:32:39
// Author:    Pavel TELKOV
// Copyright: Open CASCADE  2004


#include <MDataStd_ExtStringArrayStorageDriver.ixx>

#include <CDM_MessageDriver.hxx>
#include <MDataStd.hxx>
#include <PCollection_HExtendedString.hxx>
#include <PDataStd_ExtStringArray.hxx>
#include <PDataStd_ExtStringArray_1.hxx>
#include <TDataStd_ExtStringArray.hxx>

//=======================================================================
//function : MDataStd_ExtStringArrayStorageDriver
//purpose  : Constructor
//=======================================================================

MDataStd_ExtStringArrayStorageDriver::MDataStd_ExtStringArrayStorageDriver
  ( const Handle(CDM_MessageDriver)& theMsgDriver )
 :MDF_ASDriver(theMsgDriver)
{}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDataStd_ExtStringArrayStorageDriver::VersionNumber() const
{ return 0; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDataStd_ExtStringArrayStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(TDataStd_ExtStringArray);
  return sourceType;
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(PDF_Attribute) MDataStd_ExtStringArrayStorageDriver::NewEmpty() const
{
  return new PDataStd_ExtStringArray_1 ();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDataStd_ExtStringArrayStorageDriver::Paste ( 
			      const Handle(TDF_Attribute)&  Source,
			      const Handle(PDF_Attribute)&  Target,
			      const Handle(MDF_SRelocationTable)& ) const
{
  Handle(TDataStd_ExtStringArray)   S = 
    Handle(TDataStd_ExtStringArray)::DownCast (Source);
  Handle(PDataStd_ExtStringArray_1) T = 
    Handle(PDataStd_ExtStringArray_1)::DownCast (Target);
  
  Standard_Integer i, lower = S->Lower(), upper = S->Upper();
  T->Init(lower, upper);
  
  for(i = lower; i<=upper; i++)
  {
    Handle(PCollection_HExtendedString) aPExtStr =
      new PCollection_HExtendedString( S->Value( i ) );
    T->SetValue( i, aPExtStr );
  }
  T->SetDelta(S->GetDelta());
}
