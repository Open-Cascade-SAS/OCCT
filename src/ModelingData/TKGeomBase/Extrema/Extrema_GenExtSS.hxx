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

#include <gp_Pnt.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
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

  //! Empty constructor.
  Standard_EXPORT Extrema_GenExtSS();

  //! Destructor.
  Standard_EXPORT ~Extrema_GenExtSS();

  //! It calculates all the distances.
  //! The function F(u,v)=distance(S1(u1,v1),S2(u2,v2)) has an
  //! extremum when gradient(F)=0. The algorithm searches
  //! all the zeros inside the definition ranges of the
  //! surfaces.
  //! NbU and NbV are used to locate the close points
  //! to find the zeros.
  Standard_EXPORT Extrema_GenExtSS(const Adaptor3d_Surface& S1,
                                   const Adaptor3d_Surface& S2,
                                   const int   NbU,
                                   const int   NbV,
                                   const double      Tol1,
                                   const double      Tol2);

  //! It calculates all the distances.
  //! The function F(u,v)=distance(P,S(u,v)) has an
  //! extremum when gradient(F)=0. The algorithm searches
  //! all the zeros inside the definition ranges of the
  //! surface.
  //! NbU and NbV are used to locate the close points
  //! to find the zeros.
  Standard_EXPORT Extrema_GenExtSS(const Adaptor3d_Surface& S1,
                                   const Adaptor3d_Surface& S2,
                                   const int   NbU,
                                   const int   NbV,
                                   const double      U1min,
                                   const double      U1sup,
                                   const double      V1min,
                                   const double      V1sup,
                                   const double      U2min,
                                   const double      U2sup,
                                   const double      V2min,
                                   const double      V2sup,
                                   const double      Tol1,
                                   const double      Tol2);

  Standard_EXPORT void Initialize(const Adaptor3d_Surface& S2,
                                  const int   NbU,
                                  const int   NbV,
                                  const double      Tol2);

  Standard_EXPORT void Initialize(const Adaptor3d_Surface& S2,
                                  const int   NbU,
                                  const int   NbV,
                                  const double      U2min,
                                  const double      U2sup,
                                  const double      V2min,
                                  const double      V2sup,
                                  const double      Tol2);

  //! the algorithm is done with S1
  //! An exception is raised if the fields have not
  //! been initialized.
  Standard_EXPORT void Perform(const Adaptor3d_Surface& S1, const double Tol1);

  //! the algorithm is done withS1
  //! An exception is raised if the fields have not
  //! been initialized.
  Standard_EXPORT void Perform(const Adaptor3d_Surface& S1,
                               const double      U1min,
                               const double      U1sup,
                               const double      V1min,
                               const double      V1sup,
                               const double      Tol1);

  //! Returns True if the distances are found.
  Standard_EXPORT bool IsDone() const;

  //! Returns the number of extremum distances.
  Standard_EXPORT int NbExt() const;

  //! Returns the value of the Nth resulting square distance.
  Standard_EXPORT double SquareDistance(const int N) const;

  //! Returns the point of the Nth resulting distance.
  Standard_EXPORT const Extrema_POnSurf& PointOnS1(const int N) const;

  //! Returns the point of the Nth resulting distance.
  Standard_EXPORT const Extrema_POnSurf& PointOnS2(const int N) const;

private:
  // disallow copies
  Extrema_GenExtSS(const Extrema_GenExtSS&) = delete;
  Extrema_GenExtSS& operator=(const Extrema_GenExtSS&) = delete;

private:
  bool            myDone;
  bool            myInit;
  double               myu1min;
  double               myu1sup;
  double               myv1min;
  double               myv1sup;
  double               myu2min;
  double               myu2sup;
  double               myv2min;
  double               myv2sup;
  int            myusample;
  int            myvsample;
  occ::handle<NCollection_HArray2<gp_Pnt>> mypoints1;
  occ::handle<NCollection_HArray2<gp_Pnt>> mypoints2;
  double               mytol1;
  double               mytol2;
  Extrema_FuncExtSS           myF;
  const Adaptor3d_Surface*    myS2;
};

#endif // _Extrema_GenExtSS_HeaderFile
