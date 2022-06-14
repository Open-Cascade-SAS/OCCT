// Created on: 1992-08-26
// Created by: Jean Louis FRENKEL
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _Prs3d_HeaderFile
#define _Prs3d_HeaderFile

#include <Graphic3d_ArrayOfPrimitives.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_NListOfSequenceOfPnt.hxx>
#include <Prs3d_Presentation.hxx>

class Poly_Triangulation;

//! The Prs3d package provides the following services
//! -   a presentation object (the context for all
//! modifications to the display, its presentation will be
//! displayed in every view of an active viewer)
//! -   an attribute manager governing how objects such
//! as color, width, and type of line are displayed;
//! these are generic objects, whereas those in
//! StdPrs are specific geometries and topologies.
//! -   generic   algorithms providing default settings for
//! objects such as points, curves, surfaces and shapes
//! -   a root object which provides the abstract
//! framework for the DsgPrs definitions at work in
//! display of dimensions, relations and trihedra.
class Prs3d 
{
public:

  DEFINE_STANDARD_ALLOC
  
  //! draws an arrow at a given location, with respect
  //! to a given direction.
  Standard_EXPORT static Standard_Boolean MatchSegment (const Standard_Real X, const Standard_Real Y, const Standard_Real Z, const Standard_Real aDistance, const gp_Pnt& p1, const gp_Pnt& p2, Standard_Real& dist);

  //! Computes the absolute deflection value based on relative deflection Prs3d_Drawer::DeviationCoefficient().
  //! @param theBndMin [in] bounding box min corner
  //! @param theBndMax [in] bounding box max corner
  //! @param theDeviationCoefficient [in] relative deflection coefficient from Prs3d_Drawer::DeviationCoefficient()
  //! @return absolute deflection coefficient based on bounding box dimensions
  static Standard_Real GetDeflection (const Graphic3d_Vec3d& theBndMin,
                                      const Graphic3d_Vec3d& theBndMax,
                                      const Standard_Real theDeviationCoefficient)
  {
    const Graphic3d_Vec3d aDiag = theBndMax - theBndMin;
    return Max (aDiag.maxComp() * theDeviationCoefficient * 4.0, Precision::Confusion());
  }

  //! Computes the absolute deflection value based on relative deflection Prs3d_Drawer::DeviationCoefficient().
  //! @param theBndBox [in] bounding box
  //! @param theDeviationCoefficient [in] relative deflection coefficient from Prs3d_Drawer::DeviationCoefficient()
  //! @param theMaximalChordialDeviation [in] absolute deflection coefficient from Prs3d_Drawer::MaximalChordialDeviation()
  //! @return absolute deflection coefficient based on bounding box dimensions or theMaximalChordialDeviation if bounding box is Void or Infinite
  static Standard_Real GetDeflection (const Bnd_Box& theBndBox,
                                      const Standard_Real theDeviationCoefficient,
                                      const Standard_Real theMaximalChordialDeviation)
  {
    if (theBndBox.IsVoid())
    {
      return theMaximalChordialDeviation;
    }

    Bnd_Box aBndBox = theBndBox;
    if (theBndBox.IsOpen())
    {
      if (!theBndBox.HasFinitePart())
      {
        return theMaximalChordialDeviation;
      }
      aBndBox = theBndBox.FinitePart();
    }

    Graphic3d_Vec3d aVecMin, aVecMax;
    aBndBox.Get (aVecMin.x(), aVecMin.y(), aVecMin.z(), aVecMax.x(), aVecMax.y(), aVecMax.z());
    return GetDeflection (aVecMin, aVecMax, theDeviationCoefficient);
  }

  //! Assembles array of primitives for sequence of polylines.
  //! @param thePoints [in] the polylines sequence
  //! @return array of primitives
  Standard_EXPORT static Handle(Graphic3d_ArrayOfPrimitives) PrimitivesFromPolylines (const Prs3d_NListOfSequenceOfPnt& thePoints);

  //! Add primitives into new group in presentation and clear the list of polylines.
  Standard_EXPORT static void AddPrimitivesGroup (const Handle(Prs3d_Presentation)& thePrs,
                                                  const Handle(Prs3d_LineAspect)&   theAspect,
                                                  Prs3d_NListOfSequenceOfPnt&       thePolylines);

  //! Add triangulation free edges into sequence of line segments.
  //! @param theSegments [out] sequence of line segments to fill
  //! @param thePolyTri   [in] triangulation to process
  //! @param theLocation  [in] transformation to apply
  Standard_EXPORT static void AddFreeEdges (TColgp_SequenceOfPnt& theSegments,
                                            const Handle(Poly_Triangulation)& thePolyTri,
                                            const gp_Trsf& theLocation);

};

#endif // _Prs3d_HeaderFile
