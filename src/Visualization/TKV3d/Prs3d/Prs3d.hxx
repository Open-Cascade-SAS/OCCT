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
#include <gp_Pnt.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <NCollection_List.hxx>
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
//! -   generic algorithms providing default settings for
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
  Standard_EXPORT static bool MatchSegment(const double  X,
                                           const double  Y,
                                           const double  Z,
                                           const double  aDistance,
                                           const gp_Pnt& p1,
                                           const gp_Pnt& p2,
                                           double&       dist);

  //! Computes the absolute deflection value based on relative deflection
  //! Prs3d_Drawer::DeviationCoefficient().
  //! @param[in] theBndMin  bounding box min corner
  //! @param[in] theBndMax  bounding box max corner
  //! @param[in] theDeviationCoefficient  relative deflection coefficient from
  //! Prs3d_Drawer::DeviationCoefficient()
  //! @return absolute deflection coefficient based on bounding box dimensions
  static double GetDeflection(const NCollection_Vec3<double>& theBndMin,
                              const NCollection_Vec3<double>& theBndMax,
                              const double                    theDeviationCoefficient)
  {
    const NCollection_Vec3<double> aDiag = theBndMax - theBndMin;
    return (std::max)(aDiag.maxComp() * theDeviationCoefficient * 4.0, Precision::Confusion());
  }

  //! Computes the absolute deflection value based on relative deflection
  //! Prs3d_Drawer::DeviationCoefficient().
  //! @param[in] theBndBox  bounding box
  //! @param[in] theDeviationCoefficient  relative deflection coefficient from
  //! Prs3d_Drawer::DeviationCoefficient()
  //! @param[in] theMaximalChordialDeviation  absolute deflection coefficient from
  //! Prs3d_Drawer::MaximalChordialDeviation()
  //! @return absolute deflection coefficient based on bounding box dimensions or
  //! theMaximalChordialDeviation if bounding box is Void or Infinite
  static double GetDeflection(const Bnd_Box& theBndBox,
                              const double   theDeviationCoefficient,
                              const double   theMaximalChordialDeviation)
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

    NCollection_Vec3<double> aVecMin, aVecMax;
    aBndBox.Get(aVecMin.x(), aVecMin.y(), aVecMin.z(), aVecMax.x(), aVecMax.y(), aVecMax.z());
    return GetDeflection(aVecMin, aVecMax, theDeviationCoefficient);
  }

  //! Assembles array of primitives for sequence of polylines.
  //! @param[in] thePoints  the polylines sequence
  //! @return array of primitives
  Standard_EXPORT static occ::handle<Graphic3d_ArrayOfPrimitives> PrimitivesFromPolylines(
    const NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>>& thePoints);

  //! Add primitives into new group in presentation and clear the list of polylines.
  Standard_EXPORT static void AddPrimitivesGroup(
    const occ::handle<Prs3d_Presentation>&                        thePrs,
    const occ::handle<Prs3d_LineAspect>&                          theAspect,
    NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>>& thePolylines);

  //! Add triangulation free edges into sequence of line segments.
  //! @param[out] theSegments  sequence of line segments to fill
  //! @param[in] thePolyTri    triangulation to process
  //! @param[in] theLocation   transformation to apply
  Standard_EXPORT static void AddFreeEdges(NCollection_Sequence<gp_Pnt>&          theSegments,
                                           const occ::handle<Poly_Triangulation>& thePolyTri,
                                           const gp_Trsf&                         theLocation);
};

#endif // _Prs3d_HeaderFile
