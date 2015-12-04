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

#include <gp_GTrsf.hxx>
#include <gp_XYZ.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESDimen_CurveDimension.hxx>
#include <IGESDimen_GeneralNote.hxx>
#include <IGESDimen_LeaderArrow.hxx>
#include <IGESDimen_WitnessLine.hxx>
#include <IGESGeom_Line.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDimen_CurveDimension,IGESData_IGESEntity)

IGESDimen_CurveDimension::IGESDimen_CurveDimension ()    {  }


    void  IGESDimen_CurveDimension::Init
  (const Handle(IGESDimen_GeneralNote)& aNote,
   const Handle(IGESData_IGESEntity)&   aCurve,
   const Handle(IGESData_IGESEntity)&   anotherCurve,
   const Handle(IGESDimen_LeaderArrow)& aLeader,
   const Handle(IGESDimen_LeaderArrow)& anotherLeader,
   const Handle(IGESDimen_WitnessLine)& aLine,
   const Handle(IGESDimen_WitnessLine)& anotherLine)
{
  theNote              = aNote;
  theFirstCurve        = aCurve;
  theSecondCurve       = anotherCurve;
  theFirstLeader       = aLeader;
  theSecondLeader      = anotherLeader;
  theFirstWitnessLine  = aLine;
  theSecondWitnessLine = anotherLine;
  InitTypeAndForm(204,0);
}

    Handle(IGESDimen_GeneralNote)  IGESDimen_CurveDimension::Note () const 
{
  return theNote;
}

    Handle(IGESData_IGESEntity)  IGESDimen_CurveDimension::FirstCurve () const 
{
  return theFirstCurve;
}

    Standard_Boolean  IGESDimen_CurveDimension::HasSecondCurve () const 
{
  return (! theSecondCurve.IsNull());
}

    Handle(IGESData_IGESEntity)  IGESDimen_CurveDimension::SecondCurve () const 
{
  return theSecondCurve;
}

    Handle(IGESDimen_LeaderArrow)  IGESDimen_CurveDimension::FirstLeader () const 
{
  return theFirstLeader;
}

    Handle(IGESDimen_LeaderArrow)  IGESDimen_CurveDimension::SecondLeader () const 
{
  return theSecondLeader;
}

    Standard_Boolean  IGESDimen_CurveDimension::HasFirstWitnessLine () const 
{
  return (! theFirstWitnessLine.IsNull());
}

    Handle(IGESDimen_WitnessLine)  IGESDimen_CurveDimension::FirstWitnessLine
  () const 
{
  return theFirstWitnessLine;
}

    Standard_Boolean  IGESDimen_CurveDimension::HasSecondWitnessLine () const 
{
  return (! theSecondWitnessLine.IsNull());
}

    Handle(IGESDimen_WitnessLine)  IGESDimen_CurveDimension::SecondWitnessLine
  () const 
{
  return theSecondWitnessLine;
}
