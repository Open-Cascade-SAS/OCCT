// Created on: 2014-10-14
// Created by: Anton POLETAEV
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

#ifndef _Prs3d_BndBox_H__
#define _Prs3d_BndBox_H__

#include <Bnd_OBB.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Root.hxx>

//! Tool for computing bounding box presentation.
class Prs3d_BndBox : public Prs3d_Root
{
public:
  //! Computes presentation of a bounding box.
  //! @param[in] thePresentation  the presentation.
  //! @param[in] theBndBox  the bounding box.
  //! @param[in] theDrawer  the drawer.
  Standard_EXPORT static void Add(const Handle(Prs3d_Presentation)& thePresentation,
                                  const Bnd_Box&                    theBndBox,
                                  const Handle(Prs3d_Drawer)&       theDrawer);

  //! Computes presentation of a bounding box.
  //! @param[in] thePresentation  the presentation.
  //! @param[in] theBndBox  the bounding box.
  //! @param[in] theDrawer  the drawer.
  Standard_EXPORT static void Add(const Handle(Prs3d_Presentation)& thePresentation,
                                  const Bnd_OBB&                    theBndBox,
                                  const Handle(Prs3d_Drawer)&       theDrawer);

public:
  //! Create primitive array with line segments for displaying a box.
  //! @param[in] theBox  the box to add
  static Handle(Graphic3d_ArrayOfSegments) FillSegments(const Bnd_OBB& theBox)
  {
    if (theBox.IsVoid())
    {
      return Handle(Graphic3d_ArrayOfSegments)();
    }

    Handle(Graphic3d_ArrayOfSegments) aSegs = new Graphic3d_ArrayOfSegments(8, 12 * 2);
    FillSegments(aSegs, theBox);
    return aSegs;
  }

  //! Create primitive array with line segments for displaying a box.
  //! @param[in] theBox  the box to add
  static Handle(Graphic3d_ArrayOfSegments) FillSegments(const Bnd_Box& theBox)
  {
    if (theBox.IsVoid())
    {
      return Handle(Graphic3d_ArrayOfSegments)();
    }

    Handle(Graphic3d_ArrayOfSegments) aSegs = new Graphic3d_ArrayOfSegments(8, 12 * 2);
    FillSegments(aSegs, theBox);
    return aSegs;
  }

  //! Create primitive array with line segments for displaying a box.
  //! @param[in][out] theSegments   primitive array to be filled;
  //!                               should be at least 8 nodes and 24 edges in size
  //! @param[in] theBox  the box to add
  static void FillSegments(const Handle(Graphic3d_ArrayOfSegments)& theSegments,
                           const Bnd_OBB&                           theBox)
  {
    if (!theBox.IsVoid())
    {
      gp_Pnt aXYZ[8];
      theBox.GetVertex(aXYZ);
      fillSegments(theSegments, aXYZ);
    }
  }

  //! Create primitive array with line segments for displaying a box.
  //! @param[in][out] theSegments   primitive array to be filled;
  //!                               should be at least 8 nodes and 24 edges in size
  //! @param[in] theBox  the box to add
  static void FillSegments(const Handle(Graphic3d_ArrayOfSegments)& theSegments,
                           const Bnd_Box&                           theBox)
  {
    if (!theBox.IsVoid())
    {
      const gp_Pnt aMin    = theBox.CornerMin();
      const gp_Pnt aMax    = theBox.CornerMax();
      const gp_Pnt aXYZ[8] = {
        gp_Pnt(aMin.X(), aMin.Y(), aMin.Z()),
        gp_Pnt(aMax.X(), aMin.Y(), aMin.Z()),
        gp_Pnt(aMin.X(), aMax.Y(), aMin.Z()),
        gp_Pnt(aMax.X(), aMax.Y(), aMin.Z()),
        gp_Pnt(aMin.X(), aMin.Y(), aMax.Z()),
        gp_Pnt(aMax.X(), aMin.Y(), aMax.Z()),
        gp_Pnt(aMin.X(), aMax.Y(), aMax.Z()),
        gp_Pnt(aMax.X(), aMax.Y(), aMax.Z()),
      };
      fillSegments(theSegments, aXYZ);
    }
  }

public:
  //! Create primitive array with line segments for displaying a box.
  //! @param[in][out] theSegments   primitive array to be filled;
  //!                               should be at least 8 nodes and 24 edges in size
  //! @param[in] theBox  the box to add
  static void fillSegments(const Handle(Graphic3d_ArrayOfSegments)& theSegments,
                           const gp_Pnt*                            theBox)
  {
    const Standard_Integer aFrom = theSegments->VertexNumber();
    for (int aVertIter = 0; aVertIter < 8; ++aVertIter)
    {
      theSegments->AddVertex(theBox[aVertIter]);
    }

    theSegments->AddEdges(aFrom + 1, aFrom + 2);
    theSegments->AddEdges(aFrom + 3, aFrom + 4);
    theSegments->AddEdges(aFrom + 5, aFrom + 6);
    theSegments->AddEdges(aFrom + 7, aFrom + 8);
    //
    theSegments->AddEdges(aFrom + 1, aFrom + 3);
    theSegments->AddEdges(aFrom + 2, aFrom + 4);
    theSegments->AddEdges(aFrom + 5, aFrom + 7);
    theSegments->AddEdges(aFrom + 6, aFrom + 8);
    //
    theSegments->AddEdges(aFrom + 1, aFrom + 5);
    theSegments->AddEdges(aFrom + 2, aFrom + 6);
    theSegments->AddEdges(aFrom + 3, aFrom + 7);
    theSegments->AddEdges(aFrom + 4, aFrom + 8);
  }
};

#endif // _Prs3d_BndBox_H__
