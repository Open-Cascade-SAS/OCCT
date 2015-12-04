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

#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_XY.hxx>
#include <IGESGraph_UniformRectGrid.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGraph_UniformRectGrid,IGESData_IGESEntity)

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
