// Created on: 1996-01-23
// Created by: Laurent PAINNOT
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _Extrema_GenExtCS_HeaderFile
#define _Extrema_GenExtCS_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Extrema_FuncExtCS.hxx>
#include <Adaptor3d_SurfacePtr.hxx>
#include <TColgp_HArray2OfPnt.hxx>
#include <Adaptor3d_CurvePtr.hxx>
class StdFail_NotDone;
class Standard_OutOfRange;
class Standard_TypeMismatch;
class Adaptor3d_Curve;
class Adaptor3d_Surface;
class Extrema_POnCurv;
class Extrema_POnSurf;


//! It calculates all the extremum distances
//! between acurve and a surface.
//! These distances can be minimum or maximum.
class Extrema_GenExtCS 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Extrema_GenExtCS();
  
  //! It calculates all the distances.
  //! The function F(u,v)=distance(S1(u1,v1),S2(u2,v2)) has an
  //! extremum when gradient(F)=0. The algorithm searchs
  //! all the zeros inside the definition ranges of the
  //! surfaces.
  //! NbU and NbV are used to locate the close points on the
  //! surface and NbT on the curve to find the zeros.
  Standard_EXPORT Extrema_GenExtCS(const Adaptor3d_Curve& C, const Adaptor3d_Surface& S, const Standard_Integer NbT, const Standard_Integer NbU, const Standard_Integer NbV, const Standard_Real Tol1, const Standard_Real Tol2);
  
  //! It calculates all the distances.
  //! The function F(u,v)=distance(P,S(u,v)) has an
  //! extremum when gradient(F)=0. The algorithm searchs
  //! all the zeros inside the definition ranges of the
  //! surface.
  //! NbT,NbU and NbV are used to locate the close points
  //! to find the zeros.
  Standard_EXPORT Extrema_GenExtCS(const Adaptor3d_Curve& C, const Adaptor3d_Surface& S, const Standard_Integer NbT, const Standard_Integer NbU, const Standard_Integer NbV, const Standard_Real tmin, const Standard_Real tsup, const Standard_Real Umin, const Standard_Real Usup, const Standard_Real Vmin, const Standard_Real Vsup, const Standard_Real Tol1, const Standard_Real Tol2);
  
  Standard_EXPORT void Initialize (const Adaptor3d_Surface& S, const Standard_Integer NbU, const Standard_Integer NbV, const Standard_Real Tol2);
  
  Standard_EXPORT void Initialize (const Adaptor3d_Surface& S, const Standard_Integer NbU, const Standard_Integer NbV, const Standard_Real Umin, const Standard_Real Usup, const Standard_Real Vmin, const Standard_Real Vsup, const Standard_Real Tol2);
  
  //! the algorithm is done with S
  //! An exception is raised if the fields have not
  //! been initialized.
  Standard_EXPORT void Perform (const Adaptor3d_Curve& C, const Standard_Integer NbT, const Standard_Real Tol1);
  
  //! the algorithm is done with C
  //! An exception is raised if the fields have not
  //! been initialized.
  Standard_EXPORT void Perform (const Adaptor3d_Curve& C, const Standard_Integer NbT, const Standard_Real tmin, const Standard_Real tsup, const Standard_Real Tol1);
  
  //! Returns True if the distances are found.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns the number of extremum distances.
  Standard_EXPORT Standard_Integer NbExt() const;
  
  //! Returns the value of the Nth resulting square distance.
  Standard_EXPORT Standard_Real SquareDistance (const Standard_Integer N) const;
  
  //! Returns the point of the Nth resulting distance.
  Standard_EXPORT const Extrema_POnCurv& PointOnCurve (const Standard_Integer N) const;
  
  //! Returns the point of the Nth resulting distance.
  Standard_EXPORT const Extrema_POnSurf& PointOnSurface (const Standard_Integer N) const;




protected:





private:

  
  Standard_EXPORT Adaptor3d_CurvePtr BidonCurve() const;
  
  Standard_EXPORT Adaptor3d_SurfacePtr BidonSurface() const;


  Standard_Boolean myDone;
  Standard_Boolean myInit;
  Standard_Real mytmin;
  Standard_Real mytsup;
  Standard_Real myumin;
  Standard_Real myusup;
  Standard_Real myvmin;
  Standard_Real myvsup;
  Standard_Integer mytsample;
  Standard_Integer myusample;
  Standard_Integer myvsample;
  Standard_Real mytol1;
  Standard_Real mytol2;
  Extrema_FuncExtCS myF;
  Adaptor3d_SurfacePtr myS;
  Handle(TColgp_HArray2OfPnt) mySurfPnts;


};







#endif // _Extrema_GenExtCS_HeaderFile
