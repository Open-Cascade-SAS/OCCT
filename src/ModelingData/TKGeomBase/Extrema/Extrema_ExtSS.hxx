// Created on: 1991-02-21
// Created by: Isabelle GRIGNON
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Extrema_ExtSS_HeaderFile
#define _Extrema_ExtSS_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Extrema_ExtElSS.hxx>
#include <Extrema_POnSurf.hxx>
#include <NCollection_Sequence.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <Standard_Integer.hxx>
class Adaptor3d_Surface;
class Extrema_POnSurf;

//! It calculates all the extremum distances
//! between two surfaces.
//! These distances can be minimum or maximum.
class Extrema_ExtSS
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT Extrema_ExtSS();

  //! It calculates all the distances between S1 and S2.
  Standard_EXPORT Extrema_ExtSS(const Adaptor3d_Surface& S1,
                                const Adaptor3d_Surface& S2,
                                const double             TolS1,
                                const double             TolS2);

  //! It calculates all the distances between S1 and S2.
  Standard_EXPORT Extrema_ExtSS(const Adaptor3d_Surface& S1,
                                const Adaptor3d_Surface& S2,
                                const double             Uinf1,
                                const double             Usup1,
                                const double             Vinf1,
                                const double             Vsup1,
                                const double             Uinf2,
                                const double             Usup2,
                                const double             Vinf2,
                                const double             Vsup2,
                                const double             TolS1,
                                const double             TolS2);

  //! Initializes the fields of the algorithm.
  Standard_EXPORT void Initialize(const Adaptor3d_Surface& S2,
                                  const double             Uinf2,
                                  const double             Usup2,
                                  const double             Vinf2,
                                  const double             Vsup2,
                                  const double             TolS1);

  //! Computes the distances.
  //! An exception is raised if the fields have not been
  //! initialized.
  Standard_EXPORT void Perform(const Adaptor3d_Surface& S1,
                               const double             Uinf1,
                               const double             Usup1,
                               const double             Vinf1,
                               const double             Vsup1,
                               const double             TolS1);

  //! Returns True if the distances are found.
  Standard_EXPORT bool IsDone() const;

  //! Returns True if the surfaces are parallel
  Standard_EXPORT bool IsParallel() const;

  //! Returns the number of extremum distances.
  Standard_EXPORT int NbExt() const;

  //! Returns the value of the Nth resulting square distance.
  Standard_EXPORT double SquareDistance(const int N) const;

  //! Returns the point of the Nth resulting distance.
  Standard_EXPORT void Points(const int N, Extrema_POnSurf& P1, Extrema_POnSurf& P2) const;

private:
  const Adaptor3d_Surface*              myS2;
  bool                                  myDone;
  bool                                  myIsPar;
  Extrema_ExtElSS                       myExtElSS;
  NCollection_Sequence<Extrema_POnSurf> myPOnS1;
  NCollection_Sequence<Extrema_POnSurf> myPOnS2;
  double                                myuinf1;
  double                                myusup1;
  double                                myvinf1;
  double                                myvsup1;
  double                                myuinf2;
  double                                myusup2;
  double                                myvinf2;
  double                                myvsup2;
  double                                mytolS1;
  double                                mytolS2;
  NCollection_Sequence<double>          mySqDist;
  GeomAbs_SurfaceType                   myStype;
};

#endif // _Extrema_ExtSS_HeaderFile
