// File:	BRepFill_Filling.cxx
// Created:	Wed Aug 26 12:21:10 1998
// Author:	Julia GERASIMOVA
//		<jgv@clubox.nnov.matra-dtv.fr>


#include <BRepFill_Filling.ixx>
#include <BRepFill_EdgeFaceAndOrder.hxx>
#include <BRepFill_FaceAndOrder.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepAdaptor_HCurve.hxx>
#include <BRepAdaptor_HCurve2d.hxx>
#include <BRepFill_CurveConstraint.hxx>

#include <Geom2dAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <Adaptor3d_HCurveOnSurface.hxx>

#include <GeomPlate_MakeApprox.hxx>
#include <GeomPlate_CurveConstraint.hxx>
#include <GeomPlate_PointConstraint.hxx>

#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeEdge2d.hxx>
#include <BRepLib_MakeWire.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>
#include <BRepLib_MakeFace.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <TopExp.hxx>
#include <BRep_Tool.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <TopoDS_Vertex.hxx>

#include <Precision.hxx>
#include <GeomProjLib.hxx>
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <BRepLib.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <TColgp_Array1OfPnt2d.hxx>

#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_Surface.hxx>

#include <TColgp_SequenceOfXY.hxx>
#include <GeomPlate_PlateG0Criterion.hxx>
#include <BRep_TVertex.hxx>

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

void BRepFill_Filling::SetApproxParam( const Standard_Integer MaxDeg,
				       const Standard_Integer MaxSegments )
{
  myMaxDeg      = MaxDeg;
  myMaxSegments = MaxSegments;
}

void BRepFill_Filling::LoadInitSurface( const TopoDS_Face& aFace )
{
  myInitFace = aFace;
  myIsInitFaceGiven = Standard_True;
}

Standard_Integer BRepFill_Filling::Add( const TopoDS_Edge& anEdge,
				        const GeomAbs_Shape Order,
				        const Standard_Boolean IsBound )
{
  TopoDS_Face NullFace;
  BRepFill_EdgeFaceAndOrder EdgeFaceAndOrder( anEdge, NullFace, Order );
  if (IsBound)
    {
      myBoundary.Append( EdgeFaceAndOrder );
      return myBoundary.Length();
    }
  else
    {
      myConstraints.Append( EdgeFaceAndOrder );
      return (myBoundary.Length() + myFreeConstraints.Length() + myConstraints.Length());
    }
}

Standard_Integer BRepFill_Filling::Add( const TopoDS_Edge& anEdge,
				        const TopoDS_Face& Support,
				        const GeomAbs_Shape Order,
				        const Standard_Boolean IsBound )
{
  BRepFill_EdgeFaceAndOrder EdgeFaceAndOrder( anEdge, Support, Order );
  if (IsBound)
    {
      myBoundary.Append( EdgeFaceAndOrder );
      return myBoundary.Length();
    }
  else
    {
      myConstraints.Append( EdgeFaceAndOrder );
      return (myBoundary.Length() + myFreeConstraints.Length() + myConstraints.Length());
    }
}

Standard_Integer BRepFill_Filling::Add( const TopoDS_Face& Support,
				        const GeomAbs_Shape Order )
{
  BRepFill_FaceAndOrder FaceAndOrder( Support, Order );
  myFreeConstraints.Append( FaceAndOrder );
  return (myBoundary.Length() + myFreeConstraints.Length());
}

Standard_Integer BRepFill_Filling::Add( const gp_Pnt& Point )
{
  Handle( GeomPlate_PointConstraint ) aPC = new GeomPlate_PointConstraint( Point, GeomAbs_C0, myTol3d );
  myPoints.Append( aPC );
  return (myBoundary.Length() + myFreeConstraints.Length() + myConstraints.Length() + myPoints.Length());
}

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


void BRepFill_Filling::AddConstraints( const BRepFill_SequenceOfEdgeFaceAndOrder& SeqOfConstraints )
{
  TopoDS_Edge CurEdge;
  TopoDS_Face CurFace;
  GeomAbs_Shape CurOrder;

  Handle(GeomPlate_CurveConstraint) Constr;
  for (Standard_Integer i = 1; i <= SeqOfConstraints.Length(); i++)
    {
      CurEdge = SeqOfConstraints(i).myEdge;
      CurFace = SeqOfConstraints(i).myFace;
      CurOrder = SeqOfConstraints(i).myOrder;
      
      if (CurFace.IsNull()) {
	if (CurOrder == GeomAbs_C0) {
	  Handle( BRepAdaptor_HCurve ) HCurve = new BRepAdaptor_HCurve();
	  HCurve->ChangeCurve().Initialize( CurEdge );
	  
	  Constr = new BRepFill_CurveConstraint(HCurve,
						CurOrder,
						myNbPtsOnCur,
						myTol3d );
	}
	else { // Pas de representation Topologique
	  // On prend une representation Geometrique : au pif !
	  Handle( Geom_Surface ) Surface;
	  Handle( Geom2d_Curve ) C2d;
// Class BRep_Tool without fields and without Constructor :
//	  BRep_Tool BT;
	  TopLoc_Location loc;
	  Standard_Real f, l;
//	  BT.CurveOnSurface( CurEdge, C2d, Surface, loc, f, l);
	  BRep_Tool::CurveOnSurface( CurEdge, C2d, Surface, loc, f, l);
	  if (Surface.IsNull()) {
	    Standard_Failure::Raise( "Add" );
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

void BRepFill_Filling::BuildWires( TopTools_MapOfShape& EdgeMap, TopTools_MapOfShape& WireMap ) const
{
  TopoDS_Wire CurWire;
  TopoDS_Edge CurEdge;
  TopoDS_Vertex Wfirst, Wlast, Efirst, Elast;
  gp_Pnt Wp1, Wp2;
  TopTools_MapIteratorOfMapOfShape MapIt;

  while (! EdgeMap.IsEmpty())
    {
      BRepLib_MakeWire MW;
      MapIt.Initialize( EdgeMap );
      MW.Add( TopoDS::Edge( MapIt.Key() ) );
      EdgeMap.Remove( MapIt.Key() );
      CurWire = MW.Wire();
      TopExp::Vertices( CurWire, Wfirst, Wlast );
      Wp1 = BRep_Tool::Pnt( Wfirst );
      Wp2 = BRep_Tool::Pnt( Wlast );
      
      Standard_Boolean IsFound = Standard_True;
      while (IsFound)
	{
	  IsFound = Standard_False;
	  for (MapIt.Initialize( EdgeMap ); MapIt.More(); MapIt.Next())
	    {
	      CurEdge = TopoDS::Edge( MapIt.Key() );
	      TopExp::Vertices( CurEdge, Efirst, Elast );
	      
	      Standard_Real dist = Wp1.Distance( BRep_Tool::Pnt( Efirst ) );
	      if (dist < BRep_Tool::Tolerance( Wfirst ) || dist < BRep_Tool::Tolerance( Efirst ))
		{
		  MW.Add( CurEdge );
		  CurWire = MW.Wire();
		  Wfirst = Elast;
		  Wp1 = BRep_Tool::Pnt( Wfirst );
		  EdgeMap.Remove( CurEdge );
		  IsFound = Standard_True;
		  break;
		}
	      dist = Wp1.Distance( BRep_Tool::Pnt( Elast ) );
	      if (dist < BRep_Tool::Tolerance( Wfirst ) || dist < BRep_Tool::Tolerance( Elast ))
		{
		  MW.Add( CurEdge );
		  CurWire = MW.Wire();
		  Wfirst = Efirst;
		  Wp1 = BRep_Tool::Pnt( Wfirst );
		  EdgeMap.Remove( CurEdge );
		  IsFound = Standard_True;
		  break;
		}
	      dist = Wp2.Distance( BRep_Tool::Pnt( Efirst ) );
	      if (dist < BRep_Tool::Tolerance( Wlast ) || dist < BRep_Tool::Tolerance( Efirst ))
		{
		  MW.Add( CurEdge );
		  CurWire = MW.Wire();
		  Wlast = Elast;
		  Wp2 = BRep_Tool::Pnt( Wlast );
		  EdgeMap.Remove( CurEdge );
		  IsFound = Standard_True;
		  break;
		}
	      dist = Wp2.Distance( BRep_Tool::Pnt( Elast ) );
	      if (dist < BRep_Tool::Tolerance( Wlast ) || dist < BRep_Tool::Tolerance( Elast ))
		{
		  MW.Add( CurEdge );
		  CurWire = MW.Wire();
		  Wlast = Efirst;
		  Wp2 = BRep_Tool::Pnt( Wlast );
		  EdgeMap.Remove( CurEdge );
		  IsFound = Standard_True;
		  break;
		}
	    } //for (MapIt.Init...
	} //while (IsFound)
      WireMap.Add( CurWire );
    } //while (! EdgeMap.IsEmpty())
}

void BRepFill_Filling::FindExtremitiesOfHoles( TopTools_MapOfShape& WireMap, TColgp_SequenceOfPnt& PntSeq ) const
{
  TopoDS_Wire theWire, CurWire;
  TopTools_MapIteratorOfMapOfShape MapIt( WireMap );
  theWire = TopoDS::Wire( MapIt.Key() );
  WireMap.Remove( MapIt.Key() );

  if (theWire.Closed())
    return;

  TopoDS_Vertex Vfirst, Vlast;
  TopExp::Vertices( theWire, Vfirst, Vlast );
  gp_Vec FinVec = MakeFinVec( theWire, Vlast );
  gp_Pnt thePoint = BRep_Tool::Pnt( Vlast );
  PntSeq.Append( thePoint );

  while (! WireMap.IsEmpty())
    {
      gp_Pnt MinPnt;
      TopoDS_Wire MinWire;
#ifndef DEB
      Standard_Boolean IsLast = Standard_False;
#else
      Standard_Boolean IsLast ;
#endif
      Standard_Real MinAngle = PI;
      
      for (MapIt.Initialize( WireMap ); MapIt.More(); MapIt.Next())
	{
	  CurWire = TopoDS::Wire( MapIt.Key() );
	  TopExp::Vertices( CurWire, Vfirst, Vlast );
	  
	  Standard_Real angle = FinVec.Angle( gp_Vec( thePoint, BRep_Tool::Pnt( Vfirst ) ) );
	  if (angle < MinAngle)
	    {
	      MinAngle = angle;
	      MinPnt = BRep_Tool::Pnt( Vfirst );
	      MinWire = CurWire;
	      IsLast = Standard_True;
	    }
	  angle = FinVec.Angle( gp_Vec( thePoint, BRep_Tool::Pnt( Vlast ) ) );
	  if (angle < MinAngle)
	    {
	      MinAngle = angle;
	      MinPnt = BRep_Tool::Pnt( Vlast );
	      MinWire = CurWire;
	      IsLast = Standard_False;
	    }
	}
      PntSeq.Append( MinPnt );
      TopExp::Vertices( MinWire, Vfirst, Vlast );
      if (IsLast)
	{
	  FinVec = MakeFinVec( MinWire, Vlast );
	  thePoint = BRep_Tool::Pnt( Vlast );
	}
      else
	{
	  FinVec = MakeFinVec( MinWire, Vfirst );
	  thePoint = BRep_Tool::Pnt( Vfirst );
	}
      PntSeq.Append( thePoint );
      WireMap.Remove( MinWire );
    }
  TopExp::Vertices( theWire, Vfirst, Vlast );
  PntSeq.Append( BRep_Tool::Pnt( Vfirst ) );
}

void BRepFill_Filling::Build()
{
  GeomPlate_BuildPlateSurface thebuild( myDegree, myNbPtsOnCur, myNbIter,
					   myTol2d, myTol3d, myTolAng, myTolCurv, myAnisotropie );

  myBuilder = thebuild;
  TopoDS_Edge CurEdge;
  TopoDS_Face CurFace;
  Standard_Integer i, j;

  //Creating array of points: extremities of wires
  TColgp_SequenceOfPnt PntSeq;

  //????????????
  //Setting tollerance
  for (i = 1; i <= myBoundary.Length(); i++)
    {
      TopoDS_Edge E (myBoundary(i).myEdge);
      TopoDS_Vertex V1 (TopExp::FirstVertex( E ));
      Handle(BRep_TVertex)& TV1 = *((Handle(BRep_TVertex)*) &V1.TShape());
      if (TV1->Tolerance() > 0.001)
	TV1->Tolerance( 0.001 );
      TV1->Modified( Standard_True );
      TopoDS_Vertex V2 (TopExp::LastVertex( E ));
      Handle(BRep_TVertex)& TV2 = *((Handle(BRep_TVertex)*) &V2.TShape());
      if (TV2->Tolerance() > 0.001)
	TV2->Tolerance( 0.001 );
      TV2->Modified( Standard_True );
    }

  //Building missing bounds
  TopTools_MapOfShape EdgeMap, WireMap;
  for (i = 1; i <= myBoundary.Length(); i++)
    EdgeMap.Add( myBoundary(i).myEdge );

  BuildWires( EdgeMap, WireMap );
  FindExtremitiesOfHoles( WireMap, PntSeq );
  
  //Searching for surfaces for missing bounds
  for (j = 1; j <= myFreeConstraints.Length(); j++)
    {
      GeomAPI_ProjectPointOnSurf Projector;
      Quantity_Parameter U1, V1, U2, V2;

      CurFace = myFreeConstraints(j).myFace;
      Handle( BRepAdaptor_HSurface ) HSurf = new BRepAdaptor_HSurface();
      HSurf->ChangeSurface().Initialize( CurFace );
      Handle( Geom_Surface ) CurSurface = BRep_Tool::Surface( HSurf->ChangeSurface().Face() );
      //BRepTopAdaptor_FClass2d Classifier( CurFace, Precision::Confusion() );
	  
      for (i = 1; i <= PntSeq.Length(); i += 2)
	{
	  Projector.Init( PntSeq.Value(i), CurSurface );
	  if (Projector.LowerDistance() > Precision::Confusion())
	    continue;
	  Projector.LowerDistanceParameters( U1, V1 );
	  
	  /*
	  State = Classifier.Perform(gp_Pnt2d( U1, V1 ),
				     ((HSurf->IsUPeriodic() || HSurf->IsVPeriodic())? Standard_True : Standard_False));
	  if (State == TopAbs_OUT || State == TopAbs_UNKNOWN)
	    continue;
	  */
	  
	  Projector.Init( PntSeq.Value(i+1), CurSurface );
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
	  TopoDS_Edge E = BRepLib_MakeEdge( Line2d, CurSurface, Line2d->FirstParameter(), Line2d->LastParameter() );
	  Add( E, CurFace, myFreeConstraints(j).myOrder );
	  PntSeq.Remove( i, i+1 );
	  break;
	} //for (i = 1; i <= PntSeq.Length(); i += 2)
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
     S2d.Clear();
     S3d.Clear();
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
  
  TopoDS_Wire W;
  //Preparing EdgeMap for method BuildWires
  EdgeMap.Clear();
  WireMap.Clear();
  PntSeq.Clear();
  BRep_Builder B;
  for (i = 1; i <= myBoundary.Length(); i++)
    {
      TopoDS_Edge E;
      Handle( GeomPlate_CurveConstraint ) CC = myBuilder.CurveConstraint(i);
      E = BRepLib_MakeEdge( CC->Curve2dOnSurf(), 
			   Surface,
			   CC->FirstParameter(),
			   CC->LastParameter());
      
      B.UpdateVertex( TopExp::FirstVertex(E), dmax );
      B.UpdateVertex( TopExp::LastVertex(E), dmax );
      BRepLib::BuildCurve3d( E );
      EdgeMap.Add( E );
    }
  //Fixing the holes
  TopTools_MapOfShape EdgeMap2;
  TopTools_MapIteratorOfMapOfShape MapIt( EdgeMap );
  for (; MapIt.More(); MapIt.Next())
    EdgeMap2.Add( MapIt.Key() );
  BuildWires( EdgeMap2, WireMap );
  FindExtremitiesOfHoles( WireMap, PntSeq );

  for (i = 1; i <= PntSeq.Length(); i += 2)
    {
      GeomAPI_ProjectPointOnSurf Projector;
      Quantity_Parameter U1, V1, U2, V2;
      
      Projector.Init( PntSeq.Value(i), Surface );
      Projector.LowerDistanceParameters( U1, V1 );
      Projector.Init( PntSeq.Value(i+1), Surface );
      Projector.LowerDistanceParameters( U2, V2 );
      
      //Making the edge on the resulting surface
      TColgp_Array1OfPnt2d Points( 1, 2 );
      Points(1) = gp_Pnt2d( U1, V1 );
      Points(2) = gp_Pnt2d( U2, V2 );
      Handle( Geom2d_BezierCurve ) Line2d = new Geom2d_BezierCurve( Points );
      TopoDS_Edge E = BRepLib_MakeEdge( Line2d, Surface, Line2d->FirstParameter(), Line2d->LastParameter() );
      
      B.UpdateVertex( TopExp::FirstVertex(E), dmax );
      B.UpdateVertex( TopExp::LastVertex(E), dmax );
      BRepLib::BuildCurve3d( E );
      EdgeMap.Add( E );
    }
  WireMap.Clear();
  BuildWires( EdgeMap, WireMap );
  MapIt.Initialize( WireMap );
  W = TopoDS::Wire( MapIt.Key() );
  
  if (!(W.Closed()))
    Standard_Failure::Raise("Wire is not closed");
    
  myFace = BRepLib_MakeFace( Surface, W );
}

Standard_Boolean BRepFill_Filling::IsDone() const
{
  return myIsDone;
}

TopoDS_Face BRepFill_Filling::Face() const
{
  return myFace;
}

Standard_Real BRepFill_Filling::G0Error() const
{
  return myBuilder.G0Error();
}

Standard_Real BRepFill_Filling::G1Error() const
{
  return myBuilder.G1Error();
}

Standard_Real BRepFill_Filling::G2Error() const
{
  return myBuilder.G2Error();
}

Standard_Real BRepFill_Filling::G0Error( const Standard_Integer Index )
{
  return myBuilder.G0Error( Index );
}

Standard_Real BRepFill_Filling::G1Error( const Standard_Integer Index )
{
  return myBuilder.G1Error( Index );
}

Standard_Real BRepFill_Filling::G2Error( const Standard_Integer Index )
{
  return myBuilder.G2Error( Index );
}
