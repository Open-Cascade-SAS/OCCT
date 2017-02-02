// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESBasic_ExternalRefFileIndex.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESBasic_ExternalRefFileIndex,IGESData_IGESEntity)

IGESBasic_ExternalRefFileIndex::IGESBasic_ExternalRefFileIndex ()    {  }

    void  IGESBasic_ExternalRefFileIndex::Init
  (const Handle(Interface_HArray1OfHAsciiString)& aNameArray,
   const Handle(IGESData_HArray1OfIGESEntity)& allEntities)
{
  if (aNameArray->Lower()  != 1 || allEntities->Lower() != 1 ||
      aNameArray->Length() != allEntities->Length())
    throw Standard_DimensionMismatch("IGESBasic_ExternalRefFileIndex: Init");

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
