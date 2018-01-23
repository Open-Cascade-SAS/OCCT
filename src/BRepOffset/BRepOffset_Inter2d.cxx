// Created on: 1996-09-03
// Created by: Yves FRICAUD
// Copyright (c) 1996-1999 Matra Datavision
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

//  Modified by skv - Wed Dec 24 18:08:39 2003 OCC4455

#include <Adaptor2d_HCurve2d.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Bnd_Box.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BRep_Builder.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAlgo_AsDes.hxx>
#include <BRepLib.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <BRepOffset_Inter2d.hxx>
#include <BRepOffset_Offset.hxx>
#include <BRepOffset_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Geom2dConvert_CompCurveToBSplineCurve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <GeomLib.hxx>
#include <GeomProjLib.hxx>
#include <gp_Pnt.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <IntRes2d_IntersectionSegment.hxx>
#include <Precision.hxx>
#include <TColGeom2d_SequenceOfCurve.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>

#include <stdio.h>
#ifdef DRAW 
#include <DBRep.hxx>
#include <Geom2d_BoundedCurve.hxx>
#include <Geom_BoundedSurface.hxx>
#include <Geom_BoundedCurve.hxx>
#include <BRep_CurveOnSurface.hxx>
#include <Geom_Surface.hxx>
Standard_Boolean Inter2dAffichInt2d;
static Standard_Integer NbF2d = 0;
static Standard_Integer NbE2d = 0;
static Standard_Integer NbNewVertices  = 0;
#endif

//=======================================================================
//function : CommonVertex
//purpose  : 
//=======================================================================

static TopoDS_Vertex CommonVertex(TopoDS_Edge& E1,
				  TopoDS_Edge& E2)
{
  TopoDS_Vertex V1[2],V2[2],V;
  //
  TopExp::Vertices(E1,V1[0],V1[1], Standard_True);
  TopExp::Vertices(E2,V2[0],V2[1], Standard_True);
  // The first edge is the current one, the second edge is the next one.
  // We check last vertex of the first edge first.
  if (V1[1].IsSame(V2[0]) || V1[1].IsSame(V2[1])) return V1[1];
  if (V1[0].IsSame(V2[0]) || V1[0].IsSame(V2[1])) return V1[0];
  //
  return V;
}

static Standard_Boolean IsOrientationChanged(TopTools_IndexedMapOfShape& theMap,
                                             const TopoDS_Edge& theEdge)
{
  Standard_Boolean IsOrChanged = Standard_False;
  
  if (!theMap.Contains(theEdge))
    theMap.Add(theEdge);
  else
  {
    Standard_Integer anInd = theMap.FindIndex(theEdge);
    const TopoDS_Shape& anEdge = theMap(anInd);
    if (theEdge.Orientation() != anEdge.Orientation())
    {
      theMap.Substitute( anInd, theEdge );
      IsOrChanged = Standard_True;
    }
  }

  return IsOrChanged;
}


//=======================================================================
//function : Store
//purpose  : Store the vertices <theLV> into AsDes for the edge <theEdge>.
//           The vertices are added despite of the coincidence with
//           already added vertices. When all vertices for all edges
//           are added the coinciding chains of vertices should be fused
//           using FuseVertices() method.
//=======================================================================
static void Store(const TopoDS_Edge& theEdge,
                  const TopTools_ListOfShape& theLV,
                  const Standard_Real theTol,
                  const Standard_Boolean IsToUpdate,
                  Handle(BRepAlgo_AsDes) theAsDes2d,
                  TopTools_IndexedDataMapOfShapeListOfShape& theDMVV)
{
  const TopTools_ListOfShape& aLVEx = theAsDes2d->Descendant(theEdge);
  if (!IsToUpdate && aLVEx.IsEmpty()) {
    if (theLV.Extent()) theAsDes2d->Add(theEdge, theLV);
    return;
  }
  //
  GeomAPI_ProjectPointOnCurve aProjPC;
  if (IsToUpdate) {
    Standard_Real aT1, aT2;
    const Handle(Geom_Curve)& aC = BRep_Tool::Curve(theEdge, aT1, aT2);
    aProjPC.Init(aC, aT1, aT2);
  }
  //
  TopTools_MapOfShape aMV;
  TopTools_ListIteratorOfListOfShape aIt(theLV);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Vertex& aV = TopoDS::Vertex(aIt.Value());
    if (!aMV.Add(aV)) {
      continue;
    }
    //
    const gp_Pnt& aP = BRep_Tool::Pnt(aV);
    //
    TopTools_ListOfShape aLVC;
    TopTools_ListIteratorOfListOfShape aItEx(aLVEx);
    for (; aItEx.More(); aItEx.Next()) {
      const TopoDS_Vertex& aVEx = TopoDS::Vertex(aItEx.Value());
      if (aV.IsSame(aVEx)) {
        break;
      }
      const gp_Pnt& aPEx = BRep_Tool::Pnt(aVEx);
      if (aP.IsEqual(aPEx, theTol)) {
        aLVC.Append(aVEx);
      }
    }
    //
    if (aItEx.More()) {
      continue;
    }
    //
    if (IsToUpdate) {
      // get parameter of the vertex on the edge
      aProjPC.Perform(aP);
      if (!aProjPC.NbPoints()) {
        continue;
      }
      //
      if (aProjPC.LowerDistance() > theTol) {
        continue;
      }
      //
      Standard_Real aT = aProjPC.LowerDistanceParameter();
      TopoDS_Shape aLocalShape = aV.Oriented(TopAbs_INTERNAL);
      BRep_Builder().UpdateVertex(TopoDS::Vertex(aLocalShape), aT, theEdge, theTol);
    }
    else {
      BRep_Builder().UpdateVertex(aV, theTol);
    }
    //
    if (aLVC.Extent()) {
      TopTools_ListIteratorOfListOfShape aItLV(aLVC);
      for (; aItLV.More(); aItLV.Next()) {
        const TopoDS_Shape& aVC = aItLV.Value();
        TopTools_ListOfShape* pLV = theDMVV.ChangeSeek(aVC);
        if (!pLV) {
          pLV = &theDMVV(theDMVV.Add(aVC, TopTools_ListOfShape()));
        }
        pLV->Append(aV);
      }
      //
      TopTools_ListOfShape* pLV = theDMVV.ChangeSeek(aV);
      if (!pLV) {
        pLV = &theDMVV(theDMVV.Add(aV, TopTools_ListOfShape()));
      }
      pLV->Append(aLVC);
    }
    theAsDes2d->Add(theEdge, aV);
  }
}

//=======================================================================
//function : Store
//purpose  : Store the intersection vertices between two edges into AsDes
//=======================================================================
static void  Store (const TopoDS_Edge& theE1,
                    const TopoDS_Edge& theE2,
                    const TopTools_ListOfShape& theLV1,
                    const TopTools_ListOfShape& theLV2,
                    const Standard_Real theTol,
                    Handle(BRepAlgo_AsDes) theAsDes2d,
                    TopTools_IndexedDataMapOfShapeListOfShape& theDMVV)
{
  for (Standard_Integer i = 0; i < 2; ++i) {
    const TopoDS_Edge& aE = !i ? theE1 : theE2;
    const TopTools_ListOfShape& aLV = !i ? theLV1 : theLV2;
    Store(aE, aLV, theTol, Standard_False, theAsDes2d, theDMVV);
  }
}

//=======================================================================
//function : EdgeInter
//purpose  : 
//=======================================================================

static void EdgeInter(const TopoDS_Face&              F,
                      const BRepAdaptor_Surface&      BAsurf,
                      const TopoDS_Edge&              E1,
                      const TopoDS_Edge&              E2,
                      const Handle(BRepAlgo_AsDes)&   AsDes,
                      Standard_Real                   Tol,
                      Standard_Boolean                WithOri,
                      TopTools_IndexedDataMapOfShapeListOfShape& aDMVV)
{
#ifdef DRAW
  if (Inter2dAffichInt2d) {
    char name[256];
    sprintf(name,"E2d_%d_%d",NbF2d,NbE2d++);
    DBRep::Set(name,E1);
    sprintf(name,"E2d_%d_%d",NbF2d,NbE2d++);
    DBRep::Set(name,E2);
  }
#endif

  if (E1.IsSame(E2))
    return;

  Standard_Real f[3],l[3];
  Standard_Real TolDub = 1.e-7;
  Standard_Integer i;

  BRep_Tool::Range(E1, f[1], l[1]);
  BRep_Tool::Range(E2, f[2], l[2]);

  BRepAdaptor_Curve CE1(E1,F);
  BRepAdaptor_Curve CE2(E2,F);

  TopoDS_Edge                 EI[3]; EI[1] = E1; EI[2] = E2;
  TopTools_ListOfShape        LV1;   
  TopTools_ListOfShape        LV2; 
  BRep_Builder                B;

  TopoDS_Vertex CV;
  if (!TopExp::CommonVertex( E1, E2, CV ))
    {
      BRepLib::BuildCurve3d(E1);
      BRepLib::BuildCurve3d(E2);
      
      Standard_Real TolSum = BRep_Tool::Tolerance(E1) + BRep_Tool::Tolerance(E2);
      TolSum = Max( TolSum, 1.e-5 );

      TColgp_SequenceOfPnt   ResPoints;
      TColStd_SequenceOfReal ResParamsOnE1, ResParamsOnE2;
      gp_Pnt DegPoint;
      Standard_Boolean WithDegen = BRep_Tool::Degenerated(E1) || BRep_Tool::Degenerated(E2);
      
      if (WithDegen)
        {
          Standard_Integer ideg = (BRep_Tool::Degenerated(E1))? 1 : 2;
          TopoDS_Iterator iter( EI[ideg] );
          if (iter.More())
            {
              const TopoDS_Vertex& vdeg = TopoDS::Vertex(iter.Value());
              DegPoint = BRep_Tool::Pnt(vdeg);
            }
          else
            {
              BRepAdaptor_Curve CEdeg( EI[ideg], F );
              DegPoint = CEdeg.Value( CEdeg.FirstParameter() );
            }
        }
        //
      Handle(Geom2d_Curve) pcurve1 = BRep_Tool::CurveOnSurface(E1, F, f[1], l[1]);
      Handle(Geom2d_Curve) pcurve2 = BRep_Tool::CurveOnSurface(E2, F, f[2], l[2]);
      Geom2dAdaptor_Curve GAC1(pcurve1, f[1], l[1]);
      Geom2dAdaptor_Curve GAC2(pcurve2, f[2], l[2]);
      Geom2dInt_GInter Inter2d( GAC1, GAC2, TolDub, TolDub );
      for (i = 1; i <= Inter2d.NbPoints(); i++)
        {
          gp_Pnt P3d;
          if (WithDegen)
            P3d = DegPoint;
          else
            {
              gp_Pnt2d P2d = Inter2d.Point(i).Value();
              P3d = BAsurf.Value( P2d.X(), P2d.Y() );
            }
          ResPoints.Append( P3d );
          ResParamsOnE1.Append( Inter2d.Point(i).ParamOnFirst() );
          ResParamsOnE2.Append( Inter2d.Point(i).ParamOnSecond() );
        }

      for (i = 1; i <= ResPoints.Length(); i++)
        {
          Standard_Real aT1 = ResParamsOnE1(i); //ponc1.Parameter();
          Standard_Real aT2 = ResParamsOnE2(i); //ponc2.Parameter();
          if (Precision::IsInfinite(aT1) || Precision::IsInfinite(aT2))
            {
#ifdef OCCT_DEBUG
              cout << "Inter2d : Solution rejected due to infinite parameter"<<endl;
#endif
              continue;
            }
          
          gp_Pnt P = ResPoints(i); //ponc1.Value();
          TopoDS_Vertex aNewVertex = BRepLib_MakeVertex(P);
          aNewVertex.Orientation(TopAbs_INTERNAL);
          B.UpdateVertex( aNewVertex, aT1, E1, Tol );
          B.UpdateVertex( aNewVertex, aT2, E2, Tol );
          gp_Pnt P1 = CE1.Value(aT1);
          gp_Pnt P2 = CE2.Value(aT2);
          Standard_Real dist1, dist2, dist3;
          dist1 = P1.Distance(P);
          dist2 = P2.Distance(P);
          dist3 = P1.Distance(P2);
          dist1 = Max( dist1, dist2 );
          dist1 = Max( dist1, dist3 );
          B.UpdateVertex( aNewVertex, dist1 );
          
#ifdef OCCT_DEBUG
          if (aT1 < f[1]-Tol  || aT1 > l[1]+Tol)
            {
              cout << "out of limit"<<endl;
              cout<<"aT1 = "<<aT1<<", f[1] = "<<f[1]<<", l[1] = "<<l[1]<<endl;
            }
          if (aT2 < f[2]-Tol  || aT2 > l[2]+Tol)
            {
              cout << "out of limit"<<endl;
              cout<<"aT2 = "<<aT2<<", f[2] = "<<f[2]<<", l[2] = "<<l[2]<<endl;
            }
          Standard_Real MilTol2 = 1000*Tol*Tol;
          if (P1.SquareDistance(P) >  MilTol2 || P2.SquareDistance(P) > MilTol2 || P1.Distance(P2) > 2.*Tol)
            {
              cout << "Inter2d : Solution rejected "<<endl;
              cout<<"P  = "<<P.X()<<" "<<P.Y()<<" "<<P.Z()<<endl;
              cout<<"P1 = "<<P1.X()<<" "<<P1.Y()<<" "<<P1.Z()<<endl;
              cout<<"P2 = "<<P2.X()<<" "<<P2.Y()<<" "<<P2.Z()<<endl;
              cout<<"MaxDist = "<<dist1<<endl;
            }
#endif
          //define the orientation of a new vertex
          TopAbs_Orientation OO1 = TopAbs_REVERSED;
          TopAbs_Orientation OO2 = TopAbs_REVERSED;
          if (WithOri)
            {
              BRepAdaptor_Curve2d PCE1( E1, F );
              BRepAdaptor_Curve2d PCE2( E2, F );
              gp_Pnt2d P2d1, P2d2;
              gp_Vec2d V1, V2, V1or, V2or;
              PCE1.D1( aT1, P2d1, V1 );
              PCE2.D1( aT2, P2d2, V2 );
              V1or = V1; V2or = V2;
              if (E1.Orientation() == TopAbs_REVERSED) V1or.Reverse();
              if (E2.Orientation() == TopAbs_REVERSED) V2or.Reverse();
              Standard_Real CrossProd = V2or ^ V1;
#ifdef OCCT_DEBUG
              if (Abs(CrossProd) <= gp::Resolution())
                cout<<endl<<"CrossProd = "<<CrossProd<<endl;
#endif
              if (CrossProd > 0.)
                OO1 = TopAbs_FORWARD;
              CrossProd = V1or ^ V2;
              if (CrossProd > 0.)
                OO2 = TopAbs_FORWARD;
            }
          LV1.Append( aNewVertex.Oriented(OO1) );
          LV2.Append( aNewVertex.Oriented(OO2) );
        }
    }
  
  //----------------------------------
  // Test at end.
  //---------------------------------
  Standard_Real U1,U2;
  Standard_Real TolConf = Tol;
  TopoDS_Vertex V1[2],V2[2];
  TopExp::Vertices(E1,V1[0],V1[1]);
  TopExp::Vertices(E2,V2[0],V2[1]);

  Standard_Integer j;
  for (j = 0; j < 2; j++) {
    if (V1[j].IsNull()) continue;
    for (Standard_Integer k = 0; k < 2; k++) {
      if (V2[k].IsNull()) continue;
      if (V1[j].IsSame(V2[k])) {
        if (AsDes->HasAscendant(V1[j])) {
          continue;
        }
      }
      //
      gp_Pnt P1 = BRep_Tool::Pnt(V1[j]);
      gp_Pnt P2 = BRep_Tool::Pnt(V2[k]);
      Standard_Real Dist = P1.Distance(P2); 
      if (Dist < TolConf) {
        Standard_Real aTol = 
          Max(BRep_Tool::Tolerance(V1[j]), BRep_Tool::Tolerance(V2[k]));
        TopoDS_Vertex V = BRepLib_MakeVertex(P1);
        U1 = (j == 0) ? f[1] : l[1];
        U2 = (k == 0) ? f[2] : l[2];
        //
        TopoDS_Shape aLocalShape = V.Oriented(TopAbs_INTERNAL);
        B.UpdateVertex(TopoDS::Vertex(aLocalShape),U1,E1,aTol);
        B.UpdateVertex(TopoDS::Vertex(aLocalShape),U2,E2,aTol);
        //
        LV1.Prepend(V.Oriented(V1[j].Orientation()));
        LV2.Prepend(V.Oriented(V2[k].Orientation()));
      }
    }
  }

  Standard_Boolean AffichPurge = Standard_False;

  if ( !LV1.IsEmpty()) {
    //----------------------------------
    // Remove all vertices.
    // There can be doubles
    //----------------------------------
    TopTools_ListIteratorOfListOfShape it1LV1,it1LV2,it2LV1;
    gp_Pnt P1,P2;
    Standard_Boolean Purge = Standard_True;

    while (Purge) {
      i = 1;
      Purge = Standard_False;
      for (it1LV1.Initialize(LV1),it1LV2.Initialize(LV2); 
           it1LV1.More(); it1LV1.Next(),it1LV2.Next()) {
        j = 1;
        it2LV1.Initialize(LV1);
        while (j < i) {      
          P1 = BRep_Tool::Pnt(TopoDS::Vertex(it1LV1.Value()));
          P2 = BRep_Tool::Pnt(TopoDS::Vertex(it2LV1.Value()));
//  Modified by skv - Thu Jan 22 18:19:04 2004 OCC4455 Begin
//           if (P1.IsEqual(P2,10*Tol)) {
          Standard_Real aTol;

          aTol = Max(BRep_Tool::Tolerance(TopoDS::Vertex(it1LV1.Value())),
                     BRep_Tool::Tolerance(TopoDS::Vertex(it2LV1.Value())));
          if (P1.IsEqual(P2,aTol)) {
//  Modified by skv - Thu Jan 22 18:19:05 2004 OCC4455 End
            LV1.Remove(it1LV1);
            LV2.Remove(it1LV2);
            if (AffichPurge) cout <<"Doubles removed in EdgeInter."<<endl;
            Purge = Standard_True;
            break;
          }
          j++;
          it2LV1.Next();
        }
        if (Purge) break;
        i++;
      }
    }
    //---------------------------------
    // Vertex storage in DS.
    //---------------------------------
    Standard_Real TolStore = BRep_Tool::Tolerance(E1) + BRep_Tool::Tolerance(E2);
    TolStore = Max(TolStore, 10.*Tol);
    Store (E1,E2,LV1,LV2,TolStore,AsDes, aDMVV);
  }
}
//=======================================================================
//function : EdgeInter
//purpose  : 
//=======================================================================

static void RefEdgeInter(const TopoDS_Face&              F,
                         const BRepAdaptor_Surface&      BAsurf,
                         const TopoDS_Edge&              E1,
                         const TopoDS_Edge&              E2,
                         const Handle(BRepAlgo_AsDes)&   AsDes,
                         Standard_Real                   Tol,
                         Standard_Boolean                WithOri,
                         gp_Pnt&                         Pref,
                         TopTools_IndexedDataMapOfShapeListOfShape& aDMVV,
                         Standard_Boolean&               theCoincide)
{
#ifdef DRAW
  if (Inter2dAffichInt2d) {
    char name[256];
    sprintf(name,"E2d_%d_%d",NbF2d,NbE2d++);
    DBRep::Set(name,E1);
    sprintf(name,"E2d_%d_%d",NbF2d,NbE2d++);
    DBRep::Set(name,E2);
  }
#endif
  //
  theCoincide = Standard_False;
  //
  if (E1.IsSame(E2))
    return;

  Standard_Real f[3],l[3];
  Standard_Real TolDub = 1.e-7;
  Standard_Integer i;

  //BRep_Tool::Range(E1, f[1], l[1]);
  //BRep_Tool::Range(E2, f[2], l[2]);

  BRepAdaptor_Curve CE1(E1,F);
  BRepAdaptor_Curve CE2(E2,F);

  TopoDS_Edge                 EI[3]; EI[1] = E1; EI[2] = E2;
  TopTools_ListOfShape        LV1;   
  TopTools_ListOfShape        LV2; 
  BRep_Builder                B;

  BRepLib::BuildCurve3d(E1);
  BRepLib::BuildCurve3d(E2);

  Standard_Real TolSum = BRep_Tool::Tolerance(E1) + BRep_Tool::Tolerance(E2);
  TolSum = Max( TolSum, 1.e-5 );

  TColgp_SequenceOfPnt   ResPoints;
  TColStd_SequenceOfReal ResParamsOnE1, ResParamsOnE2;
  gp_Pnt DegPoint;
  Standard_Boolean WithDegen = BRep_Tool::Degenerated(E1) || BRep_Tool::Degenerated(E2);
  
  if (WithDegen)
    {
      Standard_Integer ideg = (BRep_Tool::Degenerated(E1))? 1 : 2;
      TopoDS_Iterator iter( EI[ideg] );
      if (iter.More())
        {
          const TopoDS_Vertex& vdeg = TopoDS::Vertex(iter.Value());
          DegPoint = BRep_Tool::Pnt(vdeg);
        }
      else
        {
          BRepAdaptor_Curve CEdeg( EI[ideg], F );
          DegPoint = CEdeg.Value( CEdeg.FirstParameter() );
        }
    }
  //
  Handle(Geom2d_Curve) pcurve1 = BRep_Tool::CurveOnSurface(E1, F, f[1], l[1]);
  Handle(Geom2d_Curve) pcurve2 = BRep_Tool::CurveOnSurface(E2, F, f[2], l[2]);
  Geom2dAdaptor_Curve GAC1(pcurve1, f[1], l[1]);
  Geom2dAdaptor_Curve GAC2(pcurve2, f[2], l[2]);
  Geom2dInt_GInter Inter2d( GAC1, GAC2, TolDub, TolDub );
  //
  if (!Inter2d.IsDone() || !Inter2d.NbPoints()) {
    theCoincide = (Inter2d.NbSegments() &&
                  (GAC1.GetType() == GeomAbs_Line) &&
                  (GAC2.GetType() == GeomAbs_Line));
    return;
  }
  //
  for (i = 1; i <= Inter2d.NbPoints(); i++)
    {
      gp_Pnt P3d;
      if (WithDegen)
        P3d = DegPoint;
      else
        {
          gp_Pnt2d P2d = Inter2d.Point(i).Value();
          P3d = BAsurf.Value( P2d.X(), P2d.Y() );
        }
      ResPoints.Append( P3d );
      ResParamsOnE1.Append( Inter2d.Point(i).ParamOnFirst() );
      ResParamsOnE2.Append( Inter2d.Point(i).ParamOnSecond() );
    }
  
  for (i = 1; i <= ResPoints.Length(); i++)
    {
      Standard_Real aT1 = ResParamsOnE1(i); //ponc1.Parameter();
      Standard_Real aT2 = ResParamsOnE2(i); //ponc2.Parameter();
      if (Precision::IsInfinite(aT1) || Precision::IsInfinite(aT2))
        {
#ifdef OCCT_DEBUG
          cout << "Inter2d : Solution rejected due to infinite parameter"<<endl;
#endif
          continue;
        }
      
      gp_Pnt P = ResPoints(i); //ponc1.Value();
      TopoDS_Vertex aNewVertex = BRepLib_MakeVertex(P);
      aNewVertex.Orientation(TopAbs_INTERNAL);
      B.UpdateVertex( aNewVertex, aT1, E1, Tol );
      B.UpdateVertex( aNewVertex, aT2, E2, Tol );
      gp_Pnt P1 = CE1.Value(aT1);
      gp_Pnt P2 = CE2.Value(aT2);
      Standard_Real dist1, dist2, dist3;
      dist1 = P1.Distance(P);
      dist2 = P2.Distance(P);
      dist3 = P1.Distance(P2);
      dist1 = Max( dist1, dist2 );
      dist1 = Max( dist1, dist3 );
      B.UpdateVertex( aNewVertex, dist1 );
      
#ifdef OCCT_DEBUG
      if (aT1 < f[1]-Tol  || aT1 > l[1]+Tol)
        {
          cout << "out of limit"<<endl;
          cout<<"aT1 = "<<aT1<<", f[1] = "<<f[1]<<", l[1] = "<<l[1]<<endl;
        }
      if (aT2 < f[2]-Tol  || aT2 > l[2]+Tol)
        {
          cout << "out of limit"<<endl;
          cout<<"aT2 = "<<aT2<<", f[2] = "<<f[2]<<", l[2] = "<<l[2]<<endl;
        }
      Standard_Real MilTol2 = 1000*Tol*Tol;
      if (P1.SquareDistance(P) >  MilTol2 || P2.SquareDistance(P) > MilTol2 || P1.Distance(P2) > 2.*Tol)
        {
          cout << "Inter2d : Solution rejected"<<endl;
          cout<<"P  = "<<P.X()<<" "<<P.Y()<<" "<<P.Z()<<endl;
          cout<<"P1 = "<<P1.X()<<" "<<P1.Y()<<" "<<P1.Z()<<endl;
          cout<<"P2 = "<<P2.X()<<" "<<P2.Y()<<" "<<P2.Z()<<endl;
          cout<<"MaxDist = "<<dist1<<endl;
        }
#endif
      //define the orientation of a new vertex
      TopAbs_Orientation OO1 = TopAbs_REVERSED;
      TopAbs_Orientation OO2 = TopAbs_REVERSED;
      if (WithOri)
        {
          BRepAdaptor_Curve2d PCE1( E1, F );
          BRepAdaptor_Curve2d PCE2( E2, F );
          gp_Pnt2d P2d1, P2d2;
          gp_Vec2d V1, V2, V1or, V2or;
          PCE1.D1( aT1, P2d1, V1 );
          PCE2.D1( aT2, P2d2, V2 );
          V1or = V1; V2or = V2;
          if (E1.Orientation() == TopAbs_REVERSED) V1or.Reverse();
          if (E2.Orientation() == TopAbs_REVERSED) V2or.Reverse();
          Standard_Real CrossProd = V2or ^ V1;
#ifdef OCCT_DEBUG
          if (Abs(CrossProd) <= gp::Resolution())
            cout<<endl<<"CrossProd = "<<CrossProd<<endl;
#endif
          if (CrossProd > 0.)
            OO1 = TopAbs_FORWARD;
          CrossProd = V1or ^ V2;
          if (CrossProd > 0.)
            OO2 = TopAbs_FORWARD;
        }
      LV1.Append( aNewVertex.Oriented(OO1) );
      LV2.Append( aNewVertex.Oriented(OO2) );
    }
  
  //----------------------------------
  // Test at end.
  //---------------------------------
  Standard_Real U1,U2;
  Standard_Real TolConf = Tol;
  TopoDS_Vertex V1[2],V2[2];
  TopExp::Vertices(E1,V1[0],V1[1]);
  TopExp::Vertices(E2,V2[0],V2[1]);

  Standard_Integer j;
  for (j = 0; j < 2; j++) {
    if (V1[j].IsNull()) continue;
    for (Standard_Integer k = 0; k < 2; k++) {
      if (V2[k].IsNull()) continue;
      if (V1[j].IsSame(V2[k])) {
        if (AsDes->HasAscendant(V1[j])) {
          continue;
        }
      }
      //
      gp_Pnt P1 = BRep_Tool::Pnt(V1[j]);
      gp_Pnt P2 = BRep_Tool::Pnt(V2[k]);
      Standard_Real Dist = P1.Distance(P2); 
      if (Dist < TolConf) {
        TopoDS_Vertex V = BRepLib_MakeVertex(P1);
        U1 = (j == 0) ? f[1] : l[1];
        U2 = (k == 0) ? f[2] : l[2];
        TopoDS_Shape aLocalShape = V.Oriented(TopAbs_INTERNAL);
        B.UpdateVertex(TopoDS::Vertex(aLocalShape),U1,E1,Tol);
        B.UpdateVertex(TopoDS::Vertex(aLocalShape),U2,E2,Tol);
        LV1.Prepend(V.Oriented(V1[j].Orientation()));
        LV2.Prepend(V.Oriented(V2[k].Orientation()));
      }
    }
  }

  Standard_Boolean AffichPurge = Standard_False;

  if ( !LV1.IsEmpty()) {
    //----------------------------------
    // Remove all vertices.
    // there can be doubles
    //----------------------------------
    TopTools_ListIteratorOfListOfShape it1LV1,it1LV2,it2LV1;
    gp_Pnt P1,P2;
    Standard_Boolean Purge = Standard_True;

    while (Purge) {
      i = 1;
      Purge = Standard_False;
      for (it1LV1.Initialize(LV1),it1LV2.Initialize(LV2); 
           it1LV1.More(); it1LV1.Next(),it1LV2.Next()) {
        j = 1;
        it2LV1.Initialize(LV1);
        while (j < i) {      
          P1 = BRep_Tool::Pnt(TopoDS::Vertex(it1LV1.Value()));
          P2 = BRep_Tool::Pnt(TopoDS::Vertex(it2LV1.Value()));
          if (P1.IsEqual(P2,10*Tol)) {
            LV1.Remove(it1LV1);
            LV2.Remove(it1LV2);
            if (AffichPurge) cout <<"Doubles removed in EdgeInter."<<endl;
            Purge = Standard_True;
            break;
          }
          j++;
          it2LV1.Next();
        }
        if (Purge) break;
        i++;
      }
    }
    //---------------------------------
    // Vertex storage in SD.
    //---------------------------------
////-----------------------------------------------------
    if(LV1.Extent() > 1) {
      //cout << "IFV - RefEdgeInter: remove vertex" << endl;
      Standard_Real dmin = RealLast();
      TopoDS_Vertex Vmin;
      for (it1LV1.Initialize(LV1); it1LV1.More(); it1LV1.Next()) {
        gp_Pnt P = BRep_Tool::Pnt(TopoDS::Vertex(it1LV1.Value()));
        Standard_Real d = P.SquareDistance(Pref);
        if(d < dmin) {
          dmin = d;
          Vmin = TopoDS::Vertex(it1LV1.Value());
        }
      }
      for (it1LV1.Initialize(LV1),it1LV2.Initialize(LV2); 
           it1LV1.More(); it1LV1.Next(),it1LV2.Next()) {
        if(!Vmin.IsSame(it1LV1.Value())) {
          LV1.Remove(it1LV1);
          LV2.Remove(it1LV2);
          if(!it1LV1.More()) break;
        }
      }
    }
      
////-----------------------------------------------------
    Standard_Real TolStore = BRep_Tool::Tolerance(E1) + BRep_Tool::Tolerance(E2);
    TolStore = Max(TolStore, 10.*Tol);
    Store (E1,E2,LV1,LV2,TolStore,AsDes, aDMVV);
  }
}


//======================================================================
//function : EvaluateMaxSegment
//purpose  : return MaxSegment to pass in approximation
//======================================================================

static Standard_Integer evaluateMaxSegment(const Adaptor3d_CurveOnSurface& aCurveOnSurface)
{
  Handle(Adaptor3d_HSurface) aSurf   = aCurveOnSurface.GetSurface();
  Handle(Adaptor2d_HCurve2d) aCurv2d = aCurveOnSurface.GetCurve();

  Standard_Real aNbSKnots = 0, aNbC2dKnots = 0;
  
  if (aSurf->GetType() == GeomAbs_BSplineSurface) {
    Handle(Geom_BSplineSurface) aBSpline = aSurf->BSpline();
    aNbSKnots = Max(aBSpline->NbUKnots(), aBSpline->NbVKnots());
  }
  if (aCurv2d->GetType() == GeomAbs_BSplineCurve) {
    aNbC2dKnots = aCurv2d->NbKnots();
  }
  Standard_Integer aReturn = (Standard_Integer) (  30 + Max(aNbSKnots, aNbC2dKnots) ) ;
  return aReturn;
}


//=======================================================================
//function : ExtendPCurve
//purpose  : 
//=======================================================================

static Standard_Boolean ExtendPCurve(const Handle(Geom2d_Curve)& aPCurve,
                                     const Standard_Real anEf,
                                     const Standard_Real anEl,
                                     const Standard_Real a2Offset,
                                     Handle(Geom2d_Curve)& NewPCurve)
{
  NewPCurve = aPCurve;
  if (NewPCurve->IsInstance(STANDARD_TYPE(Geom2d_TrimmedCurve)))
    NewPCurve = Handle(Geom2d_TrimmedCurve)::DownCast (NewPCurve)->BasisCurve();
  
  Standard_Real FirstPar = NewPCurve->FirstParameter();
  Standard_Real LastPar  = NewPCurve->LastParameter();

  if (NewPCurve->IsKind(STANDARD_TYPE(Geom2d_BoundedCurve)) &&
      (FirstPar > anEf - a2Offset || LastPar < anEl + a2Offset))
    {
      if (NewPCurve->IsInstance(STANDARD_TYPE(Geom2d_BezierCurve)))
        {
          Handle(Geom2d_BezierCurve) aBezier = Handle(Geom2d_BezierCurve)::DownCast (NewPCurve);
          if (aBezier->NbPoles() == 2)
            {
              TColgp_Array1OfPnt2d thePoles(1,2);
              aBezier->Poles(thePoles);
              gp_Vec2d aVec(thePoles(1), thePoles(2));
              NewPCurve = new Geom2d_Line(thePoles(1), aVec);
              return Standard_True;
            }
        }
      else if (NewPCurve->IsInstance(STANDARD_TYPE(Geom2d_BSplineCurve)))
        {
          Handle(Geom2d_BSplineCurve) aBSpline = Handle(Geom2d_BSplineCurve)::DownCast (NewPCurve);
          if (aBSpline->NbKnots() == 2 && aBSpline->NbPoles() == 2)
            {
              TColgp_Array1OfPnt2d thePoles(1,2);
              aBSpline->Poles(thePoles);
              gp_Vec2d aVec(thePoles(1), thePoles(2));
              NewPCurve = new Geom2d_Line(thePoles(1), aVec);
              return Standard_True;
            }
        }
    }

  FirstPar = aPCurve->FirstParameter();
  LastPar  = aPCurve->LastParameter();
  Handle(Geom2d_TrimmedCurve) aTrCurve = 
    new Geom2d_TrimmedCurve(aPCurve, FirstPar, LastPar);
  
  // The curve is not prolonged on begin or end.
  // Trying to prolong it adding a segment to its bound.
  gp_Pnt2d                              aPBnd;
  gp_Vec2d                              aVBnd;
  gp_Pnt2d                              aPBeg;
  gp_Dir2d                              aDBnd;
  Handle(Geom2d_Line)                   aLin;
  Handle(Geom2d_TrimmedCurve)           aSegment;
  Geom2dConvert_CompCurveToBSplineCurve aCompCurve(aTrCurve, Convert_RationalC1);
  Standard_Real                         aTol = Precision::Confusion();
  Standard_Real                         aDelta = Max(a2Offset, 1.);
  
  if (FirstPar > anEf - a2Offset) {
    aPCurve->D1(FirstPar, aPBnd, aVBnd);
    aDBnd.SetXY(aVBnd.XY());
    aPBeg    = aPBnd.Translated(gp_Vec2d(-aDelta*aDBnd.XY()));
    aLin     = new Geom2d_Line(aPBeg, aDBnd);
    aSegment = new Geom2d_TrimmedCurve(aLin, 0, aDelta);
    
    if (!aCompCurve.Add(aSegment, aTol))
      return Standard_False;
  }
  
  if (LastPar < anEl + a2Offset) {
    aPCurve->D1(LastPar, aPBeg, aVBnd);
    aDBnd.SetXY(aVBnd.XY());
    aLin     = new Geom2d_Line(aPBeg, aDBnd);
    aSegment = new Geom2d_TrimmedCurve(aLin, 0, aDelta);
    
    if (!aCompCurve.Add(aSegment, aTol))
      return Standard_False;
  }
  
  NewPCurve  = aCompCurve.BSplineCurve();
  return Standard_True;
}

//=======================================================================
//function : ExtentEdge
//purpose  : 
//=======================================================================

//  Modified by skv - Fri Dec 26 17:00:55 2003 OCC4455 Begin
//static void ExtentEdge(const TopoDS_Edge& E,TopoDS_Edge& NE) 
void BRepOffset_Inter2d::ExtentEdge(const TopoDS_Edge& E,TopoDS_Edge& NE, const Standard_Real theOffset)
{
  //BRepLib::BuildCurve3d(E);

  TopoDS_Shape  aLocalShape = E.EmptyCopied();
  Standard_Real anEf;
  Standard_Real anEl;
  Standard_Real a2Offset = 2.*Abs(theOffset);
  BRep_Builder BB;
  Standard_Integer i, j;

  BRep_Tool::Range(E, anEf, anEl);
  NE = TopoDS::Edge(aLocalShape); 
//  NE = TopoDS::Edge(E.EmptyCopied()); 
  // Enough for analytic edges, for general case reconstruct the
  // geometry of the edge recalculating the intersection of surfaces.

  //BRepLib::BuildCurve3d(E);

  Standard_Integer NbPCurves = 0;
  Standard_Real FirstParOnPC = RealFirst(), LastParOnPC = RealLast();
  Handle(Geom2d_Curve) MinPC;
  Handle(Geom_Surface) MinSurf;
  TopLoc_Location      MinLoc;

  BRep_ListIteratorOfListOfCurveRepresentation itr( (Handle(BRep_TEdge)::DownCast(NE.TShape()))->ChangeCurves() );
  for (; itr.More(); itr.Next())
    {
      Handle( BRep_CurveRepresentation ) CurveRep = itr.Value();
      Standard_Real FirstPar, LastPar;
      if (CurveRep->IsCurveOnSurface())
        {
          NbPCurves++;
          Handle(Geom2d_Curve) theCurve = CurveRep->PCurve();
          FirstPar = theCurve->FirstParameter();
          LastPar  = theCurve->LastParameter();

          if (theCurve->IsKind(STANDARD_TYPE(Geom2d_BoundedCurve)) &&
              (FirstPar > anEf - a2Offset || LastPar < anEl + a2Offset))
            {
              Handle(Geom2d_Curve) NewPCurve;
              if (ExtendPCurve(theCurve, anEf, anEl, a2Offset, NewPCurve))
                {
                  CurveRep->PCurve(NewPCurve);
                  FirstPar = NewPCurve->FirstParameter();
                  LastPar  = NewPCurve->LastParameter();
                  if (CurveRep->IsCurveOnClosedSurface())
                    {
                      Handle(Geom2d_Curve) PCurve2 = CurveRep->PCurve2();
                      if (ExtendPCurve(PCurve2, anEf, anEl, a2Offset, NewPCurve))
                        CurveRep->PCurve2(NewPCurve);
                    }
                }
            }
          else if (theCurve->IsPeriodic())
            {
              Standard_Real delta = (theCurve->Period() - (anEl - anEf))*0.5;
              delta *= 0.95;
              FirstPar = anEf - delta;
              LastPar  = anEl + delta;
            }
          else if (theCurve->IsClosed())
            LastPar -= 0.05*(LastPar - FirstPar);

          //check FirstPar and LastPar: the pcurve should be in its surface
          theCurve = CurveRep->PCurve();
          Handle(Geom_Surface) theSurf = CurveRep->Surface();
          Standard_Real Umin, Umax, Vmin, Vmax;
          theSurf->Bounds(Umin, Umax, Vmin, Vmax);
          TColGeom2d_SequenceOfCurve BoundLines;
          if (!Precision::IsInfinite(Vmin))
            {
              Handle(Geom2d_Line) aLine = new Geom2d_Line(gp_Pnt2d( 0., Vmin ),
                                                          gp_Dir2d( 1., 0. ));
              BoundLines.Append(aLine);
            }
          if (!Precision::IsInfinite(Umin))
            {
              Handle(Geom2d_Line) aLine = new Geom2d_Line(gp_Pnt2d( Umin, 0. ),
                                                          gp_Dir2d( 0., 1. ));
              BoundLines.Append(aLine);
            }
          if (!Precision::IsInfinite(Vmax))
            {
              Handle(Geom2d_Line) aLine = new Geom2d_Line(gp_Pnt2d( 0., Vmax ),
                                                          gp_Dir2d( 1., 0. ));
              BoundLines.Append(aLine);
            }
          if (!Precision::IsInfinite(Umax))
            {
              Handle(Geom2d_Line) aLine = new Geom2d_Line(gp_Pnt2d( Umax, 0. ),
                                                          gp_Dir2d( 0., 1. ));
              BoundLines.Append(aLine);
            }

          TColStd_SequenceOfReal params;
          Geom2dInt_GInter IntCC;
          Geom2dAdaptor_Curve GAcurve(theCurve);
          for (i = 1; i <= BoundLines.Length(); i++)
            {
              Geom2dAdaptor_Curve GAline( BoundLines(i) );
              IntCC.Perform( GAcurve, GAline, Precision::PConfusion(), Precision::PConfusion());
              if (IntCC.IsDone())
                {
                  for (j = 1; j <= IntCC.NbPoints(); j++)
                    {
                      const IntRes2d_IntersectionPoint& ip = IntCC.Point(j);
                      gp_Pnt2d aPoint = ip.Value();
                      if (aPoint.X() >= Umin && aPoint.X() <= Umax &&
                          aPoint.Y() >= Vmin && aPoint.Y() <= Vmax)
                        params.Append( ip.ParamOnFirst() );
                    }
                  for (j = 1; j <= IntCC.NbSegments(); j++)
                    {
                      const IntRes2d_IntersectionSegment& is = IntCC.Segment(j);
                      if (is.HasFirstPoint())
                        {
                          const IntRes2d_IntersectionPoint& ip = is.FirstPoint();
                          gp_Pnt2d aPoint = ip.Value();
                          if (aPoint.X() >= Umin && aPoint.X() <= Umax &&
                              aPoint.Y() >= Vmin && aPoint.Y() <= Vmax)
                            params.Append( ip.ParamOnFirst() );
                        }
                      if (is.HasLastPoint())
                        {
                          const IntRes2d_IntersectionPoint& ip = is.LastPoint();
                          gp_Pnt2d aPoint = ip.Value();
                          if (aPoint.X() >= Umin && aPoint.X() <= Umax &&
                              aPoint.Y() >= Vmin && aPoint.Y() <= Vmax)
                            params.Append( ip.ParamOnFirst() );
                        }
                    }
                }
            }
          if (!params.IsEmpty())
            {
              if (params.Length() == 1)
                {
                  gp_Pnt2d PntFirst = theCurve->Value(FirstPar);
                  if (PntFirst.X() >= Umin && PntFirst.X() <= Umax &&
                      PntFirst.Y() >= Vmin && PntFirst.Y() <= Vmax)
                    {
                      if (LastPar > params(1))
                        LastPar = params(1);
                    }
                  else if (FirstPar < params(1))
                    FirstPar = params(1);
                }
              else
                {
                  Standard_Real fpar = RealLast(), lpar = RealFirst();
                  for (i = 1; i <= params.Length(); i++)
                    {
                      if (params(i) < fpar)
                        fpar = params(i);
                      if (params(i) > lpar)
                        lpar = params(i);
                    }
                  if (FirstPar < fpar)
                    FirstPar = fpar;
                  if (LastPar > lpar)
                    LastPar = lpar;
                }
            }
          //// end of check ////
          (Handle(BRep_GCurve)::DownCast(CurveRep))->SetRange( FirstPar, LastPar );
          //gp_Pnt2d Pfirst = theCurve->Value(FirstPar);
          //gp_Pnt2d Plast  = theCurve->Value(LastPar);
          //(Handle(BRep_CurveOnSurface)::DownCast(CurveRep))->SetUVPoints( Pfirst, Plast );

          //update FirstParOnPC and LastParOnPC
          if (FirstPar > FirstParOnPC)
            {
              FirstParOnPC = FirstPar;
              MinPC   = theCurve;
              MinSurf = theSurf;
              MinLoc  = CurveRep->Location();
            }
          if (LastPar  < LastParOnPC)
            {
              LastParOnPC  = LastPar;
              MinPC   = theCurve;
              MinSurf = theSurf;
              MinLoc  = CurveRep->Location();
            }
        }
    }

  Standard_Real f, l;
  Handle(Geom_Curve) C3d = BRep_Tool::Curve( NE, f, l );
  if (NbPCurves)
    {
      MinLoc = E.Location() * MinLoc;
      if (!C3d.IsNull())
        {
          if (MinPC->IsClosed())
            {
              f = FirstParOnPC;
              l = LastParOnPC;
            }
          else if (C3d->IsPeriodic())
            {
              Standard_Real delta = (C3d->Period() - (l - f))*0.5;
              delta *= 0.95;
              f -= delta;
              l += delta;
            }
          else if (C3d->IsClosed())
            l -= 0.05*(l - f);
          else
            {
              f = FirstParOnPC;
              l = LastParOnPC;
              GeomAPI_ProjectPointOnCurve Projector;
              if (!Precision::IsInfinite(FirstParOnPC))
                {
                  gp_Pnt2d P2d1 = MinPC->Value(FirstParOnPC);
                  gp_Pnt P1 = MinSurf->Value( P2d1.X(), P2d1.Y() );
                  P1.Transform(MinLoc.Transformation());
                  Projector.Init( P1, C3d );
                  if (Projector.NbPoints() > 0)
                    f = Projector.LowerDistanceParameter();
#ifdef OCCT_DEBUG
                  else
                    cout<<"ProjectPointOnCurve not done"<<endl;
#endif
                }
              if (!Precision::IsInfinite(LastParOnPC))
                {
                  gp_Pnt2d P2d2 = MinPC->Value(LastParOnPC);
                  gp_Pnt P2 = MinSurf->Value( P2d2.X(), P2d2.Y() );
                  P2.Transform(MinLoc.Transformation());
                  Projector.Init( P2, C3d );
                  if (Projector.NbPoints() > 0)
                    l = Projector.LowerDistanceParameter();
#ifdef OCCT_DEBUG
                  else
                    cout<<"ProjectPointOnCurve not done"<<endl;
#endif
                }
            }
          BB.Range( NE, f, l );
          if (!Precision::IsInfinite(f) && !Precision::IsInfinite(l))
            BRepLib::SameParameter( NE, Precision::Confusion(), Standard_True );
        }
      else if (!BRep_Tool::Degenerated(E)) //no 3d curve
        {
          MinSurf = Handle(Geom_Surface)::DownCast
            (MinSurf->Transformed(MinLoc.Transformation()));
          Standard_Real max_deviation = 0.;
          if (Precision::IsInfinite(FirstParOnPC) || Precision::IsInfinite(LastParOnPC))
            {
              if (MinPC->IsInstance(STANDARD_TYPE(Geom2d_Line)))
                {
                  Standard_Boolean IsLine = Standard_False;
                  if (MinSurf->IsInstance(STANDARD_TYPE(Geom_Plane)))
                    IsLine = Standard_True;
                  else if (MinSurf->IsInstance(STANDARD_TYPE(Geom_CylindricalSurface)) ||
                           MinSurf->IsInstance(STANDARD_TYPE(Geom_ConicalSurface)))
                    {
                      Handle(Geom2d_Line) theLine = Handle(Geom2d_Line)::DownCast (MinPC);
                      gp_Dir2d LineDir = theLine->Direction();
                      if (LineDir.IsParallel( gp::DY2d(), Precision::Angular() ))
                        IsLine = Standard_True;
                    }
                  if (IsLine)
                    {
                      gp_Pnt2d P2d1 = MinPC->Value(0.), P2d2 = MinPC->Value(1.);
                      gp_Pnt P1 = MinSurf->Value(P2d1.X(), P2d1.Y());
                      gp_Pnt P2 = MinSurf->Value(P2d2.X(), P2d2.Y());
                      gp_Vec aVec(P1, P2);
                      C3d = new Geom_Line( P1, aVec );
                    }
                }
            }
          else
            {
              Geom2dAdaptor_Curve AC2d( MinPC, FirstParOnPC, LastParOnPC );
              GeomAdaptor_Surface GAsurf( MinSurf );
              Handle(Geom2dAdaptor_HCurve) HC2d  = new Geom2dAdaptor_HCurve( AC2d );
              Handle(GeomAdaptor_HSurface) HSurf = new GeomAdaptor_HSurface( GAsurf );
              Adaptor3d_CurveOnSurface ConS( HC2d, HSurf );
              Standard_Real /*max_deviation,*/ average_deviation;
              GeomAbs_Shape Continuity = GeomAbs_C1;
              Standard_Integer MaxDegree = 14;
              Standard_Integer MaxSegment = evaluateMaxSegment(ConS);
              GeomLib::BuildCurve3d(Precision::Confusion(),
                                    ConS, FirstParOnPC, LastParOnPC,
                                    C3d, max_deviation, average_deviation,
                                    Continuity, MaxDegree, MaxSegment);
            }
          BB.UpdateEdge( NE, C3d, max_deviation );
          //BB.Range( NE, FirstParOnPC, LastParOnPC );
          Standard_Boolean ProjectionSuccess = Standard_True;
          if (NbPCurves > 1)
            //BRepLib::SameParameter( NE, Precision::Confusion(), Standard_True );
            for (itr.Initialize((Handle(BRep_TEdge)::DownCast(NE.TShape()))->ChangeCurves());
                 itr.More();
                 itr.Next())
              {
                Handle( BRep_CurveRepresentation ) CurveRep = itr.Value();
                Standard_Real FirstPar, LastPar;
                if (CurveRep->IsCurveOnSurface())
                  {
                    Handle(Geom2d_Curve) theCurve = CurveRep->PCurve();
                    Handle(Geom_Surface) theSurf  = CurveRep->Surface();
                    TopLoc_Location      theLoc   = CurveRep->Location();
                    if (theCurve == MinPC && theSurf == MinSurf && theLoc == MinLoc)
                      continue;
                    FirstPar = (Handle(BRep_GCurve)::DownCast(CurveRep))->First();
                    LastPar  = (Handle(BRep_GCurve)::DownCast(CurveRep))->Last();
                    if (Abs(FirstPar - FirstParOnPC) > Precision::PConfusion() ||
                        Abs(LastPar  - LastParOnPC)  > Precision::PConfusion())
                      {
                        theLoc = E.Location() * theLoc;
                        theSurf = Handle(Geom_Surface)::DownCast
                          (theSurf->Transformed(theLoc.Transformation()));

                        if (theCurve->IsInstance(STANDARD_TYPE(Geom2d_Line)) &&
                            theSurf->IsKind(STANDARD_TYPE(Geom_BoundedSurface)))
                          {
                            gp_Dir2d theDir = Handle(Geom2d_Line)::DownCast (theCurve)->Direction();
                            if (theDir.IsParallel(gp::DX2d(), Precision::Angular()) ||
                                theDir.IsParallel(gp::DY2d(), Precision::Angular()))
                              {
                                Standard_Real U1, U2, V1, V2;
                                theSurf->Bounds(U1, U2, V1, V2);
                                gp_Pnt2d Origin = Handle(Geom2d_Line)::DownCast (theCurve)->Location();
                                if (Abs(Origin.X()-U1) <= Precision::Confusion() ||
                                    Abs(Origin.X()-U2) <= Precision::Confusion() ||
                                    Abs(Origin.Y()-V1) <= Precision::Confusion() ||
                                    Abs(Origin.Y()-V2) <= Precision::Confusion())
                                  {
                                    BRepLib::SameParameter( NE, Precision::Confusion(), Standard_True );
                                    break;
                                  }
                              }
                          }

                        Handle(Geom2d_Curve) ProjPCurve =
                          GeomProjLib::Curve2d( C3d, FirstParOnPC, LastParOnPC, theSurf );
                        if (ProjPCurve.IsNull())
                          ProjectionSuccess = Standard_False;
                        else
                          CurveRep->PCurve( ProjPCurve );
                      }
                  }
              }
          if (ProjectionSuccess)
            BB.Range( NE, FirstParOnPC, LastParOnPC );
          else
            {
              BB.Range( NE, FirstParOnPC, LastParOnPC, Standard_True );
              BRepLib::SameParameter( NE, Precision::Confusion(), Standard_True );
            }
        }
    }
  else //no pcurves
    {
      Standard_Real FirstPar = C3d->FirstParameter();
      Standard_Real LastPar  = C3d->LastParameter();
      
      if (C3d->IsKind(STANDARD_TYPE(Geom_BoundedCurve)) &&
          (FirstPar > anEf - a2Offset || LastPar < anEl + a2Offset))
        {
          Handle(Geom_TrimmedCurve) aTrCurve = 
            new Geom_TrimmedCurve(C3d, FirstPar, LastPar);
          
          // The curve is not prolonged on begin or end.
          // Trying to prolong it adding a segment to its bound.
          gp_Pnt                              aPBnd;
          gp_Vec                              aVBnd;
          gp_Pnt                              aPBeg;
          gp_Dir                              aDBnd;
          Handle(Geom_Line)                   aLin;
          Handle(Geom_TrimmedCurve)           aSegment;
          GeomConvert_CompCurveToBSplineCurve aCompCurve(aTrCurve, Convert_RationalC1);
          Standard_Real                       aTol = Precision::Confusion();
          Standard_Real                       aDelta = Max(a2Offset, 1.);
          
          if (FirstPar > anEf - a2Offset) {
            C3d->D1(FirstPar, aPBnd, aVBnd);
            aDBnd.SetXYZ(aVBnd.XYZ());
            aPBeg    = aPBnd.Translated(gp_Vec(-aDelta*aDBnd.XYZ()));
            aLin     = new Geom_Line(aPBeg, aDBnd);
            aSegment = new Geom_TrimmedCurve(aLin, 0, aDelta);
            
            if (!aCompCurve.Add(aSegment, aTol))
              return;
          }
          
          if (LastPar < anEl + a2Offset) {
            C3d->D1(LastPar, aPBeg, aVBnd);
            aDBnd.SetXYZ(aVBnd.XYZ());
            aLin     = new Geom_Line(aPBeg, aDBnd);
            aSegment = new Geom_TrimmedCurve(aLin, 0, aDelta);
            
            if (!aCompCurve.Add(aSegment, aTol))
              return;
          }
          
          C3d = aCompCurve.BSplineCurve();
          FirstPar = C3d->FirstParameter();
          LastPar  = C3d->LastParameter();
          BB.UpdateEdge(NE, C3d, Precision::Confusion());
        }
      else if (C3d->IsPeriodic())
        {
          Standard_Real delta = (C3d->Period() - (anEl - anEf))*0.5;
          delta *= 0.95;
          FirstPar = anEf - delta;
          LastPar  = anEl + delta;
        }
      else if (C3d->IsClosed())
        LastPar -= 0.05*(LastPar - FirstPar);
      
      BB.Range( NE, FirstPar, LastPar );
    }
}
//  Modified by skv - Fri Dec 26 17:00:57 2003 OCC4455 End


//=======================================================================
//function : UpdateVertex
//purpose  : 
//=======================================================================

static Standard_Boolean  UpdateVertex(TopoDS_Vertex V,
                                      TopoDS_Edge&  OE,
                                      TopoDS_Edge&  NE,
                                      Standard_Real TolConf)
{
  BRepAdaptor_Curve OC(OE);
  BRepAdaptor_Curve NC(NE);
  Standard_Real Of = OC.FirstParameter(); Standard_Real Ol = OC.LastParameter();
  Standard_Real Nf = NC.FirstParameter(); Standard_Real Nl = NC.LastParameter();
  Standard_Real U = 0.;
  Standard_Real ParTol = Precision::PConfusion();
  gp_Pnt           P  = BRep_Tool::Pnt(V);
  Standard_Boolean OK = Standard_False;

  if (P.Distance(OC.Value(Of)) < TolConf) {
    if (Of >= Nf + ParTol && Of <= Nl + ParTol  && P.Distance(NC.Value(Of)) < TolConf) {
      OK = Standard_True;
      U    = Of;
    }
  }
  if (P.Distance(OC.Value(Ol)) < TolConf) {
    if (Ol >= Nf + ParTol && Ol <= Nl + ParTol  && P.Distance(NC.Value(Ol)) < TolConf) {
      OK = Standard_True;
      U    = Ol;
    }
  }
  if (OK) {
    BRep_Builder B;
    TopoDS_Shape aLocalShape = NE.Oriented(TopAbs_FORWARD);
    TopoDS_Edge EE = TopoDS::Edge(aLocalShape);
//    TopoDS_Edge EE = TopoDS::Edge(NE.Oriented(TopAbs_FORWARD));
    aLocalShape = V.Oriented(TopAbs_INTERNAL);
    B.UpdateVertex(TopoDS::Vertex(aLocalShape),
                   U,NE,BRep_Tool::Tolerance(NE));
//    B.UpdateVertex(TopoDS::Vertex(V.Oriented(TopAbs_INTERNAL)),
//                   U,NE,BRep_Tool::Tolerance(NE));
  }
  return OK;  
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void BRepOffset_Inter2d::Compute (const Handle(BRepAlgo_AsDes)&     AsDes,
                                  const TopoDS_Face&                F,
                                  const TopTools_IndexedMapOfShape& NewEdges,
                                  const Standard_Real               Tol,
                                  TopTools_IndexedDataMapOfShapeListOfShape& theDMVV)
{
#ifdef DRAW
  NbF2d++;
  NbE2d = 0;
#endif 

  //Do not intersect the edges of face
  TopTools_MapOfShape EdgesOfFace;
  TopExp_Explorer Explo( F, TopAbs_EDGE );
  for (; Explo.More(); Explo.Next())
    EdgesOfFace.Add( Explo.Current() );

  //-----------------------------------------------------------
  // calculate intersections2d on faces touched by  
  // intersection3d
  //---------------------------------------------------------
  TopTools_ListIteratorOfListOfShape it1LE ;    
  TopTools_ListIteratorOfListOfShape it2LE ;  

  //-----------------------------------------------
  // Intersection of edges 2*2.
  //-----------------------------------------------
  const TopTools_ListOfShape&        LE = AsDes->Descendant(F);
  TopoDS_Vertex                      V1,V2;
  Standard_Integer                   j, i = 1;
  BRepAdaptor_Surface BAsurf(F);
  //
  for ( it1LE.Initialize(LE) ; it1LE.More(); it1LE.Next()) {
    const TopoDS_Edge& E1 = TopoDS::Edge(it1LE.Value());        
    j = 1;
    it2LE.Initialize(LE);
    
    while (j < i && it2LE.More()) {
      const TopoDS_Edge& E2 = TopoDS::Edge(it2LE.Value());
      //--------------------------------------------------------------
      // Intersections of New edges obtained by intersection
      // between them and with edges of restrictions
      //------------------------------------------------------
      if ( (!EdgesOfFace.Contains(E1) || !EdgesOfFace.Contains(E2)) &&
           (NewEdges.Contains(E1) || NewEdges.Contains(E2)) ) {
        TopoDS_Shape aLocalShape = F.Oriented(TopAbs_FORWARD);
        EdgeInter(TopoDS::Face(aLocalShape),BAsurf,E1,E2,AsDes,Tol,Standard_True, theDMVV);
//          EdgeInter(TopoDS::Face(F.Oriented(TopAbs_FORWARD)),E1,E2,AsDes,Tol,Standard_True);
      }
      it2LE.Next();
      j++;
    }
    i++;
  }
}

//=======================================================================
//function : ConnexIntByInt
//purpose  : 
//=======================================================================
void BRepOffset_Inter2d::ConnexIntByInt
 (const TopoDS_Face&            FI,
  BRepOffset_Offset&            OFI,
  TopTools_DataMapOfShapeShape& MES,
  const TopTools_DataMapOfShapeShape& Build,
  const Handle(BRepAlgo_AsDes)& AsDes2d,
  const Standard_Real           Offset,
  const Standard_Real           Tol,
  TopTools_IndexedMapOfShape&   FacesWithVerts,
  TopTools_IndexedDataMapOfShapeListOfShape& theDMVV)
{  

  TopTools_DataMapOfShapeListOfShape MVE;
  BRepOffset_Tool::MapVertexEdges(FI,MVE);

  //---------------------
  // Extension of edges.
  //---------------------
  TopoDS_Edge  NE;
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape it(MVE);
  for  ( ; it.More(); it.Next()) {
    const TopTools_ListOfShape&  L = it.Value();
    Standard_Boolean   YaBuild = 0;
    TopTools_ListIteratorOfListOfShape itL(L);
    for (; itL.More(); itL.Next()) {
      YaBuild = Build.IsBound(itL.Value());
      if (YaBuild) break;
    }
    if (YaBuild) {
      for (itL.Initialize(L); itL.More(); itL.Next()) {
        const TopoDS_Edge& EI = TopoDS::Edge(itL.Value());
        TopoDS_Shape aLocalShape = OFI.Generated(EI);
        const TopoDS_Edge& OE = TopoDS::Edge(aLocalShape);
        if (!MES.IsBound(OE) && !Build.IsBound(EI)) {
          ExtentEdge(OE,NE, Offset);
          MES.Bind  (OE,NE);
        }
      }
    } 
  }
  
  TopoDS_Face           FIO = TopoDS::Face(OFI.Face());
  if (MES.IsBound(FIO)) FIO = TopoDS::Face(MES(FIO));
  //
  BRepAdaptor_Surface BAsurf(FIO);
  
  TopExp_Explorer exp(FI.Oriented(TopAbs_FORWARD),TopAbs_WIRE);
  for (; exp.More(); exp.Next()) {
    const TopoDS_Wire&     W = TopoDS::Wire(exp.Current());
    BRepTools_WireExplorer wexp;
    Standard_Boolean       end = Standard_False ;
    TopoDS_Edge            FirstE,CurE,NextE;

    TopoDS_Shape aLocalWire = W .Oriented(TopAbs_FORWARD);
    TopoDS_Shape aLocalFace = FI.Oriented(TopAbs_FORWARD);
    wexp.Init(TopoDS::Wire(aLocalWire),TopoDS::Face(aLocalFace));
    if (!wexp.More())
      continue; // Protection from case when explorer does not contain edges.
    CurE = FirstE  = wexp.Current();
    TopTools_IndexedMapOfShape Edges;
    Standard_Boolean ToReverse1, ToReverse2;
    ToReverse1 = IsOrientationChanged(Edges, CurE);
    while (!end) {
      wexp.Next();
      if (wexp.More()) {
        NextE = wexp.Current();
      } 
      else {
        NextE = FirstE; end = Standard_True;
      }
      if (CurE.IsSame(NextE)) continue;

      ToReverse2 = IsOrientationChanged(Edges, NextE);

      TopoDS_Vertex Vref = CommonVertex(CurE, NextE); 
      gp_Pnt Pref = BRep_Tool::Pnt(Vref);

      TopoDS_Shape aLocalShape = OFI.Generated(CurE);
      TopoDS_Edge CEO = TopoDS::Edge(aLocalShape);
      aLocalShape = OFI.Generated(NextE);
      TopoDS_Edge NEO = TopoDS::Edge(aLocalShape);
      //------------------------------------------
      // Inter processing of images of CurE NextE.
      //------------------------------------------
      TopTools_ListOfShape LV1,LV2;
      Standard_Boolean     DoInter = 1;
      TopoDS_Shape         NE1,NE2;
      
      if (Build.IsBound(CurE) && Build.IsBound(NextE)) {
        NE1 = Build(CurE );
        NE2 = Build(NextE);
      }
      else if (Build.IsBound(CurE) && MES.IsBound(NEO)) {
        NE1 = Build(CurE);
        NE2 = MES  (NEO);
      }
      else if (Build.IsBound(NextE) && MES.IsBound(CEO)) {
        NE1 = Build(NextE);
        NE2 = MES(CEO);
        Standard_Boolean Tmp = ToReverse1;
        ToReverse1 = ToReverse2;
        ToReverse2 = Tmp;
      }
      else {
        DoInter = 0;
      }
      if (DoInter) {
        //------------------------------------
        // NE1,NE2 can be a compound of Edges.
        //------------------------------------
        Standard_Boolean bCoincide;
        TopExp_Explorer Exp1, Exp2;
        for (Exp1.Init(NE1, TopAbs_EDGE); Exp1.More(); Exp1.Next()) {
          TopoDS_Edge aE1 = TopoDS::Edge(Exp1.Current());
          for (Exp2.Init(NE2, TopAbs_EDGE); Exp2.More(); Exp2.Next()) {
            TopoDS_Edge aE2 = TopoDS::Edge(Exp2.Current());

            //Correct orientation of edges
            if (ToReverse1)
              aE1.Reverse();
            if (ToReverse2)
              aE2.Reverse();
            //////////////////////////////
            
            RefEdgeInter(FIO, BAsurf, aE1, aE2, AsDes2d,
                         Tol, Standard_True, Pref, theDMVV, bCoincide);
          }
        }
        //
        // check if some of the offset edges have been
        // generated out of the common vertex
        if (Build.IsBound(Vref)) {
          FacesWithVerts.Add(FI);
        }
      }
      else {
        if (MES.IsBound(CEO)) {
          TopoDS_Vertex  V = CommonVertex(CEO,NEO);
          UpdateVertex  (V,CEO,TopoDS::Edge(MES(CEO)),Tol);
          AsDes2d->Add     (MES(CEO),V);
        }
        else if (MES.IsBound(NEO)) {
          TopoDS_Vertex V = CommonVertex(CEO,NEO);
          UpdateVertex (V,NEO,TopoDS::Edge(MES(NEO)),Tol);
          AsDes2d->Add    (MES(NEO),V);
        }
      }
      CurE = NextE;
      ToReverse1 = ToReverse2;
    }
  }
}

//=======================================================================
//function : ConnexIntByIntInVert
//purpose  : Intersection of the edges generated out of vertices
//=======================================================================
void BRepOffset_Inter2d::ConnexIntByIntInVert
 (const TopoDS_Face&            FI,
  BRepOffset_Offset&            OFI,
  TopTools_DataMapOfShapeShape& MES,
  const TopTools_DataMapOfShapeShape& Build,
  const Handle(BRepAlgo_AsDes)& AsDes,
  const Handle(BRepAlgo_AsDes)& AsDes2d,
  const Standard_Real           Tol,
  TopTools_IndexedDataMapOfShapeListOfShape& theDMVV)
{
  TopoDS_Face           FIO = TopoDS::Face(OFI.Face());
  if (MES.IsBound(FIO)) FIO = TopoDS::Face(MES(FIO));
  //
  TopTools_MapOfShape aME;
  const TopTools_ListOfShape& aLE = AsDes->Descendant(FIO);
  TopTools_ListIteratorOfListOfShape aItLE(aLE);
  for (; aItLE.More(); aItLE.Next()) {
    const TopoDS_Shape& aE = aItLE.Value();
    aME.Add(aE);
  }
  //
  BRepAdaptor_Surface BAsurf(FIO);
  //
  TopExp_Explorer exp(FI.Oriented(TopAbs_FORWARD),TopAbs_WIRE);
  for (; exp.More(); exp.Next()) {
    const TopoDS_Wire&     W = TopoDS::Wire(exp.Current());
    //
    BRepTools_WireExplorer wexp;
    Standard_Boolean       end = Standard_False ;
    TopoDS_Edge            FirstE,CurE,NextE;
    //
    TopoDS_Shape aLocalWire = W .Oriented(TopAbs_FORWARD);
    TopoDS_Shape aLocalFace = FI.Oriented(TopAbs_FORWARD);
    wexp.Init(TopoDS::Wire(aLocalWire),TopoDS::Face(aLocalFace));
    if (!wexp.More())
      continue; // Protection from case when explorer does not contain edges.
    //
    CurE = FirstE  = wexp.Current(); 
    while (!end) {
      wexp.Next();
      if (wexp.More()) {
        NextE = wexp.Current();
      } 
      else {
        NextE = FirstE; end = Standard_True;
      }
      if (CurE.IsSame(NextE)) continue;
      //
      TopoDS_Vertex Vref = CommonVertex(CurE, NextE); 
      gp_Pnt Pref = BRep_Tool::Pnt(Vref);
      if (!Build.IsBound(Vref)) {
        CurE = NextE;
        continue;
      }
      //
      TopoDS_Shape aLocalShape = OFI.Generated(CurE);
      TopoDS_Edge CEO = TopoDS::Edge(aLocalShape);
      aLocalShape = OFI.Generated(NextE);
      TopoDS_Edge NEO = TopoDS::Edge(aLocalShape);
      //
      TopoDS_Shape         NE1,NE2;
      
      if (Build.IsBound(CurE) && Build.IsBound(NextE)) {
        NE1 = Build(CurE );
        NE2 = Build(NextE);
      }
      else if (Build.IsBound(CurE) && MES.IsBound(NEO)) {
        NE1 = Build(CurE);
        NE2 = MES  (NEO);
      }
      else if (Build.IsBound(NextE) && MES.IsBound(CEO)) {
        NE1 = Build(NextE);
        NE2 = MES(CEO);
      }
      else {
        CurE = NextE;
        continue;
      }
      //
      TopExp_Explorer Exp1, Exp2;
      Standard_Boolean bCoincide;
      // intersect edges generated from vertex with the edges of the face
      TopoDS_Shape NE3 = Build(Vref);
      //
      for (Exp2.Init(NE3, TopAbs_EDGE); Exp2.More(); Exp2.Next()) {
        const TopoDS_Edge& aE3 = *(TopoDS_Edge*)&Exp2.Current();
        if (!aME.Contains(aE3)) {
          continue;
        }
        //
        // intersection with first edge
        for (Exp1.Init(NE1, TopAbs_EDGE); Exp1.More(); Exp1.Next()) {
          const TopoDS_Edge& aE1 = TopoDS::Edge(Exp1.Current());
          RefEdgeInter(FIO, BAsurf, aE1, aE3, AsDes2d,
            Tol, Standard_True, Pref, theDMVV, bCoincide);
          if (bCoincide) {
            // in case of coincidence trim the edge E3 the same way as E1
            Store(aE3, AsDes2d->Descendant(aE1), Tol, Standard_True, AsDes2d, theDMVV);
          }
        }
        //
        // intersection with second edge
        for (Exp1.Init(NE2, TopAbs_EDGE); Exp1.More(); Exp1.Next()) {
          const TopoDS_Edge& aE2 = TopoDS::Edge(Exp1.Current());
          RefEdgeInter(FIO, BAsurf, aE2, aE3, AsDes2d,
            Tol, Standard_True, Pref, theDMVV, bCoincide);
          if (bCoincide) {
            // in case of coincidence trim the edge E3 the same way as E2
            Store(aE3, AsDes2d->Descendant(aE2), Tol, Standard_True, AsDes2d, theDMVV);
          }
        }
        //
        // intersection of the edges generated from vertex
        // among themselves
        for (Exp1.Init(NE3, TopAbs_EDGE); Exp1.More(); Exp1.Next()) {
          if (aE3.IsSame(Exp1.Current())) {
            break;
          }
        }
        //
        for (Exp1.Next(); Exp1.More(); Exp1.Next()) {
          const TopoDS_Edge& aE3Next = TopoDS::Edge(Exp1.Current());
          if (aME.Contains(aE3Next)) {
            RefEdgeInter(FIO, BAsurf, aE3Next, aE3, AsDes2d,
              Tol, Standard_True, Pref, theDMVV, bCoincide);
          }
        }
      }
      CurE = NextE;
    }
  }
}

//=======================================================================
//function : MakeChain
//purpose  : 
//=======================================================================
static void MakeChain(const TopoDS_Shape& theV,
                      const TopTools_IndexedDataMapOfShapeListOfShape& theDMVV,
                      TopTools_MapOfShape& theMDone,
                      TopTools_ListOfShape& theChain)
{
  if (theMDone.Add(theV)) {
    theChain.Append(theV);
    const TopTools_ListOfShape* pLV = theDMVV.Seek(theV);
    if (pLV) {
      TopTools_ListIteratorOfListOfShape aIt(*pLV);
      for (; aIt.More(); aIt.Next()) {
        MakeChain(aIt.Value(), theDMVV, theMDone, theChain);
      }
    }
  }
}

//=======================================================================
//function : FuseVertices
//purpose  : 
//=======================================================================
void BRepOffset_Inter2d::FuseVertices(const TopTools_IndexedDataMapOfShapeListOfShape& theDMVV,
                                      const Handle(BRepAlgo_AsDes)& theAsDes)
{
  BRep_Builder aBB;
  TopTools_MapOfShape aMVDone;
  Standard_Integer i, aNb = theDMVV.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Vertex& aV = TopoDS::Vertex(theDMVV.FindKey(i));
    //
    // find chain of vertices
    TopTools_ListOfShape aLVChain;
    MakeChain(aV, theDMVV, aMVDone, aLVChain);
    //
    if (aLVChain.Extent() < 2) {
      continue;
    }
    //
    // make new vertex
    TopoDS_Vertex aVNew;
    BOPTools_AlgoTools::MakeVertex(aLVChain, aVNew);
    //
    TopoDS_Vertex aVNewInt = TopoDS::Vertex(aVNew.Oriented(TopAbs_INTERNAL));
    //
    TopTools_ListIteratorOfListOfShape aIt(aLVChain);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aVOld = aIt.Value();
      // update the parameters on edges
      TopoDS_Vertex aVOldInt = TopoDS::Vertex(aVOld.Oriented(TopAbs_INTERNAL));
      const TopTools_ListOfShape& aLE = theAsDes->Ascendant(aVOld);
      //
      TopTools_ListIteratorOfListOfShape aItLE(aLE);
      for (; aItLE.More(); aItLE.Next()) {
        const TopoDS_Edge& aE = TopoDS::Edge(aItLE.Value());
        Standard_Real aTolE = BRep_Tool::Tolerance(aE);
        Standard_Real aT = BRep_Tool::Parameter(aVOldInt, aE);
        aBB.UpdateVertex(aVNewInt, aT, aE, aTolE);
      }
      // and replace the vertex
      theAsDes->Replace(aVOld, aVNew);
    }
  }
}
