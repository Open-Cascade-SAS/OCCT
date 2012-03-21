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


#include <MDataStd_ReferenceListStorageDriver.ixx>
#include <PDataStd_ReferenceList.hxx>
#include <TDataStd_ReferenceList.hxx>
#include <MDataStd.hxx>
#include <CDM_MessageDriver.hxx>
#include <TDF_ListIteratorOfLabelList.hxx>
#include <TDF_Tool.hxx>
#include <TDF_Label.hxx>
#include <PCollection_HExtendedString.hxx>

//=======================================================================
//function : MDataStd_ReferenceListStorageDriver
//purpose  : 
//=======================================================================
MDataStd_ReferenceListStorageDriver::MDataStd_ReferenceListStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{

}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MDataStd_ReferenceListStorageDriver::VersionNumber() const
{ 
  return 0;
}


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MDataStd_ReferenceListStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(TDataStd_ReferenceList);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(PDF_Attribute) MDataStd_ReferenceListStorageDriver::NewEmpty() const 
{
  return new PDataStd_ReferenceList();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MDataStd_ReferenceListStorageDriver::Paste(const Handle(TDF_Attribute)&  Source,
						const Handle(PDF_Attribute)&        Target,
						const Handle(MDF_SRelocationTable)& RelocTable) const
{
  Handle(TDataStd_ReferenceList) S = Handle(TDataStd_ReferenceList)::DownCast (Source);
  Handle(PDataStd_ReferenceList) T = Handle(PDataStd_ReferenceList)::DownCast (Target);
  
  Standard_Integer lower = 1, upper = S->Extent(), i = lower;
  if (upper >= lower)
  {
    T->Init(lower, upper);
    TDF_ListIteratorOfLabelList itr(S->List());
    for (; itr.More(); itr.Next(), i++) 
    {
      TDF_Label L = itr.Value();
      if (!L.IsNull())
      {
	TCollection_AsciiString tvalue;
	TDF_Tool::Entry(L, tvalue);
	Handle(PCollection_HExtendedString) pvalue = new PCollection_HExtendedString(tvalue);
	T->SetValue(i, pvalue);
      }
    }
  }
}
