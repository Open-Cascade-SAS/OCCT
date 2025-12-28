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

#include <IGESDimen_CurveDimension.hxx>
#include <IGESDimen_GeneralNote.hxx>
#include <IGESDimen_LeaderArrow.hxx>
#include <IGESDimen_WitnessLine.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDimen_CurveDimension, IGESData_IGESEntity)

IGESDimen_CurveDimension::IGESDimen_CurveDimension() {}

void IGESDimen_CurveDimension::Init(const occ::handle<IGESDimen_GeneralNote>& aNote,
                                    const occ::handle<IGESData_IGESEntity>&   aCurve,
                                    const occ::handle<IGESData_IGESEntity>&   anotherCurve,
                                    const occ::handle<IGESDimen_LeaderArrow>& aLeader,
                                    const occ::handle<IGESDimen_LeaderArrow>& anotherLeader,
                                    const occ::handle<IGESDimen_WitnessLine>& aLine,
                                    const occ::handle<IGESDimen_WitnessLine>& anotherLine)
{
  theNote              = aNote;
  theFirstCurve        = aCurve;
  theSecondCurve       = anotherCurve;
  theFirstLeader       = aLeader;
  theSecondLeader      = anotherLeader;
  theFirstWitnessLine  = aLine;
  theSecondWitnessLine = anotherLine;
  InitTypeAndForm(204, 0);
}

occ::handle<IGESDimen_GeneralNote> IGESDimen_CurveDimension::Note() const
{
  return theNote;
}

occ::handle<IGESData_IGESEntity> IGESDimen_CurveDimension::FirstCurve() const
{
  return theFirstCurve;
}

bool IGESDimen_CurveDimension::HasSecondCurve() const
{
  return (!theSecondCurve.IsNull());
}

occ::handle<IGESData_IGESEntity> IGESDimen_CurveDimension::SecondCurve() const
{
  return theSecondCurve;
}

occ::handle<IGESDimen_LeaderArrow> IGESDimen_CurveDimension::FirstLeader() const
{
  return theFirstLeader;
}

occ::handle<IGESDimen_LeaderArrow> IGESDimen_CurveDimension::SecondLeader() const
{
  return theSecondLeader;
}

bool IGESDimen_CurveDimension::HasFirstWitnessLine() const
{
  return (!theFirstWitnessLine.IsNull());
}

occ::handle<IGESDimen_WitnessLine> IGESDimen_CurveDimension::FirstWitnessLine() const
{
  return theFirstWitnessLine;
}

bool IGESDimen_CurveDimension::HasSecondWitnessLine() const
{
  return (!theSecondWitnessLine.IsNull());
}

occ::handle<IGESDimen_WitnessLine> IGESDimen_CurveDimension::SecondWitnessLine() const
{
  return theSecondWitnessLine;
}
