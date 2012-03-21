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

#include <IGESDimen_AngularDimension.ixx>
#include <gp_XYZ.hxx>
#include <gp_GTrsf.hxx>


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
