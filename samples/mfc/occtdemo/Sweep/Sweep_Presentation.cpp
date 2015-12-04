// Sweep_Presentation.cpp: implementation of the Sweep_Presentation class.
// Sweeping capabilities presentation
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Sweep_Presentation.h"

#include <Geom_BezierCurve.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <GeomFill_Pipe.hxx>
#include <Geom_Surface.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <Approx_CurveOnSurface.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepOffsetAPI_MakePipe.hxx>
#include <BRepOffsetAPI_MakePipeShell.hxx>
#include <Law_Interpol.hxx>
#include <BRepBuilderAPI_TransitionMode.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <Geom_BSplineCurve.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>


// Initialization of global variable with an instance of this class
OCCDemo_Presentation* OCCDemo_Presentation::Current = new Sweep_Presentation;

// Initialization of array of samples
const Sweep_Presentation::PSampleFuncType Sweep_Presentation::SampleFuncs[] =
{
  &Sweep_Presentation::sample1,
  &Sweep_Presentation::sample2,
  //&Sweep_Presentation::sample3,
  &Sweep_Presentation::sample4,
  &Sweep_Presentation::sample5,
};

#ifdef WNT
 #define EOL "\r\n"
#else
 #define EOL "\n"
#endif

#define PathColor   Quantity_Color(Quantity_NOC_CYAN1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Sweep_Presentation::Sweep_Presentation()
{
  myNbSamples = sizeof(SampleFuncs)/sizeof(PSampleFuncType);
  setName ("Sweep");
}

//////////////////////////////////////////////////////////////////////
// Sample execution
//////////////////////////////////////////////////////////////////////

void Sweep_Presentation::DoSample()
{
  getAISContext()->EraseAll();
  if (myIndex >=0 && myIndex < myNbSamples)
    (this->*SampleFuncs[myIndex])();
}

//////////////////////////////////////////////////////////////////////
// Auxiliary functions
//////////////////////////////////////////////////////////////////////

static Handle(Geom_Curve) mkBezierCurve(const Standard_Integer nPoles,
                                 const Standard_Real theCoords[][3],
                                 const Standard_Real aScale = 1,
                                 const gp_XYZ& aShift = gp_XYZ(0,0,0))
{
  TColgp_Array1OfPnt aPoles (1, nPoles);
  for (Standard_Integer i=0; i < nPoles; i++)
  {
    gp_XYZ aP (theCoords[i][0], theCoords[i][1], theCoords[i][2]);
    aPoles(i+1) = gp_Pnt (aP * aScale + aShift);
  }
  return new Geom_BezierCurve (aPoles);
}

static Handle(Geom_Curve) mkPBSplineCurve(const Standard_Integer nPoles,
                                 const Standard_Real theCoords[][3],
                                 const Standard_Real aScale = 1,
                                 const gp_XYZ& aShift = gp_XYZ(0,0,0))
{
  TColgp_Array1OfPnt aPoles (1, nPoles);
  TColStd_Array1OfReal aKnots (1, nPoles+1);
  TColStd_Array1OfInteger aMults(1, nPoles+1);

  for (Standard_Integer i=0; i < nPoles; i++)
  {
    gp_XYZ aP (theCoords[i][0], theCoords[i][1], theCoords[i][2]);
    aPoles(i+1) = gp_Pnt (aP * aScale + aShift);
  }
  for (i=1; i <= nPoles+1; i++)
    aKnots(i) = Standard_Real(i-1);
  aMults.Init(1);

  return new Geom_BSplineCurve (aPoles, aKnots, aMults, 3, Standard_True);
}

static Handle(Geom2d_Curve) mk2dBezierCurve(const Standard_Integer nPoles,
                                     const Standard_Real theCoords[][2],
                                     const Standard_Real aScale = 1,
                                     const gp_XY& aShift = gp_XY(0,0))
{
  TColgp_Array1OfPnt2d aPoles (1, nPoles);
  for (Standard_Integer i=0; i < nPoles; i++)
  {
    gp_XY aP (theCoords[i][0], theCoords[i][1]);
    aPoles(i+1) = gp_Pnt2d (aP * aScale + aShift);
  }
  return new Geom2d_BezierCurve (aPoles);
}

static TopoDS_Wire mkPolygonWire(const Standard_Integer nPoints,
                                 const Standard_Real theCoords[][3],
                                 const Standard_Real aScale = 1,
                                 const gp_XYZ& aShift = gp_XYZ(0,0,0))
{
  BRepBuilderAPI_MakePolygon aPol;
  for (Standard_Integer i=0; i < nPoints; i++)
  {
    gp_XYZ aP(theCoords[i][0], theCoords[i][1], theCoords[i][2]);
    aPol.Add (gp_Pnt (aP * aScale + aShift));
  }
  return aPol.Wire();
}

static Handle(Geom_Curve) mkCurve1()
{
  Standard_Real aCoords[][3] = {
    {0,0,0},{0,0,10},{0,10,10},{0,10,20}
  };
  Standard_Integer nPoles = sizeof(aCoords)/(sizeof(Standard_Real)*3);
  return mkBezierCurve (nPoles, aCoords);
}

static Handle(Geom_Curve) mkCurve2()
{
  Standard_Real aCoords[][3] = {
    {0,0,0},{10,0,0},{20,10,12},{25,30,20},{50,40,50}
  };
  Standard_Integer nPoles = sizeof(aCoords)/(sizeof(Standard_Real)*3);
  return mkBezierCurve (nPoles, aCoords);
}

static Handle(Geom_Curve) mkCurve3()
{
  Standard_Real aCoords[][3] = {
    {50,40,50},{70,30,30},{90,20,20},{100,50,0}
  };
  Standard_Integer nPoles = sizeof(aCoords)/(sizeof(Standard_Real)*3);
  return mkBezierCurve (nPoles, aCoords);
}

static Handle(Geom2d_Curve) mk2dCurve1()
{
  Standard_Real aCoords[][2] = {
    {0,0},{0.3,0},{0.1,0.5},{0.3,0.6},{0.6,0.5}
  };
  Standard_Integer nPoles = sizeof(aCoords)/(sizeof(Standard_Real)*2);
  return mk2dBezierCurve (nPoles, aCoords);
}

static Handle(Geom_Surface) mkSurface1()
{
  Standard_Real aCoords[3][3][3] = {
    {{-10,-10,-30},{-10,50,40},{-10,70,-20}},
    {{50,-10,40},{50,50,0},{50,70,30}},
    {{70,-10,20},{70,50,30},{70,70,-20}}
  };
  TColgp_Array2OfPnt aPoles (1, 3, 1, 3);
  for (Standard_Integer iV=0; iV < 3; iV++)
    for (Standard_Integer iU=0; iU < 3; iU++)
      aPoles(iU+1, iV+1) = gp_Pnt (aCoords[iV][iU][0],
                                   aCoords[iV][iU][1],
                                   aCoords[iV][iU][2]);
  return new Geom_BezierSurface (aPoles);
}

static TopoDS_Edge mkEdge1()
{
  BRepBuilderAPI_MakeEdge aMkEdge (mkCurve1());
  return aMkEdge.Edge();
}

static TopoDS_Wire mkWire1()
{
  BRepBuilderAPI_MakeEdge aMkEdge1 (mkCurve2());
  BRepBuilderAPI_MakeEdge aMkEdge2 (mkCurve3());
  BRepBuilderAPI_MakeWire aMkWire (aMkEdge1, aMkEdge2);
  return aMkWire.Wire();
}

static TopoDS_Wire mkWire2()
{
  Standard_Real aCoords[][3] = {
    {-20,0,0},{20,0,0},{20,10,12},{25,30,20},{50,40,50},
  };
  Standard_Integer nPoles = sizeof(aCoords)/(sizeof(Standard_Real)*3);
  Handle(Geom_Curve) aCurve = mkBezierCurve (nPoles, aCoords);
  BRepBuilderAPI_MakeEdge aMkEdge1 (aCurve);
  BRepBuilderAPI_MakeEdge aMkEdge2 (mkCurve3());
  BRepBuilderAPI_MakeWire aMkWire (aMkEdge1, aMkEdge2);
  return aMkWire.Wire();
}

static TopoDS_Wire mkWire3()
{
  BRepBuilderAPI_MakeEdge aMkEdge1 (mkCurve1());
  Standard_Real aCoords[][3] = {
    {0,10,20},{0,20,10},{0,20,0},{0,0,0}
  };
  Standard_Integer nPoles = sizeof(aCoords)/(sizeof(Standard_Real)*3);
  Handle(Geom_Curve) aCurve = mkBezierCurve (nPoles, aCoords);
  BRepBuilderAPI_MakeEdge aMkEdge2 (aCurve);
  BRepBuilderAPI_MakeWire aMkWire (aMkEdge1, aMkEdge2);
  return aMkWire.Wire();
}

static TopoDS_Wire mkWire4()
{
  Standard_Real aCoords[][3] = {
    {0,-3,0},{0,-6,6},{0,0,3},{0,6,6},{0,3,0},{0,6,-6},{0,0,-3},{0,-6,-6}
  };
  Standard_Integer nPoles = sizeof(aCoords)/(sizeof(Standard_Real)*3);
  Handle(Geom_Curve) aCurve = mkPBSplineCurve (nPoles, aCoords);
  BRepBuilderAPI_MakeEdge aMkEdge (aCurve);
  BRepBuilderAPI_MakeWire aMkWire (aMkEdge);
  return aMkWire.Wire();
}

static TopoDS_Wire mkWire5()
{
  Standard_Real aCoords[][3] = {
    {0,0,0},{10,0,0},{10,0,10},{10,10,10},{0,10,10},{0,10,0},{0,0,0}
  };
  Standard_Integer nPoints = sizeof(aCoords)/(sizeof(Standard_Real)*3);
  return mkPolygonWire(nPoints, aCoords);
}

static TopoDS_Wire mkWire6()
{
  Standard_Real aCoords[][3] = {
    {0,-2,-2},{0,2,-2},{0,2,2},{0,-2,2},{0,-2,-2}
  };
  Standard_Integer nPoints = sizeof(aCoords)/(sizeof(Standard_Real)*3);
  return mkPolygonWire(nPoints, aCoords);
}

static TopoDS_Wire mkWire7()
{
  Standard_Real aCoords[][3] = {
    {0,0,0},{10,0,0},{10,10,0}
  };
  Standard_Integer nPoints = sizeof(aCoords)/(sizeof(Standard_Real)*3);
  return mkPolygonWire(nPoints, aCoords);
}

static TopoDS_Wire mkWire8()
{
  Standard_Real aCoords[][3] = {
    {0,-10,0},{20,-10,10},{20,10,0}
  };
  Standard_Integer nPoints = sizeof(aCoords)/(sizeof(Standard_Real)*3);
  return mkPolygonWire(nPoints, aCoords);
}

static TopoDS_Wire mkWire9()
{
  Standard_Real aCoords[][3] = {
    {0,-1,0},{0,-2,2},{0,0,1},{0,2,2},{0,1,0},{0,2,-2},{0,0,-1},{0,-2,-2}
  };
  Standard_Integer nPoles = sizeof(aCoords)/(sizeof(Standard_Real)*3);
  Handle(Geom_Curve) aCurve = mkPBSplineCurve (nPoles, aCoords);
  BRepBuilderAPI_MakeEdge aMkEdge (aCurve);
  BRepBuilderAPI_MakeWire aMkWire (aMkEdge);
  return aMkWire.Wire();
}


Handle(AIS_InteractiveObject) Sweep_Presentation::drawCurveOnSurface 
    (const Handle(Geom2d_Curve)& aC2d,
     const Handle(Geom_Surface)& aSurf,
     const Quantity_Color& aColor)
{
  Handle(Geom2dAdaptor_HCurve) aHC = new Geom2dAdaptor_HCurve(aC2d);
  Handle(GeomAdaptor_HSurface) aHS = new GeomAdaptor_HSurface(aSurf);

  Standard_Real aFirst = Min (aC2d->FirstParameter(), 500.);
  Standard_Real aLast = Min (aC2d->LastParameter(), 500.);
  Standard_Real aTol = 1.e-4;
  Standard_Integer aMaxDegree = 8;
  Standard_Integer aMaxSegments = 30;
  Approx_CurveOnSurface aApprox(aHC, aHS, aFirst, aLast, aTol,
    GeomAbs_C2, aMaxDegree, aMaxSegments, Standard_True);
  if (aApprox.IsDone() && aApprox.HasResult())
  {
    Handle(Geom_Curve) aCurve = aApprox.Curve3d();
    return drawCurve (aCurve, aColor);
  }
  else
    return Handle(AIS_InteractiveObject)();
}

//////////////////////////////////////////////////////////////////////
// Sample functions
//////////////////////////////////////////////////////////////////////

void Sweep_Presentation::sample1()
{
  ResetView();
  SetViewCenter(35.024312615272, 4.0878339858469);
  SetViewScale(7.8145774983540); 

  setResultTitle ("Sweeping Curve along Path");
  // Creation of a pipe by sweeping a curve (the section)
  // along another curve (the path)

  // Create the section and the path
  Handle(Geom_Curve) aSection = mkCurve1();
  Handle(Geom_Curve) aPath = mkCurve2();

  // Creation of the swept surface
  Standard_Real aTol=1.e-4;
  GeomFill_Pipe aPipe;

  // Case 1: Tangent and Normal are fixed
  GeomFill_Trihedron aOption = GeomFill_IsFixed;
  aPipe.Init (aPath, aSection, aOption);
  aPipe.Perform (aTol, Standard_False);
  Handle(Geom_Surface) aSurface1 = aPipe.Surface();

  // Case 2: Tangent and Normal are given by Frenet frame
  aOption = GeomFill_IsFrenet;
  aPipe.Init (aPath, aSection, aOption);
  aPipe.Perform (aTol, Standard_False);
  Handle(Geom_Surface) aSurface2 = aPipe.Surface();

  // Case 3: Tangent and Normal are given by Corrected Frenet frame
  aOption = GeomFill_IsCorrectedFrenet;
  aPipe.Init (aPath, aSection, aOption);
  aPipe.Perform (aTol, Standard_False);
  Handle(Geom_Surface) aSurface3 = aPipe.Surface();

  // Case 4: constant Normal to have all the sections in a same plane
  aOption = GeomFill_IsConstantNormal;
  aPipe.Init (aPath, aSection, aOption);
  aPipe.Perform (aTol, Standard_False);
  Handle(Geom_Surface) aSurface4 = aPipe.Surface();

  // Case 5: Define the path by a surface and a 2dcurve,
  // the surface is used to define the trihedron's normal.
  Handle(Geom2d_Curve) a2dPath5 = mk2dCurve1();
  Handle(Geom_Surface) aSupport5 = mkSurface1();
  aPipe.Init (a2dPath5, aSupport5, aSection);
  aPipe.Perform (aTol, Standard_False);
  Handle(Geom_Surface) aSurface5 = aPipe.Surface();

  // Show source code
  TCollection_AsciiString aText;
  aText = aText +
    "  // Sweeping a curve (the section) along a curve (the path)" EOL EOL

    "  // Create the section and the path" EOL
    "  Handle(Geom_Curve) aSection = mkCurve1();" EOL
    "  Handle(Geom_Curve) aPath = mkCurve2();" EOL EOL

    "  // Create swept surface" EOL
    "  Standard_Real aTol=1.e-4;" EOL
    "  GeomFill_Pipe aPipe;" EOL EOL

    "  // Case 1: Tangent and Normal are fixed" EOL
    "  GeomFill_Trihedron aOption = GeomFill_IsFixed;" EOL
    "  aPipe.Init (aPath, aSection, aOption);" EOL
    "  aPipe.Perform (aTol, Standard_False);" EOL
    "  Handle(Geom_Surface) aSurface1 = aPipe.Surface();" EOL EOL

    "  // Case 2: Tangent and Normal are given by Frenet frame" EOL
    "  aOption = GeomFill_IsFrenet;" EOL
    "  aPipe.Init (aPath, aSection, aOption);" EOL
    "  aPipe.Perform (aTol, Standard_False);" EOL
    "  Handle(Geom_Surface) aSurface2 = aPipe.Surface();" EOL EOL

    "  // Case 3: Tangent and Normal are given by Corrected Frenet frame" EOL
    "  aOption = GeomFill_IsCorrectedFrenet;" EOL
    "  aPipe.Init (aPath, aSection, aOption);" EOL
    "  aPipe.Perform (aTol, Standard_False);" EOL
    "  Handle(Geom_Surface) aSurface3 = aPipe.Surface();" EOL EOL

    "  // Case 4: constant Normal to have all the sections in a same plane" EOL
    "  aOption = GeomFill_IsConstantNormal;" EOL
    "  aPipe.Init (aPath, aSection, aOption);" EOL
    "  aPipe.Perform (aTol, Standard_False);" EOL
    "  Handle(Geom_Surface) aSurface4 = aPipe.Surface();" EOL EOL

    "  // Case 5: Define the path by a surface and a 2dcurve," EOL
    "  // the surface is used to define the trihedron's normal." EOL
    "  Handle(Geom2d_Curve) a2dPath5 = mk2dCurve1();" EOL
    "  Handle(Geom_Surface) aSupport5 = mkSurface1();" EOL
    "  aPipe.Init (a2dPath5, aSupport5, aSection);" EOL
    "  aPipe.Perform (aTol, Standard_False);" EOL
    "  Handle(Geom_Surface) aSurface5 = aPipe.Surface();" EOL;
  setResultText (aText.ToCString());

  // Draw objects
  Handle(AIS_InteractiveObject) aICurv1, aICurv2, aISurf, aISupp;

  aICurv1 = drawCurve (aSection);
  if (WAIT_A_LITTLE) return;
  aICurv2 = drawCurve (aPath, PathColor);
  if (WAIT_A_LITTLE) return;
  aISurf = drawSurface (aSurface1);
  if (WAIT_A_SECOND) return;

  getAISContext()->Erase (aISurf);
  if (WAIT_A_LITTLE) return;
  aISurf = drawSurface (aSurface2);
  if (WAIT_A_SECOND) return;

  getAISContext()->Erase (aISurf);
  if (WAIT_A_LITTLE) return;
  aISurf = drawSurface (aSurface3);
  if (WAIT_A_SECOND) return;

  getAISContext()->Erase (aISurf);
  if (WAIT_A_LITTLE) return;
  aISurf = drawSurface (aSurface4);
  if (WAIT_A_SECOND) return;

  getAISContext()->Erase(aICurv2);
  getAISContext()->Erase(aISurf);
  if (WAIT_A_LITTLE) return;

  aISupp = drawSurface (aSupport5, Quantity_NOC_BLUE1);
  aICurv2 = drawCurveOnSurface (a2dPath5, aSupport5, PathColor);
  if (WAIT_A_LITTLE) return;
  aISurf = drawSurface (aSurface5);
}

void Sweep_Presentation::sample2()
{
  ResetView();
  SetViewCenter(51.059243819711, 16.906093324128);
  SetViewScale(8.5662184369261); 

  setResultTitle ("Sweeping Shape along Wire");

  // Show source code
  TCollection_AsciiString aText;
  aText = aText +
    "  // Creation of a pipe by sweeping a shape (the profile)" EOL
    "  // along a wire (the spine)" EOL EOL

    "  TopoDS_Wire aSpine;" EOL
    "  TopoDS_Shape aProfile, aResult;" EOL
    "  aSpine = mkWire1();" EOL EOL

    "  // Case 1: unclosed edge profile" EOL
    "  aProfile = mkEdge1();" EOL
    "  aResult = BRepOffsetAPI_MakePipe (aSpine, aProfile);" EOL EOL;
/*
    "  // Case 2: closed wire profile" EOL
    "  aProfile = mkWire4();" EOL
    "  aResult = BRepOffsetAPI_MakePipe (aSpine, aProfile);" EOL;*/
  setResultText (aText.ToCString());

  // Creation of a pipe by sweeping a shape (the profile)
  // along a wire (the spine)

  TopoDS_Wire aSpine;
  TopoDS_Shape aProfile, aResult;
  aSpine = mkWire1();

  // Case 1: unclosed edge profile
  aProfile = mkEdge1();
  aResult = BRepOffsetAPI_MakePipe (aSpine, aProfile);

  // Draw objects
  Handle(AIS_InteractiveObject) aISpine, aIProf, aIRes;
  aISpine = drawShape (aSpine, PathColor);
  if (WAIT_A_LITTLE) return;
  aIProf = drawShape (aProfile, Quantity_NOC_RED);
  if (WAIT_A_LITTLE) return;
  aIRes = drawShape (aResult);
  if (WAIT_A_SECOND) return;

  // Case 2: closed wire profile
  /*
  aProfile = mkWire4();
  aResult = BRepOffsetAPI_MakePipe (aSpine, aProfile);

  getAISContext()->Erase (aIProf);
  getAISContext()->Erase (aIRes);
  if (WAIT_A_LITTLE) return;
  aIProf = drawShape (aProfile, Quantity_NOC_RED);
  if (WAIT_A_LITTLE) return;
  aIRes = drawShape (aResult);
  if (WAIT_A_SECOND) return;
  */
}

void Sweep_Presentation::sample3()
{
  setResultTitle ("Sweeping Wire along Wire");

  // Show source code
  TCollection_AsciiString aText;
  aText = aText +
    "  // Creation of a shell-like pipe by sweeping a wire (the profile)" EOL
    "  // along a wire (the spine)" EOL
    "  // CorrectedFrenet mode + interpolate sections" EOL EOL

    "  // define a spine" EOL
    "  TopoDS_Wire aSpine = mkWire2();" EOL
    "  BRepOffsetAPI_MakePipeShell aSweep(aSpine);" EOL
    "  // set a CorrectedFrenet trihedron mode" EOL
    "  aSweep.SetMode(Standard_False);" EOL
    "  // define a profile and an interpolation law" EOL
    "  TopoDS_Wire aProfile = mkWire3();" EOL
    "  TColgp_Array1OfPnt2d aParAndRad(1, 3);" EOL
    "  aParAndRad(1) = gp_Pnt2d (0,1);" EOL
    "  aParAndRad(2) = gp_Pnt2d (1,2);" EOL
    "  aParAndRad(3) = gp_Pnt2d (2,0.5);" EOL
    "  Handle(Law_Interpol) aLaw = new Law_Interpol();" EOL
    "  aLaw->Set (aParAndRad, Standard_False);  // set non-periodic law" EOL
    "  aSweep.SetLaw (aProfile, aLaw, " EOL
    "                  Standard_True,    // profile have to be translated" EOL
    "                                    //  to be in contact with the spine" EOL
    "                  Standard_True);   // profile have to be rotated " EOL
    "                                    //  to be orthogonal to the spine's tangent" EOL
    "  // define transition mode to manage discontinuities on the sweep" EOL
    "  BRepBuilderAPI_TransitionMode aTransition = BRepBuilderAPI_RoundCorner;" EOL
    "  aSweep.SetTransitionMode (aTransition);" EOL EOL

    "  // perform sweeping and get a result" EOL
    "  aSweep.Build();" EOL
    "  TopoDS_Shape aResult;" EOL
    "  if (aSweep.IsDone())" EOL
    "    aResult = aSweep.Shape();" EOL;
  setResultText (aText.ToCString());

  // Creation of a shell-like pipe by sweeping a wire (the profile)
  // along a wire (the spine)
  // CorrectedFrenet mode + interpolate sections

  // define a spine
  TopoDS_Wire aSpine = mkWire2();
  BRepOffsetAPI_MakePipeShell aSweep(aSpine);
  // set a CorrectedFrenet trihedron mode
  aSweep.SetMode(Standard_False);
  // define a profile and an interpolation law
  TopoDS_Wire aProfile = mkWire3();
  TColgp_Array1OfPnt2d aParAndRad(1, 3);
  aParAndRad(1) = gp_Pnt2d (0,1);
  aParAndRad(2) = gp_Pnt2d (1,2);
  aParAndRad(3) = gp_Pnt2d (2,0.5);
  Handle(Law_Interpol) aLaw = new Law_Interpol();
  aLaw->Set (aParAndRad, Standard_False);  // set non-periodic law
  aSweep.SetLaw (aProfile, aLaw, 
                  Standard_True,    // profile have to be translated
                                    //  to be in contact with the spine
                  Standard_True);   // profile have to be rotated 
                                    //  to be orthogonal to the spine's tangent
  // define transition mode to manage discontinuities on the sweep
  BRepBuilderAPI_TransitionMode aTransition = BRepBuilderAPI_RoundCorner;
  aSweep.SetTransitionMode (aTransition);

  // Draw objects
  Handle(AIS_InteractiveObject) aISpine, aIProf, aIRes;
  if (WAIT_A_LITTLE) return;
  aISpine = drawShape (aSpine, PathColor);
  if (WAIT_A_LITTLE) return;
  aIProf = drawShape (aProfile, Quantity_NOC_RED);
  if (WAIT_A_LITTLE) return;

  // perform sweeping and get a result
  aSweep.Build();
  TopoDS_Shape aResult;
  if (aSweep.IsDone())
    aResult = aSweep.Shape();

  // Draw objects
  aIRes = drawShape (aResult);
}

void Sweep_Presentation::sample4()
{
  ResetView();
  SetViewCenter(7.0710681182763, 4.0824832263318);
  SetViewScale(43.307810016829); 
 
  setResultTitle ("Sweeping Wire along Wire");

  // Show source code
  TCollection_AsciiString aText;
  aText = aText +
    "  // Creation of a shell-like pipe by sweeping a wire (the profile)" EOL
    "  // along a wire (the spine)" EOL
    "  // CorrectedFrenet mode" EOL EOL

    "  // define a spine" EOL
    "  TopoDS_Wire aSpine = mkWire5();" EOL
    "  BRepOffsetAPI_MakePipeShell aSweep(aSpine);" EOL
    "  // set a CorrectedFrenet trihedron mode" EOL
    "  aSweep.SetMode(Standard_False);" EOL
    "  // define a profile" EOL
    "  TopoDS_Wire aProfile = mkWire6();" EOL
    "  aSweep.Add (aProfile);" EOL
    "  // define transition mode to manage discontinuities on the sweep" EOL
    "  BRepBuilderAPI_TransitionMode aTransition = BRepBuilderAPI_RoundCorner;" EOL
    "  aSweep.SetTransitionMode (aTransition);" EOL EOL

    "  // perform sweeping and get a result" EOL
    "  aSweep.Build();" EOL
    "  TopoDS_Shape aResult;" EOL
    "  if (aSweep.IsDone())" EOL
    "    aResult = aSweep.Shape();" EOL;
  setResultText (aText.ToCString());

  // Creation of a shell-like pipe by sweeping a wire (the profile)
  // along a wire (the spine)
  // CorrectedFrenet mode

  // define a spine
  TopoDS_Wire aSpine = mkWire5();
  BRepOffsetAPI_MakePipeShell aSweep(aSpine);
  // set a CorrectedFrenet trihedron mode
  aSweep.SetMode(Standard_False);
  // define a profile
  TopoDS_Wire aProfile = mkWire6();
  aSweep.Add (aProfile);
  // define transition mode to manage discontinuities on the sweep
  BRepBuilderAPI_TransitionMode aTransition = BRepBuilderAPI_RoundCorner;
  aSweep.SetTransitionMode (aTransition);

  // Draw objects
  Handle(AIS_InteractiveObject) aISpine, aIProf, aIRes;
  if (WAIT_A_LITTLE) return;
  aISpine = drawShape (aSpine, PathColor);
  if (WAIT_A_LITTLE) return;
  aIProf = drawShape (aProfile, Quantity_NOC_RED);
  if (WAIT_A_LITTLE) return;

  // perform sweeping and get a result
  aSweep.Build();
  TopoDS_Shape aResult;
  if (aSweep.IsDone())
    aResult = aSweep.Shape();

  // Draw objects
  aIRes = drawShape (aResult);
}

void Sweep_Presentation::sample5()
{
  ResetView();
  SetViewCenter(7.0710673644044, -0.81673684677605);
  SetViewScale(37.893638277722); 

  setResultTitle ("Sweeping Wire along Wire");

  // Show source code
  TCollection_AsciiString aText;
  aText = aText +
    "  // Creation of a shell-like pipe by sweeping a wire (the profile)" EOL
    "  // along a wire (the spine)" EOL
    "  // Guide line mode" EOL EOL

    "  // define a spine" EOL
    "  TopoDS_Wire aSpine = mkWire7();" EOL
    "  BRepOffsetAPI_MakePipeShell aSweep(aSpine);" EOL
    "  // define a guide" EOL
    "  TopoDS_Wire aGuide = mkWire8();" EOL
    "  aSweep.SetMode(aGuide, Standard_False);" EOL
    "  // define a profile" EOL
    "  TopoDS_Wire aProfile = mkWire9();" EOL
    "  aSweep.Add (aProfile);" EOL
    "  // define transition mode to manage discontinuities on the sweep" EOL
    "  BRepBuilderAPI_TransitionMode aTransition = BRepBuilderAPI_RoundCorner;" EOL
    "  aSweep.SetTransitionMode (aTransition);" EOL EOL

    "  // perform sweeping and get a result" EOL
    "  aSweep.Build();" EOL
    "  TopoDS_Shape aResult;" EOL
    "  if (aSweep.IsDone())" EOL
    "    aResult = aSweep.Shape();" EOL;
  setResultText (aText.ToCString());

  // Creation of a shell-like pipe by sweeping a wire (the profile)
  // along a wire (the spine)
  // Guide line mode

  // define a spine
  TopoDS_Wire aSpine = mkWire7();
  BRepOffsetAPI_MakePipeShell aSweep(aSpine);
  // define a guide
  TopoDS_Wire aGuide = mkWire8();
  aSweep.SetMode(aGuide, Standard_False, Standard_False);
  // define a profile
  TopoDS_Wire aProfile = mkWire9();
  aSweep.Add (aProfile, Standard_False, Standard_False);
  // define transition mode to manage discontinuities on the sweep
  BRepBuilderAPI_TransitionMode aTransition = BRepBuilderAPI_RoundCorner;
  aSweep.SetTransitionMode (aTransition);

  // Draw objects
  Handle(AIS_InteractiveObject) aISpine, aIGuide, aIProf, aIRes;
  if (WAIT_A_LITTLE) return;
  aISpine = drawShape (aSpine, PathColor);
  if (WAIT_A_LITTLE) return;
  aIGuide = drawShape (aGuide, Quantity_NOC_GREEN);
  if (WAIT_A_LITTLE) return;
  aIProf = drawShape (aProfile, Quantity_NOC_RED);
  if (WAIT_A_LITTLE) return;

  // perform sweeping and get a result
  aSweep.Build();
  TopoDS_Shape aResult;
  if (aSweep.IsDone())
    aResult = aSweep.Shape();

  // Draw objects
  aIRes = drawShape (aResult);
}
