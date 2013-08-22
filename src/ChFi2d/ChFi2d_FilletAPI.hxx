// Created on: 2013-06-06
// Created by: Vlad ROMASHKO
// Copyright (c) 2003-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#ifndef _CHFI2D_FILLETAPI_H_
#define _CHFI2D_FILLETAPI_H_

#include <ChFi2d_FilletAlgo.hxx>
#include <ChFi2d_AnaFilletAlgo.hxx>

//! An interface class for 2D fillets.
//! Open CASCADE provides two algorithms for 2D fillets:
//!     ChFi2d_Builder - it constructs a fillet or chamfer 
//!                      for linear and circular edges of a face.
//!     ChFi2d_FilletAPI - it encapsulates two algorithms:
//!         ChFi2d_AnaFilletAlgo - analytical constructor of the fillet.
//!                                It works only for linear and circular edges,
//!                                having a common point.
//!         ChFi2d_FilletAlgo - iteration recursive method constructing 
//!                             the fillet edge for any type of edges including
//!                             ellipses and b-splines. 
//!                             The edges may even have no common point.
//!
//! The algorithms ChFi2d_AnaFilletAlgo and ChFi2d_FilletAlgo may be used directly 
//! or via this ChFi2d_FilletAPI class. This class chooses an appropriate algorithm
//! analyzing the arguments (a wire or two edges).
class ChFi2d_FilletAPI
{
public:

  //! An empty constructor of the fillet algorithm.
  //! Call a method Init() to initialize the algorithm
  //! before calling of a Perform() method.
  Standard_EXPORT ChFi2d_FilletAPI();

  //! A constructor of a fillet algorithm: accepts a wire consisting of two edges in a plane.
  Standard_EXPORT ChFi2d_FilletAPI(const TopoDS_Wire& theWire, 
                                   const gp_Pln& thePlane);

  //! A constructor of a fillet algorithm: accepts two edges in a plane.
  Standard_EXPORT ChFi2d_FilletAPI(const TopoDS_Edge& theEdge1, 
                                   const TopoDS_Edge& theEdge2, 
                                   const gp_Pln& thePlane);

  //! Initializes a fillet algorithm: accepts a wire consisting of two edges in a plane.
  Standard_EXPORT void Init(const TopoDS_Wire& theWire, 
                            const gp_Pln& thePlane);

  //! Initializes a fillet algorithm: accepts two edges in a plane.
  Standard_EXPORT void Init(const TopoDS_Edge& theEdge1, 
                            const TopoDS_Edge& theEdge2, 
                            const gp_Pln& thePlane);

  //! Constructs a fillet edge.
  //! Returns true if at least one result was found.
  Standard_EXPORT Standard_Boolean Perform(const Standard_Real theRadius);

  //! Returns number of possible solutions.
  //! <thePoint> chooses a particular fillet in case of several fillets 
  //! may be constructed (for example, a circle intersecting a segment in 2 points).
  //! Put the intersecting (or common) point of the edges.
  Standard_EXPORT Standard_Integer NbResults(const gp_Pnt& thePoint);

  //! Returns result (fillet edge, modified edge1, modified edge2), 
  //! nearest to the given point <thePoint> if iSolution == -1
  //! <thePoint> chooses a particular fillet in case of several fillets 
  //! may be constructed (for example, a circle intersecting a segment in 2 points).
  //! Put the intersecting (or common) point of the edges.
  Standard_EXPORT TopoDS_Edge Result(const gp_Pnt& thePoint, 
                                     TopoDS_Edge& theEdge1, TopoDS_Edge& theEdge2, 
                                     const Standard_Integer iSolution = -1);

private:

  // Decides whether the input parameters may use an analytical algorithm
  // for calculation of the fillets, or an iteration-recursive method is needed.
  // The analytical solution is applicable for linear and circular edges 
  // having a common point.
  Standard_Boolean IsAnalytical(const TopoDS_Edge& theEdge1, 
                                const TopoDS_Edge& theEdge2);

  // Implementation of the fillet algorithm.
  ChFi2d_FilletAlgo    myFilletAlgo;
  ChFi2d_AnaFilletAlgo myAnaFilletAlgo;
  Standard_Boolean     myIsAnalytical;
};

#endif // _CHFI2D_FILLETAPI_H_