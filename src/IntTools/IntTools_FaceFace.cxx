// File:      IntTools_FaceFace.cxx
// Created:   Thu Nov 23 14:52:53 2000
// Author:    Michael KLOKOV
// Copyright: OPEN CASCADE 2000


#include <IntTools_FaceFace.ixx>

#include <Precision.hxx>

#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_Array1OfListOfInteger.hxx>

#include <gp_Lin2d.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Torus.hxx>
#include <gp_Cylinder.hxx>

#include <Bnd_Box.hxx>

#include <TColgp_HArray1OfPnt2d.hxx>
#include <TColgp_SequenceOfPnt2d.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>

#include <IntAna_QuadQuadGeo.hxx>

#include <IntSurf_PntOn2S.hxx>
#include <IntSurf_LineOn2S.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <IntSurf_ListOfPntOn2S.hxx>
#include <IntRes2d_Domain.hxx>
#include <ProjLib_Plane.hxx>

#include <IntPatch_GLine.hxx>
#include <IntPatch_RLine.hxx>
#include <IntPatch_WLine.hxx>
#include <IntPatch_ALine.hxx>
#include <IntPatch_ALineToWLine.hxx>

#include <ElSLib.hxx>
#include <ElCLib.hxx>

#include <Extrema_ExtCC.hxx>
#include <Extrema_POnCurv.hxx>
#include <BndLib_AddSurface.hxx>

#include <Adaptor3d_SurfacePtr.hxx>
#include <Adaptor2d_HLine2d.hxx>

#include <GeomAbs_SurfaceType.hxx>
#include <GeomAbs_CurveType.hxx>

#include <Geom_Surface.hxx>
#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Conic.hxx>

#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Circle.hxx>

#include <Geom2dAPI_InterCurveCurve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomLib_CheckBSplineCurve.hxx>
#include <GeomLib_Check2dBSplineCurve.hxx>

#include <GeomInt_WLApprox.hxx>
#include <GeomProjLib.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <Geom2dAdaptor_Curve.hxx>
//
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopExp_Explorer.hxx>

#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepAdaptor_Surface.hxx>

#include <BOPTColStd_Dump.hxx>

#include <IntTools_Curve.hxx>
#include <IntTools_Tools.hxx>
#include <IntTools_Tools.hxx>
#include <IntTools_TopolTool.hxx>
#include <IntTools_PntOnFace.hxx>
#include <IntTools_PntOn2Faces.hxx>
#include <IntTools_Context.hxx>

//
static
  void TolR3d(const TopoDS_Face& ,
	      const TopoDS_Face& ,
	      Standard_Real& );
static 
  Handle(Geom_Curve) MakeBSpline (const Handle(IntPatch_WLine)&,
				  const Standard_Integer,
				  const Standard_Integer);

static 
  void Parameters(const Handle(GeomAdaptor_HSurface)&,
		  const Handle(GeomAdaptor_HSurface)&,
		  const gp_Pnt&,
		  Standard_Real&,
		  Standard_Real&,
		  Standard_Real&,
		  Standard_Real&);

static 
  void BuildPCurves (Standard_Real f,Standard_Real l,Standard_Real& Tol,
		     const Handle (Geom_Surface)& S,
		     const Handle (Geom_Curve)&   C,
		     Handle (Geom2d_Curve)& C2d);

static 
  void CorrectSurfaceBoundaries(const TopoDS_Face&  theFace,
				const Standard_Real theTolerance,
				Standard_Real&      theumin,
				Standard_Real&      theumax, 
				Standard_Real&      thevmin, 
				Standard_Real&      thevmax);
static
  Standard_Boolean NotUseSurfacesForApprox
          (const TopoDS_Face& aF1,
	   const TopoDS_Face& aF2,
	   const Handle(IntPatch_WLine)& WL,
	   const Standard_Integer ifprm,
	   const Standard_Integer ilprm);

static 
  Handle(IntPatch_WLine) ComputePurgedWLine(const Handle(IntPatch_WLine)& theWLine);

static 
  Standard_Real AdjustPeriodic(const Standard_Real theParameter,
			       const Standard_Real parmin,
			       const Standard_Real parmax,
			       const Standard_Real thePeriod,
			       Standard_Real&      theOffset);

static 
  Handle(Geom2d_BSplineCurve) MakeBSpline2d(const Handle(IntPatch_WLine)& theWLine,
					    const Standard_Integer                         ideb,
					    const Standard_Integer                         ifin,
					    const Standard_Boolean                         onFirst);

static 
  Standard_Boolean DecompositionOfWLine(const Handle(IntPatch_WLine)& theWLine,
					const Handle(GeomAdaptor_HSurface)&            theSurface1, 
					const Handle(GeomAdaptor_HSurface)&            theSurface2,
					const TopoDS_Face&                             theFace1,
					const TopoDS_Face&                             theFace2,
					const IntTools_LineConstructor&                theLConstructor,
					const Standard_Boolean                         theAvoidLConstructor,
					IntPatch_SequenceOfLine&                       theNewLines,
					Standard_Real&                                 theReachedTol3d);

static 
  Standard_Boolean ParameterOutOfBoundary(const Standard_Real       theParameter, 
					  const Handle(Geom_Curve)& theCurve, 
					  const TopoDS_Face&        theFace1, 
					  const TopoDS_Face&        theFace2,
					  const Standard_Real       theOtherParameter,
					  const Standard_Boolean    bIncreasePar,
					  Standard_Real&            theNewParameter);

static 
  Standard_Boolean IsCurveValid(Handle(Geom2d_Curve)& thePCurve);

static 
  Standard_Boolean IsPointOnBoundary(const Standard_Real theParameter,
				     const Standard_Real theFirstBoundary,
				     const Standard_Real theSecondBoundary,
				     const Standard_Real theResolution,
				     Standard_Boolean&   IsOnFirstBoundary);
static
  Standard_Boolean FindPoint(const gp_Pnt2d&     theFirstPoint,
			     const gp_Pnt2d&     theLastPoint,
			     const Standard_Real theUmin, 
			     const Standard_Real theUmax,
			     const Standard_Real theVmin,
			     const Standard_Real theVmax,
			     gp_Pnt2d&           theNewPoint);


static 
  Standard_Integer ComputeTangentZones( const Handle(GeomAdaptor_HSurface)& theSurface1,
				       const Handle(GeomAdaptor_HSurface)&  theSurface2,
				       const TopoDS_Face&                   theFace1,
				       const TopoDS_Face&                   theFace2,
				       Handle(TColgp_HArray1OfPnt2d)&       theResultOnS1,
				       Handle(TColgp_HArray1OfPnt2d)&       theResultOnS2,
				       Handle(TColStd_HArray1OfReal)&       theResultRadius);

static
  Standard_Boolean FindPoint(const gp_Pnt2d&     theFirstPoint,
			     const gp_Pnt2d&     theLastPoint,
			     const Standard_Real theUmin, 
			     const Standard_Real theUmax,
			     const Standard_Real theVmin,
			     const Standard_Real theVmax,
			     const gp_Pnt2d&     theTanZoneCenter,
			     const Standard_Real theZoneRadius,
			     Handle(GeomAdaptor_HSurface) theGASurface,
			     gp_Pnt2d&           theNewPoint);

static
  Standard_Boolean IsInsideTanZone(const gp_Pnt2d&     thePoint,
				   const gp_Pnt2d&     theTanZoneCenter,
				   const Standard_Real theZoneRadius,
				   Handle(GeomAdaptor_HSurface) theGASurface);

static
gp_Pnt2d AdjustByNeighbour(const gp_Pnt2d&     theaNeighbourPoint,
			   const gp_Pnt2d&     theOriginalPoint,
			   Handle(GeomAdaptor_HSurface) theGASurface);
static
Standard_Boolean  ApproxWithPCurves(const gp_Cylinder& theCyl, 
				    const gp_Sphere& theSph);

static void  PerformPlanes(const Handle(GeomAdaptor_HSurface)& theS1, 
			   const Handle(GeomAdaptor_HSurface)& theS2, 
			   const Standard_Real TolAng, 
			   const Standard_Real TolTang, 
			   const Standard_Boolean theApprox1,
			   const Standard_Boolean theApprox2,
                           IntTools_SequenceOfCurves& theSeqOfCurve, 
			   Standard_Boolean& theTangentFaces);

static Standard_Boolean ClassifyLin2d(const Handle(GeomAdaptor_HSurface)& theS, 
				      const gp_Lin2d& theLin2d, 
				      const Standard_Real theTol,
				      Standard_Real& theP1, 
				      Standard_Real& theP2);

static
  void ApproxParameters(const Handle(GeomAdaptor_HSurface)& aHS1,
			const Handle(GeomAdaptor_HSurface)& aHS2,
			Standard_Integer& iDegMin,
			Standard_Integer& iDegMax,
			//modified by NIZNHY-PKV Thu Sep 23 08:38:47 2010f
			Standard_Integer& iNbIterMax,
			Standard_Boolean& anWithPC
			//modified by NIZNHY-PKV Thu Sep 23 08:38:55 2010t
			);

//
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
  IntTools_FaceFace::IntTools_FaceFace()
{
  myTangentFaces=Standard_False;
  //
  myHS1 = new GeomAdaptor_HSurface ();
  myHS2 = new GeomAdaptor_HSurface ();
  myTolReached2d=0.; 
  myTolReached3d=0.;
  SetParameters(Standard_True, Standard_True, Standard_True, 1.e-07);
}
//=======================================================================
//function : Face1
//purpose  : 
//======================================================================= 
  const TopoDS_Face&  IntTools_FaceFace::Face1() const
{
  return myFace1;
}

//=======================================================================
//function : Face2
//purpose  : 
//======================================================================= 
  const TopoDS_Face&  IntTools_FaceFace::Face2() const
{
  return myFace2;
}

//=======================================================================
//function : TangentFaces
//purpose  : 
//======================================================================= 
  Standard_Boolean IntTools_FaceFace::TangentFaces() const
{
  return myTangentFaces;
}
//=======================================================================
//function : Points
//purpose  : 
//======================================================================= 
  const  IntTools_SequenceOfPntOn2Faces& IntTools_FaceFace::Points() const
{
  return myPnts;
}
//=======================================================================
//function : IsDone
//purpose  : 
//======================================================================= 
  Standard_Boolean IntTools_FaceFace::IsDone() const
{
  return myIsDone;
}
//=======================================================================
//function : TolReached3d
//purpose  : 
//=======================================================================
  Standard_Real IntTools_FaceFace::TolReached3d() const
{
  return myTolReached3d;
}
//=======================================================================
//function : Lines
//purpose  : return lines of intersection
//=======================================================================
  const IntTools_SequenceOfCurves& IntTools_FaceFace::Lines() const
{
  StdFail_NotDone_Raise_if(!myIsDone,
			   "IntTools_FaceFace::Lines() => !myIntersector.IsDone()");
  return mySeqOfCurve;
}

//=======================================================================
//function : TolReached2d
//purpose  : 
//=======================================================================
  Standard_Real IntTools_FaceFace::TolReached2d() const
{
  return myTolReached2d;
}
// =======================================================================
// function: SetParameters
//
// =======================================================================
  void IntTools_FaceFace::SetParameters(const Standard_Boolean ToApproxC3d,
					const Standard_Boolean ToApproxC2dOnS1,
					const Standard_Boolean ToApproxC2dOnS2,
					const Standard_Real ApproximationTolerance) 
{
  myApprox = ToApproxC3d;
  myApprox1 = ToApproxC2dOnS1;
  myApprox2 = ToApproxC2dOnS2;
  myTolApprox = ApproximationTolerance;
}
//=======================================================================
//function : SetList
//purpose  : 
//=======================================================================

void IntTools_FaceFace::SetList(IntSurf_ListOfPntOn2S& aListOfPnts)
{
  myListOfPnts = aListOfPnts;  
}
//=======================================================================
//function : Perform
//purpose  : intersect surfaces of the faces
//=======================================================================
  void IntTools_FaceFace::Perform(const TopoDS_Face& F1,
				  const TopoDS_Face& F2)
{
  Standard_Boolean hasCone, RestrictLine, bTwoPlanes;
  Standard_Integer aNbLin, aNbPnts, i;
  Standard_Real TolArc, TolTang, Deflection, UVMaxStep;
  Standard_Real umin, umax, vmin, vmax;
  Standard_Real aTolF1, aTolF2;
  GeomAbs_SurfaceType aType1, aType2;
  Handle(Geom_Surface) S1, S2;
  Handle(IntTools_TopolTool) dom1, dom2;
  //
  myFace1= F1;
  myFace2= F2;
  mySeqOfCurve.Clear();
  //
  myTolReached2d=0.;
  myTolReached3d=0.;
  //
  myIsDone = Standard_False;
  //
  S1 = BRep_Tool::Surface(F1);
  S2 = BRep_Tool::Surface(F2);
  aTolF1 = BRep_Tool::Tolerance(F1);
  aTolF2 = BRep_Tool::Tolerance(F2);
  //
  TolArc= aTolF1 + aTolF2;
  TolTang = TolArc;
  //
  myNbrestr=0;//?
  // 
  BRepAdaptor_Surface aBAS1(F1, Standard_False);
  BRepAdaptor_Surface aBAS2(F2, Standard_False);
  //
  aType1=aBAS1.GetType();
  aType2=aBAS2.GetType();
  hasCone = Standard_False;
  bTwoPlanes = Standard_False;

  Standard_Integer NbLinPP = 0;
  if(aType1==GeomAbs_Plane && aType2==GeomAbs_Plane){
    bTwoPlanes = Standard_True;

    BRepTools::UVBounds(F1, umin, umax, vmin, vmax);
    myHS1->ChangeSurface().Load(S1, umin, umax, vmin, vmax);
    //
    BRepTools::UVBounds(F2, umin, umax, vmin, vmax);
    myHS2->ChangeSurface().Load(S2, umin, umax, vmin, vmax);
    Standard_Real TolAng = 1.e-8;
    PerformPlanes(myHS1, myHS2, TolAng, TolTang, 
		  myApprox1, myApprox2, 
		  mySeqOfCurve, myTangentFaces);

    myIsDone = Standard_True;

    if(myTangentFaces) return;

    NbLinPP = mySeqOfCurve.Length();
    if(NbLinPP == 0) return;


    Standard_Real aTolFMax;
    //
    myTolReached3d = 1.e-7;
    //
    aTolFMax=Max(aTolF1, aTolF2);
    //
    if (aTolFMax>myTolReached3d) {
      myTolReached3d=aTolFMax;
    }
    myTolReached2d = myTolReached3d;

    return;
       
  }
  //
  if (aType1==GeomAbs_Plane && 
      (aType2==GeomAbs_Cylinder ||
       aType2==GeomAbs_Cone ||
       aType2==GeomAbs_Torus)) {
    Standard_Real dU, dV;
    // F1
    BRepTools::UVBounds(F1, umin, umax, vmin, vmax);
    dU=0.1*(umax-umin);
    dV=0.1*(vmax-vmin);
    umin=umin-dU;
    umax=umax+dU;
    vmin=vmin-dV;
    vmax=vmax+dV;
    myHS1->ChangeSurface().Load(S1, umin, umax, vmin, vmax);
    // F2
    BRepTools::UVBounds(F2, umin, umax, vmin, vmax);
    CorrectSurfaceBoundaries(F2, (aTolF1 + aTolF2) * 2., umin, umax, vmin, vmax);
    myHS2->ChangeSurface().Load(S2, umin, umax, vmin, vmax);
    //
    if( aType2==GeomAbs_Cone ) { 
      TolArc = 0.0001; 
      //TolArc = TolTang = 0.0001; 
      hasCone = Standard_True; 
    }
  }
  //
  else if ((aType1==GeomAbs_Cylinder||
	    aType1==GeomAbs_Cone ||
	    aType1==GeomAbs_Torus) && 
	   aType2==GeomAbs_Plane) {
    Standard_Real dU, dV;
    //F1
    BRepTools::UVBounds(F1, umin, umax, vmin, vmax);
    CorrectSurfaceBoundaries(F1, (aTolF1 + aTolF2) * 2., umin, umax, vmin, vmax);
    myHS1->ChangeSurface().Load(S1, umin, umax, vmin, vmax);
    // F2
    BRepTools::UVBounds(F2, umin, umax, vmin, vmax);
    dU=0.1*(umax-umin);
    dV=0.1*(vmax-vmin);
    umin=umin-dU;
    umax=umax+dU;
    vmin=vmin-dV;
    vmax=vmax+dV;
    myHS2->ChangeSurface().Load(S2, umin, umax, vmin, vmax);
    //
    if( aType1==GeomAbs_Cone ) {
      TolArc = 0.0001; 
      //TolArc = TolTang = 0.0001; 
      hasCone = Standard_True; 
    }
  }
  
  //
  else {
    BRepTools::UVBounds(F1, umin, umax, vmin, vmax);
      //
    CorrectSurfaceBoundaries(F1, (aTolF1 + aTolF2) * 2., umin, umax, vmin, vmax);
    // 
    myHS1->ChangeSurface().Load(S1, umin, umax, vmin, vmax);
    //
    BRepTools::UVBounds(F2, umin, umax, vmin, vmax);
    // 
    CorrectSurfaceBoundaries(F2, (aTolF1 + aTolF2) * 2., umin, umax, vmin, vmax);
    //   
    myHS2->ChangeSurface().Load(S2, umin, umax, vmin, vmax);
  }
  //
  dom1 = new IntTools_TopolTool(myHS1);
  dom2 = new IntTools_TopolTool(myHS2);
  //
  myLConstruct.Load(dom1, dom2, myHS1, myHS2);
  //
  Deflection = (hasCone) ? 0.085 : 0.1;
  UVMaxStep  = 0.001;
  //
  myIntersector.SetTolerances(TolArc, TolTang, UVMaxStep, Deflection);
  //
  RestrictLine = Standard_False;
  //
  if((myHS1->IsUClosed() && !myHS1->IsUPeriodic()) || 
     (myHS1->IsVClosed() && !myHS1->IsVPeriodic()) ||
     (myHS2->IsUClosed() && !myHS2->IsUPeriodic()) || 
     (myHS2->IsVClosed() && !myHS2->IsVPeriodic())) {
    RestrictLine = Standard_True;
  }
  //
  if(((aType1 != GeomAbs_BSplineSurface) &&
      (aType1 != GeomAbs_BezierSurface)  &&
      (aType1 != GeomAbs_OtherSurface))  &&
     ((aType2 != GeomAbs_BSplineSurface) &&
      (aType2 != GeomAbs_BezierSurface)  &&
      (aType2 != GeomAbs_OtherSurface))) {
    RestrictLine = Standard_True;
    //
    if ((aType1 == GeomAbs_Torus) ||
	(aType2 == GeomAbs_Torus) ) {
      myListOfPnts.Clear();
    }
  }
  //
  if(!RestrictLine) {
    TopExp_Explorer aExp;
    //
    for(i = 0; (!RestrictLine) && (i < 2); i++) {
      const TopoDS_Face& aF=(!i) ? myFace1 : myFace2;
      aExp.Init(aF, TopAbs_EDGE);
      for(; aExp.More(); aExp.Next()) {
	const TopoDS_Edge& aE=TopoDS::Edge(aExp.Current());
	//
	if(BRep_Tool::Degenerated(aE)) {
	  RestrictLine = Standard_True;
	  break;
	}
      }
    }
  }
  //
  myIntersector.Perform(myHS1, dom1, myHS2, dom2, 
			TolArc, TolTang, 
			myListOfPnts, RestrictLine); 
  //
  myIsDone = myIntersector.IsDone();
  if (myIsDone) {
    myTangentFaces=myIntersector.TangentFaces();
    if (myTangentFaces) {
      return;
    }
    //
    if(RestrictLine) {
      myListOfPnts.Clear(); // to use LineConstructor
    }
    //
    aNbLin = myIntersector.NbLines();
    for (i=1; i<=aNbLin; ++i) {
      MakeCurve(i, dom1, dom2);
    }
    //
    ComputeTolReached3d();
    //
    Standard_Real U1,V1,U2,V2;
    IntTools_PntOnFace aPntOnF1, aPntOnF2;
    //
    aNbPnts=myIntersector.NbPnts();
    for (i=1; i<=aNbPnts; ++i) {
      const IntSurf_PntOn2S& aISPnt=myIntersector.Point(i).PntOn2S();
      const gp_Pnt& aPnt=aISPnt.Value();
      aISPnt.Parameters(U1,V1,U2,V2);
      
      aPntOnF1.Init(myFace1, aPnt, U1,V1);
      aPntOnF2.Init(myFace2, aPnt, U2,V2);

      IntTools_PntOn2Faces aPntOn2Faces(aPntOnF1, aPntOnF2);
      myPnts.Append(aPntOn2Faces);
    }
    //
  }
}
//=======================================================================
//function :ComputeTolReached3d 
//purpose  : 
//=======================================================================
  void IntTools_FaceFace::ComputeTolReached3d()
{
  Standard_Integer aNbLin;
  GeomAbs_SurfaceType aType1, aType2;
  //
  aNbLin=myIntersector.NbLines();
  //
  aType1=myHS1->Surface().GetType();
  aType2=myHS2->Surface().GetType();
  //
  if (aNbLin==2 &&
      aType1==GeomAbs_Cylinder &&
      aType2==GeomAbs_Cylinder) {
    Handle(IntPatch_Line) aIL1, aIL2;
    IntPatch_IType aTL1, aTL2;
    //
    aIL1=myIntersector.Line(1);
    aIL2=myIntersector.Line(2);
    aTL1=aIL1->ArcType();
    aTL2=aIL2->ArcType();
    if (aTL1==IntPatch_Lin && aTL2==IntPatch_Lin) {
      Standard_Real aD, aDTresh, dTol;
      gp_Lin aL1, aL2;
      //
      dTol=1.e-8;
      aDTresh=1.5e-6;
      //
      aL1=Handle(IntPatch_GLine)::DownCast(aIL1)->Line();
      aL2=Handle(IntPatch_GLine)::DownCast(aIL2)->Line();
      aD=aL1.Distance(aL2);
      aD=0.5*aD;
      if (aD<aDTresh) {
	myTolReached3d=aD+dTol;
      }
    }
  }
  //PKV  904/G3 f
  if (aType1==GeomAbs_Plane &&
      aType2==GeomAbs_Plane) {
    Standard_Real aTolF1, aTolF2, aTolFMax, aTolTresh;
    //
    aTolTresh=1.e-7;
    //
    aTolF1 = BRep_Tool::Tolerance(myFace1);
    aTolF2 = BRep_Tool::Tolerance(myFace2);
    aTolFMax=Max(aTolF1, aTolF2);
    //
    if (aTolFMax>aTolTresh) {
      myTolReached3d=aTolFMax;
    }
  }
  //PKV t

  //IFV Bug OCC20297 
  if((aType1 == GeomAbs_Cylinder && aType2 == GeomAbs_Plane) ||
     (aType2 == GeomAbs_Cylinder && aType1 == GeomAbs_Plane)) {
    if(aNbLin == 1) {
      const Handle(IntPatch_Line)& aIL1 = myIntersector.Line(1);
      if(aIL1->ArcType() == IntPatch_Circle) {
	gp_Circ aCir = Handle(IntPatch_GLine)::DownCast(aIL1)->Circle();
	gp_XYZ aCirDir = aCir.Axis().Direction().XYZ();
	gp_XYZ aPlDir;
	gp_Pln aPln;
	if(aType1 == GeomAbs_Plane) {
	  aPln = myHS1->Surface().Plane();
	}
	else {
	  aPln = myHS2->Surface().Plane();
	}
	aPlDir = aPln.Axis().Direction().XYZ();
	Standard_Real cs = aCirDir*aPlDir;
	if(cs < 0.) aPlDir.Reverse();
	Standard_Real eps = 1.e-14;
	if(!aPlDir.IsEqual(aCirDir, eps)) {
	  Standard_Integer aNbP = 11;
	  Standard_Real dt = 2.*PI / (aNbP - 1), t;
	  for(t = 0.; t < 2.*PI; t += dt) {
	    Standard_Real d = aPln.Distance(ElCLib::Value(t, aCir)); 
	    if(myTolReached3d < d) myTolReached3d = d;
	  }
	  myTolReached3d *= 1.1;
	}
      } //aIL1->ArcType() == IntPatch_Circle
    } //aNbLin == 1
  } // aType1 == GeomAbs_Cylinder && aType2 == GeomAbs_Plane) ...
  //End IFV Bug OCC20297
}
//=======================================================================
//function : MakeCurve
//purpose  : 
//=======================================================================
  void IntTools_FaceFace::MakeCurve(const Standard_Integer Index,
				    const Handle(Adaptor3d_TopolTool)& dom1,
				    const Handle(Adaptor3d_TopolTool)& dom2) 
{
  Standard_Boolean bDone, rejectSurface, reApprox, bAvoidLineConstructor;
  Standard_Boolean ok;
  Standard_Integer i, j, aNbParts;
  Standard_Real fprm, lprm;
  Standard_Real Tolpc;
  Handle(IntPatch_Line) L;
  IntPatch_IType typl;
  Handle(Geom_Curve) newc;
  //
  const Standard_Real TOLCHECK   =0.0000001;
  const Standard_Real TOLANGCHECK=0.1;
  //
  rejectSurface = Standard_False;
  reApprox = Standard_False;
 
 reapprox:;
  
  Tolpc = myTolApprox;
  bAvoidLineConstructor = Standard_False;
  L = myIntersector.Line(Index);
  typl = L->ArcType();
  //
  if(typl==IntPatch_Walking) {
    Handle(IntPatch_Line) anewL;
    //
    const Handle(IntPatch_WLine)& aWLine=
      Handle(IntPatch_WLine)::DownCast(L);
    //
    anewL = ComputePurgedWLine(aWLine);
    if(anewL.IsNull()) {
      return;
    }
    L = anewL;
    //
    if(!myListOfPnts.IsEmpty()) {
      bAvoidLineConstructor = Standard_True;
    }

    Standard_Integer nbp = aWLine->NbPnts();
    const IntSurf_PntOn2S& p1 = aWLine->Point(1);
    const IntSurf_PntOn2S& p2 = aWLine->Point(nbp);

    const gp_Pnt& P1 = p1.Value();
    const gp_Pnt& P2 = p2.Value();

    if(P1.SquareDistance(P2) < 1.e-14) {
      bAvoidLineConstructor = Standard_False;
    }

  }
  //
  // Line Constructor
  if(!bAvoidLineConstructor) {
    myLConstruct.Perform(L);
    //
    bDone=myLConstruct.IsDone();
    aNbParts=myLConstruct.NbParts();
    if (!bDone|| !aNbParts) {
      return;
    }
  }
  // Do the Curve
  
  
  typl=L->ArcType();
  switch (typl) {
  //########################################  
  // Line, Parabola, Hyperbola
  //########################################  
  case IntPatch_Lin:
  case IntPatch_Parabola: 
  case IntPatch_Hyperbola: {
    if (typl == IntPatch_Lin) {
      newc = 
	new Geom_Line (Handle(IntPatch_GLine)::DownCast(L)->Line());
    }

    else if (typl == IntPatch_Parabola) {
      newc = 
	new Geom_Parabola(Handle(IntPatch_GLine)::DownCast(L)->Parabola());
    }
    
    else if (typl == IntPatch_Hyperbola) {
      newc = 
	new Geom_Hyperbola (Handle(IntPatch_GLine)::DownCast(L)->Hyperbola());
    }
    //
    // myTolReached3d
    if (typl == IntPatch_Lin) {
      TolR3d (myFace1, myFace2, myTolReached3d);
    }
    //
    aNbParts=myLConstruct.NbParts();
    for (i=1; i<=aNbParts; i++) {
      myLConstruct.Part(i, fprm, lprm);
      
      if (!Precision::IsNegativeInfinite(fprm) && 
	  !Precision::IsPositiveInfinite(lprm)) {
	//
	IntTools_Curve aCurve;
	//
	Handle(Geom_TrimmedCurve) aCT3D=new Geom_TrimmedCurve(newc, fprm, lprm);
	aCurve.SetCurve(aCT3D);
	if (typl == IntPatch_Parabola) {
	  Standard_Real aTolF1, aTolF2, aTolBase;
	  
	  aTolF1 = BRep_Tool::Tolerance(myFace1);
	  aTolF2 = BRep_Tool::Tolerance(myFace2);
	  aTolBase=aTolF1+aTolF2;
	  myTolReached3d=IntTools_Tools::CurveTolerance(aCT3D, aTolBase);
	}
	//
	aCurve.SetCurve(new Geom_TrimmedCurve(newc, fprm, lprm));
	if(myApprox1) { 
	  Handle (Geom2d_Curve) C2d;
	  BuildPCurves(fprm, lprm, Tolpc, myHS1->ChangeSurface().Surface(), newc, C2d);
	  if(Tolpc>myTolReached2d || myTolReached2d==0.) { 
	    myTolReached2d=Tolpc;
	  }
	    // 	   
	    aCurve.SetFirstCurve2d(new Geom2d_TrimmedCurve(C2d,fprm,lprm));
	  }
	  else { 
	    Handle(Geom2d_BSplineCurve) H1;
	    //
	    aCurve.SetFirstCurve2d(H1);
	  }
	
	if(myApprox2) { 
	  Handle (Geom2d_Curve) C2d;
	  BuildPCurves(fprm,lprm,Tolpc,myHS2->ChangeSurface().Surface(),newc,C2d);
	  if(Tolpc>myTolReached2d || myTolReached2d==0.) { 
	    myTolReached2d=Tolpc;
	  }
	  //
	  aCurve.SetSecondCurve2d(new Geom2d_TrimmedCurve(C2d,fprm,lprm));
	  }
	else { 
	  Handle(Geom2d_BSplineCurve) H1;
	  //
	  aCurve.SetSecondCurve2d(H1);
	}
	mySeqOfCurve.Append(aCurve);
      } // end of if (!Precision::IsNegativeInfinite(fprm) &&  !Precision::IsPositiveInfinite(lprm))

      else {
	//  on regarde si on garde
	//
	Standard_Boolean bFNIt, bLPIt;
	Standard_Real aTestPrm, dT=100.;

	bFNIt=Precision::IsNegativeInfinite(fprm);
	bLPIt=Precision::IsPositiveInfinite(lprm);
	
	aTestPrm=0.;
	
	if (bFNIt && !bLPIt) {
	  aTestPrm=lprm-dT;
	}
	else if (!bFNIt && bLPIt) {
	  aTestPrm=fprm+dT;
	}
	
	gp_Pnt ptref(newc->Value(aTestPrm));
	//

	Standard_Real u1, v1, u2, v2, Tol;
	
	Tol = Precision::Confusion();
	Parameters(myHS1, myHS2, ptref,  u1, v1, u2, v2);
	ok = (dom1->Classify(gp_Pnt2d(u1, v1), Tol) != TopAbs_OUT);
	if(ok) { 
	  ok = (dom2->Classify(gp_Pnt2d(u2,v2),Tol) != TopAbs_OUT); 
	}
	if (ok) {
	  Handle(Geom2d_BSplineCurve) H1;
	  mySeqOfCurve.Append(IntTools_Curve(newc, H1, H1));
	}
      }
    }// end of for (i=1; i<=myLConstruct.NbParts(); i++)
  }// case IntPatch_Lin:  case IntPatch_Parabola:  case IntPatch_Hyperbola:
    break;

  //########################################  
  // Circle and Ellipse
  //########################################  
  case IntPatch_Circle: 
  case IntPatch_Ellipse: {

    if (typl == IntPatch_Circle) {
      newc = new Geom_Circle
	(Handle(IntPatch_GLine)::DownCast(L)->Circle());
    }
    else { //IntPatch_Ellipse
      newc = new Geom_Ellipse
	(Handle(IntPatch_GLine)::DownCast(L)->Ellipse());
    }
    //
    // myTolReached3d
    TolR3d (myFace1, myFace2, myTolReached3d);
    //
    aNbParts=myLConstruct.NbParts();
    //
    Standard_Real aPeriod, aNul;
    TColStd_SequenceOfReal aSeqFprm,  aSeqLprm;
    
    aNul=0.;
    aPeriod=PI+PI;

    for (i=1; i<=aNbParts; i++) {
      myLConstruct.Part(i, fprm, lprm);

      if (fprm < aNul && lprm > aNul) {
	// interval that goes through 0. is divided on two intervals;
	while (fprm<aNul || fprm>aPeriod) fprm=fprm+aPeriod;
	while (lprm<aNul || lprm>aPeriod) lprm=lprm+aPeriod;
	//
	if((aPeriod - fprm) > Tolpc) {
	  aSeqFprm.Append(fprm);
	  aSeqLprm.Append(aPeriod);
	}
	else {
	  gp_Pnt P1 = newc->Value(fprm);
	  gp_Pnt P2 = newc->Value(aPeriod);
	  Standard_Real aTolDist = BRep_Tool::Tolerance(myFace1) + BRep_Tool::Tolerance(myFace2);
	  aTolDist = (myTolReached3d > aTolDist) ? myTolReached3d : aTolDist;

	  if(P1.Distance(P2) > aTolDist) {
	    Standard_Real anewpar = fprm;

	    if(ParameterOutOfBoundary(fprm, newc, myFace1, myFace2, lprm, Standard_False, anewpar)) {
	      fprm = anewpar;
	    }
	    aSeqFprm.Append(fprm);
	    aSeqLprm.Append(aPeriod);
	  }
	}

	//
	if((lprm - aNul) > Tolpc) {
	  aSeqFprm.Append(aNul);
	  aSeqLprm.Append(lprm);
	}
	else {
	  gp_Pnt P1 = newc->Value(aNul);
	  gp_Pnt P2 = newc->Value(lprm);
	  Standard_Real aTolDist = BRep_Tool::Tolerance(myFace1) + BRep_Tool::Tolerance(myFace2);
	  aTolDist = (myTolReached3d > aTolDist) ? myTolReached3d : aTolDist;

	  if(P1.Distance(P2) > aTolDist) {
	    Standard_Real anewpar = lprm;

	    if(ParameterOutOfBoundary(lprm, newc, myFace1, myFace2, fprm, Standard_True, anewpar)) {
	      lprm = anewpar;
	    }
	    aSeqFprm.Append(aNul);
	    aSeqLprm.Append(lprm);
	  }
	}
      }
      else {
	// usual interval 
	aSeqFprm.Append(fprm);
	aSeqLprm.Append(lprm);
      }
    }
    
    //
    aNbParts=aSeqFprm.Length();
    for (i=1; i<=aNbParts; i++) {
      fprm=aSeqFprm(i);
      lprm=aSeqLprm(i);
      //
      Standard_Real aRealEpsilon=RealEpsilon();
      if (Abs(fprm) > aRealEpsilon || Abs(lprm-2.*PI) > aRealEpsilon) {
	//==============================================
	////
	IntTools_Curve aCurve;
	Handle(Geom_TrimmedCurve) aTC3D=new Geom_TrimmedCurve(newc,fprm,lprm);
	aCurve.SetCurve(aTC3D);
	fprm=aTC3D->FirstParameter();
	lprm=aTC3D->LastParameter ();
	//// 	
	if (typl == IntPatch_Circle || typl == IntPatch_Ellipse) {//// 
	  if(myApprox1) { 
	    Handle (Geom2d_Curve) C2d;
	    BuildPCurves(fprm,lprm,Tolpc,myHS1->ChangeSurface().Surface(),newc,C2d);
	    if(Tolpc>myTolReached2d || myTolReached2d==0) { 
	      myTolReached2d=Tolpc;
	    }
	    //
	    aCurve.SetFirstCurve2d(C2d);
	  }
	  else { //// 
	    Handle(Geom2d_BSplineCurve) H1;
	    aCurve.SetFirstCurve2d(H1);
	  }


	  if(myApprox2) { 
	    Handle (Geom2d_Curve) C2d;
	    BuildPCurves(fprm,lprm,Tolpc,myHS2->ChangeSurface().Surface(),newc,C2d);
	    if(Tolpc>myTolReached2d || myTolReached2d==0) { 
	      myTolReached2d=Tolpc;
	    }
	    //
	    aCurve.SetSecondCurve2d(C2d);
	  }
	  else { 
	    Handle(Geom2d_BSplineCurve) H1;
	    aCurve.SetSecondCurve2d(H1);
	  }
	}
	
	else { 
	  Handle(Geom2d_BSplineCurve) H1;
	  aCurve.SetFirstCurve2d(H1);
	  aCurve.SetSecondCurve2d(H1);
	}
	mySeqOfCurve.Append(aCurve);
	  //==============================================	
      } //if (Abs(fprm) > RealEpsilon() || Abs(lprm-2.*PI) > RealEpsilon())

      else {
	//  on regarde si on garde
	//
	if (aNbParts==1) {
// 	  if (Abs(fprm) < RealEpsilon() &&  Abs(lprm-2.*PI) < RealEpsilon()) {
	  if (Abs(fprm) <= aRealEpsilon && Abs(lprm-2.*PI) <= aRealEpsilon) {
	    IntTools_Curve aCurve;
	    Handle(Geom_TrimmedCurve) aTC3D=new Geom_TrimmedCurve(newc,fprm,lprm);
	    aCurve.SetCurve(aTC3D);
	    fprm=aTC3D->FirstParameter();
	    lprm=aTC3D->LastParameter ();
	    
	    if(myApprox1) { 
	      Handle (Geom2d_Curve) C2d;
	      BuildPCurves(fprm,lprm,Tolpc,myHS1->ChangeSurface().Surface(),newc,C2d);
	      if(Tolpc>myTolReached2d || myTolReached2d==0) { 
		myTolReached2d=Tolpc;
	      }
	      //
	      aCurve.SetFirstCurve2d(C2d);
	    }
	    else { //// 
	      Handle(Geom2d_BSplineCurve) H1;
	      aCurve.SetFirstCurve2d(H1);
	    }

	    if(myApprox2) { 
	      Handle (Geom2d_Curve) C2d;
	      BuildPCurves(fprm,lprm,Tolpc,myHS2->ChangeSurface().Surface(),newc,C2d);
	      if(Tolpc>myTolReached2d || myTolReached2d==0) { 
		myTolReached2d=Tolpc;
	      }
	      //
	      aCurve.SetSecondCurve2d(C2d);
	    }
	    else { 
	      Handle(Geom2d_BSplineCurve) H1;
	      aCurve.SetSecondCurve2d(H1);
	    }
	    mySeqOfCurve.Append(aCurve);
	    break;
	  }
	}
	//
	Standard_Real aTwoPIdiv17, u1, v1, u2, v2, Tol;

	aTwoPIdiv17=2.*PI/17.;

	for (j=0; j<=17; j++) {
	  gp_Pnt ptref (newc->Value (j*aTwoPIdiv17));
	  Tol = Precision::Confusion();

	  Parameters(myHS1, myHS2, ptref, u1, v1, u2, v2);
	  ok = (dom1->Classify(gp_Pnt2d(u1,v1),Tol) != TopAbs_OUT);
	  if(ok) { 
	    ok = (dom2->Classify(gp_Pnt2d(u2,v2),Tol) != TopAbs_OUT);
	  }
	  if (ok) {
	    IntTools_Curve aCurve;
	    aCurve.SetCurve(newc);
	    //==============================================
	    if (typl == IntPatch_Circle || typl == IntPatch_Ellipse) {
	      
	      if(myApprox1) { 
		Handle (Geom2d_Curve) C2d;
		BuildPCurves(fprm, lprm, Tolpc, myHS1->ChangeSurface().Surface(), newc, C2d);
		if(Tolpc>myTolReached2d || myTolReached2d==0) { 
		  myTolReached2d=Tolpc;
		}
		// 
		aCurve.SetFirstCurve2d(C2d);
	      }
	      else { 
		Handle(Geom2d_BSplineCurve) H1;
		aCurve.SetFirstCurve2d(H1);
	      }
		
	      if(myApprox2) { 
		Handle (Geom2d_Curve) C2d;
		BuildPCurves(fprm, lprm, Tolpc,myHS2->ChangeSurface().Surface(), newc, C2d);
		if(Tolpc>myTolReached2d || myTolReached2d==0) { 
		  myTolReached2d=Tolpc;
		}
		// 		
		aCurve.SetSecondCurve2d(C2d);
	      }
		
	      else { 
		Handle(Geom2d_BSplineCurve) H1;
		aCurve.SetSecondCurve2d(H1);
	      }
	    }//  end of if (typl == IntPatch_Circle || typl == IntPatch_Ellipse)
	     
	    else { 
	      Handle(Geom2d_BSplineCurve) H1;
	      // 	
	      aCurve.SetFirstCurve2d(H1);
	      aCurve.SetSecondCurve2d(H1);
	    }
	    //==============================================	
	    //
	    mySeqOfCurve.Append(aCurve);
	    break;

	    }//  end of if (ok) {
	  }//  end of for (Standard_Integer j=0; j<=17; j++)
	}//  end of else { on regarde si on garde
      }// for (i=1; i<=myLConstruct.NbParts(); i++)
    }// IntPatch_Circle: IntPatch_Ellipse:
    break;
    
  case IntPatch_Analytic: {
    IntSurf_Quadric quad1,quad2;
    GeomAbs_SurfaceType typs = myHS1->Surface().GetType();
    
    switch (typs) {
      case GeomAbs_Plane:
        quad1.SetValue(myHS1->Surface().Plane());
	break;
      case GeomAbs_Cylinder:
	quad1.SetValue(myHS1->Surface().Cylinder());
	break;
      case GeomAbs_Cone:
	quad1.SetValue(myHS1->Surface().Cone());
	break;
      case GeomAbs_Sphere:
	quad1.SetValue(myHS1->Surface().Sphere());
	break;
      default:
	Standard_ConstructionError::Raise("GeomInt_IntSS::MakeCurve 1");
      }
      
    typs = myHS2->Surface().GetType();
    
    switch (typs) {
      case GeomAbs_Plane:
        quad2.SetValue(myHS2->Surface().Plane());
	break;
      case GeomAbs_Cylinder:
	quad2.SetValue(myHS2->Surface().Cylinder());
	break;
      case GeomAbs_Cone:
	quad2.SetValue(myHS2->Surface().Cone());
	break;
      case GeomAbs_Sphere:
	quad2.SetValue(myHS2->Surface().Sphere());
	break;
      default:
	Standard_ConstructionError::Raise("GeomInt_IntSS::MakeCurve 2");
      }
    //
    //=========
    IntPatch_ALineToWLine convert (quad1, quad2);
      
    if (!myApprox) {
      aNbParts=myLConstruct.NbParts();
      for (i=1; i<=aNbParts; i++) {
	myLConstruct.Part(i, fprm, lprm);
	Handle(IntPatch_WLine) WL = 
	  convert.MakeWLine(Handle(IntPatch_ALine)::DownCast(L), fprm, lprm);
	//
	Handle(Geom2d_BSplineCurve) H1;
	Handle(Geom2d_BSplineCurve) H2;

	if(myApprox1) {
	  H1 = MakeBSpline2d(WL, 1, WL->NbPnts(), Standard_True);
	}
	
	if(myApprox2) {
	  H2 = MakeBSpline2d(WL, 1, WL->NbPnts(), Standard_False);
	}
	// 	 
	mySeqOfCurve.Append(IntTools_Curve(MakeBSpline(WL,1,WL->NbPnts()), H1, H2));
      }
    } // if (!myApprox)

    else { // myApprox=TRUE
      GeomInt_WLApprox theapp3d;
      // 
      Standard_Real tol2d = myTolApprox;
      // 	
      theapp3d.SetParameters(myTolApprox, tol2d, 4, 8, 0, Standard_True);
      
      aNbParts=myLConstruct.NbParts();
      for (i=1; i<=aNbParts; i++) {
	myLConstruct.Part(i, fprm, lprm);
	Handle(IntPatch_WLine) WL = 
	  convert.MakeWLine(Handle(IntPatch_ALine):: DownCast(L),fprm,lprm);

	theapp3d.Perform(myHS1,myHS2,WL,Standard_True,myApprox1,myApprox2, 1, WL->NbPnts());

	if (!theapp3d.IsDone()) {
	  //
	  Handle(Geom2d_BSplineCurve) H1;
	  Handle(Geom2d_BSplineCurve) H2;

	  if(myApprox1) {
	    H1 = MakeBSpline2d(WL, 1, WL->NbPnts(), Standard_True);
	  }
	  
	  if(myApprox2) {
	    H2 = MakeBSpline2d(WL, 1, WL->NbPnts(), Standard_False);
	  }
	  // 	 
	  mySeqOfCurve.Append(IntTools_Curve(MakeBSpline(WL,1,WL->NbPnts()), H1, H2));
	}

	else {
	  if(myApprox1 || myApprox2) { 
	    if( theapp3d.TolReached2d()>myTolReached2d || myTolReached2d==0) { 
	      myTolReached2d = theapp3d.TolReached2d();
	    }
	  }
	  
	  if( theapp3d.TolReached3d()>myTolReached3d || myTolReached3d==0) { 
	    myTolReached3d = theapp3d.TolReached3d();
	  }

	  Standard_Integer aNbMultiCurves, nbpoles;
	  aNbMultiCurves=theapp3d.NbMultiCurves();
	  for (j=1; j<=aNbMultiCurves; j++) {
	    const AppParCurves_MultiBSpCurve& mbspc = theapp3d.Value(j);
	    nbpoles = mbspc.NbPoles();
	    
	    TColgp_Array1OfPnt tpoles(1, nbpoles);
	    mbspc.Curve(1, tpoles);
	    Handle(Geom_BSplineCurve) BS=new Geom_BSplineCurve(tpoles,
							       mbspc.Knots(),
							       mbspc.Multiplicities(),
							       mbspc.Degree());
	    
	    GeomLib_CheckBSplineCurve Check(BS,TOLCHECK,TOLANGCHECK);
	    Check.FixTangent(Standard_True,Standard_True);
	    // 
	    IntTools_Curve aCurve;
	    aCurve.SetCurve(BS);
	    
	    if(myApprox1) { 
	      TColgp_Array1OfPnt2d tpoles2d(1,nbpoles);
	      mbspc.Curve(2,tpoles2d);
	      Handle(Geom2d_BSplineCurve) BS2=new Geom2d_BSplineCurve(tpoles2d,
								      mbspc.Knots(),
								      mbspc.Multiplicities(),
								      mbspc.Degree());

	      GeomLib_Check2dBSplineCurve newCheck(BS2,TOLCHECK,TOLANGCHECK);
	      newCheck.FixTangent(Standard_True,Standard_True);
	      // 		
	      aCurve.SetFirstCurve2d(BS2);
	    }
	    else {
	      Handle(Geom2d_BSplineCurve) H1;
	      aCurve.SetFirstCurve2d(H1);
	    }
	    
	    if(myApprox2) { 
	      TColgp_Array1OfPnt2d tpoles2d(1, nbpoles);
	      Standard_Integer TwoOrThree;
	      TwoOrThree=myApprox1 ? 3 : 2;
	      mbspc.Curve(TwoOrThree, tpoles2d);
	      Handle(Geom2d_BSplineCurve) BS2 =new Geom2d_BSplineCurve(tpoles2d,
								       mbspc.Knots(),
								       mbspc.Multiplicities(),
								       mbspc.Degree());
		
	      GeomLib_Check2dBSplineCurve newCheck(BS2,TOLCHECK,TOLANGCHECK);
	      newCheck.FixTangent(Standard_True,Standard_True);
	      // 	
	      aCurve.SetSecondCurve2d(BS2);
	    }
	    else { 
	      Handle(Geom2d_BSplineCurve) H2;
	      aCurve.SetSecondCurve2d(H2);
	    }
	    // 
	    mySeqOfCurve.Append(aCurve);

	  }// for (j=1; j<=aNbMultiCurves; j++) {
	}// else from if (!theapp3d.IsDone())
      }// for (i=1; i<=aNbParts; i++) {
    }// else { // myApprox=TRUE
  }// case IntPatch_Analytic:
    break;

  case IntPatch_Walking:{
    Handle(IntPatch_WLine) WL = 
      Handle(IntPatch_WLine)::DownCast(L);
    //
    Standard_Integer ifprm, ilprm;
    //
    if (!myApprox) {
      aNbParts = 1;
      if(!bAvoidLineConstructor){
	aNbParts=myLConstruct.NbParts();
      }
      for (i=1; i<=aNbParts; ++i) {
	Handle(Geom2d_BSplineCurve) H1, H2;
	Handle(Geom_Curve) aBSp;
	//
	if(bAvoidLineConstructor) {
	  ifprm = 1;
	  ilprm = WL->NbPnts();
	}
	else {
	  myLConstruct.Part(i, fprm, lprm);
	  ifprm=(Standard_Integer)fprm;
	  ilprm=(Standard_Integer)lprm;
	}
	//
	if(myApprox1) {
	  H1 = MakeBSpline2d(WL, ifprm, ilprm, Standard_True);
	}
	//
	if(myApprox2) {
	  H2 = MakeBSpline2d(WL, ifprm, ilprm, Standard_False);
	}
	// 	  
	aBSp=MakeBSpline(WL, ifprm, ilprm);
	IntTools_Curve aIC(aBSp, H1, H2);
	mySeqOfCurve.Append(aIC);
      }// for (i=1; i<=aNbParts; ++i) {
    }// if (!myApprox) {
    //
    else { // X
      Standard_Boolean bIsDecomposited;
      Standard_Integer nbiter, aNbSeqOfL;
      Standard_Real tol2d;
      IntPatch_SequenceOfLine aSeqOfL;
      GeomInt_WLApprox theapp3d;
      Approx_ParametrizationType aParType = Approx_ChordLength;
      //
      Standard_Boolean anApprox1 = myApprox1;
      Standard_Boolean anApprox2 = myApprox2;

      tol2d = myTolApprox;

      GeomAbs_SurfaceType typs1, typs2;
      typs1 = myHS1->Surface().GetType();
      typs2 = myHS2->Surface().GetType();
      Standard_Boolean anWithPC = Standard_True;

      if(typs1 == GeomAbs_Cylinder && typs2 == GeomAbs_Sphere) {
	anWithPC = 
	  ApproxWithPCurves(myHS1->Surface().Cylinder(), myHS2->Surface().Sphere());
      }
      else if (typs1 == GeomAbs_Sphere && typs2 == GeomAbs_Cylinder) {
	anWithPC = 
	  ApproxWithPCurves(myHS2->Surface().Cylinder(), myHS1->Surface().Sphere());
      }
      if(!anWithPC) {
	//aParType = Approx_Centripetal; 
	myTolApprox = 1.e-5; 
	anApprox1 = Standard_False;
	anApprox2 = Standard_False;
	// 	
	tol2d = myTolApprox;
      }
	
      if(myHS1 == myHS2) { 
	//
	theapp3d.SetParameters(myTolApprox, tol2d, 4, 8, 0, Standard_False, aParType);
	rejectSurface = Standard_True;
      }
      else { 
	if(reApprox && !rejectSurface)
	  theapp3d.SetParameters(myTolApprox, tol2d, 4, 8, 0, Standard_False, aParType);
	else {
	  //modified by NIZNHY-PKV Thu Sep 23 08:34:09 2010f
	  Standard_Integer iDegMax, iDegMin, iNbIterMax;
	  //
	  ApproxParameters(myHS1, myHS2, iDegMin, iDegMax, iNbIterMax, anWithPC);
	  if(!anWithPC) {
	    //myTolApprox = 1.e-5; 
	    anApprox1 = Standard_False;
	    anApprox2 = Standard_False;
	    tol2d = myTolApprox;
	  }
	  //
	  theapp3d.SetParameters(myTolApprox, tol2d, iDegMin, iDegMax, iNbIterMax,
				 Standard_True, aParType);
	  //theapp3d.SetParameters(myTolApprox, tol2d, 4, 8, 0, Standard_True, aParType);
	  //modified by NIZNHY-PKV Thu Sep 23 08:36:17 2010t
	}
      }
      //
      Standard_Real aReachedTol = Precision::Confusion();
      bIsDecomposited=DecompositionOfWLine(WL,
					   myHS1, 
					   myHS2, 
					   myFace1, 
					   myFace2, 
					   myLConstruct, 
					   bAvoidLineConstructor, 
					   aSeqOfL, 
					   aReachedTol);
      if ( bIsDecomposited && ( myTolReached3d < aReachedTol ) )
	myTolReached3d = aReachedTol;

      //
      aNbSeqOfL=aSeqOfL.Length();
      //
      if (bIsDecomposited) {
	nbiter=aNbSeqOfL;
      }
      else {
	nbiter=1;
	aNbParts=1;
	if (!bAvoidLineConstructor) {
	  aNbParts=myLConstruct.NbParts();
	  nbiter=aNbParts;
	}
      }
      //
      // nbiter=(bIsDecomposited) ? aSeqOfL.Length() :
      //   ((bAvoidLineConstructor) ? 1 :aNbParts);
      //
      for(i = 1; i <= nbiter; ++i) {
	if(bIsDecomposited) {
	  WL = Handle(IntPatch_WLine)::DownCast(aSeqOfL.Value(i));
	  ifprm = 1;
	  ilprm = WL->NbPnts();
	}
	else {
	  if(bAvoidLineConstructor) {
	    ifprm = 1;
	    ilprm = WL->NbPnts();
	  }
	  else {
	    myLConstruct.Part(i, fprm, lprm);
	    ifprm = (Standard_Integer)fprm;
	    ilprm = (Standard_Integer)lprm;
	  }
	}
	//-- lbr : 
	//-- Si une des surfaces est un plan , on approxime en 2d
	//-- sur cette surface et on remonte les points 2d en 3d.
	if(typs1 == GeomAbs_Plane) { 
	  theapp3d.Perform(myHS1, myHS2, WL, Standard_False,Standard_True, myApprox2,ifprm,ilprm);
	}	  
	else if(typs2 == GeomAbs_Plane) { 
	  theapp3d.Perform(myHS1,myHS2,WL,Standard_False,myApprox1,Standard_True,ifprm,ilprm);
	}
	else { 
	  //
	  if (myHS1 != myHS2){
	    if ((typs1==GeomAbs_BezierSurface || typs1==GeomAbs_BSplineSurface) &&
		(typs2==GeomAbs_BezierSurface || typs2==GeomAbs_BSplineSurface)) {
	     
	      theapp3d.SetParameters(myTolApprox, tol2d, 4, 8, 0, Standard_True, aParType);
	      
	      Standard_Boolean bUseSurfaces;
	      bUseSurfaces=NotUseSurfacesForApprox(myFace1, myFace2, WL, ifprm,  ilprm);
	      if (bUseSurfaces) {
		// ######
		rejectSurface = Standard_True;
		// ######
		theapp3d.SetParameters(myTolApprox, tol2d, 4, 8, 0, Standard_False, aParType);
	      }
	    }
	  }
	  //
	  theapp3d.Perform(myHS1,myHS2,WL,Standard_True,anApprox1,anApprox2,ifprm,ilprm);
	}
	 
	if (!theapp3d.IsDone()) {
	  // 	  
	  Handle(Geom2d_BSplineCurve) H1;
	  // 	  
	  Handle(Geom_Curve) aBSp=MakeBSpline(WL,ifprm, ilprm);
	  Handle(Geom2d_BSplineCurve) H2;

	  if(myApprox1) {
	    H1 = MakeBSpline2d(WL, ifprm, ilprm, Standard_True);
	  }
	  
	  if(myApprox2) {
	    H2 = MakeBSpline2d(WL, ifprm, ilprm, Standard_False);
	  }
	  // 	  
	  IntTools_Curve aIC(aBSp, H1, H2);
	  mySeqOfCurve.Append(aIC);
	}
	
	else {
	  if(myApprox1 || myApprox2 || (typs1==GeomAbs_Plane || typs2==GeomAbs_Plane)) { 
	    if( theapp3d.TolReached2d()>myTolReached2d || myTolReached2d==0.) { 
	      myTolReached2d = theapp3d.TolReached2d();
	    }
	  }
	  if(typs1==GeomAbs_Plane || typs2==GeomAbs_Plane) { 
	    myTolReached3d = myTolReached2d;
	    //
	    if (typs1==GeomAbs_Torus || typs2==GeomAbs_Torus) {
	      if (myTolReached3d<1.e-6) {
		myTolReached3d = theapp3d.TolReached3d();
		myTolReached3d=1.e-6;
	      }
	    }
	    //
	  }
	  else  if( theapp3d.TolReached3d()>myTolReached3d || myTolReached3d==0.) { 
	    myTolReached3d = theapp3d.TolReached3d();
	  }
	  
	  Standard_Integer aNbMultiCurves, nbpoles;
	  aNbMultiCurves=theapp3d.NbMultiCurves(); 
	  for (j=1; j<=aNbMultiCurves; j++) {
	    if(typs1 == GeomAbs_Plane) {
	      const AppParCurves_MultiBSpCurve& mbspc = theapp3d.Value(j);
	      nbpoles = mbspc.NbPoles();
	      
	      TColgp_Array1OfPnt2d tpoles2d(1,nbpoles);
	      TColgp_Array1OfPnt   tpoles(1,nbpoles);
	      
	      mbspc.Curve(1,tpoles2d);
	      const gp_Pln&  Pln = myHS1->Surface().Plane();
	      //
	      Standard_Integer ik; 
	      for(ik = 1; ik<= nbpoles; ik++) { 
		tpoles.SetValue(ik,
				ElSLib::Value(tpoles2d.Value(ik).X(),
					      tpoles2d.Value(ik).Y(),
					      Pln));
	      }
	      //
	      Handle(Geom_BSplineCurve) BS = 
		new Geom_BSplineCurve(tpoles,
				      mbspc.Knots(),
				      mbspc.Multiplicities(),
				      mbspc.Degree());
	      GeomLib_CheckBSplineCurve Check(BS,TOLCHECK,TOLANGCHECK);
	      Check.FixTangent(Standard_True, Standard_True);
	      // 	
	      IntTools_Curve aCurve;
	      aCurve.SetCurve(BS);

	      if(myApprox1) { 
		Handle(Geom2d_BSplineCurve) BS1 = 
		  new Geom2d_BSplineCurve(tpoles2d,
					  mbspc.Knots(),
					  mbspc.Multiplicities(),
					  mbspc.Degree());
		GeomLib_Check2dBSplineCurve Check1(BS1,TOLCHECK,TOLANGCHECK);
		Check1.FixTangent(Standard_True,Standard_True);
		//
		// ############################################
		if(!rejectSurface && !reApprox) {
		  Standard_Boolean isValid = IsCurveValid(BS1);
		  if(!isValid) {
		    reApprox = Standard_True;
		    goto reapprox;
		  }
		}
		// ############################################
		aCurve.SetFirstCurve2d(BS1);
	      }
	      else {
		Handle(Geom2d_BSplineCurve) H1;
		aCurve.SetFirstCurve2d(H1);
	      }

	      if(myApprox2) { 
		mbspc.Curve(2, tpoles2d);
		
		Handle(Geom2d_BSplineCurve) BS2 = new Geom2d_BSplineCurve(tpoles2d,
									  mbspc.Knots(),
									  mbspc.Multiplicities(),
									  mbspc.Degree());
		GeomLib_Check2dBSplineCurve newCheck(BS2,TOLCHECK,TOLANGCHECK);
		newCheck.FixTangent(Standard_True,Standard_True);
		
		// ###########################################
		if(!rejectSurface && !reApprox) {
		  Standard_Boolean isValid = IsCurveValid(BS2);
		  if(!isValid) {
		    reApprox = Standard_True;
		    goto reapprox;
		  }
		}
		// ###########################################
		// 
		aCurve.SetSecondCurve2d(BS2);
	      }
	      else { 
		Handle(Geom2d_BSplineCurve) H2;
		// 		
		  aCurve.SetSecondCurve2d(H2);
	      }
	      //
	      mySeqOfCurve.Append(aCurve);
	    }
	    
	    else if(typs2 == GeomAbs_Plane) { 
	      const AppParCurves_MultiBSpCurve& mbspc = theapp3d.Value(j);
	      nbpoles = mbspc.NbPoles();
	      
	      TColgp_Array1OfPnt2d tpoles2d(1,nbpoles);
	      TColgp_Array1OfPnt   tpoles(1,nbpoles);
	      mbspc.Curve((myApprox1==Standard_True)? 2 : 1,tpoles2d);
	      const gp_Pln&  Pln = myHS2->Surface().Plane();
	      //
	      Standard_Integer ik; 
	      for(ik = 1; ik<= nbpoles; ik++) { 
		tpoles.SetValue(ik,
				ElSLib::Value(tpoles2d.Value(ik).X(),
					      tpoles2d.Value(ik).Y(),
					      Pln));
		
	      }
	      //
	      Handle(Geom_BSplineCurve) BS=new Geom_BSplineCurve(tpoles,
								 mbspc.Knots(),
								 mbspc.Multiplicities(),
								 mbspc.Degree());
	      GeomLib_CheckBSplineCurve Check(BS,TOLCHECK,TOLANGCHECK);
	      Check.FixTangent(Standard_True,Standard_True);
	      // 	
	      IntTools_Curve aCurve;
	      aCurve.SetCurve(BS);

	      if(myApprox2) {
		Handle(Geom2d_BSplineCurve) BS1=new Geom2d_BSplineCurve(tpoles2d,
									mbspc.Knots(),
									mbspc.Multiplicities(),
									mbspc.Degree());
		GeomLib_Check2dBSplineCurve Check1(BS1,TOLCHECK,TOLANGCHECK);
		Check1.FixTangent(Standard_True,Standard_True);
		// 	
		// ###########################################
		if(!rejectSurface && !reApprox) {
		  Standard_Boolean isValid = IsCurveValid(BS1);
		  if(!isValid) {
		    reApprox = Standard_True;
		    goto reapprox;
		  }
		}
		// ###########################################
		aCurve.SetSecondCurve2d(BS1);
	      }
	      else {
		Handle(Geom2d_BSplineCurve) H2;
		aCurve.SetSecondCurve2d(H2);
	      }
	      
	      if(myApprox1) { 
		mbspc.Curve(1,tpoles2d);
		Handle(Geom2d_BSplineCurve) BS2=new Geom2d_BSplineCurve(tpoles2d,
									mbspc.Knots(),
									mbspc.Multiplicities(),
									mbspc.Degree());
		GeomLib_Check2dBSplineCurve Check2(BS2,TOLCHECK,TOLANGCHECK);
		Check2.FixTangent(Standard_True,Standard_True);
		//
		// ###########################################
		if(!rejectSurface && !reApprox) {
		  Standard_Boolean isValid = IsCurveValid(BS2);
		  if(!isValid) {
		    reApprox = Standard_True;
		    goto reapprox;
		  }
		}
		// ###########################################
		aCurve.SetFirstCurve2d(BS2);
	      }
	      else { 
		Handle(Geom2d_BSplineCurve) H1;
		// 		
		aCurve.SetFirstCurve2d(H1);
	      }
	      //
	      mySeqOfCurve.Append(aCurve);
	    }
	    else { 
	      const AppParCurves_MultiBSpCurve& mbspc = theapp3d.Value(j);
	      nbpoles = mbspc.NbPoles();
	      TColgp_Array1OfPnt tpoles(1,nbpoles);
	      mbspc.Curve(1,tpoles);
	      Handle(Geom_BSplineCurve) BS=new Geom_BSplineCurve(tpoles,
								 mbspc.Knots(),
								 mbspc.Multiplicities(),
								 mbspc.Degree());
	      GeomLib_CheckBSplineCurve Check(BS,TOLCHECK,TOLANGCHECK);
	      Check.FixTangent(Standard_True,Standard_True);
	      // 		
	      IntTools_Curve aCurve;
	      aCurve.SetCurve(BS);
	      
	      if(myApprox1) { 
		if(anApprox1) {
		  TColgp_Array1OfPnt2d tpoles2d(1,nbpoles);
		  mbspc.Curve(2,tpoles2d);
		  Handle(Geom2d_BSplineCurve) BS1=new Geom2d_BSplineCurve(tpoles2d,
									mbspc.Knots(),
									mbspc.Multiplicities(),
									mbspc.Degree());
		  GeomLib_Check2dBSplineCurve newCheck(BS1,TOLCHECK,TOLANGCHECK);
		  newCheck.FixTangent(Standard_True,Standard_True);
		  // 	
		  aCurve.SetFirstCurve2d(BS1);
		}
		else {
		  Handle(Geom2d_BSplineCurve) BS1;
		  fprm = BS->FirstParameter();
		  lprm = BS->LastParameter();

		  Handle(Geom2d_Curve) C2d;
		  Standard_Real aTol = myTolApprox;
		  BuildPCurves(fprm, lprm, aTol, myHS1->ChangeSurface().Surface(), BS, C2d);
		  BS1 = Handle(Geom2d_BSplineCurve)::DownCast(C2d);
		  aCurve.SetFirstCurve2d(BS1);
		}
		
	      }
	      else {
		Handle(Geom2d_BSplineCurve) H1;
		// 		
		aCurve.SetFirstCurve2d(H1);
	      }
	      if(myApprox2) { 
		if(anApprox2) {
		  TColgp_Array1OfPnt2d tpoles2d(1,nbpoles);
		  mbspc.Curve((myApprox1==Standard_True)? 3 : 2,tpoles2d);
		  Handle(Geom2d_BSplineCurve) BS2=new Geom2d_BSplineCurve(tpoles2d,
									mbspc.Knots(),
									mbspc.Multiplicities(),
									mbspc.Degree());
		  GeomLib_Check2dBSplineCurve newCheck(BS2,TOLCHECK,TOLANGCHECK);
		  newCheck.FixTangent(Standard_True,Standard_True);
		// 		
		  aCurve.SetSecondCurve2d(BS2);
		}
		else {
		  Handle(Geom2d_BSplineCurve) BS2;
		  fprm = BS->FirstParameter();
		  lprm = BS->LastParameter();

		  Handle(Geom2d_Curve) C2d;
		  Standard_Real aTol = myTolApprox;
		  BuildPCurves(fprm, lprm, aTol, myHS2->ChangeSurface().Surface(), BS, C2d);
		  BS2 = Handle(Geom2d_BSplineCurve)::DownCast(C2d);
		  aCurve.SetSecondCurve2d(BS2);
		}
		
	      }
	      else { 
		Handle(Geom2d_BSplineCurve) H2;
		// 		
		aCurve.SetSecondCurve2d(H2);
	      }
	      //
	      mySeqOfCurve.Append(aCurve);
	    }
	  }
	}
      }
    }// else { // X
  }// case IntPatch_Walking:{
    break;
    
  case IntPatch_Restriction: 
    break;

  }
}

//=======================================================================
//function : BuildPCurves
//purpose  : 
//=======================================================================
 void BuildPCurves (Standard_Real f,
		    Standard_Real l,
		    Standard_Real& Tol,
		    const Handle (Geom_Surface)& S,
		    const Handle (Geom_Curve)&   C,
		    Handle (Geom2d_Curve)& C2d)
{

  Standard_Real umin,umax,vmin,vmax;
  // 

  if (C2d.IsNull()) {

    // in class ProjLib_Function the range of parameters is shrank by 1.e-09
    if((l - f) > 2.e-09) {
      C2d = GeomProjLib::Curve2d(C,f,l,S,Tol);
      //
      if (C2d.IsNull()) {
	// proj. a circle that goes through the pole on a sphere to the sphere     
	Tol=Tol+1.e-7;
	C2d = GeomProjLib::Curve2d(C,f,l,S,Tol);
      }
    }
    else {
      if((l - f) > Epsilon(Abs(f))) {
	GeomAPI_ProjectPointOnSurf aProjector1, aProjector2;
	gp_Pnt P1 = C->Value(f);
	gp_Pnt P2 = C->Value(l);
	aProjector1.Init(P1, S);
	aProjector2.Init(P2, S);

	if(aProjector1.IsDone() && aProjector2.IsDone()) {
	  Standard_Real U=0., V=0.;
	  aProjector1.LowerDistanceParameters(U, V);
	  gp_Pnt2d p1(U, V);

	  aProjector2.LowerDistanceParameters(U, V);
	  gp_Pnt2d p2(U, V);

	  if(p1.Distance(p2) > gp::Resolution()) {
	    TColgp_Array1OfPnt2d poles(1,2);
	    TColStd_Array1OfReal knots(1,2);
	    TColStd_Array1OfInteger mults(1,2);
	    poles(1) = p1;
	    poles(2) = p2;
	    knots(1) = f;
	    knots(2) = l;
	    mults(1) = mults(2) = 2;

	    C2d = new Geom2d_BSplineCurve(poles,knots,mults,1);

	    // compute reached tolerance.begin
	    gp_Pnt PMid = C->Value((f + l) * 0.5);
	    aProjector1.Perform(PMid);

	    if(aProjector1.IsDone()) {
	      aProjector1.LowerDistanceParameters(U, V);
	      gp_Pnt2d pmidproj(U, V);
	      gp_Pnt2d pmidcurve2d = C2d->Value((f + l) * 0.5);
	      Standard_Real adist = pmidcurve2d.Distance(pmidproj);
	      Tol = (adist > Tol) ? adist : Tol;
	    }
	    // compute reached tolerance.end
	  }
	}
      }
    }
    //
    S->Bounds(umin, umax, vmin, vmax);

    if (S->IsUPeriodic() && !C2d.IsNull()) {
      // Recadre dans le domaine UV de la face
      Standard_Real period, U0, du, aEps; 
      
      du =0.0;
      aEps=Precision::PConfusion();
      period = S->UPeriod();
      gp_Pnt2d Pf = C2d->Value(f);
      U0=Pf.X();
      //
      gp_Pnt2d Pl = C2d->Value(l);
      
      U0 = Min(Pl.X(), U0);
//       while(U0-umin<aEps) { 
      while(U0-umin<-aEps) { 
	U0+=period;
	du+=period;
      }
      //
      while(U0-umax>aEps) { 
	U0-=period;
	du-=period;
      }
      if (du != 0) {
	gp_Vec2d T1(du,0.);
	C2d->Translate(T1);
      }
    }
  }
  if (C2d.IsNull()) {
    BOPTColStd_Dump::PrintMessage("BuildPCurves()=> Echec ProjLib\n");
  }
}

//=======================================================================
//function : Parameters
//purpose  : 
//=======================================================================
 void Parameters(const Handle(GeomAdaptor_HSurface)& HS1,
		 const Handle(GeomAdaptor_HSurface)& HS2,
		 const gp_Pnt& Ptref,
		 Standard_Real& U1,
		 Standard_Real& V1,
		 Standard_Real& U2,
		 Standard_Real& V2)
{

  IntSurf_Quadric quad1,quad2;
  GeomAbs_SurfaceType typs = HS1->Surface().GetType();

  switch (typs) {
  case GeomAbs_Plane:
    quad1.SetValue(HS1->Surface().Plane());
    break;
  case GeomAbs_Cylinder:
    quad1.SetValue(HS1->Surface().Cylinder());
    break;
  case GeomAbs_Cone:
    quad1.SetValue(HS1->Surface().Cone());
    break;
  case GeomAbs_Sphere:
    quad1.SetValue(HS1->Surface().Sphere());
    break;
  default:
    Standard_ConstructionError::Raise("GeomInt_IntSS::MakeCurve");
  }
  
  typs = HS2->Surface().GetType();
  switch (typs) {
  case GeomAbs_Plane:
    quad2.SetValue(HS2->Surface().Plane());
    break;
  case GeomAbs_Cylinder:
    quad2.SetValue(HS2->Surface().Cylinder());
    break;
  case GeomAbs_Cone:
    quad2.SetValue(HS2->Surface().Cone());
    break;
  case GeomAbs_Sphere:
    quad2.SetValue(HS2->Surface().Sphere());
    break;
  default:
    Standard_ConstructionError::Raise("GeomInt_IntSS::MakeCurve");
  }

  quad1.Parameters(Ptref,U1,V1);
  quad2.Parameters(Ptref,U2,V2);
}

//=======================================================================
//function : MakeBSpline
//purpose  : 
//=======================================================================
Handle(Geom_Curve) MakeBSpline  (const Handle(IntPatch_WLine)& WL,
				 const Standard_Integer ideb,
				 const Standard_Integer ifin)
{
  Standard_Integer i,nbpnt = ifin-ideb+1;
  TColgp_Array1OfPnt poles(1,nbpnt);
  TColStd_Array1OfReal knots(1,nbpnt);
  TColStd_Array1OfInteger mults(1,nbpnt);
  Standard_Integer ipidebm1;
  for(i=1,ipidebm1=i+ideb-1; i<=nbpnt;ipidebm1++, i++) {
    poles(i) = WL->Point(ipidebm1).Value();
    mults(i) = 1;
    knots(i) = i-1;
  }
  mults(1) = mults(nbpnt) = 2;
  return
    new Geom_BSplineCurve(poles,knots,mults,1);
}
//

//=======================================================================
//function : MakeBSpline2d
//purpose  : 
//=======================================================================
Handle(Geom2d_BSplineCurve) MakeBSpline2d(const Handle(IntPatch_WLine)& theWLine,
					  const Standard_Integer ideb,
					  const Standard_Integer ifin,
					  const Standard_Boolean onFirst)
{
  Standard_Integer i, nbpnt = ifin-ideb+1;
  TColgp_Array1OfPnt2d poles(1,nbpnt);
  TColStd_Array1OfReal knots(1,nbpnt);
  TColStd_Array1OfInteger mults(1,nbpnt);
  Standard_Integer ipidebm1;

  for(i = 1, ipidebm1 = i+ideb-1; i <= nbpnt; ipidebm1++, i++) {
      Standard_Real U, V;
      if(onFirst)
	theWLine->Point(ipidebm1).ParametersOnS1(U, V);
      else
	theWLine->Point(ipidebm1).ParametersOnS2(U, V);
      poles(i).SetCoord(U, V);
      mults(i) = 1;
      knots(i) = i-1;
    }
    mults(1) = mults(nbpnt) = 2;

  return new Geom2d_BSplineCurve(poles,knots,mults,1);
}
//=======================================================================
//function : PrepareLines3D
//purpose  : 
//=======================================================================
  void IntTools_FaceFace::PrepareLines3D()
{
  Standard_Integer i, aNbCurves, j, aNbNewCurves;
  IntTools_SequenceOfCurves aNewCvs;

  //
  // 1. Treatment of periodic and closed  curves
  aNbCurves=mySeqOfCurve.Length();
  for (i=1; i<=aNbCurves; i++) {
    const IntTools_Curve& aIC=mySeqOfCurve(i);
    // DEBUG
    // const Handle(Geom_Curve)&   aC3D =aIC.Curve();
    // const Handle(Geom2d_Curve)& aC2D1=aIC.FirstCurve2d();
    // const Handle(Geom2d_Curve)& aC2D2=aIC.SecondCurve2d();
    //
    IntTools_SequenceOfCurves aSeqCvs;
    aNbNewCurves=IntTools_Tools::SplitCurve(aIC, aSeqCvs);
    
    if (aNbNewCurves) {
      for (j=1; j<=aNbNewCurves; j++) {
	const IntTools_Curve& aICNew=aSeqCvs(j);
	aNewCvs.Append(aICNew);
      }
    }
    //
    else {
      aNewCvs.Append(aIC);
    }
  }
  //
  // 2. Plane\Cone intersection when we had 4 curves
  GeomAbs_SurfaceType aType1, aType2;
  BRepAdaptor_Surface aBS1, aBS2;
  
  aBS1.Initialize(myFace1);
  aType1=aBS1.GetType();
  
  aBS2.Initialize(myFace2);
  aType2=aBS2.GetType();
  
  if ((aType1==GeomAbs_Plane && aType2==GeomAbs_Cone) ||
      (aType2==GeomAbs_Plane && aType1==GeomAbs_Cone)) {
    aNbCurves=aNewCvs.Length();
    if (aNbCurves==4) {
      GeomAbs_CurveType aCType1=aNewCvs(1).Type();
      if (aCType1==GeomAbs_Line) {
	IntTools_SequenceOfCurves aSeqIn, aSeqOut;
	//
	for (i=1; i<=aNbCurves; i++) {
	  const IntTools_Curve& aIC=aNewCvs(i);
	  aSeqIn.Append(aIC);
	}
	//
	IntTools_Tools::RejectLines(aSeqIn, aSeqOut);
	//
	aNewCvs.Clear();
	aNbCurves=aSeqOut.Length(); 
	for (i=1; i<=aNbCurves; i++) {
	  const IntTools_Curve& aIC=aSeqOut(i);
	  aNewCvs.Append(aIC);
	}
	//
      }
    }
  }// end of if ((aType1==GeomAbs_Plane && ...
  //
  // 3. Fill  mySeqOfCurve
  mySeqOfCurve.Clear();
  aNbCurves=aNewCvs.Length();
  for (i=1; i<=aNbCurves; i++) {
    const IntTools_Curve& aIC=aNewCvs(i);
    mySeqOfCurve.Append(aIC);
  }

}


//=======================================================================
//function : CorrectSurfaceBoundaries
//purpose  : 
//=======================================================================
 void CorrectSurfaceBoundaries(const TopoDS_Face&  theFace,
			       const Standard_Real theTolerance,
			       Standard_Real&      theumin,
			       Standard_Real&      theumax, 
			       Standard_Real&      thevmin, 
			       Standard_Real&      thevmax) 
{
  Standard_Boolean enlarge, isuperiodic, isvperiodic;
  Standard_Real uinf, usup, vinf, vsup, delta;
  GeomAbs_SurfaceType aType;
  Handle(Geom_Surface) aSurface;
  //
  aSurface = BRep_Tool::Surface(theFace);
  aSurface->Bounds(uinf, usup, vinf, vsup);
  delta = theTolerance;
  enlarge = Standard_False;
  //
  GeomAdaptor_Surface anAdaptorSurface(aSurface);
  //
  if(aSurface->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))) {
    Handle(Geom_Surface) aBasisSurface = 
      (Handle(Geom_RectangularTrimmedSurface)::DownCast(aSurface))->BasisSurface();
    
    if(aBasisSurface->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)) ||
       aBasisSurface->IsKind(STANDARD_TYPE(Geom_OffsetSurface))) {
      return;
    }
  }
  //
  if(aSurface->IsKind(STANDARD_TYPE(Geom_OffsetSurface))) {
    Handle(Geom_Surface) aBasisSurface = 
      (Handle(Geom_OffsetSurface)::DownCast(aSurface))->BasisSurface();
    
    if(aBasisSurface->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)) ||
       aBasisSurface->IsKind(STANDARD_TYPE(Geom_OffsetSurface))) {
      return;
    }
  }
  //
  isuperiodic = anAdaptorSurface.IsUPeriodic();
  isvperiodic = anAdaptorSurface.IsVPeriodic();
  //
  aType=anAdaptorSurface.GetType();
  if((aType==GeomAbs_BezierSurface) ||
     (aType==GeomAbs_BSplineSurface) ||
     (aType==GeomAbs_SurfaceOfExtrusion) ||
     (aType==GeomAbs_SurfaceOfRevolution)) {
    enlarge=Standard_True;
  }
  //
  if(!isuperiodic && enlarge) {

    if((theumin - uinf) > delta )
      theumin -= delta;
    else {
      theumin = uinf;
    }

    if((usup - theumax) > delta )
      theumax += delta;
    else
      theumax = usup;
  }
  //
  if(!isvperiodic && enlarge) {
    if((thevmin - vinf) > delta ) {
      thevmin -= delta;
    }
    else { 
      thevmin = vinf;
    }
    if((vsup - thevmax) > delta ) {
      thevmax += delta;
    }
    else {
      thevmax = vsup;
    }
  }
  //
  {
    Standard_Integer aNbP;
    Standard_Real aXP, dXfact, aXmid, aX1, aX2, aTolPA;
    //
    aTolPA=Precision::Angular();
    // U
    if (isuperiodic) {
      aXP=anAdaptorSurface.UPeriod();
      dXfact=theumax-theumin;
      if (dXfact-aTolPA>aXP) {
	aXmid=0.5*(theumax+theumin);
	aNbP=RealToInt(aXmid/aXP);
	if (aXmid<0.) {
	  aNbP=aNbP-1;
	}
	aX1=aNbP*aXP;
	aX2=aX1+aXP;
	if (theumin<aX1) {
	  theumin=aX1;
	}
	if (theumax>aX2) {
	  theumax=aX2;
	}
      }
    }
    // V
    if (isvperiodic) {
      aXP=anAdaptorSurface.VPeriod();
      dXfact=thevmax-thevmin;
      if (dXfact-aTolPA>aXP) {
	aXmid=0.5*(thevmax+thevmin);
	aNbP=RealToInt(aXmid/aXP);
	if (aXmid<0.) {
	  aNbP=aNbP-1;
	}
	aX1=aNbP*aXP;
	aX2=aX1+aXP;
	if (thevmin<aX1) {
	  thevmin=aX1;
	}
	if (thevmax>aX2) {
	  thevmax=aX2;
	}
      }
    }
  }
  //
  if(isuperiodic || isvperiodic) {
    Standard_Boolean correct = Standard_False;
    Standard_Boolean correctU = Standard_False;
    Standard_Boolean correctV = Standard_False;
    Bnd_Box2d aBox;
    TopExp_Explorer anExp;

    for(anExp.Init(theFace, TopAbs_EDGE); anExp.More(); anExp.Next()) {
      if(BRep_Tool::IsClosed(TopoDS::Edge(anExp.Current()), theFace)) {
	correct = Standard_True;
	Standard_Real f, l;
	TopoDS_Edge anEdge = TopoDS::Edge(anExp.Current());
	
	for(Standard_Integer i = 0; i < 2; i++) {
	  if(i==0) {
	    anEdge.Orientation(TopAbs_FORWARD);
	  }
	  else {
	    anEdge.Orientation(TopAbs_REVERSED);
	  }
	  Handle(Geom2d_Curve) aCurve = BRep_Tool::CurveOnSurface(anEdge, theFace, f, l);
	  
	  if(aCurve.IsNull()) {
	    correct = Standard_False;
	    break;
	  }
	  Handle(Geom2d_Line) aLine = Handle(Geom2d_Line)::DownCast(aCurve);

	  if(aLine.IsNull()) {
	    correct = Standard_False;
	    break;
	  }
	  gp_Dir2d anUDir(1., 0.);
	  gp_Dir2d aVDir(0., 1.);
	  Standard_Real anAngularTolerance = Precision::Angular();

	  correctU = correctU || aLine->Position().Direction().IsParallel(aVDir, anAngularTolerance);
	  correctV = correctV || aLine->Position().Direction().IsParallel(anUDir, anAngularTolerance);
	  
	  gp_Pnt2d pp1 = aCurve->Value(f);
	  aBox.Add(pp1);
	  gp_Pnt2d pp2 = aCurve->Value(l);
	  aBox.Add(pp2);
	}
	if(!correct)
	  break;
      }
    }

    if(correct) {
      Standard_Real umin, vmin, umax, vmax;
      aBox.Get(umin, vmin, umax, vmax);

      if(isuperiodic && correctU) {
	
	if(theumin < umin)
	  theumin = umin;
	
	if(theumax > umax) {
	  theumax = umax;
	}
      }
      if(isvperiodic && correctV) {
	
	if(thevmin < vmin)
	  thevmin = vmin;
	if(thevmax > vmax)
	  thevmax = vmax;
      }
    }
  }
}
//
//
// The block is dedicated to determine whether WLine [ifprm, ilprm]
// crosses the degenerated zone on each given surface or not.
// If Yes -> We will not use info about surfaces during approximation
// because inside degenerated zone of the surface the approx. alogo.
// uses wrong values of normal, etc., and resulting curve will have
// oscillations that we would not like to have. 
//                                               PKV Tue Feb 12 2002  
 

static
  Standard_Boolean IsDegeneratedZone(const gp_Pnt2d& aP2d,
				     const Handle(Geom_Surface)& aS,
				     const Standard_Integer iDir);
static
  Standard_Boolean IsPointInDegeneratedZone(const IntSurf_PntOn2S& aP2S,
					    const TopoDS_Face& aF1,
					    const TopoDS_Face& aF2);
//=======================================================================
//function :  NotUseSurfacesForApprox
//purpose  : 
//=======================================================================
Standard_Boolean NotUseSurfacesForApprox(const TopoDS_Face& aF1,
					 const TopoDS_Face& aF2,
					 const Handle(IntPatch_WLine)& WL,
					 const Standard_Integer ifprm,
					 const Standard_Integer ilprm)
{
  Standard_Boolean bPInDZ;

  Handle(IntSurf_LineOn2S) aLineOn2S=WL->Curve();
  
  const IntSurf_PntOn2S& aP2Sfprm=aLineOn2S->Value(ifprm);
  bPInDZ=IsPointInDegeneratedZone(aP2Sfprm, aF1, aF2);
  if (bPInDZ) {
    return bPInDZ;
  }

  const IntSurf_PntOn2S& aP2Slprm=aLineOn2S->Value(ilprm);
  bPInDZ=IsPointInDegeneratedZone(aP2Slprm, aF1, aF2);
  
  return bPInDZ;
}
//=======================================================================
//function : IsPointInDegeneratedZone
//purpose  : 
//=======================================================================
Standard_Boolean IsPointInDegeneratedZone(const IntSurf_PntOn2S& aP2S,
					  const TopoDS_Face& aF1,
					  const TopoDS_Face& aF2)
					  
{
  Standard_Boolean bFlag=Standard_True;
  Standard_Real US11, US12, VS11, VS12, US21, US22, VS21, VS22;
  Standard_Real U1, V1, U2, V2, aDelta, aD;
  gp_Pnt2d aP2d;

  Handle(Geom_Surface)aS1 = BRep_Tool::Surface(aF1);
  aS1->Bounds(US11, US12, VS11, VS12);
  GeomAdaptor_Surface aGAS1(aS1);

  Handle(Geom_Surface)aS2 = BRep_Tool::Surface(aF2);
  aS1->Bounds(US21, US22, VS21, VS22);
  GeomAdaptor_Surface aGAS2(aS2);
  //
  //const gp_Pnt& aP=aP2S.Value();
  aP2S.Parameters(U1, V1, U2, V2);
  //
  aDelta=1.e-7;
  // Check on Surf 1
  aD=aGAS1.UResolution(aDelta);
  aP2d.SetCoord(U1, V1);
  if (fabs(U1-US11) < aD) {
    bFlag=IsDegeneratedZone(aP2d, aS1, 1);
    if (bFlag) {
      return bFlag;
    }
  }
  if (fabs(U1-US12) < aD) {
    bFlag=IsDegeneratedZone(aP2d, aS1, 1);
    if (bFlag) {
      return bFlag;
    }
  }
  aD=aGAS1.VResolution(aDelta);
  if (fabs(V1-VS11) < aDelta) {
    bFlag=IsDegeneratedZone(aP2d, aS1, 2);
    if (bFlag) {
      return bFlag;
    }
  }
  if (fabs(V1-VS12) < aDelta) {
    bFlag=IsDegeneratedZone(aP2d, aS1, 2);
    if (bFlag) {
      return bFlag;
    }
  }
  // Check on Surf 2
  aD=aGAS2.UResolution(aDelta);
  aP2d.SetCoord(U2, V2);
  if (fabs(U2-US21) < aDelta) {
    bFlag=IsDegeneratedZone(aP2d, aS2, 1);
    if (bFlag) {
      return bFlag;
    }
  }
  if (fabs(U2-US22) < aDelta) {
    bFlag=IsDegeneratedZone(aP2d, aS2, 1);
    if (bFlag) {
      return bFlag;
    }
  }
  aD=aGAS2.VResolution(aDelta);
  if (fabs(V2-VS21) < aDelta) {
    bFlag=IsDegeneratedZone(aP2d, aS2, 2);
    if (bFlag) {  
      return bFlag;
    }
  }
  if (fabs(V2-VS22) < aDelta) {
    bFlag=IsDegeneratedZone(aP2d, aS2, 2);
    if (bFlag) {
      return bFlag;
    }
  }
  return !bFlag;
}

//=======================================================================
//function : IsDegeneratedZone
//purpose  : 
//=======================================================================
Standard_Boolean IsDegeneratedZone(const gp_Pnt2d& aP2d,
				   const Handle(Geom_Surface)& aS,
				   const Standard_Integer iDir)
{
  Standard_Boolean bFlag=Standard_True;
  Standard_Real US1, US2, VS1, VS2, dY, dX, d1, d2, dD;
  Standard_Real aXm, aYm, aXb, aYb, aXe, aYe;
  aS->Bounds(US1, US2, VS1, VS2); 

  gp_Pnt aPm, aPb, aPe;
  
  aXm=aP2d.X();
  aYm=aP2d.Y();
  
  aS->D0(aXm, aYm, aPm); 
  
  dX=1.e-5;
  dY=1.e-5;
  dD=1.e-12;

  if (iDir==1) {
    aXb=aXm;
    aXe=aXm;
    aYb=aYm-dY;
    if (aYb < VS1) {
      aYb=VS1;
    }
    aYe=aYm+dY;
    if (aYe > VS2) {
      aYe=VS2;
    }
    aS->D0(aXb, aYb, aPb);
    aS->D0(aXe, aYe, aPe);
    
    d1=aPm.Distance(aPb);
    d2=aPm.Distance(aPe);
    if (d1 < dD && d2 < dD) {
      return bFlag;
    }
    return !bFlag;
  }
  //
  else if (iDir==2) {
    aYb=aYm;
    aYe=aYm;
    aXb=aXm-dX;
    if (aXb < US1) {
      aXb=US1;
    }
    aXe=aXm+dX;
    if (aXe > US2) {
      aXe=US2;
    }
    aS->D0(aXb, aYb, aPb);
    aS->D0(aXe, aYe, aPe);
    
    d1=aPm.Distance(aPb);
    d2=aPm.Distance(aPe);
    if (d1 < dD && d2 < dD) {
      return bFlag;
    }
    return !bFlag;
  }
  return !bFlag;
}

//=========================================================================
// static function : ComputePurgedWLine
// purpose : Removes equal points (leave one of equal points) from theWLine
//           and recompute vertex parameters.
//           Returns new WLine or null WLine if the number
//           of the points is less than 2.
//=========================================================================
Handle(IntPatch_WLine) ComputePurgedWLine(const Handle(IntPatch_WLine)& theWLine) {
  Handle(IntPatch_WLine) aResult;
  Handle(IntPatch_WLine) aLocalWLine;
  Handle(IntPatch_WLine) aTmpWLine = theWLine;

  Handle(IntSurf_LineOn2S) aLineOn2S = new IntSurf_LineOn2S();
  aLocalWLine = new IntPatch_WLine(aLineOn2S, Standard_False);
  Standard_Integer i, k, v, nb, nbvtx;
  nbvtx = theWLine->NbVertex();
  nb = theWLine->NbPnts();

  for(i = 1; i <= nb; i++) {
    aLineOn2S->Add(theWLine->Point(i));
  }

  for(v = 1; v <= nbvtx; v++) {
    aLocalWLine->AddVertex(theWLine->Vertex(v));
  }
  
  for(i = 1; i <= aLineOn2S->NbPoints(); i++) {
    Standard_Integer aStartIndex = i + 1;
    Standard_Integer anEndIndex = i + 5;
    nb = aLineOn2S->NbPoints();
    anEndIndex = (anEndIndex > nb) ? nb : anEndIndex;

    if((aStartIndex >= nb) || (anEndIndex <= 1)) {
      continue;
    }
    k = aStartIndex;

    while(k <= anEndIndex) {
      
      if(i != k) {
	IntSurf_PntOn2S p1 = aLineOn2S->Value(i);
	IntSurf_PntOn2S p2 = aLineOn2S->Value(k);
	
	if(p1.Value().IsEqual(p2.Value(), gp::Resolution())) {
	  aTmpWLine = aLocalWLine;
	  aLocalWLine = new IntPatch_WLine(aLineOn2S, Standard_False);

	  for(v = 1; v <= aTmpWLine->NbVertex(); v++) {
	    IntPatch_Point aVertex = aTmpWLine->Vertex(v);
	    Standard_Integer avertexindex = (Standard_Integer)aVertex.ParameterOnLine();

	    if(avertexindex >= k) {
	      aVertex.SetParameter(aVertex.ParameterOnLine() - 1.);
	    }
	    aLocalWLine->AddVertex(aVertex);
	  }
	  aLineOn2S->RemovePoint(k);
	  anEndIndex--;
	  continue;
	}
      }
      k++;
    }
  }

  if(aLineOn2S->NbPoints() > 1) {
    aResult = aLocalWLine;
  }
  return aResult;
}

//=======================================================================
//function : TolR3d
//purpose  : 
//=======================================================================
void TolR3d(const TopoDS_Face& aF1,
	    const TopoDS_Face& aF2,
	    Standard_Real& myTolReached3d)
{
  Standard_Real aTolF1, aTolF2, aTolFMax, aTolTresh;
      
  aTolTresh=2.999999e-3;
  aTolF1 = BRep_Tool::Tolerance(aF1);
  aTolF2 = BRep_Tool::Tolerance(aF2);
  aTolFMax=Max(aTolF1, aTolF2);
  
  if (aTolFMax>aTolTresh) {
    myTolReached3d=aTolFMax;
  }
}
//=======================================================================
//function : AdjustPeriodic
//purpose  : 
//=======================================================================
Standard_Real AdjustPeriodic(const Standard_Real theParameter,
			     const Standard_Real parmin,
			     const Standard_Real parmax,
			     const Standard_Real thePeriod,
			     Standard_Real&      theOffset) 
{
  Standard_Real aresult;
  //
  theOffset = 0.;
  aresult = theParameter;
  while(aresult < parmin) {
    aresult += thePeriod;
    theOffset += thePeriod;
  }

  while(aresult > parmax) {
    aresult -= thePeriod;
    theOffset -= thePeriod;
  }
  return aresult;
}
//=======================================================================
//function : IsPointOnBoundary
//purpose  : 
//=======================================================================
Standard_Boolean IsPointOnBoundary(const Standard_Real theParameter,
				   const Standard_Real theFirstBoundary,
				   const Standard_Real theSecondBoundary,
				   const Standard_Real theResolution,
				   Standard_Boolean&   IsOnFirstBoundary) 
{
  Standard_Boolean bRet;
  Standard_Integer i;
  Standard_Real adist;
  //
  bRet=Standard_False;
  for(i = 0; i < 2; ++i) {
    IsOnFirstBoundary = (i == 0);
    if (IsOnFirstBoundary) {
      adist = fabs(theParameter - theFirstBoundary);
    }
    else {
      adist = fabs(theParameter - theSecondBoundary);
    }
    if(adist < theResolution) {
      return !bRet;
    }
  }
  return bRet;
}
// ------------------------------------------------------------------------------------------------
// static function: FindPoint
// purpose:
// ------------------------------------------------------------------------------------------------
Standard_Boolean FindPoint(const gp_Pnt2d&     theFirstPoint,
			   const gp_Pnt2d&     theLastPoint,
			   const Standard_Real theUmin, 
			   const Standard_Real theUmax,
			   const Standard_Real theVmin,
			   const Standard_Real theVmax,
			   gp_Pnt2d&           theNewPoint) {
  
  gp_Vec2d aVec(theFirstPoint, theLastPoint);
  Standard_Integer i = 0, j = 0;

  for(i = 0; i < 4; i++) {
    gp_Vec2d anOtherVec;
    gp_Vec2d anOtherVecNormal;
    gp_Pnt2d aprojpoint = theLastPoint;    

    if((i % 2) == 0) {
      anOtherVec.SetX(0.);
      anOtherVec.SetY(1.);
      anOtherVecNormal.SetX(1.);
      anOtherVecNormal.SetY(0.);

      if(i < 2)
	aprojpoint.SetX(theUmin);
      else
	aprojpoint.SetX(theUmax);
    }
    else {
      anOtherVec.SetX(1.);
      anOtherVec.SetY(0.);
      anOtherVecNormal.SetX(0.);
      anOtherVecNormal.SetY(1.);

      if(i < 2)
	aprojpoint.SetY(theVmin);
      else
	aprojpoint.SetY(theVmax);
    }
    gp_Vec2d anormvec = aVec;
    anormvec.Normalize();
    Standard_Real adot1 = anormvec.Dot(anOtherVecNormal);

    if(fabs(adot1) < Precision::Angular())
      continue;
    Standard_Real adist = 0.;
    Standard_Boolean bIsOut = Standard_False;

    if((i % 2) == 0) {
      adist = (i < 2) ? fabs(theLastPoint.X() - theUmin) : fabs(theLastPoint.X() - theUmax);
      bIsOut = (i < 2) ? (theLastPoint.X() < theUmin) : (theLastPoint.X() > theUmax);
    }
    else {
      adist = (i < 2) ? fabs(theLastPoint.Y() - theVmin) : fabs(theLastPoint.Y() - theVmax);
      bIsOut = (i < 2) ? (theLastPoint.Y() < theVmin) : (theLastPoint.Y() > theVmax);
    }
    Standard_Real anoffset = adist * anOtherVec.Dot(anormvec) / adot1;

    for(j = 0; j < 2; j++) {
      anoffset = (j == 0) ? anoffset : -anoffset;
      gp_Pnt2d acurpoint(aprojpoint.XY() + (anOtherVec.XY()*anoffset));
      gp_Vec2d acurvec(theLastPoint, acurpoint);
      if ( bIsOut )
	acurvec.Reverse();

      if((aVec.Dot(acurvec) > 0.) &&
	 (aVec.Angle(acurvec) < Precision::PConfusion())) {
	if((i % 2) == 0) {
	  if((acurpoint.Y() >= theVmin) &&
	     (acurpoint.Y() <= theVmax)) {
	    theNewPoint = acurpoint;
	    return Standard_True;
	  }
	}
	else {
	  if((acurpoint.X() >= theUmin) &&
	     (acurpoint.X() <= theUmax)) {
	    theNewPoint = acurpoint;
	    return Standard_True;
	  }
	}
      }
    }
  }
  return Standard_False;
}


// ------------------------------------------------------------------------------------------------
// static function: FindPoint
// purpose: Find point on the boundary of radial tangent zone
// ------------------------------------------------------------------------------------------------
Standard_Boolean FindPoint(const gp_Pnt2d&     theFirstPoint,
			   const gp_Pnt2d&     theLastPoint,
			   const Standard_Real theUmin, 
			   const Standard_Real theUmax,
			   const Standard_Real theVmin,
			   const Standard_Real theVmax,
			   const gp_Pnt2d&     theTanZoneCenter,
			   const Standard_Real theZoneRadius,
			   Handle(GeomAdaptor_HSurface) theGASurface,
			   gp_Pnt2d&           theNewPoint) {
  theNewPoint = theLastPoint;

  if ( !IsInsideTanZone( theLastPoint, theTanZoneCenter, theZoneRadius, theGASurface) )
    return Standard_False;

  Standard_Real aUResolution = theGASurface->UResolution( theZoneRadius );
  Standard_Real aVResolution = theGASurface->VResolution( theZoneRadius );

  Standard_Real aRadius = ( aUResolution < aVResolution ) ? aUResolution : aVResolution;
  gp_Ax22d anAxis( theTanZoneCenter, gp_Dir2d(1, 0), gp_Dir2d(0, 1) );
  gp_Circ2d aCircle( anAxis, aRadius );
  
  //
  gp_Vec2d aDir( theLastPoint.XY() - theFirstPoint.XY() );
  Standard_Real aLength = aDir.Magnitude();
  if ( aLength <= gp::Resolution() )
    return Standard_False;
  gp_Lin2d aLine( theFirstPoint, aDir );

  //
  Handle(Geom2d_Line) aCLine = new Geom2d_Line( aLine );
  Handle(Geom2d_TrimmedCurve) aC1 = new Geom2d_TrimmedCurve( aCLine, 0, aLength );
  Handle(Geom2d_Circle) aC2 = new Geom2d_Circle( aCircle );

  Standard_Real aTol = aRadius * 0.001;
  aTol = ( aTol < Precision::PConfusion() ) ? Precision::PConfusion() : aTol;

  Geom2dAPI_InterCurveCurve anIntersector;
  anIntersector.Init( aC1, aC2, aTol );

  if ( anIntersector.NbPoints() == 0 )
    return Standard_False;

  Standard_Boolean aFound = Standard_False;
  Standard_Real aMinDist = aLength * aLength;
  Standard_Integer i = 0;
  for ( i = 1; i <= anIntersector.NbPoints(); i++ ) {
    gp_Pnt2d aPInt = anIntersector.Point( i );
    if ( aPInt.SquareDistance( theFirstPoint ) < aMinDist ) {
      if ( ( aPInt.X() >= theUmin ) && ( aPInt.X() <= theUmax ) &&
	   ( aPInt.Y() >= theVmin ) && ( aPInt.Y() <= theVmax ) ) {
	theNewPoint = aPInt;
	aFound = Standard_True;
      }
    }
  }

  return aFound;
}

// ------------------------------------------------------------------------------------------------
// static function: IsInsideTanZone
// purpose: Check if point is inside a radial tangent zone
// ------------------------------------------------------------------------------------------------
Standard_Boolean IsInsideTanZone(const gp_Pnt2d&     thePoint,
				 const gp_Pnt2d&     theTanZoneCenter,
				 const Standard_Real theZoneRadius,
				 Handle(GeomAdaptor_HSurface) theGASurface) {

  Standard_Real aUResolution = theGASurface->UResolution( theZoneRadius );
  Standard_Real aVResolution = theGASurface->VResolution( theZoneRadius );
  Standard_Real aRadiusSQR = ( aUResolution < aVResolution ) ? aUResolution : aVResolution;
  aRadiusSQR *= aRadiusSQR;
  if ( thePoint.SquareDistance( theTanZoneCenter ) <= aRadiusSQR )
    return Standard_True;
  return Standard_False;
}

// ------------------------------------------------------------------------------------------------
// static function: CheckTangentZonesExist
// purpose: Check if tangent zone exists
// ------------------------------------------------------------------------------------------------
Standard_Boolean CheckTangentZonesExist( const Handle(GeomAdaptor_HSurface)& theSurface1,
					const Handle(GeomAdaptor_HSurface)&  theSurface2 ) 
{
  if ( ( theSurface1->GetType() != GeomAbs_Torus ) ||
      ( theSurface2->GetType() != GeomAbs_Torus ) )
    return Standard_False;

  IntTools_Context aContext;

  gp_Torus aTor1 = theSurface1->Torus();
  gp_Torus aTor2 = theSurface2->Torus();

  if ( aTor1.Location().Distance( aTor2.Location() ) > Precision::Confusion() )
    return Standard_False;

  if ( ( fabs( aTor1.MajorRadius() - aTor2.MajorRadius() ) > Precision::Confusion() ) ||
       ( fabs( aTor1.MinorRadius() - aTor2.MinorRadius() ) > Precision::Confusion() ) )
    return Standard_False;

  if ( ( aTor1.MajorRadius() < aTor1.MinorRadius() ) ||
       ( aTor2.MajorRadius() < aTor2.MinorRadius() ) )
    return Standard_False;
  return Standard_True;
}

// ------------------------------------------------------------------------------------------------
// static function: ComputeTangentZones
// purpose: 
// ------------------------------------------------------------------------------------------------
Standard_Integer ComputeTangentZones( const Handle(GeomAdaptor_HSurface)& theSurface1,
				     const Handle(GeomAdaptor_HSurface)&  theSurface2,
				     const TopoDS_Face&                   theFace1,
				     const TopoDS_Face&                   theFace2,
				     Handle(TColgp_HArray1OfPnt2d)&       theResultOnS1,
				     Handle(TColgp_HArray1OfPnt2d)&       theResultOnS2,
				     Handle(TColStd_HArray1OfReal)&       theResultRadius) {
  Standard_Integer aResult = 0;
  if ( !CheckTangentZonesExist( theSurface1, theSurface2 ) )
    return aResult;

  IntTools_Context aContext;

  TColgp_SequenceOfPnt2d aSeqResultS1, aSeqResultS2;
  TColStd_SequenceOfReal aSeqResultRad;

  gp_Torus aTor1 = theSurface1->Torus();
  gp_Torus aTor2 = theSurface2->Torus();

  gp_Ax2 anax1( aTor1.Location(), aTor1.Axis().Direction() );
  gp_Ax2 anax2( aTor2.Location(), aTor2.Axis().Direction() );
  Standard_Integer j = 0;

  for ( j = 0; j < 2; j++ ) {
    Standard_Real aCoef = ( j == 0 ) ? -1 : 1;
    Standard_Real aRadius1 = fabs(aTor1.MajorRadius() + aCoef * aTor1.MinorRadius());
    Standard_Real aRadius2 = fabs(aTor2.MajorRadius() + aCoef * aTor2.MinorRadius());

    gp_Circ aCircle1( anax1, aRadius1 );
    gp_Circ aCircle2( anax2, aRadius2 );

    // roughly compute radius of tangent zone for perpendicular case
    Standard_Real aCriteria = Precision::Confusion() * 0.5;

    Standard_Real aT1 = aCriteria;
    Standard_Real aT2 = aCriteria;
    if ( j == 0 ) {
      // internal tangency
      Standard_Real aR = ( aRadius1 > aTor2.MinorRadius() ) ? aRadius1 : aTor2.MinorRadius();
      //aT1 = aCriteria * aCriteria + aR * aR - ( aR - aCriteria ) * ( aR - aCriteria );
      aT1 = 2. * aR * aCriteria;
      aT2 = aT1;
    }
    else {
      // external tangency
      Standard_Real aRb = ( aRadius1 > aTor2.MinorRadius() ) ? aRadius1 : aTor2.MinorRadius();
      Standard_Real aRm = ( aRadius1 < aTor2.MinorRadius() ) ? aRadius1 : aTor2.MinorRadius();
      Standard_Real aDelta = aRb - aCriteria;
      aDelta *= aDelta;
      aDelta -= aRm * aRm;
      aDelta /= 2. * (aRb - aRm);
      aDelta -= 0.5 * (aRb - aRm);
      
      aT1 = 2. * aRm * (aRm - aDelta);
      aT2 = aT1;
    }
    aCriteria = ( aT1 > aT2) ? aT1 : aT2;
    if ( aCriteria > 0 )
      aCriteria = sqrt( aCriteria );

    if ( aCriteria > 0.5 * aTor1.MinorRadius() ) {
      // too big zone -> drop to minimum
      aCriteria = Precision::Confusion();
    }

    GeomAdaptor_Curve aC1( new Geom_Circle(aCircle1) );
    GeomAdaptor_Curve aC2( new Geom_Circle(aCircle2) );
    Extrema_ExtCC anExtrema(aC1, aC2, 0, 2.*Standard_PI, 0, 2.*Standard_PI, 
			    Precision::PConfusion(), Precision::PConfusion());
    	
    if ( anExtrema.IsDone() ) {

      Standard_Integer i = 0;
      for ( i = 1; i <= anExtrema.NbExt(); i++ ) {
	if ( anExtrema.SquareDistance(i) > aCriteria * aCriteria )
	  continue;

	Extrema_POnCurv P1, P2;
	anExtrema.Points( i, P1, P2 );

	Standard_Boolean bFoundResult = Standard_True;
	gp_Pnt2d pr1, pr2;

	Standard_Integer surfit = 0;
	for ( surfit = 0; surfit < 2; surfit++ ) {
	  GeomAPI_ProjectPointOnSurf& aProjector = (surfit == 0) ? aContext.ProjPS(theFace1) : aContext.ProjPS(theFace2);

	  gp_Pnt aP3d = (surfit == 0) ? P1.Value() : P2.Value();
	  aProjector.Perform(aP3d);

	  if(!aProjector.IsDone())
	    bFoundResult = Standard_False;
	  else {
	    if(aProjector.LowerDistance() > aCriteria) {
	      bFoundResult = Standard_False;
	    }
	    else {
	      Standard_Real foundU = 0, foundV = 0;
	      aProjector.LowerDistanceParameters(foundU, foundV);
	      if ( surfit == 0 )
		pr1 = gp_Pnt2d( foundU, foundV );
	      else
		pr2 = gp_Pnt2d( foundU, foundV );
	    }
	  }
	}
	if ( bFoundResult ) {
	  aSeqResultS1.Append( pr1 );
	  aSeqResultS2.Append( pr2 );
	  aSeqResultRad.Append( aCriteria );

	  // torus is u and v periodic
	  const Standard_Real twoPI = Standard_PI + Standard_PI;
	  Standard_Real arr1tmp[2] = {pr1.X(), pr1.Y()};
	  Standard_Real arr2tmp[2] = {pr2.X(), pr2.Y()};

	  // iteration on period bounds
	  for ( Standard_Integer k1 = 0; k1 < 2; k1++ ) {
	    Standard_Real aBound = ( k1 == 0 ) ? 0 : twoPI;
	    Standard_Real aShift = ( k1 == 0 ) ? twoPI : -twoPI;

	    // iteration on surfaces
	    for ( Standard_Integer k2 = 0; k2 < 2; k2++ ) {
	      Standard_Real* arr1 = ( k2 == 0 ) ? arr1tmp : arr2tmp;
	      Standard_Real* arr2 = ( k2 != 0 ) ? arr1tmp : arr2tmp;
	      TColgp_SequenceOfPnt2d& aSeqS1 = ( k2 == 0 ) ? aSeqResultS1 : aSeqResultS2; 
	      TColgp_SequenceOfPnt2d& aSeqS2 = ( k2 != 0 ) ? aSeqResultS1 : aSeqResultS2; 

	      if (fabs(arr1[0] - aBound) < Precision::PConfusion()) {
		aSeqS1.Append( gp_Pnt2d( arr1[0] + aShift, arr1[1] ) );
		aSeqS2.Append( gp_Pnt2d( arr2[0], arr2[1] ) );
		aSeqResultRad.Append( aCriteria );
	      }
	      if (fabs(arr1[1] - aBound) < Precision::PConfusion()) {
		aSeqS1.Append( gp_Pnt2d( arr1[0], arr1[1] + aShift) );
		aSeqS2.Append( gp_Pnt2d( arr2[0], arr2[1] ) );
		aSeqResultRad.Append( aCriteria );
	      }
	    }
	  } //
	}
      }
    }
  }
  aResult = aSeqResultRad.Length();

  if ( aResult > 0 ) {
    theResultOnS1 = new TColgp_HArray1OfPnt2d( 1, aResult );
    theResultOnS2 = new TColgp_HArray1OfPnt2d( 1, aResult );
    theResultRadius = new TColStd_HArray1OfReal( 1, aResult );

    for ( Standard_Integer i = 1 ; i <= aResult; i++ ) {
      theResultOnS1->SetValue( i, aSeqResultS1.Value(i) );
      theResultOnS2->SetValue( i, aSeqResultS2.Value(i) );
      theResultRadius->SetValue( i, aSeqResultRad.Value(i) );
    }
  }
  return aResult;
}

// ------------------------------------------------------------------------------------------------
// static function: AdjustByNeighbour
// purpose:
// ------------------------------------------------------------------------------------------------
gp_Pnt2d AdjustByNeighbour(const gp_Pnt2d&     theaNeighbourPoint,
			   const gp_Pnt2d&     theOriginalPoint,
			   Handle(GeomAdaptor_HSurface) theGASurface) {
  
  gp_Pnt2d ap1 = theaNeighbourPoint;
  gp_Pnt2d ap2 = theOriginalPoint;

  if ( theGASurface->IsUPeriodic() ) {
    Standard_Real aPeriod     = theGASurface->UPeriod();
    gp_Pnt2d aPTest = ap2;
    Standard_Real aSqDistMin = 1.e+100;

    for ( Standard_Integer pIt = -1; pIt <= 1; pIt++) {
      aPTest.SetX( theOriginalPoint.X() + aPeriod * pIt );
      Standard_Real dd = ap1.SquareDistance( aPTest );

      if ( dd < aSqDistMin ) {
	ap2 = aPTest;
	aSqDistMin = dd;
      }
    }
  }
  if ( theGASurface->IsVPeriodic() ) {
    Standard_Real aPeriod     = theGASurface->VPeriod();
    gp_Pnt2d aPTest = ap2;
    Standard_Real aSqDistMin = 1.e+100;

    for ( Standard_Integer pIt = -1; pIt <= 1; pIt++) {
      aPTest.SetY( theOriginalPoint.Y() + aPeriod * pIt );
      Standard_Real dd = ap1.SquareDistance( aPTest );

      if ( dd < aSqDistMin ) {
	ap2 = aPTest;
	aSqDistMin = dd;
      }
    }
  }
  return ap2;
}

// ------------------------------------------------------------------------------------------------
//function: DecompositionOfWLine
// purpose:
// ------------------------------------------------------------------------------------------------
Standard_Boolean DecompositionOfWLine(const Handle(IntPatch_WLine)& theWLine,
				      const Handle(GeomAdaptor_HSurface)&            theSurface1, 
				      const Handle(GeomAdaptor_HSurface)&            theSurface2,
				      const TopoDS_Face&                             theFace1,
				      const TopoDS_Face&                             theFace2,
				      const IntTools_LineConstructor&                theLConstructor,
				      const Standard_Boolean                         theAvoidLConstructor,
				      IntPatch_SequenceOfLine&                       theNewLines,
				      Standard_Real&                                 theReachedTol3d) {

  Standard_Boolean bRet, bAvoidLineConstructor;
  Standard_Integer aNbPnts, aNbParts;
  //
  bRet=Standard_False;
  aNbPnts=theWLine->NbPnts();
  bAvoidLineConstructor=theAvoidLConstructor;
  //
  if(!aNbPnts){
    return bRet;
  }
  if (!bAvoidLineConstructor) {
    aNbParts=theLConstructor.NbParts();
    if (!aNbParts) {
      return bRet;
    }
  }
  //
  Standard_Boolean bIsPrevPointOnBoundary, bIsPointOnBoundary, bIsCurrentPointOnBoundary;
  Standard_Integer nblines, pit, i, j;
  Standard_Real aTol;
  TColStd_Array1OfListOfInteger anArrayOfLines(1, aNbPnts); 
  TColStd_Array1OfInteger       anArrayOfLineType(1, aNbPnts);
  TColStd_ListOfInteger aListOfPointIndex;
  IntTools_Context aContext;
  
  Handle(TColgp_HArray1OfPnt2d) aTanZoneS1;
  Handle(TColgp_HArray1OfPnt2d) aTanZoneS2;
  Handle(TColStd_HArray1OfReal) aTanZoneRadius;
  Standard_Integer aNbZone = ComputeTangentZones( theSurface1, theSurface2, theFace1, theFace2,
						 aTanZoneS1, aTanZoneS2, aTanZoneRadius );
  
  //
  nblines=0;
  aTol=Precision::Confusion();
  aTol=0.5*aTol;
  bIsPrevPointOnBoundary=Standard_False;
  bIsPointOnBoundary=Standard_False;
  //
  // 1. ...
  //
  // Points
  for(pit = 1; pit <= aNbPnts; ++pit) {
    Standard_Boolean bIsOnFirstBoundary, isperiodic;
    Standard_Real aResolution, aPeriod, alowerboundary, aupperboundary, U, V;
    Standard_Real aParameter, anoffset, anAdjustPar;
    Standard_Real umin, umax, vmin, vmax;
    //
    bIsCurrentPointOnBoundary = Standard_False;
    const IntSurf_PntOn2S& aPoint = theWLine->Point(pit);
    //
    // Surface
    for(i = 0; i < 2; ++i) {
      Handle(GeomAdaptor_HSurface) aGASurface = (!i) ? theSurface1 : theSurface2;
      aGASurface->ChangeSurface().Surface()->Bounds(umin, umax, vmin, vmax);
      if(!i) {
	aPoint.ParametersOnS1(U, V);
      }
      else {
	aPoint.ParametersOnS2(U, V);
      }
      // U, V
      for(j = 0; j < 2; j++) {
	isperiodic = (!j) ? aGASurface->IsUPeriodic() : aGASurface->IsVPeriodic();
	if(!isperiodic){
	  continue;
	}
	//
	if (!j) {
	  aResolution=aGASurface->UResolution(aTol);
	  aPeriod=aGASurface->UPeriod();
	  alowerboundary=umin;
	  aupperboundary=umax;
	  aParameter=U;
	}
	else {
	  aResolution=aGASurface->VResolution(aTol);
	  aPeriod=aGASurface->VPeriod();
	  alowerboundary=vmin;
	  aupperboundary=vmax;
	  aParameter=V;
	}
	
	anoffset = 0.;
	anAdjustPar = AdjustPeriodic(aParameter, 
				     alowerboundary, 
				     aupperboundary, 
				     aPeriod, 
				     anoffset);
	//
	bIsOnFirstBoundary = Standard_True;// ?
	bIsPointOnBoundary=
	  IsPointOnBoundary(anAdjustPar, 
			    alowerboundary, 
			    aupperboundary,
			    aResolution, 
			    bIsOnFirstBoundary);
	//
	if(bIsPointOnBoundary) {
	  bIsCurrentPointOnBoundary = Standard_True;
	  break;
	}
	else {
	  // check if a point belong to a tangent zone. Begin
	  Standard_Integer zIt = 0;
	  for ( zIt = 1; zIt <= aNbZone; zIt++ ) {
	    gp_Pnt2d aPZone = (i == 0) ? aTanZoneS1->Value(zIt) : aTanZoneS2->Value(zIt);
	    Standard_Real aZoneRadius = aTanZoneRadius->Value(zIt);

	    if ( IsInsideTanZone(gp_Pnt2d( U, V ), aPZone, aZoneRadius, aGASurface ) ) {
	      // set boundary flag to split the curve by a tangent zone
	      bIsPointOnBoundary = Standard_True;
	      bIsCurrentPointOnBoundary = Standard_True;
	      if ( theReachedTol3d < aZoneRadius ) {
		theReachedTol3d = aZoneRadius;
	      }
	      break;
	    }
	  }
	}
      }//for(j = 0; j < 2; j++) {

      if(bIsCurrentPointOnBoundary){
	break;
      }
    }//for(i = 0; i < 2; ++i) {
    //
    if((bIsCurrentPointOnBoundary != bIsPrevPointOnBoundary)) {
      if(!aListOfPointIndex.IsEmpty()) {
	nblines++;
	anArrayOfLines.SetValue(nblines, aListOfPointIndex);
	anArrayOfLineType.SetValue(nblines, bIsPrevPointOnBoundary);
	aListOfPointIndex.Clear();
      }
      bIsPrevPointOnBoundary = bIsCurrentPointOnBoundary;
    }
    aListOfPointIndex.Append(pit);
  } //for(pit = 1; pit <= aNbPnts; ++pit) {
  //
  if(!aListOfPointIndex.IsEmpty()) {
    nblines++;
    anArrayOfLines.SetValue(nblines, aListOfPointIndex);
    anArrayOfLineType.SetValue(nblines, bIsPrevPointOnBoundary);
    aListOfPointIndex.Clear();
  }
  //
  if(nblines<=1) {
    return bRet; //Standard_False;
  }
  //
  // 
  // 2. Correct wlines.begin
  TColStd_Array1OfListOfInteger anArrayOfLineEnds(1, nblines);
  Handle(IntSurf_LineOn2S) aSeqOfPntOn2S = new IntSurf_LineOn2S();
  //
  for(i = 1; i <= nblines; i++) {
    if(anArrayOfLineType.Value(i) != 0) {
      continue;
    }
    const TColStd_ListOfInteger& aListOfIndex = anArrayOfLines.Value(i);
    if(aListOfIndex.Extent() < 2) {
      continue;
    }
    TColStd_ListOfInteger aListOfFLIndex;

    for(j = 0; j < 2; j++) {
      Standard_Integer aneighbourindex = (j == 0) ? (i - 1) : (i + 1);

      if((aneighbourindex < 1) || (aneighbourindex > nblines))
	continue;

      if(anArrayOfLineType.Value(aneighbourindex) == 0)
	continue;
      const TColStd_ListOfInteger& aNeighbour = anArrayOfLines.Value(aneighbourindex);
      Standard_Integer anIndex = (j == 0) ? aNeighbour.Last() : aNeighbour.First();
      const IntSurf_PntOn2S& aPoint = theWLine->Point(anIndex);

      IntSurf_PntOn2S aNewP = aPoint;
      
      for(Standard_Integer surfit = 0; surfit < 2; surfit++) {

	Handle(GeomAdaptor_HSurface) aGASurface = (surfit == 0) ? theSurface1 : theSurface2;
	Standard_Real umin=0., umax=0., vmin=0., vmax=0.;
	aGASurface->ChangeSurface().Surface()->Bounds(umin, umax, vmin, vmax);
	Standard_Real U=0., V=0.;

	if(surfit == 0)
	  aNewP.ParametersOnS1(U, V);
	else
	  aNewP.ParametersOnS2(U, V);
	Standard_Integer nbboundaries = 0;

	Standard_Boolean bIsNearBoundary = Standard_False;
	Standard_Integer aZoneIndex = 0;
	Standard_Integer bIsUBoundary = Standard_False; // use if nbboundaries == 1
	Standard_Integer bIsFirstBoundary = Standard_False; // use if nbboundaries == 1
	

	for(Standard_Integer parit = 0; parit < 2; parit++) {
	  Standard_Boolean isperiodic = (parit == 0) ? aGASurface->IsUPeriodic() : aGASurface->IsVPeriodic();

	  Standard_Real aResolution = (parit == 0) ? aGASurface->UResolution(aTol) : aGASurface->VResolution(aTol);
	  Standard_Real alowerboundary = (parit == 0) ? umin : vmin;
	  Standard_Real aupperboundary = (parit == 0) ? umax : vmax;

	  Standard_Real aParameter = (parit == 0) ? U : V;
	  Standard_Boolean bIsOnFirstBoundary = Standard_True;
  
	  if(!isperiodic) {
	    bIsPointOnBoundary=
	      IsPointOnBoundary(aParameter, alowerboundary, aupperboundary, aResolution, bIsOnFirstBoundary);
	    if(bIsPointOnBoundary) {
	      bIsUBoundary = (parit == 0);
	      bIsFirstBoundary = bIsOnFirstBoundary;
	      nbboundaries++;
	    }
	  }
	  else {
	    Standard_Real aPeriod     = (parit == 0) ? aGASurface->UPeriod() : aGASurface->VPeriod();
	    Standard_Real anoffset = 0.;
	    Standard_Real anAdjustPar = AdjustPeriodic(aParameter, alowerboundary, aupperboundary, aPeriod, anoffset);

	    bIsPointOnBoundary=
	      IsPointOnBoundary(anAdjustPar, alowerboundary, aupperboundary, aResolution, bIsOnFirstBoundary);
	    if(bIsPointOnBoundary) {
	      bIsUBoundary = (parit == 0);
	      bIsFirstBoundary = bIsOnFirstBoundary;
	      nbboundaries++;
	    }
	    else {
	      //check neighbourhood of boundary
	      Standard_Real anEpsilon = aResolution * 100.;
	      Standard_Real aPart = ( aupperboundary - alowerboundary ) * 0.1;
	      anEpsilon = ( anEpsilon > aPart ) ? aPart : anEpsilon;
		
	      bIsNearBoundary = IsPointOnBoundary(anAdjustPar, alowerboundary, aupperboundary, 
						  anEpsilon, bIsOnFirstBoundary);

	    }
	  }
	}

	// check if a point belong to a tangent zone. Begin
	for ( Standard_Integer zIt = 1; zIt <= aNbZone; zIt++ ) {
	  gp_Pnt2d aPZone = (surfit == 0) ? aTanZoneS1->Value(zIt) : aTanZoneS2->Value(zIt);
	  Standard_Real aZoneRadius = aTanZoneRadius->Value(zIt);

	  Standard_Integer aneighbourpointindex1 = (j == 0) ? aListOfIndex.First() : aListOfIndex.Last();
	  const IntSurf_PntOn2S& aNeighbourPoint = theWLine->Point(aneighbourpointindex1);
	  Standard_Real nU1, nV1;
	    
	  if(surfit == 0)
	    aNeighbourPoint.ParametersOnS1(nU1, nV1);
	  else
	    aNeighbourPoint.ParametersOnS2(nU1, nV1);
	  gp_Pnt2d ap1(nU1, nV1);
	  gp_Pnt2d ap2 = AdjustByNeighbour( ap1, gp_Pnt2d( U, V ), aGASurface );


	  if ( IsInsideTanZone( ap2, aPZone, aZoneRadius, aGASurface ) ) {
	    aZoneIndex = zIt;
	    bIsNearBoundary = Standard_True;
	    if ( theReachedTol3d < aZoneRadius ) {
	      theReachedTol3d = aZoneRadius;
	    }
	  }
	}
	// check if a point belong to a tangent zone. End
	Standard_Boolean bComputeLineEnd = Standard_False;

	if(nbboundaries == 2) {
	  //xf
	  //bComputeLineEnd = Standard_True;
	  //xt
	}
	else if(nbboundaries == 1) {
	  Standard_Boolean isperiodic = (bIsUBoundary) ? aGASurface->IsUPeriodic() : aGASurface->IsVPeriodic();

	  if(isperiodic) {
	    Standard_Real alowerboundary = (bIsUBoundary) ? umin : vmin;
	    Standard_Real aupperboundary = (bIsUBoundary) ? umax : vmax;
	    Standard_Real aPeriod     = (bIsUBoundary) ? aGASurface->UPeriod() : aGASurface->VPeriod();
	    Standard_Real aParameter = (bIsUBoundary) ? U : V;
	    Standard_Real anoffset = 0.;
	    Standard_Real anAdjustPar = AdjustPeriodic(aParameter, alowerboundary, aupperboundary, aPeriod, anoffset);

	    Standard_Real adist = (bIsFirstBoundary) ? fabs(anAdjustPar - alowerboundary) : fabs(anAdjustPar - aupperboundary);
	    Standard_Real anotherPar = (bIsFirstBoundary) ? (aupperboundary - adist) : (alowerboundary + adist);
	    anotherPar += anoffset;
	    Standard_Integer aneighbourpointindex = (j == 0) ? aListOfIndex.First() : aListOfIndex.Last();
	    const IntSurf_PntOn2S& aNeighbourPoint = theWLine->Point(aneighbourpointindex);
	    Standard_Real nU1, nV1;

	    if(surfit == 0)
	      aNeighbourPoint.ParametersOnS1(nU1, nV1);
	    else
	      aNeighbourPoint.ParametersOnS2(nU1, nV1);
	    
	    Standard_Real adist1 = (bIsUBoundary) ? fabs(nU1 - U) : fabs(nV1 - V);
	    Standard_Real adist2 = (bIsUBoundary) ? fabs(nU1 - anotherPar) : fabs(nV1 - anotherPar);
	    bComputeLineEnd = Standard_True;
	    Standard_Boolean bCheckAngle1 = Standard_False;
	    Standard_Boolean bCheckAngle2 = Standard_False;
	    gp_Vec2d aNewVec;
	    Standard_Real anewU = (bIsUBoundary) ? anotherPar : U;
	    Standard_Real anewV = (bIsUBoundary) ? V : anotherPar;

	    if(((adist1 - adist2) > Precision::PConfusion()) && 
	       (adist2 < (aPeriod / 4.))) {
	      bCheckAngle1 = Standard_True;
	      aNewVec = gp_Vec2d(gp_Pnt2d(nU1, nV1), gp_Pnt2d(anewU, anewV));

	      if(aNewVec.SquareMagnitude() < (gp::Resolution() * gp::Resolution())) {
		aNewP.SetValue((surfit == 0), anewU, anewV);
		bCheckAngle1 = Standard_False;
	      }
	    }
	    else if(adist1 < (aPeriod / 4.)) {
	      bCheckAngle2 = Standard_True;
	      aNewVec = gp_Vec2d(gp_Pnt2d(nU1, nV1), gp_Pnt2d(U, V));

	      if(aNewVec.SquareMagnitude() < (gp::Resolution() * gp::Resolution())) {
		bCheckAngle2 = Standard_False;
	      }
	    }

	    if(bCheckAngle1 || bCheckAngle2) {
	      // assume there are at least two points in line (see "if" above)
	      Standard_Integer anindexother = aneighbourpointindex;

	      while((anindexother <= aListOfIndex.Last()) && (anindexother >= aListOfIndex.First())) {
		anindexother = (j == 0) ? (anindexother + 1) : (anindexother - 1);
		const IntSurf_PntOn2S& aPrevNeighbourPoint = theWLine->Point(anindexother);
		Standard_Real nU2, nV2;
		
		if(surfit == 0)
		  aPrevNeighbourPoint.ParametersOnS1(nU2, nV2);
		else
		  aPrevNeighbourPoint.ParametersOnS2(nU2, nV2);
		gp_Vec2d aVecOld(gp_Pnt2d(nU2, nV2), gp_Pnt2d(nU1, nV1));

		if(aVecOld.SquareMagnitude() <= (gp::Resolution() * gp::Resolution())) {
		  continue;
		}
		else {
		  Standard_Real anAngle = aNewVec.Angle(aVecOld);

		  if((fabs(anAngle) < (Standard_PI * 0.25)) && (aNewVec.Dot(aVecOld) > 0.)) {

		    if(bCheckAngle1) {
		      Standard_Real U1, U2, V1, V2;
		      IntSurf_PntOn2S atmppoint = aNewP;
		      atmppoint.SetValue((surfit == 0), anewU, anewV);
		      atmppoint.Parameters(U1, V1, U2, V2);
		      gp_Pnt P1 = theSurface1->Value(U1, V1);
		      gp_Pnt P2 = theSurface2->Value(U2, V2);
		      gp_Pnt P0 = aPoint.Value();

		      if(P0.IsEqual(P1, aTol) &&
			 P0.IsEqual(P2, aTol) &&
			 P1.IsEqual(P2, aTol)) {
			bComputeLineEnd = Standard_False;
			aNewP.SetValue((surfit == 0), anewU, anewV);
		      }
		    }

		    if(bCheckAngle2) {
		      bComputeLineEnd = Standard_False;
		    }
		  }
		  break;
		}
	      } // end while(anindexother...)
	    }
	  }
	}
	else if ( bIsNearBoundary ) {
	  bComputeLineEnd = Standard_True;
	}

	if(bComputeLineEnd) {

	  gp_Pnt2d anewpoint;
	  Standard_Boolean found = Standard_False;

	  if ( bIsNearBoundary ) {
	    // re-compute point near natural boundary or near tangent zone
	    Standard_Real u1, v1, u2, v2;
	    aNewP.Parameters( u1, v1, u2, v2 );
	    if(surfit == 0)
	      anewpoint = gp_Pnt2d( u1, v1 );
	    else
	      anewpoint = gp_Pnt2d( u2, v2 );
	    
	    Standard_Integer aneighbourpointindex1 = (j == 0) ? aListOfIndex.First() : aListOfIndex.Last();
	    const IntSurf_PntOn2S& aNeighbourPoint = theWLine->Point(aneighbourpointindex1);
	    Standard_Real nU1, nV1;
	    
	    if(surfit == 0)
	      aNeighbourPoint.ParametersOnS1(nU1, nV1);
	    else
	      aNeighbourPoint.ParametersOnS2(nU1, nV1);
	    gp_Pnt2d ap1(nU1, nV1);
	    gp_Pnt2d ap2;


	    if ( aZoneIndex ) {
	      // exclude point from a tangent zone
	      anewpoint = AdjustByNeighbour( ap1, anewpoint, aGASurface );
	      gp_Pnt2d aPZone = (surfit == 0) ? aTanZoneS1->Value(aZoneIndex) : aTanZoneS2->Value(aZoneIndex);
	      Standard_Real aZoneRadius = aTanZoneRadius->Value(aZoneIndex);

	      if ( FindPoint(ap1, anewpoint, umin, umax, vmin, vmax, 
			     aPZone, aZoneRadius, aGASurface, ap2) ) {
		anewpoint = ap2;
		found = Standard_True;
	      }
	    }
	    else if ( aGASurface->IsUPeriodic() || aGASurface->IsVPeriodic() ) {
	      // re-compute point near boundary if shifted on a period
	      ap2 = AdjustByNeighbour( ap1, anewpoint, aGASurface );

	      if ( ( ap2.X() < umin ) || ( ap2.X() > umax ) ||
		  ( ap2.Y() < vmin ) || ( ap2.Y() > vmax ) ) {
		found = FindPoint(ap1, ap2, umin, umax, vmin, vmax, anewpoint);
	      }
	      else {
		anewpoint = ap2;
		aNewP.SetValue( (surfit == 0), anewpoint.X(), anewpoint.Y() );
	      }
	    }
	  }
	  else {

	    Standard_Integer aneighbourpointindex1 = (j == 0) ? aListOfIndex.First() : aListOfIndex.Last();
	    const IntSurf_PntOn2S& aNeighbourPoint = theWLine->Point(aneighbourpointindex1);
	    Standard_Real nU1, nV1;

	    if(surfit == 0)
	      aNeighbourPoint.ParametersOnS1(nU1, nV1);
	    else
	      aNeighbourPoint.ParametersOnS2(nU1, nV1);
	    gp_Pnt2d ap1(nU1, nV1);
	    gp_Pnt2d ap2(nU1, nV1);
	    Standard_Integer aneighbourpointindex2 = aneighbourpointindex1;

	    while((aneighbourpointindex2 <= aListOfIndex.Last()) && (aneighbourpointindex2 >= aListOfIndex.First())) {
	      aneighbourpointindex2 = (j == 0) ? (aneighbourpointindex2 + 1) : (aneighbourpointindex2 - 1);
	      const IntSurf_PntOn2S& aPrevNeighbourPoint = theWLine->Point(aneighbourpointindex2);
	      Standard_Real nU2, nV2;

	      if(surfit == 0)
		aPrevNeighbourPoint.ParametersOnS1(nU2, nV2);
	      else
		aPrevNeighbourPoint.ParametersOnS2(nU2, nV2);
	      ap2.SetX(nU2);
	      ap2.SetY(nV2);

	      if(ap1.SquareDistance(ap2) > (gp::Resolution() * gp::Resolution())) {
		break;
	      }
	    }  
	    found = FindPoint(ap2, ap1, umin, umax, vmin, vmax, anewpoint);
	  }

	  if(found) {
	    // check point
	    Standard_Real aCriteria = BRep_Tool::Tolerance(theFace1) + BRep_Tool::Tolerance(theFace2);
	    GeomAPI_ProjectPointOnSurf& aProjector = (surfit == 0) ? aContext.ProjPS(theFace2) : aContext.ProjPS(theFace1);
	    Handle(GeomAdaptor_HSurface) aSurface = (surfit == 0) ? theSurface1 : theSurface2;

	    Handle(GeomAdaptor_HSurface) aSurfaceOther = (surfit == 0) ? theSurface2 : theSurface1;

	    gp_Pnt aP3d = aSurface->Value(anewpoint.X(), anewpoint.Y());
	    aProjector.Perform(aP3d);

	    if(aProjector.IsDone()) {
	      if(aProjector.LowerDistance() < aCriteria) {
		Standard_Real foundU = U, foundV = V;
		aProjector.LowerDistanceParameters(foundU, foundV);

		//Correction of projected coordinates. Begin
		//Note, it may be shifted on a period
		Standard_Integer aneindex1 = (j == 0) ? aListOfIndex.First() : aListOfIndex.Last();
		const IntSurf_PntOn2S& aNeighbourPoint = theWLine->Point(aneindex1);
		Standard_Real nUn, nVn;
		
		if(surfit == 0)
		  aNeighbourPoint.ParametersOnS2(nUn, nVn);
		else
		  aNeighbourPoint.ParametersOnS1(nUn, nVn);
		gp_Pnt2d aNeighbour2d(nUn, nVn);
		gp_Pnt2d anAdjustedPoint = AdjustByNeighbour( aNeighbour2d, gp_Pnt2d(foundU, foundV), aSurfaceOther );
		foundU = anAdjustedPoint.X();
		foundV = anAdjustedPoint.Y();

		if ( ( anAdjustedPoint.X() < umin ) && ( anAdjustedPoint.X() > umax ) &&
		    ( anAdjustedPoint.Y() < vmin ) && ( anAdjustedPoint.Y() > vmax ) ) {
		  // attempt to roughly re-compute point
		  foundU = ( foundU < umin ) ? umin : foundU;
		  foundU = ( foundU > umax ) ? umax : foundU;
		  foundV = ( foundV < vmin ) ? vmin : foundV;
		  foundV = ( foundV > vmax ) ? vmax : foundV;

		  GeomAPI_ProjectPointOnSurf& aProjector2 = (surfit == 0) ? aContext.ProjPS(theFace1) : aContext.ProjPS(theFace2);

		  aP3d = aSurfaceOther->Value(foundU, foundV);
		  aProjector2.Perform(aP3d);
		  
		  if(aProjector2.IsDone()) {
		    if(aProjector2.LowerDistance() < aCriteria) {
		      Standard_Real foundU2 = anewpoint.X(), foundV2 = anewpoint.Y();
		      aProjector2.LowerDistanceParameters(foundU2, foundV2);
		      anewpoint.SetX(foundU2);
		      anewpoint.SetY(foundV2);
		    }
		  }
		}
		//Correction of projected coordinates. End

		if(surfit == 0)
		  aNewP.SetValue(aP3d, anewpoint.X(), anewpoint.Y(), foundU, foundV);
		else
		  aNewP.SetValue(aP3d, foundU, foundV, anewpoint.X(), anewpoint.Y());
	      }
	    }
	  }
	}
      }
      aSeqOfPntOn2S->Add(aNewP);
      aListOfFLIndex.Append(aSeqOfPntOn2S->NbPoints());
    }
    anArrayOfLineEnds.SetValue(i, aListOfFLIndex);
  }
  // Correct wlines.end

  // Split wlines.begin
  Standard_Integer nbiter;
  //
  nbiter=1;
  if (!bAvoidLineConstructor) {
    nbiter=theLConstructor.NbParts();
  }
  //
  for(j = 1; j <= nbiter; ++j) {
    Standard_Real fprm, lprm;
    Standard_Integer ifprm, ilprm;
    //
    if(bAvoidLineConstructor) {
      ifprm = 1;
      ilprm = theWLine->NbPnts();
    }
    else {
      theLConstructor.Part(j, fprm, lprm);
      ifprm = (Standard_Integer)fprm;
      ilprm = (Standard_Integer)lprm;
    }

    Handle(IntSurf_LineOn2S) aLineOn2S = new IntSurf_LineOn2S();
    //
    for(i = 1; i <= nblines; i++) {
      if(anArrayOfLineType.Value(i) != 0) {
	continue;
      }
      const TColStd_ListOfInteger& aListOfIndex = anArrayOfLines.Value(i);

      if(aListOfIndex.Extent() < 2) {
	continue;
      }
      const TColStd_ListOfInteger& aListOfFLIndex = anArrayOfLineEnds.Value(i);
      Standard_Boolean bhasfirstpoint = (aListOfFLIndex.Extent() == 2);
      Standard_Boolean bhaslastpoint = (aListOfFLIndex.Extent() == 2);

      if(!bhasfirstpoint && !aListOfFLIndex.IsEmpty()) {
	bhasfirstpoint = (i != 1);
      }

      if(!bhaslastpoint && !aListOfFLIndex.IsEmpty()) {
	bhaslastpoint = (i != nblines);
      }
      Standard_Boolean bIsFirstInside = ((ifprm >= aListOfIndex.First()) && (ifprm <= aListOfIndex.Last()));
      Standard_Boolean bIsLastInside =  ((ilprm >= aListOfIndex.First()) && (ilprm <= aListOfIndex.Last()));

      if(!bIsFirstInside && !bIsLastInside) {
	if((ifprm < aListOfIndex.First()) && (ilprm > aListOfIndex.Last())) {
	  // append whole line, and boundaries if neccesary
	  if(bhasfirstpoint) {
	    const IntSurf_PntOn2S& aP = aSeqOfPntOn2S->Value(aListOfFLIndex.First());
	    aLineOn2S->Add(aP);
	  }
	  TColStd_ListIteratorOfListOfInteger anIt(aListOfIndex);

	  for(; anIt.More(); anIt.Next()) {
	    const IntSurf_PntOn2S& aP = theWLine->Point(anIt.Value());
	    aLineOn2S->Add(aP);
	  }

	  if(bhaslastpoint) {
	    const IntSurf_PntOn2S& aP = aSeqOfPntOn2S->Value(aListOfFLIndex.Last());
	    aLineOn2S->Add(aP);
	  }

	  // check end of split line (end is almost always)
	  Standard_Integer aneighbour = i + 1;
	  Standard_Boolean bIsEndOfLine = Standard_True;

	  if(aneighbour <= nblines) {
	    const TColStd_ListOfInteger& aListOfNeighbourIndex = anArrayOfLines.Value(aneighbour);

	    if((anArrayOfLineType.Value(aneighbour) != 0) &&
	       (aListOfNeighbourIndex.IsEmpty())) {
	      bIsEndOfLine = Standard_False;
	    }
	  }

	  if(bIsEndOfLine) {
	    if(aLineOn2S->NbPoints() > 1) {
	      Handle(IntPatch_WLine) aNewWLine = 
		new IntPatch_WLine(aLineOn2S, Standard_False);
	      theNewLines.Append(aNewWLine);
	    }
	    aLineOn2S = new IntSurf_LineOn2S();
	  }
	}
	continue;
      }
      // end if(!bIsFirstInside && !bIsLastInside)

      if(bIsFirstInside && bIsLastInside) {
	// append inside points between ifprm and ilprm
	TColStd_ListIteratorOfListOfInteger anIt(aListOfIndex);

	for(; anIt.More(); anIt.Next()) {
	  if((anIt.Value() < ifprm) || (anIt.Value() > ilprm))
	    continue;
	  const IntSurf_PntOn2S& aP = theWLine->Point(anIt.Value());
	  aLineOn2S->Add(aP);
	}
      }
      else {

	if(bIsFirstInside) {
	  // append points from ifprm to last point + boundary point
	  TColStd_ListIteratorOfListOfInteger anIt(aListOfIndex);

	  for(; anIt.More(); anIt.Next()) {
	    if(anIt.Value() < ifprm)
	      continue;
	    const IntSurf_PntOn2S& aP = theWLine->Point(anIt.Value());
	    aLineOn2S->Add(aP);
	  }

	  if(bhaslastpoint) {
	    const IntSurf_PntOn2S& aP = aSeqOfPntOn2S->Value(aListOfFLIndex.Last());
	    aLineOn2S->Add(aP);
	  }
	  // check end of split line (end is almost always)
	  Standard_Integer aneighbour = i + 1;
	  Standard_Boolean bIsEndOfLine = Standard_True;

	  if(aneighbour <= nblines) {
	    const TColStd_ListOfInteger& aListOfNeighbourIndex = anArrayOfLines.Value(aneighbour);

	    if((anArrayOfLineType.Value(aneighbour) != 0) &&
	       (aListOfNeighbourIndex.IsEmpty())) {
	      bIsEndOfLine = Standard_False;
	    }
	  }

	  if(bIsEndOfLine) {
	    if(aLineOn2S->NbPoints() > 1) {
	      Handle(IntPatch_WLine) aNewWLine = 
		new IntPatch_WLine(aLineOn2S, Standard_False);
	      theNewLines.Append(aNewWLine);
	    }
	    aLineOn2S = new IntSurf_LineOn2S();
	  }
	}
	// end if(bIsFirstInside)

	if(bIsLastInside) {
	  // append points from first boundary point to ilprm
	  if(bhasfirstpoint) {
	    const IntSurf_PntOn2S& aP = aSeqOfPntOn2S->Value(aListOfFLIndex.First());
	    aLineOn2S->Add(aP);
	  }
	  TColStd_ListIteratorOfListOfInteger anIt(aListOfIndex);

	  for(; anIt.More(); anIt.Next()) {
	    if(anIt.Value() > ilprm)
	      continue;
	    const IntSurf_PntOn2S& aP = theWLine->Point(anIt.Value());
	    aLineOn2S->Add(aP);
	  }
	}
	//end if(bIsLastInside)
      }
    }

    if(aLineOn2S->NbPoints() > 1) {
      Handle(IntPatch_WLine) aNewWLine = 
	new IntPatch_WLine(aLineOn2S, Standard_False);
      theNewLines.Append(aNewWLine);
    }
  }
  // Split wlines.end

  return Standard_True;
}

// ------------------------------------------------------------------------------------------------
// static function: ParameterOutOfBoundary
// purpose:         Computes a new parameter for given curve. The corresponding 2d points 
//                  does not lay on any boundary of given faces
// ------------------------------------------------------------------------------------------------
Standard_Boolean ParameterOutOfBoundary(const Standard_Real       theParameter, 
					const Handle(Geom_Curve)& theCurve, 
					const TopoDS_Face&        theFace1, 
					const TopoDS_Face&        theFace2,
					const Standard_Real       theOtherParameter,
					const Standard_Boolean    bIncreasePar,
					Standard_Real&            theNewParameter) {
  Standard_Boolean bIsComputed = Standard_False;
  theNewParameter = theParameter;

  IntTools_Context aContext;
  Standard_Real acurpar = theParameter;
  TopAbs_State aState = TopAbs_ON;
  Standard_Integer iter = 0;
  Standard_Real asumtol = BRep_Tool::Tolerance(theFace1) + BRep_Tool::Tolerance(theFace2);
  Standard_Real adelta = asumtol * 0.1;
  adelta = (adelta < Precision::Confusion()) ? Precision::Confusion() : adelta;
  Handle(Geom_Surface) aSurf1 = BRep_Tool::Surface(theFace1);
  Handle(Geom_Surface) aSurf2 = BRep_Tool::Surface(theFace2);

  Standard_Real u1, u2, v1, v2;

  GeomAPI_ProjectPointOnSurf aPrj1;
  aSurf1->Bounds(u1, u2, v1, v2);
  aPrj1.Init(aSurf1, u1, u2, v1, v2);

  GeomAPI_ProjectPointOnSurf aPrj2;
  aSurf2->Bounds(u1, u2, v1, v2);
  aPrj2.Init(aSurf2, u1, u2, v1, v2);

  while(aState == TopAbs_ON) {
    if(bIncreasePar)
      acurpar += adelta;
    else
      acurpar -= adelta;
    gp_Pnt aPCurrent = theCurve->Value(acurpar);
    aPrj1.Perform(aPCurrent);
    Standard_Real U=0., V=0.;

    if(aPrj1.IsDone()) {
      aPrj1.LowerDistanceParameters(U, V);
      aState = aContext.StatePointFace(theFace1, gp_Pnt2d(U, V));
    }

    if(aState != TopAbs_ON) {
      aPrj2.Perform(aPCurrent);
		
      if(aPrj2.IsDone()) {
	aPrj2.LowerDistanceParameters(U, V);
	aState = aContext.StatePointFace(theFace2, gp_Pnt2d(U, V));
      }
    }

    if(iter > 11) {
      break;
    }
    iter++;
  }

  if(iter <= 11) {
    theNewParameter = acurpar;
    bIsComputed = Standard_True;

    if(bIncreasePar) {
      if(acurpar >= theOtherParameter)
	theNewParameter = theOtherParameter;
    }
    else {
      if(acurpar <= theOtherParameter)
	theNewParameter = theOtherParameter;
    }
  }
  return bIsComputed;
}

Standard_Boolean IsCurveValid(Handle(Geom2d_Curve)& thePCurve)
{
  if(thePCurve.IsNull())
    return Standard_False;

  Standard_Real tolint = 1.e-10;
  Geom2dAdaptor_Curve PCA;
  IntRes2d_Domain PCD;
  Geom2dInt_GInter PCI;

  Standard_Real pf = 0., pl = 0.;
  gp_Pnt2d pntf, pntl;

  if(!thePCurve->IsClosed() && !thePCurve->IsPeriodic()) {
    pf = thePCurve->FirstParameter();
    pl = thePCurve->LastParameter();
    pntf = thePCurve->Value(pf);
    pntl = thePCurve->Value(pl);
    PCA.Load(thePCurve);
    if(!PCA.IsPeriodic()) {
      if(PCA.FirstParameter() > pf) pf = PCA.FirstParameter();
      if(PCA.LastParameter()  < pl) pl = PCA.LastParameter();
    }
    PCD.SetValues(pntf,pf,tolint,pntl,pl,tolint);
    PCI.Perform(PCA,PCD,tolint,tolint);
    if(PCI.IsDone())
      if(PCI.NbPoints() > 0) {
        return Standard_False;
      }
  }

  return Standard_True;
}

//=======================================================================
//static function : ApproxWithPCurves
//purpose  : for bug 20964 only
//=======================================================================

Standard_Boolean ApproxWithPCurves(const gp_Cylinder& theCyl, 
				   const gp_Sphere& theSph)
{
  Standard_Boolean bRes = Standard_True;
  Standard_Real R1 = theCyl.Radius(), R2 = theSph.Radius();

  if(R1 < 2.*R2) return bRes;

  gp_Lin anCylAx(theCyl.Axis());

  Standard_Real aDist = anCylAx.Distance(theSph.Location());
  Standard_Real aDRel = Abs(aDist - R1)/R2;

  if(aDRel > .2) return bRes;

  Standard_Real par = ElCLib::Parameter(anCylAx, theSph.Location());
  gp_Pnt aP = ElCLib::Value(par, anCylAx);
  gp_Vec aV(aP, theSph.Location());

  Standard_Real dd = aV.Dot(theSph.Position().XDirection());

  if(aDist < R1 && dd > 0.) return Standard_False;
  if(aDist > R1 && dd < 0.) return Standard_False;

  
  return bRes;
}

//=======================================================================
//function : PerformPlanes
//purpose  : 
//=======================================================================

void  PerformPlanes(const Handle(GeomAdaptor_HSurface)& theS1, 
		    const Handle(GeomAdaptor_HSurface)& theS2, 
		    const Standard_Real TolAng, 
		    const Standard_Real TolTang, 
		    const Standard_Boolean theApprox1,
		    const Standard_Boolean theApprox2,
		    IntTools_SequenceOfCurves& theSeqOfCurve, 
		    Standard_Boolean& theTangentFaces)
{

  gp_Pln aPln1 = theS1->Surface().Plane();
  gp_Pln aPln2 = theS2->Surface().Plane();

  IntAna_QuadQuadGeo aPlnInter(aPln1, aPln2, TolAng, TolTang);

  if(!aPlnInter.IsDone()) {
    theTangentFaces = Standard_False;
    return;
  }

  IntAna_ResultType aResType = aPlnInter.TypeInter();

  if(aResType == IntAna_Same) {
    theTangentFaces = Standard_True;
    return;
  }

  theTangentFaces = Standard_False;

  if(aResType == IntAna_Empty) {
    return;
  }

  gp_Lin aLin = aPlnInter.Line(1);

  ProjLib_Plane aProj;

  aProj.Init(aPln1);
  aProj.Project(aLin);
  gp_Lin2d aLin2d1 = aProj.Line();
  //
  aProj.Init(aPln2);
  aProj.Project(aLin);
  gp_Lin2d aLin2d2 = aProj.Line();
  //
  //classify line2d1 relatively first plane
  Standard_Real P11, P12;
  Standard_Boolean IsCrossed = ClassifyLin2d(theS1, aLin2d1, TolTang, P11, P12);
  if(!IsCrossed) return;
  //classify line2d2 relatively second plane
  Standard_Real P21, P22;
  IsCrossed = ClassifyLin2d(theS2, aLin2d2, TolTang, P21, P22);
  if(!IsCrossed) return;

  //Analysis of parametric intervals: must have common part

  if(P21 >= P12) return;
  if(P22 <= P11) return;

  Standard_Real pmin, pmax;
  pmin = Max(P11, P21);
  pmax = Min(P12, P22);

  if(pmax - pmin <= TolTang) return;

  Handle(Geom_Line) aGLin = new Geom_Line(aLin);

  IntTools_Curve aCurve;
  Handle(Geom_TrimmedCurve) aGTLin = new Geom_TrimmedCurve(aGLin, pmin, pmax);

  aCurve.SetCurve(aGTLin);

  if(theApprox1) { 
    Handle(Geom2d_Line) C2d = new Geom2d_Line(aLin2d1);
    aCurve.SetFirstCurve2d(new Geom2d_TrimmedCurve(C2d, pmin, pmax));
  }
  else { 
    Handle(Geom2d_Curve) H1;
    aCurve.SetFirstCurve2d(H1);
  }
  if(theApprox2) { 
    Handle(Geom2d_Line) C2d = new Geom2d_Line(aLin2d2);
    aCurve.SetSecondCurve2d(new Geom2d_TrimmedCurve(C2d, pmin, pmax));
  }
  else { 
    Handle(Geom2d_Curve) H1;
    aCurve.SetFirstCurve2d(H1);
  }

  theSeqOfCurve.Append(aCurve);
 
}

//=======================================================================
//function : ClassifyLin2d
//purpose  : 
//=======================================================================
static inline Standard_Boolean INTER(const Standard_Real d1, 
				     const Standard_Real d2, 
				     const Standard_Real tol) 
{
  return (d1 > tol && d2 < -tol) || 
         (d1 < -tol && d2 > tol) ||
         ((d1 <= tol && d1 >= -tol) && (d2 > tol || d2 < -tol)) || 
         ((d2 <= tol && d2 >= -tol) && (d1 > tol || d1 < -tol));
}
static inline Standard_Boolean COINC(const Standard_Real d1, 
				     const Standard_Real d2, 
				     const Standard_Real tol) 
{
  return (d1 <= tol && d1 >= -tol) && (d2 <= tol && d2 >= -tol);
}
Standard_Boolean ClassifyLin2d(const Handle(GeomAdaptor_HSurface)& theS, 
			       const gp_Lin2d& theLin2d, 
			       const Standard_Real theTol,
			       Standard_Real& theP1, 
			       Standard_Real& theP2)

{
  Standard_Real xmin, xmax, ymin, ymax, d1, d2, A, B, C;
  Standard_Real par[2];
  Standard_Integer nbi = 0;

  xmin = theS->Surface().FirstUParameter();
  xmax = theS->Surface().LastUParameter();
  ymin = theS->Surface().FirstVParameter();
  ymax = theS->Surface().LastVParameter();

  theLin2d.Coefficients(A, B, C);

  //xmin, ymin <-> xmin, ymax
  d1 = A*xmin + B*ymin + C;
  d2 = A*xmin + B*ymax + C;

  if(INTER(d1, d2, theTol)) {
    //Intersection with boundary
    Standard_Real y = -(C + A*xmin)/B;
    par[nbi] = ElCLib::Parameter(theLin2d, gp_Pnt2d(xmin, y));
    nbi++;
  }
  else if (COINC(d1, d2, theTol)) {
    //Coincidence with boundary
    par[0] = ElCLib::Parameter(theLin2d, gp_Pnt2d(xmin, ymin));
    par[1] = ElCLib::Parameter(theLin2d, gp_Pnt2d(xmin, ymax));
    nbi = 2;
  }

  if(nbi == 2) {

    if(fabs(par[0]-par[1]) > theTol) {
      theP1 = Min(par[0], par[1]);
      theP2 = Max(par[0], par[1]);
      return Standard_True;
    }
    else return Standard_False;

  }

  //xmin, ymax <-> xmax, ymax
  d1 = d2;
  d2 = A*xmax + B*ymax + C;

  if(d1 > theTol || d1 < -theTol) {//to avoid checking of
                                   //coincidence with the same point
    if(INTER(d1, d2, theTol)) {
      Standard_Real x = -(C + B*ymax)/A;
      par[nbi] = ElCLib::Parameter(theLin2d, gp_Pnt2d(x, ymax));
      nbi++;
    }
    else if (COINC(d1, d2, theTol)) {
      par[0] = ElCLib::Parameter(theLin2d, gp_Pnt2d(xmin, ymax));
      par[1] = ElCLib::Parameter(theLin2d, gp_Pnt2d(xmax, ymax));
      nbi = 2;
    }
  }

  if(nbi == 2) {

    if(fabs(par[0]-par[1]) > theTol) {
      theP1 = Min(par[0], par[1]);
      theP2 = Max(par[0], par[1]);
      return Standard_True;
    }
    else return Standard_False;

  }

  //xmax, ymax <-> xmax, ymin
  d1 = d2;
  d2 = A*xmax + B*ymin + C;

  if(d1 > theTol || d1 < -theTol) {
    if(INTER(d1, d2, theTol)) {
      Standard_Real y = -(C + A*xmax)/B;
      par[nbi] = ElCLib::Parameter(theLin2d, gp_Pnt2d(xmax, y));
      nbi++;
    }
    else if (COINC(d1, d2, theTol)) {
      par[0] = ElCLib::Parameter(theLin2d, gp_Pnt2d(xmax, ymax));
      par[1] = ElCLib::Parameter(theLin2d, gp_Pnt2d(xmax, ymin));
      nbi = 2;
    }
  }

  if(nbi == 2) {
    if(fabs(par[0]-par[1]) > theTol) {
      theP1 = Min(par[0], par[1]);
      theP2 = Max(par[0], par[1]);
      return Standard_True;
    }
    else return Standard_False;
  }

  //xmax, ymin <-> xmin, ymin 
  d1 = d2;
  d2 = A*xmin + B*ymin + C;

  if(d1 > theTol || d1 < -theTol) {
    if(INTER(d1, d2, theTol)) {
      Standard_Real x = -(C + B*ymin)/A;
      par[nbi] = ElCLib::Parameter(theLin2d, gp_Pnt2d(x, ymin));
      nbi++;
    }
    else if (COINC(d1, d2, theTol)) {
      par[0] = ElCLib::Parameter(theLin2d, gp_Pnt2d(xmax, ymin));
      par[1] = ElCLib::Parameter(theLin2d, gp_Pnt2d(xmin, ymin));
      nbi = 2;
    }
  }

  if(nbi == 2) {
    if(fabs(par[0]-par[1]) > theTol) {
      theP1 = Min(par[0], par[1]);
      theP2 = Max(par[0], par[1]);
      return Standard_True;
    }
    else return Standard_False;
  }

  return Standard_False;

}
//
//modified by NIZNHY-PKV Thu Sep 23 08:37:32 2010f
//=======================================================================
//function : ApproxParameters
//purpose  : 
//=======================================================================
void ApproxParameters(const Handle(GeomAdaptor_HSurface)& aHS1,
		      const Handle(GeomAdaptor_HSurface)& aHS2,
		      Standard_Integer& iDegMin,
		      Standard_Integer& iDegMax,
		      Standard_Integer& iNbIterMax,
		      Standard_Boolean& anWithPC
		      )
{
  GeomAbs_SurfaceType aTS1, aTS2;
  //
  iDegMin=4;
  iDegMax=8;
  iNbIterMax=0;
  //
  aTS1=aHS1->Surface().GetType();
  aTS2=aHS2->Surface().GetType();
  //
  // Cylinder/Torus
  if ((aTS1==GeomAbs_Cylinder && aTS2==GeomAbs_Torus) ||
      (aTS2==GeomAbs_Cylinder && aTS1==GeomAbs_Torus)) {
    Standard_Real aRC, aRT, dR, aPC, aDPC, aScPr;
    gp_Cylinder aCylinder;
    gp_Torus aTorus;
    //
    aPC=Precision::Confusion();
    //
    aCylinder=(aTS1==GeomAbs_Cylinder)? 
      aHS1->Surface().Cylinder() : 
      aHS2->Surface().Cylinder();
    aTorus=(aTS1==GeomAbs_Torus)? 
      aHS1->Surface().Torus() : 
      aHS2->Surface().Torus();
    //
    aRC=aCylinder.Radius();
    aRT=aTorus.MinorRadius();
    dR=aRC-aRT;
    if (dR<0.) {
      dR=-dR;
    }
    //
    if (dR<aPC) {
      const gp_Ax3& aPosT=aTorus.Position();
      const gp_Dir& aDirT=aPosT.Direction();
      //      
      const gp_Ax1& aAxisC=aCylinder.Axis();
      const gp_Dir& aDirC=aAxisC.Direction();
      //
      aScPr=aDirT*aDirC;
      if (aScPr<0.){
	aScPr=-aScPr;
      }
      //
      if (aScPr<aPC) {
	gp_Pln aPlnT(aPosT);
	const gp_Pnt& aLocC=aCylinder.Location();
	
	aDPC=aPlnT.SquareDistance(aLocC);
	if (aDPC<2.5e-9) {
	  iDegMax=6;
	  anWithPC=Standard_False;
	  iNbIterMax=10;
	}
      }
    }
  }
}
//modified by NIZNHY-PKV Thu Sep 23 08:37:39 2010t
