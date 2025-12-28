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

#include <IGESDimen_GeneralLabel.hxx>
#include <IGESDimen_GeneralNote.hxx>
#include <IGESDimen_LeaderArrow.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDimen_GeneralLabel, IGESData_IGESEntity)

IGESDimen_GeneralLabel::IGESDimen_GeneralLabel() {}

void IGESDimen_GeneralLabel::Init(
  const occ::handle<IGESDimen_GeneralNote>&                                   aNote,
  const occ::handle<NCollection_HArray1<occ::handle<IGESDimen_LeaderArrow>>>& someLeaders)
{
  if (someLeaders->Lower() != 1)
    throw Standard_DimensionMismatch("IGESDimen_GeneralLabel : Init");
  theNote    = aNote;
  theLeaders = someLeaders;
  InitTypeAndForm(210, 0);
}

occ::handle<IGESDimen_GeneralNote> IGESDimen_GeneralLabel::Note() const
{
  return theNote;
}

int IGESDimen_GeneralLabel::NbLeaders() const
{
  return theLeaders->Length();
}

occ::handle<IGESDimen_LeaderArrow> IGESDimen_GeneralLabel::Leader(const int Index) const
{
  return theLeaders->Value(Index);
}
