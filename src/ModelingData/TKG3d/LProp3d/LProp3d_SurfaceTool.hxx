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

#ifndef _LProp3d_SurfaceTool_HeaderFile
#define _LProp3d_SurfaceTool_HeaderFile

#include <Adaptor3d_Surface.hxx>

class LProp3d_SurfaceTool
{
public:
  DEFINE_STANDARD_ALLOC

  //! Computes the point <P> of parameter <U> and <V> on the
  //! HSurface <S>.
  Standard_EXPORT static void Value(const Handle(Adaptor3d_Surface)& S,
                                    const Standard_Real              U,
                                    const Standard_Real              V,
                                    gp_Pnt&                          P);

  //! Computes the point <P> and first derivative <D1*> of
  //! parameter <U> and <V> on the HSurface <S>.
  Standard_EXPORT static void D1(const Handle(Adaptor3d_Surface)& S,
                                 const Standard_Real              U,
                                 const Standard_Real              V,
                                 gp_Pnt&                          P,
                                 gp_Vec&                          D1U,
                                 gp_Vec&                          D1V);

  //! Computes the point <P>, the first derivative <D1*> and second
  //! derivative <D2*> of parameter <U> and <V> on the HSurface <S>.
  Standard_EXPORT static void D2(const Handle(Adaptor3d_Surface)& S,
                                 const Standard_Real              U,
                                 const Standard_Real              V,
                                 gp_Pnt&                          P,
                                 gp_Vec&                          D1U,
                                 gp_Vec&                          D1V,
                                 gp_Vec&                          D2U,
                                 gp_Vec&                          D2V,
                                 gp_Vec&                          DUV);

  Standard_EXPORT static gp_Vec DN(const Handle(Adaptor3d_Surface)& S,
                                   const Standard_Real              U,
                                   const Standard_Real              V,
                                   const Standard_Integer           IU,
                                   const Standard_Integer           IV);

  //! returns the order of continuity of the HSurface <S>.
  //! returns 1 : first derivative only is computable
  //! returns 2 : first and second derivative only are computable.
  Standard_EXPORT static Standard_Integer Continuity(const Handle(Adaptor3d_Surface)& S);

  //! returns the bounds of the HSurface.
  Standard_EXPORT static void Bounds(const Handle(Adaptor3d_Surface)& S,
                                     Standard_Real&                   U1,
                                     Standard_Real&                   V1,
                                     Standard_Real&                   U2,
                                     Standard_Real&                   V2);

protected:
private:
};

#endif // _LProp3d_SurfaceTool_HeaderFile
