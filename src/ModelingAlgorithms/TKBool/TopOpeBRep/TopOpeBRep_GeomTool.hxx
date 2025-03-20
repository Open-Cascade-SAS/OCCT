// Created on: 1993-06-24
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _TopOpeBRep_GeomTool_HeaderFile
#define _TopOpeBRep_GeomTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
class TopOpeBRep_LineInter;
class TopoDS_Shape;
class TopOpeBRepDS_Curve;
class Geom2d_Curve;
class Geom_Curve;

//! Provide services needed by the DSFiller
class TopOpeBRep_GeomTool
{
public:
  DEFINE_STANDARD_ALLOC

  //! Make the  DS curve <C> and the pcurves <PC1,PC2> from
  //! intersection line <L> lying on shapes <S1,S2>. <min,max> = <L> bounds
  Standard_EXPORT static void MakeCurves(const Standard_Real         min,
                                         const Standard_Real         max,
                                         const TopOpeBRep_LineInter& L,
                                         const TopoDS_Shape&         S1,
                                         const TopoDS_Shape&         S2,
                                         TopOpeBRepDS_Curve&         C,
                                         Handle(Geom2d_Curve)&       PC1,
                                         Handle(Geom2d_Curve)&       PC2);

  Standard_EXPORT static void MakeCurve(const Standard_Real         min,
                                        const Standard_Real         max,
                                        const TopOpeBRep_LineInter& L,
                                        Handle(Geom_Curve)&         C);

  Standard_EXPORT static Handle(Geom_Curve) MakeBSpline1fromWALKING3d(
    const TopOpeBRep_LineInter& L);

  Standard_EXPORT static Handle(Geom2d_Curve) MakeBSpline1fromWALKING2d(
    const TopOpeBRep_LineInter& L,
    const Standard_Integer      SI);

protected:
private:
};

#endif // _TopOpeBRep_GeomTool_HeaderFile
