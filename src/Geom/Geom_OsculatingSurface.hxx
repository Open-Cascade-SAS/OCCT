// Created on: 1998-05-05
// Created by: Stepan MISHIN
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _Geom_OsculatingSurface_HeaderFile
#define _Geom_OsculatingSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Geom_HSequenceOfBSplineSurface.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
#include <TColStd_Array1OfBoolean.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <GeomAbs_IsoType.hxx>
#include <Geom_SequenceOfBSplineSurface.hxx>
class Geom_Surface;
class Geom_BSplineSurface;


class Geom_OsculatingSurface;
DEFINE_STANDARD_HANDLE(Geom_OsculatingSurface, Standard_Transient)

class Geom_OsculatingSurface : public Standard_Transient
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Geom_OsculatingSurface();
  
  //! detects if the  surface has punctual U  or  V
  //! isoparametric  curve along on  the bounds of the surface
  //! relativly to the tolerance Tol and Builds the corresponding
  //! osculating surfaces.
  Standard_EXPORT Geom_OsculatingSurface(const Handle(Geom_Surface)& BS, const Standard_Real Tol);
  
  Standard_EXPORT void Init (const Handle(Geom_Surface)& BS, const Standard_Real Tol);
  
  Standard_EXPORT Handle(Geom_Surface) BasisSurface() const;
  
  Standard_EXPORT Standard_Real Tolerance() const;
  
  //! if Standard_True, L is the local osculating surface
  //! along U at the point U,V.
  Standard_EXPORT Standard_Boolean UOscSurf (const Standard_Real U, const Standard_Real V, Standard_Boolean& t, Handle(Geom_BSplineSurface)& L) const;
  
  //! if Standard_True, L is the local osculating surface
  //! along V at the point U,V.
  Standard_EXPORT Standard_Boolean VOscSurf (const Standard_Real U, const Standard_Real V, Standard_Boolean& t, Handle(Geom_BSplineSurface)& L) const;


  DEFINE_STANDARD_RTTIEXT(Geom_OsculatingSurface,Standard_Transient)

protected:





private:

  
  //! returns False if the osculating surface can't be built
  Standard_EXPORT Standard_Boolean BuildOsculatingSurface (const Standard_Real Param, const Standard_Integer UKnot, const Standard_Integer VKnot, const Handle(Geom_BSplineSurface)& BS, Handle(Geom_BSplineSurface)& L) const;
  
  //! returns    True    if  the    isoparametric     is
  //! quasi-punctual
  Standard_EXPORT Standard_Boolean IsQPunctual (const Handle(Geom_Surface)& S, const Standard_Real Param, const GeomAbs_IsoType IT, const Standard_Real TolMin, const Standard_Real TolMax) const;
  
  Standard_EXPORT Standard_Boolean HasOscSurf() const;
  
  Standard_EXPORT Standard_Boolean IsAlongU() const;
  
  Standard_EXPORT Standard_Boolean IsAlongV() const;
  
  Standard_EXPORT void ClearOsculFlags();
  
  Standard_EXPORT const Geom_SequenceOfBSplineSurface& GetSeqOfL1() const;
  
  Standard_EXPORT const Geom_SequenceOfBSplineSurface& GetSeqOfL2() const;


  Handle(Geom_Surface) myBasisSurf;
  Standard_Real myTol;
  Handle(Geom_HSequenceOfBSplineSurface) myOsculSurf1;
  Handle(Geom_HSequenceOfBSplineSurface) myOsculSurf2;
  Handle(TColStd_HSequenceOfInteger) myKdeg;
  TColStd_Array1OfBoolean myAlong;


};







#endif // _Geom_OsculatingSurface_HeaderFile
