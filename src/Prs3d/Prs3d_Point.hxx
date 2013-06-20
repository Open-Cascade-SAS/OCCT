// Copyright (c) 2013 OPEN CASCADE SAS
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

#ifndef _Prs3d_Point_H__
#define _Prs3d_Point_H__


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
    Graphic3d_Vertex aVertex(aX,aY,aZ);
    theGroup->Marker(aVertex);
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
