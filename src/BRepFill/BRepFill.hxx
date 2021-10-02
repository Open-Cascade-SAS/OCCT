// Created on: 1994-03-03
// Created by: Joelle CHAUVET
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

#ifndef _BRepFill_HeaderFile
#define _BRepFill_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <TColStd_Array1OfReal.hxx>
class TopoDS_Face;
class TopoDS_Edge;
class TopoDS_Shell;
class TopoDS_Wire;
class TopoDS_Shape;
class gp_Ax3;
class gp_Pnt;
class gp_Vec;



class BRepFill 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Computes a ruled surface between two edges.
  Standard_EXPORT static TopoDS_Face Face (const TopoDS_Edge& Edge1, const TopoDS_Edge& Edge2);
  
  //! Computes a ruled surface between two wires.
  //! The wires must have the same number of edges.
  Standard_EXPORT static TopoDS_Shell Shell (const TopoDS_Wire& Wire1, const TopoDS_Wire& Wire2);
  
  //! Computes  <AxeProf>  as Follow. <Location> is
  //! the Position of the nearest vertex V  of <Profile>
  //! to <Spine>.<XDirection> is confused with the tangent
  //! to <Spine> at the projected point of V on the Spine.
  //! <Direction> is normal to <Spine>.
  //! <Spine> is a plane wire or a plane face.
  Standard_EXPORT static void Axe (const TopoDS_Shape& Spine, const TopoDS_Wire& Profile, gp_Ax3& AxeProf, Standard_Boolean& ProfOnSpine, const Standard_Real Tol);
  
  //! Compute ACR on a  wire
  Standard_EXPORT static void ComputeACR (const TopoDS_Wire& wire, TColStd_Array1OfReal& ACR);
  
  //! Insert ACR on a  wire
  Standard_EXPORT static TopoDS_Wire InsertACR (const TopoDS_Wire& wire, const TColStd_Array1OfReal& ACRcuts, const Standard_Real prec);




protected:





private:

  
  //! Computes origins and orientation on a closed wire
  Standard_EXPORT static void SearchOrigin (TopoDS_Wire& W, const gp_Pnt& P, const gp_Vec& V, const Standard_Real Tol);



friend class BRepFill_Generator;
friend class BRepFill_SectionLaw;
friend class BRepFill_ShapeLaw;
friend class BRepFill_NSections;
friend class BRepFill_Draft;
friend class BRepFill_LocationLaw;
friend class BRepFill_DraftLaw;
friend class BRepFill_Edge3DLaw;
friend class BRepFill_EdgeOnSurfLaw;
friend class BRepFill_ACRLaw;
friend class BRepFill_Pipe;
friend class BRepFill_PipeShell;
friend class BRepFill_Evolved;
friend class BRepFill_Sweep;
friend class BRepFill_CompatibleWires;
friend class BRepFill_OffsetWire;
friend class BRepFill_OffsetAncestors;
friend class BRepFill_ApproxSeewing;
friend class BRepFill_ComputeCLine;
friend class BRepFill_TrimSurfaceTool;
friend class BRepFill_TrimEdgeTool;
friend class BRepFill_SectionPlacement;
friend class BRepFill_Section;
friend class BRepFill_TrimShellCorner;
friend class BRepFill_CurveConstraint;
friend class BRepFill_Filling;
friend class BRepFill_FaceAndOrder;
friend class BRepFill_EdgeFaceAndOrder;

};







#endif // _BRepFill_HeaderFile
