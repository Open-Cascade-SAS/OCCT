// Created on: 1992-06-04
// Created by: Jacques GOUSSARD
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

#ifndef _Geom2dGcc_CurveTool_HeaderFile
#define _Geom2dGcc_CurveTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Standard_Integer.hxx>
class Geom2dAdaptor_Curve;
class gp_Pnt2d;
class gp_Vec2d;

class Geom2dGcc_CurveTool
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT static Standard_Real FirstParameter(const Geom2dAdaptor_Curve& C);

  Standard_EXPORT static Standard_Real LastParameter(const Geom2dAdaptor_Curve& C);

  Standard_EXPORT static Standard_Real EpsX(const Geom2dAdaptor_Curve& C, const Standard_Real Tol);

  Standard_EXPORT static Standard_Integer NbSamples(const Geom2dAdaptor_Curve& C);

  Standard_EXPORT static gp_Pnt2d Value(const Geom2dAdaptor_Curve& C, const Standard_Real X);

  Standard_EXPORT static void D1(const Geom2dAdaptor_Curve& C,
                                 const Standard_Real        U,
                                 gp_Pnt2d&                  P,
                                 gp_Vec2d&                  T);

  Standard_EXPORT static void D2(const Geom2dAdaptor_Curve& C,
                                 const Standard_Real        U,
                                 gp_Pnt2d&                  P,
                                 gp_Vec2d&                  T,
                                 gp_Vec2d&                  N);

  Standard_EXPORT static void D3(const Geom2dAdaptor_Curve& C,
                                 const Standard_Real        U,
                                 gp_Pnt2d&                  P,
                                 gp_Vec2d&                  T,
                                 gp_Vec2d&                  N,
                                 gp_Vec2d&                  dN);

protected:
private:
};

#endif // _Geom2dGcc_CurveTool_HeaderFile
