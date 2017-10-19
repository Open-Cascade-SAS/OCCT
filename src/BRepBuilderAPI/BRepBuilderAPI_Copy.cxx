// Created on: 1994-12-12
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


#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepTools_Modification.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <Poly_Triangulation.hxx>

namespace {

//! Tool class implementing necessary functionality for copying geometry
class BRepBuilderAPI_Copy_Modification : public BRepTools_Modification 
{
public:
  BRepBuilderAPI_Copy_Modification (const Standard_Boolean copyGeom,
                                    const Standard_Boolean copyMesh)
    : myCopyGeom(copyGeom),
      myCopyMesh(copyMesh)
  {
  }

  //! Returns true to indicate the need to copy face;
  //! copies surface if requested
  Standard_Boolean NewSurface (const TopoDS_Face& F, Handle(Geom_Surface)& S,
                               TopLoc_Location& L, Standard_Real& Tol, 
                               Standard_Boolean& RevWires, Standard_Boolean& RevFace) Standard_OVERRIDE
  {
    S = BRep_Tool::Surface(F,L);
    Tol = BRep_Tool::Tolerance(F);
    RevWires = RevFace = Standard_False;

    if ( ! S.IsNull() && myCopyGeom )
      S = Handle(Geom_Surface)::DownCast(S->Copy());

    return Standard_True;
  }

  //! Returns true to indicate the need to copy triangulation;
  //! copies it if required
  Standard_Boolean NewTriangulation(const TopoDS_Face& F, Handle(Poly_Triangulation)& T) Standard_OVERRIDE
  {
    if (!myCopyMesh)
      return Standard_False;

    TopLoc_Location L;
    T = BRep_Tool::Triangulation(F, L);

    if (T.IsNull())
      return Standard_False;

    // mesh is copied if and only if the geometry need to be copied too
    if (myCopyGeom)
      T = T->Copy();
    return Standard_True;
  }

  //! Returns true to indicate the need to copy edge;
  //! copies curves if requested
  Standard_Boolean NewCurve (const TopoDS_Edge& E, Handle(Geom_Curve)& C,
                             TopLoc_Location& L, Standard_Real& Tol) Standard_OVERRIDE
  {
    Standard_Real f,l;
    C = BRep_Tool::Curve (E, L, f, l);
    Tol = BRep_Tool::Tolerance(E);

    if ( ! C.IsNull() && myCopyGeom )
      C = Handle(Geom_Curve)::DownCast(C->Copy());

    return Standard_True;
  }

  //! Returns true to indicate the need to copy polygon;
  //! copies it if required
  Standard_Boolean NewPolygon(const TopoDS_Edge& E, Handle(Poly_Polygon3D)& P) Standard_OVERRIDE
  {
    if (!myCopyMesh)
      return Standard_False;

    TopLoc_Location aLoc;
    P = BRep_Tool::Polygon3D(E, aLoc);

    if (P.IsNull())
      return Standard_False;

    // polygon is copied if and only if the geometry need to be copied too
    if (myCopyGeom)
      P = P->Copy();
    return Standard_True;
  }

  //! Returns true to indicate the need to copy polygon;
  //! copies it if required
  Standard_Boolean NewPolygonOnTriangulation(const TopoDS_Edge& E, const TopoDS_Face& F,
                                             Handle(Poly_PolygonOnTriangulation)& P) Standard_OVERRIDE
  {
    if (!myCopyMesh)
      return Standard_False;

    TopLoc_Location aLoc;
    Handle(Poly_Triangulation) aTria = BRep_Tool::Triangulation(F, aLoc);
    P = BRep_Tool::PolygonOnTriangulation(E, aTria, aLoc);

    if (P.IsNull())
      return Standard_False;

    // polygon is copied if and only if the geometry need to be copied too
    if (myCopyGeom)
      P = P->Copy();
    return Standard_True;
  }

  //! Returns true to indicate the need to copy vertex
  Standard_Boolean NewPoint (const TopoDS_Vertex& V, gp_Pnt& P,
                             Standard_Real& Tol) Standard_OVERRIDE
  {
    P = BRep_Tool::Pnt(V);
    Tol = BRep_Tool::Tolerance(V);
    return Standard_True;
  }

  //! Returns true to indicate the need to copy edge;
  //! copies pcurve if requested
  Standard_Boolean NewCurve2d (const TopoDS_Edge& E, 
                               const TopoDS_Face& F,
                               const TopoDS_Edge& /*NewE*/,
                               const TopoDS_Face& /*NewF*/,
                               Handle(Geom2d_Curve)& C, 
                               Standard_Real& Tol) Standard_OVERRIDE
  {
    Tol = BRep_Tool::Tolerance(E);
    Standard_Real f, l;
    C = BRep_Tool::CurveOnSurface (E, F, f, l);

    if ( ! C.IsNull() && myCopyGeom )
      C = Handle(Geom2d_Curve)::DownCast (C->Copy());

    return Standard_True;
  }

  //! Returns true to indicate the need to copy vertex
  Standard_Boolean NewParameter (const TopoDS_Vertex& V, const TopoDS_Edge& E,
                                 Standard_Real& P, Standard_Real& Tol) Standard_OVERRIDE
  {
    if (V.IsNull()) return Standard_False; // infinite edge may have Null vertex

    Tol = BRep_Tool::Tolerance(V);
    P = BRep_Tool::Parameter (V, E);

    return Standard_True;
  }

  //! Returns the  continuity of E between F1 and F2
  GeomAbs_Shape Continuity (const TopoDS_Edge& E, const TopoDS_Face& F1,
                            const TopoDS_Face& F2, const TopoDS_Edge&,
                            const TopoDS_Face&, const TopoDS_Face&) Standard_OVERRIDE
  {
    return BRep_Tool::Continuity (E, F1, F2);
  }

public:
  DEFINE_STANDARD_RTTI_INLINE(BRepBuilderAPI_Copy_Modification,BRepTools_Modification)

private: 
  Standard_Boolean myCopyGeom;
  Standard_Boolean myCopyMesh;
};

} // anonymous namespace

//=======================================================================
//function : BRepBuilderAPI_Copy
//purpose  : 
//=======================================================================

BRepBuilderAPI_Copy::BRepBuilderAPI_Copy ()
{
  myModification = new BRepBuilderAPI_Copy_Modification(Standard_True, Standard_False);
}


//=======================================================================
//function : BRepBuilderAPI_Copy
//purpose  : 
//=======================================================================

BRepBuilderAPI_Copy::BRepBuilderAPI_Copy(const TopoDS_Shape& S, const Standard_Boolean copyGeom, const Standard_Boolean copyMesh)
{
  myModification = new BRepBuilderAPI_Copy_Modification(copyGeom, copyMesh);
  DoModif(S);
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepBuilderAPI_Copy::Perform(const TopoDS_Shape& S, const Standard_Boolean copyGeom, const Standard_Boolean copyMesh)
{
  myModification = new BRepBuilderAPI_Copy_Modification(copyGeom, copyMesh);
  NotDone(); // on force la copie si on vient deja d`en faire une
  DoModif(S);
}

