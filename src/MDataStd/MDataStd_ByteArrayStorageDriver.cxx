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


#include <MDataStd_ByteArrayStorageDriver.ixx>
#include <PDataStd_ByteArray.hxx>
#include <PDataStd_ByteArray_1.hxx>
#include <TDataStd_ByteArray.hxx>
#include <MDataStd.hxx>
#include <CDM_MessageDriver.hxx>
#include <TColStd_HArray1OfByte.hxx>
#include <PColStd_HArray1OfInteger.hxx>

//=======================================================================
//function : MDataStd_ByteArrayStorageDriver
//purpose  : 
//=======================================================================
MDataStd_ByteArrayStorageDriver::MDataStd_ByteArrayStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{

}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MDataStd_ByteArrayStorageDriver::VersionNumber() const
{ 
  return 0;
}


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MDataStd_ByteArrayStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(TDataStd_ByteArray);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(PDF_Attribute) MDataStd_ByteArrayStorageDriver::NewEmpty() const 
{
  return new PDataStd_ByteArray_1();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MDataStd_ByteArrayStorageDriver::Paste(const Handle(TDF_Attribute)&  Source,
					    const Handle(PDF_Attribute)&   Target,
					    const Handle(MDF_SRelocationTable)& ) const
{
  Handle(TDataStd_ByteArray) S = Handle(TDataStd_ByteArray)::DownCast (Source);
  Handle(PDataStd_ByteArray_1) T = Handle(PDataStd_ByteArray_1)::DownCast (Target);
  
  const Handle(TColStd_HArray1OfByte)& tvalues = S->InternalArray();
  if (tvalues.IsNull())
    return;
  Handle(PColStd_HArray1OfInteger) pvalues = new PColStd_HArray1OfInteger(tvalues->Lower(), tvalues->Upper());
  Standard_Integer i = tvalues->Lower(), upper = tvalues->Upper();
  for (; i <= upper; i++)
  {
    pvalues->SetValue(i, tvalues->Value(i));
  }
  T->Set(pvalues);
  T->SetDelta(S->GetDelta());
}
