// Created on: 1993-02-04
// Created by: Laurent BOURESCHE
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

#include <BRepSweep_Translation.hxx>

#include <GeomAdaptor_SurfaceOfLinearExtrusion.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepTools.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Sweep_NumShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

static void SetThePCurve(const BRep_Builder&         B,
                         TopoDS_Edge&                E,
                         const TopoDS_Face&          F,
                         const TopAbs_Orientation    O,
                         const occ::handle<Geom2d_Curve>& C)
{
  // check if there is already a pcurve on non planar faces
  double        f, l;
  occ::handle<Geom2d_Curve> OC;
  TopLoc_Location      SL;
  occ::handle<Geom_Plane>   GP = occ::down_cast<Geom_Plane>(BRep_Tool::Surface(F, SL));
  if (GP.IsNull())
    OC = BRep_Tool::CurveOnSurface(E, F, f, l);
  if (OC.IsNull())
    B.UpdateEdge(E, C, F, Precision::Confusion());
  else
  {
    if (O == TopAbs_REVERSED)
      B.UpdateEdge(E, OC, C, F, Precision::Confusion());
    else
      B.UpdateEdge(E, C, OC, F, Precision::Confusion());
  }
}

//=================================================================================================

BRepSweep_Translation::BRepSweep_Translation(const TopoDS_Shape&    S,
                                             const Sweep_NumShape&  N,
                                             const TopLoc_Location& L,
                                             const gp_Vec&          V,
                                             const bool C,
                                             const bool Canonize)
    : BRepSweep_Trsf(BRep_Builder(), S, N, L, C),
      myVec(V),
      myCanonize(Canonize)
{

  Standard_ConstructionError_Raise_if(V.Magnitude() < Precision::Confusion(),
                                      "BRepSweep_Translation::Constructor");
  Init();
}

//=================================================================================================

TopoDS_Shape BRepSweep_Translation::MakeEmptyVertex(const TopoDS_Shape&   aGenV,
                                                    const Sweep_NumShape& aDirV)
{
  // Only called when the option of construction is with copy.
  Standard_ConstructionError_Raise_if(!myCopy, "BRepSweep_Translation::MakeEmptyVertex");
  gp_Pnt P = BRep_Tool::Pnt(TopoDS::Vertex(aGenV));
  if (aDirV.Index() == 2)
    P.Transform(myLocation.Transformation());
  TopoDS_Vertex V;
  ////// modified by jgv, 5.10.01, for buc61008 //////
  // myBuilder.Builder().MakeVertex(V,P,Precision::Confusion());
  myBuilder.Builder().MakeVertex(V, P, BRep_Tool::Tolerance(TopoDS::Vertex(aGenV)));
  ////////////////////////////////////////////////////
  return V;
}

//=================================================================================================

TopoDS_Shape BRepSweep_Translation::MakeEmptyDirectingEdge(const TopoDS_Shape& aGenV,
                                                           const Sweep_NumShape&)
{
  gp_Pnt            P = BRep_Tool::Pnt(TopoDS::Vertex(aGenV));
  gp_Lin            L(P, myVec);
  occ::handle<Geom_Line> GL = new Geom_Line(L);
  TopoDS_Edge       E;
  myBuilder.Builder().MakeEdge(E, GL, BRep_Tool::Tolerance(TopoDS::Vertex(aGenV)));
  return E;
}

//=================================================================================================

TopoDS_Shape BRepSweep_Translation::MakeEmptyGeneratingEdge(const TopoDS_Shape&   aGenE,
                                                            const Sweep_NumShape& aDirV)
{
  // Call only in case of construction with copy.
  Standard_ConstructionError_Raise_if(!myCopy, "BRepSweep_Translation::MakeEmptyVertex");
  TopoDS_Edge newE;
  if (BRep_Tool::Degenerated(TopoDS::Edge(aGenE)))
  {
    myBuilder.Builder().MakeEdge(newE);
    myBuilder.Builder().UpdateEdge(newE, BRep_Tool::Tolerance(TopoDS::Edge(aGenE)));
    myBuilder.Builder().Degenerated(newE, true);
  }
  else
  {
    TopLoc_Location    L;
    double      First, Last;
    occ::handle<Geom_Curve> C = BRep_Tool::Curve(TopoDS::Edge(aGenE), L, First, Last);
    if (!C.IsNull())
    {
      C = occ::down_cast<Geom_Curve>(C->Copy());
      C->Transform(L.Transformation());
      if (aDirV.Index() == 2)
        C->Transform(myLocation.Transformation());
    }
    myBuilder.Builder().MakeEdge(newE, C, BRep_Tool::Tolerance(TopoDS::Edge(aGenE)));
  }
  return newE;
}

//=================================================================================================

void BRepSweep_Translation::SetParameters(const TopoDS_Shape& aNewFace,
                                          TopoDS_Shape&       aNewVertex,
                                          const TopoDS_Shape& aGenF,
                                          const TopoDS_Shape& aGenV,
                                          const Sweep_NumShape&)
{
  // Glue the parameter of vertices directly included in cap faces.
  gp_Pnt2d pnt2d = BRep_Tool::Parameters(TopoDS::Vertex(aGenV), TopoDS::Face(aGenF));
  myBuilder.Builder().UpdateVertex(TopoDS::Vertex(aNewVertex),
                                   pnt2d.X(),
                                   pnt2d.Y(),
                                   TopoDS::Face(aNewFace),
                                   Precision::PConfusion());
}

//=================================================================================================

void BRepSweep_Translation::SetDirectingParameter(const TopoDS_Shape& aNewEdge,
                                                  TopoDS_Shape&       aNewVertex,
                                                  const TopoDS_Shape&,
                                                  const Sweep_NumShape&,
                                                  const Sweep_NumShape& aDirV)
{
  double param = 0;
  if (aDirV.Index() == 2)
    param = myVec.Magnitude();
  myBuilder.Builder().UpdateVertex(TopoDS::Vertex(aNewVertex),
                                   param,
                                   TopoDS::Edge(aNewEdge),
                                   Precision::PConfusion());
}

//=================================================================================================

void BRepSweep_Translation::SetGeneratingParameter(const TopoDS_Shape& aNewEdge,
                                                   TopoDS_Shape&       aNewVertex,
                                                   const TopoDS_Shape& aGenE,
                                                   const TopoDS_Shape& aGenV,
                                                   const Sweep_NumShape&)
{
  TopoDS_Vertex vbid = TopoDS::Vertex(aNewVertex);
  vbid.Orientation(aGenV.Orientation());
  myBuilder.Builder().UpdateVertex(vbid,
                                   BRep_Tool::Parameter(TopoDS::Vertex(aGenV), TopoDS::Edge(aGenE)),
                                   TopoDS::Edge(aNewEdge),
                                   Precision::PConfusion());
}

//=================================================================================================

TopoDS_Shape BRepSweep_Translation::MakeEmptyFace(const TopoDS_Shape&   aGenS,
                                                  const Sweep_NumShape& aDirS)
{
  double        toler;
  TopoDS_Face          F;
  occ::handle<Geom_Surface> S;
  if (myDirShapeTool.Type(aDirS) == TopAbs_EDGE)
  {
    TopLoc_Location    L;
    double      First, Last;
    occ::handle<Geom_Curve> C = BRep_Tool::Curve(TopoDS::Edge(aGenS), L, First, Last);
    toler                = BRep_Tool::Tolerance(TopoDS::Edge(aGenS));
    gp_Trsf Tr           = L.Transformation();
    C                    = occ::down_cast<Geom_Curve>(C->Copy());
    // extruded surfaces are inverted correspondingly to the topology, so reverse.
    C->Transform(Tr);
    gp_Dir D(myVec);
    D.Reverse();

    if (myCanonize)
    {
      occ::handle<GeomAdaptor_Curve>            HC = new GeomAdaptor_Curve(C, First, Last);
      GeomAdaptor_SurfaceOfLinearExtrusion AS(HC, D);
      switch (AS.GetType())
      {

        case GeomAbs_Plane:
          S = new Geom_Plane(AS.Plane());
          break;
        case GeomAbs_Cylinder:
          S = new Geom_CylindricalSurface(AS.Cylinder());
          break;
        default:
          S = new Geom_SurfaceOfLinearExtrusion(C, D);
          break;
      }
    }
    else
    {
      S = new Geom_SurfaceOfLinearExtrusion(C, D);
    }
  }
  else
  {
    TopLoc_Location L;
    S          = BRep_Tool::Surface(TopoDS::Face(aGenS), L);
    toler      = BRep_Tool::Tolerance(TopoDS::Face(aGenS));
    gp_Trsf Tr = L.Transformation();
    S          = occ::down_cast<Geom_Surface>(S->Copy());
    S->Transform(Tr);
    if (aDirS.Index() == 2)
      S->Translate(myVec);
  }
  myBuilder.Builder().MakeFace(F, S, toler);
  return F;
}

//=================================================================================================

void BRepSweep_Translation::SetPCurve(const TopoDS_Shape& aNewFace,
                                      TopoDS_Shape&       aNewEdge,
                                      const TopoDS_Shape& aGenF,
                                      const TopoDS_Shape& aGenE,
                                      const Sweep_NumShape&,
                                      const TopAbs_Orientation)
{
  // Set on edges of cap faces the same pcurves as
  // edges of the generating face.
  bool isclosed = BRep_Tool::IsClosed(TopoDS::Edge(aGenE), TopoDS::Face(aGenF));
  if (isclosed)
  {
    double        First, Last;
    TopoDS_Edge          anE = TopoDS::Edge(aGenE.Oriented(TopAbs_FORWARD));
    occ::handle<Geom2d_Curve> aC1 = BRep_Tool::CurveOnSurface(anE, TopoDS::Face(aGenF), First, Last);
    anE.Reverse();
    occ::handle<Geom2d_Curve> aC2 = BRep_Tool::CurveOnSurface(anE, TopoDS::Face(aGenF), First, Last);
    myBuilder.Builder().UpdateEdge(TopoDS::Edge(aNewEdge),
                                   aC1,
                                   aC2,
                                   TopoDS::Face(aNewFace),
                                   Precision::PConfusion());
  }
  else
  {
    double First, Last;
    myBuilder.Builder().UpdateEdge(
      TopoDS::Edge(aNewEdge),
      BRep_Tool::CurveOnSurface(TopoDS::Edge(aGenE), TopoDS::Face(aGenF), First, Last),
      TopoDS::Face(aNewFace),
      Precision::PConfusion());
  }
}

//=================================================================================================

void BRepSweep_Translation::SetGeneratingPCurve(const TopoDS_Shape& aNewFace,
                                                TopoDS_Shape&       aNewEdge,
                                                const TopoDS_Shape&,
                                                const Sweep_NumShape&,
                                                const Sweep_NumShape&    aDirV,
                                                const TopAbs_Orientation orien)
{
  TopLoc_Location     Loc;
  GeomAdaptor_Surface AS(BRep_Tool::Surface(TopoDS::Face(aNewFace), Loc));
  //  double First,Last;
  gp_Lin2d    L;
  TopoDS_Edge aNewOrientedEdge = TopoDS::Edge(aNewEdge);
  aNewOrientedEdge.Orientation(orien);

  if (AS.GetType() == GeomAbs_Plane)
  {
    /* nothing is done JAG
        gp_Pln pln = AS.Plane();
        gp_Ax3 ax3 = pln.Position();

    // JYL : the following produces bugs on an edge constructed from a trimmed 3D curve :
    //
    //    occ::handle<Geom_Line>
    //      GL = occ::down_cast<Geom_Line>(BRep_Tool::Curve(TopoDS::Edge(aGenE),
    //							Loc,First,Last));
    //    gp_Lin gl = GL->Lin();
    //    gl.Transform(Loc.Transformation());
    //
    // correction :
        const TopoDS_Edge& EE = TopoDS::Edge(aGenE);
        BRepAdaptor_Curve BRAC(EE);
        gp_Lin gl = BRAC.Line();

        if(aDirV.Index()==2) gl.Translate(myVec);
        gp_Pnt pnt = gl.Location();
        gp_Dir dir = gl.Direction();
        double u,v;
        ElSLib::PlaneParameters(ax3,pnt,u,v);
        gp_Pnt2d pnt2d(u,v);
        gp_Dir2d dir2d(dir.Dot(ax3.XDirection()),dir.Dot(ax3.YDirection()));
        L.SetLocation(pnt2d);
        L.SetDirection(dir2d);
    */
  }
  else
  {
    double v = 0;
    if (aDirV.Index() == 2)
      v = -myVec.Magnitude();
    L.SetLocation(gp_Pnt2d(0, v));
    L.SetDirection(gp_Dir2d(gp_Dir2d::D::X));
    //  }
    occ::handle<Geom2d_Line> GL = new Geom2d_Line(L);
    SetThePCurve(myBuilder.Builder(), TopoDS::Edge(aNewEdge), TopoDS::Face(aNewFace), orien, GL);
  }
}

//=================================================================================================

void BRepSweep_Translation::SetDirectingPCurve(const TopoDS_Shape& aNewFace,
                                               TopoDS_Shape&       aNewEdge,
                                               const TopoDS_Shape& aGenE,
                                               const TopoDS_Shape& aGenV,
                                               const Sweep_NumShape&,
                                               const TopAbs_Orientation orien)
{
  TopLoc_Location     Loc;
  GeomAdaptor_Surface AS(BRep_Tool::Surface(TopoDS::Face(aNewFace), Loc));
  gp_Lin2d            L;
  if (AS.GetType() != GeomAbs_Plane)
  {
    L.SetLocation(gp_Pnt2d(BRep_Tool::Parameter(TopoDS::Vertex(aGenV), TopoDS::Edge(aGenE)), 0));
    L.SetDirection(gp_Dir2d(gp_Dir2d::D::NY));
    /* JAG
      }
      else{

        gp_Pln pln = AS.Plane();
        gp_Ax3 ax3 = pln.Position();
        gp_Pnt pv = BRep_Tool::Pnt(TopoDS::Vertex(aGenV));
        gp_Dir dir(myVec);
        double u,v;
        ElSLib::PlaneParameters(ax3,pv,u,v);
        gp_Pnt2d pnt2d(u,v);
        gp_Dir2d dir2d(dir.Dot(ax3.XDirection()),dir.Dot(ax3.YDirection()));
        L.SetLocation(pnt2d);
        L.SetDirection(dir2d);

      }
    */
    occ::handle<Geom2d_Line> GL = new Geom2d_Line(L);
    SetThePCurve(myBuilder.Builder(), TopoDS::Edge(aNewEdge), TopoDS::Face(aNewFace), orien, GL);
  }
}

//=================================================================================================

TopAbs_Orientation BRepSweep_Translation::DirectSolid(const TopoDS_Shape& aGenS,
                                                      const Sweep_NumShape&)
{
  // compare the face normal and the direction
  BRepAdaptor_Surface surf(TopoDS::Face(aGenS));
  gp_Pnt              P;
  gp_Vec              du, dv;
  surf.D1((surf.FirstUParameter() + surf.LastUParameter()) / 2.,
          (surf.FirstVParameter() + surf.LastVParameter()) / 2.,
          P,
          du,
          dv);

  double      x      = myVec.DotCross(du, dv);
  TopAbs_Orientation orient = (x > 0) ? TopAbs_REVERSED : TopAbs_FORWARD;
  return orient;
}

//=================================================================================================

bool BRepSweep_Translation::GGDShapeIsToAdd(const TopoDS_Shape&,
                                                        const TopoDS_Shape&,
                                                        const TopoDS_Shape&,
                                                        const TopoDS_Shape&,
                                                        const Sweep_NumShape&) const
{
  return true;
}

//=================================================================================================

bool BRepSweep_Translation::GDDShapeIsToAdd(const TopoDS_Shape&,
                                                        const TopoDS_Shape&,
                                                        const TopoDS_Shape&,
                                                        const Sweep_NumShape&,
                                                        const Sweep_NumShape&) const
{
  return true;
}

//=================================================================================================

bool BRepSweep_Translation::SeparatedWires(const TopoDS_Shape&,
                                                       const TopoDS_Shape&,
                                                       const TopoDS_Shape&,
                                                       const TopoDS_Shape&,
                                                       const Sweep_NumShape&) const
{
  return false;
}

//=================================================================================================

bool BRepSweep_Translation::HasShape(const TopoDS_Shape&   aGenS,
                                                 const Sweep_NumShape& aDirS) const
{
  if (myDirShapeTool.Type(aDirS) == TopAbs_EDGE)
  {

    if (myGenShapeTool.Type(aGenS) == TopAbs_EDGE)
    {
      TopoDS_Edge E = TopoDS::Edge(aGenS);

      // check if the edge is degenerated
      if (BRep_Tool::Degenerated(E))
      {
        return false;
      }
      // check if the edge is a sewing edge

      //  modified by NIZHNY-EAP Fri Dec 24 11:13:09 1999 ___BEGIN___
      //      const occ::handle<BRep_TEdge>& TE = *((occ::handle<BRep_TEdge>*) &E.TShape());

      //      NCollection_List<occ::handle<BRep_CurveRepresentation>>& lcr = TE->ChangeCurves();
      //      NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr(lcr);

      //      while (itcr.More()) {
      //	const occ::handle<BRep_CurveRepresentation>& cr = itcr.Value();
      //	if (cr->IsCurveOnSurface() &&
      //	    cr->IsCurveOnClosedSurface() ) 	{
      //	  std::cout<<"sewing edge"<<std::endl;
      //	  return false;
      //	}
      //	itcr.Next();
      //      }
      TopExp_Explorer FaceExp(myGenShape, TopAbs_FACE);
      for (; FaceExp.More(); FaceExp.Next())
      {
        TopoDS_Face F = TopoDS::Face(FaceExp.Current());
        if (BRepTools::IsReallyClosed(E, F))
          return false;
      }
      //  modified by NIZHNY-EAP Fri Dec 24 11:13:21 1999 ___END___
    }
  }

  return true;
}

//=================================================================================================

bool BRepSweep_Translation::IsInvariant(const TopoDS_Shape&) const
{
  return false;
}

//=================================================================================================

gp_Vec BRepSweep_Translation::Vec() const
{
  return myVec;
}
