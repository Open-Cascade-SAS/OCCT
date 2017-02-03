// Created on: 1995-06-12
// Created by: Joelle CHAUVET
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

#ifndef _ChFi2d_Builder_HeaderFile
#define _ChFi2d_Builder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <ChFi2d_ConstructionError.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>

class TopoDS_Edge;
class TopoDS_Vertex;

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

//! This  class contains  the algorithm  used to build
//! fillet on planar wire.
class ChFi2d_Builder 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT ChFi2d_Builder();
  
  //! The face  <F> can be build  on a closed or an open
  //! wire.
  Standard_EXPORT ChFi2d_Builder(const TopoDS_Face& F);
  
  Standard_EXPORT void Init (const TopoDS_Face& F);
  
  Standard_EXPORT void Init (const TopoDS_Face& RefFace, const TopoDS_Face& ModFace);
  
  //! Add  a fillet  of   radius  <Radius> on  the  wire
  //! between the two edges connected to the vertex <V>.
  //! <AddFillet> returns the  fillet edge. The returned
  //! edge has  sense only   if the status   <status> is
  //! <IsDone>
  Standard_EXPORT TopoDS_Edge AddFillet (const TopoDS_Vertex& V, const Standard_Real Radius);
  
  //! modify the fillet radius and return the new fillet
  //! edge. this    edge has sense  only if   the status
  //! <status> is <IsDone>.
  Standard_EXPORT TopoDS_Edge ModifyFillet (const TopoDS_Edge& Fillet, const Standard_Real Radius);
  
  //! removes the fillet <Fillet> and returns the vertex
  //! connecting the two adjacent edges to  this fillet.
  Standard_EXPORT TopoDS_Vertex RemoveFillet (const TopoDS_Edge& Fillet);
  
  //! Add a chamfer on  the  wire between the two  edges
  //! connected <E1> and  <E2>. <AddChamfer> returns the
  //! chamfer  edge. This  edge  has  sense only if  the
  //! status <status> is <IsDone>.
  Standard_EXPORT TopoDS_Edge AddChamfer (const TopoDS_Edge& E1, const TopoDS_Edge& E2, const Standard_Real D1, const Standard_Real D2);
  
  //! Add  a chamfer on the   wire between the two edges
  //! connected to the vertex <V>. The chamfer will make
  //! an  angle <Ang> with the edge  <E>, and one of its
  //! extremities  will be on  <E>  at distance <D>. The
  //! returned   edge has sense   only   if the   status
  //! <status> is <IsDone>.
  //! Warning: The value of <Ang> must be expressed in Radian.
  Standard_EXPORT TopoDS_Edge AddChamfer (const TopoDS_Edge& E, const TopoDS_Vertex& V, const Standard_Real D, const Standard_Real Ang);
  
  //! modify the chamfer <Chamfer>  and returns  the new
  //! chamfer edge.
  //! This edge as sense only  if the status <status> is
  //! <IsDone>.
  Standard_EXPORT TopoDS_Edge ModifyChamfer (const TopoDS_Edge& Chamfer, const TopoDS_Edge& E1, const TopoDS_Edge& E2, const Standard_Real D1, const Standard_Real D2);
  
  //! modify the  chamfer <Chamfer>  and returns the new
  //! chamfer edge. This    edge as sense  only   if the
  //! status <status>   is  <IsDone>.
  //! Warning: The value of <Ang> must be expressed in Radian.
  Standard_EXPORT TopoDS_Edge ModifyChamfer (const TopoDS_Edge& Chamfer, const TopoDS_Edge& E, const Standard_Real D, const Standard_Real Ang);
  
  //! removes   the chamfer  <Chamfer>   and returns the
  //! vertex connecting  the two adjacent  edges to this
  //! chamfer.
  Standard_EXPORT TopoDS_Vertex RemoveChamfer (const TopoDS_Edge& Chamfer);
  
  //! returns the modified face
    TopoDS_Face Result() const;
  
    Standard_Boolean IsModified (const TopoDS_Edge& E) const;
  
  //! returns the list of new edges
    const TopTools_SequenceOfShape& FilletEdges() const;
  
    Standard_Integer NbFillet() const;
  
  //! returns the list of new edges
    const TopTools_SequenceOfShape& ChamferEdges() const;
  
    Standard_Integer NbChamfer() const;
  
    Standard_Boolean HasDescendant (const TopoDS_Edge& E) const;
  
  //! returns the modified edge if <E> has descendant or
  //! <E> in the other case.
    const TopoDS_Edge& DescendantEdge (const TopoDS_Edge& E) const;
  
  //! Returns the parent edge of  <E>
  //! Warning: If <E>is a basis edge,  the returned edge would be
  //! equal to <E>
  Standard_EXPORT const TopoDS_Edge& BasisEdge (const TopoDS_Edge& E) const;
  
    ChFi2d_ConstructionError Status() const;




protected:





private:

  
  //! Is internaly used by <AddFillet>.
  //! Warning: <TrimE1>, <TrimE2>, <Fillet> has sense only if the
  //! status <status> is equal to <IsDone>
  Standard_EXPORT void ComputeFillet (const TopoDS_Vertex& V, const TopoDS_Edge& E1, const TopoDS_Edge& E2, const Standard_Real Radius, TopoDS_Edge& TrimE1, TopoDS_Edge& TrimE2, TopoDS_Edge& Fillet);
  
  //! Is internaly used by  <AddChamfer>. The chamfer is
  //! computed  from  a  vertex,   two  edges   and  two
  //! distances
  //! Warning: <TrimE1>, <TrimE2> and <Chamfer> has sense only if
  //! if the status <status> is equal to <IsDone>
  Standard_EXPORT void ComputeChamfer (const TopoDS_Vertex& V, const TopoDS_Edge& E1, const TopoDS_Edge& E2, const Standard_Real D1, const Standard_Real D2, TopoDS_Edge& TrimE1, TopoDS_Edge& TrimE2, TopoDS_Edge& Chamfer);
  
  //! Is internaly used by <AddChamfer>.  The chamfer is
  //! computed from   an  edge,  a  vertex,   a distance
  //! and an angle
  //! Warning: <TrimE1>,  <TrimE2>, and <Chamfer> has
  //! sense only   if  the status <status> is   equal to
  //! <IsDone>
  Standard_EXPORT void ComputeChamfer (const TopoDS_Vertex& V, const TopoDS_Edge& E1, const Standard_Real D, const Standard_Real Ang, const TopoDS_Edge& E2, TopoDS_Edge& TrimE1, TopoDS_Edge& TrimE2, TopoDS_Edge& Chamfer);
  
  //! Is   internaly  used     by  <ComputeFillet>.
  //! <NewExtr1> and  <NewExtr2>  will  contains the new
  //! extremities of <AdjEdge1> and <AdjEdge2>
  //! Warning: The  returned  edge has sense   only if the status
  //! <status> is equal to <IsDone>
  //! or to one of those specific cases :
  //! <FirstEdgeDegenerated>
  //! <LastEdgeDegenerated>
  //! <BothEdgesDegenerated>
  Standard_EXPORT TopoDS_Edge BuildFilletEdge (const TopoDS_Vertex& V, const TopoDS_Edge& AdjEdge1, const TopoDS_Edge& AdjEdge2, const Standard_Real Radius, TopoDS_Vertex& NewExtr1, TopoDS_Vertex& NewExtr2);
  
  //! Is   internaly  used     by  <ComputeFillet>.
  //! <NewExtr1> and  <NewExtr2>  will  contains the new
  //! extremities of <AdjEdge1> and <AdjEdge2>
  //! Warning: The  returned  edge has sense   only if the status
  //! <status> is equal to <IsDone>
  Standard_EXPORT TopoDS_Edge BuildChamferEdge (const TopoDS_Vertex& V, const TopoDS_Edge& AdjEdge1, const TopoDS_Edge& AdjEdge2, const Standard_Real D1, const Standard_Real D2, TopoDS_Vertex& NewExtr1, TopoDS_Vertex& NewExtr2);
  
  //! Is   internaly  used     by  <ComputeFillet>.
  //! <NewExtr1> and  <NewExtr2>  will  contains the new
  //! extremities of <AdjEdge1> and <AdjEdge2>
  //! Warning: The  returned  edge has sense   only if the status
  //! <status> is equal to <IsDone>
  Standard_EXPORT TopoDS_Edge BuildChamferEdge (const TopoDS_Vertex& V, const TopoDS_Edge& AdjEdge2, const Standard_Real D, const Standard_Real Ang, const TopoDS_Edge& AdjEdge1, TopoDS_Vertex& NewExtr1, TopoDS_Vertex& NewExtr2);
  
  //! replaces in  the  new face  <newFace> <OldE1>  and
  //! <OldE2>  by <E1>, <Fillet> and <E2>
  //! or by <Fillet> and <E2> if <E1> is degenerated
  //! or by <E1> and <Fillet> if <E2> is degenerated
  //! or by <Fillet> if <E1> and <E2> are degenerated .
  Standard_EXPORT void BuildNewWire (const TopoDS_Edge& OldE1, const TopoDS_Edge& OldE2, const TopoDS_Edge& E1, const TopoDS_Edge& Fillet, const TopoDS_Edge& E2);
  
  //! Changes <OldExtr> of <E1> by <NewExtr>
  Standard_EXPORT TopoDS_Edge BuildNewEdge (const TopoDS_Edge& E1, const TopoDS_Vertex& OldExtr, const TopoDS_Vertex& NewExtr) const;
  
  //! Changes <OldExtr> of <E1> by <NewExtr>
  //! returns E1 and IsDegenerated = Standard_True
  //! if the new edge is degenerated
  Standard_EXPORT TopoDS_Edge BuildNewEdge (const TopoDS_Edge& E1, const TopoDS_Vertex& OldExtr, const TopoDS_Vertex& NewExtr, Standard_Boolean& IsDegenerated) const;
  
  //! Writes <NewEdge> in  <fillets> if <Id> is equal to
  //! 1, or in <chamfers> if <Id> is Equal to 2.
  //! Writes  the  modifications  in  <history> :
  //! <TrimE1> is given by <E1>, <TrimE2> by <E2>
  //! if <TrimE1> and <TrimE2> are not degenerated.
  Standard_EXPORT void UpDateHistory (const TopoDS_Edge& E1, const TopoDS_Edge& E2, const TopoDS_Edge& TrimE1, const TopoDS_Edge& TrimE2, const TopoDS_Edge& NewEdge, const Standard_Integer Id);
  
  //! Writes the  modifications in  <history> . <TrimE1>
  //! is given by <E1>, <TrimE2> by <E2>.
  Standard_EXPORT void UpDateHistory (const TopoDS_Edge& E1, const TopoDS_Edge& E2, const TopoDS_Edge& TrimE1, const TopoDS_Edge& TrimE2);
  
  Standard_EXPORT Standard_Boolean IsAFillet (const TopoDS_Edge& E) const;
  
  Standard_EXPORT Standard_Boolean IsAChamfer (const TopoDS_Edge& E) const;


  ChFi2d_ConstructionError status;
  TopoDS_Face refFace;
  TopoDS_Face newFace;
  TopTools_SequenceOfShape fillets;
  TopTools_SequenceOfShape chamfers;
  TopTools_DataMapOfShapeShape history;


};


#include <ChFi2d_Builder.lxx>





#endif // _ChFi2d_Builder_HeaderFile
