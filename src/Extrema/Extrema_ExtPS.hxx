// Created on: 1994-02-01
// Created by: Laurent PAINNOT
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _Extrema_ExtPS_HeaderFile
#define _Extrema_ExtPS_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Extrema_ExtPElS.hxx>
#include <Extrema_GenExtPS.hxx>
#include <Extrema_SequenceOfPOnSurf.hxx>
#include <gp_Pnt.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <Extrema_ExtFlag.hxx>
#include <Extrema_ExtAlgo.hxx>
#include <Standard_Integer.hxx>
class Extrema_ExtPExtS;
class Extrema_ExtPRevS;
class Adaptor3d_Surface;
class Extrema_POnSurf;


//! It calculates all the extremum distances
//! between a point and a surface.
//! These distances can be minimum or maximum.
class Extrema_ExtPS 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Extrema_ExtPS();
  
  //! It calculates all the distances.
  //! NbU and NbV are used to locate the close points
  //! to find the zeros. They must be great enough
  //! such that if there is N extrema, there will
  //! be N extrema between P and the grid.
  //! TolU et TolV are used to determine the conditions
  //! to stop the iterations; at the iteration number n:
  //! (Un - Un-1) < TolU and (Vn - Vn-1) < TolV .
  Standard_EXPORT Extrema_ExtPS(const gp_Pnt& P, const Adaptor3d_Surface& S, const Standard_Real TolU, const Standard_Real TolV, const Extrema_ExtFlag F = Extrema_ExtFlag_MINMAX, const Extrema_ExtAlgo A = Extrema_ExtAlgo_Grad);
  
  //! It calculates all the distances.
  //! NbU and NbV are used to locate the close points
  //! to find the zeros. They must be great enough
  //! such that if there is N extrema, there will
  //! be N extrema between P and the grid.
  //! TolU et TolV are used to determine the conditions
  //! to stop the iterations; at the iteration number n:
  //! (Un - Un-1) < TolU and (Vn - Vn-1) < TolV .
  Standard_EXPORT Extrema_ExtPS(const gp_Pnt& P, const Adaptor3d_Surface& S, const Standard_Real Uinf, const Standard_Real Usup, const Standard_Real Vinf, const Standard_Real Vsup, const Standard_Real TolU, const Standard_Real TolV, const Extrema_ExtFlag F = Extrema_ExtFlag_MINMAX, const Extrema_ExtAlgo A = Extrema_ExtAlgo_Grad);
  
  //! Initializes the fields of the algorithm.
  Standard_EXPORT void Initialize (const Adaptor3d_Surface& S, const Standard_Real Uinf, const Standard_Real Usup, const Standard_Real Vinf, const Standard_Real Vsup, const Standard_Real TolU, const Standard_Real TolV);
  
  //! Computes the distances.
  //! An exception is raised if the fieds have not been
  //! initialized.
  Standard_EXPORT void Perform (const gp_Pnt& P);
  
  //! Returns True if the distances are found.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns the number of extremum distances.
  Standard_EXPORT Standard_Integer NbExt() const;
  
  //! Returns the value of the Nth resulting square distance.
  Standard_EXPORT Standard_Real SquareDistance (const Standard_Integer N) const;
  
  //! Returns the point of the Nth resulting distance.
  Standard_EXPORT const Extrema_POnSurf& Point (const Standard_Integer N) const;
  
  //! if the surface is a trimmed surface,
  //! dUfVf is a square distance between <P> and the point
  //! of parameter FirstUParameter and FirstVParameter <PUfVf>.
  //! dUfVl is a square distance between <P> and the point
  //! of parameter FirstUParameter and LastVParameter <PUfVl>.
  //! dUlVf is a square distance between <P> and the point
  //! of parameter LastUParameter and FirstVParameter <PUlVf>.
  //! dUlVl is a square distance between <P> and the point
  //! of parameter LastUParameter and LastVParameter <PUlVl>.
  Standard_EXPORT void TrimmedSquareDistances (Standard_Real& dUfVf, Standard_Real& dUfVl, Standard_Real& dUlVf, Standard_Real& dUlVl, gp_Pnt& PUfVf, gp_Pnt& PUfVl, gp_Pnt& PUlVf, gp_Pnt& PUlVl) const;
  
  Standard_EXPORT void SetFlag (const Extrema_ExtFlag F);
  
  Standard_EXPORT void SetAlgo (const Extrema_ExtAlgo A);

private:
  
  Standard_EXPORT void TreatSolution (const Extrema_POnSurf& PS, const Standard_Real Val);

private:

  const Adaptor3d_Surface* myS;
  Standard_Boolean myDone;
  Extrema_ExtPElS myExtPElS;
  Extrema_GenExtPS myExtPS;
  Extrema_SequenceOfPOnSurf myPoints;
  Standard_Real myuinf;
  Standard_Real myusup;
  Standard_Real myvinf;
  Standard_Real myvsup;
  Standard_Real mytolu;
  Standard_Real mytolv;
  Standard_Real d11;
  Standard_Real d12;
  Standard_Real d21;
  Standard_Real d22;
  gp_Pnt P11;
  gp_Pnt P12;
  gp_Pnt P21;
  gp_Pnt P22;
  TColStd_SequenceOfReal mySqDist;
  GeomAbs_SurfaceType mytype;
  Handle(Extrema_ExtPExtS) myExtPExtS;
  Handle(Extrema_ExtPRevS) myExtPRevS;

};

#endif // _Extrema_ExtPS_HeaderFile
