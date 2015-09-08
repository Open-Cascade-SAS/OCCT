// Created on: 1995-10-27
// Created by: Yves FRICAUD
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

//  Modified by skv - Tue Mar 15 16:20:43 2005
// Add methods for supporting history.
//  Modified by skv - Mon Jan 12 11:50:02 2004 OCC4455

#include <Adaptor3d_CurveOnSurface.hxx>
#include <BRep_Builder.hxx>
#include <BRep_ListIteratorOfListOfPointRepresentation.hxx>
#include <BRep_PointRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRep_TVertex.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAlgo_AsDes.hxx>
#include <BRepAlgo_Image.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_Edge.hxx>
#include <BRepCheck_Vertex.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepGProp.hxx>
#include <BRepLib.hxx>
#include <BRepLib_FindSurface.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <BRepOffset_Analyse.hxx>
#include <BRepOffset_DataMapIteratorOfDataMapOfShapeOffset.hxx>
#include <BRepOffset_DataMapOfShapeMapOfShape.hxx>
#include <BRepOffset_DataMapOfShapeOffset.hxx>
#include <BRepOffset_Inter2d.hxx>
#include <BRepOffset_Inter3d.hxx>
#include <BRepOffset_Interval.hxx>
#include <BRepOffset_ListOfInterval.hxx>
#include <BRepOffset_MakeLoops.hxx>
#include <BRepOffset_MakeOffset.hxx>
#include <BRepOffset_Offset.hxx>
#include <BRepOffset_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepTools_Quilt.hxx>
#include <BRepTools_Substitution.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <GC_MakeCylindricalSurface.hxx>
#include <GCE2d_MakeLine.hxx>
#include <gce_MakeCone.hxx>
#include <gce_MakeDir.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomFill_Generator.hxx>
#include <GeomLib.hxx>
#include <gp_Cone.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt.hxx>
#include <GProp_GProps.hxx>
#include <IntTools_FClass2d.hxx>
#include <NCollection_List.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeReal.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <Geom_Line.hxx>
#include <NCollection_Vector.hxx>

#include <stdio.h>
// POP for NT
#ifdef DRAW

#include <DBRep.hxx>
#endif
#ifdef OCCT_DEBUG
#include <OSD_Chronometer.hxx>
//#define DEB_VERB
  Standard_Boolean AffichInt2d = Standard_False;       
  Standard_Boolean AffichOffC  = Standard_False;       
  Standard_Boolean ChronBuild  = Standard_False;
  Standard_Integer NbAE        = 0;
  Standard_Integer NbAF        = 0;  
  Standard_Integer NVP        = 0;  
  Standard_Integer NVM        = 0;  
  Standard_Integer NVN        = 0;  
  static OSD_Chronometer  Clock;
  char name[100];




//=======================================================================
//function :  DEBVerticesControl
//purpose  : 
//=======================================================================

static void DEBVerticesControl (const TopTools_IndexedMapOfShape& NewEdges,
				      Handle(BRepAlgo_AsDes)      AsDes)
{
  TopTools_ListOfShape               LVP;
  TopTools_ListIteratorOfListOfShape it1LE ;    
  TopTools_ListIteratorOfListOfShape it2LE ;
  
  Standard_Integer i;
  for (i = 1; i <= NewEdges.Extent(); i++) {
    const TopoDS_Edge& NE = TopoDS::Edge(NewEdges(i));
    if (AsDes->HasDescendant(NE)) {
      for (it1LE.Initialize(AsDes->Descendant(NE)); it1LE.More(); it1LE.Next()) {
	if (AsDes->Ascendant(it1LE.Value()).Extent() < 3) {
	  LVP.Append(it1LE.Value());
	  cout <<"Vertex on at least 3 edges."<<endl;
#ifdef DRAW
	  if (AffichInt2d) {
	    sprintf (name,"VP_%d",NVP++);
	    DBRep::Set(name,it1LE.Value());
	  }
#endif
	}
	else if (AsDes->Ascendant(it1LE.Value()).Extent() > 3) {
	  cout <<"Vertex on more than 3 edges."<<endl;
#ifdef DRAW
	  if (AffichInt2d) {
	    sprintf (name,"VM_%d",NVM++);
	    DBRep::Set(name,it1LE.Value());
	  }
#endif
	  
	}
	else {
#ifdef DRAW
	  if (AffichInt2d) {
	    sprintf (name,"VN_%d",NVN++);
	    DBRep::Set(name,it1LE.Value());
	  }
#endif
	}
      }
    }
  }
  //------------------------------------------------
  // Try to mix spoiled vertices.
  //------------------------------------------------
  BRep_Builder B;
  TopTools_ListIteratorOfListOfShape it1(LVP);
  Standard_Real                      TolConf = 1.e-5;
  Standard_Real                      Tol     = Precision::Confusion();
  //Standard_Integer                   i = 1;
  
  i = 1;
  for ( ; it1.More(); it1.Next()) {
    TopoDS_Shape   V1 = it1.Value();
    gp_Pnt         P1 = BRep_Tool::Pnt(TopoDS::Vertex(V1));
    Standard_Real  distmin = Precision::Infinite();
    TopTools_ListIteratorOfListOfShape it2(LVP);
    Standard_Integer j = 1;

    for ( ; it2.More(); it2.Next()) {
      if (j > i) {
	TopoDS_Shape V2 = it2.Value();
	gp_Pnt       P2 = BRep_Tool::Pnt(TopoDS::Vertex(V2));
	if (!V1.IsSame(V2)) {
	  Standard_Real       dist    = P1.Distance(P2);
	  if (dist < distmin) distmin = dist;
	  if (dist < TolConf) {
	    Standard_Real UV2;
	    TopoDS_Edge   EWE2;
	    const TopTools_ListOfShape& EdgeWithV2 = AsDes->Ascendant(V2);
	    TopTools_ListIteratorOfListOfShape itAsDes;
	    for (itAsDes.Initialize(EdgeWithV2); itAsDes.More(); itAsDes.Next()) {
	      EWE2  = TopoDS::Edge(itAsDes.Value());
	      TopoDS_Shape aLocalShape = V2.Oriented(TopAbs_INTERNAL);
	      UV2   = BRep_Tool::Parameter(TopoDS::Vertex(aLocalShape),EWE2);
	      aLocalShape = V1.Oriented(TopAbs_INTERNAL) ;
	      B.UpdateVertex(TopoDS::Vertex(aLocalShape),UV2,EWE2,Tol);
//	      UV2   = 
//		BRep_Tool::Parameter(TopoDS::Vertex(),EWE2);
//	      B.UpdateVertex(TopoDS::Vertex(V1.Oriented(TopAbs_INTERNAL)),
//			     UV2,EWE2,Tol);
	    }
	    AsDes->Replace(V2,V1);
	  }
	}
      }
      j++;
    }
    i++;
    cout <<" distmin between VP : "<<distmin<<endl;
  }
}  
#endif

static BRepOffset_Error checkSinglePoint(const Standard_Real theUParam,
                                         const Standard_Real theVParam,
                                         const Handle(Geom_Surface)& theSurf,
                                         const NCollection_Vector<gp_Pnt>& theBadPoints);

//---------------------------------------------------------------------
static void UpdateTolerance (      TopoDS_Shape&               myShape,
			     const TopTools_IndexedMapOfShape& myFaces);

static void CorrectSolid(TopoDS_Solid& theSol, TopTools_ListOfShape& theSolList);
//---------------------------------------------------------------------
//
static Standard_Boolean FindParameter(const TopoDS_Vertex& V, 
				      const TopoDS_Edge& E,
				      Standard_Real& U)
{
  // Search the vertex in the edge

  Standard_Boolean rev = Standard_False;
  TopoDS_Shape VF;
  TopAbs_Orientation orient = TopAbs_INTERNAL;

  TopoDS_Iterator itv(E.Oriented(TopAbs_FORWARD));

  // if the edge has no vertices
  // and is degenerated use the vertex orientation
  // RLE, june 94

  if (!itv.More() && BRep_Tool::Degenerated(E)) {
    orient = V.Orientation();
  }

  while (itv.More()) {
    const TopoDS_Shape& Vcur = itv.Value();
    if (V.IsSame(Vcur)) {
      if (VF.IsNull()) {
	VF = Vcur;
      }
      else {
	rev = E.Orientation() == TopAbs_REVERSED;
	if (Vcur.Orientation() == V.Orientation()) {
	  VF = Vcur;
	}
      }
    }
    itv.Next();
  }
  
  if (!VF.IsNull()) orient = VF.Orientation();
 
  Standard_Real f,l;

  if (orient ==  TopAbs_FORWARD) {
    BRep_Tool::Range(E,f,l);
    //return (rev) ? l : f;
    U = (rev) ? l : f;
    return Standard_True;
  }
 
  else if (orient ==  TopAbs_REVERSED) {
    BRep_Tool::Range(E,f,l);
    //return (rev) ? f : l;
    U = (rev) ? f : l;
    return Standard_True;
   }

  else {
    TopLoc_Location L;
    const Handle(Geom_Curve)& C = BRep_Tool::Curve(E,L,f,l);
    L = L.Predivided(V.Location());
    if (!C.IsNull() || BRep_Tool::Degenerated(E)) {
      BRep_ListIteratorOfListOfPointRepresentation itpr
	((*((Handle(BRep_TVertex)*) &V.TShape()))->Points());

      while (itpr.More()) {
	const Handle(BRep_PointRepresentation)& pr = itpr.Value();
	if (pr->IsPointOnCurve(C,L)) {
	  Standard_Real p = pr->Parameter();
	  Standard_Real res = p;// SVV 4 nov 99 - to avoid warnings on Linux
	  if (!C.IsNull()) {
	    // Closed curves RLE 16 june 94
	    if (Precision::IsNegativeInfinite(f))
	      {
		//return pr->Parameter();//p;
		U = pr->Parameter();
		return Standard_True;
	      }
	    if (Precision::IsPositiveInfinite(l))
	      {
		//return pr->Parameter();//p;
		U = pr->Parameter();
		return Standard_True;
	      }
	    gp_Pnt Pf = C->Value(f).Transformed(L.Transformation());
	    gp_Pnt Pl = C->Value(l).Transformed(L.Transformation());
	    Standard_Real tol = BRep_Tool::Tolerance(V);
	    if (Pf.Distance(Pl) < tol) {
	      if (Pf.Distance(BRep_Tool::Pnt(V)) < tol) {
		if (V.Orientation() == TopAbs_FORWARD) res = f;//p = f;
		else                                   res = l;//p = l;
	      }
	    }
	  }
	  //return res;//p;
	  U = res;
	  return Standard_True;
	}
	itpr.Next();
      }
    }
    else {
      // no 3d curve !!
      // let us try with the first pcurve
      Handle(Geom2d_Curve) PC;
      Handle(Geom_Surface) S;
      BRep_Tool::CurveOnSurface(E,PC,S,L,f,l);
      L = L.Predivided(V.Location()); 
      BRep_ListIteratorOfListOfPointRepresentation itpr
	((*((Handle(BRep_TVertex)*) &V.TShape()))->Points());

      while (itpr.More()) {
	const Handle(BRep_PointRepresentation)& pr = itpr.Value();
	if (pr->IsPointOnCurveOnSurface(PC,S,L)) {
	  Standard_Real p = pr->Parameter();
	  // Closed curves RLE 16 june 94
	  if (PC->IsClosed()) {
	    if ((p == PC->FirstParameter()) || 
		(p == PC->LastParameter())) {
	      if (V.Orientation() == TopAbs_FORWARD) p = PC->FirstParameter();
	      else                                   p = PC->LastParameter();
	    }
	  }
	  //return p;
	  U = p;
	  return Standard_True;
	}
	itpr.Next();
      }
    }
  }
  
  //Standard_NoSuchObject::Raise("BRep_Tool:: no parameter on edge");
  return Standard_False;
}

//=======================================================================
//function : GetEdgePoints
//purpose  : gets the first, last and middle points of the edge
//=======================================================================
static void GetEdgePoints(const TopoDS_Edge& anEdge,
                                      const TopoDS_Face& aFace,
                                      gp_Pnt& fPnt, gp_Pnt& mPnt,
                                      gp_Pnt& lPnt)
{
  Standard_Real f, l;
  Handle(Geom2d_Curve) theCurve = BRep_Tool::CurveOnSurface( anEdge, aFace, f, l );
  gp_Pnt2d fPnt2d = theCurve->Value(f);
  gp_Pnt2d lPnt2d = theCurve->Value(l);
  gp_Pnt2d mPnt2d = theCurve->Value(0.5*(f + l));
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
  fPnt = aSurf->Value(fPnt2d.X(),fPnt2d.Y());
  lPnt = aSurf->Value(lPnt2d.X(),lPnt2d.Y());
  mPnt = aSurf->Value(mPnt2d.X(), mPnt2d.Y());
}

//=======================================================================
//function : FillContours
//purpose  : fills free boundary contours and faces connected (MapEF)
//=======================================================================
static void FillContours(const TopoDS_Shape& aShape,
			 const BRepOffset_Analyse& Analyser,
			 TopTools_DataMapOfShapeListOfShape& Contours,
			 TopTools_DataMapOfShapeShape& MapEF)
{
  TopTools_ListOfShape Edges;

  TopExp_Explorer Explo(aShape, TopAbs_FACE);
  BRepTools_WireExplorer Wexp;

  for (; Explo.More(); Explo.Next())
    {
      TopoDS_Face aFace = TopoDS::Face(Explo.Current());
      TopoDS_Iterator itf(aFace);
      for (; itf.More(); itf.Next())
	{
	  TopoDS_Wire aWire = TopoDS::Wire(itf.Value());
	  for (Wexp.Init(aWire, aFace); Wexp.More(); Wexp.Next())
	    {
	      TopoDS_Edge anEdge = Wexp.Current();
	      if (BRep_Tool::Degenerated(anEdge))
		continue;
	      const BRepOffset_ListOfInterval& Lint = Analyser.Type(anEdge);
	      if (!Lint.IsEmpty() && Lint.First().Type() == BRepOffset_FreeBoundary)
		{
		  MapEF.Bind(anEdge, aFace);
		  Edges.Append(anEdge);
		}
	    }
	}
    }

  TopTools_ListIteratorOfListOfShape itl;
  while (!Edges.IsEmpty())
    {
      TopoDS_Edge StartEdge = TopoDS::Edge(Edges.First());
      Edges.RemoveFirst();
      TopoDS_Vertex StartVertex, CurVertex;
      TopExp::Vertices(StartEdge, StartVertex, CurVertex, Standard_True);
      TopTools_ListOfShape aContour;
      aContour.Append(StartEdge);
      while (!CurVertex.IsSame(StartVertex))
	for (itl.Initialize(Edges); itl.More(); itl.Next())
	  {
	    TopoDS_Edge anEdge = TopoDS::Edge(itl.Value());
	    TopoDS_Vertex V1, V2;
	    TopExp::Vertices(anEdge, V1, V2);
	    if (V1.IsSame(CurVertex) || V2.IsSame(CurVertex))
	      {
		aContour.Append(anEdge);
		CurVertex = (V1.IsSame(CurVertex))? V2 : V1;
		Edges.Remove(itl);
		break;
	      }
	  }
      Contours.Bind(StartVertex, aContour);
    }
}


//
//-----------------------------------------------------------------------
//
//=======================================================================
//function : BRepOffset_MakeOffset
//purpose  : 
//=======================================================================

BRepOffset_MakeOffset::BRepOffset_MakeOffset()
{
  myAsDes = new BRepAlgo_AsDes();
}


//=======================================================================
//function : BRepOffset_MakeOffset
//purpose  : 
//=======================================================================

BRepOffset_MakeOffset::BRepOffset_MakeOffset(const TopoDS_Shape&    S, 
					     const Standard_Real    Offset, 
					     const Standard_Real    Tol, 
					     const BRepOffset_Mode  Mode, 
					     const Standard_Boolean Inter, 
					     const Standard_Boolean SelfInter, 
					     const GeomAbs_JoinType Join,
					     const Standard_Boolean Thickening)
: 
myOffset     (Offset),
myTol        (Tol),
myShape      (S),
myMode       (Mode),
myInter      (Inter),
mySelfInter  (SelfInter),
myJoin       (Join),
myThickening (Thickening),
myDone     (Standard_False)

{
  myAsDes = new BRepAlgo_AsDes();
  MakeOffsetShape();
}


//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void BRepOffset_MakeOffset::Initialize(const TopoDS_Shape&    S, 
				       const Standard_Real    Offset, 
				       const Standard_Real    Tol, 
				       const BRepOffset_Mode  Mode,
				       const Standard_Boolean Inter,
				       const Standard_Boolean SelfInter,
				       const GeomAbs_JoinType Join,
				       const Standard_Boolean Thickening)
{
  myOffset     = Offset;
  myShape      = S;
  myTol        = Tol;
  myMode       = Mode;
  myInter      = Inter;
  mySelfInter  = SelfInter;
  myJoin       = Join;
  myThickening = Thickening;
  myDone     = Standard_False;
  myIsPerformSewing = Standard_False;
  Clear();
}


//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void BRepOffset_MakeOffset::Clear()
{
  myOffsetShape.Nullify();
  myInitOffsetFace .Clear();
  myInitOffsetEdge .Clear();
  myImageOffset    .Clear();
  myFaces          .Clear();  
  myFaceOffset     .Clear();
  myAsDes          ->Clear();
  myDone     = Standard_False;
}

//=======================================================================
//function : AddFace
//purpose  : 
//=======================================================================

void BRepOffset_MakeOffset::AddFace(const TopoDS_Face& F) {
  myFaces.Add(F);    
  //-------------
  // MAJ SD.
  //-------------
  myInitOffsetFace.SetRoot (F)  ;    
  myInitOffsetFace.Bind    (F,F);
  myImageOffset.SetRoot    (F)  ;  
}

//=======================================================================
//function : SetOffsetOnFace
//purpose  : 
//=======================================================================

void BRepOffset_MakeOffset::SetOffsetOnFace(const TopoDS_Face&  F, 
					    const Standard_Real Off)
{
  if ( myFaceOffset.IsBound(F)) myFaceOffset.UnBind(F);
  myFaceOffset.Bind(F,Off);
}

//=======================================================================
//function : RemoveCorks
//purpose  : 
//=======================================================================

static void RemoveCorks (TopoDS_Shape&               S,
			 TopTools_IndexedMapOfShape& Faces)
{  
  TopoDS_Compound SS;
  BRep_Builder    B;
  B.MakeCompound (SS);
  //-----------------------------------------------------
  // Construction of a shape without caps.
  // and Orientation of caps as in shape S.
  //-----------------------------------------------------
  TopExp_Explorer exp(S,TopAbs_FACE);
  for (; exp.More(); exp.Next()) {
    const TopoDS_Shape& Cork = exp.Current(); 
    if (!Faces.Contains(Cork)) {
      B.Add(SS,Cork);
    }
    else {
      //Faces.Remove (Cork);
      //begin instead of Remove//
      TopoDS_Shape LastShape = Faces(Faces.Extent());
      Faces.RemoveLast();
      if (Faces.FindIndex(Cork) != 0)
        Faces.Substitute(Faces.FindIndex(Cork), LastShape);
      //end instead of Remove  //
      Faces.Add(Cork); // to reset it with proper orientation.
    }
  }
  S = SS;
#ifdef DRAW
  if ( AffichOffC) 
    DBRep::Set("myInit", SS);
#endif

}

static Standard_Boolean IsConnectedShell( const TopoDS_Shape& S )
{  
  BRepTools_Quilt Glue;
  Glue.Add( S );

  TopoDS_Shape SS = Glue.Shells();
  TopExp_Explorer Explo( SS, TopAbs_SHELL );
  Explo.Next();
  if (Explo.More())
    return Standard_False;
  
  return Standard_True;
}


//=======================================================================
//function : MakeList
//purpose  : 
//=======================================================================

static void MakeList (TopTools_ListOfShape&             OffsetFaces,
		      const BRepAlgo_Image&             myInitOffsetFace,
		      const TopTools_IndexedMapOfShape& myFaces)
{
  TopTools_ListIteratorOfListOfShape itLOF(myInitOffsetFace.Roots());
  for ( ; itLOF.More(); itLOF.Next()) {
    const TopoDS_Shape& Root = itLOF.Value();
    if (!myFaces.Contains(Root))
      OffsetFaces.Append(myInitOffsetFace.Image(Root).First());
  }
}

//=======================================================================
//function : EvalMax
//purpose  : 
//=======================================================================

static void EvalMax(const TopoDS_Shape& S, Standard_Real& Tol)
{
  TopExp_Explorer exp;
  for (exp.Init(S,TopAbs_VERTEX); exp.More(); exp.Next()) {
    const TopoDS_Vertex& V    = TopoDS::Vertex(exp.Current());
    Standard_Real        TolV = BRep_Tool::Tolerance(V); 
    if (TolV > Tol) Tol = TolV;
  }
}


//=======================================================================
//function : MakeOffsetShape
//purpose  : 
//=======================================================================

void BRepOffset_MakeOffset::MakeOffsetShape()
{  
  myDone = Standard_False;
  //------------------------------------------
  // Construction of myShape without caps.
  //------------------------------------------
  if(!myFaces.IsEmpty())
  {
    RemoveCorks (myShape,myFaces);
  }

  if (!CheckInputData())
  {
    // There is error in input data.
    // Check Error() method.
    return;
  }

  TopAbs_State       Side = TopAbs_IN;
  if (myOffset < 0.) Side = TopAbs_OUT;

  // ------------
  // Preanalyse.
  // ------------
  EvalMax(myShape,myTol);
  // There are possible second variant: analytical continuation of arcsin.
  Standard_Real TolAngleCoeff = Min(myTol / (Abs(myOffset * 0.5) + Precision::Confusion()), 1.0);
  Standard_Real TolAngle = 4*ASin(TolAngleCoeff);
  myAnalyse.Perform(myShape,TolAngle);
  //---------------------------------------------------
  // Construction of Offset from preanalysis.
  //---------------------------------------------------  
  //----------------------------
  // MaJ of SD Face - Offset
  //----------------------------
  UpdateFaceOffset();

  if (myJoin == GeomAbs_Arc)          
    BuildOffsetByArc();
  else if (myJoin == GeomAbs_Intersection) 
    BuildOffsetByInter();
  //-----------------
  // Auto unwinding.
  //-----------------
  // if (mySelfInter)  SelfInter(Modif);
  //-----------------
  // Intersection 3d .
  //-----------------
  BRepOffset_Inter3d Inter(myAsDes,Side,myTol);
  Intersection3D (Inter);
  //-----------------
  // Intersection2D
  //-----------------
  TopTools_IndexedMapOfShape& Modif    = Inter.TouchedFaces(); 
  TopTools_IndexedMapOfShape& NewEdges = Inter.NewEdges();

  if (!Modif.IsEmpty()) Intersection2D (Modif,NewEdges);
  //-------------------------------------------------------
  // Unwinding 2D and reconstruction of modified faces
  //----------------------------------------------------
  MakeLoops (Modif);
  //-----------------------------------------------------
  // Reconstruction of non modified faces sharing 
  // reconstructed edges
  //------------------------------------------------------
  if (!Modif.IsEmpty()) MakeFaces (Modif);

  if (myThickening)
    MakeMissingWalls();

  //-------------------------
  // Construction of shells.
  //-------------------------
  MakeShells ();
  //--------------
  // Unwinding 3D.
  //--------------
  SelectShells ();
  //----------------------------------
  // Coding of regularities.
  //----------------------------------
  EncodeRegularity();
  //----------------------
  // Creation of solids.
  //----------------------
  MakeSolid ();

  //-----------------------------
  // MAJ Tolerance edge and Vertex
  // ----------------------------
  if (!myOffsetShape.IsNull()) {
    UpdateTolerance (myOffsetShape,myFaces);
    BRepLib::UpdateTolerances( myOffsetShape );
  }

  CorrectConicalFaces();

  // Result solid should be computed in MakeOffset scope.
  if (myThickening &&
      myIsPerformSewing)
  {
    BRepBuilderAPI_Sewing aSew(myTol);
    aSew.Add(myOffsetShape);
    aSew.Perform();
    myOffsetShape = aSew.SewedShape();

    // Rebuild solid.
    // Offset shape expected to be really closed after sewing.
    myOffsetShape.Closed(Standard_True);
    MakeSolid();
  }

  myDone = Standard_True;
}



//=======================================================================
//function : MakeThickSolid
//purpose  : 
//=======================================================================

void BRepOffset_MakeOffset::MakeThickSolid() 
{
  //--------------------------------------------------------------
  // Construction of shell parallel to shell (initial without cap).
  //--------------------------------------------------------------
  MakeOffsetShape ();

  if (!myDone)
  {
    // Save return code and myDone state.
    return;
  }

  //--------------------------------------------------------------------
  // Construction of a solid with the initial shell, parallel shell 
  // limited by caps.
  //--------------------------------------------------------------------
  if (!myFaces.IsEmpty())
  {
    TopoDS_Solid    Res;
    TopExp_Explorer exp;
    BRep_Builder    B;
    Standard_Integer NbF = myFaces.Extent();

    B.MakeSolid(Res);

    BRepTools_Quilt Glue;
    for (exp.Init(myShape,TopAbs_FACE); exp.More(); exp.Next())
    {
      NbF++;
      Glue.Add (exp.Current());
    } 
    Standard_Boolean YaResult = 0;
    if (!myOffsetShape.IsNull())
      {
      for (exp.Init(myOffsetShape,TopAbs_FACE);exp.More(); exp.Next())
        {
        YaResult = 1;
        Glue.Add (exp.Current().Reversed());
        }
#ifdef OCCT_DEBUG
      if(YaResult == 0)
        {
        cout << "OffsetShape does not contain a FACES." << endl;
        }
#endif
      }
#ifdef OCCT_DEBUG
    else
      {
      cout << "OffsetShape is null!" << endl;
      }
#endif

    if (YaResult == 0)
      {
      myDone = Standard_False;
      myError = BRepOffset_UnknownError;
      return;
      }

    myOffsetShape = Glue.Shells();
    for (exp.Init(myOffsetShape,TopAbs_SHELL); exp.More(); exp.Next())
    {
      B.Add(Res,exp.Current());
    }
    Res.Closed(Standard_True);
    myOffsetShape = Res;

    // Test of Validity of the result of thick Solid 
    // more face than the initial solid.
    Standard_Integer NbOF = 0;
    for (exp.Init(myOffsetShape,TopAbs_FACE);exp.More(); exp.Next())
    {
      NbOF++;
    }
    if (NbOF <= NbF)
    {
      myDone = Standard_False;
      myError = BRepOffset_UnknownError;
      return;
    }
  }

  if (myOffset > 0 ) myOffsetShape.Reverse();

  myDone = Standard_True;
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean BRepOffset_MakeOffset::IsDone() const
{
  return myDone;
}

//=======================================================================
//function : Error
//purpose  : 
//=======================================================================

BRepOffset_Error BRepOffset_MakeOffset::Error() const
{
  return myError;
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

const TopoDS_Shape&  BRepOffset_MakeOffset::Shape() const 
{
  return myOffsetShape;
}

//=======================================================================
//function : TrimEdge
//purpose  : Trim the edge of the largest of descendants in AsDes2d.
//           Order in AsDes two vertices that have trimmed the edge.
//=======================================================================

static void TrimEdge (TopoDS_Edge&                  NE,
		      const Handle(BRepAlgo_AsDes)& AsDes2d,
		            Handle(BRepAlgo_AsDes)& AsDes)
{
  Standard_Real aSameParTol = Precision::Confusion();
  
  TopoDS_Vertex V1,V2;
  Standard_Real U = 0.;
  Standard_Real UMin =  Precision::Infinite();
  Standard_Real UMax = -UMin;

  const TopTools_ListOfShape& LE = AsDes2d->Descendant(NE);
  
  if (LE.Extent() > 1) {
    TopTools_ListIteratorOfListOfShape it (LE);
    for (; it.More(); it.Next()) {
      TopoDS_Vertex V = TopoDS::Vertex(it.Value());
      if (NE.Orientation() == TopAbs_REVERSED)
	V.Reverse();
      //V.Orientation(TopAbs_INTERNAL);
      if (!FindParameter(V, NE, U))
	{
	  Standard_Real f, l;
	  Handle(Geom_Curve) theCurve = BRep_Tool::Curve(NE, f, l);
	  gp_Pnt thePoint = BRep_Tool::Pnt(V);
	  GeomAPI_ProjectPointOnCurve Projector(thePoint, theCurve);
	  if (Projector.NbPoints() == 0)
	    Standard_ConstructionError::Raise("BRepOffset_MakeOffset::TrimEdge no projection");
	  U = Projector.LowerDistanceParameter();
	}
      if (U < UMin) {
	UMin = U; V1   = V;
      }
      if (U > UMax) {
	UMax = U; V2   = V;
      }
    }
    if (V1.IsNull() || V2.IsNull()) {
      Standard_ConstructionError::Raise("BRepOffset_MakeOffset::TrimEdge");
    }
    if (!V1.IsSame(V2)) {
      NE.Free( Standard_True );
      BRep_Builder B;
      TopAbs_Orientation Or = NE.Orientation();
      NE.Orientation(TopAbs_FORWARD);
      TopoDS_Vertex VF,VL;
      TopExp::Vertices (NE,VF,VL);
      B.Remove(NE,VF);
      B.Remove(NE,VL);
      B.Add  (NE,V1.Oriented(TopAbs_FORWARD));
      B.Add  (NE,V2.Oriented(TopAbs_REVERSED));
      B.Range(NE,UMin,UMax);
      NE.Orientation(Or);
      AsDes->Add(NE,V1.Oriented(TopAbs_FORWARD));
      AsDes->Add(NE,V2.Oriented(TopAbs_REVERSED));
      BRepLib::SameParameter(NE, aSameParTol, Standard_True);
    }
  }
}

//=======================================================================
//function : BuildOffsetByInter
//purpose  : 
//=======================================================================
void BRepOffset_MakeOffset::BuildOffsetByInter()
{
#ifdef OCCT_DEBUG
  if ( ChronBuild) {
    cout << " CONSTRUCTION OF OFFSETS :" << endl;
    Clock.Reset();
    Clock.Start();
  }
#endif

  BRepOffset_DataMapOfShapeOffset MapSF;
  TopTools_MapOfShape             Done;
  Standard_Boolean OffsetOutside = (myOffset > 0.)? Standard_True : Standard_False;
  //--------------------------------------------------------
  // Construction of faces parallel to initial faces
  //--------------------------------------------------------
  TopExp_Explorer Exp;
  TopTools_ListOfShape LF;
  TopTools_ListIteratorOfListOfShape itLF;

  BRepLib::SortFaces(myShape,LF);

  TopTools_DataMapOfShapeShape ShapeTgt;
  for (itLF.Initialize(LF); itLF.More(); itLF.Next()) {
    const TopoDS_Face&   F = TopoDS::Face(itLF.Value());
    Standard_Real CurOffset = myOffset;
    if (myFaceOffset.IsBound(F)) CurOffset = myFaceOffset(F);
    BRepOffset_Offset    OF(F,CurOffset,ShapeTgt,OffsetOutside,myJoin);
    TopTools_ListOfShape Let;
    myAnalyse.Edges(F,BRepOffset_Tangent,Let);
    TopTools_ListIteratorOfListOfShape itl(Let);
    
    for ( ; itl.More(); itl.Next()) {
      const TopoDS_Edge& Cur = TopoDS::Edge(itl.Value());
      if ( !ShapeTgt.IsBound(Cur)) {
	TopoDS_Shape aLocalShape = OF.Generated(Cur);
	const TopoDS_Edge& OTE = TopoDS::Edge(aLocalShape);
//	const TopoDS_Edge& OTE = TopoDS::Edge(OF.Generated(Cur));
	ShapeTgt.Bind(Cur,OF.Generated(Cur));
	TopoDS_Vertex V1,V2,OV1,OV2;
	TopExp::Vertices (Cur,V1,V2);
	TopExp::Vertices (OTE,OV1,OV2);      
	TopTools_ListOfShape LE;
	if (!ShapeTgt.IsBound(V1)) {
	  myAnalyse.Edges(V1,BRepOffset_Tangent,LE);
	  const TopTools_ListOfShape& LA =myAnalyse.Ancestors(V1);
	  if (LE.Extent() == LA.Extent())
	    ShapeTgt.Bind(V1,OV1);
	}
	if (!ShapeTgt.IsBound(V2)) {
	  LE.Clear();
	  myAnalyse.Edges(V2,BRepOffset_Tangent,LE);
	  const TopTools_ListOfShape& LA =myAnalyse.Ancestors(V2);
	  if (LE.Extent() == LA.Extent())
	    ShapeTgt.Bind(V2,OV2);
	}
      }
    }
    MapSF.Bind(F,OF);
  }
  //--------------------------------------------------------------------
  // MES   : Map of OffsetShape -> Extended Shapes.
  // Build : Map of Initial SS  -> OffsetShape build by Inter.
  //                               can be an edge or a compound of edges       
  //---------------------------------------------------------------------
  TopTools_DataMapOfShapeShape MES;  
  TopTools_DataMapOfShapeShape Build; 
  TopTools_ListOfShape         Failed;
  TopAbs_State                 Side = TopAbs_IN;  
  Handle(BRepAlgo_AsDes)       AsDes = new BRepAlgo_AsDes();

  //-------------------------------------------------------------------
  // Extension of faces and calculation of new edges of intersection.
  //-------------------------------------------------------------------
  Standard_Boolean  ExtentContext = 0;
  if (myOffset > 0) ExtentContext = 1;

  BRepOffset_Inter3d Inter3 (AsDes,Side,myTol);
  // Intersection between parallel faces
  Inter3.ConnexIntByInt(myShape,MapSF,myAnalyse,MES,Build,Failed );
  // Intersection with caps.
  Inter3.ContextIntByInt(myFaces,ExtentContext,MapSF,myAnalyse,MES,Build,Failed );


  //---------------------------------------------------------------------------------
  // Extension of neighbor edges of new edges and intersection between neighbors.
  //--------------------------------------------------------------------------------
  Handle(BRepAlgo_AsDes) AsDes2d = new BRepAlgo_AsDes();
  for (Exp.Init(myShape,TopAbs_FACE) ; Exp.More(); Exp.Next())
  {
    const TopoDS_Face& FI = TopoDS::Face(Exp.Current());
    Standard_Real aCurrFaceTol = BRep_Tool::Tolerance(FI);
    BRepOffset_Inter2d::ConnexIntByInt (FI, MapSF(FI), MES, Build, 
                                        AsDes2d, myOffset, aCurrFaceTol);
  }
  //-----------------------------------------------------------
  // Great restriction of new edges and update of AsDes.
  //------------------------------------------ ----------------
  TopTools_IndexedMapOfShape NewEdges;
  TopExp_Explorer Exp2,ExpC;
  TopoDS_Shape    NE;
  TopoDS_Edge     TNE;
  TopoDS_Face     NF;
  
  for (Exp.Init(myShape,TopAbs_FACE) ; Exp.More(); Exp.Next()) {
    const TopoDS_Face& FI = TopoDS::Face(Exp.Current());
    NF = MapSF(FI).Face();
    if (MES.IsBound(NF)) {NF = TopoDS::Face(MES(NF));}
    TopTools_MapOfShape View;
    for (Exp2.Init(FI.Oriented(TopAbs_FORWARD),TopAbs_EDGE); Exp2.More(); Exp2.Next()) {
      const TopoDS_Edge& EI = TopoDS::Edge(Exp2.Current());
      if (View.Add(EI)) {
	if (Build.IsBound(EI)) {
	  NE = Build(EI);
	  if (NE.ShapeType() == TopAbs_EDGE) {
	    if (NewEdges.Add(NE)) {TrimEdge (TopoDS::Edge(NE),AsDes2d,AsDes);}
	  }
	  else {
	    //------------------------------------------------------------
	    // The Intersections are on several edges.
	    // The pieces without intersections with neighbors  
	    // are removed from AsDes.
	    //------------------------------------------------------------
	    for (ExpC.Init(NE,TopAbs_EDGE); ExpC.More(); ExpC.Next()) {
	      if (NewEdges.Add(ExpC.Current())) {
		TopoDS_Edge NEC = TopoDS::Edge(ExpC.Current());
		NEC.Free(Standard_True);
		if (!AsDes2d->Descendant(NEC).IsEmpty()) {
		  TrimEdge (NEC,AsDes2d,AsDes);
		}
		else {
		  AsDes->Remove(NEC);
		}
	      }
	    }
	  }
	}
	else {
	  NE = MapSF(FI).Generated(EI);
	  //// modified by jgv, 19.12.03 for OCC4455 ////
	  NE.Orientation( EI.Orientation() );
	  ///////////////////////////////////////////////
	  if (MES.IsBound(NE)) {
	    NE = MES(NE);
	    NE.Orientation(EI.Orientation());
	    if (NewEdges.Add(NE)) {TrimEdge (TopoDS::Edge(NE),AsDes2d,AsDes);} 
	  }
	  AsDes->Add(NF,NE);
	} 
      }
    }
  }
  
  //--------------------------------- 
  // Intersection 2D on //
  //---------------------------------  
  TopTools_ListOfShape LFE; 
  BRepAlgo_Image     IMOE;
  for (Exp.Init(myShape,TopAbs_FACE) ; Exp.More(); Exp.Next()) {
    const TopoDS_Shape& FI  = Exp.Current();
    const TopoDS_Shape& OFI = MapSF(FI).Face();
    if (MES.IsBound(OFI)) {
      const TopoDS_Face& aLocalFace = TopoDS::Face(MES(OFI));
      LFE.Append(aLocalFace);
      IMOE.SetRoot(aLocalFace);
    }
  }
  
  TopTools_ListIteratorOfListOfShape itLFE(LFE);
  for (; itLFE.More(); itLFE.Next())
  {
    const TopoDS_Face& NEF = TopoDS::Face(itLFE.Value());
    Standard_Real aCurrFaceTol = BRep_Tool::Tolerance(NEF);
    BRepOffset_Inter2d::Compute(AsDes, NEF, NewEdges, aCurrFaceTol);
  }
  //----------------------------------------------
  // Intersections 2d on caps.
  //----------------------------------------------
  Standard_Integer i;
  for (i = 1; i <= myFaces.Extent(); i++)
  {
    const TopoDS_Face& Cork = TopoDS::Face(myFaces(i));
    Standard_Real aCurrFaceTol = BRep_Tool::Tolerance(Cork);
    BRepOffset_Inter2d::Compute(AsDes, Cork, NewEdges, aCurrFaceTol);
  }

  //-------------------------------
  // Unwinding of extended Faces.
  //-------------------------------
  myMakeLoops.Build(LFE  ,AsDes,IMOE);

#ifdef OCCT_DEBUG
  TopTools_IndexedMapOfShape COES;
#endif
  //---------------------------
  // MAJ SD. for faces //
  //---------------------------
  for (Exp.Init(myShape,TopAbs_FACE) ; Exp.More(); Exp.Next()) {
    const TopoDS_Shape& FI   = Exp.Current();
    myInitOffsetFace.SetRoot(FI);
    TopoDS_Face  OF  = MapSF(FI).Face();
    if (MES.IsBound(OF)) {
      OF = TopoDS::Face(MES(OF));
      if (IMOE.HasImage(OF)) {
	const TopTools_ListOfShape& LOFE = IMOE.Image(OF);
	myInitOffsetFace.Bind(FI,LOFE);
	for (itLF.Initialize(LOFE); itLF.More(); itLF.Next()) {
	  const TopoDS_Shape& OFE =  itLF.Value();
	  myImageOffset.SetRoot(OFE);
#ifdef DRAW
	  if (AffichInt2d) {
	    sprintf(name,"AF_%d",NbAF++);
	    DBRep::Set(name,OFE);
	  }
#endif
	  TopTools_MapOfShape View;
	  for (Exp2.Init(OFE.Oriented(TopAbs_FORWARD),TopAbs_EDGE); 
	       Exp2.More(); Exp2.Next()) {
	    const TopoDS_Edge& COE = TopoDS::Edge(Exp2.Current());
	    
	    myAsDes->Add (OFE,COE);
#ifdef DRAW
	    if (AffichInt2d) {
	      sprintf(name,"AE_%d",NbAE++);
	      DBRep::Set(name,COE);
	      COES.Add(COE);
	    }
#endif
	    if (View.Add(COE)){
	      if (!myAsDes->HasDescendant(COE)) {
		TopoDS_Vertex CV1,CV2;
		TopExp::Vertices(COE,CV1,CV2);
		if (!CV1.IsNull()) myAsDes->Add(COE,CV1.Oriented(TopAbs_FORWARD));
		if (!CV2.IsNull()) myAsDes->Add(COE,CV2.Oriented(TopAbs_REVERSED));	
	      }
	    }
	  }
	}
      }
      else {
	myInitOffsetFace.Bind(FI,OF);
	myImageOffset.SetRoot(OF);
#ifdef DRAW 
	if (AffichInt2d) {
	  sprintf(name,"AF_%d",NbAF++);
	  DBRep::Set(name,OF);
	}
#endif
	const TopTools_ListOfShape& LE = AsDes->Descendant(OF);
	for (itLF.Initialize(LE) ; itLF.More(); itLF.Next()) {
	  const TopoDS_Edge& OE = TopoDS::Edge(itLF.Value());
	  if (IMOE.HasImage(OE)) {
	    const TopTools_ListOfShape& LOE = IMOE.Image(OE);
	    TopTools_ListIteratorOfListOfShape itLOE(LOE);
	    for (; itLOE.More(); itLOE.Next()) {
	      TopoDS_Shape aLocalShape = itLOE.Value().Oriented(OE.Orientation());
	      const TopoDS_Edge& COE = TopoDS::Edge(aLocalShape);
//	      const TopoDS_Edge& COE = TopoDS::Edge(itLOE.Value().Oriented(OE.Orientation()));
	      myAsDes->Add(OF,COE);
#ifdef DRAW
	      if (AffichInt2d) {
		sprintf(name,"AE_%d",NbAE++);
		DBRep::Set(name,COE);
		COES.Add(COE);
	      }
#endif
	      
	      if (!myAsDes->HasDescendant(COE)) {
		TopoDS_Vertex CV1,CV2;
		TopExp::Vertices(COE,CV1,CV2);
 		if (!CV1.IsNull()) myAsDes->Add(COE,CV1.Oriented(TopAbs_FORWARD));
		if (!CV2.IsNull()) myAsDes->Add(COE,CV2.Oriented(TopAbs_REVERSED));	
	      }
	    }
	  }
	  else {
	    myAsDes->Add(OF,OE);
#ifdef DRAW
	    if (AffichInt2d) {
	      sprintf(name,"AE_%d",NbAE++);
	      DBRep::Set(name,OE);
	      COES.Add(OE);
	    }
#endif

	    const TopTools_ListOfShape& LV = AsDes->Descendant(OE);
	    myAsDes->Add(OE,LV);
	  }
	}
      }
    }
    else {
      myInitOffsetFace.Bind(FI,OF);
      myImageOffset.SetRoot(OF);
      TopTools_MapOfShape View;
      for (Exp2.Init(OF.Oriented(TopAbs_FORWARD),TopAbs_EDGE); 
	   Exp2.More(); Exp2.Next()) {

	const TopoDS_Edge& COE = TopoDS::Edge(Exp2.Current());
	myAsDes->Add (OF,COE);
#ifdef DRAW
	if (AffichInt2d) {
	  sprintf(name,"AE_%d",NbAE++);
	  DBRep::Set(name,COE);
	  COES.Add(COE);
	}
#endif
	
	if (View.Add(Exp2.Current())) {
	  if (!myAsDes->HasDescendant(COE)) {
	    TopoDS_Vertex CV1,CV2;
	    TopExp::Vertices(COE,CV1,CV2);
	    if (!CV1.IsNull()) myAsDes->Add(COE,CV1.Oriented(TopAbs_FORWARD));
	    if (!CV2.IsNull()) myAsDes->Add(COE,CV2.Oriented(TopAbs_REVERSED));	
	  }
	}
      } 
    }
  }
  //  Modified by skv - Tue Mar 15 16:20:43 2005
  // Add methods for supporting history.
  TopTools_MapOfShape aMapEdges;

  for (Exp.Init(myShape,TopAbs_FACE) ; Exp.More(); Exp.Next()) {
    const TopoDS_Shape& aFaceRef = Exp.Current();

    Exp2.Init(aFaceRef.Oriented(TopAbs_FORWARD), TopAbs_EDGE);

    for (; Exp2.More(); Exp2.Next()) {
      const TopoDS_Shape& anEdgeRef = Exp2.Current();

      if (aMapEdges.Add(anEdgeRef)) {
	myInitOffsetEdge.SetRoot(anEdgeRef);
	if (Build.IsBound(anEdgeRef)) {
	  TopoDS_Shape aNewShape = Build(anEdgeRef);
	  
	  if (aNewShape.ShapeType() == TopAbs_EDGE) {
	    if (IMOE.HasImage(aNewShape)) {
	      const TopTools_ListOfShape& aListNewE = IMOE.Image(aNewShape);
	      
	      myInitOffsetEdge.Bind (anEdgeRef, aListNewE);
	    } else
	      myInitOffsetEdge.Bind (anEdgeRef, aNewShape);
	  } else { // aNewShape != TopAbs_EDGE
	    TopTools_ListOfShape aListNewEdge;
	    
	    for (ExpC.Init(aNewShape, TopAbs_EDGE); ExpC.More(); ExpC.Next()) {
	      const TopoDS_Shape &aResEdge = ExpC.Current();
	      
	      if (IMOE.HasImage(aResEdge)) {
		const TopTools_ListOfShape& aListNewE = IMOE.Image(aResEdge);
		TopTools_ListIteratorOfListOfShape aNewEIter(aListNewE);
		
		for (; aNewEIter.More(); aNewEIter.Next())
		  aListNewEdge.Append(aNewEIter.Value());
	      } else
		aListNewEdge.Append(aResEdge);
	    }
	    
	    myInitOffsetEdge.Bind (anEdgeRef, aListNewEdge);
	  }
	} 
	else { // Free boundary.
	  TopoDS_Shape aNewEdge = MapSF(aFaceRef).Generated(anEdgeRef);

	  if (MES.IsBound(aNewEdge))
	    aNewEdge = MES(aNewEdge);

	  if (IMOE.HasImage(aNewEdge)) {
	    const TopTools_ListOfShape& aListNewE = IMOE.Image(aNewEdge);

	    myInitOffsetEdge.Bind (anEdgeRef, aListNewE);
	  } else
	    myInitOffsetEdge.Bind (anEdgeRef, aNewEdge);
	}
      }
    }
  }
//  Modified by skv - Tue Mar 15 16:20:43 2005

  //---------------------------
  // MAJ SD. for caps 
  //---------------------------
  //TopTools_MapOfShape View; 
  for (i = 1; i <= myFaces.Extent(); i++) {
    const TopoDS_Shape& Cork = myFaces(i);
    const TopTools_ListOfShape& LE = AsDes->Descendant(Cork);
    for (itLF.Initialize(LE) ; itLF.More(); itLF.Next()) {
      const TopoDS_Edge& OE = TopoDS::Edge(itLF.Value());
      if (IMOE.HasImage(OE)) {
	const TopTools_ListOfShape& LOE = IMOE.Image(OE);
  	TopTools_ListIteratorOfListOfShape itLOE(LOE);
	for (; itLOE.More(); itLOE.Next()) {
	  const TopoDS_Edge& COE = TopoDS::Edge(itLOE.Value());
	  myAsDes->Add(Cork,COE.Oriented(OE.Orientation())) ;
#ifdef DRAW
	  if (AffichInt2d) {
	    sprintf(name,"AE_%d",NbAE++);
	    DBRep::Set(name,COE);
	    COES.Add(COE);
	  }
#endif
	  
	  if (!myAsDes->HasDescendant(COE)) {
	    TopoDS_Vertex CV1,CV2;
	    TopExp::Vertices(COE,CV1,CV2);
	    if (!CV1.IsNull()) myAsDes->Add(COE,CV1.Oriented(TopAbs_FORWARD));
	    if (!CV2.IsNull()) myAsDes->Add(COE,CV2.Oriented(TopAbs_REVERSED));	  
	  }
	}
      }
      else {
	myAsDes->Add(Cork,OE);
	if (AsDes->HasDescendant(OE)) {
	  myAsDes->Add(OE,AsDes->Descendant(OE));
	}
#ifdef DRAW
	if (AffichInt2d) {
	  sprintf(name,"AE_%d",NbAE++);
	  DBRep::Set(name,OE);
	  COES.Add(OE);
	}
#endif
      }
    }
  }
  
#ifdef OCCT_DEBUG
  DEBVerticesControl (COES,myAsDes);
  if ( ChronBuild) Clock.Show();
#endif
}


//=======================================================================
//function : BuildOffsetByArc
//purpose  : 
//=======================================================================
void BRepOffset_MakeOffset::BuildOffsetByArc()
{
#ifdef OCCT_DEBUG
  if ( ChronBuild) {
    cout << " CONSTRUCTION OF OFFSETS :" << endl;
    Clock.Reset();
    Clock.Start();
  }
#endif

  BRepOffset_DataMapOfShapeOffset MapSF;
  TopTools_MapOfShape             Done;
  Standard_Boolean OffsetOutside = (myOffset > 0.)? Standard_True : Standard_False;
  //--------------------------------------------------------
  // Construction of faces parallel to initial faces
  //--------------------------------------------------------
  TopExp_Explorer Exp;
  TopTools_ListOfShape LF;
  TopTools_ListIteratorOfListOfShape itLF;

  BRepLib::SortFaces(myShape,LF);

  TopTools_DataMapOfShapeShape EdgeTgt;
  for (itLF.Initialize(LF); itLF.More(); itLF.Next()) {
    const TopoDS_Face&   F = TopoDS::Face(itLF.Value());
    Standard_Real CurOffset = myOffset;
    if (myFaceOffset.IsBound(F)) CurOffset = myFaceOffset(F);
    BRepOffset_Offset    OF(F,CurOffset,EdgeTgt,OffsetOutside,myJoin);
    TopTools_ListOfShape Let;
    myAnalyse.Edges(F,BRepOffset_Tangent,Let);
    TopTools_ListIteratorOfListOfShape itl(Let);
    
    for ( ; itl.More(); itl.Next()) {
      const TopoDS_Edge& Cur = TopoDS::Edge(itl.Value());
      if ( !EdgeTgt.IsBound(Cur)) {
	TopoDS_Shape aLocalShape = OF.Generated(Cur);
	const TopoDS_Edge& OTE = TopoDS::Edge(aLocalShape);
//	const TopoDS_Edge& OTE = TopoDS::Edge(OF.Generated(Cur));
	EdgeTgt.Bind(Cur,OF.Generated(Cur));
	TopoDS_Vertex V1,V2,OV1,OV2;
	TopExp::Vertices (Cur,V1,V2);
	TopExp::Vertices (OTE,OV1,OV2);      
	TopTools_ListOfShape LE;
	if (!EdgeTgt.IsBound(V1)) {
	  myAnalyse.Edges(V1,BRepOffset_Tangent,LE);
	  const TopTools_ListOfShape& LA =myAnalyse.Ancestors(V1);
	  if (LE.Extent() == LA.Extent())
	    EdgeTgt.Bind(V1,OV1);
	}
	if (!EdgeTgt.IsBound(V2)) {
	  LE.Clear();
	  myAnalyse.Edges(V2,BRepOffset_Tangent,LE);
	  const TopTools_ListOfShape& LA =myAnalyse.Ancestors(V2);
	  if (LE.Extent() == LA.Extent())
	      EdgeTgt.Bind(V2,OV2);
	}
      }
    }
    MapSF.Bind(F,OF);
  }
  //--------------------------------------------------------
  // Construction of tubes on edge.
  //--------------------------------------------------------
  BRepOffset_Type    OT = BRepOffset_Convex;
  if (myOffset < 0.) OT = BRepOffset_Concave; 
   
  for (Exp.Init(myShape,TopAbs_EDGE); Exp.More(); Exp.Next()) {
    const TopoDS_Edge& E = TopoDS::Edge(Exp.Current());
    if (Done.Add(E)) {
      const TopTools_ListOfShape& Anc = myAnalyse.Ancestors(E);
      if (Anc.Extent() == 2) {
	const BRepOffset_ListOfInterval& L = myAnalyse.Type(E);
	if (!L.IsEmpty() && L.First().Type() == OT) {
	  Standard_Real CurOffset = myOffset;
	  if ( myFaceOffset.IsBound(Anc.First()))
	    CurOffset = myFaceOffset(Anc.First());
	  TopoDS_Shape aLocalShapeGen = MapSF(Anc.First()).Generated(E);
	  TopoDS_Edge EOn1 = TopoDS::Edge(aLocalShapeGen);
	  aLocalShapeGen = MapSF(Anc.Last()).Generated(E);
	  TopoDS_Edge EOn2 = TopoDS::Edge(aLocalShapeGen);
//	  TopoDS_Edge EOn1 = TopoDS::Edge(MapSF(Anc.First()).Generated(E));
//	  TopoDS_Edge EOn2 = TopoDS::Edge(MapSF(Anc.Last()) .Generated(E));
	  // find if exits tangent edges in the original shape
	  TopoDS_Edge E1f, E1l;
	  TopoDS_Vertex V1f, V1l;
	  TopExp::Vertices(E,V1f,V1l);
	  TopTools_ListOfShape TangE;
	  myAnalyse.TangentEdges(E,V1f,TangE);
	  // find if the pipe on the tangent edges are soon created.
	  TopTools_ListIteratorOfListOfShape itl(TangE);
	  Standard_Boolean Find = Standard_False;
	  for ( ; itl.More() && !Find; itl.Next()) {
	    if ( MapSF.IsBound(itl.Value())) {
	      TopoDS_Shape aLocalShape = MapSF(itl.Value()).Generated(V1f);
	      E1f  = TopoDS::Edge(aLocalShape);
//	      E1f  = TopoDS::Edge(MapSF(itl.Value()).Generated(V1f));
	      Find = Standard_True;
	    }
	  }
	  TangE.Clear();
	  myAnalyse.TangentEdges(E,V1l,TangE);
	  // find if the pipe on the tangent edges are soon created.
	  itl.Initialize(TangE);
	  Find = Standard_False;
	  for ( ; itl.More() && !Find; itl.Next()) {
	    if ( MapSF.IsBound(itl.Value())) {
	      TopoDS_Shape aLocalShape = MapSF(itl.Value()).Generated(V1l);
	      E1l  = TopoDS::Edge(aLocalShape);
//	      E1l  = TopoDS::Edge(MapSF(itl.Value()).Generated(V1l));
	      Find = Standard_True;
	    }
	  }
	  BRepOffset_Offset OF (E,EOn1,EOn2,CurOffset,E1f, E1l);
	  MapSF.Bind(E,OF);
	}
      }
      else {
	// ----------------------
	// free border.
	// ----------------------
	TopoDS_Shape aLocalShape = MapSF(Anc.First()).Generated(E);
	TopoDS_Edge EOn1 = TopoDS::Edge(aLocalShape);
///	TopoDS_Edge EOn1 = TopoDS::Edge(MapSF(Anc.First()).Generated(E));
        myInitOffsetEdge.SetRoot(E); // skv: supporting history.
	myInitOffsetEdge.Bind (E,EOn1);      
      }
    }
  }

  //--------------------------------------------------------
  // Construction of spheres on vertex.
  //--------------------------------------------------------
  Done.Clear();
  TopTools_ListIteratorOfListOfShape it;

  for (Exp.Init(myShape,TopAbs_VERTEX); Exp.More(); Exp.Next()) {
    const TopoDS_Vertex& V = TopoDS::Vertex (Exp.Current());
    if (Done.Add(V)) {
      const TopTools_ListOfShape& LA = myAnalyse.Ancestors(V);
      TopTools_ListOfShape LE;
      myAnalyse.Edges(V,OT,LE);

      if (LE.Extent() >= 3 && LE.Extent() == LA.Extent()) {
	TopTools_ListOfShape LOE;
	//--------------------------------------------------------
	// Return connected edges on tubes.
	//--------------------------------------------------------
	for (it.Initialize(LE) ; it.More(); it.Next()) {
	  LOE.Append(MapSF(it.Value()).Generated(V).Reversed());
	}
	//----------------------
	// construction sphere.
	//-----------------------
	const TopTools_ListOfShape& LLA = myAnalyse.Ancestors(LA.First());
	const TopoDS_Shape& FF = LLA.First();
	Standard_Real CurOffset = myOffset;
	if ( myFaceOffset.IsBound(FF))
	  CurOffset = myFaceOffset(FF);
	
	BRepOffset_Offset OF(V,LOE,CurOffset);
	MapSF.Bind(V,OF);
      }
      //--------------------------------------------------------------
      // Particular processing if V is at least a free border.
      //-------------------------------------------------------------
      TopTools_ListOfShape LBF;
      myAnalyse.Edges(V,BRepOffset_FreeBoundary,LBF);
      if (!LBF.IsEmpty()) {	
	Standard_Boolean First = Standard_True;
	for (it.Initialize(LE) ; it.More(); it.Next()) {
	  if (First) {
	    myInitOffsetEdge.SetRoot(V); // skv: supporting history.
	    myInitOffsetEdge.Bind(V,MapSF(it.Value()).Generated(V));
	    First = Standard_False;
	  }
	  else {
	    myInitOffsetEdge.Add(V,MapSF(it.Value()).Generated(V));
	  }
	} 
      }
    }
  }

  //------------------------------------------------------------
  // Extension of parallel faces to the context.
  // Extended faces are ordered in DS and removed from MapSF.
  //------------------------------------------------------------
  if (!myFaces.IsEmpty()) ToContext (MapSF);

  //------------------------------------------------------
  // MAJ SD.
  //------------------------------------------------------
  BRepOffset_Type    RT = BRepOffset_Concave;
  if (myOffset < 0.) RT = BRepOffset_Convex;
  BRepOffset_DataMapIteratorOfDataMapOfShapeOffset It(MapSF);
  for ( ; It.More(); It.Next()) {
    const TopoDS_Shape& SI = It.Key(); 
    const BRepOffset_Offset& SF = It.Value();
    if (SF.Status() == BRepOffset_Reversed ||
	SF.Status() == BRepOffset_Degenerated ) {
      //------------------------------------------------
      // Degenerated or returned faces are not stored.
      //------------------------------------------------
      continue; 
    }	

    const TopoDS_Face&  OF = It.Value().Face();
    myInitOffsetFace.Bind    (SI,OF);      
    myInitOffsetFace.SetRoot (SI);      // Initial<-> Offset
    myImageOffset.SetRoot    (OF);      // FaceOffset root of images
    
    if (SI.ShapeType() == TopAbs_FACE) {
      for (Exp.Init(SI.Oriented(TopAbs_FORWARD),TopAbs_EDGE); 
	   Exp.More(); Exp.Next()) {
	//--------------------------------------------------------------------
	// To each face are associatedthe edges that restrict that 
	// The edges that do not generate tubes or are not tangent
	// to two faces are removed.
	//--------------------------------------------------------------------
	const TopoDS_Edge& E = TopoDS::Edge(Exp.Current());
	const BRepOffset_ListOfInterval& L  = myAnalyse.Type(E);
	if (!L.IsEmpty() && L.First().Type() != RT) {
	  TopAbs_Orientation OO  = E.Orientation();
	  TopoDS_Shape aLocalShape = It.Value().Generated(E);
	  TopoDS_Edge        OE  = TopoDS::Edge(aLocalShape);
//	  TopoDS_Edge        OE  = TopoDS::Edge(It.Value().Generated(E));
	  myAsDes->Add (OF,OE.Oriented(OO));
	}
      }
    }
    else {
      for (Exp.Init(OF.Oriented(TopAbs_FORWARD),TopAbs_EDGE); 
	   Exp.More(); Exp.Next()) {
	myAsDes->Add (OF,Exp.Current());
      }
    }
  }

#ifdef OCCT_DEBUG
  if ( ChronBuild) Clock.Show();
#endif
}



//=======================================================================
//function : SelfInter
//purpose  : 
//=======================================================================

void BRepOffset_MakeOffset::SelfInter(TopTools_MapOfShape& /*Modif*/)
{
#ifdef OCCT_DEBUG
  if ( ChronBuild) {
    cout << " AUTODEBOUCLAGE:" << endl;
    Clock.Reset();
    Clock.Start();
  }    
#endif  

  Standard_NotImplemented::Raise();

#ifdef OCCT_DEBUG
  if ( ChronBuild) Clock.Show();
#endif
}


//=======================================================================
//function : ToContext
//purpose  : 
//=======================================================================

void BRepOffset_MakeOffset::ToContext (BRepOffset_DataMapOfShapeOffset& MapSF)
{
  TopTools_DataMapOfShapeShape        Created;   
  TopTools_DataMapOfShapeShape        MEF;
  TopTools_IndexedMapOfShape          FacesToBuild;
  TopTools_ListIteratorOfListOfShape  itl;
  TopExp_Explorer                     exp;

//  TopAbs_State       Side = TopAbs_IN;  
//  if (myOffset < 0.) Side = TopAbs_OUT;

  TopAbs_State       Side = TopAbs_OUT; 

  /*
  Standard_Integer i;
  for (i = 1; i <= myFaces.Extent(); i++) {
    const TopoDS_Face& CF = TopoDS::Face(myFaces(i));
    for (exp.Init(CF.Oriented(TopAbs_FORWARD),TopAbs_EDGE); 
	 exp.More(); exp.Next()) {
      const TopoDS_Edge& E = TopoDS::Edge(exp.Current());
      if (!myAnalyse.HasAncestor(E)) {
	//----------------------------------------------------------------
	// The edges of context faces that are not in the initial shape
	// can appear in the result.
	//----------------------------------------------------------------
	//myAsDes->Add(CF,E);
      }  
    }
  }
  */
  
  //--------------------------------------------------------
  // Determine the edges and faces reconstructed by  
  // intersection.
  //---------------------------------------------------------
  Standard_Integer j;
  for (j = 1; j <= myFaces.Extent(); j++) {
    const TopoDS_Face& CF = TopoDS::Face(myFaces(j));
    for (exp.Init(CF.Oriented(TopAbs_FORWARD),TopAbs_EDGE); 
	 exp.More(); exp.Next()) {
      const TopoDS_Edge& E = TopoDS::Edge(exp.Current()); 
      if (myAnalyse.HasAncestor(E)) {
	const TopTools_ListOfShape& LEA = myAnalyse.Ancestors(E);
	for (itl.Initialize(LEA); itl.More(); itl.Next()) {
	  const BRepOffset_Offset& OF = MapSF(itl.Value());
	  FacesToBuild.Add(itl.Value());
	  MEF.Bind(OF.Generated(E),CF);
	}
 	TopoDS_Vertex V[2];
	TopExp::Vertices(E,V[0],V[1]);
	for (Standard_Integer i = 0; i < 2; i++) {
	  const TopTools_ListOfShape& LVA =  myAnalyse.Ancestors(V[i]);
	  for ( itl.Initialize(LVA); itl.More(); itl.Next()) {
	    const TopoDS_Edge& EV = TopoDS::Edge(itl.Value());
	    if (MapSF.IsBound(EV)) {
	      const BRepOffset_Offset& OF = MapSF(EV);
	      FacesToBuild.Add(EV);
	      MEF.Bind(OF.Generated(V[i]),CF);
	    }
	  }
	}
      }
    }
  }
  //---------------------------
  // Reconstruction of faces.
  //---------------------------
  TopoDS_Face        F,NF;
  BRepOffset_Type    RT = BRepOffset_Concave;
  if (myOffset < 0.) RT = BRepOffset_Convex;
  TopoDS_Shape       OE,NE;
  TopAbs_Orientation Or;

  for (j = 1; j <= FacesToBuild.Extent(); j++) {
    const TopoDS_Shape& S   = FacesToBuild(j);
    BRepOffset_Offset   BOF;
    BOF = MapSF(S);
    F = TopoDS::Face(BOF.Face());
    BRepOffset_Tool::ExtentFace(F,Created,MEF,Side,myTol,NF);
    MapSF.UnBind(S);
    //--------------
    // MAJ SD.
    //--------------
    myInitOffsetFace.Bind    (S,NF);      
    myInitOffsetFace.SetRoot (S);      // Initial<-> Offset
    myImageOffset.SetRoot    (NF);

    if (S.ShapeType() == TopAbs_FACE) {
      for (exp.Init(S.Oriented(TopAbs_FORWARD),TopAbs_EDGE); 
	   exp.More(); exp.Next()) {
	
	const TopoDS_Edge& E = TopoDS::Edge(exp.Current());
	const BRepOffset_ListOfInterval& L  = myAnalyse.Type(E);
	OE = BOF.Generated(E);
	Or = E.Orientation();
	OE.Orientation(Or);
	if (!L.IsEmpty() && L.First().Type() != RT) {
	  if (Created.IsBound(OE)) {
	    NE = Created(OE); 
	    if (NE.Orientation() == TopAbs_REVERSED) 
	      NE.Orientation(TopAbs::Reverse(Or));
	    else
	      NE.Orientation(Or);
	    myAsDes->Add(NF,NE);
	  }
	  else {
	    myAsDes->Add(NF,OE);
	  }
	}
      }
    }
    else {
      //------------------
      // Tube
      //---------------------
      for (exp.Init(NF.Oriented(TopAbs_FORWARD),TopAbs_EDGE); 
	   exp.More(); exp.Next()) {
	myAsDes->Add (NF,exp.Current());
      }
    }    
    MapSF.UnBind(S);
  }

  //------------------
  // MAJ free borders
  //------------------
  TopTools_DataMapIteratorOfDataMapOfShapeShape itc;
  for (itc.Initialize(Created); itc.More(); itc.Next()) {
    OE = itc.Key();
    NE = itc.Value();
    if (myInitOffsetEdge.IsImage(OE)) {
      TopoDS_Shape E = myInitOffsetEdge.ImageFrom (OE);
      Or = myInitOffsetEdge.Image(E).First().Orientation();
      if (NE.Orientation() == TopAbs_REVERSED) 
	NE.Orientation(TopAbs::Reverse(Or));
      else
	NE.Orientation(Or);
      myInitOffsetEdge.Remove(OE);
      myInitOffsetEdge.Bind(E,NE);
    }
  }
}


//=======================================================================
//function : UpdateFaceOffset
//purpose  : 
//=======================================================================

void BRepOffset_MakeOffset::UpdateFaceOffset()
{
  TopTools_MapOfShape M;
  TopTools_DataMapOfShapeReal CopiedMap;
  CopiedMap.Assign(myFaceOffset);
  TopTools_DataMapIteratorOfDataMapOfShapeReal it(CopiedMap);

  BRepOffset_Type    RT = BRepOffset_Convex;
  if (myOffset < 0.) RT = BRepOffset_Concave;

  for ( ; it.More(); it.Next()) {
    const TopoDS_Face& F = TopoDS::Face(it.Key());
    Standard_Real CurOffset = CopiedMap(F);
    if ( !M.Add(F)) continue;
    TopoDS_Compound Co;
    BRep_Builder Build;
    Build.MakeCompound(Co);
    TopTools_MapOfShape Dummy;
    Build.Add(Co,F);
    if (myJoin == GeomAbs_Arc)
      myAnalyse.AddFaces(F,Co,Dummy,BRepOffset_Tangent,RT);
    else   
      myAnalyse.AddFaces(F,Co,Dummy,BRepOffset_Tangent);

    TopExp_Explorer exp(Co,TopAbs_FACE);
    for (; exp.More(); exp.Next()) {
      const TopoDS_Face& FF = TopoDS::Face(exp.Current());
      if ( !M.Add(FF)) continue;
      if ( myFaceOffset.IsBound(FF))
	myFaceOffset.UnBind(FF);
      myFaceOffset.Bind(FF,CurOffset);
    }
  }
}

//=======================================================================
//function : CorrectConicalFaces
//purpose  : 
//=======================================================================

void BRepOffset_MakeOffset::CorrectConicalFaces()
{
  if(myOffsetShape.IsNull())
  {
    return;
  }
  //
  TopTools_SequenceOfShape Cones;
  TopTools_SequenceOfShape Circs;
  TopTools_SequenceOfShape Seams;
  Standard_Real TolApex = 1.e-5;

  Standard_Integer i;

  TopTools_DataMapOfShapeListOfShape FacesOfCone;
  //TopTools_DataMapOfShapeShape DegEdges;
  TopExp_Explorer Explo( myOffsetShape, TopAbs_FACE );
  if (myJoin == GeomAbs_Arc)
  {
    for (; Explo.More(); Explo.Next())
    {
      TopoDS_Face aFace = TopoDS::Face( Explo.Current() );
      Handle(Geom_Surface) aSurf = BRep_Tool::Surface( aFace );
      //if (aSurf->DynamicType() == STANDARD_TYPE(Geom_OffsetSurface))
      //aSurf = (Handle(Geom_OffsetSurface)::DownCast(aSurf))->BasisSurface(); //???
      
      TopTools_IndexedMapOfShape Emap;
      TopExp::MapShapes( aFace, TopAbs_EDGE, Emap );
      for (i = 1; i <= Emap.Extent(); i++)
      {
        TopoDS_Edge anEdge = TopoDS::Edge( Emap(i) );
        //Standard_Real f, l;
        //Handle(Geom_Curve) theCurve = BRep_Tool::Curve( anEdge, f, l );
        //Handle(BRep_TEdge)& TE = *((Handle(BRep_TEdge)*) &anEdge.TShape());
        if (BRep_Tool::Degenerated(anEdge))
        {
          //Check if anEdge is a really degenerated edge or not
          BRepAdaptor_Curve BACurve(anEdge, aFace);
          gp_Pnt Pfirst, Plast, Pmid;
          Pfirst = BACurve.Value(BACurve.FirstParameter());
          Plast  = BACurve.Value(BACurve.LastParameter());
          Pmid   = BACurve.Value((BACurve.FirstParameter()+BACurve.LastParameter())/2.);
          if (Pfirst.Distance(Plast) <= TolApex &&
              Pfirst.Distance(Pmid)  <= TolApex)
            continue;
          //Cones.Append( aFace );
          //Circs.Append( anEdge );
          //TopoDS_Vertex Vdeg = TopExp::FirstVertex( anEdge );
          TopoDS_Edge OrEdge = 
            TopoDS::Edge( myInitOffsetEdge.Root( anEdge) );
          TopoDS_Vertex VF = TopExp::FirstVertex( OrEdge );
          if ( FacesOfCone.IsBound(VF) )
          {
            //add a face to the existing list
            TopTools_ListOfShape& aFaces = FacesOfCone.ChangeFind(VF);
            aFaces.Append (aFace);
            //DegEdges.Bind(aFace, anEdge);
          }
          else
          {
            //the vertex is not in the map => create a new key and items
            TopTools_ListOfShape aFaces;
            aFaces.Append (aFace);
            FacesOfCone.Bind(VF, aFaces);
            //DegEdges.Bind(aFace, anEdge);
          }
        }
      } //for (i = 1; i <= Emap.Extent(); i++)
    } //for (; fexp.More(); fexp.Next())
  } //if (myJoin == GeomAbs_Arc)

  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape Cone(FacesOfCone);
  BRep_Builder BB;
  TopLoc_Location L;
  Standard_Boolean IsModified = Standard_False;
  for (; Cone.More(); Cone.Next() ) {
    gp_Sphere theSphere;
    Handle(Geom_SphericalSurface) aSphSurf;
    TopoDS_Wire SphereWire;
    BB.MakeWire(SphereWire);
    TopoDS_Vertex anApex = TopoDS::Vertex(Cone.Key());
    const TopTools_ListOfShape& Faces = Cone.Value(); //FacesOfCone(anApex);
    TopTools_ListIteratorOfListOfShape itFaces(Faces);
    Standard_Boolean isFirstFace = Standard_True;
    gp_Pnt FirstPoint;
    TopoDS_Vertex theFirstVertex, CurFirstVertex;
    for (; itFaces.More(); itFaces.Next())
    {
      TopoDS_Face aFace = TopoDS::Face(itFaces.Value()); //TopoDS::Face(Faces.First());
      TopoDS_Edge DegEdge; // = TopoDS::Edge(DegEdges(aFace));
      for (Explo.Init(aFace, TopAbs_EDGE); Explo.More(); Explo.Next())
      {
        DegEdge = TopoDS::Edge(Explo.Current());
        if (BRep_Tool::Degenerated(DegEdge))
        {
          TopoDS_Edge OrEdge = TopoDS::Edge( myInitOffsetEdge.Root( DegEdge) );
          TopoDS_Vertex VF = TopExp::FirstVertex( OrEdge );
          if (VF.IsSame(anApex))
            break;
        }
      }
      TopoDS_Shape aLocalDegShape = DegEdge.Oriented(TopAbs_FORWARD);
      TopoDS_Edge CurEdge = TopoDS::Edge(aLocalDegShape);
      BB.Degenerated(CurEdge, Standard_False);
      BB.SameRange(CurEdge, Standard_False);
      BB.SameParameter(CurEdge, Standard_False);
      gp_Pnt fPnt, lPnt, mPnt;
      GetEdgePoints(CurEdge, aFace, fPnt, mPnt, lPnt);
      Standard_Real f, l;
      BRep_Tool::Range(CurEdge, f, l);
      if (isFirstFace)
      {
        gp_Vec aVec1(fPnt, mPnt);
        gp_Vec aVec2(fPnt, lPnt);
        gp_Vec aNorm = aVec1.Crossed(aVec2);
        gp_Pnt theApex = BRep_Tool::Pnt(anApex);
        gp_Vec ApexToFpnt(theApex, fPnt);
        gp_Vec Ydir = aNorm ^ ApexToFpnt;
        gp_Vec Xdir = Ydir ^ aNorm;
        //Xdir.Rotate(gp_Ax1(theApex, aNorm), -f);
        gp_Ax2 anAx2(theApex, gp_Dir(aNorm), gp_Dir(Xdir));
        theSphere.SetRadius(myOffset);
        theSphere.SetPosition(gp_Ax3(anAx2) /*gp_Ax3(theApex, gp_Dir(aNorm))*/);
        aSphSurf = new Geom_SphericalSurface(theSphere);
        FirstPoint = fPnt;
        theFirstVertex = BRepLib_MakeVertex(fPnt);
        CurFirstVertex = theFirstVertex;
      }
      
      TopoDS_Vertex v1, v2, FirstVert, EndVert;
      TopExp::Vertices(CurEdge, v1, v2);
      FirstVert = CurFirstVertex;
      if (lPnt.Distance(FirstPoint) <= Precision::Confusion())
        EndVert = theFirstVertex;
      else
        EndVert = BRepLib_MakeVertex(lPnt);
      CurEdge.Free( Standard_True );
      BB.Remove(CurEdge, v1);
      BB.Remove(CurEdge, v2);
      BB.Add(CurEdge, FirstVert.Oriented(TopAbs_FORWARD));
      BB.Add(CurEdge, EndVert.Oriented(TopAbs_REVERSED));
      //take the curve from sphere an put it to the edge
      Standard_Real Uf, Vf, Ul, Vl;
      ElSLib::Parameters( theSphere, fPnt, Uf, Vf );
      ElSLib::Parameters( theSphere, lPnt, Ul, Vl );
      if (Abs(Ul) <= Precision::Confusion())
        Ul = 2.*M_PI;
      Handle(Geom_Curve) aCurv = aSphSurf->VIso(Vf);
      /*
	if (!isFirstFace)
        {
        gp_Circ aCircle = (Handle(Geom_Circle)::DownCast(aCurv))->Circ();
        if (Abs(Uf - f) > Precision::Confusion())
        {
        aCircle.Rotate(aCircle.Axis(), f - Uf);
        aCurv = new Geom_Circle(aCircle);
        }
        }
      */
      Handle(Geom_TrimmedCurve) aTrimCurv = new Geom_TrimmedCurve(aCurv, Uf, Ul);
      BB.UpdateEdge(CurEdge, aTrimCurv, Precision::Confusion());
      BB.Range(CurEdge, Uf, Ul, Standard_True);
      Handle(Geom2d_Line) theLin2d = new Geom2d_Line( gp_Pnt2d( 0., Vf ), gp::DX2d() );
      Handle(Geom2d_TrimmedCurve) theTrimLin2d = new Geom2d_TrimmedCurve(theLin2d, Uf, Ul);
      BB.UpdateEdge(CurEdge, theTrimLin2d, aSphSurf, L, Precision::Confusion());
      BB.Range(CurEdge, aSphSurf, L, Uf, Ul);
      BRepLib::SameParameter(CurEdge);
      BB.Add(SphereWire, CurEdge);
      //Modifying correspondent edges in aFace: substitute vertices common with CurEdge
      BRepAdaptor_Curve2d BAc2d(CurEdge, aFace);
      gp_Pnt2d fPnt2d, lPnt2d;
      fPnt2d = BAc2d.Value(BAc2d.FirstParameter());
      lPnt2d = BAc2d.Value(BAc2d.LastParameter());
      TopTools_IndexedMapOfShape Emap;
      TopExp::MapShapes(aFace, TopAbs_EDGE, Emap);
      TopoDS_Edge EE [2];
      Standard_Integer j = 0, k;
      for (k = 1; k <= Emap.Extent(); k++)
      {
        const TopoDS_Edge& anEdge = TopoDS::Edge(Emap(k));
        if (!BRep_Tool::Degenerated(anEdge))
        {
          TopoDS_Vertex V1, V2;
          TopExp::Vertices(anEdge, V1, V2);
          if (V1.IsSame(v1) || V2.IsSame(v1))
            EE[j++] = anEdge;
        }
      }
      for (k = 0; k < j; k++)
      {
        TopoDS_Shape aLocalShape = EE[k].Oriented(TopAbs_FORWARD);
        TopoDS_Edge Eforward = TopoDS::Edge(aLocalShape);
        Eforward.Free(Standard_True);
        TopoDS_Vertex V1, V2;
        TopExp::Vertices( Eforward, V1, V2 );
        BRepAdaptor_Curve2d EEc( Eforward, aFace );
        gp_Pnt2d p2d1, p2d2;
        p2d1 = EEc.Value(EEc.FirstParameter());
        p2d2 = EEc.Value(EEc.LastParameter());
        if (V1.IsSame(v1))
        {
          TopoDS_Vertex NewV = (p2d1.Distance(fPnt2d) <= Precision::Confusion())?
            FirstVert : EndVert;
          BB.Remove( Eforward, V1 );
          BB.Add( Eforward, NewV.Oriented(TopAbs_FORWARD) );
        }
        else
        {
          TopoDS_Vertex NewV = (p2d2.Distance(fPnt2d) <= Precision::Confusion())?
            FirstVert : EndVert;
          BB.Remove( Eforward, V2 );
          BB.Add( Eforward, NewV.Oriented(TopAbs_REVERSED) );
        }
      }
      
      isFirstFace = Standard_False;
      CurFirstVertex = EndVert;
    }
    //Building new spherical face
    Standard_Real Ufirst = RealLast(), Ulast = RealFirst();
    gp_Pnt2d p2d1, p2d2;
    TopTools_ListOfShape EdgesOfWire;
    TopoDS_Iterator itw(SphereWire);
    for (; itw.More(); itw.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(itw.Value());
      EdgesOfWire.Append(anEdge);
      Standard_Real f, l;
      Handle(Geom2d_Curve) aC2d = BRep_Tool::CurveOnSurface(anEdge, aSphSurf, L, f, l);
      p2d1 = aC2d->Value(f);
      p2d2 = aC2d->Value(l);
      if (p2d1.X() < Ufirst)
        Ufirst = p2d1.X();
      if (p2d1.X() > Ulast)
        Ulast = p2d1.X();
      if (p2d2.X() < Ufirst)
        Ufirst = p2d2.X();
      if (p2d2.X() > Ulast)
        Ulast = p2d2.X();
    }
    TopTools_ListOfShape NewEdges;
    TopoDS_Edge FirstEdge;
    TopTools_ListIteratorOfListOfShape itl(EdgesOfWire);
    for (; itl.More(); itl.Next())
    {
      FirstEdge = TopoDS::Edge(itl.Value());
      Standard_Real f, l;
      Handle(Geom2d_Curve) aC2d = BRep_Tool::CurveOnSurface(FirstEdge, aSphSurf, L, f, l);
      p2d1 = aC2d->Value(f);
      p2d2 = aC2d->Value(l);
      if (Abs(p2d1.X() - Ufirst) <= Precision::Confusion())
      {
        EdgesOfWire.Remove(itl);
        break;
      }
    }
    NewEdges.Append(FirstEdge);
    TopoDS_Vertex Vf1, CurVertex;
    TopExp::Vertices(FirstEdge, Vf1, CurVertex);
    itl.Initialize(EdgesOfWire);
    while (itl.More())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(itl.Value());
      TopoDS_Vertex V1, V2;
      TopExp::Vertices(anEdge, V1, V2);
      if (V1.IsSame(CurVertex) || V2.IsSame(CurVertex))
      {
        NewEdges.Append(anEdge);
        CurVertex = (V1.IsSame(CurVertex))? V2 : V1;
        EdgesOfWire.Remove(itl);
      }
      else
        itl.Next();
    }
    
    Standard_Real Vfirst, Vlast;
    if (p2d1.Y() > 0.)
    {
      Vfirst = p2d1.Y(); Vlast = M_PI/2.;
    }
    else
    {
      Vfirst = -M_PI/2.; Vlast = p2d1.Y();
    }
    TopoDS_Face NewSphericalFace = BRepLib_MakeFace(aSphSurf, Ufirst, Ulast, Vfirst, Vlast, Precision::Confusion());
    TopoDS_Edge OldEdge;
    for (Explo.Init(NewSphericalFace, TopAbs_EDGE); Explo.More(); Explo.Next())
    {
      OldEdge = TopoDS::Edge(Explo.Current());
      if (!BRep_Tool::Degenerated(OldEdge))
      {
        BRepAdaptor_Curve2d BAc2d(OldEdge, NewSphericalFace);
        p2d1 = BAc2d.Value(BAc2d.FirstParameter());
        p2d2 = BAc2d.Value(BAc2d.LastParameter());
        if (Abs(p2d1.X() - Ufirst) <= Precision::Confusion() &&
            Abs(p2d2.X() - Ulast)  <= Precision::Confusion())
          break;
      }
    }
    TopoDS_Vertex V1, V2;
    TopExp::Vertices(OldEdge, V1, V2);
    TopTools_ListOfShape LV1, LV2;
    LV1.Append(Vf1);
    LV2.Append(CurVertex);
    BRepTools_Substitution theSubstitutor;
    theSubstitutor.Substitute(V1, LV1);
    if (!V1.IsSame(V2))
      theSubstitutor.Substitute(V2, LV2);
    theSubstitutor.Substitute(OldEdge, NewEdges);
    theSubstitutor.Build(NewSphericalFace);
    if (theSubstitutor.IsCopied(NewSphericalFace))
    {
      const TopTools_ListOfShape& listSh = theSubstitutor.Copy(NewSphericalFace);
      NewSphericalFace = TopoDS::Face(listSh.First());
    }
    
    //Adding NewSphericalFace to the shell
    Explo.Init( myOffsetShape, TopAbs_SHELL );
    TopoDS_Shape theShell = Explo.Current();
    theShell.Free( Standard_True );
    BB.Add( theShell, NewSphericalFace );
    IsModified = Standard_True;
    if(!theShell.Closed())
    {
      if(BRep_Tool::IsClosed(theShell))
      {
        theShell.Closed(Standard_True);
      }
    }
  }
  //
  if(!IsModified)
  {
    return;
  }
  //
  if (myShape.ShapeType() == TopAbs_SOLID || myThickening)
  {
    //Explo.Init( myOffsetShape, TopAbs_SHELL );

    //if (Explo.More()) {
    //  TopoDS_Shape theShell = Explo.Current();
    //  theShell.Closed( Standard_True );
    //}

    Standard_Integer            NbShell = 0;
    TopoDS_Compound             NC;
    TopoDS_Shape                S1;
    BB.MakeCompound (NC);

    TopoDS_Solid Sol;
    BB.MakeSolid(Sol);
    Sol.Closed(Standard_True);
    for (Explo.Init(myOffsetShape,TopAbs_SHELL); Explo.More(); Explo.Next()) {
      TopoDS_Shell Sh = TopoDS::Shell(Explo.Current());
      //if (myThickening && myOffset > 0.)
      //  Sh.Reverse();
      NbShell++;
      if (Sh.Closed()) {
        BB.Add(Sol,Sh);
      }
      else {
        BB.Add (NC,Sh);
        if(NbShell == 1)
        {
          S1 = Sh;
        }
      }
    }
    TopoDS_Iterator anIt(Sol);
    Standard_Boolean SolIsNull = !anIt.More();
    //Checking solid
    if(!SolIsNull)
    {
      Standard_Integer nbs = 0;
      while(anIt.More()) {anIt.Next(); ++nbs;}
      if(nbs > 1)
      {
        BRepCheck_Analyzer aCheck(Sol, Standard_False);
        if(!aCheck.IsValid())
        {
          TopTools_ListOfShape aSolList;
          CorrectSolid(Sol, aSolList);
          if(!aSolList.IsEmpty())
          {
            BB.Add(NC, Sol);
            TopTools_ListIteratorOfListOfShape aSLIt(aSolList);
            for(; aSLIt.More(); aSLIt.Next())
            {
              BB.Add(NC, aSLIt.Value());
            }
            SolIsNull = Standard_True;
          }
        }
      }
    }
    //
    anIt.Initialize(NC);
    Standard_Boolean NCIsNull = !anIt.More();
    if((!SolIsNull) && (!NCIsNull))
    {
      BB.Add(NC, Sol);
      myOffsetShape = NC;
    }
    else if(SolIsNull && (!NCIsNull))
    {
      if (NbShell == 1) 
      {
        myOffsetShape = S1;
      }
      else
      {
        myOffsetShape = NC;
      }
    }
    else if((!SolIsNull) && NCIsNull)
    {
      myOffsetShape = Sol;
    }
    else
    {
      myOffsetShape = NC;
    }
  }
}


//=======================================================================
//function : Intersection3D
//purpose  : 
//=======================================================================

void BRepOffset_MakeOffset::Intersection3D(BRepOffset_Inter3d& Inter)
{
#ifdef OCCT_DEBUG
  if (ChronBuild) {
    cout << " INTERSECTION 3D:" << endl;
    Clock.Reset();
    Clock.Start();  
  }
#endif
  TopTools_ListOfShape OffsetFaces;  // list of faces // created.
  MakeList (OffsetFaces,myInitOffsetFace,myFaces);

  if (!myFaces.IsEmpty()) {     
    Standard_Boolean InSide = (myOffset < 0.); // Temporary
    // it is necessary to calculate Inside taking account of the concavity or convexity of edges
    // between the cap and the part.

    if (myJoin == GeomAbs_Arc) 
      Inter.ContextIntByArc (myFaces,InSide,myAnalyse,myInitOffsetFace,myInitOffsetEdge);
  }
  if (myInter) {
    //-------------
    //Complete.
    //-------------
    Inter.CompletInt (OffsetFaces,myInitOffsetFace);
    TopTools_IndexedMapOfShape& NewEdges = Inter.NewEdges();
    if (myJoin == GeomAbs_Intersection) {
      BRepOffset_Tool::CorrectOrientation (myShape,NewEdges,myAsDes,myInitOffsetFace,myOffset);
    }
  }
  else {
    //--------------------------------
    // Only between neighbor faces.
    //--------------------------------
    Inter.ConnexIntByArc(OffsetFaces,myShape,myAnalyse,myInitOffsetFace);
  }
#ifdef OCCT_DEBUG
  if ( ChronBuild) Clock.Show();
#endif
}

//=======================================================================
//function : Intersection2D
//purpose  : 
//=======================================================================

void BRepOffset_MakeOffset::Intersection2D(const TopTools_IndexedMapOfShape& Modif,
					   const TopTools_IndexedMapOfShape& NewEdges)
{
#ifdef OCCT_DEBUG
  if (ChronBuild) {
    cout << " INTERSECTION 2D:" << endl;
    Clock.Reset();
    Clock.Start();  
  }
#endif
  //--------------------------------------------------------
  // calculate intersections2d on faces concerned by 
  // intersection3d
  //---------------------------------------------------------
  //TopTools_MapIteratorOfMapOfShape it(Modif);
  //-----------------------------------------------
  // Intersection of edges 2 by 2.
  //-----------------------------------------------
  Standard_Integer i;
  for (i = 1; i <= Modif.Extent(); i++) {
    const TopoDS_Face& F  = TopoDS::Face(Modif(i));
    BRepOffset_Inter2d::Compute(myAsDes,F,NewEdges,myTol);
  }

#ifdef OCCT_DEBUG
  if (AffichInt2d) {
    DEBVerticesControl (NewEdges,myAsDes);
  }
  if ( ChronBuild) Clock.Show();
#endif
}


//=======================================================================
//function : MakeLoops
//purpose  : 
//=======================================================================

void BRepOffset_MakeOffset::MakeLoops(TopTools_IndexedMapOfShape& Modif)
{
#ifdef OCCT_DEBUG
  if (ChronBuild) {
     cout << " DEBOUCLAGE 2D:" << endl;
     Clock.Reset();
     Clock.Start(); 
  }
#endif
  //TopTools_MapIteratorOfMapOfShape    it(Modif);
  TopTools_ListOfShape                LF,LC;
  //-----------------------------------------
  // unwinding of faces // modified.
  //-----------------------------------------
  Standard_Integer i;
  for (i = 1; i <= Modif.Extent(); i++) { 
    if (!myFaces.Contains(Modif(i)))
      LF.Append(Modif(i));
  }
  myMakeLoops.Build(LF,myAsDes,myImageOffset);

  //-----------------------------------------
  // unwinding of caps.
  //-----------------------------------------
  for (i = 1; i <= myFaces.Extent(); i++)
    LC.Append(myFaces(i));

  Standard_Boolean   InSide = 1;
  if (myOffset > 0 ) InSide = 0;
  myMakeLoops.BuildOnContext(LC,myAnalyse,myAsDes,myImageOffset,InSide);

#ifdef OCCT_DEBUG
  if ( ChronBuild) Clock.Show();
#endif
}

//=======================================================================
//function : MakeFaces
//purpose  : Reconstruction of topologically unchanged faces that
//           share edges that were reconstructed.
//=======================================================================

void BRepOffset_MakeOffset::MakeFaces(TopTools_IndexedMapOfShape& /*Modif*/)
{
#ifdef OCCT_DEBUG
  if (ChronBuild) {  
    cout << " RECONSTRUCTION OF FACES:" << endl;
    Clock.Reset();
    Clock.Start();
  }
#endif
  TopTools_ListIteratorOfListOfShape itr;
  const TopTools_ListOfShape& Roots = myInitOffsetFace.Roots();
  TopTools_ListOfShape        LOF;
  //----------------------------------
  // Loop on all faces //.
  //----------------------------------
  for (itr.Initialize(Roots); itr.More(); itr.Next()) {
    TopoDS_Face F = TopoDS::Face(myInitOffsetFace.Image(itr.Value()).First());
    LOF.Append(F);
  }
  myMakeLoops.BuildFaces(LOF,myAsDes,myImageOffset);
  
#ifdef OCCT_DEBUG
  if ( ChronBuild) Clock.Show();
#endif
}

//=======================================================================
//function : UpdateInitOffset
//purpose  : Update and cleaning of myInitOffset 
//=======================================================================

static void UpdateInitOffset (BRepAlgo_Image&         myInitOffset,
			      BRepAlgo_Image&         myImageOffset,
			      const TopoDS_Shape&     myOffsetShape,
			      const TopAbs_ShapeEnum &theShapeType) // skv
{
  BRepAlgo_Image NIOF;
  const TopTools_ListOfShape& Roots = myInitOffset.Roots();
  TopTools_ListIteratorOfListOfShape it(Roots);
  for (; it.More(); it.Next()) {
    NIOF.SetRoot (it.Value());    
  }
  for (it.Initialize(Roots); it.More(); it.Next()) {
    const TopoDS_Shape& SI = it.Value();
    TopTools_ListOfShape LI;
    TopTools_ListOfShape L1;
    myInitOffset.LastImage(SI,L1);
    TopTools_ListIteratorOfListOfShape itL1(L1);
    for (; itL1.More(); itL1.Next()) {
      const TopoDS_Shape& O1 = itL1.Value();
      TopTools_ListOfShape L2;
      myImageOffset.LastImage(O1,L2);
      LI.Append(L2);
    }
    NIOF.Bind(SI,LI);
  }
//  Modified by skv - Mon Apr  4 18:17:27 2005 Begin
//  Supporting history.
//   NIOF.Filter(myOffsetShape,TopAbs_FACE);
  NIOF.Filter(myOffsetShape, theShapeType);
//  Modified by skv - Mon Apr  4 18:17:27 2005 End
  myInitOffset = NIOF;
}

//=======================================================================
//function : MakeMissingWalls
//purpose  : 
//=======================================================================
void BRepOffset_MakeOffset::MakeMissingWalls ()
{
  TopTools_DataMapOfShapeListOfShape Contours; //Start vertex + list of connected edges (free boundary)
  TopTools_DataMapOfShapeShape MapEF; //Edges of contours: edge + face
  Standard_Real OffsetVal = Abs(myOffset);

  FillContours(myShape, myAnalyse, Contours, MapEF);

  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape iter(Contours);
  for (; iter.More(); iter.Next())
  {
    TopoDS_Vertex StartVertex = TopoDS::Vertex(iter.Key());
    TopoDS_Edge StartEdge;
    const TopTools_ListOfShape& aContour = iter.Value();
    TopTools_ListIteratorOfListOfShape itl(aContour);
    Standard_Boolean FirstStep = Standard_True;
    TopoDS_Edge PrevEdge;
    TopoDS_Vertex PrevVertex = StartVertex;
    Standard_Boolean isBuildFromScratch = Standard_False; // Problems with edges.
    for (; itl.More(); itl.Next())
    {
      TopoDS_Edge anEdge = TopoDS::Edge(itl.Value());

      // Check for offset existence.
      if (!myInitOffsetEdge.HasImage(anEdge))
        continue;

      // Check for existence of two different vertices.
      TopTools_ListOfShape LOE, LOE2;
      myInitOffsetEdge.LastImage( anEdge, LOE );
      myImageOffset.LastImage( LOE.Last(), LOE2 );
      TopoDS_Edge OE = TopoDS::Edge( LOE2.Last() );
      TopoDS_Vertex V1, V2, V3, V4;
      TopExp::Vertices(OE,     V4, V3);
      TopExp::Vertices(anEdge, V1, V2);
      Standard_Real aF, aL;
      const Handle(Geom_Curve) aC = BRep_Tool::Curve(anEdge, aF, aL);
      if (!aC.IsNull() &&
         (!aC->IsClosed() && !aC->IsPeriodic()))
      {
        gp_Pnt aPntF = BRep_Tool::Pnt(V1);
        gp_Pnt aPntL = BRep_Tool::Pnt(V2);
        Standard_Real aDistE = aPntF.SquareDistance(aPntL);
        if ( aDistE < Precision::SquareConfusion())
        {
          // Bad case: non closed, but vertexes mapped to same 3d point.
          continue;
        }

        Standard_Real anEdgeTol = BRep_Tool::Tolerance(anEdge);
        if (aDistE < anEdgeTol)
        {
          // Potential problems not detected via checkshape.
          gp_Pnt aPntOF = BRep_Tool::Pnt(V4);
          gp_Pnt aPntOL = BRep_Tool::Pnt(V3);
          if (aPntOF.SquareDistance(aPntOL) > gp::Resolution())
          {
            // To avoid computation of complex analytical continuation of Sin / ArcSin.
            Standard_Real aSinValue = Min(2 * anEdgeTol / aPntOF.Distance(aPntOL), 1.0);
            Standard_Real aMaxAngle = Min(Abs(ASin(aSinValue)), M_PI_4); // Maximal angle.
            Standard_Real aCurrentAngle =  gp_Vec(aPntF, aPntL).Angle(gp_Vec(aPntOF, aPntOL));
            if (aC->IsKind(STANDARD_TYPE(Geom_Line)) &&
                Abs (aCurrentAngle) > aMaxAngle)
            {
              // anEdge not collinear to offset edge.
              isBuildFromScratch = Standard_True;
              myIsPerformSewing = Standard_True;
              continue;
            }
          }
        }
      }

      Standard_Boolean ToReverse = Standard_False;
      if (!V1.IsSame(PrevVertex))
      {
        TopoDS_Vertex aVtx = V1; V1 = V2; V2 = aVtx;
        aVtx = V3; V3 = V4; V4 = aVtx;
        ToReverse = Standard_True;
      }

      OE.Orientation(TopAbs::Reverse(anEdge.Orientation()));
      TopoDS_Edge E3, E4;
      Standard_Boolean ArcOnV2 = ((myJoin == GeomAbs_Arc) && (myInitOffsetEdge.HasImage(V2)));
      if (FirstStep || isBuildFromScratch)
      {
        E4 = BRepLib_MakeEdge( V1, V4 );
        if (FirstStep)
          StartEdge = E4;
      }
      else
        E4 = PrevEdge;
      if (V2.IsSame(StartVertex) && !ArcOnV2)
        E3 = StartEdge;
      else
        E3 = BRepLib_MakeEdge( V2, V3 );
      E4.Reverse();

      if (isBuildFromScratch)
      {
        E3.Reverse();
        E4.Reverse();
      }

      TopoDS_Shape localAnEdge = anEdge.Oriented(TopAbs_FORWARD);
      const TopoDS_Edge& anEdgeFWD = TopoDS::Edge(localAnEdge);
      Standard_Real ParV1 = BRep_Tool::Parameter(V1, anEdgeFWD);
      Standard_Real ParV2 = BRep_Tool::Parameter(V2, anEdgeFWD);
      BRep_Builder BB;
      TopoDS_Wire theWire;
      BB.MakeWire(theWire);
      if (ToReverse)
      {
        BB.Add(theWire, anEdge.Reversed());
        BB.Add(theWire, E3.Reversed());
        BB.Add(theWire, OE.Reversed());
        BB.Add(theWire, E4.Reversed());
      }
      else
      {
        BB.Add(theWire, anEdge);
        BB.Add(theWire, E3);
        BB.Add(theWire, OE);
        BB.Add(theWire, E4);
      }

      BRepLib::BuildCurves3d( theWire, myTol );
      theWire.Closed(Standard_True);
      TopoDS_Face NewFace;
      Handle(Geom_Surface) theSurf;
      BRepAdaptor_Curve BAcurve(anEdge);
      BRepAdaptor_Curve BAcurveOE(OE);
      Standard_Real fpar = BAcurve.FirstParameter();
      Standard_Real lpar = BAcurve.LastParameter();
      gp_Pnt PonE  = BAcurve.Value(fpar);
      gp_Pnt PonOE = BAcurveOE.Value(fpar);
      gp_Dir OffsetDir = gce_MakeDir( PonE, PonOE );
      Handle(Geom2d_Line) EdgeLine2d, OELine2d, aLine2d, aLine2d2;
      Standard_Boolean IsPlanar = Standard_False;
      if (BAcurve.GetType() == GeomAbs_Circle &&
        BAcurveOE.GetType() == GeomAbs_Circle)
      {
        gp_Circ aCirc = BAcurve.Circle();
        gp_Circ aCircOE = BAcurveOE.Circle();
        gp_Lin anAxisLine(aCirc.Axis());
        gp_Dir CircAxisDir = aCirc.Axis().Direction();
        if (aCirc.Axis().IsParallel(aCircOE.Axis(), Precision::Confusion()) &&
          anAxisLine.Contains(aCircOE.Location(), Precision::Confusion()))
        { //cylinder, plane or cone
          if (Abs(aCirc.Radius() - aCircOE.Radius()) <= Precision::Confusion()) //case of cylinder
            theSurf = GC_MakeCylindricalSurface(aCirc).Value();
          else if (aCirc.Location().Distance(aCircOE.Location()) <= Precision::Confusion()) {//case of plane
            IsPlanar = Standard_True;
            //
            gp_Pnt PonEL = BAcurve.Value(lpar);
            if (PonEL.Distance(PonE) <= Precision::PConfusion()) {
              Standard_Boolean bIsHole;
              TopoDS_Edge aE1, aE2;
              TopoDS_Wire aW1, aW2;
              Handle(Geom_Plane) aPL;
              IntTools_FClass2d aClsf;
              //
              if (aCirc.Radius()>aCircOE.Radius()) {
                aE1 = anEdge;
                aE2 = OE;
              } else {
                aE1 = OE;
                aE2 = anEdge;
              }
              //
              BB.MakeWire(aW1);
              BB.Add(aW1, aE1);
              BB.MakeWire(aW2);
              BB.Add(aW2, aE2);
              //
              aPL = new Geom_Plane(aCirc.Location(), CircAxisDir);
              for (Standard_Integer i = 0; i < 2; ++i) {
                TopoDS_Wire& aW = (i==0) ? aW1 : aW2;
                TopoDS_Edge& aE = (i==0) ? aE1 : aE2;
                //
                TopoDS_Face aFace;
                BB.MakeFace(aFace, aPL, Precision::Confusion());
                BB.Add (aFace, aW);
                aClsf.Init(aFace, Precision::Confusion());
                bIsHole=aClsf.IsHole();
                if ((bIsHole && !i) || (!bIsHole && i)) {
                  aW.Nullify();
                  BB.MakeWire(aW);
                  BB.Add(aW, aE.Reversed());
                }
              }
              //
              BB.MakeFace(NewFace, aPL, Precision::Confusion());
              BB.Add(NewFace, aW1);
              BB.Add(NewFace, aW2);
            }
          }
          else //case of cone
          {
            gp_Cone theCone = gce_MakeCone(aCirc.Location(), aCircOE.Location(),
              aCirc.Radius(), aCircOE.Radius());
            gp_Ax3 theAx3(aCirc.Position());
            if (CircAxisDir * theCone.Axis().Direction() < 0.)
            {
              theAx3.ZReverse();
              CircAxisDir.Reverse();
            }
            theCone.SetPosition(theAx3);
            theSurf = new Geom_ConicalSurface(theCone);
          }
          if (!IsPlanar) {
            TopLoc_Location Loc;
            EdgeLine2d = new Geom2d_Line(gp_Pnt2d(0., 0.), gp_Dir2d(1., 0.));
            BB.UpdateEdge(anEdge, EdgeLine2d, theSurf, Loc, Precision::Confusion());
            Standard_Real Coeff = (OffsetDir * CircAxisDir > 0.)? 1. : -1.;
            OELine2d = new Geom2d_Line(gp_Pnt2d(0., OffsetVal*Coeff), gp_Dir2d(1., 0.));
            BB.UpdateEdge(OE, OELine2d, theSurf, Loc, Precision::Confusion());
            aLine2d  = new Geom2d_Line(gp_Pnt2d(ParV2, 0.), gp_Dir2d(0., Coeff));
            aLine2d2 = new Geom2d_Line(gp_Pnt2d(ParV1, 0.), gp_Dir2d(0., Coeff));
            if (E3.IsSame(E4))
            {
              if (Coeff > 0.)
                BB.UpdateEdge(E3, aLine2d, aLine2d2, theSurf, Loc, Precision::Confusion());
              else
              {
                BB.UpdateEdge(E3, aLine2d2, aLine2d, theSurf, Loc, Precision::Confusion());
                theWire.Nullify();
                BB.MakeWire(theWire);
                BB.Add(theWire, anEdge.Oriented(TopAbs_REVERSED));
                BB.Add(theWire, E4);
                BB.Add(theWire, OE.Oriented(TopAbs_FORWARD));
                BB.Add(theWire, E3);
                theWire.Closed(Standard_True);
              }
            }
            else
            {
              BB.SameParameter(E3, Standard_False);
              BB.SameRange(E3, Standard_False);
              BB.SameParameter(E4, Standard_False);
              BB.SameRange(E4, Standard_False);
              BB.UpdateEdge(E3, aLine2d,  theSurf, Loc, Precision::Confusion());
              BB.Range(E3, theSurf, Loc, 0., OffsetVal);
              BB.UpdateEdge(E4, aLine2d2, theSurf, Loc, Precision::Confusion());
              BB.Range(E4, theSurf, Loc, 0., OffsetVal);
            }
            NewFace = BRepLib_MakeFace(theSurf, theWire);
          }
        } //cylinder or cone
      } //if both edges are arcs of circles
      if (NewFace.IsNull())
      {
        BRepLib_MakeFace MF(theWire, Standard_True); //Only plane
        if (MF.Error() == BRepLib_FaceDone)
        {
          NewFace = MF.Face();
          IsPlanar = Standard_True;
        }
        else //Extrusion (by thrusections)
        {
          Handle(Geom_Curve) EdgeCurve = BRep_Tool::Curve(anEdge, fpar, lpar);
          Handle(Geom_TrimmedCurve) TrEdgeCurve =
            new Geom_TrimmedCurve( EdgeCurve, fpar, lpar );
          Standard_Real fparOE, lparOE;
          Handle(Geom_Curve) OffsetCurve = BRep_Tool::Curve(OE, fparOE, lparOE);
          Handle(Geom_TrimmedCurve) TrOffsetCurve =
            new Geom_TrimmedCurve( OffsetCurve, fparOE, lparOE );
          GeomFill_Generator ThrusecGenerator;
          ThrusecGenerator.AddCurve( TrEdgeCurve );
          ThrusecGenerator.AddCurve( TrOffsetCurve );
          ThrusecGenerator.Perform( Precision::PConfusion() );
          theSurf = ThrusecGenerator.Surface();
          //theSurf = new Geom_SurfaceOfLinearExtrusion( TrOffsetCurve, OffsetDir );
          Standard_Real Uf, Ul, Vf, Vl;
          theSurf->Bounds(Uf, Ul, Vf, Vl);
          TopLoc_Location Loc;
          EdgeLine2d = new Geom2d_Line(gp_Pnt2d(0., Vf), gp_Dir2d(1., 0.));
          BB.UpdateEdge(anEdge, EdgeLine2d, theSurf, Loc, Precision::Confusion());
          OELine2d = new Geom2d_Line(gp_Pnt2d(0., Vl), gp_Dir2d(1., 0.));
          BB.UpdateEdge(OE, OELine2d, theSurf, Loc, Precision::Confusion());
          Standard_Real UonV1 = (ToReverse)? Ul : Uf;
          Standard_Real UonV2 = (ToReverse)? Uf : Ul;
          aLine2d  = new Geom2d_Line(gp_Pnt2d(UonV2, 0.), gp_Dir2d(0., 1.));
          aLine2d2 = new Geom2d_Line(gp_Pnt2d(UonV1, 0.), gp_Dir2d(0., 1.));
          if (E3.IsSame(E4))
          {
            BB.UpdateEdge(E3, aLine2d, aLine2d2, theSurf, Loc, Precision::Confusion());
            Handle(Geom_Curve) BSplC34 = theSurf->UIso( Uf );
            BB.UpdateEdge(E3, BSplC34, Precision::Confusion());
            BB.Range(E3, Vf, Vl);
          }
          else
          {
            BB.SameParameter(E3, Standard_False);
            BB.SameRange(E3, Standard_False);
            BB.SameParameter(E4, Standard_False);
            BB.SameRange(E4, Standard_False);
            BB.UpdateEdge(E3, aLine2d,  theSurf, Loc, Precision::Confusion());
            BB.Range(E3, theSurf, Loc, Vf, Vl);
            BB.UpdateEdge(E4, aLine2d2, theSurf, Loc, Precision::Confusion());
            BB.Range(E4, theSurf, Loc, Vf, Vl);
            Handle(Geom_Curve) BSplC3 = theSurf->UIso( UonV2 );
            BB.UpdateEdge(E3, BSplC3, Precision::Confusion());
            BB.Range(E3, Vf, Vl, Standard_True); //only for 3d curve
            Handle(Geom_Curve) BSplC4 = theSurf->UIso( UonV1 );
            BB.UpdateEdge(E4, BSplC4, Precision::Confusion());
            BB.Range(E4, Vf, Vl, Standard_True); //only for 3d curve
          }
          NewFace = BRepLib_MakeFace(theSurf, theWire);
        }
      }
      if (!IsPlanar)
      {
        Standard_Real fparOE = BAcurveOE.FirstParameter();
        Standard_Real lparOE = BAcurveOE.LastParameter();
        TopLoc_Location Loc;
        if (Abs(fpar - fparOE) > Precision::Confusion())
        {
          const TopoDS_Edge& anE4 = (ToReverse)? E3 : E4;
          gp_Pnt2d fp2d   = EdgeLine2d->Value(fpar);
          gp_Pnt2d fp2dOE = OELine2d->Value(fparOE);
          aLine2d2 = GCE2d_MakeLine( fp2d, fp2dOE ).Value();
          Handle(Geom_Curve) aCurve;
          Standard_Real FirstPar = 0., LastPar = fp2d.Distance(fp2dOE);
          Geom2dAdaptor_Curve AC2d( aLine2d2, FirstPar, LastPar );
          GeomAdaptor_Surface GAsurf( theSurf );
          Handle(Geom2dAdaptor_HCurve) HC2d  = new Geom2dAdaptor_HCurve( AC2d );
          Handle(GeomAdaptor_HSurface) HSurf = new GeomAdaptor_HSurface( GAsurf );
          Adaptor3d_CurveOnSurface ConS( HC2d, HSurf );
          Standard_Real max_deviation = 0., average_deviation;
          GeomLib::BuildCurve3d(Precision::Confusion(),
            ConS, FirstPar, LastPar,
            aCurve, max_deviation, average_deviation);
          BB.UpdateEdge( anE4, aCurve, max_deviation );
          BB.UpdateEdge( anE4, aLine2d2, theSurf, Loc, max_deviation );
          BB.Range( anE4, FirstPar, LastPar );
        }
        if (Abs(lpar - lparOE) > Precision::Confusion())
        {
          const TopoDS_Edge& anE3 = (ToReverse)? E4 : E3;
          gp_Pnt2d lp2d   = EdgeLine2d->Value(lpar);
          gp_Pnt2d lp2dOE = OELine2d->Value(lparOE);
          aLine2d = GCE2d_MakeLine( lp2d, lp2dOE ).Value();
          Handle(Geom_Curve) aCurve;
          Standard_Real FirstPar = 0., LastPar = lp2d.Distance(lp2dOE);
          Geom2dAdaptor_Curve AC2d( aLine2d, FirstPar, LastPar );
          GeomAdaptor_Surface GAsurf( theSurf );
          Handle(Geom2dAdaptor_HCurve) HC2d  = new Geom2dAdaptor_HCurve( AC2d );
          Handle(GeomAdaptor_HSurface) HSurf = new GeomAdaptor_HSurface( GAsurf );
          Adaptor3d_CurveOnSurface ConS( HC2d, HSurf );
          Standard_Real max_deviation = 0., average_deviation;
          GeomLib::BuildCurve3d(Precision::Confusion(),
            ConS, FirstPar, LastPar,
            aCurve, max_deviation, average_deviation);
          BB.UpdateEdge( anE3, aCurve, max_deviation );
          BB.UpdateEdge( anE3, aLine2d, theSurf, Loc, max_deviation );
          BB.Range( anE3, FirstPar, LastPar );
        }
      }
      BRepLib::SameParameter(NewFace);
      BRepTools::Update(NewFace);
      myWalls.Append(NewFace);
      if (ArcOnV2)
      {
        TopoDS_Edge anArc = TopoDS::Edge(myInitOffsetEdge.Image(V2).First());
        TopoDS_Vertex arcV1, arcV2;
        TopExp::Vertices(anArc, arcV1, arcV2);
        Standard_Boolean ArcReverse = Standard_False;
        if (!arcV1.IsSame(V3))
        {
          TopoDS_Vertex aVtx = arcV1; arcV1 = arcV2; arcV2 = aVtx;
          ArcReverse = Standard_True;
        }
        TopoDS_Edge EA1, EA2;
        //EA1 = (ToReverse)? E3 : TopoDS::Edge(E3.Reversed());
        EA1 = E3;
        EA1.Reverse();
        if (ToReverse)
          EA1.Reverse();
        //////////////////////////////////////////////////////
        if (V2.IsSame(StartVertex))
          EA2 = StartEdge;
        else
          EA2 = BRepLib_MakeEdge( V2, arcV2 );
        anArc.Orientation( ((ArcReverse)? TopAbs_REVERSED : TopAbs_FORWARD) );
        if (EA1.Orientation() == TopAbs_REVERSED)
          anArc.Reverse();
        EA2.Orientation(TopAbs::Reverse(EA1.Orientation()));
        TopoDS_Wire arcWire;
        BB.MakeWire(arcWire);
        BB.Add(arcWire, EA1);
        BB.Add(arcWire, anArc);
        BB.Add(arcWire, EA2);
        BRepLib::BuildCurves3d( arcWire, myTol );
        arcWire.Closed(Standard_True);
        TopoDS_Face arcFace = BRepLib_MakeFace(arcWire, Standard_True);
        BRepTools::Update(arcFace);
        myWalls.Append(arcFace);
        TopoDS_Shape localEA2 = EA2.Oriented(TopAbs_FORWARD);
        const TopoDS_Edge& CEA2 = TopoDS::Edge(localEA2);
        PrevEdge = CEA2;
        PrevVertex = V2;
      }
      else
      {
        if (isBuildFromScratch)
        {
          PrevEdge = TopoDS::Edge(E4);
          PrevVertex = V1;
          isBuildFromScratch = Standard_False;
        }
        else
        {
          PrevEdge = E3;
          PrevVertex = V2;
        }
      }
      FirstStep = Standard_False;
    }
  }
}

//=======================================================================
//function : MakeShells
//purpose  : 
//=======================================================================

void BRepOffset_MakeOffset::MakeShells ()
{
#ifdef OCCT_DEBUG
  if (ChronBuild) {  
    cout << " RECONSTRUCTION OF SHELLS:" << endl;
    Clock.Reset();
    Clock.Start();
  }
#endif
  BRepTools_Quilt Glue;
  const TopTools_ListOfShape& R = myImageOffset.Roots();
  TopTools_ListIteratorOfListOfShape it(R);

  for ( ; it.More(); it.Next()) {
    TopTools_ListOfShape Image;
    myImageOffset.LastImage(it.Value(),Image);
    TopTools_ListIteratorOfListOfShape it2(Image);
    for ( ; it2.More(); it2.Next()) {
      Glue.Add(it2.Value());
    }
  }

  if (myThickening)
  {
    TopExp_Explorer Explo(myShape, TopAbs_FACE);
    for (; Explo.More(); Explo.Next())
	    Glue.Add(Explo.Current());
      
    for (it.Initialize(myWalls); it.More(); it.Next())
	    Glue.Add(it.Value());
  }

  myOffsetShape = Glue.Shells();
  //
  //Set correct value for closed flag
  TopExp_Explorer Explo(myOffsetShape, TopAbs_SHELL);
  for(; Explo.More(); Explo.Next())
  {
    TopoDS_Shape aS = Explo.Current(); 
    if(!aS.Closed())
    {
      if(BRep_Tool::IsClosed(aS))
      {
        aS.Closed(Standard_True);
      }
    }
  }               
}

//=======================================================================
//function : MakeSolid
//purpose  : 
//=======================================================================

void BRepOffset_MakeOffset::MakeSolid ()
{
 if (myOffsetShape.IsNull()) return;

//  Modified by skv - Mon Apr  4 18:17:27 2005 Begin
//  Supporting history.
  UpdateInitOffset (myInitOffsetFace,myImageOffset,myOffsetShape, TopAbs_FACE);
  UpdateInitOffset (myInitOffsetEdge,myImageOffset,myOffsetShape, TopAbs_EDGE);
//  Modified by skv - Mon Apr  4 18:17:27 2005 End
  TopExp_Explorer             exp;
  BRep_Builder                B;
  Standard_Integer            NbShell = 0;
  TopoDS_Compound             NC;
  TopoDS_Shape                S1;
  B.MakeCompound (NC);

  TopoDS_Solid Sol;
  B.MakeSolid(Sol);
  Sol.Closed(Standard_True);
  Standard_Boolean aMakeSolid = (myShape.ShapeType() == TopAbs_SOLID) || myThickening;
  for (exp.Init(myOffsetShape,TopAbs_SHELL); exp.More(); exp.Next()) {
    TopoDS_Shell Sh = TopoDS::Shell(exp.Current());
    if (myThickening && myOffset > 0.)
      Sh.Reverse();
    NbShell++;
    if (Sh.Closed() && aMakeSolid) {
      B.Add(Sol,Sh);
    }
    else {
      B.Add (NC,Sh);
      if(NbShell == 1)
      {
        S1 = Sh;
      }
    }
  }
  TopoDS_Iterator anIt(Sol);
  Standard_Boolean SolIsNull = !anIt.More();
  //Checking solid
  if(!SolIsNull)
  {
    Standard_Integer nbs = 0;
    while(anIt.More()) {anIt.Next(); ++nbs;}
    if(nbs > 1)
    {
      BRepCheck_Analyzer aCheck(Sol, Standard_False);
      if(!aCheck.IsValid())
      {
        TopTools_ListOfShape aSolList;
        CorrectSolid(Sol, aSolList);
        if(!aSolList.IsEmpty())
        {
          B.Add(NC, Sol);
          TopTools_ListIteratorOfListOfShape aSLIt(aSolList);
          for(; aSLIt.More(); aSLIt.Next())
          {
            B.Add(NC, aSLIt.Value());
          }
          SolIsNull = Standard_True;
        }
      }
    }
  }
  anIt.Initialize(NC);
  Standard_Boolean NCIsNull = !anIt.More();
  if((!SolIsNull) && (!NCIsNull))
  {
    B.Add(NC, Sol);
    myOffsetShape = NC;
  }
  else if(SolIsNull && (!NCIsNull))
  {
    if (NbShell == 1) 
    {
      myOffsetShape = S1;
    }
    else
    {
      myOffsetShape = NC;
    }
  }
  else if((!SolIsNull) && NCIsNull)
  {
    myOffsetShape = Sol;
  }
  else
  {
    myOffsetShape = NC;
  }
}

//=======================================================================
//function : SelectShells
//purpose  : 
//=======================================================================

void BRepOffset_MakeOffset::SelectShells ()
{  
  TopTools_MapOfShape FreeEdges;
  TopExp_Explorer exp(myShape,TopAbs_EDGE);
  //-------------------------------------------------------------
  // FreeEdges all edges that can have free border in the  
  // parallel shell
  // 1 - free borders of myShape .
  //-------------------------------------------------------------
  for ( ; exp.More(); exp.Next()) {
    const TopoDS_Edge& E = TopoDS::Edge(exp.Current());
    const TopTools_ListOfShape& LA = myAnalyse.Ancestors(E);
    if (LA.Extent() < 2) {
      if (myAnalyse.Type(E).First().Type() == BRepOffset_FreeBoundary) {
	      FreeEdges.Add(E);                       
      }
    }  
  }
  // myShape has free borders and there are no caps
  // no unwinding 3d.
  if (!FreeEdges.IsEmpty() && myFaces.IsEmpty()) return;

  myOffsetShape = BRepOffset_Tool::Deboucle3D(myOffsetShape,FreeEdges);
}

//=======================================================================
//function : OffsetFacesFromShapes
//purpose  : 
//=======================================================================

const BRepAlgo_Image& BRepOffset_MakeOffset::OffsetFacesFromShapes() const
{
  return myInitOffsetFace;
}

//  Modified by skv - Tue Mar 15 16:20:43 2005 Begin

//=======================================================================
//function : GetJoinType
//purpose  : Query offset join type.
//=======================================================================

GeomAbs_JoinType BRepOffset_MakeOffset::GetJoinType() const
{
  return myJoin;
}

//=======================================================================
//function : OffsetEdgesFromShapes
//purpose  : 
//=======================================================================

const BRepAlgo_Image& BRepOffset_MakeOffset::OffsetEdgesFromShapes() const
{
  return myInitOffsetEdge;
}

//  Modified by skv - Tue Mar 15 16:20:43 2005 End

//=======================================================================
//function : ClosingFaces
//purpose  : 
//=======================================================================

const TopTools_IndexedMapOfShape& BRepOffset_MakeOffset::ClosingFaces () const
{
  return myFaces;
}



//=======================================================================
//function : EncodeRegularity
//purpose  : 
//=======================================================================

void BRepOffset_MakeOffset::EncodeRegularity ()
{
#ifdef OCCT_DEBUG
  if (ChronBuild) {  
    cout << " CODING OF REGULARITIES:" << endl;
    Clock.Reset();
    Clock.Start();
  }
#endif

  if (myOffsetShape.IsNull()) return;
  // find edges G1 in the result
  TopExp_Explorer exp(myOffsetShape,TopAbs_EDGE);

  BRep_Builder B;
  TopTools_MapOfShape MS;

  for ( ; exp.More(); exp.Next()) {
    TopoDS_Edge OE  = TopoDS::Edge(exp.Current());
    BRepLib::BuildCurve3d(OE,myTol);
    TopoDS_Edge ROE = OE;
    
    if ( !MS.Add(OE)) continue;
      
    if ( myImageOffset.IsImage(OE)) 
      ROE = TopoDS::Edge(myImageOffset.Root(OE));

    const TopTools_ListOfShape& LofOF    = myAsDes->Ascendant(ROE);
    
    if (LofOF.Extent() != 2) {
#ifdef OCCT_DEBUG_VERB
    cout << " Edge shared by " << LofOF.Extent() << " Faces" << endl;
#endif
      continue;
    }

    const TopoDS_Face& F1 = TopoDS::Face(LofOF.First());
    const TopoDS_Face& F2 = TopoDS::Face(LofOF.Last() );
    
    if ( F1.IsNull() || F2.IsNull()) 
      continue;
   
    const TopoDS_Shape& Root1 = myInitOffsetFace.Root(F1);
    const TopoDS_Shape& Root2 = myInitOffsetFace.Root(F2);

    TopAbs_ShapeEnum Type1 = Root1.ShapeType();
    TopAbs_ShapeEnum Type2 = Root2.ShapeType();
 
    if (F1.IsSame(F2)) {      
      if (BRep_Tool::IsClosed(OE,F1)) {
	// Temporary Debug for the Bench.
	// Check with YFR.
	// In mode intersection, the edges are not coded in myInitOffsetEdge
	// so, manage case by case
	// Note DUB; for Hidden parts, it is NECESSARY to code CN 
	// Analytic Surfaces.
	if (myJoin == GeomAbs_Intersection) {
	  BRepAdaptor_Surface BS(F1,Standard_False);
	  GeomAbs_SurfaceType SType = BS.GetType();
	  if (SType == GeomAbs_Cylinder ||
	      SType == GeomAbs_Cone     ||
	      SType == GeomAbs_Sphere   ||
	      SType == GeomAbs_Torus      ) {
	    B.Continuity(OE,F1,F1,GeomAbs_CN);
	  }
	  else {
	    // See YFR : MaJ of myInitOffsetFace
	  }
	}
	else if (myInitOffsetEdge.IsImage(ROE)) {
	  if ( Type1 == TopAbs_FACE && Type2 == TopAbs_FACE) {
	    const TopoDS_Face& FRoot = TopoDS::Face(Root1);
	    const TopoDS_Edge& EI = TopoDS::Edge(myInitOffsetEdge.ImageFrom(ROE));
	    GeomAbs_Shape Conti = BRep_Tool::Continuity(EI,FRoot,FRoot);
	    if (Conti == GeomAbs_CN) {
	      B.Continuity(OE,F1,F1,GeomAbs_CN);
	    }
	    else if ( Conti > GeomAbs_C0) {
	      B.Continuity(OE,F1,F1,GeomAbs_G1);
	    }
	  }
	}
      }
      continue;
    }


    //  code regularities G1 between :
    //    - sphere and tube : one root is a vertex, the other is an edge 
    //                        and the vertex is included in the edge
    //    - face and tube   : one root is a face, the other an edge 
    //                        and the edge is included in the face
    //    - face and face    : if two root faces are tangent in 
    //                        the initial shape, they will be tangent in the offset shape
    //    - tube and tube  : if 2 edges generating tubes are
    //                        tangents, the 2 will be tangent either.
    if ( Type1 == TopAbs_EDGE && Type2 == TopAbs_VERTEX) {
      TopoDS_Vertex V1,V2;
      TopExp::Vertices(TopoDS::Edge(Root1), V1, V2);
      if ( V1.IsSame(Root2) || V2.IsSame(Root2)) {
	B.Continuity(OE,F1,F2,GeomAbs_G1);
      }
    }
    else if ( Type1 == TopAbs_VERTEX && Type2 == TopAbs_EDGE) {
      TopoDS_Vertex V1,V2;
      TopExp::Vertices(TopoDS::Edge(Root2), V1, V2);
      if ( V1.IsSame(Root1) || V2.IsSame(Root1)) {
	B.Continuity(OE,F1,F2,GeomAbs_G1);
      }
    }
    else if ( Type1 == TopAbs_FACE && Type2 == TopAbs_EDGE) {
      TopExp_Explorer exp2(Root1,TopAbs_EDGE);
      for ( ; exp2.More(); exp2.Next()) {
	if ( exp2.Current().IsSame(Root2)) {
	  B.Continuity(OE,F1,F2,GeomAbs_G1);
	  break;
	}
      }
    }
    else if ( Type1 == TopAbs_EDGE && Type2 == TopAbs_FACE) {
      TopExp_Explorer exp2(Root2,TopAbs_EDGE);
      for ( ; exp2.More(); exp2.Next()) {
	if ( exp2.Current().IsSame(Root1)) {
	  B.Continuity(OE,F1,F2,GeomAbs_G1);
	  break;
	}
      }
    }
    else if ( Type1 == TopAbs_FACE && Type2 == TopAbs_FACE) {
    //  if two root faces are tangent in 
    //  the initial shape, they will be tangent in the offset shape
      TopTools_ListOfShape LE,LV;
      BRepOffset_Tool::HasCommonShapes(TopoDS::Face(Root1),
				       TopoDS::Face(Root2),
				       LE,LV);
      if ( LE.Extent() == 1) { 
	const TopoDS_Edge& Ed = TopoDS::Edge(LE.First());
	if ( myAnalyse.HasAncestor(Ed)) {
	  const BRepOffset_ListOfInterval& LI = myAnalyse.Type(Ed);
	  if (LI.Extent()       == 1   && 
	      LI.First().Type() == BRepOffset_Tangent) {
	    B.Continuity(OE,F1,F2,GeomAbs_G1);
	  }
	}
      }
    }
    else if ( Type1 == TopAbs_EDGE && Type2 == TopAbs_EDGE) {
      TopTools_ListOfShape LV;
      TopExp_Explorer exp1;
      for (exp1.Init(Root1,TopAbs_VERTEX); exp1.More(); exp1.Next()) {
	TopExp_Explorer exp2(F2,TopAbs_EDGE);
	for (exp2.Init(Root2,TopAbs_VERTEX); exp2.More(); exp2.Next()) {
	  if (exp1.Current().IsSame(exp2.Current())) {
	    LV.Append(exp1.Current());
	  }
	}
      }
      if ( LV.Extent() == 1) {
	TopTools_ListOfShape LEdTg;
	myAnalyse.TangentEdges(TopoDS::Edge(Root1),
			       TopoDS::Vertex(LV.First()),
			       LEdTg);
	TopTools_ListIteratorOfListOfShape it(LEdTg);
	for (; it.More(); it.Next()) {
	  if ( it.Value().IsSame(Root2)) {
	    B.Continuity(OE,F1,F2,GeomAbs_G1);
	    break;
	  }
	}
      }
    }
  }

#ifdef OCCT_DEBUG
  if ( ChronBuild) Clock.Show();
#endif
}



//=======================================================================
//function : UpDateTolerance
//purpose  : 
//=======================================================================

void UpdateTolerance (TopoDS_Shape& S,
			                const TopTools_IndexedMapOfShape& Faces)
{
  BRep_Builder B;
  TopTools_MapOfShape View;
  TopoDS_Vertex V[2];

  // The edges of caps are not modified.
  Standard_Integer j;
  for (j = 1; j <= Faces.Extent(); j++) {
    const TopoDS_Shape& F = Faces(j);
    TopExp_Explorer Exp;
    for (Exp.Init(F,TopAbs_EDGE); Exp.More(); Exp.Next()) {
      View.Add(Exp.Current());
    }
  }
  
  TopExp_Explorer Exp;
  for (Exp.Init(S,TopAbs_EDGE); Exp.More(); Exp.Next()) {
    TopoDS_Edge E = TopoDS::Edge(Exp.Current());
    if (View.Add(E)) {
      Handle(BRepCheck_Edge) EdgeCorrector = new BRepCheck_Edge(E);
      Standard_Real    Tol = EdgeCorrector->Tolerance();
      B.UpdateEdge (E,Tol);
      
      // Update the vertices.
      TopExp::Vertices(E,V[0],V[1]);
     
      for (Standard_Integer i = 0 ; i <=1 ; i++) {
	if (View.Add(V[i])) {
	  Handle(BRep_TVertex) TV = Handle(BRep_TVertex)::DownCast(V[i].TShape());
	  TV->Tolerance(0.);
	  Handle(BRepCheck_Vertex) VertexCorrector = new BRepCheck_Vertex(V[i]);
	  B.UpdateVertex (V[i],VertexCorrector->Tolerance());
	  // use the occasion to clean the vertices.
	  (TV->ChangePoints()).Clear();
	}
	B.UpdateVertex(V[i],Tol);
      }
    }
  }
}

//=======================================================================
//function : CorrectSolid
//purpose  : 
//=======================================================================
void CorrectSolid(TopoDS_Solid& theSol, TopTools_ListOfShape& theSolList)
{
  BRep_Builder aBB;
  TopoDS_Shape anOuterShell;
  NCollection_List<Standard_Real> aVols;
  Standard_Real aVolMax = 0., anOuterVol = 0.;

  TopoDS_Iterator anIt(theSol);
  for(; anIt.More(); anIt.Next())
  {
    const TopoDS_Shape& aSh = anIt.Value();
    GProp_GProps aVProps;
    BRepGProp::VolumeProperties(aSh, aVProps, Standard_True);
    if(Abs(aVProps.Mass()) > aVolMax)
    {
      anOuterVol = aVProps.Mass();
      aVolMax = Abs(anOuterVol);
      anOuterShell = aSh; 
    }
    aVols.Append(aVProps.Mass());
  }
  //
  if(anOuterVol < 0.)
  {
    anOuterShell.Reverse();
  }
  TopoDS_Solid aNewSol;
  aBB.MakeSolid(aNewSol);
  aNewSol.Closed(Standard_True);
  aBB.Add(aNewSol, anOuterShell);
  BRepClass3d_SolidClassifier aSolClass(aNewSol);
  //
  anIt.Initialize(theSol);
  NCollection_List<Standard_Real>::Iterator aVIt(aVols);
  for(; anIt.More(); anIt.Next(), aVIt.Next())
  {
    TopoDS_Shell aSh = TopoDS::Shell(anIt.Value());
    if(aSh.IsSame(anOuterShell))
    {
      continue;
    }
    else
    {
      TopExp_Explorer aVExp(aSh, TopAbs_VERTEX);
      const TopoDS_Vertex& aV = TopoDS::Vertex(aVExp.Current());
      gp_Pnt aP = BRep_Tool::Pnt(aV);
      aSolClass.Perform(aP, BRep_Tool::Tolerance(aV));
      if(aSolClass.State() == TopAbs_IN)
      {
        if(aVIt.Value() > 0.)
        {
          aSh.Reverse();
        }
        aBB.Add(aNewSol, aSh);
      }
      else
      {
        if(aVIt.Value() < 0.)
        {
          aSh.Reverse();
        }
        TopoDS_Solid aSol;
        aBB.MakeSolid(aSol);
        aSol.Closed(Standard_True);
        aBB.Add(aSol, aSh);
        theSolList.Append(aSol);
      }
    }
  }
  theSol = aNewSol;
}

//=======================================================================
//function : CheckInputData
//purpose  : Check input data for possiblity of offset perform.
//=======================================================================
Standard_Boolean BRepOffset_MakeOffset::CheckInputData()
{
  // Set initial error state.
  myError = BRepOffset_NoError;
  TopoDS_Shape aTmpShape;
  myBadShape = aTmpShape;

  // Non-null offset.
  if (Abs(myOffset) <= myTol)
  {
    Standard_Boolean isFound = Standard_False;
    TopTools_DataMapIteratorOfDataMapOfShapeReal anIter(myFaceOffset);
    for( ; anIter.More(); anIter.Next())
    {
      if (Abs(anIter.Value()) > myTol)
      {
        isFound = Standard_True;
        break;
      }
    }

    if (!isFound)
    {
      // No face with non-null offset found.
      myError = BRepOffset_NullOffset;
      return Standard_False;
    }
  }

  // Connectivity of input shape.
  if (!IsConnectedShell(myShape))
  {
    myError = BRepOffset_NotConnectedShell;
    return Standard_False;
  }

  // Normals check and continuity check.
  const Standard_Integer aPntPerDim = 20; // 21 points on each dimension.
  Standard_Real aUmin, aUmax, aVmin, aVmax;
  TopExp_Explorer anExpSF(myShape, TopAbs_FACE);
  NCollection_Map<Handle(TopoDS_TShape)> aPresenceMap;
  TopLoc_Location L;
  gp_Pnt2d aPnt2d;
  for( ; anExpSF.More(); anExpSF.Next())
  {
    const TopoDS_Face& aF = TopoDS::Face(anExpSF.Current());

    if (aPresenceMap.Contains(aF.TShape()))
    {
      // Not perform computations with partner shapes,
      // since they are contain same geometry.
      continue;
    }
    aPresenceMap.Add(aF.TShape());

    const Handle(Geom_Surface)& aSurf = BRep_Tool::Surface(aF, L);
    BRepTools::UVBounds(aF, aUmin, aUmax, aVmin, aVmax);

    // Continuity check.
    if (aSurf->Continuity() == GeomAbs_C0)
    {
      myError = BRepOffset_C0Geometry;
      return Standard_False;
    }

    // Get degenerated points, to avoid check them.
    NCollection_Vector<gp_Pnt> aBad3dPnts;
    TopExp_Explorer anExpFE(aF, TopAbs_EDGE);
    for( ; anExpFE.More(); anExpFE.Next())
    {
      const TopoDS_Edge &aE = TopoDS::Edge(anExpFE.Current());
      if (BRep_Tool::Degenerated(aE))
      {
        aBad3dPnts.Append(BRep_Tool::Pnt((TopExp::FirstVertex(aE))));
      }
    }

    // Geometry grid check.
    for(Standard_Integer i = 0; i <= aPntPerDim; i++)
    {
      Standard_Real aUParam = aUmin + (aUmax - aUmin) * i / aPntPerDim;
      for(Standard_Integer j = 0; j <= aPntPerDim; j++)
      {
        Standard_Real aVParam = aVmin + (aVmax - aVmin) * j / aPntPerDim;

        myError = checkSinglePoint(aUParam, aVParam, aSurf, aBad3dPnts);
        if (myError != BRepOffset_NoError)
          return Standard_False;
      }
    }

    // Vertex list check.
    TopExp_Explorer anExpFV(aF, TopAbs_VERTEX);
    for( ; anExpFV.More(); anExpFV.Next())
    {
      const TopoDS_Vertex &aV = TopoDS::Vertex(anExpFV.Current());
      aPnt2d = BRep_Tool::Parameters(aV, aF);

      myError = checkSinglePoint(aPnt2d.X(), aPnt2d.Y(), aSurf, aBad3dPnts);
      if (myError != BRepOffset_NoError)
        return Standard_False;
    }
  }

  return Standard_True;
}


//=======================================================================
//function : GetBadShape
//purpose  : Get shape where problems detected.
//=======================================================================
const TopoDS_Shape& BRepOffset_MakeOffset::GetBadShape() const
{
  return myBadShape;
}


//=======================================================================
//function : checkSinglePoint
//purpose  : Check single point on surface for bad normals
//=======================================================================
BRepOffset_Error checkSinglePoint(const Standard_Real theUParam,
                                  const Standard_Real theVParam,
                                  const Handle(Geom_Surface)& theSurf,
                                  const NCollection_Vector<gp_Pnt>& theBadPoints)
{
  gp_Pnt aPnt;
  gp_Vec aD1U, aD1V;
  theSurf->D1(theUParam, theVParam, aPnt, aD1U, aD1V);

  if (aD1U.SquareMagnitude() < Precision::SquareConfusion() ||
      aD1V.SquareMagnitude() < Precision::SquareConfusion() )
  {
    Standard_Boolean isKnownBadPnt = Standard_False;
    for(Standard_Integer anIdx  = theBadPoints.Lower();
                         anIdx <= theBadPoints.Upper();
                       ++anIdx)
    {
      if (aPnt.SquareDistance(theBadPoints(anIdx)) < Precision::SquareConfusion())
      {
        isKnownBadPnt = Standard_True;
        break;
      }
    } // for(Standard_Integer anIdx  = theBadPoints.Lower();

    if (!isKnownBadPnt)
    {
      return BRepOffset_BadNormalsOnGeometry;
    }
    else
    {
      return BRepOffset_NoError;
    }
  } //  if (aD1U.SquareMagnitude() < Precision::SquareConfusion() ||

  if (aD1U.IsParallel(aD1V, Precision::Confusion()))
  {
    // Isolines are collinear.
    return BRepOffset_BadNormalsOnGeometry;
  }

  return BRepOffset_NoError;
}
