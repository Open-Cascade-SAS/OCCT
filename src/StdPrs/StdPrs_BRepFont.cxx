// Created on: 2013-09-16
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

#include <StdPrs_BRepFont.hxx>

#include <BRep_Tool.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <Font_FTLibrary.hxx>
#include <Font_FontMgr.hxx>
#include <GCE2d_MakeSegment.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_Plane.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Line.hxx>
#include <GeomAPI.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomLib.hxx>
#include <gp_Pln.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>
#include <TopTools_DataMapOfShapeSequenceOfShape.hxx>

#ifdef HAVE_FREETYPE
  #include <ft2build.h>
  #include FT_FREETYPE_H
#endif

IMPLEMENT_STANDARD_RTTIEXT(StdPrs_BRepFont, Standard_Transient)

namespace
{
  // pre-defined font rendering options
  static const unsigned int THE_FONT_SIZE      = 72;
  static const unsigned int THE_RESOLUTION_DPI = 4800;
  static const Font_FTFontParams THE_FONT_PARAMS (THE_FONT_SIZE, THE_RESOLUTION_DPI);

  // compute scaling factor for specified font size
  inline Standard_Real getScale (const Standard_Real theSize)
  {
    return theSize / Standard_Real(THE_FONT_SIZE) * 72.0 / Standard_Real(THE_RESOLUTION_DPI);
  }

#ifdef HAVE_FREETYPE
  //! Auxiliary method to convert FT_Vector to gp_XY
  static gp_XY readFTVec (const FT_Vector& theVec,
                          const Standard_Real theScaleUnits,
                          const Standard_Real theWidthScaling = 1.0)
  {
    return gp_XY (theScaleUnits * Standard_Real(theVec.x) * theWidthScaling / 64.0, theScaleUnits * Standard_Real(theVec.y) / 64.0);
  }

  //! Auxiliary method for classification wire theW2 with respect to wire theW1
  static TopAbs_State classifyWW (const TopoDS_Wire& theW1,
                                  const TopoDS_Wire& theW2,
                                  const TopoDS_Face& theF)
  {
    TopAbs_State aRes = TopAbs_UNKNOWN;

    TopoDS_Face aF = TopoDS::Face (theF.EmptyCopied());
    aF.Orientation (TopAbs_FORWARD);
    BRep_Builder aB;
    aB.Add (aF, theW1);
    BRepTopAdaptor_FClass2d aClass2d (aF, ::Precision::PConfusion());
    for (TopoDS_Iterator anEdgeIter (theW2); anEdgeIter.More(); anEdgeIter.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge (anEdgeIter.Value());
      Standard_Real aPFirst = 0.0, aPLast = 0.0;
      Handle(Geom2d_Curve) aCurve2d = BRep_Tool::CurveOnSurface (anEdge, theF, aPFirst, aPLast);
      if (aCurve2d.IsNull())
      {
        continue;
      }

      gp_Pnt2d aPnt2d = aCurve2d->Value ((aPFirst + aPLast) / 2.0);
      TopAbs_State aState = aClass2d.Perform (aPnt2d, Standard_False);
      if (aState == TopAbs_OUT
       || aState == TopAbs_IN)
      {
        if (aRes == TopAbs_UNKNOWN)
        {
          aRes = aState;
        }
        else if (aRes != aState)
        {
          return TopAbs_UNKNOWN;
        }
      }
    }
    return aRes;
  }
#endif
}

// =======================================================================
// function : Constructor
// purpose  :
// =======================================================================
StdPrs_BRepFont::StdPrs_BRepFont ()
: myPrecision  (Precision::Confusion()),
  myScaleUnits (1.0),
  myIsCompositeCurve (Standard_False),
  my3Poles     (1, 3),
  my4Poles     (1, 4)
{
  myFTFont = new Font_FTFont();
  init();
}

// =======================================================================
// function : init
// purpose  :
// =======================================================================
void StdPrs_BRepFont::init()
{
  mySurface        = new Geom_Plane (gp_Pln (gp::XOY()));
  myCurve2dAdaptor = new Geom2dAdaptor_Curve();
  Handle(Adaptor3d_Surface) aSurfAdaptor = new GeomAdaptor_Surface (mySurface);
  myCurvOnSurf.Load (aSurfAdaptor);
}

// =======================================================================
// function : Constructor
// purpose  :
// =======================================================================
StdPrs_BRepFont::StdPrs_BRepFont (const NCollection_String& theFontPath,
                                  const Standard_Real       theSize,
                                  const Standard_Integer    theFaceId)
: myPrecision  (Precision::Confusion()),
  myScaleUnits (1.0),
  myIsCompositeCurve (Standard_False),
  my3Poles     (1, 3),
  my4Poles     (1, 4)
{
  init();
  if (theSize <= myPrecision * 100.0)
  {
    return;
  }

  myScaleUnits = getScale (theSize);
  myFTFont = new Font_FTFont();
  myFTFont->Init (theFontPath.ToCString(), THE_FONT_PARAMS, theFaceId);
}

// =======================================================================
// function : Constructor
// purpose  :
// =======================================================================
StdPrs_BRepFont::StdPrs_BRepFont (const NCollection_String& theFontName,
                                  const Font_FontAspect     theFontAspect,
                                  const Standard_Real       theSize,
                                  const Font_StrictLevel    theStrictLevel)
: myPrecision  (Precision::Confusion()),
  myScaleUnits (1.0),
  myIsCompositeCurve (Standard_False),
  my3Poles     (1, 3),
  my4Poles     (1, 4)
{
  init();
  if (theSize <= myPrecision * 100.0)
  {
    return;
  }

  myScaleUnits = getScale (theSize);
  myFTFont = new Font_FTFont();
  myFTFont->FindAndInit (theFontName.ToCString(), theFontAspect, THE_FONT_PARAMS, theStrictLevel);
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void StdPrs_BRepFont::Release()
{
  myCache.Clear();
  myFTFont->Release();
}

// =======================================================================
// function : FindAndCreate
// purpose  :
// =======================================================================
Handle(StdPrs_BRepFont) StdPrs_BRepFont::FindAndCreate (const TCollection_AsciiString& theFontName,
                                                        const Font_FontAspect     theFontAspect,
                                                        const Standard_Real       theSize,
                                                        const Font_StrictLevel    theStrictLevel)
{
  Handle(StdPrs_BRepFont) aFont = new StdPrs_BRepFont();

  if (aFont->FindAndInit (theFontName, theFontAspect, theSize, theStrictLevel))
    return aFont;

  return Handle(StdPrs_BRepFont)();
}

// =======================================================================
// function : SetCompositeCurveMode
// purpose  :
// =======================================================================
void StdPrs_BRepFont::SetCompositeCurveMode (const Standard_Boolean theToConcatenate)
{
  if (myIsCompositeCurve != theToConcatenate)
  {
    myIsCompositeCurve = theToConcatenate;
    myCache.Clear();
  }
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool StdPrs_BRepFont::Init (const NCollection_String& theFontPath,
                            const Standard_Real       theSize,
                            const Standard_Integer    theFaceId)
{
  if (theSize <= myPrecision * 100.0)
  {
    return false;
  }

  myScaleUnits = getScale (theSize);
  myCache.Clear();
  return myFTFont->Init (theFontPath.ToCString(), THE_FONT_PARAMS, theFaceId);
}

// =======================================================================
// function : FindAndInit
// purpose  :
// =======================================================================
bool StdPrs_BRepFont::FindAndInit (const TCollection_AsciiString& theFontName,
                                   const Font_FontAspect  theFontAspect,
                                   const Standard_Real    theSize,
                                   const Font_StrictLevel theStrictLevel)
{
  if (theSize <= myPrecision * 100.0)
  {
    return false;
  }

  myScaleUnits = getScale (theSize);
  myCache.Clear();
  return myFTFont->FindAndInit (theFontName.ToCString(), theFontAspect, THE_FONT_PARAMS, theStrictLevel);
}

// =======================================================================
// function : RenderGlyph
// purpose  :
// =======================================================================
TopoDS_Shape StdPrs_BRepFont::RenderGlyph (const Standard_Utf32Char& theChar)
{
  TopoDS_Shape aShape;
  Standard_Mutex::Sentry aSentry (myMutex);
  renderGlyph (theChar, aShape);
  return aShape;
}

// =======================================================================
// function : to3d
// purpose  :
// =======================================================================
bool StdPrs_BRepFont::to3d (const Handle(Geom2d_Curve)& theCurve2d,
                            const GeomAbs_Shape        theContinuity,
                            Handle(Geom_Curve)&        theCurve3d)
{
  Standard_Real aMaxDeviation   = 0.0;
  Standard_Real anAverDeviation = 0.0;
  myCurve2dAdaptor->Load (theCurve2d);
  const Handle(Adaptor2d_Curve2d)& aCurve = myCurve2dAdaptor; // to avoid ambiguity
  myCurvOnSurf.Load (aCurve);
  GeomLib::BuildCurve3d (myPrecision, myCurvOnSurf,
                         myCurve2dAdaptor->FirstParameter(), myCurve2dAdaptor->LastParameter(),
                         theCurve3d, aMaxDeviation, anAverDeviation, theContinuity);
  return !theCurve3d.IsNull();
}


// =======================================================================
// function : buildFaces
// purpose  :
// =======================================================================
Standard_Boolean StdPrs_BRepFont::buildFaces (const NCollection_Sequence<TopoDS_Wire>& theWires,
                                              TopoDS_Shape& theRes)
{
#ifdef HAVE_FREETYPE
  // classify wires
  NCollection_DataMap<TopoDS_Shape, NCollection_Sequence<TopoDS_Wire>, TopTools_ShapeMapHasher> aMapOutInts;
  TopTools_DataMapOfShapeInteger aMapNbOuts;
  TopoDS_Face aF;
  myBuilder.MakeFace (aF, mySurface, myPrecision);
  Standard_Integer aWireIter1Index = 1;
  for (NCollection_Sequence<TopoDS_Wire>::Iterator aWireIter1 (theWires); aWireIter1.More(); ++aWireIter1Index, aWireIter1.Next())
  {
    const TopoDS_Wire& aW1 = aWireIter1.Value();
    if (!aMapNbOuts.IsBound (aW1))
    {
      const Standard_Integer aNbOuts = 0;
      aMapNbOuts.Bind (aW1, aNbOuts);
    }

    NCollection_Sequence<TopoDS_Wire>* anIntWs = aMapOutInts.Bound (aW1, NCollection_Sequence<TopoDS_Wire>());
    Standard_Integer aWireIter2Index = 1;
    for (NCollection_Sequence<TopoDS_Wire>::Iterator aWireIter2 (theWires); aWireIter2.More(); ++aWireIter2Index, aWireIter2.Next())
    {
      if (aWireIter1Index == aWireIter2Index)
      {
        continue;
      }

      const TopoDS_Wire& aW2 = aWireIter2.Value();
      const TopAbs_State aClass = classifyWW (aW1, aW2, aF);
      if (aClass == TopAbs_IN)
      {
        anIntWs->Append (aW2);
        if (Standard_Integer* aNbOutsPtr = aMapNbOuts.ChangeSeek (aW2))
        {
          ++(*aNbOutsPtr);
        }
        else
        {
          const Standard_Integer aNbOuts = 1;
          aMapNbOuts.Bind (aW2, aNbOuts);
        }
      }
    }
  }

  // check out wires and remove "not out" wires from maps
  for (TopTools_DataMapIteratorOfDataMapOfShapeInteger anOutIter (aMapNbOuts); anOutIter.More(); anOutIter.Next())
  {
    const Standard_Integer aTmp = anOutIter.Value() % 2;
    if (aTmp > 0)
    {
      // not out wire
      aMapOutInts.UnBind (anOutIter.Key());
    }
  }

  // create faces for out wires
  TopTools_MapOfShape anUsedShapes;
  TopoDS_Compound aFaceComp;
  myBuilder.MakeCompound (aFaceComp);
  for (; !aMapOutInts.IsEmpty(); )
  {
    // find out wire with max number of outs
    TopoDS_Shape aW;
    Standard_Integer aMaxNbOuts = -1;
    for (NCollection_DataMap<TopoDS_Shape, NCollection_Sequence<TopoDS_Wire>, TopTools_ShapeMapHasher>::Iterator itMOI (aMapOutInts);
         itMOI.More(); itMOI.Next())
    {
      const TopoDS_Shape& aKey = itMOI.Key();
      const Standard_Integer aNbOuts = aMapNbOuts.Find (aKey);
      if (aNbOuts > aMaxNbOuts)
      {
        aMaxNbOuts = aNbOuts;
        aW = aKey;
      }
    }

    // create face for selected wire
    TopoDS_Face aNewF;
    myBuilder.MakeFace (aNewF, mySurface, myPrecision);
    myBuilder.Add (aNewF, aW);
    anUsedShapes.Add (aW);
    const NCollection_Sequence<TopoDS_Wire>& anIns = aMapOutInts.Find (aW);
    for (NCollection_Sequence<TopoDS_Wire>::Iterator aWireIter (anIns); aWireIter.More(); aWireIter.Next())
    {
      TopoDS_Wire aWin = aWireIter.Value();
      if (anUsedShapes.Contains (aWin))
      {
        continue;
      }

      aWin.Reverse();
      myBuilder.Add (aNewF, aWin);
      anUsedShapes.Add (aWin);
    }

    myBuilder.Add (aFaceComp, aNewF);
    aMapOutInts.UnBind (aW);
  }

  if (aFaceComp.NbChildren() == 0)
  {
    return Standard_False;
  }

  if (aFaceComp.NbChildren() == 1)
  {
    theRes = TopoDS_Iterator (aFaceComp).Value();
  }
  else
  {
    theRes = aFaceComp;
  }
  return Standard_True;
#else
  (void )theWires;
  (void )theRes;
  return Standard_False;
#endif
}

// =======================================================================
// function : renderGlyph
// purpose  :
// =======================================================================
Standard_Boolean StdPrs_BRepFont::renderGlyph (const Standard_Utf32Char theChar,
                                               TopoDS_Shape&            theShape)
{
  theShape.Nullify();
#ifdef HAVE_FREETYPE
  const FT_Outline* anOutline = myFTFont->renderGlyphOutline (theChar);
  if (!anOutline)
  {
    return Standard_False;
  }
  else if (myCache.Find (theChar, theShape))
  {
    return !theShape.IsNull();
  }

  if (!anOutline->n_contours)
    return Standard_False;

  TopLoc_Location aLoc;
  NCollection_Sequence<TopoDS_Wire> aWires;
  TopoDS_Compound aFaceCompDraft;

  // Get orientation is useless since it doesn't retrieve any in-font information and just computes orientation.
  // Because it fails in some cases - leave this to ShapeFix.
  //const FT_Orientation anOrient = FT_Outline_Get_Orientation (&anOutline);
  for (short aContour = 0, aStartIndex = 0; aContour < anOutline->n_contours; ++aContour)
  {
    const FT_Vector* aPntList = &anOutline->points[aStartIndex];
    const char* aTags      = &anOutline->tags[aStartIndex];
    const short anEndIndex = anOutline->contours[aContour];
    const short aPntsNb    = (anEndIndex - aStartIndex) + 1;
    aStartIndex = anEndIndex + 1;
    if (aPntsNb < 3 && !myFTFont->IsSingleStrokeFont())
    {
      // closed contour can not be constructed from < 3 points
      continue;
    }

    BRepBuilderAPI_MakeWire aWireMaker;

    gp_XY aPntPrev;
    gp_XY aPntCurr = readFTVec (aPntList[aPntsNb - 1], myScaleUnits, myFTFont->WidthScaling());
    gp_XY aPntNext = readFTVec (aPntList[0], myScaleUnits, myFTFont->WidthScaling());

    bool isLineSeg = !myFTFont->IsSingleStrokeFont()
                  && FT_CURVE_TAG(aTags[aPntsNb - 1]) == FT_Curve_Tag_On;
    gp_XY aPntLine1 = aPntCurr;

    // see http://freetype.sourceforge.net/freetype2/docs/glyphs/glyphs-6.html
    // for a full description of FreeType tags.
    for (short aPntId = 0; aPntId < aPntsNb; ++aPntId)
    {
      aPntPrev = aPntCurr;
      aPntCurr = aPntNext;
      aPntNext = readFTVec (aPntList[(aPntId + 1) % aPntsNb], myScaleUnits, myFTFont->WidthScaling());

      // process tags
      if (FT_CURVE_TAG(aTags[aPntId]) == FT_Curve_Tag_On)
      {
        if (!isLineSeg)
        {
          aPntLine1 = aPntCurr;
          isLineSeg = true;
          continue;
        }

        const gp_XY         aDirVec  = aPntCurr - aPntLine1;
        const Standard_Real aLen     = aDirVec.Modulus();
        if (aLen <= myPrecision)
        {
          aPntLine1 = aPntCurr;
          isLineSeg = true;
          continue;
        }

        if (myIsCompositeCurve)
        {
          Handle(Geom2d_TrimmedCurve) aLine = GCE2d_MakeSegment (gp_Pnt2d (aPntLine1), gp_Pnt2d (aPntCurr));
          myConcatMaker.Add (aLine, myPrecision);
        }
        else
        {
          Handle(Geom_Curve)  aCurve3d;
          Handle(Geom2d_Line) aCurve2d = new Geom2d_Line (gp_Pnt2d (aPntLine1), gp_Dir2d (aDirVec));
          if (to3d (aCurve2d, GeomAbs_C1, aCurve3d))
          {
            TopoDS_Edge anEdge = BRepLib_MakeEdge (aCurve3d, 0.0, aLen);
            myBuilder.UpdateEdge (anEdge, aCurve2d, mySurface, aLoc, myPrecision);
            aWireMaker.Add (anEdge);
          }
        }
        aPntLine1 = aPntCurr;
      }
      else if (FT_CURVE_TAG(aTags[aPntId]) == FT_Curve_Tag_Conic)
      {
        isLineSeg = false;
        gp_XY aPntPrev2 = aPntPrev;
        gp_XY aPntNext2 = aPntNext;

        // previous point is either the real previous point (an "on" point),
        // or the midpoint between the current one and the previous "conic off" point
        if (FT_CURVE_TAG(aTags[(aPntId - 1 + aPntsNb) % aPntsNb]) == FT_Curve_Tag_Conic)
        {
          aPntPrev2 = (aPntCurr + aPntPrev) * 0.5;
        }

        // next point is either the real next point or the midpoint
        if (FT_CURVE_TAG(aTags[(aPntId + 1) % aPntsNb]) == FT_Curve_Tag_Conic)
        {
          aPntNext2 = (aPntCurr + aPntNext) * 0.5;
        }

        my3Poles.SetValue (1, aPntPrev2);
        my3Poles.SetValue (2, aPntCurr);
        my3Poles.SetValue (3, aPntNext2);
        Handle(Geom2d_BezierCurve) aBezierArc = new Geom2d_BezierCurve (my3Poles);
        if (myIsCompositeCurve)
        {
          myConcatMaker.Add (aBezierArc, myPrecision);
        }
        else
        {
          Handle(Geom_Curve) aCurve3d;
          if (to3d (aBezierArc, GeomAbs_C1, aCurve3d))
          {
            TopoDS_Edge anEdge = BRepLib_MakeEdge (aCurve3d);
            myBuilder.UpdateEdge (anEdge, aBezierArc, mySurface, aLoc, myPrecision);
            aWireMaker.Add (anEdge);
          }
        }
      }
      else if (FT_CURVE_TAG(aTags[aPntId])                 == FT_Curve_Tag_Cubic
            && FT_CURVE_TAG(aTags[(aPntId + 1) % aPntsNb]) == FT_Curve_Tag_Cubic)
      {
        isLineSeg = false;
        my4Poles.SetValue (1, aPntPrev);
        my4Poles.SetValue (2, aPntCurr);
        my4Poles.SetValue (3, aPntNext);
        my4Poles.SetValue (4, gp_Pnt2d(readFTVec (aPntList[(aPntId + 2) % aPntsNb], myScaleUnits, myFTFont->WidthScaling())));
        Handle(Geom2d_BezierCurve) aBezier = new Geom2d_BezierCurve (my4Poles);
        if (myIsCompositeCurve)
        {
          myConcatMaker.Add (aBezier, myPrecision);
        }
        else
        {
          Handle(Geom_Curve) aCurve3d;
          if (to3d (aBezier, GeomAbs_C1, aCurve3d))
          {
            TopoDS_Edge anEdge = BRepLib_MakeEdge (aCurve3d);
            myBuilder.UpdateEdge (anEdge, aBezier, mySurface, aLoc, myPrecision);
            aWireMaker.Add (anEdge);
          }
        }
      }
    }

    if (myIsCompositeCurve)
    {
      Handle(Geom2d_BSplineCurve) aDraft2d = myConcatMaker.BSplineCurve();
      if (aDraft2d.IsNull())
      {
        continue;
      }

      const gp_Pnt2d aFirstPnt = aDraft2d->StartPoint();
      const gp_Pnt2d aLastPnt  = aDraft2d->EndPoint();
      if (!myFTFont->IsSingleStrokeFont()
       && !aFirstPnt.IsEqual (aLastPnt, myPrecision))
      {
        Handle(Geom2d_TrimmedCurve) aLine = GCE2d_MakeSegment (aLastPnt, aFirstPnt);
        myConcatMaker.Add (aLine, myPrecision);
      }

      Handle(Geom2d_BSplineCurve) aCurve2d = myConcatMaker.BSplineCurve();
      Handle(Geom_Curve)          aCurve3d;
      if (to3d (aCurve2d, GeomAbs_C0, aCurve3d))
      {
        TopoDS_Edge anEdge = BRepLib_MakeEdge (aCurve3d);
        myBuilder.UpdateEdge (anEdge, aCurve2d, mySurface, aLoc, myPrecision);
        aWireMaker.Add (anEdge);
      }
      myConcatMaker.Clear();
    }
    else
    {
      if (!aWireMaker.IsDone())
      {
        continue;
      }

      TopoDS_Vertex aFirstV, aLastV;
      TopExp::Vertices (aWireMaker.Wire(), aFirstV, aLastV);
      gp_Pnt aFirstPoint = BRep_Tool::Pnt (aFirstV);
      gp_Pnt aLastPoint  = BRep_Tool::Pnt (aLastV);
      if (!myFTFont->IsSingleStrokeFont()
       && !aFirstPoint.IsEqual (aLastPoint, myPrecision))
      {
        aWireMaker.Add (BRepLib_MakeEdge (aFirstV, aLastV));
      }
    }

    if (!aWireMaker.IsDone())
    {
      continue;
    }

    TopoDS_Wire aWireDraft = aWireMaker.Wire();
    if (!myFTFont->IsSingleStrokeFont())
    {
      // collect all wires and set CCW orientation
      TopoDS_Face aFace;
      myBuilder.MakeFace (aFace, mySurface, myPrecision);
      myBuilder.Add (aFace, aWireDraft);
      BRepTopAdaptor_FClass2d aClass2d (aFace, ::Precision::PConfusion());
      TopAbs_State aState = aClass2d.PerformInfinitePoint();
      if (aState != TopAbs_OUT)
      {
        // need to reverse
        aWireDraft.Reverse();
      }
      aWires.Append (aWireDraft);
    }
    else
    {
      if (aFaceCompDraft.IsNull())
      {
        myBuilder.MakeCompound (aFaceCompDraft);
      }
      myBuilder.Add (aFaceCompDraft, aWireDraft);
    }
  }

  if (!aWires.IsEmpty())
  {
    buildFaces (aWires, theShape);
  }
  else if (!aFaceCompDraft.IsNull())
  {
    theShape = aFaceCompDraft;
  }
#else
  (void )theChar;
#endif
  myCache.Bind (theChar, theShape);
  return !theShape.IsNull();
}
