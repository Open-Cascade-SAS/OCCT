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

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Extrema_HArray2OfPOnSurfParams.hxx>
#include <Extrema_HUBTreeOfSphere.hxx>
#include <Bnd_HArray1OfSphere.hxx>
#include <Extrema_FuncPSNorm.hxx>
#include <Adaptor3d_SurfacePtr.hxx>
#include <Extrema_ExtFlag.hxx>
#include <Extrema_ExtAlgo.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <Extrema_POnSurfParams.hxx>
class StdFail_NotDone;
class Standard_OutOfRange;
class Standard_TypeMismatch;
class gp_Pnt;
class Adaptor3d_Surface;
class Extrema_POnSurf;
class Extrema_POnSurfParams;


//! It calculates all the extremum distances
//! between a point and a surface.
//! These distances can be minimum or maximum.
class Extrema_GenExtPS 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Extrema_GenExtPS();
  
  //! It calculates all the distances.
  //! The function F(u,v)=distance(P,S(u,v)) has an
  //! extremum when gradient(F)=0. The algorithm searchs
  //! all the zeros inside the definition ranges of the
  //! surface.
  //! NbU and NbV are used to locate the close points
  //! to find the zeros. They must be great enough
  //! such that if there is N extrema, there will
  //! be N extrema between P and the grid.
  //! TolU et TolV are used to determine the conditions
  //! to stop the iterations; at the iteration number n:
  //! (Un - Un-1) < TolU and (Vn - Vn-1) < TolV .
  Standard_EXPORT Extrema_GenExtPS(const gp_Pnt& P, const Adaptor3d_Surface& S, const Standard_Integer NbU, const Standard_Integer NbV, const Standard_Real TolU, const Standard_Real TolV, const Extrema_ExtFlag F = Extrema_ExtFlag_MINMAX, const Extrema_ExtAlgo A = Extrema_ExtAlgo_Grad);
  
  //! It calculates all the distances.
  //! The function F(u,v)=distance(P,S(u,v)) has an
  //! extremum when gradient(F)=0. The algorithm searchs
  //! all the zeros inside the definition ranges of the
  //! surface.
  //! NbU and NbV are used to locate the close points
  //! to find the zeros. They must be great enough
  //! such that if there is N extrema, there will
  //! be N extrema between P and the grid.
  //! TolU et TolV are used to determine the conditions
  //! to stop the iterations; at the iteration number n:
  //! (Un - Un-1) < TolU and (Vn - Vn-1) < TolV .
  Standard_EXPORT Extrema_GenExtPS(const gp_Pnt& P, const Adaptor3d_Surface& S, const Standard_Integer NbU, const Standard_Integer NbV, const Standard_Real Umin, const Standard_Real Usup, const Standard_Real Vmin, const Standard_Real Vsup, const Standard_Real TolU, const Standard_Real TolV, const Extrema_ExtFlag F = Extrema_ExtFlag_MINMAX, const Extrema_ExtAlgo A = Extrema_ExtAlgo_Grad);
  
  Standard_EXPORT void Initialize (const Adaptor3d_Surface& S, const Standard_Integer NbU, const Standard_Integer NbV, const Standard_Real TolU, const Standard_Real TolV);
  
  Standard_EXPORT void Initialize (const Adaptor3d_Surface& S, const Standard_Integer NbU, const Standard_Integer NbV, const Standard_Real Umin, const Standard_Real Usup, const Standard_Real Vmin, const Standard_Real Vsup, const Standard_Real TolU, const Standard_Real TolV);
  
  //! the algorithm is done with the point P.
  //! An exception is raised if the fields have not
  //! been initialized.
  Standard_EXPORT void Perform (const gp_Pnt& P);
  
  Standard_EXPORT void SetFlag (const Extrema_ExtFlag F);
  
  Standard_EXPORT void SetAlgo (const Extrema_ExtAlgo A);
  
  //! Returns True if the distances are found.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns the number of extremum distances.
  Standard_EXPORT Standard_Integer NbExt() const;
  
  //! Returns the value of the Nth resulting square distance.
  Standard_EXPORT Standard_Real SquareDistance (const Standard_Integer N) const;
  
  //! Returns the point of the Nth resulting distance.
  Standard_EXPORT const Extrema_POnSurf& Point (const Standard_Integer N) const;




protected:





private:

  
  Standard_EXPORT Adaptor3d_SurfacePtr Bidon() const;
  
  Standard_EXPORT void BuildTree();
  
  Standard_EXPORT void FindSolution (const gp_Pnt& P, const Extrema_POnSurfParams& theParams);
  
  //! Selection of points to build grid, depending on the type of surface
  Standard_EXPORT void GetGridPoints (const Adaptor3d_Surface& theSurf);
  
  //! Creation of grid of parametric points
  Standard_EXPORT void BuildGrid (const gp_Pnt& thePoint);
  
  //! Compute new edge parameters.
  Standard_EXPORT const Extrema_POnSurfParams& ComputeEdgeParameters (const Standard_Boolean IsUEdge, const Extrema_POnSurfParams& theParam0, const Extrema_POnSurfParams& theParam1, const gp_Pnt& thePoints, const Standard_Real theDiffTol);


  Standard_Boolean myDone;
  Standard_Boolean myInit;
  Standard_Real myumin;
  Standard_Real myusup;
  Standard_Real myvmin;
  Standard_Real myvsup;
  Standard_Integer myusample;
  Standard_Integer myvsample;
  Standard_Real mytolu;
  Standard_Real mytolv;
  Handle(Extrema_HArray2OfPOnSurfParams) myPoints;
  Extrema_HUBTreeOfSphere mySphereUBTree;
  Handle(Bnd_HArray1OfSphere) mySphereArray;
  Extrema_FuncPSNorm myF;
  Adaptor3d_SurfacePtr myS;
  Extrema_ExtFlag myFlag;
  Extrema_ExtAlgo myAlgo;
  Handle(TColStd_HArray1OfReal) myUParams;
  Handle(TColStd_HArray1OfReal) myVParams;
  Handle(Extrema_HArray2OfPOnSurfParams) myFacePntParams;
  Handle(Extrema_HArray2OfPOnSurfParams) myUEdgePntParams;
  Handle(Extrema_HArray2OfPOnSurfParams) myVEdgePntParams;
  Extrema_POnSurfParams myGridParam;


};







#endif // _Extrema_GenExtPS_HeaderFile
