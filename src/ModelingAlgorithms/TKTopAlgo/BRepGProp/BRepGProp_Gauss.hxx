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

#include <Standard.hxx>

#include <functional>

class BRepGProp_Domain;
class BRepGProp_Face;
class gp_Mat;
class gp_Pnt;
class gp_Vec;

//! Class performs computing of the global inertia properties
//! of geometric object in 3D space by adaptive and non-adaptive
//! 2D Gauss integration algorithms.
class BRepGProp_Gauss
{
  using GaussFunc = std::function<double(double, double)>;

  //! Auxiliary structure for storing of inertial moments.
  struct Inertia
  {
    //! Mass of the current system (without density).
    //! May correspond to: length, area, volume.
    double Mass = 0.0;

    //! Static moments of inertia.
    double Ix = 0.0;
    double Iy = 0.0;
    double Iz = 0.0;

    //! Quadratic moments of inertia.
    double Ixx = 0.0;
    double Iyy = 0.0;
    double Izz = 0.0;
    double Ixy = 0.0;
    double Ixz = 0.0;
    double Iyz = 0.0;
  };

public: //! @name public API
  //! Describes types of geometric objects.
  //! - Vinert is 3D closed region of space delimited with:
  //! -- Surface;
  //! -- Point and Surface;
  //! -- Plane and Surface.
  //! - Sinert is face in 3D space.
  enum class GaussType
  {
    Vinert = 0,
    Sinert
  };

  //! Constructor
  Standard_EXPORT explicit BRepGProp_Gauss(const GaussType theType);

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
  //! @param[out] theOutMass - mass (volume) of region;
  //! @param[out] theOutGravityCenter - garvity center of region;
  //! @param[out] theOutInertia - matrix of inertia;
  Standard_EXPORT void Compute(const BRepGProp_Face& theSurface,
                               const gp_Pnt&         theLocation,
                               const double          theCoeff[],
                               const bool            theIsByPoint,
                               double&               theOutMass,
                               gp_Pnt&               theOutGravityCenter,
                               gp_Mat&               theOutInertia);

  //! Computes the global properties of a surface. Surface can be closed.
  //! The method is quick and its precision is enough for many cases of analytical surfaces.
  //! Non-adaptive 2D Gauss integration with predefined numbers of Gauss points
  //! is used. Numbers of points depend on types of surfaces and curves.
  //! Error of the computation is not calculated.
  //! @param theSurface - bounding surface of the region;
  //! @param theLocation - surface location;
  //! @param[out] theOutMass - mass (volume) of region;
  //! @param[out] theOutGravityCenter - garvity center of region;
  //! @param[out] theOutInertia - matrix of inertia;
  Standard_EXPORT void Compute(const BRepGProp_Face& theSurface,
                               const gp_Pnt&         theLocation,
                               double&               theOutMass,
                               gp_Pnt&               theOutGravityCenter,
                               gp_Mat&               theOutInertia);

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
  //! @param[out] theOutMass - mass (volume) of region;
  //! @param[out] theOutGravityCenter - garvity center of region;
  //! @param[out] theOutInertia - matrix of inertia;
  Standard_EXPORT void Compute(BRepGProp_Face&   theSurface,
                               BRepGProp_Domain& theDomain,
                               const gp_Pnt&     theLocation,
                               const double      theCoeff[],
                               const bool        theIsByPoint,
                               double&           theOutMass,
                               gp_Pnt&           theOutGravityCenter,
                               gp_Mat&           theOutInertia);

  //! Computes the global properties of a surface. Surface can be closed.
  //! The method is quick and its precision is enough for many cases of analytical surfaces.
  //! Non-adaptive 2D Gauss integration with predefined numbers of Gauss points
  //! is used. Numbers of points depend on types of surfaces and curves.
  //! Error of the computation is not calculated.
  //! @param theSurface - bounding surface of the region;
  //! @param theDomain - surface boundings;
  //! @param theLocation - surface location;
  //! @param[out] theOutMass - mass (volume) of region;
  //! @param[out] theOutGravityCenter - garvity center of region;
  //! @param[out] theOutInertia - matrix of inertia;
  Standard_EXPORT void Compute(BRepGProp_Face&   theSurface,
                               BRepGProp_Domain& theDomain,
                               const gp_Pnt&     theLocation,
                               double&           theOutMass,
                               gp_Pnt&           theOutGravityCenter,
                               gp_Mat&           theOutInertia);

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
  //! @param[out] theOutMass - mass (volume) of region;
  //! @param[out] theOutGravityCenter - garvity center of region;
  //! @param[out] theOutInertia - matrix of inertia;
  //! @return value of error which is calculated as
  //! std::abs((M(i+1)-M(i))/M(i+1)), M(i+1) and M(i) are values
  //! for two successive steps of adaptive integration.
  Standard_EXPORT double Compute(BRepGProp_Face&   theSurface,
                                 BRepGProp_Domain& theDomain,
                                 const gp_Pnt&     theLocation,
                                 const double      theEps,
                                 const double      theCoeff[],
                                 const bool        theByPoint,
                                 double&           theOutMass,
                                 gp_Pnt&           theOutGravityCenter,
                                 gp_Mat&           theOutInertia);

  //! Computes the global properties of the face. Adaptive 2D Gauss integration is used.
  //! If Epsilon more than 0.001 then algorithm performs non-adaptive integration.
  //! @param theSurface - bounding surface of the region;
  //! @param theDomain - surface boundings;
  //! @param theLocation - surface location;
  //! @param theEps - maximal relative error of computed mass (square) for face;
  //! @param[out] theOutMass - mass (volume) of region;
  //! @param[out] theOutGravityCenter - garvity center of region;
  //! @param[out] theOutInertia - matrix of inertia;
  //! @return value of error which is calculated as
  //! std::abs((M(i+1)-M(i))/M(i+1)), M(i+1) and M(i) are values
  //! for two successive steps of adaptive integration.
  Standard_EXPORT double Compute(BRepGProp_Face&   theSurface,
                                 BRepGProp_Domain& theDomain,
                                 const gp_Pnt&     theLocation,
                                 const double      theEps,
                                 double&           theOutMass,
                                 gp_Pnt&           theOutGravityCenter,
                                 gp_Mat&           theOutInertia);

private: //! @name private methods
  BRepGProp_Gauss(BRepGProp_Gauss const&)            = delete;
  BRepGProp_Gauss& operator=(BRepGProp_Gauss const&) = delete;

  void computeVInertiaOfElementaryPart(const gp_Pnt&             thePoint,
                                       const gp_Vec&             theNormal,
                                       const gp_Pnt&             theLocation,
                                       const double              theWeight,
                                       const double              theCoeff[],
                                       const bool                theIsByPoint,
                                       BRepGProp_Gauss::Inertia& theOutInertia);

  void computeSInertiaOfElementaryPart(const gp_Pnt&             thePoint,
                                       const gp_Vec&             theNormal,
                                       const gp_Pnt&             theLocation,
                                       const double              theWeight,
                                       BRepGProp_Gauss::Inertia& theOutInertia);

  void checkBounds(const double theU1, const double theU2, const double theV1, const double theV2);

  void addAndRestoreInertia(const BRepGProp_Gauss::Inertia& theInInertia,
                            BRepGProp_Gauss::Inertia&       theOutInertia);

  void multAndRestoreInertia(const double theValue, BRepGProp_Gauss::Inertia& theInertia);

  void convert(const BRepGProp_Gauss::Inertia& theInertia,
               gp_Pnt&                         theOutGravityCenter,
               gp_Mat&                         theOutMatrixOfInertia,
               double&                         theOutMass);

  void convert(const BRepGProp_Gauss::Inertia& theInertia,
               const double                    theCoeff[],
               const bool                      theIsByPoint,
               gp_Pnt&                         theOutGravityCenter,
               gp_Mat&                         theOutMatrixOfInertia,
               double&                         theOutMass);

private:            //! @name private fields
  GaussType myType; //!< Type of geometric object
  GaussFunc add;    //!< Addition operation
  GaussFunc mult;   //!< Multiplication operation
};

#endif
