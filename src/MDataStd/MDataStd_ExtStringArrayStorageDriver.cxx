// Created on: 2004-09-27
// Created by: Pavel TELKOV
// Copyright (c) 2004-2012 OPEN CASCADE SAS
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
