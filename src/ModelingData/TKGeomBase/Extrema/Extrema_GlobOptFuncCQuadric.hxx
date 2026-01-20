// Copyright (c) 2020 OPEN CASCADE SAS
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
// commercial license or contractual agreement

#ifndef _Extrema_GlobOptFuncCQuadric_HeaderFile
#define _Extrema_GlobOptFuncCQuadric_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <math_MultipleVarFunction.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <gp_Pln.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Cone.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>

//! This class implements function which calculate square Eucluidean distance
//! between point on surface and nearest point on Conic.
class Extrema_GlobOptFuncCQuadric : public math_MultipleVarFunction
{
public:
  //! Curve and surface should exist during all the lifetime of Extrema_GlobOptFuncCQuadric.
  Standard_EXPORT Extrema_GlobOptFuncCQuadric(const Adaptor3d_Curve* C);

  Standard_EXPORT Extrema_GlobOptFuncCQuadric(const Adaptor3d_Curve* C,
                                              const double    theTf,
                                              const double    theTl);

  Standard_EXPORT Extrema_GlobOptFuncCQuadric(const Adaptor3d_Curve* C, const Adaptor3d_Surface* S);

  Standard_EXPORT void LoadQuad(const Adaptor3d_Surface* S,
                                const double      theUf,
                                const double      theUl,
                                const double      theVf,
                                const double      theVl);

  Standard_EXPORT virtual int NbVariables() const;

  Standard_EXPORT virtual bool Value(const math_Vector& theX, double& theF);
  //! Parameters of quadric for point on curve defined by theCT
  Standard_EXPORT void QuadricParameters(const math_Vector& theCT, math_Vector& theUV) const;

private:
  bool checkInputData(const math_Vector& X, double& ct);

  void value(double ct, double& F);

  const Adaptor3d_Curve*   myC;
  const Adaptor3d_Surface* myS;
  GeomAbs_SurfaceType      mySType;
  gp_Pln                   myPln;
  gp_Cone                  myCone;
  gp_Cylinder              myCylinder;
  gp_Sphere                mySphere;
  gp_Torus                 myTorus;
  gp_Pnt                   myPTrim[4];
  // Boundaries
  double myTf;
  double myTl;
  double myUf;
  double myUl;
  double myVf;
  double myVl;
};

#endif
