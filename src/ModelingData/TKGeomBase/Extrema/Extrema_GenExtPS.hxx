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

#include <BVH_BoxSet.hxx>
#include <Extrema_Array2OfPOnSurfParams.hxx>
#include <Extrema_POnSurfParams.hxx>
#include <Extrema_FuncPSNorm.hxx>
#include <Extrema_ExtFlag.hxx>
#include <Extrema_ExtAlgo.hxx>
#include <TColStd_HArray1OfReal.hxx>

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
  //! The function F(u,v)=distance(P,S(u,v)) has an extremum when gradient(F)=0.
  //! The algorithm searches all the zeros inside the definition ranges of the surface.
  //! @param[in] theP     query point
  //! @param[in] theS     surface to project onto
  //! @param[in] theNbU   number of sample points in U direction
  //! @param[in] theNbV   number of sample points in V direction
  //! @param[in] theTolU  tolerance in U direction for iteration convergence
  //! @param[in] theTolV  tolerance in V direction for iteration convergence
  //! @param[in] theF     extrema search flag (MIN, MAX, or MINMAX)
  //! @param[in] theA     algorithm type (Grad or Tree)
  Standard_EXPORT Extrema_GenExtPS(const gp_Pnt&            theP,
                                   const Adaptor3d_Surface& theS,
                                   const Standard_Integer   theNbU,
                                   const Standard_Integer   theNbV,
                                   const Standard_Real      theTolU,
                                   const Standard_Real      theTolV,
                                   const Extrema_ExtFlag    theF = Extrema_ExtFlag_MINMAX,
                                   const Extrema_ExtAlgo    theA = Extrema_ExtAlgo_Grad);

  //! It calculates all the distances with explicit parameter bounds.
  //! @param[in] theP     query point
  //! @param[in] theS     surface to project onto
  //! @param[in] theNbU   number of sample points in U direction
  //! @param[in] theNbV   number of sample points in V direction
  //! @param[in] theUMin  minimum U parameter
  //! @param[in] theUSup  maximum U parameter
  //! @param[in] theVMin  minimum V parameter
  //! @param[in] theVSup  maximum V parameter
  //! @param[in] theTolU  tolerance in U direction for iteration convergence
  //! @param[in] theTolV  tolerance in V direction for iteration convergence
  //! @param[in] theF     extrema search flag (MIN, MAX, or MINMAX)
  //! @param[in] theA     algorithm type (Grad or Tree)
  Standard_EXPORT Extrema_GenExtPS(const gp_Pnt&            theP,
                                   const Adaptor3d_Surface& theS,
                                   const Standard_Integer   theNbU,
                                   const Standard_Integer   theNbV,
                                   const Standard_Real      theUMin,
                                   const Standard_Real      theUSup,
                                   const Standard_Real      theVMin,
                                   const Standard_Real      theVSup,
                                   const Standard_Real      theTolU,
                                   const Standard_Real      theTolV,
                                   const Extrema_ExtFlag    theF = Extrema_ExtFlag_MINMAX,
                                   const Extrema_ExtAlgo    theA = Extrema_ExtAlgo_Grad);

  //! Initialize the algorithm with surface and sampling parameters.
  //! Uses full surface parameter range.
  //! @param[in] theS     surface to project onto
  //! @param[in] theNbU   number of sample points in U direction
  //! @param[in] theNbV   number of sample points in V direction
  //! @param[in] theTolU  tolerance in U direction
  //! @param[in] theTolV  tolerance in V direction
  Standard_EXPORT void Initialize(const Adaptor3d_Surface& theS,
                                  const Standard_Integer   theNbU,
                                  const Standard_Integer   theNbV,
                                  const Standard_Real      theTolU,
                                  const Standard_Real      theTolV);

  //! Initialize the algorithm with surface, bounds, and sampling parameters.
  //! @param[in] theS     surface to project onto
  //! @param[in] theNbU   number of sample points in U direction
  //! @param[in] theNbV   number of sample points in V direction
  //! @param[in] theUMin  minimum U parameter
  //! @param[in] theUSup  maximum U parameter
  //! @param[in] theVMin  minimum V parameter
  //! @param[in] theVSup  maximum V parameter
  //! @param[in] theTolU  tolerance in U direction
  //! @param[in] theTolV  tolerance in V direction
  Standard_EXPORT void Initialize(const Adaptor3d_Surface& theS,
                                  const Standard_Integer   theNbU,
                                  const Standard_Integer   theNbV,
                                  const Standard_Real      theUMin,
                                  const Standard_Real      theUSup,
                                  const Standard_Real      theVMin,
                                  const Standard_Real      theVSup,
                                  const Standard_Real      theTolU,
                                  const Standard_Real      theTolV);

  //! Performs the extrema computation for the given point.
  //! @param[in] theP query point
  //! @throws Standard_Failure if not initialized
  Standard_EXPORT void Perform(const gp_Pnt& theP);

  //! Sets the extrema search flag (MIN, MAX, or MINMAX).
  //! @param[in] theF extrema search flag
  Standard_EXPORT void SetFlag(const Extrema_ExtFlag theF);

  //! Sets the algorithm type (Grad or Tree).
  //! @param[in] theA algorithm type
  Standard_EXPORT void SetAlgo(const Extrema_ExtAlgo theA);

  //! Returns true if the computation was successful.
  Standard_EXPORT Standard_Boolean IsDone() const;

  //! Returns the number of extremum distances found.
  Standard_EXPORT Standard_Integer NbExt() const;

  //! Returns the value of the Nth resulting square distance.
  //! @param[in] theN index of the extremum (1..NbExt())
  Standard_EXPORT Standard_Real SquareDistance(const Standard_Integer theN) const;

  //! Returns the point of the Nth resulting distance.
  //! @param[in] theN index of the extremum (1..NbExt())
  Standard_EXPORT const Extrema_POnSurf& Point(const Standard_Integer theN) const;

public:
  //! UV index pair for BVH element identification.
  using UVIndex = std::pair<Standard_Integer, Standard_Integer>;

private:
  //! Build BVH for tree algorithm (called from Initialize).
  void BuildBVH();

  //! Find solution using Newton method from given starting point.
  //! @param[in] theP      query point (unused, kept for interface consistency)
  //! @param[in] theParams starting point parameters on surface
  void FindSolution(const gp_Pnt& theP, const Extrema_POnSurfParams& theParams);

  //! Selection of points to build grid, depending on the type of surface.
  //! Fills myUParams and myVParams for BSpline/Bezier surfaces.
  //! @param[in] theSurf surface to analyze
  void GetGridPoints(const Adaptor3d_Surface& theSurf);

  //! Creation of grid of parametric points.
  //! @param[in] thePoint query point for distance computation
  void BuildGrid(const gp_Pnt& thePoint);

  //! Compute edge parameters for point projection on edge.
  //! @param[in] theIsUEdge true if edge is U-iso, false if V-iso
  //! @param[in] theParam0  first endpoint parameters
  //! @param[in] theParam1  second endpoint parameters
  //! @param[in] thePoint   query point
  //! @param[in] theDiffTol tolerance for distance comparison
  //! @return parameters of closest point on edge
  const Extrema_POnSurfParams& ComputeEdgeParameters(const Standard_Boolean       theIsUEdge,
                                                     const Extrema_POnSurfParams& theParam0,
                                                     const Extrema_POnSurfParams& theParam1,
                                                     const gp_Pnt&                thePoint,
                                                     const Standard_Real          theDiffTol);

  //! Compute grid step and initial offset for uniform parameter distribution.
  //! Uses small offset from boundaries to avoid singular points.
  //! @param[in]  theMin    minimum parameter value
  //! @param[in]  theMax    maximum parameter value
  //! @param[in]  theNbSamples number of sample points
  //! @param[out] theStep   computed step between parameters
  //! @param[out] theStart  computed starting parameter value
  static void computeGridParameters(const Standard_Real    theMin,
                                    const Standard_Real    theMax,
                                    const Standard_Integer theNbSamples,
                                    Standard_Real&         theStep,
                                    Standard_Real&         theStart);

  //! Populate parameter array with uniform distribution.
  //! @param[out] theParams array to fill
  //! @param[in]  theStart  starting parameter value
  //! @param[in]  theStep   step between parameters
  //! @param[in]  theCount  number of parameters
  static void populateParamArray(const Handle(TColStd_HArray1OfReal)& theParams,
                                 const Standard_Real                  theStart,
                                 const Standard_Real                  theStep,
                                 const Standard_Integer               theCount);

  //! Add surface point to grid and optionally to BVH.
  //! @param[in] theNoU     U index in grid
  //! @param[in] theNoV     V index in grid
  //! @param[in] thePnt     3D point on surface
  //! @param[in] theAddToBVH if true, also adds point to BVH structure
  void addGridPoint(const Standard_Integer theNoU,
                    const Standard_Integer theNoV,
                    const gp_Pnt&          thePnt,
                    const Standard_Boolean theAddToBVH);

  //! Initialize grid structure (parameter arrays, point arrays, boundaries).
  //! Called once on first Perform() call.
  void initializeGrid();

  //! Compute squared distances from query point to all grid nodes.
  //! @param[in] thePoint query point
  void computeNodeDistances(const gp_Pnt& thePoint);

  //! Compute parameters for all edge projections.
  //! @param[in] thePoint   query point
  //! @param[in] theDiffTol tolerance for distance comparison
  void computeEdgeDistances(const gp_Pnt& thePoint, const Standard_Real theDiffTol);

  //! Compute parameters for all face projections.
  //! @param[in] thePoint   query point
  //! @param[in] theDiffTol tolerance for distance comparison
  void computeFaceDistances(const gp_Pnt& thePoint, const Standard_Real theDiffTol);

  //! Compute face parameter for a single grid cell.
  //! @param[in] theNoU     U index of face
  //! @param[in] theNoV     V index of face
  //! @param[in] thePoint   query point
  //! @param[in] theDiffTol tolerance for distance comparison
  void computeFaceParameter(const Standard_Integer theNoU,
                            const Standard_Integer theNoV,
                            const gp_Pnt&          thePoint,
                            const Standard_Real    theDiffTol);

private:
  // disallow copies
  Extrema_GenExtPS(const Extrema_GenExtPS&) Standard_DELETE;
  Extrema_GenExtPS& operator=(const Extrema_GenExtPS&) Standard_DELETE;

private:
  Standard_Boolean myDone;
  Standard_Boolean myInit;
  Standard_Real    myumin;
  Standard_Real    myusup;
  Standard_Real    myvmin;
  Standard_Real    myvsup;
  Standard_Integer myusample;
  Standard_Integer myvsample;
  Standard_Real    mytolu;
  Standard_Real    mytolv;

  Extrema_Array2OfPOnSurfParams         myPoints;
  BVH_BoxSet<Standard_Real, 3, UVIndex> myBVHSet; //!< BVH for surface points (tree algorithm)
  Extrema_FuncPSNorm                    myF;
  const Adaptor3d_Surface*              myS;
  Extrema_ExtFlag                       myFlag;
  Extrema_ExtAlgo                       myAlgo;
  Handle(TColStd_HArray1OfReal)         myUParams;
  Handle(TColStd_HArray1OfReal)         myVParams;
  Extrema_Array2OfPOnSurfParams         myFacePntParams;
  Extrema_Array2OfPOnSurfParams         myUEdgePntParams;
  Extrema_Array2OfPOnSurfParams         myVEdgePntParams;
  Extrema_POnSurfParams                 myGridParam;
};

#endif // _Extrema_GenExtPS_HeaderFile
