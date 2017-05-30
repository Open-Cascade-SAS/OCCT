// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#include <Hatch_Hatcher.hxx>
#include <Graphic3d_Group.hxx>
#include <gp_Pnt.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Adaptor3d_IsoCurve.hxx>
#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <Precision.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <Geom_Surface.hxx>
#include <StdPrs_WFRestrictedFace.hxx>
#include <StdPrs_ToolRFace.hxx>
#include <StdPrs_Curve.hxx>
#include <Prs3d_NListOfSequenceOfPnt.hxx>

//=========================================================================
// function: Add
// purpose
//=========================================================================

void StdPrs_WFRestrictedFace::Add
                (const Handle (Prs3d_Presentation)&  thePresentation,
                 const Handle(BRepAdaptor_HSurface)& theFace,
                 const Standard_Boolean              theDrawUIso,
                 const Standard_Boolean              theDrawVIso,
                 const Standard_Integer              theNbUIso,
                 const Standard_Integer              theNBVIso,
                 const Handle(Prs3d_Drawer)&         theDrawer,
                 Prs3d_NListOfSequenceOfPnt&         theCurves)
{
  Standard_Integer aNbPoints = theDrawer->Discretisation();
  StdPrs_ToolRFace aToolRst (theFace);

  // Compute bounds of the restriction
  Standard_Real aUMin,aUMax,aVMin,aVMax;
  Standard_Integer anI;
  gp_Pnt2d aPoint1,aPoint2;
  Bnd_Box2d aBndBox;

  for (aToolRst.Init(); aToolRst.More(); aToolRst.Next())
  {
    Adaptor2d_Curve2dPtr aRCurve = aToolRst.Value();
    BndLib_Add2dCurve::Add(*aRCurve, Precision::PConfusion(), aBndBox);
  }
  if (!aBndBox.IsVoid())
    aBndBox.Get(aUMin, aVMin, aUMax, aVMax);
  else
  { // No pcurves -- take natural bounds
    aUMin = theFace->Surface().FirstUParameter();
    aVMin = theFace->Surface().FirstVParameter();
    aUMax = theFace->Surface().LastUParameter();
    aVMax = theFace->Surface().LastVParameter();
  }

  // Load the isos
  Hatch_Hatcher anIsoBuild(1.e-5,aToolRst.IsOriented());
  Standard_Boolean isFaceUClosed = theFace->IsUClosed();
  Standard_Boolean isFaceVClosed = theFace->IsVClosed();

  if (!isFaceUClosed)
  {
    aUMin = aUMin + ( aUMax - aUMin) /1000.;
    aUMax = aUMax - ( aUMax - aUMin) /1000.; 
  }

  if (!isFaceVClosed)
  {
    aVMin = aVMin + ( aVMax - aVMin) /1000.;
    aVMax = aVMax - ( aVMax - aVMin) /1000.; 
  }

  if (theDrawUIso)
  {
    if (theNbUIso > 0)
    {
      isFaceUClosed = Standard_False;
      Standard_Real du= isFaceUClosed ? (aUMax-aUMin)/theNbUIso : (aUMax-aUMin)/(1+theNbUIso);
      for (anI=1; anI<=theNbUIso;anI++)
      {
        anIsoBuild.AddXLine(aUMin+du*anI);
      }
    }
  }
  if (theDrawVIso)
  {
    if (theNBVIso > 0)
    {
      isFaceVClosed = Standard_False;
      Standard_Real dv= isFaceVClosed ?(aVMax-aVMin)/theNBVIso : (aVMax-aVMin)/(1+theNBVIso);
      for (anI=1; anI<=theNBVIso;anI++)
      {
        anIsoBuild.AddYLine(aVMin+dv*anI);
      }
    }
  }

  // Trim the isos
  Standard_Real anU1, anU2, anU, aDU;

  for (aToolRst.Init(); aToolRst.More(); aToolRst.Next())
  {
    TopAbs_Orientation anOrientation = aToolRst.Orientation();
    if (anOrientation == TopAbs_FORWARD || anOrientation == TopAbs_REVERSED)
    {
      Adaptor2d_Curve2dPtr aRCurve = aToolRst.Value();
      anU1 = aRCurve->FirstParameter();
      anU2 = aRCurve->LastParameter();
      if (aRCurve->GetType() != GeomAbs_Line)
      {
        aDU = (anU2-anU1)/(aNbPoints-1);
        aPoint2 = aRCurve->Value(anU1);
        for (anI = 2; anI <= aNbPoints; ++anI)
        {
          anU = anU1 + (anI-1)*aDU;
          aPoint1 = aPoint2;
          aPoint2 = aRCurve->Value(anU);
          if(anOrientation == TopAbs_FORWARD )
            anIsoBuild.Trim(aPoint1,aPoint2);
          else
            anIsoBuild.Trim(aPoint2,aPoint1);
        }
      }
      else {
        aPoint1 = aRCurve->Value(anU1);
        aPoint2 = aRCurve->Value(anU2);
        if(anOrientation == TopAbs_FORWARD )
          anIsoBuild.Trim(aPoint1,aPoint2);
        else
          anIsoBuild.Trim(aPoint2,aPoint1);
      }
    }
  }

  // Draw the isos
  Adaptor3d_IsoCurve anIsoCurve;
  anIsoCurve.Load(theFace);
  Handle(Geom_Curve) aBCurve;
  const BRepAdaptor_Surface& aBSurface = *(BRepAdaptor_Surface*)&(theFace->Surface());
  GeomAbs_SurfaceType aFaceType = theFace->GetType();

  Standard_Integer aNbLines = anIsoBuild.NbLines();
  Handle(Geom_Surface) aGeomBSurface;
  if (aFaceType == GeomAbs_BezierSurface)
  {
    aGeomBSurface = aBSurface.Bezier();
  }
  else if (aFaceType == GeomAbs_BSplineSurface)
  {
    aGeomBSurface = aBSurface.BSpline();
  }

  for (anI = 1; anI <= aNbLines; ++anI)
  {
    Standard_Integer NumberOfIntervals = anIsoBuild.NbIntervals(anI);
    Standard_Real anIsoCoord = anIsoBuild.Coordinate(anI);
    for (Standard_Integer aJ = 1; aJ <= NumberOfIntervals; aJ++)
    {
      Standard_Real b1=anIsoBuild.Start(anI,aJ),b2=anIsoBuild.End(anI,aJ);

      if(b1 == RealFirst() || b2 == RealLast())
        continue;

      Handle(TColgp_HSequenceOfPnt) aPoints = new TColgp_HSequenceOfPnt;
      if (!aGeomBSurface.IsNull())
      {
        if (anIsoBuild.IsXLine (anI))
        {
          aBCurve = aGeomBSurface->UIso (anIsoCoord);
        }
        else
        {
          aBCurve = aGeomBSurface->VIso (anIsoCoord);
        }

        //Note that the isos are the part of the shape, it will be displayed after a computation the whole shape
        //NbPoints = 30 - default parameter for computation of such curves
        StdPrs_Curve::Add (thePresentation, GeomAdaptor_Curve (aBCurve), b1, b2, aPoints->ChangeSequence(), 30, Standard_False);
        theCurves.Append (aPoints);
      }
      else
      {
        if (anIsoBuild.IsXLine (anI))
        {
          anIsoCurve.Load (GeomAbs_IsoU, anIsoCoord, b1, b2);
        }
        else
        {
          anIsoCurve.Load(GeomAbs_IsoV, anIsoCoord, b1, b2);
        }
        StdPrs_Curve::Add (thePresentation, anIsoCurve, theDrawer, aPoints->ChangeSequence(), Standard_False);
        theCurves.Append (aPoints);
      }
    }
  }
}


//=========================================================================
// function: Match
// purpose
//=========================================================================

Standard_Boolean StdPrs_WFRestrictedFace::Match
                  (const Standard_Real               theX,
                   const Standard_Real               theY,
                   const Standard_Real               theZ,
                   const Standard_Real               theDistance,
                   const Handle(BRepAdaptor_HSurface)& theFace,
                   const Standard_Boolean              theDrawUIso,
                   const Standard_Boolean              theDrawVIso,
                   const Standard_Real               theDeflection,
                   const Standard_Integer              theNbUIso,
                   const Standard_Integer              theNBVIso,
                   const Handle(Prs3d_Drawer)&         theDrawer)
{
  Standard_Real aLimit = theDrawer->MaximalParameterValue();
  Standard_Integer aNbPoints = theDrawer->Discretisation();
  StdPrs_ToolRFace aToolRst (theFace);

  // Compute bounds of the restriction
  Standard_Real anUMin,anUMax,aVMin,aVMax;
  Standard_Real anU,aV,aStep;
  Standard_Integer anI,anNbP = 10;
  anUMin = aVMin = RealLast();
  anUMax = aVMax = RealFirst();
  gp_Pnt2d aPoint1,aPoint2;

  for (aToolRst.Init(); aToolRst.More(); aToolRst.Next())
  {
    Adaptor2d_Curve2dPtr aRCurve = aToolRst.Value();
    anU = aRCurve->FirstParameter();
    aV = aRCurve->LastParameter();
    if (aRCurve->GetType() != GeomAbs_Line)
    {
      aStep = ( aV - anU) / anNbP;
      for (anI = 0; anI <= anNbP; ++anI)
      {
        gp_Pnt2d aRCurvePoint = aRCurve->Value(anU);
        if (aRCurvePoint.X() < anUMin) anUMin = aRCurvePoint.X();
        if (aRCurvePoint.X() > anUMax) anUMax = aRCurvePoint.X();
        if (aRCurvePoint.Y() < aVMin) aVMin = aRCurvePoint.Y();
        if (aRCurvePoint.Y() > aVMax) aVMax = aRCurvePoint.Y();
        anU += aStep;
      }
    }
    else
    {
      aPoint1 = aRCurve->Value(anU);
      if (aPoint1.X() < anUMin) anUMin = aPoint1.X();
      if (aPoint1.X() > anUMax) anUMax = aPoint1.X();
      if (aPoint1.Y() < aVMin) aVMin = aPoint1.Y();
      if (aPoint1.Y() > aVMax) aVMax = aPoint1.Y();

      aPoint2 = aRCurve->Value(aV);
      if (aPoint2.X() < anUMin) anUMin = aPoint2.X();
      if (aPoint2.X() > anUMax) anUMax = aPoint2.X();
      if (aPoint2.Y() < aVMin) aVMin = aPoint2.Y();
      if (aPoint2.Y() > aVMax) aVMax = aPoint2.Y();
    }
  }

  // Load the isos
  Hatch_Hatcher anIsoBuild(1.e-5,aToolRst.IsOriented());
  Standard_Boolean anUClosed = theFace->IsUClosed();
  Standard_Boolean aVClosed = theFace->IsVClosed();

  if ( ! anUClosed )
  {
    anUMin = anUMin + ( anUMax - anUMin) /1000.;
    anUMax = anUMax - ( anUMax - anUMin) /1000.; 
  }

  if ( ! aVClosed )
  {
    aVMin = aVMin + ( aVMax - aVMin) /1000.;
    aVMax = aVMax - ( aVMax - aVMin) /1000.; 
  }

  if (theDrawUIso)
  {
    if (theNbUIso > 0)
    {
      anUClosed = Standard_False;
      Standard_Real du= anUClosed ? (anUMax-anUMin)/theNbUIso : (anUMax-anUMin)/(1+theNbUIso);
      for (anI=1; anI<=theNbUIso;anI++){
        anIsoBuild.AddXLine(anUMin+du*anI);
      }
    }
  }
  if (theDrawVIso){
    if ( theNBVIso > 0) {
      aVClosed = Standard_False;
      Standard_Real dv= aVClosed ?(aVMax-aVMin)/theNBVIso : (aVMax-aVMin)/(1+theNBVIso);
      for (anI=1; anI<=theNBVIso;anI++){
        anIsoBuild.AddYLine(aVMin+dv*anI);
      }
    }
  }

  // Trim the isos
  Standard_Real anU1, anU2, aDU;

  for (aToolRst.Init(); aToolRst.More(); aToolRst.Next())
  {
    TopAbs_Orientation Orient = aToolRst.Orientation();
    if (Orient == TopAbs_FORWARD || Orient == TopAbs_REVERSED)
    {
      Adaptor2d_Curve2dPtr aRCurve = aToolRst.Value();
      anU1 = aRCurve->FirstParameter();
      anU2 = aRCurve->LastParameter();
      if (aRCurve->GetType() != GeomAbs_Line) {
        aDU = (anU2-anU1)/(aNbPoints-1);
        aPoint2 = aRCurve->Value(anU1);
        for (anI = 2; anI <= aNbPoints; anI++) {
          anU = anU1 + (anI-1)*aDU;
          aPoint1 = aPoint2;
          aPoint2 = aRCurve->Value(anU);
          if(Orient == TopAbs_FORWARD )
            anIsoBuild.Trim(aPoint1,aPoint2);
          else
            anIsoBuild.Trim(aPoint2,aPoint1);
        }
      }
      else {
        aPoint1 = aRCurve->Value(anU1);
        aPoint2 = aRCurve->Value(anU2);
        if(Orient == TopAbs_FORWARD )
          anIsoBuild.Trim(aPoint1,aPoint2);
        else
          anIsoBuild.Trim(aPoint2,aPoint1);
      }	  
    }
  }

  // Draw the isos

  Adaptor3d_IsoCurve anIso;
  anIso.Load(theFace);
  Standard_Integer aNbLines = anIsoBuild.NbLines();

  for (anI = 1; anI <= aNbLines; anI++)
  {
    Standard_Integer aNbIntervals = anIsoBuild.NbIntervals(anI);
    Standard_Real aCoord = anIsoBuild.Coordinate(anI);
    for (Standard_Integer j = 1; j <= aNbIntervals; j++)
    {
      Standard_Real anIsoStart=anIsoBuild.Start(anI,j),anIsoEnd=anIsoBuild.End(anI,j);

      anIsoStart = anIsoStart == RealFirst() ? - aLimit : anIsoStart;
      anIsoEnd = anIsoEnd == RealLast()  ?   aLimit : anIsoEnd;


      if (anIsoBuild.IsXLine(anI))
        anIso.Load(GeomAbs_IsoU,aCoord,anIsoStart,anIsoEnd);
      else
        anIso.Load(GeomAbs_IsoV,aCoord,anIsoStart,anIsoEnd);

      if (StdPrs_Curve::Match(theX,theY,theZ,theDistance,anIso, 
        theDeflection, aLimit, aNbPoints))
        return Standard_True;


    }
  }
  return Standard_False;
}


//=========================================================================
// function: Add
// purpose
//=========================================================================

void StdPrs_WFRestrictedFace::Add
    (const Handle (Prs3d_Presentation)&  thePresentation,
     const Handle(BRepAdaptor_HSurface)& theFace,
     const Handle (Prs3d_Drawer)&        theDrawer)
{
  Prs3d_NListOfSequenceOfPnt aCurves;
  StdPrs_WFRestrictedFace::Add (thePresentation,
                                theFace,
                                Standard_True,
                                Standard_True,
                                theDrawer->UIsoAspect()->Number(),
                                theDrawer->VIsoAspect()->Number(),
                                theDrawer,
                                aCurves);
}


//=========================================================================
// function: AddUIso
// purpose
//=========================================================================

void StdPrs_WFRestrictedFace::AddUIso
    (const Handle (Prs3d_Presentation)&  thePresentation,
     const Handle(BRepAdaptor_HSurface)& theFace,
     const Handle (Prs3d_Drawer)&        theDrawer)
{
  Prs3d_NListOfSequenceOfPnt aCurves;
  StdPrs_WFRestrictedFace::Add (thePresentation,
                                theFace,
                                Standard_True,
                                Standard_False,
                                theDrawer->UIsoAspect()->Number(),
                                theDrawer->VIsoAspect()->Number(),
                                theDrawer,
                                aCurves);
}


//=========================================================================
// function: AddVIso
// purpose
//=========================================================================

void StdPrs_WFRestrictedFace::AddVIso
    (const Handle (Prs3d_Presentation)&  thePresentation,
     const Handle(BRepAdaptor_HSurface)& theFace,
     const Handle (Prs3d_Drawer)&        theDrawer)
{
  Prs3d_NListOfSequenceOfPnt aCurves;
  StdPrs_WFRestrictedFace::Add (thePresentation,
                                theFace,
                                Standard_False,
                                Standard_True,
                                theDrawer->UIsoAspect()->Number(),
                                theDrawer->VIsoAspect()->Number(),
                                theDrawer,
                                aCurves);
}


//=========================================================================
// function: Match
// purpose
//=========================================================================

Standard_Boolean StdPrs_WFRestrictedFace::Match
                (const Standard_Real               theX,
                 const Standard_Real               theY,
                 const Standard_Real               theZ,
                 const Standard_Real               theDistance,
                 const Handle(BRepAdaptor_HSurface)& theFace,
                 const Handle (Prs3d_Drawer)&        theDrawer)
{
  return StdPrs_WFRestrictedFace::Match (
                      theX, theY, theZ, theDistance,
                      theFace,
                      Standard_True,
                      Standard_True,
                      theDrawer->MaximalChordialDeviation(),
                      theDrawer->UIsoAspect()->Number(),
                      theDrawer->VIsoAspect()->Number(),
                      theDrawer);
}


//=========================================================================
// function: MatchUIso
// purpose
//=========================================================================

Standard_Boolean StdPrs_WFRestrictedFace::MatchUIso
                (const Standard_Real               theX,
                 const Standard_Real               theY,
                 const Standard_Real               theZ,
                 const Standard_Real               theDistance,
                 const Handle(BRepAdaptor_HSurface)& theFace,
                 const Handle (Prs3d_Drawer)&        theDrawer)
{
  return StdPrs_WFRestrictedFace::Match (
                      theX, theY, theZ,theDistance,
                      theFace,
                      Standard_True,
                      Standard_False,
                      theDrawer->MaximalChordialDeviation(),
                      theDrawer->UIsoAspect()->Number(),
                      theDrawer->VIsoAspect()->Number(),
                      theDrawer);
}


//=========================================================================
// function: MatchVIso
// purpose
//=========================================================================

Standard_Boolean StdPrs_WFRestrictedFace::MatchVIso
                (const Standard_Real               theX,
                 const Standard_Real               theY,
                 const Standard_Real               theZ,
                 const Standard_Real               theDistance,
                 const Handle(BRepAdaptor_HSurface)& theFace,
                 const Handle (Prs3d_Drawer)&        theDrawer)
{
  return StdPrs_WFRestrictedFace::Match (
                      theX, theY, theZ, theDistance,
                      theFace,
                      Standard_False,
                      Standard_True,
                      theDrawer->MaximalChordialDeviation(),
                      theDrawer->UIsoAspect()->Number(),
                      theDrawer->VIsoAspect()->Number(),
                      theDrawer);
}
