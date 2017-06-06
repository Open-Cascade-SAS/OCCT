// Created on: 1998-08-26
// Created by: Julia GERASIMOVA
// Copyright (c) 1998-1999 Matra Datavision
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
#include <BRep_Builder.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRep_TVertex.hxx>
#include <BRepAdaptor_HCurve.hxx>
#include <BRepAdaptor_HCurve2d.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepFill_CurveConstraint.hxx>
#include <BRepFill_EdgeFaceAndOrder.hxx>
#include <BRepFill_FaceAndOrder.hxx>
#include <BRepFill_Filling.hxx>
#include <BRepLib.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeEdge2d.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BRepLib_MakeWire.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomPlate_CurveConstraint.hxx>
#include <GeomPlate_MakeApprox.hxx>
#include <GeomPlate_PlateG0Criterion.hxx>
#include <GeomPlate_PointConstraint.hxx>
#include <GeomProjLib.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <TColGeom2d_HArray1OfCurve.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TColgp_SequenceOfXY.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <Geom_Surface.hxx>

static gp_Vec MakeFinVec( const TopoDS_Wire aWire, const TopoDS_Vertex aVertex )
{
  TopoDS_Vertex Vfirst, Vlast, Origin;
  BRepTools_WireExplorer Explo( aWire );
  for (; Explo.More(); Explo.Next())
    {
      TopExp::Vertices( Explo.Current(), Vfirst, Vlast );
      if (Vfirst.IsSame( aVertex ))
	{
	  Origin = Vlast;
	  break;
	}
      if (Vlast.IsSame( aVertex ))
	{
	  Origin = Vfirst;
	  break;
	}
    }
  return gp_Vec( BRep_Tool::Pnt( Origin ), BRep_Tool::Pnt( aVertex ) );
}

static TopoDS_Wire WireFromList(TopTools_ListOfShape& Edges)
{
  BRepLib_MakeWire MW;
  TopoDS_Edge anEdge = TopoDS::Edge(Edges.First());
  MW.Add(anEdge);
  Edges.RemoveFirst();

  while (!Edges.IsEmpty())
  {
    TopoDS_Wire CurWire = MW.Wire();
    TopoDS_Vertex V1, V2;
    TopExp::Vertices(CurWire, V1, V2);
    TopTools_ListIteratorOfListOfShape itl(Edges);
    for (; itl.More(); itl.Next())
    {
      anEdge = TopoDS::Edge(itl.Value());
      TopoDS_Vertex V3, V4;
      TopExp::Vertices(anEdge, V3, V4);
      if (V1.IsSame(V3) || V1.IsSame(V4) ||
          V2.IsSame(V3) || V2.IsSame(V4))
        break;
    }
    MW.Add(anEdge);
    Edges.Remove(itl);
  }

  return (MW.Wire());
}

//=======================================================================
//function : Constructor
//purpose  : 
//======================================================================
BRepFill_Filling::BRepFill_Filling( const Standard_Integer Degree,
				    const Standard_Integer NbPtsOnCur,
				    const Standard_Integer NbIter,
				    const Standard_Boolean Anisotropie,
				    const Standard_Real Tol2d,
				    const Standard_Real Tol3d,
				    const Standard_Real TolAng,
				    const Standard_Real TolCurv,
				    const Standard_Integer MaxDeg,
				    const Standard_Integer MaxSegments )
{
  myDegree = Degree;
  myNbPtsOnCur = NbPtsOnCur;
  myNbIter = NbIter;
  myAnisotropie = Anisotropie;

  myTol2d = Tol2d;
  myTol3d = Tol3d;
  myTolAng = TolAng;
  myTolCurv = TolCurv;

  myMaxDeg = MaxDeg;
  myMaxSegments = MaxSegments;

  myIsInitFaceGiven = Standard_False;

  myIsDone = Standard_False;
}

//=======================================================================
//function : SetConstrParam
//purpose  : 
//======================================================================
void BRepFill_Filling::SetConstrParam( const Standard_Real Tol2d,
				       const Standard_Real Tol3d,
				       const Standard_Real TolAng,
				       const Standard_Real TolCurv )
{
  myTol2d = Tol2d;
  myTol3d = Tol3d;
  myTolAng = TolAng;
  myTolCurv = TolCurv;
}

//=======================================================================
//function : SetResolParam
//purpose  : 
//======================================================================
void BRepFill_Filling::SetResolParam( const Standard_Integer Degree,
				      const Standard_Integer NbPtsOnCur,
				      const Standard_Integer NbIter,
				      const Standard_Boolean Anisotropie )
{
  myDegree = Degree;
  myNbPtsOnCur = NbPtsOnCur;
  myNbIter = NbIter;
  myAnisotropie = Anisotropie;
}

//=======================================================================
//function : SetApproxParam
//purpose  : 
//======================================================================
void BRepFill_Filling::SetApproxParam( const Standard_Integer MaxDeg,
				       const Standard_Integer MaxSegments )
{
  myMaxDeg      = MaxDeg;
  myMaxSegments = MaxSegments;
}

//=======================================================================
//function : LoadInitSurface
//purpose  : 
//======================================================================
void BRepFill_Filling::LoadInitSurface( const TopoDS_Face& aFace )
{
  myInitFace = aFace;
  myIsInitFaceGiven = Standard_True;
}

//=======================================================================
//function : Add
//purpose  : adds an edge as a constraint
//======================================================================
Standard_Integer BRepFill_Filling::Add( const TopoDS_Edge& anEdge,
				        const GeomAbs_Shape Order,
				        const Standard_Boolean IsBound )
{
  TopoDS_Face NullFace;
  BRepFill_EdgeFaceAndOrder EdgeFaceAndOrder( anEdge, NullFace, Order );
  if (IsBound)
    {
      myBoundary.Append( EdgeFaceAndOrder );
      TopTools_ListOfShape EmptyList;
      myOldNewMap.Bind(anEdge, EmptyList);
      return myBoundary.Length();
    }
  else
    {
      myConstraints.Append( EdgeFaceAndOrder );
      return (myBoundary.Length() + myFreeConstraints.Length() + myConstraints.Length());
    }
}

//=======================================================================
//function : Add
//purpose  : adds an edge with supporting face as a constraint
//======================================================================
Standard_Integer BRepFill_Filling::Add( const TopoDS_Edge& anEdge,
				        const TopoDS_Face& Support,
				        const GeomAbs_Shape Order,
				        const Standard_Boolean IsBound )
{
  BRepFill_EdgeFaceAndOrder EdgeFaceAndOrder( anEdge, Support, Order );
  if (IsBound)
    {
      myBoundary.Append( EdgeFaceAndOrder );
      TopTools_ListOfShape EmptyList;
      myOldNewMap.Bind(anEdge, EmptyList);
      return myBoundary.Length();
    }
  else
    {
      myConstraints.Append( EdgeFaceAndOrder );
      return (myBoundary.Length() + myFreeConstraints.Length() + myConstraints.Length());
    }
}

//=======================================================================
//function : Add
//purpose  : adds a "free constraint": face without edge
//======================================================================
Standard_Integer BRepFill_Filling::Add( const TopoDS_Face& Support,
				        const GeomAbs_Shape Order )
{
  BRepFill_FaceAndOrder FaceAndOrder( Support, Order );
  myFreeConstraints.Append( FaceAndOrder );
  return (myBoundary.Length() + myFreeConstraints.Length());
}

//=======================================================================
//function : Add
//purpose  : adds a point constraint
//======================================================================
Standard_Integer BRepFill_Filling::Add( const gp_Pnt& Point )
{
  Handle( GeomPlate_PointConstraint ) aPC = new GeomPlate_PointConstraint( Point, GeomAbs_C0, myTol3d );
  myPoints.Append( aPC );
  return (myBoundary.Length() + myFreeConstraints.Length() + myConstraints.Length() + myPoints.Length());
}

//=======================================================================
//function : Add
//purpose  : adds a point constraint on a face
//======================================================================
Standard_Integer BRepFill_Filling::Add( const Standard_Real U,
				        const Standard_Real V,
				        const TopoDS_Face& Support,
				        const GeomAbs_Shape Order )
{
  Handle( BRepAdaptor_HSurface ) HSurf = new BRepAdaptor_HSurface();
  HSurf->ChangeSurface().Initialize( Support );
  Handle( GeomPlate_PointConstraint ) aPC = 
    new GeomPlate_PointConstraint( U, V, BRep_Tool::Surface( HSurf->ChangeSurface().Face() ), Order,
				   myTol3d, myTolAng, myTolCurv );
  myPoints.Append( aPC );
  return (myBoundary.Length() + myFreeConstraints.Length() + myConstraints.Length() + myPoints.Length());
}


//=======================================================================
//function : AddConstraints
//purpose  : 
//======================================================================
void BRepFill_Filling::AddConstraints( const BRepFill_SequenceOfEdgeFaceAndOrder& SeqOfConstraints )
{
  TopoDS_Edge CurEdge;
  TopoDS_Face CurFace;
  GeomAbs_Shape CurOrder;

  Handle(GeomPlate_CurveConstraint) Constr;
  Standard_Integer i;
  for (i = 1; i <= SeqOfConstraints.Length(); i++)
    {
      CurEdge = SeqOfConstraints(i).myEdge;
      CurFace = SeqOfConstraints(i).myFace;
      CurOrder = SeqOfConstraints(i).myOrder;
      
      if (CurFace.IsNull()) {
	if (CurOrder == GeomAbs_C0) {
	  Handle( BRepAdaptor_HCurve ) HCurve = new BRepAdaptor_HCurve();
	  HCurve->ChangeCurve().Initialize( CurEdge );
	  const Handle(Adaptor3d_HCurve)& aHCurve = HCurve; // to avoid ambiguity
	  Constr = new BRepFill_CurveConstraint(aHCurve,
						CurOrder,
						myNbPtsOnCur,
						myTol3d );
	}
	else { // Pas de representation Topologique
	  // On prend une representation Geometrique : au pif !
	  Handle( Geom_Surface ) Surface;
	  Handle( Geom2d_Curve ) C2d;
	  TopLoc_Location loc;
	  Standard_Real f, l;
	  BRep_Tool::CurveOnSurface( CurEdge, C2d, Surface, loc, f, l);
	  if (Surface.IsNull()) {
	    throw Standard_Failure( "Add" );
	    return;
	  }
	  Surface = Handle(Geom_Surface)::DownCast(Surface->Copy());
	  Surface->Transform(loc.Transformation());
	  Handle( GeomAdaptor_HSurface ) Surf = new GeomAdaptor_HSurface(Surface);
	  Handle( Geom2dAdaptor_HCurve ) Curve2d = new Geom2dAdaptor_HCurve(C2d);
	  
	  Adaptor3d_CurveOnSurface CurvOnSurf( Curve2d, Surf );
	  Handle (Adaptor3d_HCurveOnSurface) HCurvOnSurf = new Adaptor3d_HCurveOnSurface( CurvOnSurf );
	  
	  Constr = new GeomPlate_CurveConstraint(HCurvOnSurf,
						 CurOrder,
						 myNbPtsOnCur,
						 myTol3d,
						 myTolAng,
						 myTolCurv );
	}
      }
      else
	{
	  Handle( BRepAdaptor_HSurface ) Surf = new BRepAdaptor_HSurface();
	  Surf->ChangeSurface().Initialize( CurFace );
	  Handle( BRepAdaptor_HCurve2d ) Curve2d = new BRepAdaptor_HCurve2d();
	  Curve2d->ChangeCurve2d().Initialize( CurEdge, CurFace );
	  // If CurEdge has no 2d representation on CurFace,
	  // there will be exception "Attempt to access to null object"
	  // in this initialization (null pcurve).
	  Adaptor3d_CurveOnSurface CurvOnSurf( Curve2d, Surf );
	  Handle (Adaptor3d_HCurveOnSurface) HCurvOnSurf = new Adaptor3d_HCurveOnSurface( CurvOnSurf );

	  Constr = new BRepFill_CurveConstraint( HCurvOnSurf,
						 CurOrder,
						 myNbPtsOnCur,
						 myTol3d,
						 myTolAng,
						 myTolCurv );
	}
      if (myIsInitFaceGiven)
        {
	  Handle( Geom2d_Curve ) Curve2d;
	  Standard_Real FirstPar, LastPar;
	  Curve2d = BRep_Tool::CurveOnSurface( CurEdge, myInitFace, FirstPar, LastPar );
	  if (! Curve2d.IsNull()) 
	    {
	      Curve2d = new Geom2d_TrimmedCurve( Curve2d, FirstPar, LastPar );
	      Constr->SetCurve2dOnSurf( Curve2d );
	    }
	}
      myBuilder.Add( Constr );
    }
}

//=======================================================================
//function : BuildWires
//purpose  : 
//======================================================================
void BRepFill_Filling::BuildWires( TopTools_ListOfShape& EdgeList, TopTools_ListOfShape& WireList )
{
  TopTools_ListIteratorOfListOfShape Itl;
  Standard_Integer i, j;

  while (! EdgeList.IsEmpty())
    {
      BRepLib_MakeWire MW;
      TopoDS_Edge FirstEdge = TopoDS::Edge(EdgeList.First());
      MW.Add(FirstEdge);
      EdgeList.RemoveFirst();
      TopoDS_Vertex V_wire[2], V_edge[2];

      for (;;)
      {
        TopoDS_Wire CurWire = MW.Wire();
        TopExp::Vertices(CurWire, V_wire[0], V_wire[1]);
        Standard_Boolean found = Standard_False;
        for (Itl.Initialize( EdgeList ); Itl.More(); Itl.Next())
        {
          TopoDS_Edge CurEdge = TopoDS::Edge(Itl.Value());
          TopExp::Vertices(CurEdge, V_edge[0], V_edge[1]);
          for (i = 0; i < 2; i++)
          {
            for (j = 0; j < 2; j++)
              if (V_wire[i].IsSame(V_edge[j]))
              {
                MW.Add(CurEdge);
                EdgeList.Remove(Itl);
                found = Standard_True;
                break;
              }
            if (found)
              break;
          }
          if (found)
            break;
        }
        if (!found) //try to find geometric coincidence
        {
          gp_Pnt P_wire[2];
          P_wire[0] = BRep_Tool::Pnt(V_wire[0]);
          P_wire[1] = BRep_Tool::Pnt(V_wire[1]);
          for (Itl.Initialize( EdgeList ); Itl.More(); Itl.Next())
          {
            TopoDS_Edge CurEdge = TopoDS::Edge(Itl.Value());
            TopExp::Vertices(CurEdge, V_edge[0], V_edge[1]);
            for (i = 0; i < 2; i++)
            {
              for (j = 0; j < 2; j++)
              {
                Standard_Real aDist = P_wire[i].Distance(BRep_Tool::Pnt(V_edge[j]));
                if (aDist < BRep_Tool::Tolerance(V_wire[i]) &&
                    aDist < BRep_Tool::Tolerance(V_edge[j]))
                {
                  MW.Add(CurEdge);
                  myOldNewMap(CurEdge).Append(MW.Edge());
                  EdgeList.Remove(Itl);
                  found = Standard_True;
                  break;
                }
              }
              if (found)
                break;
            }
            if (found)
              break;
          }
        }
        if (!found) //end of current wire, begin next wire
        {
          WireList.Append( MW.Wire() );
          break;
        }
      } //end of for (;;)
    } //end of while (! EdgeList.IsEmpty())  
}

//=======================================================================
//function : FindExtremitiesOfHoles
//purpose  : 
//======================================================================
void BRepFill_Filling::FindExtremitiesOfHoles(const TopTools_ListOfShape& WireList,
                                              TopTools_SequenceOfShape& VerSeq ) const
{
  TopTools_SequenceOfShape WireSeq;
  TopTools_ListIteratorOfListOfShape Itl(WireList);
  for (; Itl.More(); Itl.Next())
    WireSeq.Append(Itl.Value());
       
  TopoDS_Wire theWire;
  theWire = TopoDS::Wire(WireSeq(1));
  WireSeq.Remove(1);

  if (BRep_Tool::IsClosed(theWire))
    return;

  TopoDS_Vertex Vfirst, Vlast;
  TopExp::Vertices( theWire, Vfirst, Vlast );

  if (Vfirst.IsSame(Vlast))
  {
    // The Wire is closed indeed despite its 
    // being not detected earlier.

    return;
  }

  gp_Vec FinVec = MakeFinVec( theWire, Vlast );
  TopoDS_Vertex theVertex = Vlast;
  VerSeq.Append( Vlast );

  while (! WireSeq.IsEmpty())
    {
      TopoDS_Vertex MinVtx;
      Standard_Integer i, MinInd = 1;
      Standard_Boolean IsLast = Standard_False;
      Standard_Real MinAngle = M_PI;
      
      for (i = 1; i <= WireSeq.Length(); i++)
	{
	  const TopoDS_Wire& CurWire = TopoDS::Wire( WireSeq(i) );
	  TopExp::Vertices( CurWire, Vfirst, Vlast );
	  
	  Standard_Real angle =
            FinVec.Angle(gp_Vec(BRep_Tool::Pnt(theVertex), BRep_Tool::Pnt(Vfirst)));
	  if (angle < MinAngle)
	    {
	      MinAngle = angle;
              MinVtx = Vfirst;
              MinInd = i;
	      IsLast = Standard_True;
	    }
	  angle = FinVec.Angle(gp_Vec(BRep_Tool::Pnt(theVertex), BRep_Tool::Pnt(Vlast)));
	  if (angle < MinAngle)
	    {
	      MinAngle = angle;
              MinVtx = Vlast;
              MinInd = i;
	      IsLast = Standard_False;
	    }
	}
      VerSeq.Append( MinVtx );
      const TopoDS_Wire& MinWire = TopoDS::Wire(WireSeq(MinInd));
      TopExp::Vertices( MinWire, Vfirst, Vlast );
      if (IsLast)
	{
	  FinVec = MakeFinVec( MinWire, Vlast );
	  theVertex = Vlast;
	}
      else
	{
	  FinVec = MakeFinVec( MinWire, Vfirst );
	  theVertex = Vfirst;
	}
      VerSeq.Append( theVertex );
      WireSeq.Remove(MinInd);
    }
  TopExp::Vertices( theWire, Vfirst, Vlast );
  VerSeq.Append( Vfirst );
}

//=======================================================================
//function : Build
//purpose  : builds the resulting face
//======================================================================
void BRepFill_Filling::Build()
{
  GeomPlate_BuildPlateSurface thebuild( myDegree, myNbPtsOnCur, myNbIter,
                                        myTol2d, myTol3d, myTolAng, myTolCurv, myAnisotropie );

  myBuilder = thebuild;
  TopoDS_Edge CurEdge;
  TopoDS_Face CurFace;
  Standard_Integer i, j;

  //Creating array of vertices: extremities of wires
  TopTools_SequenceOfShape VerSeq;

  //Building missing bounds
  TopTools_ListOfShape EdgeList, WireList;
  for (i = 1; i <= myBoundary.Length(); i++)
    EdgeList.Append( myBoundary(i).myEdge );

  BuildWires( EdgeList, WireList );
  FindExtremitiesOfHoles( WireList, VerSeq );
  
  //Searching for surfaces for missing bounds
  for (j = 1; j <= myFreeConstraints.Length(); j++)
    {
      GeomAPI_ProjectPointOnSurf Projector;
      Standard_Real U1, V1, U2, V2;

      CurFace = myFreeConstraints(j).myFace;
      Handle( BRepAdaptor_HSurface ) HSurf = new BRepAdaptor_HSurface();
      HSurf->ChangeSurface().Initialize( CurFace );
      Handle( Geom_Surface ) CurSurface = BRep_Tool::Surface( HSurf->ChangeSurface().Face() );
      //BRepTopAdaptor_FClass2d Classifier( CurFace, Precision::Confusion() );
	  
      for (i = 1; i <= VerSeq.Length(); i += 2)
	{
          const TopoDS_Vertex& FirstVtx = TopoDS::Vertex(VerSeq(i));
          const TopoDS_Vertex& LastVtx  = TopoDS::Vertex(VerSeq(i+1));
          
          gp_Pnt FirstPnt = BRep_Tool::Pnt(FirstVtx);
	  Projector.Init( FirstPnt, CurSurface );
	  if (Projector.LowerDistance() > Precision::Confusion())
	    continue;
	  Projector.LowerDistanceParameters( U1, V1 );
	  
	  /*
	  State = Classifier.Perform(gp_Pnt2d( U1, V1 ),
				     ((HSurf->IsUPeriodic() || HSurf->IsVPeriodic())? Standard_True : Standard_False));
	  if (State == TopAbs_OUT || State == TopAbs_UNKNOWN)
	    continue;
	  */

          gp_Pnt LastPnt = BRep_Tool::Pnt(LastVtx);
	  Projector.Init( LastPnt, CurSurface );
	  if (Projector.LowerDistance() > Precision::Confusion())
	    continue;
	  Projector.LowerDistanceParameters( U2, V2 );
	  
	  /*
	  State = Classifier.Perform(gp_Pnt2d( U2, V2 ),
				     ((HSurf->IsUPeriodic() || HSurf->IsVPeriodic())? Standard_True : Standard_False));
	  if (State == TopAbs_OUT || State == TopAbs_UNKNOWN)
	    continue;
	  */
	  
	  //Making the constraint
	  TColgp_Array1OfPnt2d Points( 1, 2 );
	  Points(1) = gp_Pnt2d( U1, V1 );
	  Points(2) = gp_Pnt2d( U2, V2 );
	  Handle( Geom2d_BezierCurve ) Line2d = new Geom2d_BezierCurve( Points );
	  TopoDS_Edge E = BRepLib_MakeEdge( Line2d, CurSurface, FirstVtx, LastVtx );
	  Add( E, CurFace, myFreeConstraints(j).myOrder );
	  VerSeq.Remove( i, i+1 );
	  break;
	} //for (i = 1; i <= VerSeq.Length(); i += 2)
    } //for (j = 1; j <= myFreeConstraints.Length(); j++)
  
  //Load initial surface to myBuilder if it is given
  if (myIsInitFaceGiven)
    {
      Handle( BRepAdaptor_HSurface ) HSurfInit = new BRepAdaptor_HSurface();
      HSurfInit->ChangeSurface().Initialize( myInitFace );
      myBuilder.LoadInitSurface( BRep_Tool::Surface( HSurfInit->ChangeSurface().Face() ) );
    }

  //Adding constraints to myBuilder
  AddConstraints( myBoundary );
  myBuilder.SetNbBounds( myBoundary.Length() );
  AddConstraints( myConstraints );
  for (i = 1; i <= myPoints.Length(); i++)
    myBuilder.Add( myPoints(i) );

  myBuilder.Perform();
  if (myBuilder.IsDone())
    myIsDone = Standard_True;
  else
    {
      myIsDone = Standard_False;
      return;
    }

  Handle( GeomPlate_Surface ) GPlate = myBuilder.Surface();
  Handle( Geom_BSplineSurface ) Surface;
  // Approximation
  Standard_Real dmax = 1.1 * myBuilder.G0Error(); //???????????
  //Standard_Real dmax = myTol3d;
  if (! myIsInitFaceGiven)
    {
     Standard_Real seuil; //?????

     TColgp_SequenceOfXY S2d;
     TColgp_SequenceOfXYZ S3d;
     myBuilder.Disc2dContour(4,S2d);
     myBuilder.Disc3dContour(4,0,S3d);
     seuil = Max( myTol3d, 10*myBuilder.G0Error() ); //????????
     GeomPlate_PlateG0Criterion Criterion( S2d, S3d, seuil );
     GeomPlate_MakeApprox Approx( GPlate, Criterion, myTol3d, myMaxSegments, myMaxDeg );
     Surface = Approx.Surface();
   }
  else
    {
      GeomPlate_MakeApprox Approx( GPlate, myTol3d, myMaxSegments, myMaxDeg, dmax, 0 ); //?????????????
      //GeomConvert_ApproxSurface Approx( GPlate, myTol3d, GeomAbs_C1, GeomAbs_C1, myMaxDeg, myMaxDeg, myMaxSegments, 1 );
      //Approx.Dump( cout );
      Surface = Approx.Surface();
    }

  //Build the final wire and final face
  TopTools_ListOfShape FinalEdges;
  Handle(TColGeom2d_HArray1OfCurve) CurvesOnPlate = myBuilder.Curves2d();
  BRep_Builder BB;
  for (i = 1; i <= myBoundary.Length(); i++)
  {
    const TopoDS_Edge& InitEdge = myBoundary(i).myEdge;
    TopoDS_Edge anEdge = InitEdge;
    if (!myOldNewMap(anEdge).IsEmpty())
      anEdge = TopoDS::Edge( myOldNewMap(anEdge).First() );
    Handle(Geom2d_Curve) aCurveOnPlate = CurvesOnPlate->Value(i);

    TopoDS_Edge NewEdge = TopoDS::Edge(anEdge.EmptyCopied());

    TopoDS_Vertex V1, V2;
    TopExp::Vertices(anEdge, V1, V2, Standard_True); //with orientation
    BB.UpdateVertex(V1, dmax);
    BB.UpdateVertex(V2, dmax);
    BB.Add(NewEdge, V1);
    BB.Add(NewEdge, V2);
    TopLoc_Location Loc;
    BB.UpdateEdge(NewEdge, aCurveOnPlate, Surface, Loc, dmax);
    //BRepLib::SameRange(NewEdge);
    BRepLib::SameParameter(NewEdge, dmax, Standard_True);
    FinalEdges.Append(NewEdge);
    myOldNewMap(InitEdge).Clear();
    myOldNewMap(InitEdge).Append(NewEdge);
  }
  
  TopoDS_Wire FinalWire = WireFromList(FinalEdges);
  if (!(FinalWire.Closed()))
    throw Standard_Failure("Wire is not closed");
  
  myFace = BRepLib_MakeFace( Surface, FinalWire );
}

//=======================================================================
//function : IsDone
//purpose  : 
//======================================================================
Standard_Boolean BRepFill_Filling::IsDone() const
{
  return myIsDone;
}

//=======================================================================
//function : Face
//purpose  : returns the result
//======================================================================
TopoDS_Face BRepFill_Filling::Face() const
{
  return myFace;
}

//=======================================================================
//function : Generated
//purpose  : returns the new edge (first in list) made from old edge "S"
//=======================================================================
 const TopTools_ListOfShape& BRepFill_Filling::Generated(const TopoDS_Shape& S) 
{
  myGenerated.Clear();
  
  if (myOldNewMap.IsBound(S))
    myGenerated.Append(myOldNewMap(S));
  
  return myGenerated;
}

//==========================================================================
//function : G0Error
//purpose  : returns maximum distance from boundary to the resulting surface
//==========================================================================
Standard_Real BRepFill_Filling::G0Error() const
{
  return myBuilder.G0Error();
}

//=======================================================================
//function : G1Error
//purpose  : returns maximum angle between the resulting surface
//           and constraint surfaces at boundaries
//======================================================================
Standard_Real BRepFill_Filling::G1Error() const
{
  return myBuilder.G1Error();
}

//=======================================================================
//function : G2Error
//purpose  : returns maximum difference of curvature between
//           the resulting surface and constraint surfaces at boundaries
//======================================================================
Standard_Real BRepFill_Filling::G2Error() const
{
  return myBuilder.G2Error();
}

//==========================================================================
//function : G0Error
//purpose  : returns maximum distance between the constraint number Index
//           and the resulting surface
//==========================================================================
Standard_Real BRepFill_Filling::G0Error( const Standard_Integer Index )
{
  return myBuilder.G0Error( Index );
}

//==========================================================================
//function : G1Error
//purpose  : returns maximum angle between the constraint number Index
//           and the resulting surface
//==========================================================================
Standard_Real BRepFill_Filling::G1Error( const Standard_Integer Index )
{
  return myBuilder.G1Error( Index );
}

//==========================================================================
//function : G2Error
//purpose  : returns maximum difference of curvature between
//           the constraint number Index and the resulting surface
//==========================================================================
Standard_Real BRepFill_Filling::G2Error( const Standard_Integer Index )
{
  return myBuilder.G2Error( Index );
}
