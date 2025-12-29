// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <HelixBRep_BuilderHelix.hxx>
#include <HelixGeom_BuilderHelix.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <NCollection_List.hxx>

//=================================================================================================

HelixBRep_BuilderHelix::HelixBRep_BuilderHelix()

{
  gp_Pnt aP0(0., 0., 0);
  // Initialize coordinate system and data arrays
  myAxis3.SetDirection(gp::DZ());
  myAxis3.SetLocation(aP0);
  myDiams.Nullify();
  myHeights.Nullify();
  myPitches.Nullify();
  myIsPitches.Nullify();
  myNParts = 1;
  myShape.Nullify();

  // Initialize approximation parameters
  myTolerance   = 0.0001;
  myContinuity  = GeomAbs_C1;
  myMaxDegree   = 8;
  myMaxSegments = 1000;
  // Initialize status variables
  myTolReached = 99.;
  // Set initial error state
  myErrorStatus   = 1;
  myWarningStatus = 1;
}

//=================================================================================================

HelixBRep_BuilderHelix::~HelixBRep_BuilderHelix() = default;

//=================================================================================================

void HelixBRep_BuilderHelix::SetParameters(const gp_Ax3&                     theAxis,
                                           const NCollection_Array1<double>& theDiams,
                                           const NCollection_Array1<double>& theHeights,
                                           const NCollection_Array1<double>& thePitches,
                                           const NCollection_Array1<bool>&   bIsPitches)
{
  myNParts = theDiams.Length() - 1;

  myAxis3 = theAxis;

  myDiams.Nullify();
  myHeights.Nullify();
  myPitches.Nullify();
  myShape.Nullify();

  myErrorStatus   = 1;
  myWarningStatus = 1;

  if (myNParts != theHeights.Length() || myNParts != thePitches.Length()
      || myNParts != bIsPitches.Length())
  {
    throw Standard_ConstructionError(
      "HelixBRep_BuilderHelix::SetParameters: wrong array dimension");
  }

  myDiams     = new NCollection_HArray1<double>(1, myNParts + 1);
  myHeights   = new NCollection_HArray1<double>(1, myNParts);
  myPitches   = new NCollection_HArray1<double>(1, myNParts);
  myIsPitches = new NCollection_HArray1<bool>(1, myNParts);

  myDiams->ChangeArray1()     = theDiams;
  myHeights->ChangeArray1()   = theHeights;
  myPitches->ChangeArray1()   = thePitches;
  myIsPitches->ChangeArray1() = bIsPitches;

  myErrorStatus   = 0;
  myWarningStatus = 0;
}

//=================================================================================================

void HelixBRep_BuilderHelix::SetParameters(const gp_Ax3&                     theAxis,
                                           const double                      theDiam,
                                           const NCollection_Array1<double>& theHeights,
                                           const NCollection_Array1<double>& thePitches,
                                           const NCollection_Array1<bool>&   bIsPitches)
{
  int                        aNbParts = theHeights.Length();
  NCollection_Array1<double> aDiams(1, aNbParts + 1);
  aDiams.Init(theDiam);
  SetParameters(theAxis, aDiams, theHeights, thePitches, bIsPitches);
}

//=================================================================================================

void HelixBRep_BuilderHelix::SetParameters(const gp_Ax3&                     theAxis,
                                           const double                      theDiam1,
                                           const double                      theDiam2,
                                           const NCollection_Array1<double>& theHeights,
                                           const NCollection_Array1<double>& thePitches,
                                           const NCollection_Array1<bool>&   bIsPitches)
{
  int                        aNbParts = theHeights.Length();
  NCollection_Array1<double> aDiams(1, aNbParts + 1);

  int    i, j;
  double anH = 0.;
  for (i = theHeights.Lower(); i <= theHeights.Upper(); ++i)
  {
    anH += theHeights(i);
  }
  double K             = (theDiam2 - theDiam1) / anH;
  aDiams(1)            = theDiam1;
  aDiams(aNbParts + 1) = theDiam2;

  anH = theHeights(1);
  for (i = theHeights.Lower() + 1, j = 2; i <= theHeights.Upper(); ++i, ++j)
  {
    aDiams(j) = theDiam1 + K * anH;
    anH += theHeights(i);
  }

  SetParameters(theAxis, aDiams, theHeights, thePitches, bIsPitches);
}

//=================================================================================================

void HelixBRep_BuilderHelix::SetApproxParameters(const double        aTolerance,
                                                 const int           aMaxDegree,
                                                 const GeomAbs_Shape aCont)
{
  myTolerance  = aTolerance;
  myMaxDegree  = aMaxDegree;
  myContinuity = aCont;
}

//=================================================================================================

double HelixBRep_BuilderHelix::ToleranceReached() const
{
  return myTolReached;
}

//=================================================================================================

void HelixBRep_BuilderHelix::Perform()
{

  if (myErrorStatus != 0)
    return;

  int i;

  myTolReached = 0.;
  myShape.Nullify();
  BRep_Builder aBB;
  // aBB.MakeCompound(TopoDS::Compound(myShape));

  NCollection_List<TopoDS_Shape> anLst;

  gp_Ax1 anAxis(myAxis3.Axis());
  gp_Pnt aPStart = myAxis3.Location();
  aPStart.Translate(.5 * myDiams->Value(1) * myAxis3.XDirection());
  bool bIsClockwise = myAxis3.Direct();

  double aHeight;
  double aPitch;
  double aTaperAngle;

  for (i = 1; i <= myNParts; ++i)
  {

    aHeight = myHeights->Value(i);

    if (myIsPitches->Value(i))
    {
      aPitch = myPitches->Value(i);
    }
    else
    {
      aPitch = aHeight / myPitches->Value(i);
    }

    aTaperAngle = std::atan(.5 * (myDiams->Value(i + 1) - myDiams->Value(i)) / aHeight);

    TopoDS_Wire aPart;
    aBB.MakeWire(aPart);

    BuildPart(anAxis, aPStart, aHeight, aPitch, aTaperAngle, bIsClockwise, aPart);
    if (myErrorStatus != 0)
      return;

    TopoDS_Vertex V1, V2;

    TopExp::Vertices(aPart, V1, V2);

    aPStart = BRep_Tool::Pnt(V2);

    anAxis.SetLocation(anAxis.Location().Translated(aHeight * anAxis.Direction()));

    anLst.Append(aPart);
    // aBB.Add(myShape, aPart);
  }

  Smoothing(anLst);

  NCollection_List<TopoDS_Shape>::Iterator anIt(anLst);
  BRepBuilderAPI_MakeWire                  aMkWire(TopoDS::Wire(anLst.First()));
  anIt.Next();
  for (; anIt.More(); anIt.Next())
  {
    aMkWire.Add(TopoDS::Wire(anIt.Value()));
  }

  myShape = aMkWire.Shape();
}

//=================================================================================================

void HelixBRep_BuilderHelix::BuildPart(const gp_Ax1& theAxis,
                                       const gp_Pnt& thePStart,
                                       const double  theHeight,
                                       const double  thePitch,
                                       const double  theTaperAngle,
                                       const bool    bIsClockwise,
                                       TopoDS_Wire&  thePart)
{

  if (myErrorStatus != 0)
    return;

  myErrorStatus   = 0;
  myWarningStatus = 0;

  // 1. check & prepare data
  double aTolPrec, aDist, aDM, aTwoPI, aC1, aT2, aT1, aT0;
  // Initialize tolerance and angular precision
  aTolPrec       = myTolerance;
  double aTolAng = 1.e-7;
  // Validate input parameters
  if (theTaperAngle > M_PI / 2. - aTolAng)
  {
    myErrorStatus = 13; // invalid TaperAngle value
    return;
  }
  if (theHeight < aTolPrec)
  {
    myErrorStatus = 12; // invalid Height value
    return;
  }
  if (thePitch < aTolPrec)
  {
    myErrorStatus = 11; // invalid Pitch value
    return;
  }
  //
  gp_Lin aLin(theAxis);
  aDist = aLin.Distance(thePStart);
  if (aDist < aTolPrec)
  {
    myErrorStatus = 10; // myPStart belongs to the myAxis
    return;
  }

  aTolAng            = aTolPrec / aDist;
  double aAngleStart = 0.;

  // const bool bIsOutWard = theTaperAngle > 0.0;
  const gp_Dir& aDir  = theAxis.Direction();
  gp_Vec        aVec1 = gp_Vec(aDir);
  gp_Pnt        aM0   = theAxis.Location();
  gp_Vec        aVec(aM0, thePStart);
  aDM        = aVec1.Dot(aVec);
  gp_Pnt aM1 = aM0.Translated(aDM * aVec1);
  gp_Vec aVecX(aM1, thePStart);
  gp_Dir aDirX(aVecX);
  gp_Ax2 aAx2(aM1, aDir, aDirX);
  //
  aTwoPI = 2. * M_PI;
  aC1    = thePitch / aTwoPI;
  aT0    = 0.;
  aT1    = aAngleStart;

  aT2 = theHeight / aC1;
  //
  // 2. compute
  bool                    bIsDone;
  int                     iErr, aNbC, i;
  HelixGeom_BuilderHelix  aBH;
  gp_Pnt                  aP1, aP2;
  BRep_Builder            aBB;
  BRepBuilderAPI_MakeEdge aBME;
  TopoDS_Vertex           aV1, aV2;
  // TopoDS_Wire aW;
  TopoDS_Edge aE;
  //
  aBH.SetPosition(aAx2);
  aBH.SetCurveParameters(aT0, aT2, thePitch, aDist, theTaperAngle, bIsClockwise);
  aBH.SetTolerance(myTolerance);
  aBH.SetApproxParameters(myContinuity, myMaxDegree, myMaxSegments);
  //
  aBH.Perform();
  iErr = aBH.ErrorStatus();
  if (iErr)
  {
    myErrorStatus = 2;
    return;
  }
  //
  aBB.MakeWire(thePart);
  //
  myTolReached = std::max(myTolReached, aBH.ToleranceReached());
  NCollection_Sequence<occ::handle<Geom_Curve>> aSC;
  aSC.Assign(aBH.Curves());
  if (aT1 < 0.)
  {
    HelixGeom_BuilderHelix aBH1;
    aBH1.SetPosition(aAx2);
    aBH1.SetCurveParameters(aT1, aT0, thePitch, aDist, theTaperAngle, bIsClockwise);
    aBH1.SetTolerance(myTolerance);
    aBH1.SetApproxParameters(myContinuity, myMaxDegree, myMaxSegments);
    //
    aBH1.Perform();
    iErr = aBH1.ErrorStatus();
    if (iErr)
    {
      myErrorStatus = 2;
      return;
    }

    myTolReached = std::max(myTolReached, aBH1.ToleranceReached());
    const NCollection_Sequence<occ::handle<Geom_Curve>>& aSC1 = aBH1.Curves();
    int                                                  nbc  = aSC1.Length();
    for (i = nbc; i >= 1; i--)
    {
      aSC.Prepend(aSC1.Value(i));
    }
  }

  aNbC = aSC.Length();
  for (i = 1; i <= aNbC; ++i)
  {
    occ::handle<Geom_Curve> aC = aSC(i);
    //
    if (i == 1)
    {
      if (aT1 > 0.)
      {
        aT2                                = aC->LastParameter();
        occ::handle<Geom_TrimmedCurve> aCT = new Geom_TrimmedCurve(aC, aT1, aT2);
        aC                                 = aCT;
      }
      aT1 = aC->FirstParameter();
      aC->D0(aT1, aP1);
      aBB.MakeVertex(aV1, aP1, myTolReached);
      aV1.Orientation(TopAbs_FORWARD);
    }
    //
    aT2 = aC->LastParameter();
    aC->D0(aT2, aP2);
    aBB.MakeVertex(aV2, aP2, myTolReached);
    aV2.Orientation(TopAbs_REVERSED);
    //
    aBME.Init(aC, aV1, aV2);
    bIsDone = aBME.IsDone();
    if (!bIsDone)
    {
      myErrorStatus = 3;
      return;
    }
    aE = aBME.Edge();
    aBB.UpdateEdge(aE, myTolReached);
    aBB.Add(thePart, aE);

    //
    aV1 = aV2;
    aV1.Orientation(TopAbs_FORWARD);
  }
  //
  if (myTolReached > myTolerance)
    myWarningStatus = 1;
}

//=================================================================================================

void HelixBRep_BuilderHelix::Smoothing(NCollection_List<TopoDS_Shape>& theParts)
{
  if (theParts.Extent() == 1)
    return;

  BRepTools_WireExplorer anExpl;

  TopoDS_Wire aPrevWire = TopoDS::Wire(theParts.First());
  anExpl.Init(aPrevWire);
  TopoDS_Edge aPrevEdge;
  for (; anExpl.More(); anExpl.Next())
  {
    aPrevEdge = anExpl.Current();
  }

  NCollection_List<TopoDS_Shape>::Iterator anIter(theParts);
  anIter.Next();
  for (; anIter.More(); anIter.Next())
  {
    TopoDS_Wire aNextWire = TopoDS::Wire(anIter.Value());
    anExpl.Clear();
    anExpl.Init(aNextWire);
    TopoDS_Edge aNextEdge = anExpl.Current();

    // Smoothing curves
    SmoothingEdges(aPrevEdge, aNextEdge);

    for (; anExpl.More(); anExpl.Next())
    {
      aPrevEdge = anExpl.Current();
    }
  }
}

//=================================================================================================

void HelixBRep_BuilderHelix::SmoothingEdges(TopoDS_Edge& thePrev, TopoDS_Edge& theNext)
{

  static const double EpsAng = 1.e-7;
  BRep_Builder        aBB;

  double                         f1, l1, f2, l2;
  occ::handle<Geom_BSplineCurve> aCPrev =
    occ::down_cast<Geom_BSplineCurve>(BRep_Tool::Curve(thePrev, f1, l1));

  occ::handle<Geom_BSplineCurve> aCNext =
    occ::down_cast<Geom_BSplineCurve>(BRep_Tool::Curve(theNext, f2, l2));

  gp_Pnt P1, P2;
  gp_Vec V1, V2;

  aCPrev->D1(l1, P1, V1);
  aCNext->D1(f2, P2, V2);

  if (V1.Angle(V2) < EpsAng)
    return;

  V1 = 0.5 * (V1 + V2);
  V2 = V1;

  int  anErrorStatus = 1;
  int  aDegMax       = Geom_BSplineCurve::MaxDegree();
  int  aDeg          = aCPrev->Degree();
  int  i;
  bool bPrevOK = false;
  bool bNextOK = false;

  aCPrev->MovePointAndTangent(l1, P1, V1, myTolerance, 1, -1, anErrorStatus);
  if (anErrorStatus != 0)
  {
    for (i = aDeg + 1; i <= aDegMax; ++i)
    {
      aCPrev->IncreaseDegree(i);
      aCPrev->MovePointAndTangent(l1, P1, V1, myTolerance, 1, -1, anErrorStatus);
      if (anErrorStatus == 0)
      {
        bPrevOK = true;
        break;
      }
    }
  }
  else
  {
    bPrevOK = true;
  }

  if (bPrevOK)
  {
    aBB.UpdateEdge(thePrev, aCPrev, BRep_Tool::Tolerance(thePrev));
  }

  aDeg = aCNext->Degree();
  aCNext->MovePointAndTangent(f2, P2, V2, myTolerance, -1, 1, anErrorStatus);
  if (anErrorStatus != 0)
  {
    for (i = aDeg + 1; i <= aDegMax; ++i)
    {
      aCNext->IncreaseDegree(i);
      aCNext->MovePointAndTangent(f2, P2, V2, myTolerance, -1, 1, anErrorStatus);
      if (anErrorStatus == 0)
      {
        bNextOK = true;
        break;
      }
    }
  }
  else
  {
    bNextOK = true;
  }

  if (bNextOK)
  {
    aBB.UpdateEdge(theNext, aCNext, BRep_Tool::Tolerance(theNext));
  }
}

//=================================================================================================

int HelixBRep_BuilderHelix::ErrorStatus() const
{
  return myErrorStatus;
}

//=================================================================================================

int HelixBRep_BuilderHelix::WarningStatus() const
{
  return myWarningStatus;
}

//=================================================================================================

const TopoDS_Shape& HelixBRep_BuilderHelix::Shape() const
{
  return myShape;
}

//=================================================================================================

void HelixBRep_BuilderHelix::SetParameters(const gp_Ax3&                     theAxis,
                                           const NCollection_Array1<double>& theDiams,
                                           const NCollection_Array1<double>& thePitches,
                                           const NCollection_Array1<double>& theNbTurns)

{
  int aNbParts = theDiams.Length() - 1;

  if (aNbParts != thePitches.Length() || aNbParts != theNbTurns.Length())
  {
    throw Standard_ConstructionError(
      "HelixBRep_BuilderHelix::SetParameters: wrong array dimension");
  }

  NCollection_Array1<double> aHeights(1, aNbParts);
  NCollection_Array1<bool>   bIsPitches(1, aNbParts);
  bIsPitches.Init(true);
  int i, ip, in;
  for (i = 1, ip = thePitches.Lower(), in = theNbTurns.Lower(); i <= thePitches.Length();
       ++i, ip++, in++)
  {
    aHeights(i) = thePitches(ip) * theNbTurns(in);
  }

  SetParameters(theAxis, theDiams, aHeights, thePitches, bIsPitches);
}

//=================================================================================================

void HelixBRep_BuilderHelix::SetParameters(const gp_Ax3&                     theAxis,
                                           const double                      theDiam,
                                           const NCollection_Array1<double>& thePitches,
                                           const NCollection_Array1<double>& theNbTurns)
{
  int aNbParts = thePitches.Length();

  if (aNbParts != theNbTurns.Length())
  {
    throw Standard_ConstructionError(
      "HelixBRep_BuilderHelix::SetParameters: wrong array dimension");
  }

  NCollection_Array1<double> aHeights(1, aNbParts);
  NCollection_Array1<bool>   bIsPitches(1, aNbParts);
  bIsPitches.Init(true);
  int i, ip, in;
  for (i = 1, ip = thePitches.Lower(), in = theNbTurns.Lower(); i <= thePitches.Length();
       ++i, ip++, in++)
  {
    aHeights(i) = thePitches(ip) * theNbTurns(in);
  }

  SetParameters(theAxis, theDiam, aHeights, thePitches, bIsPitches);
}

//=================================================================================================

void HelixBRep_BuilderHelix::SetParameters(const gp_Ax3&                     theAxis,
                                           const double                      theDiam1,
                                           const double                      theDiam2,
                                           const NCollection_Array1<double>& thePitches,
                                           const NCollection_Array1<double>& theNbTurns)
{
  int aNbParts = thePitches.Length();

  if (aNbParts != theNbTurns.Length())
  {
    throw Standard_ConstructionError(
      "HelixBRep_BuilderHelix::SetParameters: wrong array dimension");
  }

  NCollection_Array1<double> aHeights(1, aNbParts);
  NCollection_Array1<bool>   bIsPitches(1, aNbParts);
  bIsPitches.Init(true);
  int i, ip, in;
  for (i = 1, ip = thePitches.Lower(), in = theNbTurns.Lower(); i <= thePitches.Length();
       ++i, ip++, in++)
  {
    aHeights(i) = thePitches(ip) * theNbTurns(in);
  }

  SetParameters(theAxis, theDiam1, theDiam2, aHeights, thePitches, bIsPitches);
}
