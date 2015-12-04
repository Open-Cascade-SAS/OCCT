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
#include <gp_Pnt2d.hxx>
#include <gp_XY.hxx>
#include <gp_XYZ.hxx>
#include <IGESDimen_AngularDimension.hxx>
#include <IGESDimen_GeneralNote.hxx>
#include <IGESDimen_LeaderArrow.hxx>
#include <IGESDimen_WitnessLine.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDimen_AngularDimension,IGESData_IGESEntity)

IGESDimen_AngularDimension::IGESDimen_AngularDimension ()    {  }

    void  IGESDimen_AngularDimension::Init
  (const Handle(IGESDimen_GeneralNote)& aNote, 
   const Handle(IGESDimen_WitnessLine)& aLine,
   const Handle(IGESDimen_WitnessLine)& anotherLine,
   const gp_XY& aVertex, const Standard_Real aRadius,
   const Handle(IGESDimen_LeaderArrow)& aLeader,
   const Handle(IGESDimen_LeaderArrow)& anotherLeader)
{
  theNote              = aNote;
  theFirstWitnessLine  = aLine;
  theSecondWitnessLine = anotherLine;
  theVertex            = aVertex;
  theRadius            = aRadius;
  theFirstLeader       = aLeader;
  theSecondLeader      = anotherLeader;
  InitTypeAndForm(202,0);
}

    Handle(IGESDimen_GeneralNote)  IGESDimen_AngularDimension::Note () const 
{
  return theNote;
}

    Standard_Boolean  IGESDimen_AngularDimension::HasFirstWitnessLine () const 
{
  return (! theFirstWitnessLine.IsNull());
}

    Handle(IGESDimen_WitnessLine)  IGESDimen_AngularDimension::FirstWitnessLine
  () const 
{
  return theFirstWitnessLine;
}

    Standard_Boolean  IGESDimen_AngularDimension::HasSecondWitnessLine () const 
{
  return (! theSecondWitnessLine.IsNull());
}

    Handle(IGESDimen_WitnessLine)  IGESDimen_AngularDimension::SecondWitnessLine
  () const 
{
  return theSecondWitnessLine;
}

    gp_Pnt2d  IGESDimen_AngularDimension::Vertex () const 
{
  gp_Pnt2d vertex(theVertex);
  return vertex;
}

    gp_Pnt2d  IGESDimen_AngularDimension::TransformedVertex () const 
{
  gp_XYZ point(theVertex.X(), theVertex.Y(), 0.0);
  if (HasTransf()) Location().Transforms(point);
  return gp_Pnt2d(point.X(), point.Y());
}

    Standard_Real  IGESDimen_AngularDimension::Radius () const 
{
  return theRadius;
}

    Handle(IGESDimen_LeaderArrow)  IGESDimen_AngularDimension::FirstLeader () const 
{
  return theFirstLeader;
}

    Handle(IGESDimen_LeaderArrow)  IGESDimen_AngularDimension::SecondLeader () const 
{
  return theSecondLeader;
}
