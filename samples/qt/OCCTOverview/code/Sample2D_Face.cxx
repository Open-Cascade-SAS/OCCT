// Copyright (c) 2020 OPEN CASCADE SAS
//
// This file is part of the examples of the Open CASCADE Technology software library.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

#include "Sample2D_Face.h"

#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <GeomLib.hxx>
#include <Select3D_SensitiveGroup.hxx>
#include <Select3D_SensitiveCurve.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

Sample2D_Face::Sample2D_Face (const TopoDS_Shape& theFace)
: myFORWARDColor  (Quantity_NOC_BLUE1),
  myREVERSEDColor (Quantity_NOC_YELLOW),
  myINTERNALColor (Quantity_NOC_RED1),
  myEXTERNALColor (Quantity_NOC_MAGENTA1),
  myWidthIndex (1),
  myTypeIndex  (1),
  //
  myshape (theFace),
  myForwardNum  (0),
  myReversedNum (0),
  myInternalNum (0),
  myExternalNum (0),
  //
  myForwardBounds  (0),
  myReversedBounds (0),
  myInternalBounds (0),
  myExternalBounds (0)
{
  SetAutoHilight(Standard_False);
  FillData(Standard_True);
}

void Sample2D_Face::DrawMarker (const Handle(Geom2d_TrimmedCurve)& theCurve,
                                const Handle(Prs3d_Presentation)& thePresentation)
{
  Standard_Real aCenterParam = (theCurve->FirstParameter() + theCurve->LastParameter()) / 2;
  gp_Pnt2d p;
  gp_Vec2d v;
  theCurve->D1(aCenterParam, p, v);
  if (v.Magnitude() > gp::Resolution())
  {
    gp_Vec aDir(v.X(), v.Y(), 0.);
    gp_Pnt aPoint(p.X(), p.Y(), 0.);
    aDir.Normalize();
    aDir.Reverse();
    gp_Dir aZ(0, 0, 1);
    gp_Pnt aLeft (aPoint.Translated(aDir.Rotated(gp_Ax1(aPoint, aZ), M_PI / 6) * 5));
    gp_Pnt aRight(aPoint.Translated(aDir.Rotated(gp_Ax1(aPoint, aZ), M_PI * 11 / 6) * 5));

    Handle(Graphic3d_ArrayOfPolylines) anArrow = new Graphic3d_ArrayOfPolylines(3);
    anArrow->AddVertex(aLeft);
    anArrow->AddVertex(aPoint);
    anArrow->AddVertex(aRight);

    thePresentation->CurrentGroup()->AddPrimitiveArray(anArrow);
  }
}

void Sample2D_Face::FillData(Standard_Boolean isSizesRecompute)
{
  if (myshape.IsNull() || myshape.ShapeType() != TopAbs_FACE)
  {
    return;
  }

  Standard_Real f, l;
  TopoDS_Face aFace = TopoDS::Face(myshape);

  // count number of vertices and bounds in primitive arrays
  if (isSizesRecompute)
  {
    mySeq_FORWARD.Clear();
    mySeq_REVERSED.Clear();
    mySeq_INTERNAL.Clear();
    mySeq_EXTERNAL.Clear();

    myshape.Orientation(TopAbs_FORWARD);
    for (TopExp_Explorer anEdgeIter (myshape, TopAbs_EDGE); anEdgeIter.More(); anEdgeIter.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge (anEdgeIter.Current());
      BRepAdaptor_Curve2d aCurveOnEdge (anEdge, aFace);
      GCPnts_QuasiUniformDeflection anEdgeDistrib(aCurveOnEdge, 1.e-2);
      if (!anEdgeDistrib.IsDone())
      {
        continue;
      }

      switch (anEdge.Orientation())
      {
        case TopAbs_FORWARD:
        {
          myForwardNum += anEdgeDistrib.NbPoints();
          myForwardBounds++;
          break;
        }
        case TopAbs_REVERSED:
        {
          myReversedNum += anEdgeDistrib.NbPoints();
          myReversedBounds++;
          break;
        }
        case TopAbs_INTERNAL:
        {
          myInternalNum += anEdgeDistrib.NbPoints();
          myInternalBounds++;
          break;
        }
        case TopAbs_EXTERNAL:
        {
          myExternalNum += anEdgeDistrib.NbPoints();
          myExternalBounds++;
          break;
        }
      }
    }
  }

  myForwardArray = new Graphic3d_ArrayOfPolylines(myForwardNum, myForwardBounds);
  myReversedArray = new Graphic3d_ArrayOfPolylines(myReversedNum, myReversedBounds);
  myInternalArray = new Graphic3d_ArrayOfPolylines(myInternalNum, myInternalBounds);
  myExternalArray = new Graphic3d_ArrayOfPolylines(myExternalNum, myExternalBounds);

  // fill primitive arrays
  for (TopExp_Explorer anEdgeIter (myshape, TopAbs_EDGE); anEdgeIter.More(); anEdgeIter.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge (anEdgeIter.Current());
    const Handle(Geom2d_Curve) aCurve = BRep_Tool::CurveOnSurface (anEdge, aFace, f, l);
    Handle(Geom2d_TrimmedCurve) aTrimmedCurve = new Geom2d_TrimmedCurve(aCurve, f, l);
    if (!aTrimmedCurve.IsNull())
    {
      Handle(Geom_Curve) aCurve3d = GeomLib::To3d(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aTrimmedCurve);
      BRepAdaptor_Curve2d aCurveOnEdge (anEdge, aFace);
      GCPnts_QuasiUniformDeflection anEdgeDistrib (aCurveOnEdge, 1.e-2);
      if (!anEdgeDistrib.IsDone())
      {
        continue;
      }

      switch (anEdge.Orientation())
      {
        case TopAbs_FORWARD:
        {
          myForwardArray->AddBound(anEdgeDistrib.NbPoints());
          for (Standard_Integer i = 1; i <= anEdgeDistrib.NbPoints(); ++i)
          {
            myForwardArray->AddVertex(anEdgeDistrib.Value(i));
          }
          if (isSizesRecompute)
          {
            mySeq_FORWARD.Append(aCurve3d);
          }
          break;
        }
        case TopAbs_REVERSED:
        {
          myReversedArray->AddBound(anEdgeDistrib.NbPoints());
          for (Standard_Integer i = 1; i <= anEdgeDistrib.NbPoints(); ++i)
          {
            myReversedArray->AddVertex(anEdgeDistrib.Value(i));
          }
          if (isSizesRecompute)
          {
            mySeq_REVERSED.Append(aCurve3d);
          }
          break;
        }
        case TopAbs_INTERNAL:
        {
          myInternalArray->AddBound(anEdgeDistrib.NbPoints());
          for (Standard_Integer i = 1; i <= anEdgeDistrib.NbPoints(); ++i)
          {
            myInternalArray->AddVertex(anEdgeDistrib.Value(i));
          }
          if (isSizesRecompute)
          {
            mySeq_INTERNAL.Append(aCurve3d);
          }
          break;
        }
        case TopAbs_EXTERNAL:
        {
          myExternalArray->AddBound(anEdgeDistrib.NbPoints());
          for (Standard_Integer i = 1; i <= anEdgeDistrib.NbPoints(); ++i)
          {
            myExternalArray->AddVertex(anEdgeDistrib.Value(i));
          }
          if (isSizesRecompute)
          {
            mySeq_EXTERNAL.Append(aCurve3d);
          }
          break;
        }
      }
    }
  }
}

void Sample2D_Face::Compute (const Handle(PrsMgr_PresentationManager)& ,
                             const Handle(Prs3d_Presentation)& thePresentation,
                             const Standard_Integer theMode)
{
  if (theMode != 0)
  {
    return;
  }

  thePresentation->Clear();
  myDrawer->SetWireDraw(1);

  if (myshape.IsNull() || myshape.ShapeType() != TopAbs_FACE)
  {
    return;
  }

  Handle(Graphic3d_AspectLine3d) aLineAspect_FORWARD  = new Graphic3d_AspectLine3d(myFORWARDColor,  Aspect_TOL_SOLID, 1);
  Handle(Graphic3d_AspectLine3d) aLineAspect_REVERSED = new Graphic3d_AspectLine3d(myREVERSEDColor, Aspect_TOL_SOLID, 1);
  Handle(Graphic3d_AspectLine3d) aLineAspect_INTERNAL = new Graphic3d_AspectLine3d(myINTERNALColor, Aspect_TOL_SOLID, 1);
  Handle(Graphic3d_AspectLine3d) aLineAspect_EXTERNAL = new Graphic3d_AspectLine3d(myEXTERNALColor, Aspect_TOL_SOLID, 1);

  Standard_Real f, l;
  TopoDS_Face aFace = TopoDS::Face(myshape);
  // estimating number of vertices in primitive arrays
  for (TopExp_Explorer anEdgeIter (myshape, TopAbs_EDGE); anEdgeIter.More(); anEdgeIter.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeIter.Current());
    const Handle(Geom2d_Curve) aCurve = BRep_Tool::CurveOnSurface (anEdge, aFace, f, l);

    Handle(Geom2d_TrimmedCurve) aTrimmedCurve = new Geom2d_TrimmedCurve(aCurve, f, l);
    // make a 3D curve from 2D trimmed curve to display it
    Handle(Geom_Curve) aCurve3d = GeomLib::To3d(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aTrimmedCurve);
    // make distribution of points
    BRepAdaptor_Curve2d aCurveOnEdge (anEdge, aFace);
    GCPnts_QuasiUniformDeflection anEdgeDistrib(aCurveOnEdge, 1.e-2);
    if (anEdgeDistrib.IsDone())
    {
      switch (anEdge.Orientation())
      {
        case TopAbs_FORWARD:
        {
          thePresentation->CurrentGroup()->SetPrimitivesAspect(aLineAspect_FORWARD);
          DrawMarker(aTrimmedCurve, thePresentation);
          break;
        }
        case TopAbs_REVERSED:
        {
          thePresentation->CurrentGroup()->SetPrimitivesAspect(aLineAspect_REVERSED);
          DrawMarker(aTrimmedCurve, thePresentation);
          break;
         }
        case TopAbs_INTERNAL:
        {
          thePresentation->CurrentGroup()->SetPrimitivesAspect(aLineAspect_INTERNAL);
          DrawMarker(aTrimmedCurve, thePresentation);

          mySeq_INTERNAL.Append(aCurve3d);
          break;
        }
        case TopAbs_EXTERNAL:
        {
          thePresentation->CurrentGroup()->SetPrimitivesAspect(aLineAspect_EXTERNAL);
          DrawMarker(aTrimmedCurve, thePresentation);
          break;
        }
      }
    }
  }

  // add all primitives to the presentation
  thePresentation->CurrentGroup()->SetPrimitivesAspect(aLineAspect_FORWARD);
  thePresentation->CurrentGroup()->AddPrimitiveArray(myForwardArray);

  thePresentation->CurrentGroup()->SetPrimitivesAspect(aLineAspect_REVERSED);
  thePresentation->CurrentGroup()->AddPrimitiveArray(myReversedArray);

  thePresentation->CurrentGroup()->SetPrimitivesAspect(aLineAspect_INTERNAL);
  thePresentation->CurrentGroup()->AddPrimitiveArray(myInternalArray);

  thePresentation->CurrentGroup()->SetPrimitivesAspect(aLineAspect_EXTERNAL);
  thePresentation->CurrentGroup()->AddPrimitiveArray(myExternalArray);
}

void Sample2D_Face::HilightSelected (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                     const SelectMgr_SequenceOfOwner& theOwners)
{
  Handle(Prs3d_Presentation) aSelectionPrs = GetSelectPresentation (thePrsMgr);

  Handle(Graphic3d_AspectLine3d) aLineAspect = new Graphic3d_AspectLine3d(Quantity_NOC_ANTIQUEWHITE, Aspect_TOL_SOLID, 2);
  if (HasPresentation())
  {
    aSelectionPrs->SetTransformPersistence(Presentation()->TransformPersistence());
  }

  const Standard_Integer aLength = theOwners.Length();
  aSelectionPrs->Clear();
  FillData();

  Handle(Graphic3d_Group) aSelectGroup = aSelectionPrs->NewGroup();

  for (Standard_Integer i = 1; i <= aLength; ++i)
  {
    Handle(SelectMgr_EntityOwner) anOwner = theOwners.Value(i);
    // check priority of owner to add primitives in one of array
    // containing primitives with certain type of orientation
    switch (anOwner->Priority())
    {
      case 7:
      {
        // add to objects with forward orientation
        aSelectGroup->SetGroupPrimitivesAspect(aLineAspect);
        aSelectGroup->AddPrimitiveArray(myForwardArray);
        break;
      }
      case 6:
      {
        // add to objects with reversed orientation
        aSelectGroup->SetGroupPrimitivesAspect(aLineAspect);
        aSelectGroup->AddPrimitiveArray(myReversedArray);
        break;
      }
      case 5:
      {
        // add to objects with internal orientation
        aSelectGroup->SetGroupPrimitivesAspect(aLineAspect);
        aSelectGroup->AddPrimitiveArray(myInternalArray);
        break;
      }
      case 4:
      {
        // add to objects with external orientation
        aSelectGroup->SetGroupPrimitivesAspect(aLineAspect);
        aSelectGroup->AddPrimitiveArray(myExternalArray);
        break;
      }
    }
  }
  aSelectionPrs->Display();
}

void Sample2D_Face::ClearSelected()
{
  if (Handle(Prs3d_Presentation) aSelectionPrs = GetSelectPresentation(NULL))
  {
    aSelectionPrs->Clear();
  }
}

void Sample2D_Face::HilightOwnerWithColor (const Handle(PrsMgr_PresentationManager)& thePM,
                                           const Handle(Prs3d_Drawer)& theStyle,
                                           const Handle(SelectMgr_EntityOwner)& theOwner)
{
  Handle(Prs3d_Presentation) aHighlightPrs = GetHilightPresentation(thePM);
  if (HasPresentation())
  {
    aHighlightPrs->SetTransformPersistence(Presentation()->TransformPersistence());
  }
  if (theOwner.IsNull())
  {
    return;
  }

  aHighlightPrs->Clear();
  FillData();

  // Direct highlighting
  aHighlightPrs->NewGroup();
  Handle(Graphic3d_Group) aHilightGroup = aHighlightPrs->CurrentGroup();
  Handle(Graphic3d_AspectLine3d) aLineAspect = new Graphic3d_AspectLine3d(theStyle->Color(), Aspect_TOL_SOLID, 2);
  switch (theOwner->Priority())
  {
    case 7:
    {
      aHilightGroup->SetGroupPrimitivesAspect(aLineAspect);
      aHilightGroup->AddPrimitiveArray(myForwardArray);
      break;
    }
    case 6:
    {
      aHilightGroup->SetGroupPrimitivesAspect(aLineAspect);
      aHilightGroup->AddPrimitiveArray(myReversedArray);
      break;
    }
    case 5:
    {
      aHilightGroup->SetGroupPrimitivesAspect(aLineAspect);
      aHilightGroup->AddPrimitiveArray(myInternalArray);
      break;
    }
    case 4:
    {
      aHilightGroup->SetGroupPrimitivesAspect(aLineAspect);
      aHilightGroup->AddPrimitiveArray(myExternalArray);
      break;
    }
  }
  if (thePM->IsImmediateModeOn())
  {
    thePM->AddToImmediateList(aHighlightPrs);
  }
}

void Sample2D_Face::ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                                      const Standard_Integer theMode)
{
  if (myshape.IsNull()
   || theMode != 0)
  {
    return;
  }

  if (mySeq_FORWARD.IsEmpty()
   && mySeq_REVERSED.IsEmpty()
   && mySeq_INTERNAL.IsEmpty()
   && mySeq_EXTERNAL.IsEmpty())
  {
    return;
  }

  // create entity owner for every part of the face
  // set different priorities for primitives of different orientation
  Handle(SelectMgr_EntityOwner) anOwner_Forward  = new SelectMgr_EntityOwner(this, 7);
  Handle(SelectMgr_EntityOwner) anOwner_Reversed = new SelectMgr_EntityOwner(this, 6);
  Handle(SelectMgr_EntityOwner) anOwner_Internal = new SelectMgr_EntityOwner(this, 5);
  Handle(SelectMgr_EntityOwner) anOwner_External = new SelectMgr_EntityOwner(this, 4);

  // create a sensitive for every part
  Handle(Select3D_SensitiveGroup) aForwardGroup  = new Select3D_SensitiveGroup(anOwner_Forward);
  Handle(Select3D_SensitiveGroup) aReversedGroup = new Select3D_SensitiveGroup(anOwner_Reversed);
  Handle(Select3D_SensitiveGroup) aInternalGroup = new Select3D_SensitiveGroup(anOwner_Internal);
  Handle(Select3D_SensitiveGroup) aExternalGroup = new Select3D_SensitiveGroup(anOwner_External);

  Standard_Integer aLength = mySeq_FORWARD.Length();
  for (Standard_Integer i = 1; i <= aLength; ++i)
  {
    Handle(Select3D_SensitiveCurve) aSensitveCurve = new Select3D_SensitiveCurve(anOwner_Forward, mySeq_FORWARD(i));
    aForwardGroup->Add(aSensitveCurve);
  }
  theSelection->Add(aForwardGroup);

  aLength = mySeq_REVERSED.Length();
  for (Standard_Integer i = 1; i <= aLength; ++i)
  {
    Handle(Select3D_SensitiveCurve) aSensitveCurve = new Select3D_SensitiveCurve(anOwner_Reversed, mySeq_REVERSED(i));
    aReversedGroup->Add(aSensitveCurve);
  }
  theSelection->Add(aReversedGroup);

  aLength = mySeq_INTERNAL.Length();
  for (Standard_Integer i = 1; i <= aLength; ++i)
  {
    Handle(Select3D_SensitiveCurve) aSensitveCurve = new Select3D_SensitiveCurve(anOwner_Internal, mySeq_INTERNAL(i));
    aInternalGroup->Add(aSensitveCurve);
  }
  theSelection->Add(aInternalGroup);

  aLength = mySeq_EXTERNAL.Length();
  for (Standard_Integer i = 1; i <= aLength; ++i)
  {
    Handle(Select3D_SensitiveCurve) aSensitveCurve = new Select3D_SensitiveCurve(anOwner_External, mySeq_EXTERNAL(i));
    aExternalGroup->Add(aSensitveCurve);
  }
  theSelection->Add(aExternalGroup);
}
