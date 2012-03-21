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

#include <IGESGraph_UniformRectGrid.ixx>


IGESGraph_UniformRectGrid::IGESGraph_UniformRectGrid ()    {  }

    void IGESGraph_UniformRectGrid::Init
  (const Standard_Integer nbProps,
   const Standard_Integer finite,
   const Standard_Integer line,
   const Standard_Integer weighted,
   const gp_XY&           aGridPoint,
   const gp_XY&           aGridSpacing,
   const Standard_Integer pointsX,
   const Standard_Integer pointsY)
{
  theNbPropertyValues = nbProps;
  isItFinite          = finite;
  isItLine            = line;
  isItWeighted        = weighted;
  theGridPoint        = aGridPoint;
  theGridSpacing      = aGridSpacing;
  theNbPointsX        = pointsX;
  theNbPointsY        = pointsY;
  InitTypeAndForm(406,22);
}

    Standard_Integer IGESGraph_UniformRectGrid::NbPropertyValues () const
{
  return theNbPropertyValues;
}

    Standard_Boolean IGESGraph_UniformRectGrid::IsFinite () const
{
  return (isItFinite == 1);
}

    Standard_Boolean IGESGraph_UniformRectGrid::IsLine () const
{
  return (isItLine == 1);
}

    Standard_Boolean IGESGraph_UniformRectGrid::IsWeighted () const
{
  return (isItWeighted == 0);
}

    gp_Pnt2d IGESGraph_UniformRectGrid::GridPoint () const
{
  return ( gp_Pnt2d(theGridPoint) );
}

    gp_Vec2d IGESGraph_UniformRectGrid::GridSpacing () const
{
  return ( gp_Vec2d(theGridSpacing) );
}

    Standard_Integer IGESGraph_UniformRectGrid::NbPointsX () const
{
  return theNbPointsX;
}

    Standard_Integer IGESGraph_UniformRectGrid::NbPointsY () const
{
  return theNbPointsY;
}
