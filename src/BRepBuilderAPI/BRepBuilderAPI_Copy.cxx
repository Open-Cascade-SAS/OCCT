// Created on: 1994-12-12
// Created by: Jacques GOUSSARD
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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



#include <BRepBuilderAPI_Copy.ixx>

#include <Geom_Surface.hxx>
#include <Geom_Curve.hxx>
#include <Geom2d_Curve.hxx>
#include <BRepTools_Modification.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS_Vertex.hxx>
#include <gp_Pnt.hxx>

//! Tool class implementing necessary functionality for copying geometry
class BRepBuilderAPI_Copy_Modification : public BRepTools_Modification 
{
public:
  BRepBuilderAPI_Copy_Modification (const Standard_Boolean copyGeom)
    : myCopyGeom(copyGeom)
  {
  }

  //! Returns true to indicate the need to copy face;
  //! copies surface if requested
  Standard_Boolean NewSurface (const TopoDS_Face& F, Handle(Geom_Surface)& S,
                               TopLoc_Location& L, Standard_Real& Tol, 
                               Standard_Boolean& RevWires, Standard_Boolean& RevFace)
  {
    S = BRep_Tool::Surface(F,L);
    Tol = BRep_Tool::Tolerance(F);
    RevWires = RevFace = Standard_False;

    if ( ! S.IsNull() && myCopyGeom )
      S = Handle(Geom_Surface)::DownCast(S->Copy());

    return Standard_True;
  }

  //! Returns true to indicate the need to copy edge;
  //! copies curves if requested
  Standard_Boolean NewCurve (const TopoDS_Edge& E, Handle(Geom_Curve)& C,
                             TopLoc_Location& L, Standard_Real& Tol)
  {
    Standard_Real f,l;
    C = BRep_Tool::Curve (E, L, f, l);
    Tol = BRep_Tool::Tolerance(E);

    if ( ! C.IsNull() && myCopyGeom )
      C = Handle(Geom_Curve)::DownCast(C->Copy());

    return Standard_True;
  }

  //! Returns true to indicate the need to copy vertex
  Standard_Boolean NewPoint (const TopoDS_Vertex& V, gp_Pnt& P,
                             Standard_Real& Tol)
  {
    P = BRep_Tool::Pnt(V);
    Tol = BRep_Tool::Tolerance(V);
    return Standard_True;
  }

  //! Returns true to indicate the need to copy edge;
  //! copies pcurve if requested
  Standard_Boolean NewCurve2d (const TopoDS_Edge& E, const TopoDS_Face& F,
                               const TopoDS_Edge& NewE, const TopoDS_Face& NewF,
                               Handle(Geom2d_Curve)& C, Standard_Real& Tol)
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
                                 Standard_Real& P, Standard_Real& Tol)
  {
    if (V.IsNull()) return Standard_False; // infinite edge may have Null vertex

    Tol = BRep_Tool::Tolerance(V);
    P = BRep_Tool::Parameter (V, E);

    return Standard_True;
  }

  //! Returns the  continuity of E between F1 and F2
  GeomAbs_Shape Continuity (const TopoDS_Edge& E, const TopoDS_Face& F1,
                            const TopoDS_Face& F2, const TopoDS_Edge&,
                            const TopoDS_Face&, const TopoDS_Face&)
  {
    return BRep_Tool::Continuity (E, F1, F2);
  }

public:
  DEFINE_STANDARD_RTTI(BRepBuilderAPI_Copy_Modification)

private: 
  Standard_Boolean myCopyGeom;
};

DEFINE_STANDARD_HANDLE(BRepBuilderAPI_Copy_Modification, BRepTools_Modification)
IMPLEMENT_STANDARD_HANDLE(BRepBuilderAPI_Copy_Modification, BRepTools_Modification)

IMPLEMENT_STANDARD_RTTIEXT(BRepBuilderAPI_Copy_Modification, BRepTools_Modification)

//=======================================================================
//function : BRepBuilderAPI_Copy
//purpose  : 
//=======================================================================

BRepBuilderAPI_Copy::BRepBuilderAPI_Copy ()
{
  myModification = new BRepBuilderAPI_Copy_Modification(Standard_True);
}


//=======================================================================
//function : BRepBuilderAPI_Copy
//purpose  : 
//=======================================================================

BRepBuilderAPI_Copy::BRepBuilderAPI_Copy(const TopoDS_Shape& S, const Standard_Boolean copyGeom)
{
  myModification = new BRepBuilderAPI_Copy_Modification(copyGeom);
  DoModif(S);
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepBuilderAPI_Copy::Perform(const TopoDS_Shape& S, const Standard_Boolean copyGeom)
{
  myModification = new BRepBuilderAPI_Copy_Modification(copyGeom);
  NotDone(); // on force la copie si on vient deja d`en faire une
  DoModif(S);
}

