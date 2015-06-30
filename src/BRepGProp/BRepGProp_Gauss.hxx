// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _BRepGProp_Gauss_HeaderFile
#define _BRepGProp_Gauss_HeaderFile

#include <NCollection_Handle.hxx>
#include <NCollection_Array1.hxx>

class math_Vector;

//! Class performs computing of the global inertia properties
//! of geometric object in 3D space by adaptive and non-adaptive
//! 2D Gauss integration algorithms.
class BRepGProp_Gauss
{
  //! Auxiliary structure for storing of inertial moments.
  struct Inertia
  {
    //! Mass of the current system (without density).
    //! May correspond to: length, area, volume.
    Standard_Real Mass;

    //! Static moments of inertia.
    Standard_Real Ix;
    Standard_Real Iy;
    Standard_Real Iz;

    //! Quadratic moments of inertia.
    Standard_Real Ixx;
    Standard_Real Iyy;
    Standard_Real Izz;
    Standard_Real Ixy;
    Standard_Real Ixz;
    Standard_Real Iyz;

    //! Default constructor.
    Inertia();

    //! Zeroes all values.
    void Reset();
  };

  typedef NCollection_Handle< NCollection_Array1<Inertia> > InertiaArray;
  typedef Standard_Real(*BRepGProp_GaussFunc)(const Standard_Real, const Standard_Real);

public: //! @name public API

  //! Describes types of geometric objects.
  //! - Vinert is 3D closed region of space delimited with:
  //! -- Surface;
  //! -- Point and Surface;
  //! -- Plane and Surface.
  //! - Sinert is face in 3D space.
  typedef enum { Vinert = 0, Sinert } BRepGProp_GaussType;

  //! Constructor
  Standard_EXPORT explicit BRepGProp_Gauss(const BRepGProp_GaussType theType);

  //! Computes the global properties of a solid region of 3D space which can be
  //! delimited by the surface and point or surface and plane. Surface can be closed.
  //! The method is quick and its precision is enough for many cases of analytical surfaces.
  //! Non-adaptive 2D Gauss integration with predefined numbers of Gauss points
  //! is used. Numbers of points depend on types of surfaces and curves.
  //! Error of the computation is not calculated.
  //! @param theSurface - bounding surface of the region;
  //! @param theLocation - location of the point or the plane;
  //! @param theCoeff - plane coefficients;
  //! @param theIsByPoint - flag of restricition (point/plane);
  //! @param theOutMass[out] - mass (volume) of region;
  //! @param theOutGravityCenter[out] - garvity center of region;
  //! @param theOutInertia[out] - matrix of inertia;
  Standard_EXPORT void Compute(
    const BRepGProp_Face&  theSurface,
    const gp_Pnt&          theLocation,
    const Standard_Real    theCoeff[],
    const Standard_Boolean theIsByPoint,
    Standard_Real&         theOutMass,
    gp_Pnt&                theOutGravityCenter,
    gp_Mat&                theOutInertia);

  //! Computes the global properties of a surface. Surface can be closed.
  //! The method is quick and its precision is enough for many cases of analytical surfaces.
  //! Non-adaptive 2D Gauss integration with predefined numbers of Gauss points
  //! is used. Numbers of points depend on types of surfaces and curves.
  //! Error of the computation is not calculated.
  //! @param theSurface - bounding surface of the region;
  //! @param theLocation - surface location;
  //! @param theOutMass[out] - mass (volume) of region;
  //! @param theOutGravityCenter[out] - garvity center of region;
  //! @param theOutInertia[out] - matrix of inertia;
  Standard_EXPORT void Compute(
    const BRepGProp_Face&  theSurface,
    const gp_Pnt&          theLocation,
    Standard_Real&         theOutMass,
    gp_Pnt&                theOutGravityCenter,
    gp_Mat&                theOutInertia);

  //! Computes the global properties of a region of 3D space which can be
  //! delimited by the surface and point or surface and plane. Surface can be closed.
  //! The method is quick and its precision is enough for many cases of analytical surfaces.
  //! Non-adaptive 2D Gauss integration with predefined numbers of Gauss points is used.
  //! Numbers of points depend on types of surfaces and curves.
  //! Error of the computation is not calculated.
  //! @param theSurface - bounding surface of the region;
  //! @param theDomain - surface boundings;
  //! @param theLocation - location of the point or the plane;
  //! @param theCoeff - plane coefficients;
  //! @param theIsByPoint - flag of restricition (point/plane);
  //! @param theOutMass[out] - mass (volume) of region;
  //! @param theOutGravityCenter[out] - garvity center of region;
  //! @param theOutInertia[out] - matrix of inertia;
  Standard_EXPORT void Compute(
    BRepGProp_Face&        theSurface,
    BRepGProp_Domain&      theDomain,
    const gp_Pnt&          theLocation,
    const Standard_Real    theCoeff[],
    const Standard_Boolean theIsByPoint,
    Standard_Real&         theOutMass,
    gp_Pnt&                theOutGravityCenter,
    gp_Mat&                theOutInertia);

  //! Computes the global properties of a surface. Surface can be closed.
  //! The method is quick and its precision is enough for many cases of analytical surfaces.
  //! Non-adaptive 2D Gauss integration with predefined numbers of Gauss points
  //! is used. Numbers of points depend on types of surfaces and curves.
  //! Error of the computation is not calculated.
  //! @param theSurface - bounding surface of the region;
  //! @param theDomain - surface boundings;
  //! @param theLocation - surface location;
  //! @param theOutMass[out] - mass (volume) of region;
  //! @param theOutGravityCenter[out] - garvity center of region;
  //! @param theOutInertia[out] - matrix of inertia;
  Standard_EXPORT void Compute(
    BRepGProp_Face&        theSurface,
    BRepGProp_Domain&      theDomain,
    const gp_Pnt&          theLocation,
    Standard_Real&         theOutMass,
    gp_Pnt&                theOutGravityCenter,
    gp_Mat&                theOutInertia);

  //! Computes the global properties of the region of 3D space which can be
  //! delimited by the surface and point or surface and plane.
  //! Adaptive 2D Gauss integration is used.
  //! If Epsilon more than 0.001 then algorithm performs non-adaptive integration.
  //! @param theSurface - bounding surface of the region;
  //! @param theDomain - surface boundings;
  //! @param theLocation - location of the point or the plane;
  //! @param theEps - maximal relative error of computed mass (volume) for face;
  //! @param theCoeff - plane coefficients;
  //! @param theIsByPoint - flag of restricition (point/plane);
  //! @param theOutMass[out] - mass (volume) of region;
  //! @param theOutGravityCenter[out] - garvity center of region;
  //! @param theOutInertia[out] - matrix of inertia;
  //! @return value of error which is calculated as
  //! Abs((M(i+1)-M(i))/M(i+1)), M(i+1) and M(i) are values
  //! for two successive steps of adaptive integration.
  Standard_EXPORT Standard_Real Compute(
    BRepGProp_Face&        theSurface,
    BRepGProp_Domain&      theDomain,
    const gp_Pnt&          theLocation,
    const Standard_Real    theEps,
    const Standard_Real    theCoeff[],
    const Standard_Boolean theByPoint,
    Standard_Real&         theOutMass,
    gp_Pnt&                theOutGravityCenter,
    gp_Mat&                theOutInertia);

  //! Computes the global properties of the face. Adaptive 2D Gauss integration is used.
  //! If Epsilon more than 0.001 then algorithm performs non-adaptive integration.
  //! @param theSurface - bounding surface of the region;
  //! @param theDomain - surface boundings;
  //! @param theLocation - surface location;
  //! @param theEps - maximal relative error of computed mass (square) for face;
  //! @param theOutMass[out] - mass (volume) of region;
  //! @param theOutGravityCenter[out] - garvity center of region;
  //! @param theOutInertia[out] - matrix of inertia;
  //! @return value of error which is calculated as
  //! Abs((M(i+1)-M(i))/M(i+1)), M(i+1) and M(i) are values
  //! for two successive steps of adaptive integration.
  Standard_EXPORT Standard_Real Compute(
    BRepGProp_Face&        theSurface,
    BRepGProp_Domain&      theDomain,
    const gp_Pnt&          theLocation,
    const Standard_Real    theEps,
    Standard_Real&         theOutMass,
    gp_Pnt&                theOutGravityCenter,
    gp_Mat&                theOutInertia);

private: //! @name private methods

  BRepGProp_Gauss(BRepGProp_Gauss const&);
  BRepGProp_Gauss& operator= (BRepGProp_Gauss const&);

  void computeVInertiaOfElementaryPart(
    const gp_Pnt&             thePoint,
    const gp_Vec&             theNormal,
    const gp_Pnt&             theLocation,
    const Standard_Real       theWeight,
    const Standard_Real       theCoeff[],
    const Standard_Boolean    theIsByPoint,
    BRepGProp_Gauss::Inertia& theOutInertia);

  void computeSInertiaOfElementaryPart(
    const gp_Pnt&             thePoint,
    const gp_Vec&             theNormal,
    const gp_Pnt&             theLocation,
    const Standard_Real       theWeight,
    BRepGProp_Gauss::Inertia& theOutInertia);

  void checkBounds(
    const Standard_Real theU1,
    const Standard_Real theU2,
    const Standard_Real theV1,
    const Standard_Real theV2);

  void addAndRestoreInertia(
    const BRepGProp_Gauss::Inertia& theInInertia,
    BRepGProp_Gauss::Inertia&       theOutInertia);

  void multAndRestoreInertia(
    const Standard_Real       theValue,
    BRepGProp_Gauss::Inertia& theInertia);

  void convert(
    const BRepGProp_Gauss::Inertia& theInertia,
    gp_Pnt&                         theOutGravityCenter,
    gp_Mat&                         theOutMatrixOfInertia,
    Standard_Real&                  theOutMass);

  void convert(
    const BRepGProp_Gauss::Inertia& theInertia,
    const Standard_Real             theCoeff[],
    const Standard_Boolean          theIsByPoint,
    gp_Pnt&                         theOutGravityCenter,
    gp_Mat&                         theOutMatrixOfInertia,
    Standard_Real&                  theOutMass);

  static Standard_Integer MaxSubs(
    const Standard_Integer theN,
    const Standard_Integer theCoeff = 32);

  static void Init(
    NCollection_Handle<math_Vector>&         theOutVec,
    const Standard_Real    theValue,
    const Standard_Integer theFirst = 0,
    const Standard_Integer theLast  = 0);

  static void InitMass(
    const Standard_Real    theValue,
    const Standard_Integer theFirst,
    const Standard_Integer theLast,
    InertiaArray&          theArray);

  static Standard_Integer FillIntervalBounds(
    const Standard_Real         theA,
    const Standard_Real         theB,
    const TColStd_Array1OfReal& theKnots,
    const Standard_Integer      theNumSubs,
    InertiaArray&               theInerts,
    NCollection_Handle<math_Vector>&              theParam1,
    NCollection_Handle<math_Vector>&              theParam2,
    NCollection_Handle<math_Vector>&              theError,
    NCollection_Handle<math_Vector>&              theCommonError);

private: //! @name private fields

  BRepGProp_GaussType myType; //!< Type of geometric object
  BRepGProp_GaussFunc add;    //!< Pointer on the add function
  BRepGProp_GaussFunc mult;   //!< Pointer on the mult function
};

#endif