// Created on: 1994-08-30
// Created by: Jacques GOUSSARD
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

#ifndef _Draft_Modification_HeaderFile
#define _Draft_Modification_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Draft_IndexedDataMapOfFaceFaceInfo.hxx>
#include <Draft_IndexedDataMapOfEdgeEdgeInfo.hxx>
#include <Draft_IndexedDataMapOfVertexVertexInfo.hxx>
#include <Standard_Boolean.hxx>
#include <TopoDS_Shape.hxx>
#include <Draft_ErrorStatus.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <BRepTools_Modification.hxx>
#include <Standard_Real.hxx>
#include <GeomAbs_Shape.hxx>
#include <TopAbs_Orientation.hxx>
class StdFail_NotDone;
class Standard_NoSuchObject;
class Standard_ConstructionError;
class TopoDS_Shape;
class TopoDS_Face;
class gp_Dir;
class gp_Pln;
class Geom_Surface;
class TopLoc_Location;
class TopoDS_Edge;
class Geom_Curve;
class TopoDS_Vertex;
class gp_Pnt;
class Geom2d_Curve;


class Draft_Modification;
DEFINE_STANDARD_HANDLE(Draft_Modification, BRepTools_Modification)


class Draft_Modification : public BRepTools_Modification
{

public:

  
  Standard_EXPORT Draft_Modification(const TopoDS_Shape& S);
  
  //! Resets on the same shape.
  Standard_EXPORT void Clear();
  
  //! Changes the basis shape and resets.
  Standard_EXPORT void Init (const TopoDS_Shape& S);
  
  //! Adds  the  face  F    and propagates    the  draft
  //! modification to  its  neighbour faces if they  are
  //! tangent. If an error occurs, will return False and
  //! ProblematicShape  will  return the "bad" face.
  Standard_EXPORT Standard_Boolean Add (const TopoDS_Face& F, const gp_Dir& Direction, const Standard_Real Angle, const gp_Pln& NeutralPlane, const Standard_Boolean Flag = Standard_True);
  
  //! Removes the face F and the neighbour faces if they
  //! are tangent.   It will be  necessary to  call this
  //! method if  the  method Add returns Standard_False,
  //! to unset ProblematicFace.
  Standard_EXPORT void Remove (const TopoDS_Face& F);
  
  //! Performs the draft angle modification and sets the
  //! value returned by the method  IsDone.  If an error
  //! occurs, IsDone  will return Standard_False, and an
  //! error status will  be  given by the  method Error,
  //! and the  shape on which  the problem appeared will
  //! be given by ProblematicShape
  Standard_EXPORT void Perform();
  
  //! Returns  True  if   Perform has  been  succesfully
  //! called. Otherwise more information can be obtained
  //! using the methods Error() and ProblematicShape().
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  Standard_EXPORT Draft_ErrorStatus Error() const;
  
  //! Returns the shape (Face,  Edge or Vertex) on which
  //! an error occured.
  Standard_EXPORT const TopoDS_Shape& ProblematicShape() const;
  
  //! Returns all  the  faces   which  have been   added
  //! together with the face <F>.
  Standard_EXPORT const TopTools_ListOfShape& ConnectedFaces (const TopoDS_Face& F);
  
  //! Returns all the faces  on which a modification has
  //! been given.
  Standard_EXPORT const TopTools_ListOfShape& ModifiedFaces();
  
  //! Returns Standard_True if   the face <F>  has  been
  //! modified.  In this case,  <S> is the new geometric
  //! support of the  face,  <L> the new  location,<Tol>
  //! the   new tolerance.<RevWires>  has  to  be set to
  //! Standard_True when   the modification reverses the
  //! normal   of  the  surface.(the  wires   have to be
  //! reversed).  <RevFace>    has    to  be   set    to
  //! Standard_True  if  the orientation of the modified
  //! face changes in  the shells which contain it. Here
  //! it will be set to Standard_False.
  //!
  //! Otherwise, returns Standard_False, and <S>,   <L>,
  //! <Tol> , <RevWires> ,<RevFace> are not  significant.
  Standard_EXPORT Standard_Boolean NewSurface (const TopoDS_Face& F, Handle(Geom_Surface)& S, TopLoc_Location& L, Standard_Real& Tol, Standard_Boolean& RevWires, Standard_Boolean& RevFace) Standard_OVERRIDE;
  
  //! Returns Standard_True  if  the edge  <E> has  been
  //! modified.  In this case,  <C> is the new geometric
  //! support of the  edge, <L> the  new location, <Tol>
  //! the         new    tolerance.   Otherwise, returns
  //! Standard_False,    and  <C>,  <L>,   <Tol> are not
  //! significant.
  Standard_EXPORT Standard_Boolean NewCurve (const TopoDS_Edge& E, Handle(Geom_Curve)& C, TopLoc_Location& L, Standard_Real& Tol) Standard_OVERRIDE;
  
  //! Returns  Standard_True if the  vertex <V> has been
  //! modified.  In this  case, <P> is the new geometric
  //! support of the vertex,   <Tol> the new  tolerance.
  //! Otherwise, returns Standard_False, and <P>,  <Tol>
  //! are not significant.
  Standard_EXPORT Standard_Boolean NewPoint (const TopoDS_Vertex& V, gp_Pnt& P, Standard_Real& Tol) Standard_OVERRIDE;
  
  //! Returns Standard_True if  the edge  <E> has a  new
  //! curve on surface on the face <F>.In this case, <C>
  //! is the new geometric support of  the edge, <L> the
  //! new location, <Tol> the new tolerance.
  //!
  //! Otherwise, returns  Standard_False, and <C>,  <L>,
  //! <Tol> are not significant.
  //!
  //! <NewE> is the new  edge created from  <E>.  <NewF>
  //! is the new face created from <F>. They may be usefull.
  Standard_EXPORT Standard_Boolean NewCurve2d (const TopoDS_Edge& E, const TopoDS_Face& F, const TopoDS_Edge& NewE, const TopoDS_Face& NewF, Handle(Geom2d_Curve)& C, Standard_Real& Tol) Standard_OVERRIDE;
  
  //! Returns Standard_True if the Vertex  <V> has a new
  //! parameter on the  edge <E>. In  this case,  <P> is
  //! the parameter,    <Tol>  the     new    tolerance.
  //! Otherwise, returns Standard_False, and <P>,  <Tol>
  //! are not significant.
  Standard_EXPORT Standard_Boolean NewParameter (const TopoDS_Vertex& V, const TopoDS_Edge& E, Standard_Real& P, Standard_Real& Tol) Standard_OVERRIDE;
  
  //! Returns the  continuity of  <NewE> between <NewF1>
  //! and <NewF2>.
  //!
  //! <NewE> is the new  edge created from <E>.  <NewF1>
  //! (resp. <NewF2>) is the new  face created from <F1>
  //! (resp. <F2>).
  Standard_EXPORT GeomAbs_Shape Continuity (const TopoDS_Edge& E, const TopoDS_Face& F1, const TopoDS_Face& F2, const TopoDS_Edge& NewE, const TopoDS_Face& NewF1, const TopoDS_Face& NewF2) Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(Draft_Modification,BRepTools_Modification)

protected:




private:

  
  Standard_EXPORT Standard_Boolean InternalAdd (const TopoDS_Face& F, const gp_Dir& Direction, const Standard_Real Angle, const gp_Pln& NeutralPlane, const Standard_Boolean Flag = Standard_True);
  
  Standard_EXPORT Standard_Boolean Propagate();
  
  Standard_EXPORT Handle(Geom_Curve) NewCurve (const Handle(Geom_Curve)& C, const Handle(Geom_Surface)& S, const TopAbs_Orientation OriS, const gp_Dir& Direction, const Standard_Real Angle, const gp_Pln& NeutralPlane, const Standard_Boolean Flag = Standard_True);
  
  Standard_EXPORT Handle(Geom_Surface) NewSurface (const Handle(Geom_Surface)& S, const TopAbs_Orientation OriS, const gp_Dir& Direction, const Standard_Real Angle, const gp_Pln& NeutralPlane);

  Draft_IndexedDataMapOfFaceFaceInfo myFMap;
  Draft_IndexedDataMapOfEdgeEdgeInfo myEMap;
  Draft_IndexedDataMapOfVertexVertexInfo myVMap;
  Standard_Boolean myComp;
  TopoDS_Shape myShape;
  TopoDS_Shape badShape;
  Draft_ErrorStatus errStat;
  TopoDS_Face curFace;
  TopTools_ListOfShape conneF;
  TopTools_IndexedDataMapOfShapeListOfShape myEFMap;


};







#endif // _Draft_Modification_HeaderFile
