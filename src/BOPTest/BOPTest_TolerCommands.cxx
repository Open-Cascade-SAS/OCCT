// Created on: 2000-03-16
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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


#include <BOPTest.hxx>
#include <BRep_Builder.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_ListOfCurveRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_TFace.hxx>
#include <BRep_Tool.hxx>
#include <BRep_TVertex.hxx>
#include <BRepLib.hxx>
#include <DBRep.hxx>
#include <Draw.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt2d.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_IndexedMapOfTransient.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

#include <stdio.h>
//
static 
  void ProcessVertex(const TopoDS_Vertex&,
                     const TopTools_ListOfShape&,
                     const TopTools_ListOfShape&);
static
  void ProcessEdge(const TopoDS_Edge&, const Standard_Real);

static
  void ReduceVertexTolerance (const TopoDS_Shape&);

static 
  void ReduceFaceTolerance (const TopoDS_Shape&);

static
  void ReduceEdgeTolerance (const TopoDS_Shape&, 
                            const Standard_Real);

static
  void PreparePCurves(const TopoDS_Shape& , 
                      Draw_Interpretor& di);
//
static Standard_Integer breducetolerance (Draw_Interpretor&, Standard_Integer, const char** );
static Standard_Integer  btolx           (Draw_Interpretor&, Standard_Integer, const char** );
static Standard_Integer  bopaddpcs       (Draw_Interpretor&, Standard_Integer, const char** );
//=======================================================================
//function : TolerCommands
//purpose  : 
//=======================================================================
  void  BOPTest::TolerCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) 
    return;

  done = Standard_True;
  // Chapter's name
  const char* g = "BOPTest commands";
  //
  theCommands.Add("breducetolerance" ,  "use breducetolerance Shape",  
                  __FILE__, breducetolerance, g);
  theCommands.Add("btolx"     ,  "use btolx Shape [minTol=1.e-7]",  
                  __FILE__, btolx, g);
  theCommands.Add("bopaddpcs" ,  "Use >bopaddpcs Shape",
                  __FILE__, bopaddpcs, g);
}
//=======================================================================
//function : btolx
//purpose  : 
//=======================================================================
Standard_Integer btolx(Draw_Interpretor& di, 
                       Standard_Integer n,  
                       const char** a)
{
  if (n<2) {
    di << " use btolx Shape [minTol=1.e-7]\n";
    return 1;
  }

  TopoDS_Shape aS = DBRep::Get(a[1]);
  
  if (aS.IsNull()) {
    di << " Null shape is not allowed\n";
    return 1;
  }
  //
  Standard_Real aTolEMin=1.e-7;
  if (n==3) {
    aTolEMin=Draw::Atof(a[2]);
  }
  //
  // Edge Tolerances
  ReduceEdgeTolerance(aS, aTolEMin);
  //
  // Face Tolerances
  ReduceFaceTolerance(aS);
  //
  // Vertex Tolerances
  ReduceVertexTolerance(aS);
  //
  BRepLib::SameParameter(aS, 1.e-7, Standard_True);
  //
  DBRep::Set (a[1], aS);
  return 0;
}
//=======================================================================
//function : ReduceEdgeTolerance
//purpose  : 
//=======================================================================
void ReduceEdgeTolerance (const TopoDS_Shape& aS, 
                          const Standard_Real aTolTreshold)
{
  Standard_Integer i, aNbE;
  TopTools_IndexedMapOfShape aEMap;
  //
  TopExp::MapShapes(aS, TopAbs_EDGE, aEMap);
  //
  aNbE=aEMap.Extent();
  for (i=1; i<=aNbE; i++) {
    const TopoDS_Edge& aE= TopoDS::Edge(aEMap(i));

    ProcessEdge(aE, aTolTreshold);
  } 
}
//=======================================================================
//function : ReduceFaceTolerance
//purpose  : 
//=======================================================================
void ReduceFaceTolerance (const TopoDS_Shape& aS)
{
  Standard_Integer i, j, aNbF, aNbE;
  Standard_Real aTolE, aTolx, aTolEMin;
  TopTools_IndexedMapOfShape aFMap, aEMap;
  //
  aTolEMin=1.e-7;
  //
  TopExp::MapShapes(aS, TopAbs_FACE, aFMap);
  aNbF=aFMap.Extent();
  for (i=1; i<=aNbF; i++) {
    aTolx=1.e6;
    const TopoDS_Face& aF= TopoDS::Face(aFMap(i));
    Handle(BRep_TFace)& aTF = *((Handle(BRep_TFace)*)&aF.TShape());
    //
    TopExp::MapShapes(aF, TopAbs_EDGE, aEMap);
    aNbE=aEMap.Extent();
    for (j=1; j<=aNbE; ++j) {
      const TopoDS_Edge& aE= TopoDS::Edge(aEMap(j));
      aTolE =BRep_Tool::Tolerance(aE);
      if (aTolE<aTolx) {
        aTolx=aTolE;
      }
    }
    aTolE=(aTolx>aTolEMin) ? aTolx : aTolEMin;
    aTF->Tolerance(aTolE);
  }
}
//=======================================================================
//function : ReduceVertexTolerance
//purpose  : 
//=======================================================================
void ReduceVertexTolerance (const TopoDS_Shape& aS)
{
  Standard_Integer i, aNbV;
  TopTools_IndexedDataMapOfShapeListOfShape aVEMap, aVFMap;
  
  TopExp::MapShapesAndUniqueAncestors(aS, TopAbs_VERTEX, TopAbs_EDGE, aVEMap);
  TopExp::MapShapesAndUniqueAncestors(aS, TopAbs_VERTEX, TopAbs_FACE, aVFMap);

  aNbV=aVEMap.Extent();
  for (i=1; i<=aNbV; i++) {
    const TopoDS_Vertex& aV= TopoDS::Vertex(aVEMap.FindKey(i));
    const TopTools_ListOfShape& aLE=aVEMap(i);
    const TopTools_ListOfShape& aLF=aVFMap.FindFromKey(aV);
    
    ProcessVertex(aV, aLE, aLF);
  }
}
//=======================================================================
//function : ProcessEdge
//purpose  : 
//=======================================================================
void ProcessEdge(const TopoDS_Edge& aE, const Standard_Real aTolTreshold)
{
  Standard_Integer i, aNb=23;
  Standard_Real aD2, aTolMax2, aT1, aT2, aT, dT;
  gp_Pnt aPC3D, aP3D;
  gp_Pnt2d aPC2D;

  //TopTools_ListIteratorOfListOfShape anIt;// Wng in Gcc 3.0
  BRep_ListIteratorOfListOfCurveRepresentation itcr;
  //
  Handle(Geom_Curve) aC3D=BRep_Tool::Curve(aE, aT1, aT2);
  if (aC3D.IsNull()) {
    return;
  }
  //
  dT=(aT2-aT1)/aNb;
  //
  Handle(BRep_TEdge)& TE = *((Handle(BRep_TEdge)*)&aE.TShape());
  const TopLoc_Location& Eloc = aE.Location();
  //
  aTolMax2=-1.e6;
  const BRep_ListOfCurveRepresentation& aLCR=TE->Curves();
  //
  itcr.Initialize(aLCR);
  for (; itcr.More(); itcr.Next()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    const TopLoc_Location& loc = cr->Location();
    TopLoc_Location L = (Eloc * loc);//.Predivided(aV.Location());
    //
    // 3D-Curve
    if (cr->IsCurve3D()) {
      continue;
    }
    //
    // 2D-Curve
    else if (cr->IsCurveOnSurface()) {
      const Handle(Geom2d_Curve)& aC2D = cr->PCurve();
      if (aC2D.IsNull()) {
        continue;
      }
      // Surface
      const Handle(Geom_Surface)& aS=cr->Surface();
      //
      // 2D-point treatment
      for (i=0; i<=aNb; ++i) {
        aT=aT1+i*dT;
        if (i==aNb) {
          aT=aT2;
        }
        aPC3D=aC3D->Value(aT);
        aPC2D=aC2D->Value(aT);
        aS->D0(aPC2D.X(), aPC2D.Y(), aP3D);
        aP3D.Transform(L.Transformation());
        aD2=aPC3D.SquareDistance(aP3D);
        if (aD2 > aTolMax2) {
          aTolMax2=aD2;
        }
      }
    } //if (cr->IsCurveOnSurface())
  }//for (; itcr.More(); itcr.Next())

  //#########################################################
  //
  if (aTolMax2<0.){
    return;
  }
  //
  //
  aTolMax2=sqrt(aTolMax2); 
  
  //printf(" aTolMax=%15.10lf, aTolWas=%15.10lf\n", aTolMax2, aTolE);

  Standard_Real aTolSet;
  aTolSet=(aTolMax2>aTolTreshold) ? aTolMax2 : aTolTreshold;

  TE->Tolerance(aTolSet);
}
//=======================================================================
//function : ProcessVertex
//purpose  : 
//=======================================================================
void ProcessVertex(const TopoDS_Vertex& aV,
                   const TopTools_ListOfShape& aLE,
                   const TopTools_ListOfShape& aLF)
{
  Standard_Real aTol, aD2, aTolMax2, aTolE, aParam;
  gp_Pnt aPC3D;
  gp_Pnt2d aPC2D;
  TopAbs_Orientation anOrV;

  TopTools_ListIteratorOfListOfShape anIt;
  TopExp_Explorer aVExp;
  
  BRep_ListIteratorOfListOfCurveRepresentation itcr;
  //
  aTolMax2=-1.e6;
  //
  Handle(BRep_TVertex)& TV = *((Handle(BRep_TVertex)*) &aV.TShape());
  const gp_Pnt& aPV3D = TV->Pnt();
  aTol =BRep_Tool::Tolerance(aV);
  //
  anIt.Initialize(aLE);
  for (; anIt.More(); anIt.Next()) {
    const TopoDS_Edge& aE=TopoDS::Edge(anIt.Value());
    //
    Handle(BRep_TEdge)& TE = *((Handle(BRep_TEdge)*)&aE.TShape());
    const TopLoc_Location& Eloc = aE.Location();
    //
    aVExp.Init(aE, TopAbs_VERTEX);
    for (; aVExp.More(); aVExp.Next()) {
      const TopoDS_Vertex& aVx=TopoDS::Vertex(aVExp.Current());
      //
      if (!aVx.IsSame(aV)) {
        continue;
      }
      //
      anOrV=aVx.Orientation();
      if (!(anOrV==TopAbs_FORWARD || anOrV==TopAbs_REVERSED)) {
        continue;
      }
      //
      const BRep_ListOfCurveRepresentation& aLCR=TE->Curves();
      itcr.Initialize(aLCR);
      for (; itcr.More(); itcr.Next()) {
        const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
        const TopLoc_Location& loc = cr->Location();
        TopLoc_Location L = (Eloc * loc).Predivided(aV.Location());
        //
        // 3D-Curve
        if (cr->IsCurve3D()) {
          const Handle(Geom_Curve)& aC3D = cr->Curve3D();
          //
          if (aC3D.IsNull()) {
            continue;
          }
          // 3D-point treatment
          aParam=BRep_Tool::Parameter(aVx, aE);
          aPC3D= aC3D->Value(aParam);
          aPC3D.Transform(L.Transformation());
          aD2=aPV3D.SquareDistance(aPC3D);
          if (aD2 > aTolMax2) {
            aTolMax2=aD2;
          }
          //
        }//if (cr->IsCurve3D())
        //
        // 2D-Curve
        else if (cr->IsCurveOnSurface()) {
          const Handle(Geom2d_Curve)& aC2D = cr->PCurve();
          if (aC2D.IsNull()) {
            continue;
          }
          // Surface
          const Handle(Geom_Surface)& aS=cr->Surface();
          //
          // 2D-point treatment
          aParam=BRep_Tool::Parameter(aVx, aE, aS, L);
          aPC2D=aC2D->Value(aParam);
          aS->D0(aPC2D.X(), aPC2D.Y(), aPC3D);
          aPC3D.Transform(L.Transformation());
          aD2=aPV3D.SquareDistance(aPC3D);
          if (aD2 > aTolMax2) {
            aTolMax2=aD2;
          }
        } //if (cr->IsCurveOnSurface())
        
      }//for (; itcr.More(); itcr.Next())
    }//for (; aVExp.More(); aVExp.Next()) 
  }//for (; anIt.More(); anIt.Next()) 
  //#########################################################
  //
  // Reducing
  if (aTolMax2<0.){
    return;
  }
  //
  aTolMax2=sqrt(aTolMax2); 
  if (aTolMax2>aTol) {
    return;
  }
  //
  anIt.Initialize(aLE);
  for (; anIt.More(); anIt.Next()) {
    const TopoDS_Edge& aE=TopoDS::Edge(anIt.Value());

    aTolE =BRep_Tool::Tolerance(aE);
    if (aTolMax2 < aTolE) {
      aTolMax2=aTolE;
    }
  }
  //
  anIt.Initialize(aLF);
  for (; anIt.More(); anIt.Next()) {
    const TopoDS_Face& aF=TopoDS::Face(anIt.Value());
    
    aTolE =BRep_Tool::Tolerance(aF);
    if (aTolMax2 < aTolE) {
      aTolMax2=aTolE;
    }
  }
  //
  if (aTolMax2>aTol) {
    return;
  }
  //
  // Update Tolerance
  TV->Tolerance(aTolMax2);
}
//=======================================================================
//function : breducetolerance
//purpose  : 
//=======================================================================
Standard_Integer  breducetolerance(Draw_Interpretor& di, 
                                   Standard_Integer n, 
                                   const char** a)
{
  if (n<2) {
    di << " use bupdatetolerance Shape\n";
    return 1;
  }

  TopoDS_Shape aS = DBRep::Get(a[1]);
  
  if (aS.IsNull()) {
    di << " Null shape is not allowed \n";
    return 1;
  }
  ReduceVertexTolerance(aS);
  DBRep::Set (a[1], aS);
  
  return 0;
}
//
//=======================================================================
//function : bopaddpcs
//purpose  : Some Edges do not contain P-Curves on Faces to which 
//           they belong to. 
//           These faces usually based on Geom_Plane surface.
//           To prevent sophisticated treatment the Command "bopaddpcs:
//           adds P-Curves for the edges . 
//=======================================================================
Standard_Integer bopaddpcs(Draw_Interpretor& di, 
                           Standard_Integer n,  
                           const char** a)
{
  if (n<2) {
    di << " Use >bopaddpcs Shape\n";
    return 1;
  }

  TopoDS_Shape aS = DBRep::Get(a[1]);
  
  if (aS.IsNull()) {
    di << " Null shape is not allowed \n";
    return 1;
  }
  //
  PreparePCurves(aS, di);
  //
  DBRep::Set (a[1], aS);
  return 0;
}
//=======================================================================
//function : PreparePCurves
//purpose  : 
//=======================================================================
void PreparePCurves(const TopoDS_Shape& aShape, Draw_Interpretor& di)
{
  Standard_Integer i, aNbE;
  Standard_Real aTolE, aT1, aT2;
  TopTools_IndexedDataMapOfShapeListOfShape aEFMap;
  TopLoc_Location aLoc;
  Handle(Geom_Curve)   aC3D;
  Handle(Geom2d_Curve) aC2D;
  BRep_Builder aBB;
  //
  TopExp::MapShapesAndAncestors (aShape, TopAbs_EDGE, TopAbs_FACE, aEFMap);
  //
  aNbE=aEFMap.Extent();
  for (i=1; i<=aNbE; ++i) {
    const TopoDS_Edge& aE=TopoDS::Edge(aEFMap.FindKey(i));
    //
    if (BRep_Tool::Degenerated(aE)) {
      continue;
    }
    //
    aC3D=BRep_Tool::Curve(aE, aT1, aT2);
    if (aC3D.IsNull()) {
      continue;
    }
    aTolE=BRep_Tool::Tolerance(aE);
    //
    const TopTools_ListOfShape& aLF=aEFMap(i);
    TopTools_ListIteratorOfListOfShape aFIt(aLF);
    for (; aFIt.More(); aFIt.Next()) {
      const TopoDS_Face& aF=TopoDS::Face(aFIt.Value());
      //
      // Map of surfaces on which the edge lays .
      TColStd_IndexedMapOfTransient aSCRMap;
      Handle(BRep_TEdge)& TE = *((Handle(BRep_TEdge)*)&aE.TShape());
      const BRep_ListOfCurveRepresentation& aLCR=TE->Curves();
      BRep_ListIteratorOfListOfCurveRepresentation itcr;
      itcr.Initialize(aLCR);
      for (; itcr.More(); itcr.Next()) {
        const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
        //
        if (cr->IsCurveOnSurface()) {
          const Handle(Geom_Surface)& aSCR=cr->Surface();
          aSCRMap.Add(aSCR);
        }
        //
      }
      //
      const Handle(Geom_Surface)& aS=BRep_Tool::Surface(aF, aLoc);
      if (!aSCRMap.Contains(aS)) {
        // try to obtain 2D curve
        aC2D=BRep_Tool::CurveOnSurface(aE, aS, aLoc, aT1, aT2);
        if (aC2D.IsNull()) {
          di << " Warning: Can not obtain P-Curve\n";
          continue;
        }
        else {
          aBB.UpdateEdge(aE, aC2D, aF, aTolE);
        }
      }
    }
  }
}
