// Created on: 1995-12-12
// Created by: Jacques GOUSSARD
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

#ifndef _BRepCheck_Wire_HeaderFile
#define _BRepCheck_Wire_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <BRepCheck_Status.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <BRepCheck_Result.hxx>
class TopoDS_Wire;
class TopoDS_Shape;
class TopoDS_Face;
class TopoDS_Edge;


class BRepCheck_Wire;
DEFINE_STANDARD_HANDLE(BRepCheck_Wire, BRepCheck_Result)


class BRepCheck_Wire : public BRepCheck_Result
{

public:

  
  Standard_EXPORT BRepCheck_Wire(const TopoDS_Wire& W);
  
  //! if <ContextShape> is  a  face, consequently checks
  //! SelfIntersect(),   Closed(),   Orientation()   and
  //! Closed2d until faulty is found
  Standard_EXPORT void InContext (const TopoDS_Shape& ContextShape) Standard_OVERRIDE;
  
  //! checks that the  wire  is  not empty and "connex".
  //! Called by constructor
  Standard_EXPORT void Minimum() Standard_OVERRIDE;
  
  //! Does nothing
  Standard_EXPORT void Blind() Standard_OVERRIDE;
  
  //! Checks if the  oriented  edges of the wire  give a
  //! closed  wire.   If the  wire   is closed,  returns
  //! BRepCheck_NoError.    Warning :  if the first  and
  //! last  edge   are  infinite,   the  wire   will  be
  //! considered as a closed one.  If <Update> is set to
  //! Standard_True, registers the status in the list.
  //! May return (and registers):
  //! **BRepCheck_NotConnected,   if    wire    is   not
  //! topologically closed
  //! **BRepCheck_RedundantEdge, if an  edge  is in wire
  //! more than 3 times  or  in  case of 2 occurences if
  //! not with FORWARD and REVERSED orientation.
  //! **BRepCheck_NoError
  Standard_EXPORT BRepCheck_Status Closed (const Standard_Boolean Update = Standard_False);
  
  //! Checks if edges of the  wire give a wire closed in
  //! 2d space.
  //! Returns BRepCheck_NoError,  or BRepCheck_NotClosed
  //! If <Update> is set to Standard_True, registers the
  //! status in the list.
  Standard_EXPORT BRepCheck_Status Closed2d (const TopoDS_Face& F, const Standard_Boolean Update = Standard_False);
  
  //! Checks   if  the oriented edges   of  the wire are
  //! correctly oriented.  An  internal call is made  to
  //! the  method Closed.   If no face  exists, call the
  //! method with   a  null  face  (TopoDS_face()).   If
  //! <Update> is  set  to Standard_True,  registers the
  //! status in the list.
  //! May return (and registers):
  //! BRepCheck_InvalidDegeneratedFlag,
  //! BRepCheck_BadOrientationOfSubshape,
  //! BRepCheck_NotClosed,
  //! BRepCheck_NoError
  Standard_EXPORT BRepCheck_Status Orientation (const TopoDS_Face& F, const Standard_Boolean Update = Standard_False);
  
  //! Checks if  the wire intersect   itself on the face
  //! <F>.  <E1>  and <E2>   are the first  intersecting
  //! edges  found.  <E2>  may  be a  null  edge when  a
  //! self-intersecting edge is found.If <Update> is set
  //! to Standard_True,   registers  the  status in  the
  //! list.
  //! May return (and register):
  //! BRepCheck_EmptyWire,
  //! BRepCheck_SelfIntersectingWire,
  //! BRepCheck_NoCurveOnSurface,
  //! BRepCheck_NoError
  Standard_EXPORT BRepCheck_Status SelfIntersect (const TopoDS_Face& F, TopoDS_Edge& E1, TopoDS_Edge& E2, const Standard_Boolean Update = Standard_False);
  
  //! report SelfIntersect() check would be (is) done
  Standard_EXPORT Standard_Boolean GeometricControls() const;
  
  //! set SelfIntersect() to be checked
  Standard_EXPORT void GeometricControls (const Standard_Boolean B);
  
  //! Sets status of Wire;
  Standard_EXPORT void SetStatus (const BRepCheck_Status theStatus);




  DEFINE_STANDARD_RTTIEXT(BRepCheck_Wire,BRepCheck_Result)

protected:




private:


  Standard_Boolean myCdone;
  BRepCheck_Status myCstat;
  TopTools_IndexedDataMapOfShapeListOfShape myMapVE;
  Standard_Boolean myGctrl;


};







#endif // _BRepCheck_Wire_HeaderFile
