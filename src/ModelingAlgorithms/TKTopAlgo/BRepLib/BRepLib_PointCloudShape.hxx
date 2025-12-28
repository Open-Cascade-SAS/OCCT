// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef _BRepLib_PointCloudShape_HeaderFile
#define _BRepLib_PointCloudShape_HeaderFile

#include <TopoDS_Shape.hxx>
#include <Standard_Integer.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <Quantity_Color.hxx>
#include <Precision.hxx>

//! This tool is intended to get points from shape with specified distance from shape along normal.
//! Can be used to simulation of points obtained in result of laser scan of shape.
//! There are 2 ways for generation points by shape:
//! 1. Generation points with specified density
//! 2. Generation points using triangulation Nodes
//! Generation of points by density using the GeneratePointsByDensity() function is not thread safe.
class BRepLib_PointCloudShape
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor initialized by shape
  Standard_EXPORT BRepLib_PointCloudShape(const TopoDS_Shape& theShape = TopoDS_Shape(),
                                          const double        theTol   = Precision::Confusion());

  //! Virtual destructor
  Standard_EXPORT virtual ~BRepLib_PointCloudShape();

  //! Return loaded shape.
  const TopoDS_Shape& Shape() const { return myShape; }

  //! Set shape.
  void SetShape(const TopoDS_Shape& theShape) { myShape = theShape; }

  //! Return tolerance.
  double Tolerance() const { return myTol; }

  //! Set tolerance.
  void SetTolerance(double theTol) { myTol = theTol; }

  //! Returns value of the distance to define deflection of points from shape along normal to shape;
  //! 0.0 by default.
  double GetDistance() const { return myDist; }

  //! Sets value of the distance to define deflection of points from shape along normal to shape.
  //! Negative values of theDist parameter are ignored.
  void SetDistance(const double theDist) { myDist = theDist; }

  //! Returns size of the point cloud for specified density.
  Standard_EXPORT int NbPointsByDensity(const double theDensity = 0.0);

  //! Returns size of the point cloud for using triangulation.
  Standard_EXPORT int NbPointsByTriangulation() const;

  //! Computes points with specified density for initial shape.
  //! If parameter Density is equal to 0 then density will be computed automatically by criterion:
  //! - 10 points per minimal unreduced face area.
  //!
  //! Note: this function should not be called from concurrent threads without external lock.
  Standard_EXPORT bool GeneratePointsByDensity(const double theDensity = 0.0);

  //! Get points from triangulation existing in the shape.
  Standard_EXPORT bool GeneratePointsByTriangulation();

protected:
  //! Compute area of the specified face.
  Standard_EXPORT double faceArea(const TopoDS_Shape& theShape);

  //! Computes default density points per face.
  Standard_EXPORT double computeDensity();

  //! Adds points to face in accordance with the specified density randomly in the specified range
  //! [0, Dist].
  Standard_EXPORT bool addDensityPoints(const TopoDS_Shape& theFace);

  //! Adds points to face by nodes of the existing triangulation randomly in the specified range [0,
  //! Dist].
  Standard_EXPORT bool addTriangulationPoints(const TopoDS_Shape& theFace);

protected:
  //! Method to clear maps.
  Standard_EXPORT virtual void clear();

  //! Method to add point, normal to surface in this point and face for which point computed.
  //! @param[in] thePoint 3D point on the surface
  //! @param[in] theNorm  surface normal at this point
  //! @param[in] theUV    surface UV parameters
  //! @param[in] theFace  surface (face) definition
  Standard_EXPORT virtual void addPoint(const gp_Pnt&       thePoint,
                                        const gp_Vec&       theNorm,
                                        const gp_Pnt2d&     theUV,
                                        const TopoDS_Shape& theFace) = 0;

protected:
  TopoDS_Shape                                                       myShape;
  double                                                             myDist;
  double                                                             myTol;
  NCollection_DataMap<TopoDS_Shape, double, TopTools_ShapeMapHasher> myFaceArea;
  NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher>    myFacePoints;
  int                                                                myNbPoints;
};

#endif // _BRepLib_PointCloudShape_HeaderFile
