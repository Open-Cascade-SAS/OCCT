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
#include <Extrema_SequenceOfPOnSurf.hxx>
#include <TColStd_SequenceOfReal.hxx>
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
                                const Standard_Real      TolS1,
                                const Standard_Real      TolS2);

  //! It calculates all the distances between S1 and S2.
  Standard_EXPORT Extrema_ExtSS(const Adaptor3d_Surface& S1,
                                const Adaptor3d_Surface& S2,
                                const Standard_Real      Uinf1,
                                const Standard_Real      Usup1,
                                const Standard_Real      Vinf1,
                                const Standard_Real      Vsup1,
                                const Standard_Real      Uinf2,
                                const Standard_Real      Usup2,
                                const Standard_Real      Vinf2,
                                const Standard_Real      Vsup2,
                                const Standard_Real      TolS1,
                                const Standard_Real      TolS2);

  //! Initializes the fields of the algorithm.
  Standard_EXPORT void Initialize(const Adaptor3d_Surface& S2,
                                  const Standard_Real      Uinf2,
                                  const Standard_Real      Usup2,
                                  const Standard_Real      Vinf2,
                                  const Standard_Real      Vsup2,
                                  const Standard_Real      TolS1);

  //! Computes the distances.
  //! An exception is raised if the fields have not been
  //! initialized.
  Standard_EXPORT void Perform(const Adaptor3d_Surface& S1,
                               const Standard_Real      Uinf1,
                               const Standard_Real      Usup1,
                               const Standard_Real      Vinf1,
                               const Standard_Real      Vsup1,
                               const Standard_Real      TolS1);

  //! Returns True if the distances are found.
  Standard_EXPORT Standard_Boolean IsDone() const;

  //! Returns True if the surfaces are parallel
  Standard_EXPORT Standard_Boolean IsParallel() const;

  //! Returns the number of extremum distances.
  Standard_EXPORT Standard_Integer NbExt() const;

  //! Returns the value of the Nth resulting square distance.
  Standard_EXPORT Standard_Real SquareDistance(const Standard_Integer N) const;

  //! Returns the point of the Nth resulting distance.
  Standard_EXPORT void Points(const Standard_Integer N,
                              Extrema_POnSurf&       P1,
                              Extrema_POnSurf&       P2) const;

private:
  const Adaptor3d_Surface*  myS2;
  Standard_Boolean          myDone;
  Standard_Boolean          myIsPar;
  Extrema_ExtElSS           myExtElSS;
  Extrema_SequenceOfPOnSurf myPOnS1;
  Extrema_SequenceOfPOnSurf myPOnS2;
  Standard_Real             myuinf1;
  Standard_Real             myusup1;
  Standard_Real             myvinf1;
  Standard_Real             myvsup1;
  Standard_Real             myuinf2;
  Standard_Real             myusup2;
  Standard_Real             myvinf2;
  Standard_Real             myvsup2;
  Standard_Real             mytolS1;
  Standard_Real             mytolS2;
  TColStd_SequenceOfReal    mySqDist;
  GeomAbs_SurfaceType       myStype;
};

#endif // _Extrema_ExtSS_HeaderFile
