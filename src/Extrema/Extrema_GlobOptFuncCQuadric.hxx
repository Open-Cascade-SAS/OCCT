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
  Standard_EXPORT  Extrema_GlobOptFuncCQuadric(const Adaptor3d_Curve *C);

  Standard_EXPORT  Extrema_GlobOptFuncCQuadric(const Adaptor3d_Curve *C, 
                                               const Standard_Real   theTf, 
                                               const Standard_Real   theTl);

  Standard_EXPORT  Extrema_GlobOptFuncCQuadric(const Adaptor3d_Curve   *C,
                                               const Adaptor3d_Surface *S);

  Standard_EXPORT void LoadQuad(const Adaptor3d_Surface *S, 
                                const Standard_Real     theUf, 
                                const Standard_Real     theUl,
                                const Standard_Real     theVf, 
                                const Standard_Real     theVl);

  Standard_EXPORT virtual Standard_Integer NbVariables() const;

  Standard_EXPORT virtual Standard_Boolean Value(const math_Vector &theX,
                                                 Standard_Real     &theF);
  //! Parameters of quadric for point on curve defined by theCT
  Standard_EXPORT void QuadricParameters(const math_Vector& theCT,
                                               math_Vector& theUV) const;

private:

  Standard_Boolean checkInputData(const math_Vector   &X,
                                  Standard_Real       &ct);

  void value(Standard_Real ct,
             Standard_Real &F);


  const Adaptor3d_Curve   *myC;
  const Adaptor3d_Surface *myS;
  GeomAbs_SurfaceType mySType;
  gp_Pln myPln;
  gp_Cone myCone;
  gp_Cylinder myCylinder;
  gp_Sphere mySphere;
  gp_Torus myTorus;
  gp_Pnt myPTrim[4];
  // Boundaries
  Standard_Real myTf;
  Standard_Real myTl;
  Standard_Real myUf;
  Standard_Real myUl;
  Standard_Real myVf;
  Standard_Real myVl;

};

#endif
