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

#include <IGESDimen_GeneralNote.hxx>
#include <IGESDimen_GeneralSymbol.hxx>
#include <IGESDimen_LeaderArrow.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDimen_GeneralSymbol, IGESData_IGESEntity)

IGESDimen_GeneralSymbol::IGESDimen_GeneralSymbol() {}

void IGESDimen_GeneralSymbol::Init(
  const occ::handle<IGESDimen_GeneralNote>&                                   aNote,
  const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>&   allGeoms,
  const occ::handle<NCollection_HArray1<occ::handle<IGESDimen_LeaderArrow>>>& allLeaders)
{
  if (!allGeoms.IsNull() && allGeoms->Lower() != 1)
    throw Standard_DimensionMismatch("IGESDimen_GeneralSymbol : Init");
  if (!allLeaders.IsNull())
    if (allLeaders->Lower() != 1)
      throw Standard_DimensionMismatch("$");
  theNote    = aNote;
  theGeoms   = allGeoms;
  theLeaders = allLeaders;
  InitTypeAndForm(228, FormNumber());
  //  FormNumber specifies the Nature of the Symbol, see G.14 (0-3 or > 5000)
}

void IGESDimen_GeneralSymbol::SetFormNumber(const int form)
{
  if ((form < 0 || form > 3) && form < 5000)
    throw Standard_OutOfRange("IGESDimen_GeneralSymbol : SetFormNumber");
  InitTypeAndForm(228, form);
}

bool IGESDimen_GeneralSymbol::HasNote() const
{
  return (!theNote.IsNull());
}

occ::handle<IGESDimen_GeneralNote> IGESDimen_GeneralSymbol::Note() const
{
  return theNote;
}

int IGESDimen_GeneralSymbol::NbGeomEntities() const
{
  return theGeoms->Length();
}

occ::handle<IGESData_IGESEntity> IGESDimen_GeneralSymbol::GeomEntity(const int Index) const
{
  return theGeoms->Value(Index);
}

int IGESDimen_GeneralSymbol::NbLeaders() const
{
  return (theLeaders.IsNull() ? 0 : theLeaders->Length());
}

occ::handle<IGESDimen_LeaderArrow> IGESDimen_GeneralSymbol::LeaderArrow(const int Index) const
{
  return theLeaders->Value(Index);
}
