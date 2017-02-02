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

#include <IGESAppli_PWBArtworkStackup.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_PWBArtworkStackup,IGESData_IGESEntity)

IGESAppli_PWBArtworkStackup::IGESAppli_PWBArtworkStackup ()    {  }


    void  IGESAppli_PWBArtworkStackup::Init
  (const Standard_Integer nbPropVal,
   const Handle(TCollection_HAsciiString)& anArtIdent,
   const Handle(TColStd_HArray1OfInteger)& allLevelNums)
{
  if (allLevelNums->Lower() != 1)
    throw Standard_DimensionMismatch("IGESAppli_PWBArtworkStackup : Init");
  theNbPropertyValues    = nbPropVal;
  theArtworkStackupIdent = anArtIdent;
  theLevelNumbers        = allLevelNums;
  InitTypeAndForm(406,25);
}

    Standard_Integer  IGESAppli_PWBArtworkStackup::NbPropertyValues () const
{
  return theNbPropertyValues;
}

    Handle(TCollection_HAsciiString)  IGESAppli_PWBArtworkStackup::Identification () const
{
  return theArtworkStackupIdent;
}

    Standard_Integer  IGESAppli_PWBArtworkStackup::NbLevelNumbers () const
{
  return theLevelNumbers->Length();
}

    Standard_Integer  IGESAppli_PWBArtworkStackup::LevelNumber
  (const Standard_Integer Index) const
{
  return theLevelNumbers->Value(Index);
}
