// Created on: 1995-06-06
// Created by: Jean Yves LEBEY
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _BRepApprox_ThePrmPrmSvSurfacesOfApprox_HeaderFile
#define _BRepApprox_ThePrmPrmSvSurfacesOfApprox_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Pnt2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Vec.hxx>
#include <Standard_Boolean.hxx>
#include <BRepApprox_TheInt2SOfThePrmPrmSvSurfacesOfApprox.hxx>
#include <ApproxInt_SvSurfaces.hxx>
#include <Standard_Real.hxx>
class BRepAdaptor_Surface;
class BRepApprox_SurfaceTool;
class BRepApprox_ApproxLine;
class BRepApprox_TheInt2SOfThePrmPrmSvSurfacesOfApprox;
class BRepApprox_TheFunctionOfTheInt2SOfThePrmPrmSvSurfacesOfApprox;
class gp_Pnt;
class gp_Vec;
class gp_Vec2d;



class BRepApprox_ThePrmPrmSvSurfacesOfApprox  : public ApproxInt_SvSurfaces
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepApprox_ThePrmPrmSvSurfacesOfApprox(const BRepAdaptor_Surface& Surf1, const BRepAdaptor_Surface& Surf2);
  
  //! returns True if Tg,Tguv1 Tguv2 can be computed.
  Standard_EXPORT Standard_Boolean Compute (Standard_Real& u1, Standard_Real& v1, Standard_Real& u2, Standard_Real& v2,
                                            gp_Pnt& Pt, gp_Vec& Tg, gp_Vec2d& Tguv1, gp_Vec2d& Tguv2);
  
  Standard_EXPORT void Pnt (const Standard_Real u1, const Standard_Real v1, const Standard_Real u2, const Standard_Real v2, gp_Pnt& P);
  
  Standard_EXPORT Standard_Boolean SeekPoint(const Standard_Real u1,
                                             const Standard_Real v1,
                                             const Standard_Real u2,
                                             const Standard_Real v2,
                                             IntSurf_PntOn2S& Point);
  
  Standard_EXPORT Standard_Boolean Tangency (const Standard_Real u1, const Standard_Real v1, const Standard_Real u2, const Standard_Real v2, gp_Vec& Tg);
  
  Standard_EXPORT Standard_Boolean TangencyOnSurf1 (const Standard_Real u1, const Standard_Real v1, const Standard_Real u2, const Standard_Real v2, gp_Vec2d& Tg);
  
  Standard_EXPORT Standard_Boolean TangencyOnSurf2 (const Standard_Real u1, const Standard_Real v1, const Standard_Real u2, const Standard_Real v2, gp_Vec2d& Tg);




protected:





private:



  gp_Pnt2d MyParOnS1;
  gp_Pnt2d MyParOnS2;
  gp_Pnt MyPnt;
  gp_Vec2d MyTguv1;
  gp_Vec2d MyTguv2;
  gp_Vec MyTg;
  Standard_Boolean MyIsTangent;
  Standard_Boolean MyHasBeenComputed;
  gp_Pnt2d MyParOnS1bis;
  gp_Pnt2d MyParOnS2bis;
  gp_Pnt MyPntbis;
  gp_Vec2d MyTguv1bis;
  gp_Vec2d MyTguv2bis;
  gp_Vec MyTgbis;
  Standard_Boolean MyIsTangentbis;
  Standard_Boolean MyHasBeenComputedbis;
  BRepApprox_TheInt2SOfThePrmPrmSvSurfacesOfApprox MyIntersectionOn2S;


};







#endif // _BRepApprox_ThePrmPrmSvSurfacesOfApprox_HeaderFile
