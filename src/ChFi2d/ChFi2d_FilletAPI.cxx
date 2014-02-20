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

#include <ChFi2d_FilletAPI.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS.hxx>

// An empty constructor of the fillet algorithm.
// Call a method Init() to initialize the algorithm
// before calling of a Perform() method.
ChFi2d_FilletAPI::ChFi2d_FilletAPI():myIsAnalytical(Standard_False)
{

}

// A constructor of a fillet algorithm: accepts a wire consisting of two edges in a plane.
ChFi2d_FilletAPI::ChFi2d_FilletAPI(const TopoDS_Wire& theWire, 
                                   const gp_Pln& thePlane):myIsAnalytical(Standard_False)
{
  Init(theWire, thePlane);
}

// A constructor of a fillet algorithm: accepts two edges in a plane.
ChFi2d_FilletAPI::ChFi2d_FilletAPI(const TopoDS_Edge& theEdge1, 
                                   const TopoDS_Edge& theEdge2, 
                                   const gp_Pln& thePlane):myIsAnalytical(Standard_False)
{
  Init(theEdge1, theEdge2, thePlane);
}

// Initializes a fillet algorithm: accepts a wire consisting of two edges in a plane.
void ChFi2d_FilletAPI::Init(const TopoDS_Wire& theWire, 
                            const gp_Pln& thePlane)
{
  // Decide whether we may apply an analytical solution.
  TopoDS_Edge E1, E2;
  TopoDS_Iterator itr(theWire);
  for (; itr.More(); itr.Next())
  {
    if (E1.IsNull())
      E1 = TopoDS::Edge(itr.Value());
    else if (E2.IsNull())
      E2 = TopoDS::Edge(itr.Value());
    else
      break;
  }
  if (!E1.IsNull() && !E2.IsNull())
    myIsAnalytical = IsAnalytical(E1, E2);

  // Initialize the algorithm.
  myIsAnalytical ? myAnaFilletAlgo.Init(theWire, thePlane) :
                   myFilletAlgo.Init(theWire, thePlane);
}

// Initializes a fillet algorithm: accepts two edges in a plane.
void ChFi2d_FilletAPI::Init(const TopoDS_Edge& theEdge1, 
                            const TopoDS_Edge& theEdge2, 
                            const gp_Pln& thePlane)
{
  // Decide whether we may apply an analytical solution.
  myIsAnalytical = IsAnalytical(theEdge1, theEdge2);

  // Initialize the algorithm.
  myIsAnalytical ? myAnaFilletAlgo.Init(theEdge1, theEdge2, thePlane) :
                   myFilletAlgo.Init(theEdge1, theEdge2, thePlane);
}

// Returns true, if at least one result was found.
Standard_Boolean ChFi2d_FilletAPI::Perform(const Standard_Real theRadius)
{
  return myIsAnalytical ? myAnaFilletAlgo.Perform(theRadius) : 
                          myFilletAlgo.Perform(theRadius);
}

// Returns number of possible solutions.
Standard_Integer ChFi2d_FilletAPI::NbResults(const gp_Pnt& thePoint)
{
  return myIsAnalytical ? 1 : myFilletAlgo.NbResults(thePoint);
}

// Returns result (fillet edge, modified edge1, modified edge2), 
// nearest to the given point <thePoint> if iSolution == -1
TopoDS_Edge ChFi2d_FilletAPI::Result(const gp_Pnt& thePoint, 
                                     TopoDS_Edge& theEdge1, TopoDS_Edge& theEdge2, 
                                     const Standard_Integer iSolution)
{
  return myIsAnalytical ? myAnaFilletAlgo.Result(theEdge1, theEdge2) :
                          myFilletAlgo.Result(thePoint, theEdge1, theEdge2, iSolution);
}

// Decides whether the input parameters may use an analytical algorithm
// for calculation of the fillets, or an iteration-recursive method is needed.
// The analytical solution is applicable for linear and circular edges having a common point.
Standard_Boolean ChFi2d_FilletAPI::IsAnalytical(const TopoDS_Edge& theEdge1, 
                                                const TopoDS_Edge& theEdge2)
{
  Standard_Boolean ret(Standard_False);
  BRepAdaptor_Curve AC1(theEdge1), AC2(theEdge2);
  if ((AC1.GetType() == GeomAbs_Line || AC1.GetType() == GeomAbs_Circle) &&
      (AC2.GetType() == GeomAbs_Line || AC2.GetType() == GeomAbs_Circle))
  {
    // The edges are lines or arcs of circle.
    // Now check wether they have a common point.
    gp_Pnt p11 = AC1.Value(AC1.FirstParameter());
    gp_Pnt p12 = AC1.Value(AC1.LastParameter());
    gp_Pnt p21 = AC2.Value(AC2.FirstParameter());
    gp_Pnt p22 = AC2.Value(AC2.LastParameter());
    if (p11.SquareDistance(p21) < Precision::SquareConfusion() ||
        p11.SquareDistance(p22) < Precision::SquareConfusion() ||
        p12.SquareDistance(p21) < Precision::SquareConfusion() ||
        p12.SquareDistance(p22) < Precision::SquareConfusion())
    {
      ret = Standard_True;
    }
  }
  return ret;
}
