// Created on: 1993-06-17
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _TopOpeBRepDS_BuildTool_HeaderFile
#define _TopOpeBRepDS_BuildTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRep_Builder.hxx>
#include <TopOpeBRepTool_CurveTool.hxx>
#include <Standard_Boolean.hxx>
#include <TopOpeBRepTool_OutCurveType.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <TopAbs_Orientation.hxx>
class TopOpeBRepTool_GeomTool;
class TopoDS_Shape;
class TopOpeBRepDS_Point;
class TopOpeBRepDS_Curve;
class TopOpeBRepDS_DataStructure;
class Geom_Curve;
class TopOpeBRepDS_Surface;
class TopoDS_Edge;
class TopoDS_Vertex;
class TopoDS_Face;
class TopOpeBRepDS_HDataStructure;
class Geom2d_Curve;
class Geom_Surface;


//! Provides  a  Tool  to  build  topologies. Used  to
//! instantiate the Builder algorithm.
class TopOpeBRepDS_BuildTool 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRepDS_BuildTool();
  
  Standard_EXPORT TopOpeBRepDS_BuildTool(const TopOpeBRepTool_OutCurveType OutCurveType);
  
  Standard_EXPORT TopOpeBRepDS_BuildTool(const TopOpeBRepTool_GeomTool& GT);
  
  Standard_EXPORT const TopOpeBRepTool_GeomTool& GetGeomTool() const;
  
  Standard_EXPORT TopOpeBRepTool_GeomTool& ChangeGeomTool();
  
  Standard_EXPORT void MakeVertex (TopoDS_Shape& V, const TopOpeBRepDS_Point& P) const;
  
  Standard_EXPORT void MakeEdge (TopoDS_Shape& E, const TopOpeBRepDS_Curve& C) const;
  
  Standard_EXPORT void MakeEdge (TopoDS_Shape& E, const TopOpeBRepDS_Curve& C, const TopOpeBRepDS_DataStructure& DS) const;
  
  Standard_EXPORT void MakeEdge (TopoDS_Shape& E, const Handle(Geom_Curve)& C, const Standard_Real Tol) const;
  
  Standard_EXPORT void MakeEdge (TopoDS_Shape& E) const;
  
  Standard_EXPORT void MakeWire (TopoDS_Shape& W) const;
  
  Standard_EXPORT void MakeFace (TopoDS_Shape& F, const TopOpeBRepDS_Surface& S) const;
  
  Standard_EXPORT void MakeShell (TopoDS_Shape& Sh) const;
  
  Standard_EXPORT void MakeSolid (TopoDS_Shape& S) const;
  
  //! Make an edge <Eou> with the curve of the edge <Ein>
  Standard_EXPORT void CopyEdge (const TopoDS_Shape& Ein, TopoDS_Shape& Eou) const;
  
  Standard_EXPORT void GetOrientedEdgeVertices (TopoDS_Edge& E, TopoDS_Vertex& Vmin, TopoDS_Vertex& Vmax, Standard_Real& Parmin, Standard_Real& Parmax) const;
  
  Standard_EXPORT void UpdateEdgeCurveTol (const TopoDS_Face& F1, const TopoDS_Face& F2, TopoDS_Edge& E, const Handle(Geom_Curve)& C3Dnew, const Standard_Real tol3d, const Standard_Real tol2d1, const Standard_Real tol2d2, Standard_Real& newtol, Standard_Real& newparmin, Standard_Real& newparmax) const;
  
  Standard_EXPORT void ApproxCurves (const TopOpeBRepDS_Curve& C, TopoDS_Edge& E, Standard_Integer& inewC, const Handle(TopOpeBRepDS_HDataStructure)& HDS) const;
  
  Standard_EXPORT void ComputePCurves (const TopOpeBRepDS_Curve& C, TopoDS_Edge& E, TopOpeBRepDS_Curve& newC, const Standard_Boolean CompPC1, const Standard_Boolean CompPC2, const Standard_Boolean CompC3D) const;
  
  Standard_EXPORT void PutPCurves (const TopOpeBRepDS_Curve& newC, TopoDS_Edge& E, const Standard_Boolean CompPC1, const Standard_Boolean CompPC2) const;
  
  Standard_EXPORT void RecomputeCurves (const TopOpeBRepDS_Curve& C, const TopoDS_Edge& oldE, TopoDS_Edge& E, Standard_Integer& inewC, const Handle(TopOpeBRepDS_HDataStructure)& HDS) const;
  
  //! Make a face <Fou> with the surface of the face <Fin>
  Standard_EXPORT void CopyFace (const TopoDS_Shape& Fin, TopoDS_Shape& Fou) const;
  
  Standard_EXPORT void AddEdgeVertex (const TopoDS_Shape& Ein, TopoDS_Shape& Eou, const TopoDS_Shape& V) const;
  
  Standard_EXPORT void AddEdgeVertex (TopoDS_Shape& E, const TopoDS_Shape& V) const;
  
  Standard_EXPORT void AddWireEdge (TopoDS_Shape& W, const TopoDS_Shape& E) const;
  
  Standard_EXPORT void AddFaceWire (TopoDS_Shape& F, const TopoDS_Shape& W) const;
  
  Standard_EXPORT void AddShellFace (TopoDS_Shape& Sh, const TopoDS_Shape& F) const;
  
  Standard_EXPORT void AddSolidShell (TopoDS_Shape& S, const TopoDS_Shape& Sh) const;
  
  //! Sets the parameter <P>  for  the vertex <V> on the
  //! edge <E>.
  Standard_EXPORT void Parameter (const TopoDS_Shape& E, const TopoDS_Shape& V, const Standard_Real P) const;
  
  //! Sets the range of edge <E>.
  Standard_EXPORT void Range (const TopoDS_Shape& E, const Standard_Real first, const Standard_Real last) const;
  
  //! Sets the range of edge <Eou> from <Ein>
  //! only when <Ein> has a closed geometry.
  Standard_EXPORT void UpdateEdge (const TopoDS_Shape& Ein, TopoDS_Shape& Eou) const;
  
  //! Compute the parameter of  the vertex <V>, supported
  //! by   the edge <E>, on the curve  <C>.
  Standard_EXPORT void Parameter (const TopOpeBRepDS_Curve& C, TopoDS_Shape& E, TopoDS_Shape& V) const;
  
  //! Sets the  curve <C> for the edge  <E>
  Standard_EXPORT void Curve3D (TopoDS_Shape& E, const Handle(Geom_Curve)& C, const Standard_Real Tol) const;
  
  //! Sets  the pcurve <C> for  the edge <E> on the face
  //! <F>.  If OverWrite is True the old pcurve if there
  //! is one  is overwritten, else the  two  pcurves are
  //! set.
  Standard_EXPORT void PCurve (TopoDS_Shape& F, TopoDS_Shape& E, const Handle(Geom2d_Curve)& C) const;
  
  Standard_EXPORT void PCurve (TopoDS_Shape& F, TopoDS_Shape& E, const TopOpeBRepDS_Curve& CDS, const Handle(Geom2d_Curve)& C) const;
  
  Standard_EXPORT void Orientation (TopoDS_Shape& S, const TopAbs_Orientation O) const;
  
  Standard_EXPORT TopAbs_Orientation Orientation (const TopoDS_Shape& S) const;
  
  Standard_EXPORT void Closed (TopoDS_Shape& S, const Standard_Boolean B) const;
  
  Standard_EXPORT Standard_Boolean Approximation() const;
  
  Standard_EXPORT void UpdateSurface (const TopoDS_Shape& F, const Handle(Geom_Surface)& SU) const;
  
  Standard_EXPORT void UpdateSurface (const TopoDS_Shape& E, const TopoDS_Shape& oldF, const TopoDS_Shape& newF) const;
  
  Standard_EXPORT Standard_Boolean OverWrite() const;
  
  Standard_EXPORT void OverWrite (const Standard_Boolean O);
  
  Standard_EXPORT Standard_Boolean Translate() const;
  
  Standard_EXPORT void Translate (const Standard_Boolean T);




protected:





private:

  
  Standard_EXPORT void TranslateOnPeriodic (TopoDS_Shape& F, TopoDS_Shape& E, Handle(Geom2d_Curve)& C) const;


  BRep_Builder myBuilder;
  TopOpeBRepTool_CurveTool myCurveTool;
  Standard_Boolean myOverWrite;
  Standard_Boolean myTranslate;


};







#endif // _TopOpeBRepDS_BuildTool_HeaderFile
