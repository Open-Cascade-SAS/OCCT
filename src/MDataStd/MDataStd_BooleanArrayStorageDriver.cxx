// Created on: 2007-05-29
// Created by: Vlad Romashko
// Copyright (c) 2007-2012 OPEN CASCADE SAS
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


#include <MDataStd_BooleanArrayStorageDriver.ixx>
#include <PDataStd_BooleanArray.hxx>
#include <TDataStd_BooleanArray.hxx>
#include <MDataStd.hxx>
#include <CDM_MessageDriver.hxx>
#include <TColStd_HArray1OfByte.hxx>
#include <PColStd_HArray1OfInteger.hxx>

//=======================================================================
//function : MDataStd_BooleanArrayStorageDriver
//purpose  : 
//=======================================================================
MDataStd_BooleanArrayStorageDriver::MDataStd_BooleanArrayStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{

}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MDataStd_BooleanArrayStorageDriver::VersionNumber() const
{ 
  return 0;
}


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MDataStd_BooleanArrayStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(TDataStd_BooleanArray);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(PDF_Attribute) MDataStd_BooleanArrayStorageDriver::NewEmpty() const 
{
  return new PDataStd_BooleanArray();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MDataStd_BooleanArrayStorageDriver::Paste(const Handle(TDF_Attribute)&  Source,
					       const Handle(PDF_Attribute)&   Target,
					       const Handle(MDF_SRelocationTable)& ) const
{
  Handle(TDataStd_BooleanArray) S = Handle(TDataStd_BooleanArray)::DownCast (Source);
  Handle(PDataStd_BooleanArray) T = Handle(PDataStd_BooleanArray)::DownCast (Target);
  
  if (S->Upper() >= S->Lower())
  {
    T->SetLower(S->Lower());
    T->SetUpper(S->Upper());
    const Handle(TColStd_HArray1OfByte)& tvalues = S->InternalArray();
    Handle(PColStd_HArray1OfInteger) pvalues = new PColStd_HArray1OfInteger(tvalues->Lower(), tvalues->Upper());
    Standard_Integer i = tvalues->Lower(), upper = tvalues->Upper();
    for (; i <= upper; i++)
    {
      pvalues->SetValue(i, tvalues->Value(i));
    }
    T->Set(pvalues);
  }
}
