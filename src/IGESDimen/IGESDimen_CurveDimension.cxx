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

#include <IGESDimen_CurveDimension.ixx>
#include <IGESGeom_Line.hxx>
#include <gp_XYZ.hxx>
#include <gp_GTrsf.hxx>


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
