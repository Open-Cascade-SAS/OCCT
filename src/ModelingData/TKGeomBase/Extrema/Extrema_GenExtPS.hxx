// Created on: 1995-07-18
// Created by: Modelistation
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

#ifndef _Extrema_GenExtPS_HeaderFile
#define _Extrema_GenExtPS_HeaderFile

#include <Bnd_Sphere.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Extrema_POnSurfParams.hxx>
#include <NCollection_Array2.hxx>
#include <Extrema_POnSurfParams.hxx>
#include <NCollection_UBTreeFiller.hxx>
#include <NCollection_Handle.hxx>
#include <Bnd_Sphere.hxx>
#include <Extrema_FuncPSNorm.hxx>
#include <Extrema_ExtFlag.hxx>
#include <Extrema_ExtAlgo.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

class Adaptor3d_Surface;

//! It calculates all the extremum distances
//! between a point and a surface.
//! These distances can be minimum or maximum.
class Extrema_GenExtPS
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor.
  Standard_EXPORT Extrema_GenExtPS();

  //! Destructor.
  Standard_EXPORT ~Extrema_GenExtPS();

  //! It calculates all the distances.
  //! The function F(u,v)=distance(P,S(u,v)) has an
  //! extremum when gradient(F)=0. The algorithm searches
  //! all the zeros inside the definition ranges of the
  //! surface.
  //! NbU and NbV are used to locate the close points
  //! to find the zeros. They must be great enough
  //! such that if there is N extrema, there will
  //! be N extrema between P and the grid.
  //! TolU et TolV are used to determine the conditions
  //! to stop the iterations; at the iteration number n:
  //! (Un - Un-1) < TolU and (Vn - Vn-1) < TolV .
  Standard_EXPORT Extrema_GenExtPS(const gp_Pnt&            P,
                                   const Adaptor3d_Surface& S,
                                   const int   NbU,
                                   const int   NbV,
                                   const double      TolU,
                                   const double      TolV,
                                   const Extrema_ExtFlag    F = Extrema_ExtFlag_MINMAX,
                                   const Extrema_ExtAlgo    A = Extrema_ExtAlgo_Grad);

  //! It calculates all the distances.
  //! The function F(u,v)=distance(P,S(u,v)) has an
  //! extremum when gradient(F)=0. The algorithm searches
  //! all the zeros inside the definition ranges of the
  //! surface.
  //! NbU and NbV are used to locate the close points
  //! to find the zeros. They must be great enough
  //! such that if there is N extrema, there will
  //! be N extrema between P and the grid.
  //! TolU et TolV are used to determine the conditions
  //! to stop the iterations; at the iteration number n:
  //! (Un - Un-1) < TolU and (Vn - Vn-1) < TolV .
  Standard_EXPORT Extrema_GenExtPS(const gp_Pnt&            P,
                                   const Adaptor3d_Surface& S,
                                   const int   NbU,
                                   const int   NbV,
                                   const double      Umin,
                                   const double      Usup,
                                   const double      Vmin,
                                   const double      Vsup,
                                   const double      TolU,
                                   const double      TolV,
                                   const Extrema_ExtFlag    F = Extrema_ExtFlag_MINMAX,
                                   const Extrema_ExtAlgo    A = Extrema_ExtAlgo_Grad);

  Standard_EXPORT void Initialize(const Adaptor3d_Surface& S,
                                  const int   NbU,
                                  const int   NbV,
                                  const double      TolU,
                                  const double      TolV);

  Standard_EXPORT void Initialize(const Adaptor3d_Surface& S,
                                  const int   NbU,
                                  const int   NbV,
                                  const double      Umin,
                                  const double      Usup,
                                  const double      Vmin,
                                  const double      Vsup,
                                  const double      TolU,
                                  const double      TolV);

  //! the algorithm is done with the point P.
  //! An exception is raised if the fields have not
  //! been initialized.
  Standard_EXPORT void Perform(const gp_Pnt& P);

  Standard_EXPORT void SetFlag(const Extrema_ExtFlag F);

  Standard_EXPORT void SetAlgo(const Extrema_ExtAlgo A);

  //! Returns True if the distances are found.
  Standard_EXPORT bool IsDone() const;

  //! Returns the number of extremum distances.
  Standard_EXPORT int NbExt() const;

  //! Returns the value of the Nth resulting square distance.
  Standard_EXPORT double SquareDistance(const int N) const;

  //! Returns the point of the Nth resulting distance.
  Standard_EXPORT const Extrema_POnSurf& Point(const int N) const;

private:
  Standard_EXPORT void BuildTree();

  Standard_EXPORT void FindSolution(const gp_Pnt& P, const Extrema_POnSurfParams& theParams);

  //! Selection of points to build grid, depending on the type of surface
  Standard_EXPORT void GetGridPoints(const Adaptor3d_Surface& theSurf);

  //! Creation of grid of parametric points
  Standard_EXPORT void BuildGrid(const gp_Pnt& thePoint);

  //! Compute new edge parameters.
  Standard_EXPORT const Extrema_POnSurfParams& ComputeEdgeParameters(
    const bool       IsUEdge,
    const Extrema_POnSurfParams& theParam0,
    const Extrema_POnSurfParams& theParam1,
    const gp_Pnt&                thePoints,
    const double          theDiffTol);

private:
  // disallow copies
  Extrema_GenExtPS(const Extrema_GenExtPS&) = delete;
  Extrema_GenExtPS& operator=(const Extrema_GenExtPS&) = delete;

private:
  bool myDone;
  bool myInit;
  double    myumin;
  double    myusup;
  double    myvmin;
  double    myvsup;
  int myusample;
  int myvsample;
  double    mytolu;
  double    mytolv;

  NCollection_Array2<Extrema_POnSurfParams> myPoints;
  NCollection_Handle<NCollection_UBTree<int, Bnd_Sphere>>       mySphereUBTree;
  occ::handle<NCollection_HArray1<Bnd_Sphere>>   mySphereArray;
  Extrema_FuncPSNorm            myF;
  const Adaptor3d_Surface*      myS;
  Extrema_ExtFlag               myFlag;
  Extrema_ExtAlgo               myAlgo;
  occ::handle<NCollection_HArray1<double>> myUParams;
  occ::handle<NCollection_HArray1<double>> myVParams;
  NCollection_Array2<Extrema_POnSurfParams> myFacePntParams;
  NCollection_Array2<Extrema_POnSurfParams> myUEdgePntParams;
  NCollection_Array2<Extrema_POnSurfParams> myVEdgePntParams;
  Extrema_POnSurfParams         myGridParam;
};

#endif // _Extrema_GenExtPS_HeaderFile
