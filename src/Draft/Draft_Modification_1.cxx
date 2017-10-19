// Created on: 1994-12-02
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

#include <Adaptor3d_CurveOnSurface.hxx>
#include <Adaptor3d_HCurveOnSurface.hxx>
#include <GeomAdaptor_SurfaceOfLinearExtrusion.hxx>
#include <Approx_CurveOnSurface.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepExtrema_ExtCF.hxx>
#include <BRepExtrema_ExtPC.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BRepTools.hxx>
#include <Draft_EdgeInfo.hxx>
#include <Draft_FaceInfo.hxx>
#include <Draft_Modification.hxx>
#include <Draft_VertexInfo.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <Extrema_ExtPC.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <Geom2dConvert.hxx>
#include <Geom2dConvert_CompCurveToBSplineCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Conic.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_ElementarySurface.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <GeomInt_IntSS.hxx>
#include <GeomProjLib.hxx>
#include <gp.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Parab.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <IntAna_IntConicQuad.hxx>
#include <IntAna_QuadQuadGeo.hxx>
#include <IntCurveSurface_HInter.hxx>
#include <IntCurveSurface_IntersectionPoint.hxx>
#include <Precision.hxx>
#include <ProjLib_CompProjectedCurve.hxx>
#include <ProjLib_HCompProjectedCurve.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Failure.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <StdFail_NotDone.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

static Standard_Boolean Choose(const Draft_IndexedDataMapOfFaceFaceInfo&,
  Draft_IndexedDataMapOfEdgeEdgeInfo&,
  const TopoDS_Vertex&,
  Draft_VertexInfo&,
  GeomAdaptor_Curve&,
  GeomAdaptor_Surface&);

static Standard_Real Parameter(const Handle(Geom_Curve)&,
  const gp_Pnt&,
  Standard_Integer&);

static Standard_Real SmartParameter(Draft_EdgeInfo&,
  const Standard_Real EdgeTol,
  const gp_Pnt&,
  const Standard_Integer,
  const Handle(Geom_Surface)&,
  const Handle(Geom_Surface)&);

static TopAbs_Orientation Orientation(const TopoDS_Shape&,
  const TopoDS_Face&);

static Standard_Boolean FindRotation(const gp_Pln&,
  const TopAbs_Orientation,
  const gp_Dir&,
  const Standard_Real,
  const gp_Pln&,
  gp_Ax1&,
  Standard_Real&);


//=======================================================================
//function : InternalAdd
//purpose  : 
//=======================================================================

Standard_Boolean Draft_Modification::InternalAdd(const TopoDS_Face& F,
  const gp_Dir& Direction,
  const Standard_Real Angle,
  const gp_Pln& NeutralPlane,
  const Standard_Boolean Flag)
{

  if (myFMap.Contains(F)) {
    return (badShape.IsNull());
  }

  TopAbs_Orientation oris = Orientation(myShape,F);
  TopLoc_Location Lo;
  //gp_Dir NewDirection = Direction;
  //Standard_Real NewAngle = Angle;
  Handle(Geom_Surface) S = BRep_Tool::Surface(F,Lo);
  S = Handle(Geom_Surface)::DownCast(S->Transformed(Lo.Transformation()));
  if (S->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
    S = Handle(Geom_RectangularTrimmedSurface)::DownCast(S)->BasisSurface();
  }
  Handle(Geom_Surface) NewS;
  Handle(Geom_Circle) theCircle;

  Standard_Boolean postponed = (Flag == Standard_False);
  if (postponed) {
    Handle(Standard_Type) typS = S->DynamicType();
    if (typS == STANDARD_TYPE(Geom_CylindricalSurface) ||
        typS == STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)) {
      gp_Circ Cir;
      if (typS == STANDARD_TYPE(Geom_CylindricalSurface)) {
        gp_Cylinder cyl = 
          Handle(Geom_CylindricalSurface)::DownCast(S)->Cylinder();
        gp_Ax1 axcyl = cyl.Axis();
        Cir = ElSLib::CylinderVIso( cyl.Position(), cyl.Radius(), 0.);
        gp_Vec VV(cyl.Location(),NeutralPlane.Location());
        Cir.Translate(VV.Dot(axcyl.Direction())*axcyl.Direction());
      }
      else {
        Handle(Geom_Curve) Cbas = 
          Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(S)->BasisCurve();
        gp_Dir theDirextr = 
          Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(S)->Direction();

        if (Cbas->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) {
          Cbas = Handle(Geom_TrimmedCurve)::DownCast(Cbas)->BasisCurve();
        }
        if (Cbas->IsKind(STANDARD_TYPE(Geom_Circle))) {
          Cir = Handle(Geom_Circle)::DownCast(Cbas)->Circ();
          gp_Dir dircir = Cir.Axis().Direction();
          if (!Direction.IsParallel(dircir,Precision::Angular())) {
            badShape = F;
            errStat = Draft_FaceRecomputation;
            return Standard_False;
          }
        }
        else {
          badShape = F;
          errStat = Draft_FaceRecomputation;
          return Standard_False;
        }

        gp_Ax3 Axis = NeutralPlane.Position();
        Standard_Real L =
          gp_Vec(Cir.Location(),Axis.Location()).
          Dot(Axis.Direction());
        Standard_Real Cos = theDirextr.Dot(Axis.Direction());
        gp_Vec VV = ( L / Cos) * theDirextr;
        Cir.Translate(VV);
      }

      theCircle = new Geom_Circle(Cir);

    }
    else {
      postponed = Standard_False;
    }
  }


  if (!postponed) {
    NewS = NewSurface(S,oris,Direction,Angle,NeutralPlane); 
    if (NewS.IsNull()) {
      badShape = F;
      errStat = Draft_FaceRecomputation;
      return Standard_False;
    }
    // To avoid some problems with infinite restrictions
    const Handle(Standard_Type)& typs = NewS->DynamicType();
    if (typs == STANDARD_TYPE(Geom_CylindricalSurface) ||
        typs == STANDARD_TYPE(Geom_ConicalSurface)) {
      Standard_Real umin,umax,vmin,vmax;
      BRepTools::UVBounds(F,umin,umax,vmin,vmax);
      if (!Precision::IsNegativeInfinite(vmin) &&
          !Precision::IsPositiveInfinite(vmax)) {
        Standard_Real deltav = 10.*(vmax-vmin);
        if(typs == STANDARD_TYPE(Geom_CylindricalSurface)) {
          vmin = vmin - deltav;
          vmax = vmax + deltav;
        }
        else {
          gp_Cone Co = Handle(Geom_ConicalSurface)::DownCast(NewS)->Cone();
          Standard_Real Vapex = - Co.RefRadius()/Sin(Co.SemiAngle());
          if (vmin < Vapex) { // vmax should not exceed Vapex
            if (vmax + deltav > Vapex) {
              vmax = Vapex;
              vmin = vmin - 10.*(vmax - vmin);
              // JAG debug to avoid apex
              vmax = vmax-Precision::Confusion();
            }
            else {
              vmin = vmin - deltav;
              vmax = vmax + deltav;
            }
          }
          else { // Vapex <= vmin < vmax
            if (vmin - deltav < Vapex) {
              vmin = Vapex;
              vmax = vmax + 10.*(vmax - vmin);
              // JAG debug to avoid apex
              vmin = vmin+Precision::Confusion();
            }
            else {
              vmin = vmin - deltav;
              vmax = vmax + deltav;
            }
          }
        }
        NewS = new Geom_RectangularTrimmedSurface(NewS,0.,2.*M_PI,vmin,vmax);
      }
    }
  }

  if (postponed || S != NewS) {
    Draft_FaceInfo FI(NewS,Standard_True);
    FI.RootFace(curFace);
    myFMap.Add(F,FI);
    if (postponed) {
      myFMap.ChangeFromKey(F).ChangeCurve() = theCircle;
    }
  }    

  TopExp_Explorer aExp(F,TopAbs_EDGE);
  TopTools_MapOfShape MapOfE;
  while (aExp.More() && badShape.IsNull()) {
    const TopoDS_Edge& edg = TopoDS::Edge(aExp.Current());
    if (!myEMap.Contains(edg)) {
      Standard_Boolean addedg  = Standard_False;
      Standard_Boolean addface = Standard_False;
      TopoDS_Face OtherF;
      //if (BRep_Tool::IsClosed(edg,F)) {
      if (BRepTools::IsReallyClosed(edg,F)) {
        addedg = Standard_True;
        addface = Standard_False;
      }
      else {
        // Find the other face containing the edge.
        TopTools_ListIteratorOfListOfShape it;
        it.Initialize(myEFMap.FindFromKey(edg));
        Standard_Integer nbother = 0;
        while (it.More()) {
          if (!it.Value().IsSame(F)) {
            if (OtherF.IsNull()) {
              OtherF = TopoDS::Face(it.Value());
            }
            nbother++;
          }
          it.Next();
        }	  
        if (nbother >=2) {
          badShape = edg;
          errStat = Draft_EdgeRecomputation;
        }
        else if (! OtherF.IsNull() && 
                 BRep_Tool::Continuity(edg,F,OtherF) >= GeomAbs_G1) {
          addface= Standard_True;
          addedg = Standard_True;
        }
        else if (nbother == 0) {
          //	    badShape = F;
        }
      }
      if (addedg) {
        if (postponed) {
          myFMap.ChangeFromKey(F).Add(OtherF);
        }
        Standard_Real f,l;
        TopLoc_Location L;
        Handle(Geom_Curve) C = BRep_Tool::Curve(edg,L,f,l);
        C = Handle(Geom_Curve)::DownCast(C->Transformed(L));
        if (C->DynamicType() == STANDARD_TYPE(Geom_TrimmedCurve)) {
          C = Handle(Geom_TrimmedCurve)::DownCast(C)->BasisCurve();
        }
        Handle(Geom_Curve) NewC;
        Draft_EdgeInfo EInf(Standard_True);	
        if(postponed) {
          EInf.Add(F);
          EInf.Add(OtherF);

          // find fixed point 
          Handle(Geom_Line) aLocalGeom = Handle(Geom_Line)::DownCast(C);
          if (aLocalGeom.IsNull()) {
            badShape = edg;
            errStat = Draft_EdgeRecomputation;
          }
          else {
            gp_Lin lin = aLocalGeom->Lin();
            IntAna_IntConicQuad ilipl(lin,NeutralPlane,Precision::Angular());
            if (ilipl.IsDone() && ilipl.NbPoints() != 0){
              EInf.Tangent(ilipl.Point(1));
            }
            else {
              badShape = edg;
              errStat = Draft_EdgeRecomputation;
            }
          }
        }
        else {
          NewC = NewCurve(C,S,oris,Direction,Angle,NeutralPlane, Flag);
          if (NewC.IsNull()) {
            badShape = edg;
            errStat = Draft_EdgeRecomputation;
          }
        }

        Handle(Geom_TrimmedCurve) T = Handle(Geom_TrimmedCurve)::DownCast(NewC);
        if (!T.IsNull()) NewC = T->BasisCurve();
        EInf.ChangeGeometry() = NewC;

        EInf.RootFace(curFace);
        myEMap.Add(edg,EInf);
        MapOfE.Add(edg);
        if (addface) {
          Standard_Boolean Fl = Flag;
          Handle(Geom_Surface) alocalSurface = BRep_Tool::Surface(OtherF,Lo);
          if (alocalSurface->DynamicType() == 
              STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
            alocalSurface = Handle(Geom_RectangularTrimmedSurface)::
              DownCast(alocalSurface)->BasisSurface();
          }
          Handle(Standard_Type) typS = alocalSurface->DynamicType();
          if (typS == STANDARD_TYPE(Geom_CylindricalSurface) || 
              typS == STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)) {
            if ( myFMap.Contains(F)) {
              if ( Flag == Standard_False && !postponed) {
                myFMap.RemoveKey(F);
                TopTools_MapIteratorOfMapOfShape itm(MapOfE);
                for ( ; itm.More(); itm.Next())
                {
                  myEMap.RemoveKey(TopoDS::Edge(itm.Key()));
                }
              }
            }
          } 
          InternalAdd(OtherF,Direction,Angle,NeutralPlane, Fl);
        }
      }
    }
    aExp.Next();
  }
  return (badShape.IsNull());
}


//=======================================================================
//function : Propagate
//purpose  : 
//=======================================================================

Standard_Boolean Draft_Modification::Propagate ()
{

  if (!badShape.IsNull()) return Standard_False;

  // Set all edges and vertices of modified faces
  TopoDS_Face F;
  TopoDS_Edge E;
  TopoDS_Vertex V;
  TopExp_Explorer editer;
  TopExp_Explorer vtiter;

  for (Standard_Integer i = 1; i <= myFMap.Extent(); i++)
  {
    const TopoDS_Face& Fc = myFMap.FindKey(i);

    // Exploration of the edges of the face
    editer.Init(Fc,TopAbs_EDGE);
    while (editer.More()) {
      E = TopoDS::Edge(editer.Current());

      if (!myEMap.Contains(E)) {
        Draft_EdgeInfo EInf(Standard_True);	
        myEMap.Add(E,EInf);
      }
      myEMap.ChangeFromKey(E).Add(Fc);

      // Exploration of the vertices of the edge
      vtiter.Init(E,TopAbs_VERTEX);
      while (vtiter.More()) {
        V = TopoDS::Vertex(vtiter.Current());
        if (!myVMap.Contains(V)) {
          Draft_VertexInfo VInf;
          myVMap.Add(V,VInf);
        }

        myVMap.ChangeFromKey(V).Add(E);
        myVMap.ChangeFromKey(V).ChangeParameter(E) = BRep_Tool::Parameter(V, E);
        vtiter.Next();
      }
      editer.Next();
    }
  }


  TopExp_Explorer anc;
  Standard_Boolean found;

  // Set edges containing modified vertices.

  for (Standard_Integer i = 1; i <= myVMap.Extent(); i++)
  {
    const TopoDS_Vertex& Vt = myVMap.FindKey(i);

    // Exploration of the ancestors of the vertex
    anc.Init(myShape,TopAbs_EDGE);

    while (anc.More()) {
      E = TopoDS::Edge(anc.Current());
      vtiter.Init(E,TopAbs_VERTEX);
      found = Standard_False;
      while (vtiter.More()) {
        if (Vt.IsSame(TopoDS::Vertex(vtiter.Current()))) {
          found = Standard_True;
          break;
        }
        vtiter.Next();
      }
      if (found) {
        if (!myEMap.Contains(E)) {
          Draft_EdgeInfo EInf(Standard_False);
          myEMap.Add(E,EInf);
        }
        myVMap.ChangeFromKey(Vt).Add(E);
        myVMap.ChangeFromKey(Vt).ChangeParameter(E) = BRep_Tool::Parameter(Vt, E);
      }
      anc.Next();
    }
  }


  // Set faces containing modified edges
  for (Standard_Integer i = 1; i <= myEMap.Extent(); i++)
  {
    const TopoDS_Edge& Ed = myEMap.FindKey(i);
    TopTools_ListIteratorOfListOfShape it;
    for (it.Initialize(myEFMap.FindFromKey(Ed)); it.More(); it.Next()) {
      F = TopoDS::Face(it.Value());
      if (!myFMap.Contains(F)) {
        TopLoc_Location L;
        Handle(Geom_Surface) S = BRep_Tool::Surface(F,L);
        Handle(Geom_Surface) NewS = 
          Handle(Geom_Surface)::DownCast(S->Transformed(L.Transformation()));

        const Handle(Standard_Type)& typs = S->DynamicType();
        if (typs == STANDARD_TYPE(Geom_CylindricalSurface) ||
            typs == STANDARD_TYPE(Geom_ConicalSurface)) {
          Standard_Real umin,umax,vmin,vmax;
          BRepTools::UVBounds(F,umin,umax,vmin,vmax);
          if (!Precision::IsNegativeInfinite(vmin) &&
              !Precision::IsPositiveInfinite(vmax)) {
            Standard_Real deltav = 10.*(vmax-vmin);
            vmin = vmin - deltav;
            vmax = vmax + deltav;
            NewS = new Geom_RectangularTrimmedSurface(NewS,0.,2.*M_PI,vmin,vmax);
          }
        }

        Draft_FaceInfo FInf(NewS,Standard_False);
        myFMap.Add(F,FInf);
      }
      myEMap.ChangeFromKey(Ed).Add(F);
    }
  }

  //  Try to add faces for free borders...
  // JAG 09.11.95
  for (Standard_Integer i = 1; i <= myEMap.Extent(); i++)
  {
    Draft_EdgeInfo& Einf = myEMap.ChangeFromIndex(i);
    if (Einf.NewGeometry() && 
        Einf.Geometry().IsNull() &&	
        Einf.SecondFace().IsNull()) {

      TopLoc_Location Loc;
      Handle(Geom_Surface) S1 = BRep_Tool::Surface(Einf.FirstFace(),Loc);
      S1 = Handle(Geom_Surface)::
        DownCast(S1->Transformed(Loc.Transformation()));
      Handle(Geom_Surface) S2;

      Standard_Real f,l;
      const TopoDS_Edge& EK = myEMap.FindKey(i);
      Handle(Geom_Curve) C = BRep_Tool::Curve(EK,Loc,f,l);
      C = Handle(Geom_Curve)::DownCast(C->Transformed(Loc.Transformation()));
      if (C->DynamicType() == STANDARD_TYPE(Geom_TrimmedCurve)) {
        C = Handle(Geom_TrimmedCurve)::DownCast(C)->BasisCurve();
      }
      if (!S1->IsKind(STANDARD_TYPE(Geom_Plane))) {
        if (C->IsKind(STANDARD_TYPE(Geom_Conic))) {
          gp_Ax3 thePl(Handle(Geom_Conic)::DownCast(C)->Position());
          S2 = new Geom_Plane(thePl);
        }
        else if (C->DynamicType() == STANDARD_TYPE(Geom_Line)) {
          gp_Ax1 axis;
          if (S1->DynamicType()== STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
            axis = Handle(Geom_ElementarySurface)::DownCast
              (Handle(Geom_RectangularTrimmedSurface)::DownCast(S1)->
              BasisSurface())->Axis();
          }
          else {
            axis = Handle(Geom_ElementarySurface)::DownCast(S1)->Axis();
          }
          gp_Vec they(axis.Location(), C->Value(0.));
          gp_Dir axz(axis.Direction().Crossed(they));
          S2=new Geom_Plane(gp_Ax3(axis.Location(),axz,axis.Direction()));

        }
        else {
          badShape = EK;
          errStat = Draft_EdgeRecomputation;
          break; // leave from for
        }
      }
      else { // on the plane
        for (Standard_Integer j = 1; j <= myVMap.Extent(); j++)
        {
          Draft_VertexInfo& Vinf = myVMap.ChangeFromIndex(j);
          for (Vinf.InitEdgeIterator();Vinf.MoreEdge();Vinf.NextEdge()) {
            if (Vinf.Edge().IsSame(EK)) {
              break;
            }
          }
          if (Vinf.MoreEdge()) {
            for (Vinf.InitEdgeIterator();Vinf.MoreEdge();Vinf.NextEdge()) {
              const TopoDS_Edge& edg = Vinf.Edge();
              if (!edg.IsSame(EK)) {
                const Draft_EdgeInfo& EI = myEMap.FindFromKey(edg);
                if (!EI.FirstFace().IsSame(Einf.FirstFace()) &&
                  (EI.SecondFace().IsNull() || 
                  !EI.SecondFace().IsSame(Einf.FirstFace()))) {
                    break;
                }
              }
            }
            if (Vinf.MoreEdge()) {
              Handle(Geom_Curve) C2 = BRep_Tool::Curve(Vinf.Edge(), Loc,f,l);
              Handle(GeomAdaptor_HCurve) HCur;
              gp_Vec Direc;
              C2 = Handle(Geom_Curve)::DownCast
                (C2->Transformed(Loc.Transformation()));
              if (C2->DynamicType() == STANDARD_TYPE(Geom_TrimmedCurve)) {
                C2 = Handle(Geom_TrimmedCurve)::DownCast(C2)->BasisCurve();
              }
              if (C->DynamicType() == STANDARD_TYPE(Geom_Line)) {
                Direc = Handle(Geom_Line)::DownCast(C)->Lin().Direction();
                HCur = new GeomAdaptor_HCurve(C2);

              }
              else if (C2->DynamicType() == STANDARD_TYPE(Geom_Line)) {
                Direc = Handle(Geom_Line)::DownCast(C2)->Lin().Direction();
                HCur = new GeomAdaptor_HCurve(C);
              }
              else {
                badShape = EK;
                errStat = Draft_EdgeRecomputation;
                break; // leave from while
              }
              GeomAdaptor_SurfaceOfLinearExtrusion SLE(HCur,Direc);
              switch(SLE.GetType()){

              case GeomAbs_Plane :
                {
                  S2 = new Geom_Plane(SLE.Plane());
                }
                break;
              case GeomAbs_Cylinder :
                {
                  S2 =   new Geom_CylindricalSurface(SLE.Cylinder());
                }
                break;
              default :
                {
                  S2 = new Geom_SurfaceOfLinearExtrusion(HCur->ChangeCurve().
                    Curve(),
                    Direc);
                }
                break;
              }

            }
            else {
              badShape = EK;
              errStat = Draft_EdgeRecomputation;
              break; // leave from while
            }
            break;
          }
          //j++;
        }
      }

      if (badShape.IsNull()) {
        BRep_Builder B;
        TopoDS_Face TheNewFace;
        B.MakeFace(TheNewFace,S2,Precision::Confusion());
        Einf.Add(TheNewFace);
        Draft_FaceInfo FI(S2,Standard_False);
        myFMap.Add(TheNewFace,FI);
      }
      else {
        break; // leave from for
      }
      // Fin JAG 09.11.95
    }
  }
  return (badShape.IsNull());
}




//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void Draft_Modification::Perform ()
{
  if (!badShape.IsNull())  throw Standard_ConstructionError();

  if (!myComp) {
    myComp = Standard_True;
    if (!Propagate()) {
      return;
    }

    // Calculate eventual faces

    for (Standard_Integer i = 1; i <= myFMap.Extent(); i++) 
    {
      const TopoDS_Face& FK = myFMap.FindKey(i);
      Draft_FaceInfo& Finf = myFMap.ChangeFromIndex(i);
      if (Finf.NewGeometry() && Finf.Geometry().IsNull()) {
        const TopoDS_Face& F1 = Finf.FirstFace();
        const TopoDS_Face& F2 = Finf.SecondFace();

        if (F1.IsNull() || F2.IsNull()) {
          errStat = Draft_FaceRecomputation;
          badShape = FK;
          return;
        }
        Handle(Geom_Surface) S1 = myFMap.FindFromKey(F1).Geometry();
        Handle(Geom_Surface) S2 = myFMap.FindFromKey(F2).Geometry();
        if (S1.IsNull() || S2.IsNull()) {
          errStat = Draft_FaceRecomputation;
          badShape = FK;
          return;
        }
        if (S1->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
          S1 = Handle(Geom_RectangularTrimmedSurface)::
            DownCast(S1)->BasisSurface();
        }
        if (S2->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
          S2 = Handle(Geom_RectangularTrimmedSurface)::
            DownCast(S2)->BasisSurface();
        }
        Handle(Geom_Plane) P1 = Handle(Geom_Plane)::DownCast(S1);
        Handle(Geom_Plane) P2 = Handle(Geom_Plane)::DownCast(S2);
        if (P1.IsNull() || P2.IsNull()) {
          errStat = Draft_FaceRecomputation;
          badShape = FK;
          return;
        }
        gp_Pln pp1 = P1->Pln();
        gp_Pln pp2 = P2->Pln();
        IntAna_QuadQuadGeo i2p(pp1,pp2,
          Precision::Angular(),Precision::Confusion());
        if (!i2p.IsDone() || i2p.TypeInter() != IntAna_Line) {
          errStat = Draft_FaceRecomputation;
          badShape = FK;
          return;
        }

        gp_Dir extrdir = i2p.Line(1).Direction();

        // Preserve the same direction as the base face
        Handle(Geom_Surface) RefSurf = 
          BRep_Tool::Surface(FK);
        if (RefSurf->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
          RefSurf = 
            Handle(Geom_RectangularTrimmedSurface)::DownCast(RefSurf)
            ->BasisSurface();
        }
        gp_Dir DirRef;

        if ( RefSurf->DynamicType() == STANDARD_TYPE(Geom_CylindricalSurface)) {
          gp_Ax3 AxeRef = 
            Handle(Geom_CylindricalSurface)::DownCast(RefSurf)
            ->Cylinder().Position();
          DirRef = AxeRef.Direction();
        }
        else if (RefSurf->DynamicType() == 
                 STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)) {
          DirRef = 
            Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(RefSurf)->Direction();
        }

        if (extrdir.Dot(DirRef) < 0.) extrdir.Reverse();

        // it is possible to accelerate speed by storing the info during
        // InternalAdd --> modification of FaceInfo to preserve the circle

        Handle(Geom_Circle) CCir = 
          Handle(Geom_Circle)::DownCast(Finf.Curve());
        Handle(Geom_Surface) NewS = 
          new Geom_SurfaceOfLinearExtrusion(CCir, extrdir);    

        Standard_Real umin, umax, vmin, vmax;
        BRepTools::UVBounds(FK,umin,umax,vmin,vmax);
        if (!Precision::IsNegativeInfinite(vmin) &&
            !Precision::IsPositiveInfinite(vmax)) {
          Standard_Real deltav = 2.*(vmax-vmin);
          vmin = vmin - deltav;
          vmax = vmax + deltav;
        }

        // very temporary
        else {
          vmax = 300;
          vmin = -300;
        }

        NewS = new Geom_RectangularTrimmedSurface(NewS,0.,1.9*M_PI,vmin,vmax);
        Finf.ChangeGeometry() = NewS;
      }
    }

    // Calculate new edges.
    for (Standard_Integer ii = 1; ii <= myEMap.Extent(); ii++) 
    {
      Draft_EdgeInfo& Einf = myEMap.ChangeFromIndex(ii); 

      const TopoDS_Edge& theEdge = TopoDS::Edge(myEMap.FindKey(ii));

      Handle(Geom_Surface) S1,S2;
      Handle(Geom_Curve) C, newC;
      Standard_Real f,l;
      TopLoc_Location L;
      C = BRep_Tool::Curve(theEdge,L,f,l);
      C = Handle(Geom_Curve)::DownCast(C->Transformed(L.Transformation()));

      if (Einf.NewGeometry() && Einf.Geometry().IsNull()) {
        gp_Pnt ptfixe;
        if (!Einf.IsTangent(ptfixe)) {
          const TopoDS_Face& FirstFace = Einf.FirstFace();
          const TopoDS_Face& SecondFace = Einf.SecondFace();

          S1 = myFMap.FindFromKey(FirstFace).Geometry();
          S2 = myFMap.FindFromKey(SecondFace).Geometry();

          Standard_Integer detrompeur = 0;

          // Return FirstVertex and the tangent at this point.
          TopoDS_Vertex FV = TopExp::FirstVertex(theEdge);
          TopoDS_Vertex LV = TopExp::LastVertex(theEdge);
          Standard_Real pmin = 0.;
          Standard_Real prmfv = BRep_Tool::Parameter(FV, theEdge);
          Standard_Real prmlv = BRep_Tool::Parameter(LV, theEdge);
          gp_Pnt pfv, plv;
          gp_Vec d1fv,d1lv, newd1;
          C->D1(prmfv,pfv,d1fv);
          C->D1(prmlv,plv,d1lv);

          Standard_Real TolF1 = BRep_Tool::Tolerance (FirstFace);
          Standard_Real TolF2 = BRep_Tool::Tolerance (SecondFace);

          //Pass the tolerance of the face to project
          GeomAPI_ProjectPointOnSurf proj1 (pfv, S1, TolF1);
          GeomAPI_ProjectPointOnSurf proj2 (plv, S1, TolF1);
          GeomAPI_ProjectPointOnSurf proj3 (pfv, S2, TolF2);
          GeomAPI_ProjectPointOnSurf proj4 (plv, S2, TolF2);

          if (proj1.IsDone () && proj2.IsDone ()) {
            if(proj1.LowerDistance()<= Precision::Confusion() &&
               proj2.LowerDistance()<= Precision::Confusion())  {
               detrompeur = 1;
            }
          }

          if (proj3.IsDone () && proj4.IsDone ()) {
            if(proj3.LowerDistance() <= Precision::Confusion() &&
               proj4.LowerDistance() <= Precision::Confusion())  {
               detrompeur = 2;
            }
          }

          gp_Dir TheDirExtr;
          gp_Ax3 Axis;
          Handle(Geom_Curve) TheNewCurve;
          Standard_Boolean KPart = Standard_False;

          if ( S1->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
            S1 = Handle(Geom_RectangularTrimmedSurface)::
              DownCast(S1)->BasisSurface();
          }
          if ( S2->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
            S2 = Handle(Geom_RectangularTrimmedSurface)::
              DownCast(S2)->BasisSurface();
          }

          Standard_Boolean PC1 = Standard_True; // KPart on S1
          if (S1->DynamicType() == STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion) &&
              S2->DynamicType() == STANDARD_TYPE(Geom_Plane) ) {
            KPart = Standard_True;
            Axis = Handle(Geom_Plane)::DownCast(S2)->Position();
            TheNewCurve = Handle(Geom_SurfaceOfLinearExtrusion)::
              DownCast(S1)->BasisCurve();
            TheDirExtr = Handle(Geom_SurfaceOfLinearExtrusion)::
              DownCast(S1)->Direction();
          }
          else if (S2->DynamicType() == STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion) &&
                   S1->DynamicType() == STANDARD_TYPE(Geom_Plane) ) {
            KPart = Standard_True;
            PC1 = Standard_False;
            Axis = Handle(Geom_Plane)::DownCast(S1)->Position();
            TheNewCurve = Handle(Geom_SurfaceOfLinearExtrusion)::
              DownCast(S2)->BasisCurve();
            TheDirExtr = Handle(Geom_SurfaceOfLinearExtrusion)::
              DownCast(S2)->Direction();
          }
          Handle(Geom_Circle) aCirc ;
          if ( KPart) {  // very temporary on circles !!!
            aCirc = Handle(Geom_Circle)::DownCast(TheNewCurve);
            if (aCirc.IsNull())
              KPart = Standard_False;
            else
            {
              gp_Dir AxofCirc = aCirc->Position().Direction();
              if (AxofCirc.IsParallel(Axis.Direction(),Precision::Angular()))
                KPart = Standard_True;
              else
                KPart = Standard_False;
            }
          }

          Standard_Integer imin;
          GeomInt_IntSS i2s;
          if ( KPart) {
            // direct calculation of NewC
            Standard_Real aLocalReal =
              gp_Vec(aCirc->Circ().Location(),Axis.Location()).
              Dot(Axis.Direction());
            Standard_Real Cos = TheDirExtr.Dot(Axis.Direction());
            gp_Vec VV = ( aLocalReal / Cos) * TheDirExtr;
            newC = Handle(Geom_Curve)::DownCast(TheNewCurve->Translated(VV));
            // it is possible to calculate PCurve
            Handle(Geom2d_Line) L2d 
              = new Geom2d_Line(gp_Pnt2d(0.,aLocalReal/Cos),
              gp::DX2d());

            if ( PC1) 
              Einf.ChangeFirstPC() = L2d;
            else
              Einf.ChangeSecondPC() = L2d;
          }
          else {
            S1 = myFMap.FindFromKey(Einf.FirstFace()).Geometry();
            S2 = myFMap.FindFromKey(Einf.SecondFace()).Geometry();


            // PCurves are not calculated immediately for 2 reasons:
            // 1 - If ProjLib should make an Approx, it is stupid to approximate the 
            //     entire intersection curve.
            // 2 - Additionally, if YaRev, there is a risk to not be SameRange.
            i2s.Perform(S1,S2,Precision::Confusion(),
              Standard_True,Standard_False,Standard_False);

            if (!i2s.IsDone() || i2s.NbLines() <= 0) {
              errStat = Draft_EdgeRecomputation;
              badShape = theEdge;
              return;
            }

            Standard_Real Glob2Min = RealLast();
            GeomAdaptor_Curve TheCurve;

            Standard_Integer i,j; //,jmin;

            if (i2s.Line(1)->DynamicType() != STANDARD_TYPE(Geom_BSplineCurve))
            {
              Standard_Real Dist2Min = RealLast();
              imin = 0;
              for (i=1; i<= i2s.NbLines(); i++) {
                TheCurve.Load(i2s.Line(i));
                Extrema_ExtPC myExtPC(pfv,TheCurve);

                Standard_Real locpmin = 0.;
                if (myExtPC.IsDone()) {
                  if(myExtPC.NbExt() >= 1) {
                    Dist2Min = myExtPC.SquareDistance(1);
                    locpmin = myExtPC.Point(1).Parameter();
                  }
                  if(myExtPC.NbExt() == 2 && Dist2Min > Precision::SquareConfusion()) {
                    //to avoid incorrectly choosing the image 
                    //of the first vertex of the initial edge
                    Standard_Real d1_2 = myExtPC.SquareDistance(1);
                    Standard_Real d2_2 = myExtPC.SquareDistance(2);
                    if(d1_2 > 1.21*d2_2) {
                      Dist2Min = myExtPC.SquareDistance(2);
                      locpmin = myExtPC.Point(2).Parameter();
                    }
                    else if(d2_2 > 1.21*d1_2) {
                      Dist2Min = myExtPC.SquareDistance(1);
                      locpmin = myExtPC.Point(1).Parameter();
                    }
                    else {
                      Standard_Real pfvpar = myExtPC.Point(1).Parameter();
                      Standard_Real plvpar = myExtPC.Point(2).Parameter();
                      newC = i2s.Line(i);

                      gp_Pnt pfvprim, plvprim;

                      newC->D0(pfvpar,pfvprim);
                      newC->D0(plvpar,plvprim);

                      Handle(Geom_Surface) theSurf;
                      if(detrompeur == 1) {
                        if(S1->DynamicType() == 
                           STANDARD_TYPE(Geom_RectangularTrimmedSurface))  
                          S1 = Handle(Geom_RectangularTrimmedSurface)::
                          DownCast(S1)->BasisSurface();
                        theSurf = S1;

                      }
                      else if(detrompeur == 2) {
                        if(S2->DynamicType() == 
                           STANDARD_TYPE(Geom_RectangularTrimmedSurface)) 
                          S2 = Handle(Geom_RectangularTrimmedSurface)::
                          DownCast(S2)->BasisSurface();
                        theSurf = S2;		    
                      }
                      if(detrompeur != 0 && detrompeur != 4) {
                        Standard_Real ul = 0., vl = 0., uf = 0., vf = 0.;
                        Standard_Real ufprim = 0., ulprim = 0., vfprim = 0., vlprim = 0.;

                        if(theSurf->DynamicType() == STANDARD_TYPE(Geom_Plane)) {	
                          gp_Pln pl = Handle(Geom_Plane)::DownCast(S2)->Pln();
                          ElSLib::Parameters(pl, plv, ul, vl);
                          ElSLib::Parameters(pl, pfv, uf, vf);
                          ElSLib::Parameters(pl, plvprim, ulprim, vlprim);
                          ElSLib::Parameters(pl, pfvprim, ufprim, vfprim);
                        }
                        else if(theSurf->DynamicType() == 
                                STANDARD_TYPE(Geom_CylindricalSurface)) {
                          gp_Cylinder cy = Handle(Geom_CylindricalSurface)
                            ::DownCast(S2)->Cylinder();
                          ElSLib::Parameters(cy, plv, ul, vl);
                          ElSLib::Parameters(cy, pfv, uf, vf);
                          ElSLib::Parameters(cy, plvprim, ulprim, vlprim);
                          ElSLib::Parameters(cy, pfvprim, ufprim, vfprim);
                        }
                        else detrompeur = 4;

                        if(detrompeur == 1 || detrompeur == 2) {
                          gp_Vec2d v1((ul-ufprim), (vl-vfprim));
                          gp_Vec2d norm((vf-vfprim), (ufprim-uf)); 
                          gp_Vec2d v2((ulprim-ufprim), (vlprim-vfprim));
                          if( (v1.Dot(norm))*(v2.Dot(norm)) < 0) {
                            Dist2Min = myExtPC.SquareDistance(2);
                            locpmin = myExtPC.Point(2).Parameter();
                          }
                        }
                      }
                    }
                  }
                  if (myExtPC.NbExt() == 1 || myExtPC.NbExt() > 2 || detrompeur ==4) {
                    Dist2Min = myExtPC.SquareDistance(1);
                    locpmin = myExtPC.Point(1).Parameter();
                    for (j=2; j<=myExtPC.NbExt(); j++) {
                      const Standard_Real Dist2 = myExtPC.SquareDistance(j);
                      if (Dist2 < Dist2Min) {
                        Dist2Min = Dist2;
                        locpmin = myExtPC.Point(j).Parameter();
                      }
                    }
                  }
                  else if(myExtPC.NbExt() < 1){
                    Standard_Real dist1_2,dist2_2;
                    gp_Pnt p1b,p2b;
                    myExtPC.TrimmedSquareDistances(dist1_2,dist2_2,p1b,p2b);
                    if (dist1_2 < dist2_2) {
                      Dist2Min = dist1_2;
                      locpmin = TheCurve.FirstParameter();
                    }
                    else {
                      Dist2Min = dist2_2;
                      locpmin = TheCurve.LastParameter();
                    }
                  }

                  if (Dist2Min  < Glob2Min) {
                    Glob2Min = Dist2Min;
                    imin = i;
                    pmin = locpmin;
                  }
                }
              }
              if (imin == 0) {
                errStat = Draft_EdgeRecomputation;
                badShape = theEdge;
                return;
              }

              newC = i2s.Line(imin);

              newC->D1(pmin,pfv,newd1);
              Standard_Boolean YaRev = d1fv.Dot(newd1) <0.; 

              if (YaRev)
                newC->Reverse();

              if (i2s.HasLineOnS1(imin)) {
                Einf.ChangeFirstPC() = i2s.LineOnS1(imin);
                if ( YaRev) 
                  Einf.ChangeFirstPC()->Reverse();
              }

              if (i2s.HasLineOnS2(imin)) {
                Einf.ChangeSecondPC() = i2s.LineOnS2(imin);
                if ( YaRev) 
                  Einf.ChangeSecondPC()->Reverse();
              }
            } // if (i2s.Line(1)->DynamicType() != STANDARD_TYPE(Geom_BSplineCurve))
            else // i2s.Line(1) is BSplineCurve
            {
              //Find the first curve to glue
              TColGeom_SequenceOfCurve Candidates;
              if (S1->DynamicType() == STANDARD_TYPE(Geom_CylindricalSurface) ||
                  S1->DynamicType() == STANDARD_TYPE(Geom_ConicalSurface)) 
              {
                for (i = 1; i <= i2s.NbLines(); i++)
                {
                  Handle( Geom_Curve ) aCurve = i2s.Line(i);
                  gp_Pnt Pnt = aCurve->Value( aCurve->FirstParameter() );
                  GeomAPI_ProjectPointOnSurf projector( Pnt, S1, Precision::Confusion() );
                  Standard_Real U, V;
                  projector.LowerDistanceParameters( U, V );
                  if (Abs(U) <= Precision::Confusion() || Abs(U-2.*M_PI) <= Precision::Confusion())
                    Candidates.Append( aCurve );
                  else
                  {
                    Pnt = aCurve->Value( aCurve->LastParameter() );
                    projector.Init( Pnt, S1, Precision::Confusion() );
                    projector.LowerDistanceParameters( U, V );
                    if (Abs(U) <= Precision::Confusion() || Abs(U-2.*M_PI) <= Precision::Confusion())
                    {
                      aCurve->Reverse();
                      Candidates.Append( aCurve );
                    }
                  }
                }

                if(Candidates.Length() == 0) 
                {
                  //errStat = Draft_EdgeRecomputation;
                  //badShape = TopoDS::Edge(ite.Key());
                  //return;
                  for (i = 1; i <= i2s.NbLines(); i++)
                    Candidates.Append( i2s.Line(i) );
                }
              }
              else 
              {
                for (i = 1; i <= i2s.NbLines(); i++)
                  Candidates.Append( i2s.Line(i) );
              }

              Handle( Geom_Curve ) FirstCurve;
              if (Candidates.Length() > 1)
              {
                Standard_Real DistMin = Precision::Infinite();
                for (i = 1; i <= Candidates.Length(); i++)
                {
                  Handle( Geom_Curve ) aCurve = Candidates(i);
                  gp_Pnt Pnt = aCurve->Value( aCurve->FirstParameter() );
                  const Standard_Real Dist = Pnt.Distance( pfv );
                  if (Dist - DistMin < -Precision::Confusion())
                  {
                    DistMin = Dist;
                    FirstCurve = aCurve;
                  }
                }
              }
              else
                FirstCurve = Candidates(1);

              //Glueing
              TColGeom_SequenceOfCurve Curves;
              for (i = 1; i <= i2s.NbLines(); i++)
                if (FirstCurve != i2s.Line(i))
                  Curves.Append( i2s.Line(i) );

              TColGeom_SequenceOfCurve ToGlue;
              gp_Pnt EndPoint = FirstCurve->Value( FirstCurve->LastParameter() );
              Standard_Boolean added = Standard_True;
              while (added)
              {
                added = Standard_False;
                for (i = 1; i <= Curves.Length(); i++)
                {
                  Handle( Geom_Curve ) aCurve = Curves(i);
                  gp_Pnt pfirst, plast;
                  pfirst = aCurve->Value( aCurve->FirstParameter() );
                  plast = aCurve->Value( aCurve->LastParameter() );
                  if (pfirst.Distance( EndPoint ) <= Precision::Confusion())
                  {
                    ToGlue.Append( aCurve );
                    EndPoint = plast;
                    Curves.Remove(i);
                    added = Standard_True;
                    break;
                  }
                  if (plast.Distance( EndPoint ) <= Precision::Confusion())
                  {
                    aCurve->Reverse();
                    ToGlue.Append( aCurve );
                    EndPoint = pfirst;
                    Curves.Remove(i);
                    added = Standard_True;
                    break;
                  }
                }
              }

              if (FirstCurve.IsNull()) {
                errStat = Draft_EdgeRecomputation;
                badShape = theEdge;
                return;
              }

              GeomConvert_CompCurveToBSplineCurve Concat( Handle(Geom_BSplineCurve)::DownCast(FirstCurve) );
              for (i = 1; i <= ToGlue.Length(); i++)
                Concat.Add( Handle(Geom_BSplineCurve)::DownCast(ToGlue(i)), Precision::Confusion(), Standard_True );

              newC = Concat.BSplineCurve();

              TheCurve.Load( newC );
              Extrema_ExtPC myExtPC( pfv, TheCurve );
              Standard_Real Dist2Min = RealLast();
              for (i = 1; i <= myExtPC.NbExt(); i++)
              {
                if (myExtPC.IsMin(i))
                {
                  const Standard_Real Dist2 = myExtPC.SquareDistance(i);
                  if (Dist2 < Dist2Min)
                  {
                    Dist2Min = Dist2;
                    pmin = myExtPC.Point(i).Parameter();
                  }
                }
              }
              newC->D1(pmin,pfv,newd1);
              Standard_Boolean YaRev = d1fv.Dot(newd1) < 0.; 

              if (YaRev)
                newC->Reverse();
              /*
              if (i2s.HasLineOnS1(imin)) {
              Einf.ChangeFirstPC() = i2s.LineOnS1(imin);
              if ( YaRev) 
              Einf.ChangeFirstPC()->Reverse();
              }

              if (i2s.HasLineOnS2(imin)) {
              Einf.ChangeSecondPC() = i2s.LineOnS2(imin);
              if ( YaRev) 
              Einf.ChangeSecondPC()->Reverse();
              }
              */
            } // else: i2s.NbLines() > 2 && S1 is Cylinder or Cone

            Einf.Tolerance(Max(Einf.Tolerance(), i2s.TolReached3d()));
          }  // End step KPart
        }
        else { // case of tangency
          const TopoDS_Face& F1 = Einf.FirstFace();
          const TopoDS_Face& F2 = Einf.SecondFace();

          Handle(Geom_Surface) aLocalS1 = myFMap.FindFromKey(F1).Geometry();
          Handle(Geom_Surface) aLocalS2 = myFMap.FindFromKey(F2).Geometry();
          if (aLocalS1.IsNull() || aLocalS2.IsNull()) {
            errStat = Draft_EdgeRecomputation;
            badShape = theEdge;
            return;
          }
          if (aLocalS1->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
            aLocalS1 = Handle(Geom_RectangularTrimmedSurface)::
              DownCast(aLocalS1)->BasisSurface();
          }
          if (aLocalS2->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
            aLocalS2 = Handle(Geom_RectangularTrimmedSurface)::
              DownCast(aLocalS2)->BasisSurface();
          }

          gp_Dir dirextr;
          //Standard_Boolean dirfound = Standard_False;
          if (aLocalS1->DynamicType() == STANDARD_TYPE(Geom_CylindricalSurface)) {
            gp_Cylinder cyl = 
              Handle(Geom_CylindricalSurface)::DownCast(aLocalS1)->Cylinder();
            dirextr = cyl.Axis().Direction();
            //dirfound = Standard_True;
            // see direction...

          }
          else if (aLocalS1->DynamicType() == STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)) {
            dirextr = Handle(Geom_SurfaceOfLinearExtrusion)::
              DownCast(aLocalS1)->Direction();
            //dirfound = Standard_True;
            // see direction...

            // Here it is possible to calculate PCurve.
            Handle(Geom_SurfaceOfLinearExtrusion) SEL = 
              Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(aLocalS1);
            Handle(Geom_Circle) GCir = 
              Handle(Geom_Circle)::DownCast(SEL->BasisCurve());
            if ( !GCir.IsNull()) {
              Standard_Real U = ElCLib::Parameter(GCir->Circ(),ptfixe);
              Handle(Geom2d_Line) PC1 = 
                new Geom2d_Line(gp_Pnt2d(U,0.),gp::DY2d());
              Einf.ChangeFirstPC() = PC1;
            }
          }

          else if (aLocalS2->DynamicType() == STANDARD_TYPE(Geom_CylindricalSurface)) {
            gp_Cylinder cyl = 
              Handle(Geom_CylindricalSurface)::DownCast(aLocalS2)->Cylinder();
            dirextr = cyl.Axis().Direction();
            // dirfound = Standard_True;
            // see direction...

          }
          else if (aLocalS2->DynamicType() == STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)) {
            dirextr = Handle(Geom_SurfaceOfLinearExtrusion)::
              DownCast(aLocalS2)->Direction();
            // dirfound = Standard_True;
            // see direction...

            // Here it is possible to calculate PCurve.
            Handle(Geom_SurfaceOfLinearExtrusion) SEL = 
              Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(aLocalS2);
            Handle(Geom_Circle) GCir = 
              Handle(Geom_Circle)::DownCast(SEL->BasisCurve());
            if ( !GCir.IsNull()) {
              Standard_Real U = ElCLib::Parameter(GCir->Circ(),ptfixe);
              Handle(Geom2d_Line) PC2 = 
                new Geom2d_Line(gp_Pnt2d(U,0.),gp::DY2d());
              Einf.ChangeSecondPC() = PC2;
            }
          }
          newC = new Geom_Line(ptfixe,dirextr);

          gp_Pnt pfv;
          gp_Vec d1fv,newd1;
          C->D1(0.,pfv,d1fv);
          newC->D1(0.,pfv,newd1);
          Standard_Boolean YaRev = d1fv.Dot(newd1) <0.; 
          if (YaRev) {
            newC->Reverse();
            if(!Einf.FirstPC().IsNull()) {
              Einf.ChangeFirstPC()->Reverse();
            }
            if(!Einf.SecondPC().IsNull()) {
              Einf.ChangeSecondPC()->Reverse();
            }
          }
        }

        Handle(Geom_TrimmedCurve) T = Handle(Geom_TrimmedCurve)::DownCast(newC);
        if (!T.IsNull()) newC = T->BasisCurve();
        Einf.ChangeGeometry() = newC;
      }
      else if (!Einf.NewGeometry()){
        // set existing curve 3D
        Handle(Geom_TrimmedCurve) T = Handle(Geom_TrimmedCurve)::DownCast(C);
        if (!T.IsNull()) C = T->BasisCurve();
        Einf.ChangeGeometry() = C;
      }
    }

    // Calculate new vertices.

    Handle(GeomAdaptor_HCurve)   HAC = new GeomAdaptor_HCurve;
    Handle(GeomAdaptor_HSurface) HAS = new GeomAdaptor_HSurface;

    for (Standard_Integer ii = 1; ii <= myVMap.Extent(); ii++)
    {
      GeomAdaptor_Curve    AC;
      GeomAdaptor_Surface  AS;

      const TopoDS_Vertex& TVV = myVMap.FindKey(ii);
      Draft_VertexInfo& Vinf = myVMap.ChangeFromIndex(ii);
      if (!Choose(myFMap,myEMap,TVV,Vinf,AC,AS)) {

        // no concerted edge => alignment of two consecutive edges.
        gp_Pnt pvt;
        Vinf.ChangeGeometry() = pvt;
        Vinf.InitEdgeIterator();
        if (Vinf.MoreEdge()) {
          const TopoDS_Edge& Edg1 = Vinf.Edge();
          //const Draft_EdgeInfo& Einf1 = myEMap(Edg1);
          Draft_EdgeInfo& Einf1 = myEMap.ChangeFromKey(Edg1);
          gp_Pnt vtori = BRep_Tool::Pnt(TVV);
          //Einf1.Geometry()->D0(Vinf.Parameter(Edg1), pvt);
          GeomAPI_ProjectPointOnCurve Projector( vtori, Einf1.Geometry() ); //patch
          pvt = Projector.NearestPoint();

#ifdef OCCT_DEBUG
          static Standard_Integer VertexRecomp = 1;
          if (VertexRecomp!=0) {
            cout << "pori :" << vtori.X() << " " << vtori.Y() << " " << vtori.Z() << endl;
            cout << "  Edg 1 :" << Vinf.Parameter(Edg1) << endl;
            cout << "pvt :" << pvt.X() << " " << pvt.Y() << " " << pvt.Z() << endl;
          }
#endif

          Standard_Real dion=pvt.SquareDistance(vtori);
          Vinf.NextEdge();
          if (Vinf.MoreEdge()) {
            const TopoDS_Edge& Edg2 = Vinf.Edge();
            //const Draft_EdgeInfo& Einf2 = myEMap(Edg2);
            Draft_EdgeInfo& Einf2 = myEMap.ChangeFromKey(Edg2);
            //	    Standard_Real f;
            gp_Pnt opvt;
            Einf2.Geometry()->D0(Vinf.Parameter(Edg2), opvt);

#ifdef OCCT_DEBUG
            if (VertexRecomp!=0) {
              cout << "  Edg 2 :" << Vinf.Parameter(Vinf.Edge()) << endl;
              cout << "opvt " << opvt.X() << " " << opvt.Y() << " " << opvt.Z() << endl;
            }
#endif

            if (opvt.SquareDistance(vtori) < dion) {
              pvt = opvt;
            }
            //Vinf.ChangeParameter(Edg2) = Parameter(Einf2.Geometry(), pvt);
            Standard_Integer done;
            Standard_Real param = Parameter(Einf2.Geometry(), pvt, done);
            if (done != 0)
            {
              Handle(Geom_Surface) S1 = myFMap.FindFromKey(Einf2.FirstFace()).Geometry();
              Handle(Geom_Surface) S2 = myFMap.FindFromKey(Einf2.SecondFace()).Geometry();
              Vinf.ChangeParameter(Edg2) = SmartParameter( Einf2, BRep_Tool::Tolerance(Edg2), pvt, done, S1, S2 );
            }
            else
              Vinf.ChangeParameter(Edg2) = param;
          }

          Vinf.ChangeGeometry() = pvt;
          //Vinf.ChangeParameter(Edg1) = Parameter(Einf1.Geometry(), pvt);
          Standard_Integer done;
          Standard_Real param = Parameter(Einf1.Geometry(), pvt, done);
          if (done != 0)
          {
            Handle(Geom_Surface) S1 = myFMap.FindFromKey(Einf1.FirstFace()).Geometry();
            Handle(Geom_Surface) S2 = myFMap.FindFromKey(Einf1.SecondFace()).Geometry();
            Vinf.ChangeParameter(Edg1) = SmartParameter( Einf1, BRep_Tool::Tolerance(Edg1), pvt, done, S1, S2 );
          }
          else
            Vinf.ChangeParameter(Edg1) = param;
          continue;
        }


        errStat = Draft_VertexRecomputation;
        badShape = TVV;
        return;
      }

      IntCurveSurface_HInter myintcs;
      HAC->Set(AC);
      HAS->Set(AS);

      myintcs.Perform(HAC,HAS);


      if (!myintcs.IsDone()) {
        errStat = Draft_VertexRecomputation;
        badShape = TVV;
        return;
      }

      gp_Pnt vtori = BRep_Tool::Pnt(TVV);
      gp_Pnt pvt;

      Standard_Integer nbsol = myintcs.NbPoints();
      if (nbsol <= 0)
      {
        Extrema_ExtCS extr( AC, AS, Precision::PConfusion(), Precision::PConfusion() );

        if(!extr.IsDone() || extr.NbExt() == 0) {
          errStat = Draft_VertexRecomputation;
          badShape = TVV;
          return;
        }


        Standard_Real disref = RealLast();
        Standard_Integer iref = 0;
        Extrema_POnCurv Pc;
        Extrema_POnSurf Ps;
        for (Standard_Integer i = 1; i <= extr.NbExt(); i++)
        {
          extr.Points( i, Pc, Ps );
          Standard_Real distemp = Pc.Value().SquareDistance(vtori);
          if ( distemp < disref)
          {
            disref = distemp;
            iref = i;
          }
        } 
        extr.Points( iref, Pc, Ps );
        pvt = Pc.Value();
      }
      else
      {
        Standard_Real disref = RealLast();
        Standard_Integer iref = 0;
        for (Standard_Integer i = 1; i <= nbsol; i++)
        {
          Standard_Real distemp = myintcs.Point(i).Pnt().SquareDistance(vtori);
          if ( distemp < disref)
          {
            disref = distemp;
            iref = i;
          }
        } 
        pvt = myintcs.Point(iref).Pnt();
      }

      Vinf.ChangeGeometry() = pvt;

      for (Vinf.InitEdgeIterator();Vinf.MoreEdge(); Vinf.NextEdge()) {
        const TopoDS_Edge& Edg = Vinf.Edge();
        Standard_Real initpar = Vinf.Parameter(Edg);
        //const Draft_EdgeInfo& Einf = myEMap(Edg);
        Draft_EdgeInfo& Einf = myEMap.ChangeFromKey(Edg);
        //Vinf.ChangeParameter(Edg) = Parameter(Einf.Geometry(),pvt);
        Standard_Integer done;
        Standard_Real param = Parameter(Einf.Geometry(), pvt, done);
        if (done != 0)
        {
          Handle(Geom_Surface) S1 = myFMap.FindFromKey(Einf.FirstFace()).Geometry();
          Handle(Geom_Surface) S2 = myFMap.FindFromKey(Einf.SecondFace()).Geometry();
          Vinf.ChangeParameter(Edg) = SmartParameter( Einf, BRep_Tool::Tolerance(Edg), pvt, done, S1, S2 );
        }
        else
        {
          if(Abs(initpar - param) > Precision::PConfusion())
          {
            Standard_Real f, l;
            TopLoc_Location Loc;
            const Handle(Geom_Curve)& aC = BRep_Tool::Curve(Edg, Loc, f, l);
            if(aC->DynamicType() == STANDARD_TYPE(Geom_TrimmedCurve))
            {
              Einf.SetNewGeometry(Standard_True);
            }
          }
          Vinf.ChangeParameter(Edg) = param;
        }
      }
    }
  }

  // small loop of validation/protection

  for (Standard_Integer i = 1; i <= myEMap.Extent(); i++) 
  {
    const TopoDS_Edge& edg = TopoDS::Edge(myEMap.FindKey(i));

    TopoDS_Vertex Vf,Vl;
    TopExp::Vertices(edg,Vf,Vl);
    if (edg.Orientation() == TopAbs_REVERSED) {
      Vf.Reverse();
      Vl.Reverse();
    }

    if(myVMap.Contains(Vf) && myVMap.Contains(Vl))
    {
      //Here, we compare directions of the source edge (from input shape)
      //and corresponding selected part of the intersection edge.
      //If these directions are opposite then we reverse intersection edge
      //and recompute corresponding vertex-parameters.

      Standard_Real aParF = myVMap.ChangeFromKey(Vf).Parameter(edg);
      Standard_Real aParL = myVMap.ChangeFromKey(Vl).Parameter(edg);

      if(aParL < aParF)
      {
        Draft_EdgeInfo& aEinf = myEMap.ChangeFromKey(edg);
        TopLoc_Location aLoc;
        Standard_Real aF = 0.0, aL = 0.0;
        const Handle(Geom_Curve) aSCurve = BRep_Tool::Curve(edg, aF, aL);
        Handle(Geom_Curve) anIntCurv = aEinf.Geometry();
        gp_Pnt aPf, aPl;
        gp_Vec aDirNF, aDirNL, aDirOF, aDirOL;
        aSCurve->D1(BRep_Tool::Parameter(Vf, edg), aPf, aDirOF);
        aSCurve->D1(BRep_Tool::Parameter(Vl, edg), aPl, aDirOL);
        anIntCurv->D1(aParF, aPf, aDirNF);
        anIntCurv->D1(aParL, aPl, aDirNL);

        Standard_Real aSqMagn = aDirNF.SquareMagnitude();

        if (aSqMagn > Precision::SquareConfusion())
          aDirNF.Divide(sqrt(aSqMagn));

        aSqMagn = aDirNL.SquareMagnitude();
        if (aSqMagn > Precision::SquareConfusion())
          aDirNL.Divide(sqrt(aSqMagn));

        aSqMagn = aDirOF.SquareMagnitude();
        if (aSqMagn > Precision::SquareConfusion())
          aDirOF.Divide(sqrt(aSqMagn));

        aSqMagn = aDirOL.SquareMagnitude();
        if (aSqMagn > Precision::SquareConfusion())
          aDirOL.Divide(sqrt(aSqMagn));

        const Standard_Real aCosF = aDirNF.Dot(aDirOF), aCosL = aDirNL.Dot(aDirOL);
        const Standard_Real aCosMax = Abs(aCosF) > Abs(aCosL) ? aCosF : aCosL;

        if(aCosMax < 0.0)
        {
          Standard_Integer anErr = 0;
          anIntCurv->Reverse();
          aEinf.ChangeGeometry() = anIntCurv;
          Standard_Real aPar = Parameter(aEinf.Geometry(), aPf, anErr);
          if(anErr == 0)
          {
            myVMap.ChangeFromKey(Vf).ChangeParameter(edg) = aPar;
          }
          aPar = Parameter(aEinf.Geometry(), aPl, anErr);
          if(anErr == 0)
          {
            myVMap.ChangeFromKey(Vl).ChangeParameter(edg) = aPar;
          }
        }
      }
    }

    Standard_Real pf,pl,tolerance;
    if (!NewParameter(Vf,edg,pf,tolerance)) {
      pf = BRep_Tool::Parameter(Vf,edg);
    }
    if (!NewParameter(Vl,edg,pl,tolerance)) {
      pl = BRep_Tool::Parameter(Vl,edg);
    }
    if (pl <= pf) {
      //      const Handle(Geom_Curve) gc=ite.Value().Geometry();
      //      if (!gc.IsNull()) {
      //	pl = gc->LastParameter();
      //	pf = gc->FirstParameter();
      //      }
      Handle( Geom_Curve ) theCurve = myEMap.FindFromKey(edg).Geometry();
      if (theCurve->IsClosed())
      {
        // pf >= pl
        Standard_Real FirstPar = theCurve->FirstParameter(), LastPar = theCurve->LastParameter();
        Standard_Real pconf = Precision::PConfusion();
        if (Abs( pf - LastPar ) <= pconf)
          pf = FirstPar;
        else if (Abs( pl - FirstPar ) <= pconf)
          pl = LastPar;

        if(pl <= pf) {
          pl += (LastPar-FirstPar);
        }

      }
      if (pl <= pf) {
        errStat = Draft_EdgeRecomputation;
        badShape = edg;
        return;
      }
    }
    if (myVMap.Contains( Vf ))
      myVMap.ChangeFromKey(Vf).ChangeParameter(edg) = pf;
    if (myVMap.Contains( Vl ))
      myVMap.ChangeFromKey(Vl).ChangeParameter(edg) = pl;
  }
}



//=======================================================================
//function : NewSurface
//purpose  : 
//=======================================================================

Handle(Geom_Surface) Draft_Modification::NewSurface
  (const Handle(Geom_Surface)& S,
  const TopAbs_Orientation Oris,
  const gp_Dir& Direction,
  const Standard_Real Angle,
  const gp_Pln& NeutralPlane) 
{
  Handle(Geom_Surface) NewS;

  Handle(Standard_Type) TypeS = S->DynamicType();

  if (TypeS == STANDARD_TYPE(Geom_Plane)) {
    gp_Pln Pl = Handle(Geom_Plane)::DownCast(S)->Pln();
    gp_Ax1 Axe;
    Standard_Real Theta;
    if (FindRotation(Pl,Oris,Direction,Angle,NeutralPlane,Axe,Theta)) {
      if ( Abs(Theta) > Precision::Angular()) {
        NewS = Handle(Geom_Surface)::DownCast(S->Rotated(Axe,Theta));
      }
      else {
        NewS = S;
      }
    }
  }
  else if (TypeS == STANDARD_TYPE(Geom_CylindricalSurface)) {
    Standard_Real testdir = Direction.Dot(NeutralPlane.Axis().Direction());
    if (Abs(testdir) <= 1.-Precision::Angular()) {	
#ifdef OCCT_DEBUG
      cout << "NewSurfaceCyl:Draft_Direction_and_Neutral_Perpendicular" << endl;
#endif
      return NewS;	
    }	  
    gp_Cylinder Cy = Handle(Geom_CylindricalSurface)::DownCast(S)->Cylinder();     
    testdir = Direction.Dot(Cy.Axis().Direction());
    if (Abs(testdir) <= 1.-Precision::Angular()) {
#ifdef OCCT_DEBUG
      cout << "NewSurfaceCyl:Draft_Direction_and_Cylinder_Perpendicular" << endl;
#endif
      return NewS;
    }
    if (Abs(Angle) > Precision::Angular())
    {
      IntAna_QuadQuadGeo i2s;
      i2s.Perform(NeutralPlane,Cy,Precision::Angular(),Precision::Confusion());
      Standard_Boolean isIntDone = i2s.IsDone();

      if(i2s.TypeInter() == IntAna_Ellipse)
      {
        const gp_Elips anEl = i2s.Ellipse(1);
        const Standard_Real aMajorR = anEl.MajorRadius();
        const Standard_Real aMinorR = anEl.MinorRadius();
        isIntDone = (aMajorR < 100000.0 * aMinorR);
      }

      if (!isIntDone || i2s.TypeInter() != IntAna_Circle) {
#ifdef OCCT_DEBUG
        cout << "NewSurfaceCyl:Draft_Intersection_Neutral_Cylinder_NotDone" << endl;
#endif
        return NewS;
      } 
      gp_Ax3 axcone = Cy.Position();
      // Pb : Where is the material???
      Standard_Real alpha = Angle;
      Standard_Boolean direct(axcone.Direct());
      if ((direct && Oris == TopAbs_REVERSED) ||
         (!direct && Oris == TopAbs_FORWARD)) {
        alpha = -alpha;
      }

      gp_Pnt Center = i2s.Circle(1).Location();
      if (testdir <0.) {
        alpha = -alpha;
      }
      Standard_Real Z = ElCLib::LineParameter(Cy.Axis(),Center);
      Standard_Real Rad = Cy.Radius()+Z*Tan(alpha);
      if (Rad < 0.) {
        Rad = -Rad;
      }
      else {
        alpha = -alpha;
      }
      gp_Cone co(axcone,alpha,Rad);
      NewS = new Geom_ConicalSurface(co);
    }
    else {
      NewS = S;
    }
  }
  else if (TypeS == STANDARD_TYPE(Geom_ConicalSurface)) {

    Standard_Real testdir = Direction.Dot(NeutralPlane.Axis().Direction());
    if (Abs(testdir) <= 1.-Precision::Angular()) {	
#ifdef OCCT_DEBUG
      cout << "NewSurfaceCone:Draft_Direction_and_Neutral_Perpendicular" << endl;
#endif
      return NewS;	
    }	

    gp_Cone Co1 = Handle(Geom_ConicalSurface)::DownCast(S)->Cone();

    testdir = Direction.Dot(Co1.Axis().Direction());
    if (Abs(testdir) <= 1.-Precision::Angular()) {
#ifdef OCCT_DEBUG
      cout << "NewSurfaceCone:Draft_Direction_and_Cone_Perpendicular" << endl;
#endif
      return NewS;
    }


    IntAna_QuadQuadGeo i2s;
    i2s.Perform(NeutralPlane,Co1,Precision::Angular(),Precision::Confusion());
    if (!i2s.IsDone() || i2s.TypeInter() != IntAna_Circle) {
#ifdef OCCT_DEBUG
      cout << "NewSurfaceCone:Draft_Intersection_Neutral_Conical_NotDone" << endl;
#endif
      return NewS;
    }
    gp_Ax3 axcone = Co1.Position();
    // Pb : Where is the material???
    Standard_Real alpha = Angle;
    Standard_Boolean direct(axcone.Direct());
    if ((direct && Oris == TopAbs_REVERSED) ||
       (!direct && Oris == TopAbs_FORWARD)) {
      alpha = -alpha;
    }

    gp_Pnt Center = i2s.Circle(1).Location();
    if (Abs(Angle) > Precision::Angular()) {
      if (testdir <0.) {
        alpha = -alpha;
      }
      Standard_Real Z = ElCLib::LineParameter(Co1.Axis(),Center);
      Standard_Real Rad = i2s.Circle(1).Radius()+Z*Tan(alpha);
      if (Rad < 0.) {
        Rad = -Rad;
      }
      else {
        alpha = -alpha;
      }
      if (Abs(alpha-Co1.SemiAngle()) < Precision::Angular()) {
        NewS = S;
      }
      else {
        gp_Cone co(axcone,alpha,Rad);
        NewS = new Geom_ConicalSurface(co);
      }
    }
    else {
      NewS = new
        Geom_CylindricalSurface(gp_Cylinder(axcone,i2s.Circle(1).Radius()));
    }
  }
  else {
#ifdef OCCT_DEBUG
    cout << "NewSurface:Draft_SurfNotYetImplemented" << endl;
#endif
  }
  return NewS;
} 


//=======================================================================
//function : NewCurve
//purpose  : 
//=======================================================================

Handle(Geom_Curve) Draft_Modification::NewCurve
  (const Handle(Geom_Curve)& C,
  const Handle(Geom_Surface)& S,
  const TopAbs_Orientation Oris,
  const gp_Dir& Direction,
  const Standard_Real Angle,
  const gp_Pln& NeutralPlane,
  const Standard_Boolean )

{
  Handle(Geom_Curve) NewC;

  Handle(Standard_Type) TypeS = S->DynamicType();

  if (TypeS == STANDARD_TYPE(Geom_Plane)) {
    gp_Pln Pl = Handle(Geom_Plane)::DownCast(S)->Pln();
    gp_Ax1 Axe;
    Standard_Real Theta;
    if (FindRotation(Pl,Oris,Direction,Angle,NeutralPlane,Axe,Theta)) {
      if ( Abs(Theta) > Precision::Angular()) {
        NewC = Handle(Geom_Curve)::DownCast(C->Rotated(Axe,Theta));
      }
      else {
        NewC = C;
      }
    }
    return NewC;
  }


  if (C->DynamicType() != STANDARD_TYPE(Geom_Line)) {
    return NewC;
  }


  gp_Lin lin = Handle(Geom_Line)::DownCast(C)->Lin();
  //  Standard_Real testdir = Direction.Dot(lin.Direction());
  //  if (Abs(testdir) <= 1.-Precision::Angular()) {
  //    return NewC;
  //  }
  gp_Dir Norm;
  if (TypeS == STANDARD_TYPE(Geom_CylindricalSurface)) {
    Standard_Real U,V;
    gp_Vec d1u,d1v;
    gp_Pnt pbid;
    gp_Cylinder Cy = Handle(Geom_CylindricalSurface)::DownCast(S)->Cylinder();
    ElSLib::Parameters(Cy,lin.Location(),U,V);
    ElSLib::D1(U,V,Cy,pbid,d1u,d1v);
    Norm = d1u.Crossed(d1v);
  }
  else if (TypeS == STANDARD_TYPE(Geom_ConicalSurface)) {
    Standard_Real U,V;
    gp_Vec d1u,d1v;
    gp_Pnt pbid;
    gp_Cone Co = Handle(Geom_ConicalSurface)::DownCast(S)->Cone();
    ElSLib::Parameters(Co,lin.Location(),U,V);
    ElSLib::D1(U,V,Co,pbid,d1u,d1v);
    Norm = d1u.Crossed(d1v);
  }

  IntAna_IntConicQuad ilipl(lin,NeutralPlane,Precision::Angular());
  if (ilipl.IsDone() && ilipl.NbPoints() != 0){
    if (Oris == TopAbs_REVERSED) {
      Norm.Reverse();
    }
    gp_Ax1 axrot(ilipl.Point(1), Norm.Crossed(Direction));
    gp_Lin lires = gp_Lin(gp_Ax1(ilipl.Point(1),Direction)).
      Rotated(axrot,Angle);
    if (lires.Direction().Dot(lin.Direction()) < 0.) {
      lires.Reverse();
    }
    NewC = new Geom_Line(lires);
  }
  return NewC;
}


//=======================================================================
//function : Choose
//purpose  : 
//=======================================================================

static Standard_Boolean Choose(const Draft_IndexedDataMapOfFaceFaceInfo& theFMap,
  Draft_IndexedDataMapOfEdgeEdgeInfo& theEMap,
  const TopoDS_Vertex& Vtx,
  Draft_VertexInfo& Vinf,
  GeomAdaptor_Curve& AC,
  GeomAdaptor_Surface& AS)
{
  gp_Vec tgref; 
  Vinf.InitEdgeIterator();

  // Find a regular edge with null SecondFace
  while (Vinf.MoreEdge()) {
    const TopoDS_Edge& E1 = Vinf.Edge();
    const Draft_EdgeInfo& Einf1 = theEMap.FindFromKey(E1);
    if (Einf1.SecondFace().IsNull()) {
      break;
    }
    else {
      GeomAbs_Shape te = BRep_Tool::Continuity(E1,Einf1.FirstFace(),
        Einf1.SecondFace());
      if (te >= GeomAbs_G1) {
        break;
      }
    }
    Vinf.NextEdge();
  }
  if (!Vinf.MoreEdge()) { // take the first edge
    Vinf.InitEdgeIterator();
  }

  const TopoDS_Edge& Eref = Vinf.Edge();
  //const Draft_EdgeInfo& Einf = theEMap(Eref);
  Draft_EdgeInfo& Einf = theEMap.ChangeFromKey(Eref);

  AC.Load(Einf.Geometry());

  Standard_Real f,l,prm;
  TopLoc_Location Loc;
  Handle(Geom_Curve) C = BRep_Tool::Curve(Eref,Loc,f,l);
  C = Handle(Geom_Curve)::DownCast(C->Transformed(Loc.Transformation()));
  gp_Pnt ptbid;
  //prm = Parameter(C,BRep_Tool::Pnt(Vtx));
  Standard_Integer done;
  Standard_Real param = Parameter( C, BRep_Tool::Pnt(Vtx), done );
  if (done != 0)
  {
    Handle( Geom_Surface ) S1 = theFMap.FindFromKey(Einf.FirstFace()).Geometry();
    Handle( Geom_Surface ) S2 = theFMap.FindFromKey(Einf.SecondFace()).Geometry();
    prm = SmartParameter( Einf, BRep_Tool::Tolerance(Eref), BRep_Tool::Pnt(Vtx), done, S1, S2 );
  }
  else
    prm = param;
  C->D1(prm,ptbid,tgref);


  Vinf.InitEdgeIterator();
  while (Vinf.MoreEdge()) {
    // Find a non tangent edge
    const TopoDS_Edge& Edg = Vinf.Edge();
    if (!Edg.IsSame(Eref)) {
      //const Draft_EdgeInfo& Einfo = theEMap(Edg);
      Draft_EdgeInfo& Einfo = theEMap.ChangeFromKey(Edg);
      if (!Einfo.SecondFace().IsNull() &&
          BRep_Tool::Continuity(Edg,Einfo.FirstFace(),Einfo.SecondFace()) 
          <= GeomAbs_C0) {
        C = BRep_Tool::Curve(Edg,Loc,f,l);
        C = Handle(Geom_Curve)::DownCast(C->Transformed(Loc.Transformation()));
        //prm = Parameter(C,BRep_Tool::Pnt(Vtx));
        Standard_Integer anewdone;
        Standard_Real anewparam = Parameter( C, BRep_Tool::Pnt(Vtx), anewdone );
        if (anewdone != 0)
        {
          Handle( Geom_Surface ) S1 = theFMap.FindFromKey(Einfo.FirstFace()).Geometry();
          Handle( Geom_Surface ) S2 = theFMap.FindFromKey(Einfo.SecondFace()).Geometry();
          prm = SmartParameter( Einfo, BRep_Tool::Tolerance(Edg), BRep_Tool::Pnt(Vtx), anewdone, S1, S2 );
        }
        else
          prm = anewparam;
        gp_Vec tg;
        C->D1(prm,ptbid,tg);
        if (tg.CrossMagnitude(tgref) > Precision::Confusion()) {
          break;
        }
      }
    }
    Vinf.NextEdge();
  }
  if (!Vinf.MoreEdge()) {
    return Standard_False;
  }

  const Draft_EdgeInfo& Einf2 = theEMap.FindFromKey(Vinf.Edge());
  if (!Einf.SecondFace().IsNull()) {

    if (Einf2.FirstFace().IsSame(Einf.FirstFace()) ||
        Einf2.FirstFace().IsSame(Einf.SecondFace())) {
      AS.Load(theFMap.FindFromKey(Einf2.SecondFace()).Geometry());
    }
    else {
      AS.Load(theFMap.FindFromKey(Einf2.FirstFace()).Geometry());
    }
  }
  else {
    if (Einf2.FirstFace().IsSame(Einf.FirstFace())) {
      AS.Load(theFMap.FindFromKey(Einf2.SecondFace()).Geometry());
    }
    else {
      AS.Load(theFMap.FindFromKey(Einf2.FirstFace()).Geometry());
    }
  }
  return Standard_True;
}


//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

static Standard_Real Parameter(const Handle(Geom_Curve)& C,
  const gp_Pnt& P,
  Standard_Integer& done)
{
  done = 0;
  Handle(Geom_Curve) cbase = C;
  Handle(Standard_Type) ctyp = C->DynamicType();
  if (ctyp == STANDARD_TYPE(Geom_TrimmedCurve)) {
    cbase = Handle(Geom_TrimmedCurve)::DownCast(C)->BasisCurve();
    ctyp = cbase->DynamicType();
  }
  Standard_Real param;
  if (ctyp == STANDARD_TYPE(Geom_Line)) {
    param = ElCLib::Parameter(Handle(Geom_Line)::DownCast(cbase)->Lin(),P);
  }
  else if (ctyp == STANDARD_TYPE(Geom_Circle)) {
    param = ElCLib::Parameter(Handle(Geom_Circle)::DownCast(cbase)->Circ(),P);
    if (Abs(2.*M_PI-param) <=Epsilon(2.*M_PI)) {
      param = 0.;
    }
  }
  else if (ctyp == STANDARD_TYPE(Geom_Ellipse)) {
    param = ElCLib::Parameter(Handle(Geom_Ellipse)::DownCast(cbase)->Elips(),P);
    if (Abs(2.*M_PI-param) <=Epsilon(2.*M_PI)) {
      param = 0.;
    }
  }
  else if (ctyp == STANDARD_TYPE(Geom_Parabola)) {
    param = ElCLib::Parameter(Handle(Geom_Parabola)::DownCast(cbase)->Parab(),P);
  }
  else if (ctyp == STANDARD_TYPE(Geom_Hyperbola)) {
    param = ElCLib::Parameter(Handle(Geom_Hyperbola)::DownCast(cbase)->Hypr(),P);
  }
  else {
    GeomAdaptor_Curve TheCurve(C);
    Extrema_ExtPC myExtPC(P,TheCurve);
    if (!myExtPC.IsDone()) {
      throw Standard_Failure("Draft_Modification_1::Parameter: ExtremaPC not done.");
    }
    if (myExtPC.NbExt() >= 1) {
      Standard_Real Dist2, Dist2Min = myExtPC.SquareDistance(1);
      Standard_Integer j, jmin = 1;
      for (j = 2; j <= myExtPC.NbExt(); j++) {
        Dist2 = myExtPC.SquareDistance(j);
        if (Dist2 < Dist2Min) {
          Dist2Min = Dist2;
          jmin = j;
        }
      }
      param = myExtPC.Point(jmin).Parameter();
    }
    else {
      Standard_Real dist1_2,dist2_2;
      gp_Pnt p1b,p2b;
      myExtPC.TrimmedSquareDistances(dist1_2,dist2_2,p1b,p2b);
      if (dist1_2 < dist2_2) {
        done = -1;
        param = TheCurve.FirstParameter();
      }
      else {
        done = 1;
        param = TheCurve.LastParameter();
      }
    }

    if (cbase->IsPeriodic()) {
      Standard_Real Per  = cbase->Period();
      Standard_Real Tolp = Precision::Parametric(Precision::Confusion());  
      if (Abs(Per-param) <= Tolp) {
        param = 0.;
      }
    }
  }
  return param;
}

//=======================================================================
//function : SmartParameter
//purpose  : 
//=======================================================================

static Standard_Real SmartParameter(Draft_EdgeInfo& Einf,
  const Standard_Real EdgeTol,
  const gp_Pnt& Pnt,
  const Standard_Integer sign,
  const Handle(Geom_Surface)& S1,
  const Handle(Geom_Surface)& S2)
{
  Handle( Geom2d_Curve ) NewC2d;
  Standard_Real Tol = Precision::Confusion();
  Standard_Real Etol = EdgeTol;

  Handle( Geom2d_Curve ) pcu1 = Einf.FirstPC();
  Handle( Geom2d_Curve ) pcu2 = Einf.SecondPC();

  if (pcu1.IsNull())
  {
    Handle( Geom_Curve ) theCurve = Einf.Geometry();
    pcu1 = GeomProjLib::Curve2d( theCurve, theCurve->FirstParameter(), theCurve->LastParameter(), S1, Etol );
    Einf.ChangeFirstPC() = pcu1;
  }
  if (pcu2.IsNull())
  {
    Handle( Geom_Curve ) theCurve = Einf.Geometry();
    pcu2 = GeomProjLib::Curve2d( theCurve, theCurve->FirstParameter(), theCurve->LastParameter(), S2, Etol );
    Einf.ChangeSecondPC() = pcu2;
  }

  GeomAPI_ProjectPointOnSurf Projector( Pnt, S1 );
  Standard_Real U, V;
  Projector.LowerDistanceParameters( U, V );

  NewC2d = Einf.FirstPC();
  if (NewC2d->DynamicType() == STANDARD_TYPE(Geom2d_TrimmedCurve))
    NewC2d = (Handle(Geom2d_TrimmedCurve)::DownCast(NewC2d))->BasisCurve();

  gp_Pnt2d P2d( U, V );
  Geom2dAPI_ProjectPointOnCurve Projector2d( P2d, NewC2d );
  if (Projector2d.NbPoints() == 0 || Projector2d.LowerDistance() > Tol)
  {
    Handle( Geom2d_BSplineCurve ) BCurve;
    if (NewC2d->DynamicType() != STANDARD_TYPE(Geom2d_BSplineCurve))
      BCurve = Geom2dConvert::CurveToBSplineCurve( NewC2d );
    else
      BCurve = Handle( Geom2d_BSplineCurve )::DownCast( NewC2d );
    if (sign == -1)
    {
      TColgp_Array1OfPnt2d PntArray( 1, 2 );
      PntArray(1) = P2d;
      PntArray(2) = BCurve->Pole(1);
      Handle( Geom2d_BezierCurve ) Patch = new Geom2d_BezierCurve( PntArray );
      Geom2dConvert_CompCurveToBSplineCurve Concat( BCurve, Convert_QuasiAngular );
      Concat.Add( Patch, Tol, Standard_False );
      BCurve = Concat.BSplineCurve();
    }
    else
    {
      TColgp_Array1OfPnt2d PntArray( 1, 2 );
      PntArray(1) = BCurve->Pole( BCurve->NbPoles() );
      PntArray(2) = P2d;
      Handle( Geom2d_BezierCurve ) Patch = new Geom2d_BezierCurve( PntArray );
      Geom2dConvert_CompCurveToBSplineCurve Concat( BCurve, Convert_QuasiAngular );
      Concat.Add( Patch, Tol, Standard_True );
      BCurve = Concat.BSplineCurve();
    }
    NewC2d = BCurve;
  }
  Einf.ChangeFirstPC() = NewC2d;
  Handle( Geom2dAdaptor_HCurve ) hcur = new Geom2dAdaptor_HCurve( NewC2d );
  Handle( GeomAdaptor_HSurface ) hsur = new GeomAdaptor_HSurface( S1 );
  Adaptor3d_CurveOnSurface cons( hcur, hsur );
  Handle( Adaptor3d_HCurveOnSurface ) hcons = new Adaptor3d_HCurveOnSurface( cons );
  Handle( GeomAdaptor_HSurface ) hsur2 = new GeomAdaptor_HSurface( S2 );
  ProjLib_CompProjectedCurve ProjCurve( hsur2, hcons, Tol, Tol );
  Handle(ProjLib_HCompProjectedCurve) HProjector = new ProjLib_HCompProjectedCurve();
  HProjector->Set( ProjCurve );
  Standard_Real Udeb, Ufin;
  ProjCurve.Bounds(1, Udeb, Ufin);
  Standard_Integer MaxSeg = 20 + HProjector->NbIntervals(GeomAbs_C3);
  Approx_CurveOnSurface appr( HProjector, hsur2, Udeb, Ufin, Tol,
    GeomAbs_C1, 10, MaxSeg, 
    Standard_False, Standard_False );
  Einf.ChangeSecondPC() = appr.Curve2d();
  Einf.ChangeGeometry() = appr.Curve3d();
  Einf.SetNewGeometry( Standard_True );

  if (sign == -1)
    return Einf.Geometry()->FirstParameter();
  else
    return Einf.Geometry()->LastParameter();

}

//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================

static TopAbs_Orientation Orientation(const TopoDS_Shape& S,
  const TopoDS_Face& F)
{
  //
  // change porting NT
  //
  TopExp_Explorer expl ;
  expl.Init(S,
    TopAbs_FACE) ;
  while (expl.More()) {
    if (TopoDS::Face(expl.Current()).IsSame(F)) {
      return expl.Current().Orientation();
    }
    expl.Next();
  }
  return TopAbs_FORWARD;
}


//=======================================================================
//function : FindRotation
//purpose  : 
//=======================================================================

static Standard_Boolean FindRotation(const gp_Pln& Pl,
  const TopAbs_Orientation Oris,
  const gp_Dir& Direction,
  const Standard_Real Angle,
  const gp_Pln& NeutralPlane,
  gp_Ax1& Axe,
  Standard_Real& theta)
{				     
  IntAna_QuadQuadGeo i2pl(Pl,NeutralPlane,
    Precision::Angular(),Precision::Confusion());

  if (i2pl.IsDone() && i2pl.TypeInter() == IntAna_Line) {
    gp_Lin li = i2pl.Line(1);
    // Try to turn around this line
    gp_Dir nx = li.Direction();
    gp_Dir ny = Pl.Axis().Direction().Crossed(nx);
    Standard_Real a = Direction.Dot(nx);
    if (Abs(a) <=1-Precision::Angular()) { 
      Standard_Real b = Direction.Dot(ny);
      Standard_Real c = Direction.Dot(Pl.Axis().Direction());
      Standard_Boolean direct(Pl.Position().Direct());
      if ((direct && Oris == TopAbs_REVERSED) ||
        (!direct && Oris == TopAbs_FORWARD)) {
          b = -b;
          c = -c;
      }
      Standard_Real denom = Sqrt(1-a*a);
      Standard_Real Sina = Sin(Angle);
      if (denom>Abs(Sina)) {
        Standard_Real phi = ATan2(b/denom,c/denom);
        Standard_Real theta0 = ACos(Sina/denom); 
        theta = theta0 - phi;
        if (Cos(theta) <0.) {
          theta = -theta0 -phi;
        }
        //  modified by NIZHNY-EAP Tue Nov 16 15:51:38 1999 ___BEGIN___
        while (Abs(theta)>M_PI) {
          theta = theta + M_PI*(theta<0 ? 1 : -1);
        }
        //  modified by NIZHNY-EAP Tue Nov 16 15:53:32 1999 ___END___
        Axe = li.Position();
        return Standard_True;
      }
    }
  }
  return Standard_False;
}

