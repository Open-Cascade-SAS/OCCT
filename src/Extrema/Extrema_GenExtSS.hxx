// Created on: 1996-01-18
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

#ifndef _Extrema_GenExtSS_HeaderFile
#define _Extrema_GenExtSS_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColgp_HArray2OfPnt.hxx>
#include <Extrema_FuncExtSS.hxx>

class Adaptor3d_Surface;
class Extrema_POnSurf;

//! It calculates all the extremum distances
//! between two surfaces.
//! These distances can be minimum or maximum.
class Extrema_GenExtSS 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Extrema_GenExtSS();
  
  //! It calculates all the distances.
  //! The function F(u,v)=distance(S1(u1,v1),S2(u2,v2)) has an
  //! extremum when gradient(F)=0. The algorithm searches
  //! all the zeros inside the definition ranges of the
  //! surfaces.
  //! NbU and NbV are used to locate the close points
  //! to find the zeros.
  Standard_EXPORT Extrema_GenExtSS(const Adaptor3d_Surface& S1, const Adaptor3d_Surface& S2, const Standard_Integer NbU, const Standard_Integer NbV, const Standard_Real Tol1, const Standard_Real Tol2);
  
  //! It calculates all the distances.
  //! The function F(u,v)=distance(P,S(u,v)) has an
  //! extremum when gradient(F)=0. The algorithm searches
  //! all the zeros inside the definition ranges of the
  //! surface.
  //! NbU and NbV are used to locate the close points
  //! to find the zeros.
  Standard_EXPORT Extrema_GenExtSS(const Adaptor3d_Surface& S1, const Adaptor3d_Surface& S2, const Standard_Integer NbU, const Standard_Integer NbV, const Standard_Real U1min, const Standard_Real U1sup, const Standard_Real V1min, const Standard_Real V1sup, const Standard_Real U2min, const Standard_Real U2sup, const Standard_Real V2min, const Standard_Real V2sup, const Standard_Real Tol1, const Standard_Real Tol2);
  
  Standard_EXPORT void Initialize (const Adaptor3d_Surface& S2, const Standard_Integer NbU, const Standard_Integer NbV, const Standard_Real Tol2);
  
  Standard_EXPORT void Initialize (const Adaptor3d_Surface& S2, const Standard_Integer NbU, const Standard_Integer NbV, const Standard_Real U2min, const Standard_Real U2sup, const Standard_Real V2min, const Standard_Real V2sup, const Standard_Real Tol2);
  
  //! the algorithm is done with S1
  //! An exception is raised if the fields have not
  //! been initialized.
  Standard_EXPORT void Perform (const Adaptor3d_Surface& S1, const Standard_Real Tol1);
  
  //! the algorithm is done withS1
  //! An exception is raised if the fields have not
  //! been initialized.
  Standard_EXPORT void Perform (const Adaptor3d_Surface& S1, const Standard_Real U1min, const Standard_Real U1sup, const Standard_Real V1min, const Standard_Real V1sup, const Standard_Real Tol1);
  
  //! Returns True if the distances are found.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns the number of extremum distances.
  Standard_EXPORT Standard_Integer NbExt() const;
  
  //! Returns the value of the Nth resulting square distance.
  Standard_EXPORT Standard_Real SquareDistance (const Standard_Integer N) const;
  
  //! Returns the point of the Nth resulting distance.
  Standard_EXPORT const Extrema_POnSurf& PointOnS1 (const Standard_Integer N) const;
  
  //! Returns the point of the Nth resulting distance.
  Standard_EXPORT const Extrema_POnSurf& PointOnS2 (const Standard_Integer N) const;

private:

  // disallow copies
  Extrema_GenExtSS            (const Extrema_GenExtSS& ) Standard_DELETE;
  Extrema_GenExtSS& operator= (const Extrema_GenExtSS& ) Standard_DELETE;

private:

  Standard_Boolean myDone;
  Standard_Boolean myInit;
  Standard_Real myu1min;
  Standard_Real myu1sup;
  Standard_Real myv1min;
  Standard_Real myv1sup;
  Standard_Real myu2min;
  Standard_Real myu2sup;
  Standard_Real myv2min;
  Standard_Real myv2sup;
  Standard_Integer myusample;
  Standard_Integer myvsample;
  Handle(TColgp_HArray2OfPnt) mypoints1;
  Handle(TColgp_HArray2OfPnt) mypoints2;
  Standard_Real mytol1;
  Standard_Real mytol2;
  Extrema_FuncExtSS myF;
  const Adaptor3d_Surface* myS2;

};

#endif // _Extrema_GenExtSS_HeaderFile
