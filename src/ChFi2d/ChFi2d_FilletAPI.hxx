// Created on: 2013-06-06
// Created by: Vlad ROMASHKO
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef ChFi2d_FilletAPI_HeaderFile
#define ChFi2d_FilletAPI_HeaderFile

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
