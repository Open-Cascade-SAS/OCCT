// Created on: 1997-10-08
// Created by: Olga KOULECHOVA
// Copyright (c) 1997-1999 Matra Datavision
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


#include <Bnd_Box.hxx>
#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAlgo.hxx>
#include <BRepAlgoAPI_BooleanOperation.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBndLib.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepFeat.hxx>
#include <BRepFeat_Builder.hxx>
#include <BRepFeat_RibSlot.hxx>
#include <BRepIntCurveSurface_Inter.hxx>
#include <BRepLib.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <BRepLib_MakeWire.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <CSLib.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <Geom2dAPI_InterCurveCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Plane.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAPI.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomLib.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <LocOpe.hxx>
#include <LocOpe_CSIntersector.hxx>
#include <LocOpe_FindEdges.hxx>
#include <LocOpe_Gluer.hxx>
#include <LocOpe_PntFace.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TColGeom_SequenceOfCurve.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

#ifdef OCCT_DEBUG
extern Standard_Boolean BRepFeat_GettraceFEAT();
extern Standard_Boolean BRepFeat_GettraceFEATRIB();
#endif

//=======================================================================
//function : LFPerform
//purpose  : topological reconstruction of ribs 
//=======================================================================

void BRepFeat_RibSlot::LFPerform()
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
  if (trc) cout << "BRepFeat_RibSlot::LFPerform()" << endl;
#endif
  if (mySbase.IsNull() || myPbase.IsNull() || mySkface.IsNull() 
      || myGShape.IsNull() || myLFMap.IsEmpty()) {
#ifdef OCCT_DEBUG
    cout << "Topological reconstruction is impossible" << endl;
    if (trc) cout << " Fields not initialized" << endl;
#endif
    myStatusError = BRepFeat_NotInitialized;
    NotDone();
    return;
  }

  TopExp_Explorer exp,exp2;
  Standard_Integer theOpe = 2;

  if (!myGluedF.IsEmpty()) {
    theOpe = 1;
  }

      // Hope that there is just a solid in the result
  if (!mySUntil.IsNull()) {
    for (exp2.Init(mySUntil,TopAbs_FACE); exp2.More(); exp2.Next()) {
      const TopoDS_Shape& funtil = exp2.Current();
      for (exp.Init(mySbase,TopAbs_FACE); exp.More(); exp.Next()) {
        if (exp.Current().IsSame(funtil)) {
          break;
        }
      }
      if (!exp.More()) {
        break;
      }
    }
  }

  TopTools_ListIteratorOfListOfShape it,it2;
  TopTools_DataMapIteratorOfDataMapOfShapeShape itm;
  //Standard_Integer sens = 0;

  LocOpe_Gluer theGlue;
  
  //case of gluing 

  if (theOpe == 1) {
    Standard_Boolean Collage = Standard_True;  

    LocOpe_FindEdges theFE;
    TopTools_DataMapOfShapeListOfShape locmap;
    theGlue.Init(mySbase,myGShape);
    for (itm.Initialize(myGluedF); itm.More();itm.Next()) {
      const TopoDS_Face& glface = TopoDS::Face(itm.Key());
      const TopoDS_Face& fac = TopoDS::Face(myGluedF(glface));
      for (exp.Init(myGShape,TopAbs_FACE); exp.More(); exp.Next()) {
        if (exp.Current().IsSame(glface)) {
          break;
        }
      }
      if (exp.More()) {
        Collage = BRepFeat::IsInside(glface, fac);
        if(!Collage) {
          theOpe = 2;
          break;
        }
        else {
          theGlue.Bind(glface, fac);
          theFE.Set(glface, fac);
          for (theFE.InitIterator(); theFE.More();theFE.Next()) {
            theGlue.Bind(theFE.EdgeFrom(),theFE.EdgeTo());
          }
        }
      }
    }
  
    LocOpe_Operation ope = theGlue.OpeType();
    if (ope == LocOpe_INVALID ||
        (myFuse && ope != LocOpe_FUSE) ||
        (!myFuse && ope != LocOpe_CUT) ||
        (!Collage)) {
      theOpe = 2;
#ifdef OCCT_DEBUG
      cout << "Passage to topological operations" << endl;
#endif
    }
  }

// gluing is always applicable

  if (theOpe == 1) {
    theGlue.Perform();
    if (theGlue.IsDone()) {
      UpdateDescendants(theGlue);
      myNewEdges = theGlue.Edges();
      myTgtEdges = theGlue.TgtEdges();
      //
      Done();
      myShape = theGlue.ResultingShape();
      BRepLib::SameParameter(myShape, 1.e-7, Standard_True);
    }
    else {
      theOpe = 2;
#ifdef OCCT_DEBUG
      cout << "Passage to topologic operation" << endl;
#endif
    }
  }

  // case without gluing
  if (theOpe == 2) {
    BRepFeat_Builder theBuilder;
    TopTools_ListOfShape partsoftool;
    BRepClass3d_SolidClassifier oussa;
    Standard_Boolean bFlag;
    TopTools_ListIteratorOfListOfShape aIt;

    bFlag = (myPerfSelection == BRepFeat_NoSelection) ? 0 : 1;
    //
    theBuilder.Init(mySbase, myGShape);
    theBuilder.SetOperation(myFuse, bFlag);
    //
    theBuilder.Perform();
    if (bFlag) { 
      theBuilder.PartsOfTool(partsoftool);
      aIt.Initialize(partsoftool);
      if (aIt.More() && myPerfSelection != BRepFeat_NoSelection) {
        Standard_Real toler = (BRep_Tool::Tolerance(myPbase))*2;
        //
        for(; aIt.More(); aIt.Next()) {
          oussa.Load(aIt.Value());
          oussa.Perform(myFirstPnt, toler);
          TopAbs_State sp1=oussa.State();
          oussa.Perform(myLastPnt, toler);
          TopAbs_State sp2=oussa.State();
          if (!(sp1 == TopAbs_OUT || sp2 == TopAbs_OUT)) {
            const TopoDS_Shape& S = aIt.Value();
            theBuilder.KeepPart(S);
          }
        }
      }
      //
      theBuilder.PerformResult();
      myShape = theBuilder.Shape();
    } else {
      myShape = theBuilder.Shape();
    }
    Done();
  }
}

//=======================================================================
//function : IsDeleted
//purpose  : 
//=======================================================================

Standard_Boolean BRepFeat_RibSlot::IsDeleted(const TopoDS_Shape& F) 
{
  return (myMap(F).IsEmpty());
}


//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepFeat_RibSlot::Modified
   (const TopoDS_Shape& F)
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
  if (trc) cout << "BRepFeat_RibSlot::Modified" << endl;
#endif
  if (myMap.IsBound(F)) {
    static TopTools_ListOfShape list;
    list.Clear();
    TopTools_ListIteratorOfListOfShape ite(myMap(F));
    for(; ite.More(); ite.Next()) {
      const TopoDS_Shape& sh = ite.Value();
      if(!sh.IsSame(F)) 
        list.Append(sh);
    }
    return list;
  }
  return myGenerated; // empty list
}

//=======================================================================
//function : Generated
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepFeat_RibSlot::Generated
   (const TopoDS_Shape& S)
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
  if (trc) cout << "BRepFeat_RibSlot::Generated" << endl;
#endif
  if(S.ShapeType() != TopAbs_FACE) {
    myGenerated.Clear();
    if(myLFMap.IsEmpty() || !myLFMap.IsBound(S)) {
      if (myMap.IsBound(S)) { // check if filter on face or not
        static TopTools_ListOfShape list;
        list.Clear();
        TopTools_ListIteratorOfListOfShape ite(myMap(S));
        for(; ite.More(); ite.Next()) {
          const TopoDS_Shape& sh = ite.Value();
          if(!sh.IsSame(S)) 
            list.Append(sh);
        }
        return list;
      }
      else return myGenerated;
    }
    else {
      myGenerated.Clear();
      TopTools_ListIteratorOfListOfShape it(myLFMap(S));
      static TopTools_ListOfShape list;
      list.Clear();
      for(; it.More(); it.Next()) {
        if(myMap.IsBound(it.Value())) {
          TopTools_ListIteratorOfListOfShape it1(myMap(it.Value()));
          for(; it1.More(); it1.Next()) {
            const TopoDS_Shape& sh = it1.Value();
            if(!sh.IsSame(S)) 
              list.Append(sh);
          }
        }
      }
      return list;
    }
  }
  else return myGenerated;
} 


//=======================================================================
//function : UpdateDescendants
//purpose  : 
//=======================================================================

void BRepFeat_RibSlot::UpdateDescendants(const LocOpe_Gluer& G)
{
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itdm;
  TopTools_ListIteratorOfListOfShape it,it2;
  TopTools_MapIteratorOfMapOfShape itm;

  for (itdm.Initialize(myMap);itdm.More();itdm.Next()) {
    const TopoDS_Shape& orig = itdm.Key();
    TopTools_MapOfShape newdsc;
    for (it.Initialize(itdm.Value());it.More();it.Next()) {
      const TopoDS_Face& fdsc = TopoDS::Face(it.Value()); 
      for (it2.Initialize(G.DescendantFaces(fdsc));
           it2.More();it2.Next()) {
        newdsc.Add(it2.Value());
      }
    }
    myMap.ChangeFind(orig).Clear();
    for (itm.Initialize(newdsc);itm.More();itm.Next()) {
      myMap.ChangeFind(orig).Append(itm.Key());
    }
  }
}


//=======================================================================
//function : FirstShape
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepFeat_RibSlot::FirstShape() const
{
  if (!myFShape.IsNull()) {
    return myMap(myFShape);
  }
  return myGenerated; // empty list
}


//=======================================================================
//function : LastShape
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepFeat_RibSlot::LastShape() const
{
  if (!myLShape.IsNull()) {
    return myMap(myLShape);
  }
  return myGenerated; // empty list
}

//=======================================================================
//function : FacesForDraft
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepFeat_RibSlot::FacesForDraft() const
{
  return myFacesForDraft;
}


//=======================================================================
//function : NewEdges
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepFeat_RibSlot::NewEdges() const
{
 return myNewEdges;
}

//=======================================================================
//function : TgtEdges
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepFeat_RibSlot::TgtEdges() const
{
  return myTgtEdges;
}


//=======================================================================
//function : CurrentStatusError
//purpose  : 
//=======================================================================

BRepFeat_StatusError BRepFeat_RibSlot::CurrentStatusError() const
{
  return myStatusError;
}


//=======================================================================
//function : CheckPoint
//purpose  : Proofing point material side (side of extrusion)
//=======================================================================

gp_Pnt BRepFeat_RibSlot::CheckPoint(const TopoDS_Edge& e,
                                    const Standard_Real ,//bnd,
                                    const Handle(Geom_Plane)& Pln) 

{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEATRIB();
  if (trc) cout << "BRepFeat_RibSlot::CheckPoint" << endl;
#endif
// Vector product : normal to plane X direction Wire
// -> gives the material side
// Proofing point somewhat inside the material side
  Standard_Real f, l;
  Handle(Geom_Curve) cc = BRep_Tool::Curve(e, f, l);

  gp_Vec tgt; gp_Pnt pp;
  Standard_Real par = ( f + l) / 2.;
  
  cc->D1(par, pp, tgt);

  if ( e.Orientation() == TopAbs_REVERSED) tgt.Reverse();

  gp_Vec D = -tgt.Crossed(Pln->Pln().Position().Direction())/10.;
  pp.Translate(D);

  return pp;

}


//=======================================================================
//function : Normal
//purpose  : calculate the normal to a face in a point
//=======================================================================

gp_Dir BRepFeat_RibSlot::Normal(const TopoDS_Face& F,const gp_Pnt& P)

{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEATRIB();
  if (trc) cout << "BRepFeat_RibSlot::Normal" << endl;
#endif 
  Standard_Real U, V;
  gp_Pnt pt;

  BRepAdaptor_Surface AS(F, Standard_True);
    
  switch ( AS.GetType()) {
    
  case GeomAbs_Plane:
    ElSLib::Parameters(AS.Plane(),P,U,V); break;
    
  case GeomAbs_Cylinder:
    ElSLib::Parameters(AS.Cylinder(),P,U,V); break;

  case GeomAbs_Cone:
    ElSLib::Parameters(AS.Cone(),P,U,V); break;

  case GeomAbs_Torus:
    ElSLib::Parameters(AS.Torus(),P,U,V); break;
    
  default:
    {
      return gp_Dir(1., 0., 0.);
    }    
  }

  gp_Vec D1U, D1V;

  AS.D1(U, V, pt, D1U, D1V);             
  gp_Dir N;
  CSLib_DerivativeStatus St;
  CSLib::Normal(D1U, D1V, Precision::Confusion(), St, N);
  if(F.Orientation() == TopAbs_FORWARD) N.Reverse();
  return N;
}

//=======================================================================
//function : IntPar
//purpose  : calculate the parameter of a point on a curve
//=======================================================================

Standard_Real BRepFeat_RibSlot::IntPar(const Handle(Geom_Curve)& C,
                                       const gp_Pnt& P)

{
  if ( C.IsNull()) return 0.;

  GeomAdaptor_Curve AC(C);
  Standard_Real U;
  
  switch ( AC.GetType()) {
    
  case GeomAbs_Line:
    U = ElCLib::Parameter(AC.Line(),P); break;
    
  case GeomAbs_Circle:
    U = ElCLib::Parameter(AC.Circle(),P); break;
    
  case GeomAbs_Ellipse:
    U = ElCLib::Parameter(AC.Ellipse(),P); break;
    
  case GeomAbs_Hyperbola:
    U = ElCLib::Parameter(AC.Hyperbola(),P); break;
    
  case GeomAbs_Parabola:
    U = ElCLib::Parameter(AC.Parabola(),P); break;
    
  default:
    U = 0.;
  }

  return U;
}


//=======================================================================
//function : EdgeExtention
//purpose  : extention of a edge by tangence
//=======================================================================

void BRepFeat_RibSlot::EdgeExtention(TopoDS_Edge& e,
                                     const Standard_Real bnd,
                                     const Standard_Boolean FirstLast)
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
  if (trc) cout << "BRepFeat_RibSlot::EdgeExtention" << endl;
#endif
  Standard_Real f, l;
  Handle(Geom_Curve) cu = BRep_Tool::Curve(e, f, l);
  Handle(Geom_BoundedCurve) C = 
    new Geom_TrimmedCurve(cu, f, l);

  TopoDS_Edge E;

  if(cu->DynamicType() == STANDARD_TYPE(Geom_Line) ||
     cu->DynamicType() == STANDARD_TYPE(Geom_Circle) ||
     cu->DynamicType() == STANDARD_TYPE(Geom_Ellipse) ||
     cu->DynamicType() == STANDARD_TYPE(Geom_Hyperbola) ||
     cu->DynamicType() == STANDARD_TYPE(Geom_Parabola)) {
    if(FirstLast) {
      BRepLib_MakeEdge Edg(cu, f-bnd/10., l);      
      E = TopoDS::Edge(Edg.Shape());
    }
    else { 
      BRepLib_MakeEdge Edg(cu, f, l+bnd/10.); 
      E = TopoDS::Edge(Edg.Shape());
    }
  }
  else {
    Handle(Geom_Line) ln;
    gp_Pnt Pt;
    gp_Pnt pnt;
    gp_Vec vct;
    if(FirstLast) {
      C->D1(f, pnt, vct);
      ln = new Geom_Line(pnt, -vct);
      ln->D0(bnd/1000., Pt); 
      GeomLib::ExtendCurveToPoint(C, Pt, GeomAbs_G1, Standard_False);
      BRepLib_MakeEdge Edg(C, Pt, BRep_Tool::Pnt(TopExp::LastVertex(e,Standard_True)));
      E = TopoDS::Edge(Edg.Shape());
    }
    else {
      C->D1(l, pnt, vct);
      ln = new Geom_Line(pnt, vct);
      ln->D0(bnd/1000., Pt); 
      GeomLib::ExtendCurveToPoint(C, Pt, GeomAbs_G1, Standard_True);
      BRepLib_MakeEdge Edg(C, BRep_Tool::Pnt(TopExp::FirstVertex(e,Standard_True)), Pt);
      E = TopoDS::Edge(Edg.Shape());
    }
  }
  e = E;
}


//=======================================================================
//function : ChoiceOfFaces
//purpose  : choose face of support in case of support on an edge
//=======================================================================

TopoDS_Face BRepFeat_RibSlot::ChoiceOfFaces(TopTools_ListOfShape& faces,
                                            const Handle(Geom_Curve)& cc,
                                            const Standard_Real par,
                                            const Standard_Real ,//bnd,
                                            const Handle(Geom_Plane)& Pln)

{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEATRIB();
  if (trc) cout << "BRepFeat_RibSlot::ChoiceOfFaces" << endl;
#endif
  TopoDS_Face FFF;

  gp_Pnt pp;
  gp_Vec tgt;

  cc->D1(par, pp, tgt);

  Handle(Geom_Line) l1 = new Geom_Line(pp, tgt);

  TColGeom_SequenceOfCurve scur;
  Standard_Integer Counter = 0;
  

  gp_Ax1 Axe(pp, Pln->Position().Direction());
  for ( Standard_Integer i = 1; i <=8; i++) {
    Handle(Geom_Curve) L = 
      Handle(Geom_Curve)::DownCast(l1->Rotated(Axe, i*M_PI/9.));
    scur.Append(L);
    Counter++;
  }

  TopTools_ListIteratorOfListOfShape it;
  it.Initialize(faces);
  Standard_Real Par = RealLast();
  for(; it.More(); it.Next()) {
    const TopoDS_Face& f = TopoDS::Face(it.Value());
    LocOpe_CSIntersector ASI(f);
    ASI.Perform(scur);
    if(!ASI.IsDone()) continue;
    for(Standard_Integer jj = 1; jj<=Counter; jj++) {
      if(ASI.NbPoints(jj) >= 1) {
        Standard_Real app = ASI.Point(jj,1).Parameter();
        if(app >= 0 &&  app < Par) {
          Par = app;
          FFF = f;
        }
      }
    }
  }
      
  return FFF;      
}


//=======================================================================
//function : HeightMax
//purpose  : Calculate the height of the prism following the parameters of a bounding box
//=======================================================================

Standard_Real BRepFeat_RibSlot::HeightMax(const TopoDS_Shape& theSbase,
                                          const TopoDS_Shape& theSUntil,
                                          gp_Pnt& p1, 
                                          gp_Pnt& p2)
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEATRIB();
  if (trc) cout << "BRepFeat_RibSlot::HeightMax" << endl;
#endif
  Bnd_Box Box;
  BRepBndLib::Add(theSbase,Box);
  if(!theSUntil.IsNull()) {
    BRepBndLib::Add(theSUntil,Box);
  }
  Standard_Real c[6], bnd;
  Box.Get(c[0],c[2],c[4],c[1],c[3],c[5]);
  bnd = c[0];
  for(Standard_Integer i = 0 ; i < 6; i++) {
    if(c[i] > bnd) bnd = c[i];
  }
  p1.SetCoord(c[0]-2.*bnd, c[1]-2.*bnd, c[2]-2.*bnd);
  p2.SetCoord(c[3]+2.*bnd, c[4]+2.*bnd, c[5]+2.*bnd);
  return(bnd);
}

//=======================================================================
//function : ExtremeFaces
//purpose  : Calculate the base faces of the rib
//=======================================================================

Standard_Boolean BRepFeat_RibSlot::ExtremeFaces(const Standard_Boolean RevolRib,
                                                const Standard_Real bnd,
                                                const Handle(Geom_Plane)& Pln,
                                                TopoDS_Edge&   FirstEdge,
                                                TopoDS_Edge&   LastEdge,
                                                TopoDS_Face&   FirstFace,
                                                TopoDS_Face&   LastFace,
                                                TopoDS_Vertex& FirstVertex,
                                                TopoDS_Vertex& LastVertex, 
                                                Standard_Boolean& OnFirstFace,
                                                Standard_Boolean& OnLastFace,
                                                Standard_Boolean& PtOnFirstEdge,
                                                Standard_Boolean& PtOnLastEdge,
                                                TopoDS_Edge& OnFirstEdge,
                                                TopoDS_Edge& OnLastEdge)

{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
  if (trc) cout << "BRepFeat_RibSlot::ExtremeFaces" << endl;
#endif
  Standard_Boolean Data = Standard_True;
  FirstFace.Nullify();
  LastFace.Nullify();
  FirstEdge.Nullify();
  LastEdge.Nullify();
  PtOnFirstEdge = Standard_False;
  PtOnLastEdge = Standard_False;
  OnFirstEdge.Nullify();
  OnLastEdge.Nullify();

  BRepIntCurveSurface_Inter inter;
  BRep_Builder B;
  TopExp_Explorer ex1;

  Standard_Boolean FirstOK = Standard_False, LastOK = Standard_False;

  Standard_Integer NumberOfEdges = 0;
  TopExp_Explorer exp(myWire, TopAbs_EDGE);
  
  for(; exp.More(); exp.Next()) {
    NumberOfEdges++;
  }

// ---the wire includes only one edge
  if(NumberOfEdges == 1) {
#ifdef OCCT_DEBUG
    if (trc) cout << " One Edge" << endl;
#endif
    exp.ReInit();
    Standard_Real f, l;//, f1, l1, temp;
    gp_Pnt firstpoint, lastpoint;
   
// Points limit the unique edge
    const TopoDS_Edge& E = TopoDS::Edge(exp.Current());
    Handle(Geom_Curve) cc = BRep_Tool::Curve(E, f, l);
    gp_Pnt p1 = BRep_Tool::Pnt(TopExp::FirstVertex(E,Standard_True));
    gp_Pnt p2 = BRep_Tool::Pnt(TopExp::LastVertex(E,Standard_True));

    Standard_Real FirstPar = f; Standard_Real LastPar = l;


// ---Find if 2 points limiting the unique edge of the wire
//    are on an edge or a vertex of the base shape
    Standard_Boolean PtOnFirstVertex = Standard_False; 
    Standard_Boolean PtOnLastVertex = Standard_False;
    TopoDS_Vertex OnFirstVertex, OnLastVertex;
    PtOnEdgeVertex(RevolRib, mySbase, p1, FirstVertex, LastVertex,
                   PtOnFirstEdge, OnFirstEdge, PtOnFirstVertex, OnFirstVertex);
    PtOnEdgeVertex(RevolRib, mySbase, p2, FirstVertex, LastVertex,
                   PtOnLastEdge, OnLastEdge, PtOnLastVertex, OnLastVertex);

    TopTools_MapOfShape Map;

    if(PtOnFirstEdge) {
      if (!PtOnFirstVertex) {
// Find FirstFace : face of the base shape containing OnFirstEdge
//                  meeting ChoiceOfFaces
        TopExp_Explorer ex4, ex5;
        ex4.Init(mySbase, TopAbs_FACE);
        TopTools_ListOfShape faces;
        faces.Clear();
        Map.Clear();
        for(; ex4.More(); ex4.Next()) {
          const TopoDS_Face& fx = TopoDS::Face(ex4.Current());
          if ( !Map.Add(fx)) continue;
          ex5.Init(ex4.Current(), TopAbs_EDGE);
          for(; ex5.More(); ex5.Next()) {
            const TopoDS_Edge& ee = TopoDS::Edge(ex5.Current()); 
            if(ee.IsSame(OnFirstEdge)) {
              faces.Append(fx);
            }
          }
        }
        if(!faces.IsEmpty())  {
          TopoDS_Face FFF = ChoiceOfFaces(faces, cc, FirstPar+bnd/50., bnd/50., Pln);
          if(!FFF.IsNull()) FirstFace = FFF;
        }
      }
      else if(PtOnFirstVertex) {
// Find FirstFace : face of the base shape containing OnFirstVertex
//                  meeting ChoiceOfFaces
        TopExp_Explorer ex4, ex5;
        ex4.Init(mySbase, TopAbs_FACE);
        TopTools_ListOfShape faces;
        faces.Clear();
        Map.Clear();
        for(; ex4.More(); ex4.Next()) {
          const TopoDS_Face& fx = TopoDS::Face(ex4.Current());
          if ( !Map.Add(fx)) continue;
          ex5.Init(ex4.Current(), TopAbs_VERTEX);
          for(; ex5.More(); ex5.Next()) {
            const TopoDS_Vertex& vv = TopoDS::Vertex(ex5.Current()); 
            if(vv.IsSame(OnFirstVertex)) {
              faces.Append(fx);
              break;
            }
          }
        }
        if(!faces.IsEmpty())  {
          TopoDS_Face FFF = ChoiceOfFaces(faces, cc, FirstPar+bnd/50., bnd/50., Pln);
          if(!FFF.IsNull()) FirstFace = FFF;
        }
      }
      FirstEdge = E;
      BRepLib_MakeVertex v(p1);
      FirstVertex = v;
      OnFirstFace = Standard_True;
    }

    if(PtOnLastEdge) {
      if (!PtOnLastVertex) {
// Find LastFace : face of the base shape containing OnLastEdge
//                 meeting ChoiceOfFaces
        TopExp_Explorer ex4, ex5;
        ex4.Init(mySbase, TopAbs_FACE);
        TopTools_ListOfShape faces;
        faces.Clear();
        Map.Clear();
        for(; ex4.More(); ex4.Next()) {
          const TopoDS_Face& fx = TopoDS::Face(ex4.Current());
          if ( !Map.Add(fx)) continue;
          ex5.Init(ex4.Current(), TopAbs_EDGE);
          for(; ex5.More(); ex5.Next()) {
            const TopoDS_Edge& ee = TopoDS::Edge(ex5.Current()); 
            if(ee.IsSame(OnLastEdge)) {
              faces.Append(fx);
              break;
            }
          }
        }
        if(!faces.IsEmpty())  {
          TopoDS_Face FFF = ChoiceOfFaces(faces, cc, LastPar-bnd/50., bnd/50., Pln);
          if(!FFF.IsNull()) LastFace = FFF;
        }
      }
      else if(PtOnLastEdge && PtOnLastVertex) {
// Find LastFace : face of the base shape containing OnLastVertex
//                 meeting ChoiceOfFaces
        TopExp_Explorer ex4, ex5;
        ex4.Init(mySbase, TopAbs_FACE);
        TopTools_ListOfShape faces;
        faces.Clear();
        Map.Clear();
        for(; ex4.More(); ex4.Next()) {
          const TopoDS_Face& fx = TopoDS::Face(ex4.Current());
          if ( !Map.Add(fx)) continue;
          ex5.Init(ex4.Current(), TopAbs_VERTEX);
          for(; ex5.More(); ex5.Next()) {
            const TopoDS_Vertex& vv = TopoDS::Vertex(ex5.Current()); 
            if(vv.IsSame(OnLastVertex)) {
              faces.Append(fx);
              break;
            }
          }
        }
        if(!faces.IsEmpty())  {
          TopoDS_Face FFF = ChoiceOfFaces(faces, cc, LastPar-bnd/50., bnd/50., Pln);
          if(!FFF.IsNull()) LastFace = FFF;
        }
      }
      LastEdge = E;
      BRepLib_MakeVertex v(p2);
      LastVertex = v;
      OnLastFace = Standard_True;
    }
    
    if(!FirstFace.IsNull() && !LastFace.IsNull())  {
      return Standard_True;
    }

//--- FirstFace or LastFace was not found
#ifdef OCCT_DEBUG
    if (trc) cout << " FirstFace or LastFace null" << endl;
#endif
    LocOpe_CSIntersector ASI(mySbase);
    TColGeom_SequenceOfCurve scur;
    scur.Clear();
    scur.Append(cc);
    ASI.Perform(scur);
    Standard_Real lastpar, firstpar;
    if(ASI.IsDone() && ASI.NbPoints(1) >= 2) {
      lastpar = ASI.Point(1, ASI.NbPoints(1)).Parameter();
      Standard_Integer lastindex = ASI.NbPoints(1);
      if(lastpar > l) {
        for(Standard_Integer jj=ASI.NbPoints(1)-1; jj>=1; jj--) {
          Standard_Real par = ASI.Point(1,jj).Parameter();
          if(par <= l) {
            lastpar = par;
            lastindex = jj;
            break;
          }
        }
      } 
      Standard_Integer firstindex = lastindex -1;      
      firstpar = ASI.Point(1,firstindex).Parameter();

      if(FirstFace.IsNull()) {
        FirstFace = ASI.Point(1, firstindex).Face();
        cc->D0(firstpar, firstpoint);
        BRepLib_MakeVertex v1(firstpoint);
        FirstVertex = TopoDS::Vertex(v1.Shape());
        FirstEdge = E;
      }

      if(LastFace.IsNull()) {      
        LastFace = ASI.Point(1, lastindex).Face();
        cc->D0(lastpar, lastpoint);
        BRepLib_MakeVertex v2(lastpoint);
        LastVertex = TopoDS::Vertex(v2.Shape());
        LastEdge = E;
      }
    }
    else {
#ifdef OCCT_DEBUG
      if (trc) cout << " Less than 2 intersection points" << endl;
#endif
      Data = Standard_False;
      return Data;
    }

    if(!OnFirstFace) {
      if(p1.Distance(firstpoint) <= Precision::Confusion()) 
        OnFirstFace = Standard_True;
      else OnFirstFace = Standard_False;
    }
    
    if(!OnLastFace) {
      if(p2.Distance(lastpoint) <= Precision::Confusion()) 
        OnLastFace = Standard_True;
      else OnLastFace = Standard_False;      
    }

    if(FirstFace.IsNull() || LastFace.IsNull()) {
#ifdef OCCT_DEBUG
      if (trc) cout << " First or Last Faces still null" << endl;
#endif
      Data = Standard_False;
    }
    else {
#ifdef OCCT_DEBUG
      if (trc) cout << " FirstFace and LastFace OK" << endl;
#endif
      Data = Standard_True;
    }
    
    return Data;
  }
// ---The wire consists of several edges
  else {
#ifdef OCCT_DEBUG
    if (trc) cout << " Multiple Edges" << endl;
#endif
    BRepTools_WireExplorer ex(myWire);
    for(; ex.More(); ex.Next()) {
      const TopoDS_Edge& E = TopoDS::Edge(ex.Current());
      Standard_Real f, l;
      Handle(Geom_Curve) Cur = BRep_Tool::Curve(E, f, l);
      f = f - bnd/10000; l = l +bnd/10000;
      Handle(Geom_TrimmedCurve) curve;
      curve = new Geom_TrimmedCurve(Cur, f, l, Standard_True);
#ifdef OCCT_DEBUG
      gp_Pnt P1 = BRep_Tool::Pnt(TopExp::FirstVertex(E,Standard_True)); (void)P1;
#endif
      gp_Pnt P2 = BRep_Tool::Pnt(TopExp::LastVertex(E,Standard_True));
      ex1.Init(mySbase, TopAbs_FACE);
      TopoDS_Vertex theVertex;
      TopoDS_Edge theEdge;
      TopoDS_Face theFace;
      Standard_Boolean PtOnEdge = Standard_False;
      Standard_Boolean PtOnVertex = Standard_False;
      TopoDS_Edge OnEdge; 
      TopoDS_Vertex OnVertex;
      Standard_Real intpar;
      for(; ex1.More(); ex1.Next()) {
        const TopoDS_Face& aCurFace = TopoDS::Face(ex1.Current());
        GeomAdaptor_Curve aGAC (curve);
        inter.Init (aCurFace, aGAC, BRep_Tool::Tolerance(aCurFace));
        if(!inter.More()) continue;
        for(; inter.More(); inter.Next()) {
          gp_Pnt thePoint = inter.Pnt();
          if(!FirstVertex.IsNull()) {
            gp_Pnt point = BRep_Tool::Pnt(FirstVertex);
            if(point.Distance(thePoint) <= BRep_Tool::Tolerance(aCurFace)) {
              continue;
            }
          }
          intpar = IntPar(curve, thePoint);
          theEdge = E;
          theFace = aCurFace;
          B.MakeVertex(theVertex, thePoint, Precision::Confusion());       
          if(!FirstOK) {
            if(thePoint.Distance(P2) <= Precision::Confusion()) {
              continue;
            }
          }

// ---Find thepoint on an edge or a vertex of face f
          PtOnEdgeVertex(RevolRib, aCurFace, thePoint, FirstVertex, LastVertex,
                         PtOnEdge,OnEdge,PtOnVertex,OnVertex);


//          if(!theEdge.IsNull()) break;

          if (FirstEdge.IsNull() && !theEdge.IsNull() &&
              !theFace.IsNull() && !theVertex.IsNull()) {
            FirstEdge = theEdge;
            FirstFace = theFace;
            FirstVertex = theVertex;
            PtOnFirstEdge = PtOnEdge;
            OnFirstEdge = OnEdge;
            theEdge.Nullify(); theFace.Nullify(); theVertex.Nullify();
            if(PtOnEdge && !PtOnVertex) {
              TopTools_ListOfShape faces;
              faces.Clear();
              faces.Append(FirstFace);
              TopExp_Explorer ex2;
              ex2.Init(mySbase, TopAbs_FACE);
              for(; ex2.More(); ex2.Next()) {
                TopoDS_Face fx = TopoDS::Face(ex2.Current());
                TopExp_Explorer ex3;
                ex3.Init(fx, TopAbs_EDGE);
                for(; ex3.More(); ex3.Next()) {
                  const TopoDS_Edge& e = TopoDS::Edge(ex3.Current());
                  if(e.IsSame(OnEdge) && !fx.IsSame(FirstFace)) {
                    faces.Append(fx);
                  }
                }
              }
              TopoDS_Face FFF = ChoiceOfFaces(faces, curve, intpar+bnd/10., bnd/10., Pln);
              if(!FFF.IsNull()) FirstFace = FFF;
            }
            else if(PtOnEdge && PtOnVertex) {
              TopTools_ListOfShape faces;
              faces.Clear();
              faces.Append(FirstFace);
              TopExp_Explorer ex2;
              ex2.Init(mySbase, TopAbs_FACE);
              for(; ex2.More(); ex2.Next()) {
                TopoDS_Face fx = TopoDS::Face(ex2.Current());
                TopExp_Explorer ex3;
                ex3.Init(fx, TopAbs_VERTEX);
                for(; ex3.More(); ex3.Next()) {
                  const TopoDS_Vertex& v = TopoDS::Vertex(ex3.Current());
                  if(v.IsSame(OnVertex) && !fx.IsSame(FirstFace)) {
                    faces.Append(fx);
                  }
                }
              }
              TopoDS_Face FFF = ChoiceOfFaces(faces, curve, intpar+bnd/10., bnd/10.,  Pln);
              if(!FFF.IsNull()) FirstFace = FFF;
            }
            if(!FirstEdge.IsNull() && !FirstFace.IsNull() 
             && !FirstVertex.IsNull()) {
              FirstOK = Standard_True;
            }
          }
          if(LastEdge.IsNull() && !theEdge.IsNull() &&
             !theFace.IsNull() && !theVertex.IsNull() && 
             !FirstEdge.IsNull()) {
            LastEdge = theEdge;
            LastFace = theFace;
            LastVertex = theVertex;
            PtOnLastEdge = PtOnEdge;
            OnLastEdge = OnEdge; 
            if(PtOnEdge && !PtOnVertex) {
              TopTools_ListOfShape faces;
              faces.Clear();
              faces.Append(LastFace);
              TopExp_Explorer ex2;
              ex2.Init(mySbase, TopAbs_FACE);
              for(; ex2.More(); ex2.Next()) {
                TopoDS_Face fx = TopoDS::Face(ex2.Current());
                TopExp_Explorer ex3;
                ex3.Init(fx, TopAbs_EDGE);
                for(; ex3.More(); ex3.Next()) {
                  const TopoDS_Edge& e = TopoDS::Edge(ex3.Current());
                  if(e.IsSame(OnEdge) && !fx.IsSame(LastFace)) {
                    faces.Append(fx);
                  }
                }
              }
              TopoDS_Face FFF = ChoiceOfFaces(faces, curve, intpar-bnd/10.,bnd/10.,  Pln);
              if(!FFF.IsNull()) LastFace = FFF;          
            }
            else if(PtOnEdge && PtOnVertex) {
              TopTools_ListOfShape faces;
              faces.Clear();
              faces.Append(LastFace);
              TopExp_Explorer ex2;
              ex2.Init(mySbase, TopAbs_FACE);
              for(; ex2.More(); ex2.Next()) {
                TopoDS_Face fx = TopoDS::Face(ex2.Current());
                TopExp_Explorer ex3;
                ex3.Init(fx, TopAbs_VERTEX);
                for(; ex3.More(); ex3.Next()) {
                  const TopoDS_Vertex& v = TopoDS::Vertex(ex3.Current());
                  if(v.IsSame(OnVertex) && !fx.IsSame(LastFace)) {
                    faces.Append(fx);
                  }
                }
              }
              TopoDS_Face FFF = ChoiceOfFaces(faces, curve, intpar-bnd/10.,bnd/10.,  Pln);
              if(!FFF.IsNull()) LastFace = FFF;
            }
            if(!LastEdge.IsNull() && !LastFace.IsNull() 
               && !LastVertex.IsNull()) {
              LastOK = Standard_True;
            }
            break;     
          }
        }
      }
    }
    
    if(FirstOK && LastOK)  {
      Data = Standard_True;
      gp_Pnt PP1 = BRep_Tool::Pnt(TopExp::FirstVertex(FirstEdge,Standard_True));
      gp_Pnt PP2 = BRep_Tool::Pnt(TopExp::LastVertex(LastEdge,Standard_True));
      gp_Pnt p1 = BRep_Tool::Pnt(FirstVertex);
      gp_Pnt p2 = BRep_Tool::Pnt(LastVertex);
      if(p1.Distance(PP1) <= BRep_Tool::Tolerance(FirstFace)) {
        OnFirstFace = Standard_True;
      }
      if(p2.Distance(PP2) <= BRep_Tool::Tolerance(LastFace)) {
        OnLastFace = Standard_True;
      }     
      return Standard_True;
    }
    else {
#ifdef OCCT_DEBUG
      if (trc) cout << " First or Last not OK" << endl;
#endif
      return Standard_False;
    }
  }
}


//=======================================================================
//function : PtOnEdgeVertex
//purpose  : Find if 2 limit points of the unique edge of a wire
//           are on an edge or a vertex of the base shape
//=======================================================================

void BRepFeat_RibSlot::PtOnEdgeVertex(const Standard_Boolean RevolRib,
                                      const TopoDS_Shape& shape,
                                      const gp_Pnt& point,
                                      const TopoDS_Vertex& ,//FirstVertex,
                                      const TopoDS_Vertex& ,//LastVertex,
                                      Standard_Boolean& PtOnEdge,
                                      TopoDS_Edge& OnEdge,
                                      Standard_Boolean& PtOnVertex,
                                      TopoDS_Vertex& OnVertex)
     
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEATRIB();
  if (trc) cout << "BRepFeat_RibSlot::PtOnEdgeVertex" << endl;
#endif
  Standard_Boolean TestOK;
//  PtOnEdge = Standard_False;
//  OnEdge.Nullify();
//  PtOnVertex = Standard_False;
//  OnVertex.Nullify();

  TopExp_Explorer EXP;
  EXP.Init(shape, TopAbs_EDGE);
  TopTools_MapOfShape Map;
  for(; EXP.More(); EXP.Next()) {
    const TopoDS_Edge& e = TopoDS::Edge(EXP.Current());
    if ( !Map.Add(e)) continue;
    if (!RevolRib) {
      if (BRep_Tool::Degenerated(e)) continue;
    }
    Standard_Real fff, lll;
    Handle(Geom_Curve) ccc = BRep_Tool::Curve(e, fff, lll);
    if (!RevolRib) {
      ccc = new Geom_TrimmedCurve(ccc, fff, lll);
    }
    GeomAPI_ProjectPointOnCurve proj(point, ccc);
    TestOK = Standard_False;
    if (!RevolRib) {
      if(proj.NbPoints() == 1) TestOK = Standard_True;
    } 
    else {
      if(proj.NbPoints() >= 1) TestOK = Standard_True;
    }
    if(TestOK && proj.Distance(1) <= BRep_Tool::Tolerance(e)) {
      PtOnEdge = Standard_True;
      OnEdge = e;            
      TopoDS_Vertex ev1 = TopExp::FirstVertex(e,Standard_True);
      TopoDS_Vertex ev2 = TopExp::LastVertex(e,Standard_True);
      gp_Pnt ep1 = BRep_Tool::Pnt(ev1);
      gp_Pnt ep2 = BRep_Tool::Pnt(ev2);
      if(point.Distance(ep1) <= BRep_Tool::Tolerance(ev1)) {
        PtOnVertex = Standard_True;
        OnVertex = ev1;
        break;
      }
      else if(point.Distance(ep2) <= BRep_Tool::Tolerance(ev1)) {
        PtOnVertex = Standard_True;
        OnVertex = ev2;
        break;
      }        
      break;
    }
  } 
}


//=======================================================================
//function : SlidingProfile
//purpose  : construction of the profile face in case of sliding
//=======================================================================
 
Standard_Boolean BRepFeat_RibSlot::SlidingProfile(TopoDS_Face& Prof,
                                                  const Standard_Boolean RevolRib,
                                                  const Standard_Real myTol,
                                                  Standard_Integer& Concavite,
                                                  const Handle(Geom_Plane)& myPln,
                                                  const TopoDS_Face& BndFace,
                                                  const gp_Pnt& CheckPnt,
                                                  const TopoDS_Face& FirstFace,
                                                  const TopoDS_Face& LastFace,
                                                  const TopoDS_Vertex& ,//FirstVertex,
                                                  const TopoDS_Vertex& ,//LastVertex,
                                                  const TopoDS_Edge& FirstEdge,
                                                  const TopoDS_Edge& LastEdge)
     
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
  if (trc) cout << "BRepFeat_RibSlot::SlidingProfile" << endl;
#endif
  Standard_Boolean ProfileOK = Standard_True;
// --case of sliding : construction of the wire of the profile
// --> 1 part bounding box + 1 part wire
//   attention to the compatibility of orientations

  gp_Dir FN, LN;
  BRepLib_MakeWire WW;
  
  FN = Normal(FirstFace, myFirstPnt);
  LN = Normal(LastFace, myLastPnt);

// Case of the groove (cut) <> rib (fuse)
// -> we are in the material
// -> make everything in 2d in the working plane : easier  
  if(!myFuse) {
    FN = -FN;
    LN = -LN;
  }
  
  
  Handle(Geom_Line) ln1, ln2;
  gp_Pnt  Pt;//,p1, p2;
  
  ln2 = new Geom_Line(myFirstPnt, FN);
  ln1 = new Geom_Line(myLastPnt, LN);
  
  Handle(Geom2d_Curve) ln2d1 = GeomAPI::To2d(ln1, myPln->Pln());
  Handle(Geom2d_Curve) ln2d2 = GeomAPI::To2d(ln2, myPln->Pln());
  
  Geom2dAPI_InterCurveCurve inter(ln2d1, ln2d2, Precision::Confusion());
    
  Standard_Boolean TestOK = Standard_True;
  if (RevolRib) {
    gp_Dir d1, d2;
    d1 = ln1->Position().Direction();
    d2 = ln2->Position().Direction();
    if(d1.IsOpposite(d2, myTol)) {
      Standard_Real par1 = ElCLib::Parameter(ln1->Lin(), myFirstPnt);
      Standard_Real par2 = ElCLib::Parameter(ln2->Lin(), myLastPnt);
      if(par1 >= myTol  ||  par2 >= myTol)  {
        Concavite = 2;    //paralel and concave
        BRepLib_MakeEdge e1(myLastPnt, myFirstPnt);
        WW.Add(e1);
      } 
    } 
    if(d1.IsEqual(d2, myTol)) {
       if(Concavite == 3) TestOK = Standard_False;
    }
  }
  
  if(TestOK) {
    if(inter.NbPoints() > 0) {
      gp_Pnt2d P = inter.Point(1);
      myPln->D0(P.X(), P.Y(), Pt);
      Standard_Real par = IntPar(ln1, Pt);
      if(par>0) Concavite = 1;    //concave
    }
  }

// ---Construction of the profile face 
  if(Concavite == 1) {
// if concave : it is possible to extend first and last edges of the wire
//              to the bounding box
    BRepLib_MakeEdge e1(myLastPnt, Pt);
    WW.Add(e1);
    BRepLib_MakeEdge e2(Pt, myFirstPnt);
    WW.Add(e2);
  }
  else if(Concavite == 3) {
// BndEdge : edges of intersection with the bounding box
    TopoDS_Edge BndEdge1, BndEdge2;
// Points of intersection with the bounding box / Find Profile
    gp_Pnt BndPnt1, BndPnt2, LastPnt;
    TopExp_Explorer expl;
    expl.Init(BndFace, TopAbs_WIRE);
    BRepTools_WireExplorer explo;
    TopoDS_Wire BndWire = TopoDS::Wire(expl.Current());
    explo.Init(BndWire);
    for(; explo.More(); explo.Next()) {
      const TopoDS_Edge& e = TopoDS::Edge(explo.Current());
      Standard_Real first, last;
      Handle(Geom_Curve) c = BRep_Tool::Curve(e, first, last);
      Handle(Geom2d_Curve) c2d = GeomAPI::To2d(c, myPln->Pln());
      Geom2dAPI_InterCurveCurve intcln1(ln2d1, c2d, 
                                        Precision::Confusion());
      if(intcln1.NbPoints() > 0) {
        gp_Pnt2d p2d = intcln1.Point(1);
        gp_Pnt p;
        myPln->D0(p2d.X(), p2d.Y(), p);
        Standard_Real parl = IntPar(ln1, p);
        Standard_Real parc = IntPar(c, p);
        if(parc >= first && parc <= last && parl >= 0) {
          BndEdge1 = e;
          BndPnt1 = p;
        }
      }
      
      Geom2dAPI_InterCurveCurve intcln2(ln2d2, c2d, 
                                        Precision::Confusion());
      if(intcln2.NbPoints() > 0) {
        gp_Pnt2d p2d = intcln2.Point(1);
        gp_Pnt p;
        myPln->D0(p2d.X(), p2d.Y(), p);
        Standard_Real parl = IntPar(ln2, p);
        Standard_Real parc = IntPar(c, p);
        if(parc >= first && parc <= last && parl >= 0) {
          BndEdge2 = e;
          BndPnt2 = p;
        }
      }
      if(!BndEdge1.IsNull() && !BndEdge2.IsNull()) break;
    }
    
    if(BndEdge1.IsNull() || BndEdge2.IsNull())  {
#ifdef OCCT_DEBUG
      if (trc) cout << " Null bounding edge" << endl;
#endif
      ProfileOK = Standard_False;
      return ProfileOK;
    }
    
    
    BRepLib_MakeEdge e1(myLastPnt, BndPnt1);
    WW.Add(e1);
    
    if(BndEdge1.IsSame(BndEdge2)) {
// Particular case : same edge -> simply determined path
      BRepLib_MakeEdge e2(BndPnt1, BndPnt2);
      WW.Add(e2);
      BRepLib_MakeEdge e3(BndPnt2, myFirstPnt);
      WW.Add(e3);        
    }
    else {
      explo.Init(BndWire);
      for(; explo.More(); explo.Next()) {
        const TopoDS_Edge& e = TopoDS::Edge(explo.Current());
        if(e.IsSame(BndEdge1)) {
          gp_Pnt pp;
          pp = BRep_Tool::Pnt(TopExp::LastVertex(e,Standard_True));
          if(pp.Distance(BndPnt1) >= BRep_Tool::Tolerance(e)) {
            LastPnt = pp;
          }
//            else {         //LinearForm
//              gp_Pnt ppp = BRep_Tool::Pnt(TopExp::FirstVertex(e,Standard_True));
//              LastPnt = ppp;
//            }
          BRepLib_MakeEdge e2(BndPnt1, LastPnt);
          WW.Add(e2);
          break;        
        }
      }
      
      if(explo.More()) {
        explo.Next();
        if(explo.Current().IsNull()) explo.Init(BndWire);
      }
      else explo.Init(BndWire);

// Check if this is BndEdge2
// -> if yes : it is required to turn to join FirstPnt
// -> if no : add edges
      Standard_Boolean Fin = Standard_False;
      while(!Fin) {
        const TopoDS_Edge& e = TopoDS::Edge(explo.Current());
        if(!e.IsSame(BndEdge2)) {
          gp_Pnt pp;
          pp = BRep_Tool::Pnt(TopExp::LastVertex(e,Standard_True));  
          BRepLib_MakeEdge ee(LastPnt, pp);
          WW.Add(ee);
          LastPnt = pp;
        }
        else {
// the path is closed
// -> since met BndEdge2, end of borders on BndFace
          Fin = Standard_True;
          BRepLib_MakeEdge ee(LastPnt, BndPnt2);
          WW.Add(ee);
          LastPnt = BndPnt2;
        }
        if(explo.More()) {
          explo.Next();
          if(explo.Current().IsNull()) {
            explo.Init(BndWire);
          }
        }
        else explo.Init(BndWire);
      }
      
      BRepLib_MakeEdge e3(BndPnt2, myFirstPnt);
      WW.Add(e3);
    }   
  }

// ---Construction of the profile

// Explore the wire provided by the user
// BRepTools_WireExplorer : correct order - without repetition <> TopExp : non ordered
  BRepTools_WireExplorer EX(myWire);
  
  Standard_Real ff, ll;
  Handle(Geom_Curve) FirstCurve = BRep_Tool::Curve(FirstEdge, ff, ll);
  
  if(!FirstEdge.IsSame(LastEdge)) {
    TopoDS_Vertex FLVert = TopExp::LastVertex(FirstEdge,Standard_True);
    gp_Pnt FLPnt = BRep_Tool::Pnt(FLVert);
    BRepLib_MakeEdge ef(FirstCurve, myFirstPnt, FLPnt);
    WW.Add(ef);   
    for(; EX.More(); EX.Next()) {
      const TopoDS_Edge& E = EX.Current();
      if(E.IsSame(FirstEdge))        break;
    }      
    EX.Next();
    for(; EX.More(); EX.Next()) {
      const TopoDS_Edge& E = EX.Current();
      if(!E.IsSame(LastEdge)) {
        WW.Add(E);
      }        
      else break;
    }
    Handle(Geom_Curve) LastCurve = BRep_Tool::Curve(LastEdge, ff, ll);
    TopoDS_Vertex LFVert = TopExp::FirstVertex(LastEdge,Standard_True);
    gp_Pnt LFPnt = BRep_Tool::Pnt(LFVert);
    BRepLib_MakeEdge el(LastCurve, LFPnt, myLastPnt);
    WW.Add(el);
  }
  else {
// only one edge : particular processing
    Standard_Real fpar = IntPar(FirstCurve, myFirstPnt);
    Standard_Real lpar = IntPar(FirstCurve, myLastPnt);
    Handle(Geom_Curve) c;
    if(fpar > lpar) 
      c = FirstCurve->Reversed();
    else 
      c = FirstCurve;
    
    BRepLib_MakeEdge ef(c, myFirstPnt, myLastPnt);
    WW.Add(ef);
  }
  
  BRepLib_MakeFace f(myPln->Pln(), WW, Standard_True);
  TopoDS_Face fac = TopoDS::Face(f.Shape());
    
  if (!BRepAlgo::IsValid(fac)) {
#ifdef OCCT_DEBUG
    if (trc) cout << " Invalid Face" << endl;
#endif
    ProfileOK = Standard_False;
    return ProfileOK;
  }
  
  if(Concavite != 3) {
// if concave : face is OK
    Prof = fac;
  }
  else {
// if not concave
// CheckPnt : point slightly inside the material side
// Bndface  : face/cut of the bounding box in the plane of the profile
    BRepTopAdaptor_FClass2d Cl(fac, BRep_Tool::Tolerance(fac));
    Standard_Real u, v;
    ElSLib::Parameters(myPln->Pln(), CheckPnt, u, v);
    gp_Pnt2d checkpnt2d(u, v);
    if(Cl.Perform(checkpnt2d, Standard_True) == TopAbs_OUT) {
// If face is not the correct part of BndFace take the complementary
      BRepAlgoAPI_Cut c(BndFace, fac);     
      TopExp_Explorer exp(c.Shape(), TopAbs_WIRE);
      const TopoDS_Wire& w = TopoDS::Wire(exp.Current());
      BRepLib_MakeFace ffx(w);
      Prof = TopoDS::Face(ffx.Shape());
    }
    else {
// If face is the correct part of BndFace  : face is OK
      Prof = fac;
    }
  }
  
  if (!BRepAlgo::IsValid(Prof)) {
#ifdef OCCT_DEBUG
    if (trc) cout << " Invalid Face Profile" << endl;
#endif
    ProfileOK = Standard_False;
    return ProfileOK;    
  }
  return ProfileOK;
}
//=======================================================================
//function : NoSlidingProfile
//purpose  : construction of the face profile in case of sliding
//=======================================================================
 
Standard_Boolean BRepFeat_RibSlot::NoSlidingProfile(TopoDS_Face& Prof,
                                                    const Standard_Boolean RevolRib,
                                                    const Standard_Real myTol,
                                                    Standard_Integer& Concavite,
                                                    const Handle(Geom_Plane)& myPln,
                                                    const Standard_Real bnd,
                                                    const TopoDS_Face& BndFace,
                                                    const gp_Pnt& CheckPnt,
                                                    const TopoDS_Face& ,//FirstFace,
                                                    const TopoDS_Face& ,//LastFace,
                                                    const TopoDS_Vertex& ,//FirstVertex,
                                                    const TopoDS_Vertex& ,//LastVertex,
                                                    const TopoDS_Edge& FirstEdge,
                                                    const TopoDS_Edge& LastEdge,
                                                    const Standard_Boolean OnFirstFace,
                                                    const Standard_Boolean OnLastFace)
     
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
  if (trc) cout << "BRepFeat_RibSlot::NoSlidingProfile" << endl;
#endif
  Standard_Boolean ProfileOK = Standard_True;

  Standard_Real l1, f1, f2, l2;//, p;        
  TopoDS_Vertex theFV; theFV.Nullify();
  gp_Pnt theFirstpoint;
  TopoDS_Edge theLastEdge; theLastEdge.Nullify();
  gp_Pnt firstpoint, lastpoint;//, pp1, pp2;
  gp_Vec firstvect, lastvect; 
  TopoDS_Wire w;
  BRep_Builder BB;
  BB.MakeWire(w);
  //gp_Pnt p1, p3;
  TopoDS_Edge FalseFirstEdge, FalseLastEdge, FalseOnlyOne;
  
  Handle(Geom_Curve) FirstCurve = BRep_Tool::Curve(FirstEdge, f1, l1);
  Handle(Geom_Curve) LastCurve = BRep_Tool::Curve(LastEdge, f2, l2);

  Handle(Geom_Line) firstln, lastln;  
  FirstCurve->D1(f1, firstpoint, firstvect);
  lastln = new Geom_Line(firstpoint, -firstvect);
  LastCurve->D1(l2, lastpoint, lastvect);
  firstln = new Geom_Line(lastpoint, lastvect);
  
  gp_Pnt Pt;
  
  Handle(Geom2d_Curve) ln2d1 = GeomAPI::To2d(firstln, myPln->Pln());
  Handle(Geom2d_Curve) ln2d2 = GeomAPI::To2d(lastln, myPln->Pln());
  
  Geom2dAPI_InterCurveCurve inter(ln2d1, ln2d2, Precision::Confusion());

  Standard_Boolean TestOK = Standard_True;
  if (RevolRib) {
    gp_Dir d1, d2;
    d1 = firstln->Position().Direction();
    d2 = lastln->Position().Direction();
    if(d1.IsOpposite(d2, myTol)) {
      Standard_Real par1 = ElCLib::Parameter(firstln->Lin(), myFirstPnt);
      Standard_Real par2 = ElCLib::Parameter(lastln->Lin(), myLastPnt);
      if(par1 >= myTol  ||  par2 >= myTol)  
        Concavite = 2;    //parallel and concave
    }      
    if(d1.IsEqual(d2, myTol)) {
       if(Concavite == 3) TestOK = Standard_False;
    }
  }
  
  if(TestOK) {
    if(inter.NbPoints() > 0) {
      gp_Pnt2d P = inter.Point(1);
      myPln->D0(P.X(), P.Y(), Pt);
      Standard_Real par = IntPar(firstln, Pt);
      if(par>0) Concavite = 1;    //concave
    }
  }

// ---Construction of the face profile  
  if(Concavite == 3) {
    if(OnFirstFace) {
      Standard_Real f, l;
      FalseFirstEdge = FirstEdge;
      EdgeExtention(FalseFirstEdge, bnd, Standard_True);
      const TopoDS_Vertex& vv1 = TopExp::FirstVertex(FalseFirstEdge,Standard_True);
      firstpoint = BRep_Tool::Pnt(vv1);
      Handle(Geom_Curve) cc = BRep_Tool::Curve(FalseFirstEdge, f, l);
      cc->D1(f, firstpoint, firstvect);
      lastln = new Geom_Line(firstpoint, -firstvect);
      if(FirstEdge.IsSame(LastEdge)) FalseOnlyOne = FalseFirstEdge;         
      ln2d2 = GeomAPI::To2d(lastln, myPln->Pln());
    }
    if(OnLastFace) {
      Standard_Real f, l;
      if(!FirstEdge.IsSame(LastEdge)) {
        FalseLastEdge = LastEdge;
      }
      else {
        if(FalseOnlyOne.IsNull()) FalseOnlyOne = LastEdge;
        FalseLastEdge = FalseOnlyOne;
      }
      EdgeExtention(FalseLastEdge, bnd, Standard_False);
      if(FirstEdge.IsSame(LastEdge)) {
        FalseOnlyOne = FalseLastEdge;
      }
      const TopoDS_Vertex& vv2 = TopExp::LastVertex(FalseLastEdge,Standard_True);
      lastpoint = BRep_Tool::Pnt(vv2);
      Handle(Geom_Curve) cc = BRep_Tool::Curve(FalseLastEdge, f, l);
      cc->D1(l, lastpoint, lastvect);
      lastpoint = BRep_Tool::Pnt(vv2);
      firstln = new Geom_Line(lastpoint, lastvect);
      ln2d1 = GeomAPI::To2d(firstln, myPln->Pln());
    }
    
    TopoDS_Edge BndEdge1, BndEdge2;
    gp_Pnt BndPnt1, BndPnt2, LastPnt;
    TopExp_Explorer expl;
    expl.Init(BndFace, TopAbs_WIRE);
    BRepTools_WireExplorer explo;
    TopoDS_Wire BndWire = TopoDS::Wire(expl.Current());
    explo.Init(BndWire);
    for(; explo.More(); explo.Next()) {
      const TopoDS_Edge& e = TopoDS::Edge(explo.Current());
      Standard_Real first, last;
      Handle(Geom_Curve) c = BRep_Tool::Curve(e, first, last);
      Handle(Geom2d_Curve) c2d = GeomAPI::To2d(c, myPln->Pln());
      Geom2dAPI_InterCurveCurve intcln1(ln2d1, c2d, 
                                        Precision::Confusion());
      if(intcln1.NbPoints() > 0) {
        gp_Pnt2d p2d = intcln1.Point(1);
        gp_Pnt p;
        myPln->D0(p2d.X(), p2d.Y(), p);
        Standard_Real parl = IntPar(firstln, p);
        Standard_Real parc = IntPar(c, p);
        if(parc >= first && parc <= last && parl >= 0) {
          BndEdge1 = e;
          BndPnt1 = p;
        }
      }
      
      Geom2dAPI_InterCurveCurve intcln2(ln2d2, c2d, 
                                        Precision::Confusion());
      if(intcln2.NbPoints() > 0) {
        gp_Pnt2d p2d = intcln2.Point(1);
        gp_Pnt p;
        myPln->D0(p2d.X(), p2d.Y(), p);
        Standard_Real parl = IntPar(lastln, p);
        Standard_Real parc = IntPar(c, p);
        if(parc >= first && parc <= last && parl >= 0) {
          BndEdge2 = e;
          BndPnt2 = p;
        }
      }
      if(!BndEdge1.IsNull() && !BndEdge2.IsNull()) break;
    }
    
    if(BndEdge1.IsNull() || BndEdge2.IsNull())  {
#ifdef OCCT_DEBUG
      if (trc) cout << " Null bounding edge" << endl;
#endif
      ProfileOK = Standard_False;
      return ProfileOK;
    }
    
    TopoDS_Edge ee1;
    if(theLastEdge.IsNull()) {
      BRepLib_MakeEdge e1(lastpoint, BndPnt1);
      ee1 = TopoDS::Edge(e1.Shape());
    }
    else {
      const TopoDS_Vertex& v1 = TopExp::LastVertex(theLastEdge,Standard_True);
      BRepLib_MakeVertex v2(BndPnt1);
      BRepLib_MakeEdge e1(v1, v2);
      ee1 = TopoDS::Edge(e1.Shape());
    }
    BB.Add(w, ee1);
    theLastEdge = ee1;
    if(theFV.IsNull()) {
      theFV = TopExp::FirstVertex(ee1,Standard_True);
      theFirstpoint = BRep_Tool::Pnt(theFV);
    }
    
    if(BndEdge1.IsSame(BndEdge2)) {
      TopoDS_Edge ee2, ee3;
      if(theLastEdge.IsNull()) {
        BRepLib_MakeEdge e2(BndPnt1, BndPnt2);
        ee2 = TopoDS::Edge(e2.Shape());
      }
      else {
        const TopoDS_Vertex& v1 = TopExp::LastVertex(theLastEdge,Standard_True);
        BRepLib_MakeVertex v2(BndPnt2);
        BRepLib_MakeEdge e2(v1, v2);
        ee2 = TopoDS::Edge(e2.Shape());
      }
      BB.Add(w, ee2);
      theLastEdge = ee2;
      if(theFV.IsNull()) {
        theFV = TopExp::FirstVertex(ee2,Standard_True);
        theFirstpoint = BRep_Tool::Pnt(theFV);
      }
      if(theLastEdge.IsNull()) {
        BRepLib_MakeEdge e3(BndPnt2, firstpoint);
        ee3 = TopoDS::Edge(e3.Shape());        
      }
      else {
        const TopoDS_Vertex& v1 = TopExp::LastVertex(theLastEdge,Standard_True);
        BRepLib_MakeVertex v2(firstpoint);
        BRepLib_MakeEdge e3(v1, v2);
        ee3 = TopoDS::Edge(e3.Shape());
      }
      BB.Add(w, ee3);        
      theLastEdge = ee3;
      if(theFV.IsNull()) {
        theFV = TopExp::FirstVertex(ee3,Standard_True);
        theFirstpoint = BRep_Tool::Pnt(theFV);
      }
    }
    else {
      explo.Init(BndWire);
      for(; explo.More(); explo.Next()) {
        const TopoDS_Edge& e = TopoDS::Edge(explo.Current());
        if(e.IsSame(BndEdge1)) {
          gp_Pnt pp;
          pp = BRep_Tool::Pnt(TopExp::LastVertex(e,Standard_True));
          if(pp.Distance(BndPnt1) > BRep_Tool::Tolerance(e)) {
            LastPnt = pp;
          }
          TopoDS_Edge eee;
          if(theLastEdge.IsNull()) {
            BRepLib_MakeEdge e2(BndPnt1, LastPnt);
            eee = TopoDS::Edge(e2.Shape());
          }
          else {
            const TopoDS_Vertex& v1 = TopExp::LastVertex(theLastEdge,Standard_True);
            BRepLib_MakeVertex v2(LastPnt);
            BRepLib_MakeEdge e2(v1, v2);
            eee = TopoDS::Edge(e2.Shape());
          }
          BB.Add(w, eee);
          theLastEdge = eee;
          if(theFV.IsNull()) {
            theFV = TopExp::FirstVertex(eee,Standard_True);
            theFirstpoint = BRep_Tool::Pnt(theFV);
          }
          break;        
        }
      }
      
      if(explo.More()) {
        explo.Next();
        if(explo.Current().IsNull()) explo.Init(BndWire);
      }
      else explo.Init(BndWire);
      Standard_Boolean Fin = Standard_False;
      while(!Fin) {
        const TopoDS_Edge& e = TopoDS::Edge(explo.Current());
        if(!e.IsSame(BndEdge2)) {
          gp_Pnt pp;
          pp = BRep_Tool::Pnt(TopExp::LastVertex(e,Standard_True));
          TopoDS_Edge eee1;
          if(theLastEdge.IsNull()) {
            BRepLib_MakeEdge ee(LastPnt, pp);
            eee1 = TopoDS::Edge(ee.Shape());
          }
          else {
            const TopoDS_Vertex& v1 = TopExp::LastVertex(theLastEdge,Standard_True);
            BRepLib_MakeVertex v2(pp);
            BRepLib_MakeEdge ee(v1, v2);
            eee1 = TopoDS::Edge(ee.Shape());
          }
          BB.Add(w, eee1);
          theLastEdge = eee1;
          if(theFV.IsNull()) {
            theFV = TopExp::FirstVertex(eee1,Standard_True);
            theFirstpoint = BRep_Tool::Pnt(theFV);
          }
          LastPnt = pp;
        }
        else {
          Fin = Standard_True;
          TopoDS_Edge eee2;
          if(theLastEdge.IsNull()) {
            BRepLib_MakeEdge ee(LastPnt, BndPnt2);
            eee2 = TopoDS::Edge(ee.Shape());
          }
          else {
            const TopoDS_Vertex& v1 = TopExp::LastVertex(theLastEdge,Standard_True);
            BRepLib_MakeVertex v2(BndPnt2);
            BRepLib_MakeEdge ee(v1, v2);
            eee2 = TopoDS::Edge(ee.Shape());
          }
          BB.Add(w, eee2);
          theLastEdge = eee2;
          if(theFV.IsNull()) {
            theFV = TopExp::FirstVertex(eee2,Standard_True);
            theFirstpoint = BRep_Tool::Pnt(theFV);
          }
          LastPnt = BndPnt2;
        }
        if(explo.More()) {
          explo.Next();
          if(explo.Current().IsNull()) {
            explo.Init(BndWire);
          }
        }
        else explo.Init(BndWire);
      }
      
      TopoDS_Edge eee3;
      if(theLastEdge.IsNull()) {
        BRepLib_MakeEdge e3(BndPnt2, firstpoint);
        eee3 = TopoDS::Edge(e3.Shape());
      }
      else {
        const TopoDS_Vertex& v1 = TopExp::LastVertex(theLastEdge,Standard_True);
        BRepLib_MakeVertex v2(firstpoint);
        BRepLib_MakeEdge e3(v1, v2);
        eee3 = TopoDS::Edge(e3.Shape());
      }
      BB.Add(w, eee3);
      theLastEdge = eee3;
      if(theFV.IsNull()) {
        theFV = TopExp::FirstVertex(eee3,Standard_True);
        theFirstpoint = BRep_Tool::Pnt(theFV);
      }
    }   
  }
 
  if(Concavite == 1) {
    TopoDS_Edge eee4;
    if(theLastEdge.IsNull()) {
      BRepLib_MakeEdge  e(Pt, firstpoint); 
      eee4 = TopoDS::Edge(e.Shape());      
    }
    else {
      const TopoDS_Vertex& v1 = TopExp::LastVertex(theLastEdge,Standard_True);
      BRepLib_MakeVertex v2(firstpoint);
      BRepLib_MakeEdge e(v1, v2);
      eee4 = TopoDS::Edge(e.Shape());
    }
    BB.Add(w, eee4);
    if(theFV.IsNull()) {
      theFV = TopExp::FirstVertex(eee4,Standard_True);
      theFirstpoint = BRep_Tool::Pnt(theFV);
    }
    theLastEdge = eee4;
  }
  
  
  if(FirstEdge.IsSame(LastEdge)) {
    if(!myLFMap.IsBound(FirstEdge)) {
      TopTools_ListOfShape thelist;
      myLFMap.Bind(FirstEdge, thelist);
    }
    if(OnFirstFace || OnLastFace) {
      TopoDS_Edge theEdge;
      Standard_Real f, l;
      Handle(Geom_Curve) cc = BRep_Tool::Curve(FalseOnlyOne, f, l);
      if(!theLastEdge.IsNull()) {
        const TopoDS_Vertex& v1 = TopExp::LastVertex(theLastEdge,Standard_True);
        TopoDS_Vertex v2;
        const gp_Pnt& pp = BRep_Tool::
          Pnt(TopExp::LastVertex(FalseOnlyOne,Standard_True));
        if(!theFV.IsNull() && theFirstpoint.Distance(pp) <= myTol) {
          v2 = theFV;
        }
        else v2 = TopExp::LastVertex(FalseOnlyOne,Standard_True);
        BRepLib_MakeEdge e(cc, v1, v2);
        theEdge = TopoDS::Edge(e.Shape());
      }
      else {
        const TopoDS_Vertex& v1 = TopExp::FirstVertex(FalseOnlyOne,Standard_True);
        TopoDS_Vertex v2;
        const gp_Pnt& pp = BRep_Tool::
          Pnt(TopExp::LastVertex(FalseOnlyOne,Standard_True));
        if(!theFV.IsNull() && theFirstpoint.Distance(pp) <= myTol) {
          v2 = theFV;
        }
        else v2 = TopExp::LastVertex(FalseOnlyOne,Standard_True);                  
        BRepLib_MakeEdge e(cc, v1, v2);
        theEdge = TopoDS::Edge(e.Shape());
      }
      myLFMap(FirstEdge).Append(theEdge);
      BB.Add(w, theEdge);
      if(theFV.IsNull()) theFV = TopExp::FirstVertex(theEdge,Standard_True);
      theLastEdge = theEdge;
    }
    else {
      Standard_Real f, l;
      Handle(Geom_Curve) cc = BRep_Tool::Curve(FirstEdge, f, l);
      TopoDS_Edge theEdge;
      if(!theLastEdge.IsNull()) {
        const TopoDS_Vertex& v1 = TopExp::LastVertex(theLastEdge,Standard_True);
        TopoDS_Vertex v2;
// Attention case Wire Reversed -> LastVertex without Standard_True
        const gp_Pnt& pp = BRep_Tool::Pnt(TopExp::LastVertex(FirstEdge));
        if(!theFV.IsNull() && theFirstpoint.Distance(pp) <= myTol) {
          v2 = theFV;
        }
        else v2 = TopExp::LastVertex(FirstEdge);
        BRepLib_MakeEdge e(cc, v1, v2);
        theEdge = TopoDS::Edge(e.Shape());
      }
      else {
        const TopoDS_Vertex& v1 = TopExp::FirstVertex(FirstEdge,Standard_True);
        TopoDS_Vertex v2;
        const gp_Pnt& pp = BRep_Tool::
          Pnt(TopExp::LastVertex(FirstEdge,Standard_True));
        if(!theFV.IsNull() && theFirstpoint.Distance(pp) <= myTol) {
          v2 = theFV;
        }
        else v2 = TopExp::LastVertex(FirstEdge,Standard_True);
        BRepLib_MakeEdge e(cc, v1, v2);
        theEdge = TopoDS::Edge(e.Shape());
      }
      myLFMap(FirstEdge).Append(theEdge);
      BB.Add(w, theEdge); 
      if(theFV.IsNull()) theFV = TopExp::FirstVertex(theEdge,Standard_True);
      theLastEdge = theEdge;
    }
  }
  else {
    if(!myLFMap.IsBound(FirstEdge)) {
      TopTools_ListOfShape thelist1;
      myLFMap.Bind(FirstEdge, thelist1);
    }
    if(!OnFirstFace) {
      TopoDS_Edge theEdge;
      Standard_Real f, l;
      Handle(Geom_Curve) cc = BRep_Tool::Curve(FirstEdge, f, l);
      if(!theLastEdge.IsNull()) {
        const TopoDS_Vertex& v1 = TopExp::LastVertex(theLastEdge,Standard_True);
        const TopoDS_Vertex& v2 = TopExp::LastVertex(FirstEdge,Standard_True);
        BRepLib_MakeEdge e(cc, v1, v2);
        theEdge = TopoDS::Edge(e.Shape());          
      }
      else {
        theEdge = FirstEdge;
      }
      myLFMap(FirstEdge).Append(theEdge);
      BB.Add(w, theEdge); 
      if(theFV.IsNull()) theFV = TopExp::FirstVertex(theEdge,Standard_True);
      theLastEdge = theEdge;
    }
    else {
      TopoDS_Edge theEdge;
      Standard_Real f, l;
      Handle(Geom_Curve) cc = BRep_Tool::Curve(FalseFirstEdge, f, l);
      if(!theLastEdge.IsNull()) {
        const TopoDS_Vertex& v1 = TopExp::LastVertex(theLastEdge,Standard_True);
        const TopoDS_Vertex& v2 = TopExp::LastVertex(FalseFirstEdge,Standard_True);
        BRepLib_MakeEdge e(cc, v1, v2);
        theEdge = TopoDS::Edge(e.Shape());          
      }
      else {
        theEdge = FalseFirstEdge;
      }
      myLFMap(FirstEdge).Append(theEdge);
      BB.Add(w, theEdge); 
      if(theFV.IsNull()) theFV = TopExp::FirstVertex(theEdge,Standard_True);
      theLastEdge = theEdge;
    }

    BRepTools_WireExplorer ex(myWire);
    for(; ex.More(); ex.Next()) {
      const TopoDS_Edge& E = ex.Current();
      if(E.IsSame(FirstEdge)) break;
    }
    
    ex.Next();
    
    for(; ex.More(); ex.Next()) {
      const TopoDS_Edge& E = ex.Current();
      if(!E.IsSame(LastEdge)) {
        if(!myLFMap.IsBound(E)) {
          TopTools_ListOfShape thelist2;
          myLFMap.Bind(E, thelist2);
        }
        TopoDS_Edge eee;
        Standard_Real f, l;
        Handle(Geom_Curve) cc = BRep_Tool::Curve(E, f, l);
        if(!theLastEdge.IsNull()) {
          const TopoDS_Vertex& v1 = TopExp::LastVertex(theLastEdge,Standard_True);
          const TopoDS_Vertex& v2 = TopExp::LastVertex(E,Standard_True);
          BRepLib_MakeEdge e(cc, v1, v2);
          eee = TopoDS::Edge(e.Shape());          
        }
        else {
          eee = E;
        }
        myLFMap(E).Append(eee);
        BB.Add(w, eee);
        if(theFV.IsNull()) theFV = TopExp::FirstVertex(eee,Standard_True);
        theLastEdge = eee;
      }
      else break;
    }
    
    
    if(!OnLastFace) {
      if(!FirstEdge.IsSame(LastEdge)) {
        const TopoDS_Edge& edg = TopoDS::Edge(ex.Current()); 
        if(!myLFMap.IsBound(edg)) {
          TopTools_ListOfShape thelist3;
          myLFMap.Bind(edg, thelist3);
        }
        TopoDS_Edge eee;
        Standard_Real f, l;
        Handle(Geom_Curve) cc = BRep_Tool::Curve(edg, f, l);
        if(!theLastEdge.IsNull()) {
          const TopoDS_Vertex& v1 = TopExp::LastVertex(theLastEdge,Standard_True);
          TopoDS_Vertex v2;
          const gp_Pnt& pp = BRep_Tool::
            Pnt(TopExp::LastVertex(edg,Standard_True));
          if(!theFV.IsNull() && theFirstpoint.Distance(pp) <= myTol) {
            v2 = theFV;
          }
          else v2 = TopExp::LastVertex(edg,Standard_True);
          BRepLib_MakeEdge e(cc, v1, v2);
          eee = TopoDS::Edge(e.Shape());          
        }
        else {
          const TopoDS_Vertex& v1 = TopExp::FirstVertex(edg,Standard_True);
          TopoDS_Vertex v2;
          const gp_Pnt& pp = BRep_Tool::
            Pnt(TopExp::LastVertex(edg,Standard_True));
          if(!theFV.IsNull() && theFirstpoint.Distance(pp) <= myTol) {
            v2 = theFV;
          }
          else v2 = TopExp::LastVertex(edg,Standard_True);
          BRepLib_MakeEdge e(cc, v1, v2);
          eee = TopoDS::Edge(e.Shape());          
        }
        myLFMap(edg).Append(eee);
        BB.Add(w, eee);
        if(theFV.IsNull()) theFV = TopExp::FirstVertex(eee,Standard_True);
        theLastEdge = eee;
      }
      else {
        TopoDS_Edge eee;
        Standard_Real f, l;
        if(!myLFMap.IsBound(LastEdge)) {
          TopTools_ListOfShape thelist4;
          myLFMap.Bind(LastEdge, thelist4);
        }
        Handle(Geom_Curve) cc = BRep_Tool::Curve(FalseOnlyOne, f, l);
        if(!theLastEdge.IsNull()) {
          const TopoDS_Vertex& v1 = TopExp::LastVertex(theLastEdge,Standard_True);
          TopoDS_Vertex v2;
          const gp_Pnt& pp = BRep_Tool::
            Pnt(TopExp::LastVertex(FalseOnlyOne,Standard_True));
          if(!theFV.IsNull() && theFirstpoint.Distance(pp) <= myTol) {
            v2 = theFV;
          }
          else v2 = TopExp::LastVertex(FalseOnlyOne,Standard_True);
          BRepLib_MakeEdge e(cc, v1, v2);
          eee = TopoDS::Edge(e.Shape());          
        }          
        else {
          const TopoDS_Vertex& v1 = TopExp::FirstVertex(FalseOnlyOne,Standard_True);
          TopoDS_Vertex v2;
          const gp_Pnt& pp = BRep_Tool::
            Pnt(TopExp::LastVertex(FalseOnlyOne,Standard_True));
          if(!theFV.IsNull() && theFirstpoint.Distance(pp) <= myTol) {
            v2 = theFV;
          }
          else v2 = TopExp::LastVertex(FalseOnlyOne,Standard_True);
          BRepLib_MakeEdge e(cc, v1, v2);
          eee = TopoDS::Edge(e.Shape());          
        }
        myLFMap(LastEdge).Append(eee);
        BB.Add(w, eee);
        if(theFV.IsNull()) theFV = TopExp::FirstVertex(eee,Standard_True);
        theLastEdge = eee;
      }
    }
    else {
      TopoDS_Edge eee;
      Standard_Real f, l;
      if(!myLFMap.IsBound(LastEdge)) {
        TopTools_ListOfShape thelist5;
        myLFMap.Bind(LastEdge, thelist5);
      }
      Handle(Geom_Curve) cc = BRep_Tool::Curve(FalseLastEdge, f, l);
      if(!theLastEdge.IsNull()) {
        const TopoDS_Vertex& v1 = TopExp::LastVertex(theLastEdge,Standard_True);
        TopoDS_Vertex v2;
        const gp_Pnt& pp = BRep_Tool::
          Pnt(TopExp::LastVertex(FalseLastEdge,Standard_True));
        if(!theFV.IsNull() && theFirstpoint.Distance(pp) <= myTol) {
          v2 = theFV;
        }
        else v2 = TopExp::LastVertex(FalseLastEdge,Standard_True);
        BRepLib_MakeEdge e(cc, v1, v2);
        eee = TopoDS::Edge(e.Shape());          
      }          
      else {
        const TopoDS_Vertex& v1 = TopExp::FirstVertex(FalseLastEdge,Standard_True);
        TopoDS_Vertex v2;
        const gp_Pnt& pp = BRep_Tool::
          Pnt(TopExp::LastVertex(FalseLastEdge,Standard_True));
        if(!theFV.IsNull() && theFirstpoint.Distance(pp) <= myTol) {
          v2 = theFV;
        }
        else v2 = TopExp::LastVertex(FalseLastEdge,Standard_True);
        BRepLib_MakeEdge e(cc, v1, v2);
        eee = TopoDS::Edge(e.Shape());          
      }
      myLFMap(LastEdge).Append(eee);
      BB.Add(w, eee);
      if(theFV.IsNull()) theFV = TopExp::FirstVertex(eee,Standard_True);
      theLastEdge = eee;
    }
  }
  
  if(Concavite == 1)  {
    TopoDS_Edge eef;
    if(theLastEdge.IsNull()) {        
      BRepLib_MakeEdge ef(lastpoint, Pt);
      eef = TopoDS::Edge(ef.Shape());
    }
    else {
      const TopoDS_Vertex& v1 = TopExp::LastVertex(theLastEdge,Standard_True);
      BRepLib_MakeVertex vv(Pt);
      TopoDS_Vertex v2 = TopoDS::Vertex(vv.Shape());
      if(!theFV.IsNull() && 
         Pt.Distance(theFirstpoint) <= myTol) v2 = theFV;
      
      BRepLib_MakeEdge ef(v1, v2);
      eef = TopoDS::Edge(ef.Shape());        
    }
    BB.Add(w, eef);
    if(theFV.IsNull()) theFV = TopExp::FirstVertex(eef,Standard_True);
    theLastEdge = eef;
  }
  
  if(Concavite == 2) {
    BRepLib_MakeEdge ee(lastpoint, firstpoint);
    const TopoDS_Edge& e = ee.Edge();
    BB.Add(w, e);
  }
  
  w.Closed (BRep_Tool::IsClosed (w));
  BRepLib_MakeFace fa(myPln->Pln(), w, Standard_True);
  TopoDS_Face fac = TopoDS::Face(fa.Shape());
  
  if (!BRepAlgo::IsValid(fac)) {
#ifdef OCCT_DEBUG
    if (trc) cout << " Invalid Face" << endl;
#endif
    ProfileOK = Standard_False;
    return ProfileOK;
  }
  
//    if(!Concavite) {
  if(Concavite == 3) {
    BRepTopAdaptor_FClass2d Cl(fac, BRep_Tool::Tolerance(fac));
    Standard_Real u, v;
    ElSLib::Parameters(myPln->Pln(), CheckPnt, u, v);
    gp_Pnt2d checkpnt2d(u, v);
    if(Cl.Perform(checkpnt2d, Standard_True) == TopAbs_OUT) {
      BRepAlgoAPI_Cut c(BndFace, fac);     
      TopExp_Explorer exp(c.Shape(), TopAbs_WIRE);
      UpdateDescendants(c, c.Shape(), Standard_False);
      const TopoDS_Wire& ww = TopoDS::Wire(exp.Current());
      BRepLib_MakeFace ff(ww);
      Prof = TopoDS::Face(ff.Shape());
    }
    else {
      Prof = fac;
    }
  }
  else {
    Prof = fac;
  }  

  if (!BRepAlgo::IsValid(Prof)) {
#ifdef OCCT_DEBUG
    if (trc) cout << " Invalid Face Profile" << endl;
#endif
    ProfileOK = Standard_False;
    return ProfileOK;
  }
  return ProfileOK;
}

//=======================================================================
//function : UpdateDescendants
//purpose  : 
//=======================================================================
  void BRepFeat_RibSlot::UpdateDescendants(const BRepAlgoAPI_BooleanOperation& aBOP,
                                           const TopoDS_Shape& S,
                                           const Standard_Boolean SkipFace)
{
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itdm;
  TopTools_ListIteratorOfListOfShape it,it2;
  TopTools_MapIteratorOfMapOfShape itm;
  TopExp_Explorer exp;

  for (itdm.Initialize(myMap);itdm.More();itdm.Next()) {
    const TopoDS_Shape& orig = itdm.Key();
    if (SkipFace && orig.ShapeType() == TopAbs_FACE) {
      continue;
    }
    TopTools_MapOfShape newdsc;

    //if (itdm.Value().IsEmpty()) {myMap.ChangeFind(orig).Append(orig);}

    for (it.Initialize(itdm.Value());it.More();it.Next()) {
      const TopoDS_Shape& sh = it.Value();
      if(sh.ShapeType() != TopAbs_FACE) continue;
      const TopoDS_Face& fdsc = TopoDS::Face(it.Value()); 
      for (exp.Init(S,TopAbs_FACE);exp.More();exp.Next()) {
        if (exp.Current().IsSame(fdsc)) { // preserved
          newdsc.Add(fdsc);
          break;
        }
      }
      if (!exp.More()) {
        BRepAlgoAPI_BooleanOperation* pBOP=(BRepAlgoAPI_BooleanOperation*)&aBOP;
        const TopTools_ListOfShape& aLM=pBOP->Modified(fdsc);
        it2.Initialize(aLM);
        for (; it2.More(); it2.Next()) {
          const TopoDS_Shape& aS=it2.Value();
          newdsc.Add(aS);
        }
        
      }
    }
    myMap.ChangeFind(orig).Clear();
    for (itm.Initialize(newdsc); itm.More(); itm.Next()) {
       // check the belonging to the shape...
      for (exp.Init(S,TopAbs_FACE);exp.More();exp.Next()) {
        if (exp.Current().IsSame(itm.Key())) {
//          const TopoDS_Shape& sh = itm.Key();
          myMap.ChangeFind(orig).Append(itm.Key());
          break;
        }
      }
    }
  }
}
