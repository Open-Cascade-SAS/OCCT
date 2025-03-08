// Created on: 2002-08-02
// Created by: Alexander KARTOMIN  (akm)
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _LProp3d_CurveTool_HeaderFile
#define _LProp3d_CurveTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>

class gp_Pnt;
class gp_Vec;

class LProp3d_CurveTool
{
public:
  DEFINE_STANDARD_ALLOC

  //! Computes the point <P> of parameter <U> on the HCurve <C>.
  Standard_EXPORT static void Value(const Handle(Adaptor3d_Curve)& C,
                                    const Standard_Real            U,
                                    gp_Pnt&                        P);

  //! Computes the point <P> and first derivative <V1> of
  //! parameter <U> on the HCurve <C>.
  Standard_EXPORT static void D1(const Handle(Adaptor3d_Curve)& C,
                                 const Standard_Real            U,
                                 gp_Pnt&                        P,
                                 gp_Vec&                        V1);

  //! Computes the point <P>, the first derivative <V1> and second
  //! derivative <V2> of parameter <U> on the HCurve <C>.
  Standard_EXPORT static void D2(const Handle(Adaptor3d_Curve)& C,
                                 const Standard_Real            U,
                                 gp_Pnt&                        P,
                                 gp_Vec&                        V1,
                                 gp_Vec&                        V2);

  //! Computes the point <P>, the first derivative <V1>, the
  //! second derivative <V2> and third derivative <V3> of
  //! parameter <U> on the HCurve <C>.
  Standard_EXPORT static void D3(const Handle(Adaptor3d_Curve)& C,
                                 const Standard_Real            U,
                                 gp_Pnt&                        P,
                                 gp_Vec&                        V1,
                                 gp_Vec&                        V2,
                                 gp_Vec&                        V3);

  //! returns the order of continuity of the HCurve <C>.
  //! returns 1 : first derivative only is computable
  //! returns 2 : first and second derivative only are computable.
  //! returns 3 : first, second and third are computable.
  Standard_EXPORT static Standard_Integer Continuity(const Handle(Adaptor3d_Curve)& C);

  //! returns the first parameter bound of the HCurve.
  Standard_EXPORT static Standard_Real FirstParameter(const Handle(Adaptor3d_Curve)& C);

  //! returns the last parameter bound of the HCurve.
  //! FirstParameter must be less than LastParamenter.
  Standard_EXPORT static Standard_Real LastParameter(const Handle(Adaptor3d_Curve)& C);

protected:
private:
};

#endif // _LProp3d_CurveTool_HeaderFile
