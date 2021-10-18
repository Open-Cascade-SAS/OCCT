// Created on: 2013-05-20
// Created by: Mikhail PONIKAROV
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#ifndef ChFi2d_FilletAlgo_HeaderFile
#define ChFi2d_FilletAlgo_HeaderFile

#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <gp_Pnt.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Plane.hxx>
#include <TColStd_ListOfReal.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <TColStd_SequenceOfBoolean.hxx>
#include <TColStd_SequenceOfInteger.hxx>

class FilletPoint;

//! Algorithm that creates fillet edge: arc tangent to two edges in the start
//! and in the end vertices. Initial edges must be located on the plane and 
//! must be connected by the end or start points (shared vertices are not 
//! obligatory). Created fillet arc is created with the given radius, that is
//! useful in sketcher applications.
//! 
//! The algorithm is iterative that allows to create fillet on any curves
//! of initial edges, that supports projection of point and C2 continuous.
//! Principles of algorithm can de reduced to the Newton method:
//! 1. Splitting initial edge into N segments where probably only 1 root can be
//!    found. N depends on the complexity of the underlying curve.
//! 2. On each segment compute value and derivative of the function:
//!    - argument of the function is the parameter on the curve
//!    - take point on the curve by the parameter: point of tangency
//!    - make center of fillet: perpendicular vector from the point of tagency
//!    - make projection from the center to the second curve
//!    - length of the projection minus radius of the fillet is result of the 
//!      function
//!    - derivative of this function in the point is computed by value in 
//!      point with small shift
//! 3. Using Newton search method take the point on the segment where function
//!    value is most close to zero. If it is not enough close, step 2 and 3 are
//!    repeated taking as start or end point the found point.
//! 4. If solution is found, result is created on point on root of the function (as a start point),
//!    point of the projection onto second curve (as an end point) and center of arc in found center.
//!    Initial edges are cut by the start and end point of tangency.
class ChFi2d_FilletAlgo 
{
public:

  //! An empty constructor of the fillet algorithm.
  //! Call a method Init() to initialize the algorithm
  //! before calling of a Perform() method.
  Standard_EXPORT ChFi2d_FilletAlgo();

  //! A constructor of a fillet algorithm: accepts a wire consisting of two edges in a plane.
  Standard_EXPORT ChFi2d_FilletAlgo(const TopoDS_Wire& theWire, 
                                    const gp_Pln& thePlane);

  //! A constructor of a fillet algorithm: accepts two edges in a plane.
  Standard_EXPORT ChFi2d_FilletAlgo(const TopoDS_Edge& theEdge1, 
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
  //! Returns true, if at least one result was found
  Standard_EXPORT Standard_Boolean Perform(const Standard_Real theRadius);

  //! Returns number of possible solutions.
  //! <thePoint> chooses a particular fillet in case of several fillets
  //! may be constructed (for example, a circle intersecting a segment in 2 points).
  //! Put the intersecting (or common) point of the edges.
  Standard_EXPORT Standard_Integer NbResults(const gp_Pnt& thePoint);

  //! Returns result (fillet edge, modified edge1, modified edge2),
  //! nearest to the given point <thePoint> if iSolution == -1.
  //! <thePoint> chooses a particular fillet in case of several fillets
  //! may be constructed (for example, a circle intersecting a segment in 2 points).
  //! Put the intersecting (or common) point of the edges.
  Standard_EXPORT TopoDS_Edge Result(const gp_Pnt& thePoint,
                                     TopoDS_Edge& theEdge1, TopoDS_Edge& theEdge2,
                                     const Standard_Integer iSolution = -1);

private:
  //! Computes the value the function in the current point.
  //! <theLimit> is end parameter of the segment
  void FillPoint(FilletPoint*, const Standard_Real theLimit);
  //! Computes the derivative value of the function in the current point.
  //! <theDiffStep> is small step for approximate derivative computation
  //! <theFront> is direction of the step: from or reversed
  void FillDiff(FilletPoint*, Standard_Real theDiffStep, Standard_Boolean theFront);
  //! Using Newton methods computes optimal point, that can be root of the
  //! function taking into account two input points, functions value and derivatives.
  //! Performs iteration until root is found or failed to find root.
  //! Stores roots in myResultParams.
  void PerformNewton(FilletPoint*, FilletPoint*);
  //! Splits segment by the parameter and calls Newton method for both segments.
  //! It supplies recursive iterations of the Newton methods calls
  //! (PerformNewton calls this function and this calls Netwton two times).
  Standard_Boolean ProcessPoint(FilletPoint*, FilletPoint*, Standard_Real);

  //! Initial edges where the fillet must be computed.
  TopoDS_Edge myEdge1, myEdge2;
  //! Plane where fillet arc must be created.
  Handle(Geom_Plane) myPlane;
  //! Underlying curves of the initial edges
  Handle(Geom2d_Curve) myCurve1, myCurve2;
  //! Start and end parameters of curves of initial edges.
  Standard_Real myStart1, myEnd1, myStart2, myEnd2, myRadius;
  //! List of params where roots were found.
  TColStd_ListOfReal myResultParams;
  //! sequence of 0 or 1: position of the fillet relatively to the first curve
  TColStd_SequenceOfInteger myResultOrientation;
  //! position of the fillet relatively to the first curve
  Standard_Boolean myStartSide;
  //! are initial edges where exchanged in the beginning: to make first edge 
  //! more simple and minimize number of iterations
  Standard_Boolean myEdgesExchnged;
  //! Number to avoid infinity recursion: indicates how deep the recursion is performed.
  Standard_Integer myDegreeOfRecursion;
};

//! Private class. Corresponds to the point on the first curve, computed
//! fillet function and derivative on it.
class FilletPoint 
{
public:
  //! Creates a point on a first curve by parameter on this curve.
  FilletPoint(const Standard_Real theParam);

  //! Changes the point position by changing point parameter on the first curve.
  void setParam(Standard_Real theParam) {myParam = theParam;}

  //! Returns the point parameter on the first curve.
  Standard_Real getParam() const {return myParam;}

  //! Returns number of found values of function in this point.
  Standard_Integer getNBValues() {return myV.Length();}

  //! Returns value of function in this point.
  Standard_Real getValue(int theIndex) {return myV.Value(theIndex);}

  //! Returns derivatives of function in this point.
  Standard_Real getDiff(int theIndex) {return myD.Value(theIndex);}

  //! Returns true if function is valid (rediuses vectors of fillet do not intersect any curve).
  Standard_Boolean isValid(int theIndex) {return myValid.Value(theIndex);}

  //! Returns the index of the nearest value
  int getNear(int theIndex) {return myNear.Value(theIndex);}

  //! Defines the parameter of the projected point on the second curve.
  void setParam2(const Standard_Real theParam2) {myParam2 = theParam2;}

  //! Returns the parameter of the projected point on the second curve.
  Standard_Real getParam2() { return myParam2 ; }

  //! Center of the fillet.
  void setCenter(const gp_Pnt2d thePoint) {myCenter = thePoint;}
  //! Center of the fillet.
  const gp_Pnt2d getCenter() {return myCenter;}

  //! Appends value of the function.
  void appendValue(Standard_Real theValue, Standard_Boolean theValid);

  //! Computes difference between this point and the given. Stores difference in myD.
  Standard_Boolean calculateDiff(FilletPoint*);

  //! Filters out the values and leaves the most optimal one.
  void FilterPoints(FilletPoint*);

  //! Returns a pointer to created copy of the point
  //! warning: this is not the full copy! Copies only: myParam, myV, myD, myValid
  FilletPoint* Copy();

  //! Returns the index of the solution or zero if there is no solution
  Standard_Integer hasSolution(Standard_Real theRadius);

  //! For debug only
  Standard_Real LowerValue() 
  {
    Standard_Integer a, aResultIndex = 0;
    Standard_Real aValue;
    for(a = myV.Length(); a > 0; a--) 
    {
      if (aResultIndex == 0 || Abs(aValue) > Abs(myV.Value(a))) 
      {
        aResultIndex = a;
        aValue = myV.Value(a);
      }
    }
    return aValue;
  }
  //! Removes the found value by the given index.
  void remove(Standard_Integer theIndex);

private:
  //! Parameter on the first curve (start fillet point).
  Standard_Real myParam;
  //! Parameter on the second curve (end fillet point).
  Standard_Real myParam2;
  //! Values and derivative values of the fillet function.
  //! May be several if there are many projections on the second curve.
  TColStd_SequenceOfReal myV, myD;
  //! Center of the fillet arc.
  gp_Pnt2d myCenter;
  //! Flags for storage the validity of solutions. Indexes corresponds to indexes
  //! in sequences myV, myD.
  TColStd_SequenceOfBoolean myValid;
  TColStd_SequenceOfInteger myNear;
};

#endif // _FILLETALGO_H_
