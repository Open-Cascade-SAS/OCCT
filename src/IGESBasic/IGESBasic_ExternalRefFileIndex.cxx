// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESBasic_ExternalRefFileIndex.ixx>


IGESBasic_ExternalRefFileIndex::IGESBasic_ExternalRefFileIndex ()    {  }

    void  IGESBasic_ExternalRefFileIndex::Init
  (const Handle(Interface_HArray1OfHAsciiString)& aNameArray,
   const Handle(IGESData_HArray1OfIGESEntity)& allEntities)
{
  if (aNameArray->Lower()  != 1 || allEntities->Lower() != 1 ||
      aNameArray->Length() != allEntities->Length())
    Standard_DimensionMismatch::Raise("IGESBasic_ExternalRefFileIndex: Init");

  theNames = aNameArray;
  theEntities = allEntities;
  InitTypeAndForm(402,12);
}

    Standard_Integer  IGESBasic_ExternalRefFileIndex::NbEntries () const
{
  return theNames->Length();
}

    Handle(TCollection_HAsciiString)  IGESBasic_ExternalRefFileIndex::Name
  (const Standard_Integer Index) const
{
  return theNames->Value(Index);
}

    Handle(IGESData_IGESEntity)  IGESBasic_ExternalRefFileIndex::Entity
  (const Standard_Integer Index) const
{
  return theEntities->Value(Index);
}
