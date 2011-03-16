// File:	MgtBRep_TranslateTool1.cxx
//      	--------------------------
// Author:	DAUTRY Philippe
//		<fid@fox.paris1.matra-dtv.fr>
// Copyright:	Matra Datavision 1998

// Version:	0.0
// History:	Version	Date		Purpose
//		0.0	Feb  3 1998	Creation
// sln 25.09.2001 BUC60998: correction for edge which has null 3d curve representation


#include <MgtBRep_TranslateTool1.ixx>

#include <MgtTopLoc.hxx>
#include <MgtGeom.hxx>
#include <MgtGeom2d.hxx>
#include <MgtPoly.hxx>

#include <TopoDS.hxx>

#include <BRep_Builder.hxx>
#include <BRep_TVertex.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_TFace.hxx>

#include <BRep_Curve3D.hxx>
#include <BRep_CurveOnSurface.hxx>
#include <BRep_CurveOnClosedSurface.hxx>
#include <BRep_CurveOn2Surfaces.hxx>
#include <BRep_Polygon3D.hxx>
#include <BRep_PolygonOnSurface.hxx>
#include <BRep_PolygonOnClosedSurface.hxx>
#include <BRep_PolygonOnTriangulation.hxx>
#include <BRep_PolygonOnClosedTriangulation.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>

#include <BRep_PointOnCurve.hxx>
#include <BRep_PointOnCurveOnSurface.hxx>
#include <BRep_PointOnSurface.hxx>
#include <BRep_ListIteratorOfListOfPointRepresentation.hxx>

#include <PTopoDS_TWire1.hxx>
#include <PTopoDS_TShell1.hxx>
#include <PTopoDS_TSolid1.hxx>
#include <PTopoDS_TCompSolid1.hxx>
#include <PTopoDS_TCompound1.hxx>

#include <PBRep_TVertex1.hxx>
#include <PBRep_TEdge1.hxx>
#include <PBRep_TFace1.hxx>

#include <PBRep_Curve3D.hxx>
#include <PBRep_CurveOnSurface.hxx>
#include <PBRep_CurveOnClosedSurface.hxx>
#include <PBRep_CurveOn2Surfaces.hxx>

#include <PBRep_PointOnCurve.hxx>
#include <PBRep_PointOnCurveOnSurface.hxx>
#include <PBRep_PointOnSurface.hxx>
#include <PBRep_Polygon3D.hxx>
#include <PBRep_PolygonOnSurface.hxx>
#include <PBRep_PolygonOnClosedSurface.hxx>
#include <PBRep_PolygonOnTriangulation.hxx>
#include <PBRep_PolygonOnClosedTriangulation.hxx>

#include <Standard_NullObject.hxx>


#ifdef chrono
#include <OSD_Timer.hxx>
extern OSD_Timer CurveTimer;
extern OSD_Timer Curve2dTimer;
extern OSD_Timer SurfaceTimer;
#define TIMER_START(THETIMER) THETIMER.Start()
#define TIMER_STOP(THETIMER) THETIMER.Stop()
#else
#define TIMER_START(THETIMER)
#define TIMER_STOP(THETIMER)
#endif

// *****************************
// Auxiliary translation methods
// *****************************

//=======================================================================
//function : Translate
//purpose  : Curve T->P
//=======================================================================

Handle(PGeom_Curve) MgtBRep_TranslateTool1::Translate
(const Handle(Geom_Curve)& TP,
 PTColStd_TransientPersistentMap& aMap) const 
{
  Handle(PGeom_Curve) PP;
  if (!TP.IsNull()) {
    if (aMap.IsBound(TP)) {
      PP = *((Handle(PGeom_Curve)*) &(aMap.Find(TP)));
    }
    else {
      PP = MgtGeom::Translate(TP);
      aMap.Bind(TP,PP);
    }
  }
  return PP;
}

//=======================================================================
//function : Translate
//purpose  : Curve P->T
//=======================================================================

Handle(Geom_Curve) MgtBRep_TranslateTool1::Translate
(const Handle(PGeom_Curve)& PP,
 PTColStd_PersistentTransientMap& aMap) const  
{
  TIMER_START(CurveTimer);
  Handle(Geom_Curve) TP;
  if (!PP.IsNull()) {
    if (aMap.IsBound(PP)) {
      TP = *((Handle(Geom_Curve)*) &(aMap.Find(PP)));
    }
    else {
      TP = MgtGeom::Translate(PP);
      aMap.Bind(PP, TP);
    }
  }
  TIMER_STOP(CurveTimer);
  return TP;
}

//=======================================================================
//function : Translate
//purpose  : Curve2d T->P
//=======================================================================

Handle(PGeom2d_Curve) MgtBRep_TranslateTool1::Translate
(const Handle(Geom2d_Curve)& TP,
 PTColStd_TransientPersistentMap& aMap) const 
{
  Handle(PGeom2d_Curve) PP;
  if (!TP.IsNull()) {
    if (aMap.IsBound(TP)) {
      PP = *((Handle(PGeom2d_Curve)*) &(aMap.Find(TP)));
    }
    else {
      PP = MgtGeom2d::Translate(TP);
      aMap.Bind(TP,PP);
    }
  }
  return PP;
}

//=======================================================================
//function : Translate
//purpose  : Curve2d P->T
//=======================================================================

Handle(Geom2d_Curve) MgtBRep_TranslateTool1::Translate
(const Handle(PGeom2d_Curve)& PP,
 PTColStd_PersistentTransientMap& aMap) const 
{
  TIMER_START(Curve2dTimer);
  Handle(Geom2d_Curve) TP;
  if (!PP.IsNull()) {
    if (aMap.IsBound(PP)) {
      TP = *((Handle(Geom2d_Curve)*) &(aMap.Find(PP)));
    }
    else {
      TP = MgtGeom2d::Translate(PP);
      aMap.Bind(PP, TP);
    }
  }
  TIMER_STOP(Curve2dTimer);
  return TP;
}

//=======================================================================
//function : Translate
//purpose  : Surface T->P
//=======================================================================

Handle(PGeom_Surface) MgtBRep_TranslateTool1::Translate
(const Handle(Geom_Surface)& TP,
 PTColStd_TransientPersistentMap& aMap) const 
{
  Handle(PGeom_Surface) PP;
  if (!TP.IsNull()) {
    if (aMap.IsBound(TP)) {
      PP = *((Handle(PGeom_Surface)*) &(aMap.Find(TP)));
    }
    else {
      PP = MgtGeom::Translate(TP);
      aMap.Bind(TP,PP);
    }
  }
  return PP;
}

//=======================================================================
//function : Translate
//purpose  : Surface P->T
//=======================================================================

Handle(Geom_Surface) MgtBRep_TranslateTool1::Translate
(const Handle(PGeom_Surface)& PP,
 PTColStd_PersistentTransientMap& aMap) const 
{
  TIMER_START(SurfaceTimer);
  Handle(Geom_Surface) TP;
  if (!PP.IsNull()) {
    if (aMap.IsBound(PP)) {
      TP = *((Handle(Geom_Surface)*) &(aMap.Find(PP)));
    }
    else {
      TP = MgtGeom::Translate(PP);
      aMap.Bind(PP,TP);
    }
  }
  TIMER_STOP(SurfaceTimer);
  return TP;
}

//=======================================================================
//function : MgtBRep_TranslateTool1
//purpose  : Constructor 
//=======================================================================

MgtBRep_TranslateTool1::MgtBRep_TranslateTool1
(const MgtBRep_TriangleMode aTriMode) : 
myTriangleMode(aTriMode)
{
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void MgtBRep_TranslateTool1::Add(TopoDS_Shape& S1,
				const TopoDS_Shape& S2) const 
{
  BRep_Builder B;
  B.Add(S1,S2);
}

//=======================================================================
//function : MakeVertex
//purpose  : 
//=======================================================================

void MgtBRep_TranslateTool1::MakeVertex(TopoDS_Shape& S) const 
{
  BRep_Builder B;
  B.MakeVertex(TopoDS::Vertex(S));
}


//=======================================================================
//function : MakeVertex
//purpose  : 
//=======================================================================

void MgtBRep_TranslateTool1::MakeVertex(PTopoDS_Shape1& S) const 
{
  Handle(PBRep_TVertex1) T = new PBRep_TVertex1();
  S.TShape(T);
}


//=======================================================================
//function : MakeEdge
//purpose  : 
//=======================================================================

void MgtBRep_TranslateTool1::MakeEdge(TopoDS_Shape& S) const 
{
  BRep_Builder B;
  B.MakeEdge(TopoDS::Edge(S));
}


//=======================================================================
//function : MakeEdge
//purpose  : 
//=======================================================================

void MgtBRep_TranslateTool1::MakeEdge(PTopoDS_Shape1& S) const 
{
  Handle(PBRep_TEdge1) T = new PBRep_TEdge1();
  S.TShape(T);
}


//=======================================================================
//function : MakeWire
//purpose  : 
//=======================================================================

void MgtBRep_TranslateTool1::MakeWire(TopoDS_Shape& S) const 
{
  BRep_Builder B;
  B.MakeWire(TopoDS::Wire(S));
}


//=======================================================================
//function : MakeWire
//purpose  : 
//=======================================================================

void MgtBRep_TranslateTool1::MakeWire(PTopoDS_Shape1& S) const 
{
  Handle(PTopoDS_TWire1) T = new PTopoDS_TWire1();
  S.TShape(T);
}


//=======================================================================
//function : MakeFace
//purpose  : 
//=======================================================================

void MgtBRep_TranslateTool1::MakeFace(TopoDS_Shape& S) const 
{
  BRep_Builder B;
  B.MakeFace(TopoDS::Face(S));
}


//=======================================================================
//function : MakeFace
//purpose  : 
//=======================================================================

void MgtBRep_TranslateTool1::MakeFace(PTopoDS_Shape1& S) const 
{
  Handle(PBRep_TFace1) T = new PBRep_TFace1();
  S.TShape(T);
}


//=======================================================================
//function : MakeShell
//purpose  : 
//=======================================================================

void MgtBRep_TranslateTool1::MakeShell(TopoDS_Shape& S) const 
{
  BRep_Builder B;
  B.MakeShell(TopoDS::Shell(S));
}


//=======================================================================
//function : MakeShell
//purpose  : 
//=======================================================================

void MgtBRep_TranslateTool1::MakeShell(PTopoDS_Shape1& S) const 
{
  Handle(PTopoDS_TShell1) T = new PTopoDS_TShell1();
  S.TShape(T);
}


//=======================================================================
//function : MakeSolid
//purpose  : 
//=======================================================================

void  MgtBRep_TranslateTool1::MakeSolid(TopoDS_Shape& S) const 
{
  BRep_Builder B;
  B.MakeSolid(TopoDS::Solid(S));
}


//=======================================================================
//function : MakeSolid
//purpose  : 
//=======================================================================

void MgtBRep_TranslateTool1::MakeSolid(PTopoDS_Shape1& S) const 
{
  Handle(PTopoDS_TSolid1) T = new PTopoDS_TSolid1();
  S.TShape(T);
}


//=======================================================================
//function : MakeCompSolid
//purpose  : 
//=======================================================================

void MgtBRep_TranslateTool1::MakeCompSolid(TopoDS_Shape& S) const 
{
  BRep_Builder B;
  B.MakeCompSolid(TopoDS::CompSolid(S));
}


//=======================================================================
//function : MakeCompSolid
//purpose  : 
//=======================================================================

void MgtBRep_TranslateTool1::MakeCompSolid(PTopoDS_Shape1& S) const 
{
  Handle(PTopoDS_TCompSolid1) T = new PTopoDS_TCompSolid1();
  S.TShape(T);
}


//=======================================================================
//function : MakeCompound
//purpose  : 
//=======================================================================

void MgtBRep_TranslateTool1::MakeCompound(TopoDS_Shape& S) const 
{
  BRep_Builder B;
  B.MakeCompound(TopoDS::Compound(S));
}


//=======================================================================
//function : MakeCompound
//purpose  : 
//=======================================================================

void MgtBRep_TranslateTool1::MakeCompound(PTopoDS_Shape1& S) const 
{
  Handle(PTopoDS_TCompound1) T = new PTopoDS_TCompound1();
  S.TShape(T);
}


//=======================================================================
//function : UpdateVertex
//purpose  : Transient->Persistent
//=======================================================================

void MgtBRep_TranslateTool1::UpdateVertex
(const TopoDS_Shape& S1,
 PTopoDS_Shape1& S2,
 PTColStd_TransientPersistentMap& aMap) const 
{
  const Handle(BRep_TVertex)& TTV = *((Handle(BRep_TVertex)*) &(S1.TShape()));
  const Handle(PBRep_TVertex1)& PTV = *((Handle(PBRep_TVertex1)*) &(S2.TShape()));

  // Point
  PTV->Pnt(TTV->Pnt());

  // Tolerance
  PTV->Tolerance(TTV->Tolerance());

  // Representations
  BRep_ListIteratorOfListOfPointRepresentation itpr(TTV->Points());

  Handle(PBRep_PointRepresentation) PPR, CPPR;
  while (itpr.More()) {

    const Handle(BRep_PointRepresentation)& PR = itpr.Value();

    if (PR->IsPointOnCurve()) {
      Handle(PBRep_PointOnCurve) POC = new 
	PBRep_PointOnCurve(PR->Parameter(),
			   MgtBRep_TranslateTool1::Translate(PR->Curve(), aMap),
			   MgtTopLoc::Translate(PR->Location(), aMap));
      CPPR = POC;
    }

    else if (PR->IsPointOnCurveOnSurface()) {
      Handle(PBRep_PointOnCurveOnSurface) POCS = 
	new PBRep_PointOnCurveOnSurface(PR->Parameter(),
					MgtBRep_TranslateTool1::Translate(PR->PCurve(), aMap),
					MgtBRep_TranslateTool1::Translate(PR->Surface(), aMap),
					MgtTopLoc::Translate(PR->Location(), aMap));
      CPPR = POCS;
    }

    else if (PR->IsPointOnSurface()) {
      Handle(PBRep_PointOnSurface) POS = 
	new PBRep_PointOnSurface(PR->Parameter(),
				 PR->Parameter2(),
				 MgtBRep_TranslateTool1::Translate(PR->Surface(), aMap),
				 MgtTopLoc::Translate(PR->Location(), aMap));
      CPPR = POS;
    }
    
    CPPR->Next(PPR);
    PPR = CPPR;
    itpr.Next();
  }
  
  PTV->Points(PPR);
  
  MgtTopoDS_TranslateTool1::UpdateVertex(S1,S2, aMap);
}


//=======================================================================
//function : UpdateVertex
//purpose  : Persistent->Transient
//=======================================================================

void MgtBRep_TranslateTool1::UpdateVertex
(const PTopoDS_Shape1& S1, 
 TopoDS_Shape& S2,
 PTColStd_PersistentTransientMap& aMap) const 
{
  const Handle(PBRep_TVertex1)& PTV = *((Handle(PBRep_TVertex1)*) &(S1.TShape()));
  const Handle(BRep_TVertex)& TTV = *((Handle(BRep_TVertex)*) &(S2.TShape()));

  // Point
  TTV->Pnt(PTV->Pnt());

  // Tolerance
  TTV->Tolerance(PTV->Tolerance());

  // Representations
  Handle(PBRep_PointRepresentation) PPR = PTV->Points();
  BRep_ListOfPointRepresentation& lpr = TTV->ChangePoints();

  lpr.Clear();

  Handle(BRep_PointRepresentation) PR;

  while (!PPR.IsNull()) {

    if (PPR->IsPointOnCurve()) {
      Handle(PBRep_PointOnCurve)& POC = (Handle(PBRep_PointOnCurve)&) PPR;
      Handle(BRep_PointOnCurve) OC =
	new BRep_PointOnCurve(POC->Parameter(),
			      MgtBRep_TranslateTool1::Translate(POC->Curve(), aMap),
			      MgtTopLoc::Translate(POC->Location(), aMap));
      PR = OC;
    }
    
    else if (PPR->IsPointOnCurveOnSurface()) {
      Handle(PBRep_PointOnCurveOnSurface)& POCS =
	(Handle(PBRep_PointOnCurveOnSurface)&) PPR;
      Handle(BRep_PointOnCurveOnSurface) OCS =
	new BRep_PointOnCurveOnSurface(POCS->Parameter(),
				       MgtBRep_TranslateTool1::Translate(POCS->PCurve(), aMap),
				       MgtBRep_TranslateTool1::Translate(POCS->Surface(), aMap),
				       MgtTopLoc::Translate(POCS->Location(), aMap));
      PR = OCS;
    }
    
    else if (PPR->IsPointOnSurface()) {
      Handle(PBRep_PointOnSurface)& POS = (Handle(PBRep_PointOnSurface)&) PPR;
      Handle(BRep_PointOnSurface) OS =
	new BRep_PointOnSurface(POS->Parameter(),
				POS->Parameter2(),
				MgtBRep_TranslateTool1::Translate(POS->Surface(), aMap),
				MgtTopLoc::Translate(POS->Location(), aMap));
      PR = OS;
    }
    
    lpr.Prepend(PR);
    PPR = PPR->Next();
  }
  
  MgtTopoDS_TranslateTool1::UpdateVertex(S1,S2, aMap);
}


//=======================================================================
//function : UpdateEdge
//purpose  : Transient->Persistent
//=======================================================================

void MgtBRep_TranslateTool1::UpdateEdge
(const TopoDS_Shape& S1,
 PTopoDS_Shape1& S2,
 PTColStd_TransientPersistentMap& aMap) const 
{
  const Handle(BRep_TEdge)&  TTE = *((Handle(BRep_TEdge)*) &(S1.TShape()));
  const Handle(PBRep_TEdge1)& PTE = *((Handle(PBRep_TEdge1)*) &(S2.TShape()));

  // tolerance
  PTE->Tolerance(TTE->Tolerance());
  
  // same parameter
  PTE->SameParameter(TTE->SameParameter());

  // same range
  PTE->SameRange(TTE->SameRange());

  // Degenerated
  PTE->Degenerated(TTE->Degenerated());
  
  // Representations
  BRep_ListIteratorOfListOfCurveRepresentation itcr(TTE->Curves());

  Handle(PBRep_CurveRepresentation) PCR, CPCR;
  Handle(BRep_GCurve) GC;
  Standard_Real f, l;

  while (itcr.More()) {

    const Handle(BRep_CurveRepresentation)& CR = itcr.Value();
    
    GC = Handle(BRep_GCurve)::DownCast(CR);
    if (!GC.IsNull()) {
      GC->Range(f, l);

      // CurveRepresentation is Curve3D
      if (CR->IsCurve3D()) {
	Handle(PBRep_Curve3D) C3D = new 
	  PBRep_Curve3D(MgtBRep_TranslateTool1::Translate(CR->Curve3D(),aMap),
			f, l, MgtTopLoc::Translate(CR->Location(), aMap));
	CPCR = C3D;
      }
      
      // CurveRepresentation is CurveOnSurface
      else if (CR->IsCurveOnSurface()) {
	
	Handle(BRep_CurveOnSurface)& theCOS=(Handle(BRep_CurveOnSurface)&) CR;
	Handle(PBRep_CurveOnSurface) COS;

	// CurveRepresentation is CurveOnSurface
	if (!CR->IsCurveOnClosedSurface()) {
	  COS =
	    new PBRep_CurveOnSurface
	      (MgtBRep_TranslateTool1::Translate(CR->PCurve(), aMap),
	       f, l,
	       MgtBRep_TranslateTool1::Translate(CR->Surface(), aMap),
	       MgtTopLoc::Translate(CR->Location(), aMap));
	}
	
	// CurveRepresentation is CurveOnClosedSurface
	else {
	  // get UVPoints for the CurveOnClosedSurface definition.
	  Handle(BRep_CurveOnClosedSurface)& theCOCS =
	    (Handle(BRep_CurveOnClosedSurface)&) CR;
	  gp_Pnt2d Pnt21, Pnt22;
	  theCOCS->UVPoints2(Pnt21, Pnt22);
	  Handle(PBRep_CurveOnClosedSurface) COCS =
	    new PBRep_CurveOnClosedSurface
	      (MgtBRep_TranslateTool1::Translate(CR->PCurve(), aMap),
	       MgtBRep_TranslateTool1::Translate(CR->PCurve2(), aMap),
	       f, l, 
	       MgtBRep_TranslateTool1::Translate(CR->Surface(), aMap),
	       MgtTopLoc::Translate(CR->Location(), aMap),
	       CR->Continuity());
	  COCS->SetUVPoints2(Pnt21, Pnt22);
	  COS = COCS;
	}

	// get UVPoints for the CurveOnSurface definition.
	gp_Pnt2d Pnt1, Pnt2;
	theCOS->UVPoints(Pnt1, Pnt2);

	// If we don't do that, there is anything (NAN: Not A Number) instead.
	COS->SetUVPoints(Pnt1, Pnt2);
	CPCR = COS;
	
      }
    }
    
    // CurveRepresentation is CurveOn2Surfaces
    else if (CR->IsRegularity()) {
      
      Handle(PBRep_CurveOn2Surfaces) R =
	new PBRep_CurveOn2Surfaces
	  (MgtBRep_TranslateTool1::Translate(CR->Surface(), aMap),
	   MgtBRep_TranslateTool1::Translate(CR->Surface2(), aMap),
	   MgtTopLoc::Translate(CR->Location(), aMap),
	   MgtTopLoc::Translate(CR->Location2(), aMap),
	   CR->Continuity());
      CPCR = R;
    }
    
    // CurveRepresentation is Polygon or Triangulation
    else if (myTriangleMode == MgtBRep_WithTriangle) {
      
      // CurveRepresentation is Polygon3D
      if (CR->IsPolygon3D()) {
	Handle(PBRep_Polygon3D) P3D = 
	  new PBRep_Polygon3D(MgtPoly::Translate(CR->Polygon3D(), aMap),
			      MgtTopLoc::Translate(CR->Location(), aMap));
	CPCR = P3D;
      }

      // CurveRepresentation is PolygonOnSurface
      else if (CR->IsPolygonOnSurface()) {

	// CurveRepresentation is PolygonOnClosedSurface
	if (CR->IsPolygonOnClosedSurface()) {
	  Handle(PBRep_PolygonOnClosedSurface)  PolOCS = new 
	    PBRep_PolygonOnClosedSurface(MgtPoly::Translate(CR->Polygon(), aMap),
					 MgtPoly::Translate(CR->Polygon2(), aMap),
					 MgtBRep_TranslateTool1::Translate(CR->Surface(), aMap),
					 MgtTopLoc::Translate(CR->Location(), aMap));
	  CPCR = PolOCS;
	}

	// CurveRepresentation is PolygonOnSurface
	else {
	  Handle(PBRep_PolygonOnSurface) PolOS = new
	    PBRep_PolygonOnSurface(MgtPoly::Translate(CR->Polygon(), aMap),
				   MgtBRep_TranslateTool1::Translate(CR->Surface(),aMap),
				   MgtTopLoc::Translate(CR->Location(), aMap));
	  CPCR = PolOS;
	}
      }

      // CurveRepresentation is PolygonOnTriangulation
      else if (CR->IsPolygonOnTriangulation()) {

	// CurveRepresentation is PolygonOnClosedTriangulation
	if (CR->IsPolygonOnClosedTriangulation()) {

	  Handle(PBRep_PolygonOnClosedTriangulation) PolOCT= new
	    PBRep_PolygonOnClosedTriangulation
	      (MgtPoly::Translate(CR->PolygonOnTriangulation(), aMap),
	       MgtPoly::Translate(CR->PolygonOnTriangulation2(), aMap),
	       MgtPoly::Translate(CR->Triangulation(), aMap),
	       MgtTopLoc::Translate(CR->Location(), aMap));
	  CPCR = PolOCT;
	}

	// CurveRepresentation is PolygonOnTriangulation
	else {
	  Handle(PBRep_PolygonOnTriangulation) PolOT = new
	    PBRep_PolygonOnTriangulation
	      (MgtPoly::Translate(CR->PolygonOnTriangulation(), aMap),
	       MgtPoly::Translate(CR->Triangulation(), aMap),
	       MgtTopLoc::Translate(CR->Location(), aMap));
	  CPCR = PolOT;
	}
      }
    }
    else {
      // jumps the curve representation
      itcr.Next();
      continue;
    }
    
    Standard_NullObject_Raise_if (CPCR.IsNull(), "Null CurveRepresentation");
    
    CPCR->Next(PCR);
    PCR = CPCR;
    itcr.Next();
  }
  
  // set
  PTE->Curves(PCR);
  
  MgtTopoDS_TranslateTool1::UpdateEdge(S1,S2, aMap);
}


//=======================================================================
//function : UpdateEdge
//purpose  : Persistent->Transient
//=======================================================================

void MgtBRep_TranslateTool1::UpdateEdge
(const PTopoDS_Shape1& S1,
 TopoDS_Shape& S2,
 PTColStd_PersistentTransientMap& aMap) const 
{
  const Handle(PBRep_TEdge1)& PTE = *((Handle(PBRep_TEdge1)*) &(S1.TShape()));
  const Handle(BRep_TEdge)&  TTE = *((Handle(BRep_TEdge)*) &(S2.TShape()));

  // tolerance
  TTE->Tolerance(PTE->Tolerance());
  
  // same parameter
  TTE->SameParameter(PTE->SameParameter());

  // same range
  TTE->SameRange(PTE->SameRange());

  // Degenerated
  TTE->Degenerated(PTE->Degenerated());
  
  // Representations
  Handle(PBRep_CurveRepresentation) PCR = PTE->Curves();
  BRep_ListOfCurveRepresentation& lcr = TTE->ChangeCurves();

  lcr.Clear();
  Handle(BRep_CurveRepresentation) CR;

  while (!PCR.IsNull()) {
    
    if (PCR->IsGCurve()) {
      Handle(PBRep_GCurve)& PGC = (Handle(PBRep_GCurve)&) PCR;
      if (PCR->IsCurveOnSurface()) {
	gp_Pnt2d Pnt1, Pnt2;
	if (!PCR->IsCurveOnClosedSurface()) {
	  // CurveRepresentation is a PBRep_CurveOnSurface
	  Handle(PBRep_CurveOnSurface)& PCOS = (Handle(PBRep_CurveOnSurface)&) PCR;
	  Handle(BRep_CurveOnSurface) COS =
	    new BRep_CurveOnSurface
	      (MgtBRep_TranslateTool1::Translate(PCOS->PCurve(), aMap),
	       MgtBRep_TranslateTool1::Translate(PCOS->Surface(), aMap),
	       MgtTopLoc::Translate(PCOS->Location(), aMap));
	  Pnt1 = PCOS->FirstUV();
	  Pnt2 = PCOS->LastUV();
	  COS->SetUVPoints(Pnt1, Pnt2);
	  COS->SetRange(PGC->First(), PGC->Last());
	  CR = COS;
	}
	else {
	  // CurveRepresentation is a PBRep_CurveOnClosedSurface
	  gp_Pnt2d Pnt21, Pnt22;
	  Handle(PBRep_CurveOnClosedSurface)& PCOCS =
	    (Handle(PBRep_CurveOnClosedSurface)&) PCR;
	  Handle(BRep_CurveOnClosedSurface) COCS =
	    new BRep_CurveOnClosedSurface
	      (MgtBRep_TranslateTool1::Translate(PCOCS->PCurve(), aMap),
	       MgtBRep_TranslateTool1::Translate(PCOCS->PCurve2(), aMap),
	       MgtBRep_TranslateTool1::Translate(PCOCS->Surface(), aMap),
	       MgtTopLoc::Translate(PCOCS->Location(), aMap),
	       PCOCS->Continuity());
	  Pnt21 = PCOCS->FirstUV2();
	  Pnt22 = PCOCS->LastUV2();
	  COCS->SetUVPoints(Pnt1, Pnt2); // Is it necessary?
	  COCS->SetUVPoints2(Pnt21, Pnt22);
	  COCS->SetRange(PGC->First(), PGC->Last());
	  CR = COCS;
	}
      }
      else if (PCR->IsCurve3D()) {
	Handle(PBRep_Curve3D)& PC3D = (Handle(PBRep_Curve3D)&) PCR;
// sln 25.09.2001 BUC60998: correction for edge which has null 3d curve representation        
//	if (! PC3D->Curve3D().IsNull()) {
	  Handle(BRep_Curve3D) C3D =
	    new BRep_Curve3D(MgtBRep_TranslateTool1::Translate(PC3D->Curve3D(), aMap),
			     MgtTopLoc::Translate(PC3D->Location(), aMap));
	  C3D->SetRange(PGC->First(), PGC->Last());
	  CR = C3D;
//	}
      }
    }
    else if (PCR->IsRegularity()) {
      Handle(PBRep_CurveOn2Surfaces)& PR = 
	(Handle(PBRep_CurveOn2Surfaces)&) PCR;
      Handle(BRep_CurveOn2Surfaces) R =
	new BRep_CurveOn2Surfaces
	  (MgtBRep_TranslateTool1::Translate(PR->Surface(), aMap),
	   MgtBRep_TranslateTool1::Translate(PR->Surface2(), aMap),
	   MgtTopLoc::Translate(PR->Location(), aMap),
	   MgtTopLoc::Translate(PR->Location2(), aMap),
	   PR->Continuity());
      CR = R;
    }
    
    else if (myTriangleMode == MgtBRep_WithTriangle) {
      if (PCR->IsPolygon3D()) {
	Handle(PBRep_Polygon3D)& PP3D = (Handle(PBRep_Polygon3D)&) PCR;
	Handle(BRep_Polygon3D) P3D = new
	  BRep_Polygon3D(MgtPoly::Translate(PP3D->Polygon3D(), aMap),
			 MgtTopLoc::Translate(PP3D->Location(), aMap));
	CR = P3D;
      }
      else if (PCR->IsPolygonOnSurface()) {
	if (PCR->IsPolygonOnClosedSurface()) {
	  Handle(PBRep_PolygonOnClosedSurface)& PPol = 
	    (Handle(PBRep_PolygonOnClosedSurface)&) PCR;
	  Handle(BRep_PolygonOnClosedSurface)  PolOCS = new 
	    BRep_PolygonOnClosedSurface
	      (MgtPoly::Translate(PPol->Polygon(), aMap),
	       MgtPoly::Translate(PPol->Polygon2(), aMap),
	       MgtBRep_TranslateTool1::Translate(PPol->Surface(), aMap),
	       MgtTopLoc::Translate(PPol->Location(), aMap));
	  CR = PolOCS;
	}
	else {
	  Handle(PBRep_PolygonOnSurface)& PPol = 
	    (Handle(PBRep_PolygonOnSurface)&) PCR;
	  Handle(BRep_PolygonOnSurface) PolOS = new
	    BRep_PolygonOnSurface
	      (MgtPoly::Translate(PPol->Polygon(), aMap),
	       MgtBRep_TranslateTool1::Translate(PPol->Surface(), aMap),
	       MgtTopLoc::Translate(PPol->Location(), aMap));
	  CR = PolOS;
	}
      }
      // PCR is a PolygonOnTriangulation
      else if (PCR->IsPolygonOnTriangulation()) {
	
	Handle(PBRep_PolygonOnTriangulation)& POT =
	  (Handle(PBRep_PolygonOnTriangulation)&) PCR;
	
	// PCR is a PolygonOnClosedTriangulation
	if (PCR->IsPolygonOnClosedTriangulation()) {
	  Handle(PBRep_PolygonOnClosedTriangulation)& POCT =
	    (Handle(PBRep_PolygonOnClosedTriangulation)&) PCR;
	  
	  Handle(BRep_PolygonOnClosedTriangulation) PolOCT= new
	    BRep_PolygonOnClosedTriangulation
	      (MgtPoly::Translate(POCT->PolygonOnTriangulation(), aMap),
	       MgtPoly::Translate(POCT->PolygonOnTriangulation2(), aMap),
	       MgtPoly::Translate(POCT->Triangulation(), aMap),
	       MgtTopLoc::Translate(POCT->Location(), aMap));
	  CR = PolOCT;
	}
	// PCR is a PolygonOnTriangulation
	else {
	  Handle(BRep_PolygonOnTriangulation) PolOT = new
	    BRep_PolygonOnTriangulation
	      (MgtPoly::Translate(POT->PolygonOnTriangulation(), aMap),
	       MgtPoly::Translate(POT->Triangulation(), aMap),
	       MgtTopLoc::Translate(POT->Location(), aMap));
	  CR = PolOT;
	}
      }
    }
    else {
      // jumps the Curve Representation
      PCR = PCR->Next();
      continue;
    }
    
    Standard_NullObject_Raise_if (CR.IsNull(), "Persistant CurveRep is Null");
    
    lcr.Prepend(CR); 
    PCR = PCR->Next();
  }
  
  MgtTopoDS_TranslateTool1::UpdateEdge(S1,S2, aMap);
}


//=======================================================================
//function : UpdateFace
//purpose  : Transient->Persistent
//=======================================================================

void MgtBRep_TranslateTool1::UpdateFace
(const TopoDS_Shape& S1, 
 PTopoDS_Shape1& S2,
 PTColStd_TransientPersistentMap& aMap) const 
{
  const Handle(BRep_TFace)&  TTF = *((Handle(BRep_TFace)*) &(S1.TShape()));
  const Handle(PBRep_TFace1)& PTF = *((Handle(PBRep_TFace1)*) &(S2.TShape()));

  // natural restriction
  PTF->NaturalRestriction(TTF->NaturalRestriction());

  // tolerance
  PTF->Tolerance(TTF->Tolerance());

  // location
  PTF->Location(MgtTopLoc::Translate(TTF->Location(), aMap));

  // surface
  PTF->Surface(MgtBRep_TranslateTool1::Translate(TTF->Surface(), aMap));

  // Triangulation
  if (myTriangleMode == MgtBRep_WithTriangle) {
    PTF->Triangulation(MgtPoly::Translate(TTF->Triangulation(), aMap));
  }
  
  MgtTopoDS_TranslateTool1::UpdateFace(S1,S2, aMap);
}


//=======================================================================
//function : UpdateFace
//purpose  : Persistent->Transient
//=======================================================================

void MgtBRep_TranslateTool1::UpdateFace
(const PTopoDS_Shape1& S1, 
 TopoDS_Shape& S2,
 PTColStd_PersistentTransientMap& aMap) const 
{
  const Handle(PBRep_TFace1)& PTF = *((Handle(PBRep_TFace1)*) &(S1.TShape()));
  const Handle(BRep_TFace)&  TTF = *((Handle(BRep_TFace)*) &(S2.TShape()));

  // natural restriction
  TTF->NaturalRestriction(PTF->NaturalRestriction());

  // tolerance
  TTF->Tolerance(PTF->Tolerance());

  // location
  TTF->Location(MgtTopLoc::Translate(PTF->Location(), aMap));

  // surface
  TTF->Surface(MgtBRep_TranslateTool1::Translate(PTF->Surface(), aMap));

  // Triangulation
  if ( myTriangleMode == MgtBRep_WithTriangle) {
    TTF->Triangulation(MgtPoly::Translate(PTF->Triangulation(), aMap));
  }

  MgtTopoDS_TranslateTool1::UpdateFace(S1,S2, aMap);
}
