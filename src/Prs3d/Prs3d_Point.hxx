// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef _Prs3d_Point_H__
#define _Prs3d_Point_H__


#include <Graphic3d_ArrayOfPoints.hxx>
#include <Graphic3d_Vertex.hxx>
#include <Graphic3d_Group.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_Root.hxx>
#include <Quantity_Length.hxx>

template <class AnyPoint, class PointTool>
class Prs3d_Point : Prs3d_Root
{
public:
  DEFINE_STANDARD_ALLOC

private:
  Standard_EXPORT static void DrawPoint
                 (const AnyPoint& thePoint,
                  const Handle(Graphic3d_Group) theGroup)
  {
    Quantity_Length aX,aY,aZ;
    PointTool::Coord(thePoint,aX,aY,aZ);
    Handle(Graphic3d_ArrayOfPoints) anArrayOfPoints = new Graphic3d_ArrayOfPoints (1);
    anArrayOfPoints->AddVertex (aX, aY, aZ);
    theGroup->AddPrimitiveArray (anArrayOfPoints);
  }

public:
  Standard_EXPORT static void Add
                 (const Handle (Prs3d_Presentation)& thePresentation,
                  const AnyPoint& thePoint,
                  const Handle (Prs3d_Drawer)& theDrawer)
  {
    Handle(Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup(thePresentation);
    aGroup->SetPrimitivesAspect(theDrawer->PointAspect()->Aspect());
    DrawPoint(thePoint, aGroup);
  }

  Standard_EXPORT static Standard_Boolean Match
                  (const AnyPoint& thePoint,
                  const Quantity_Length theX,
                  const Quantity_Length theY,
                  const Quantity_Length theZ,
                  const Quantity_Length theDistance)
  {
    Quantity_Length aX,aY,aZ;
    PointTool::Coord(thePoint, aX, aY, aZ);
    return Sqrt((theX - aX)*(theX - aX) + (theY - aY)*(theY - aY)
                 + (theZ - aZ)*(theZ - aZ)) <= theDistance;
  }
};
#endif
