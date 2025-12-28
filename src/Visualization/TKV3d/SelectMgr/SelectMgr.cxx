// Copyright (c) 2020 OPEN CASCADE SAS
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

#include <SelectMgr.hxx>

#include <Geom_Circle.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Poly_Triangulation.hxx>
#include <Prs3d.hxx>
#include <Prs3d_LineAspect.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Select3D_SensitiveBox.hxx>
#include <Select3D_SensitiveCircle.hxx>
#include <Select3D_SensitiveCylinder.hxx>
#include <Select3D_SensitiveEntity.hxx>
#include <Select3D_SensitiveFace.hxx>
#include <Select3D_SensitivePoint.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <Select3D_SensitiveTriangle.hxx>
#include <Select3D_SensitiveTriangulation.hxx>
#include <Select3D_SensitiveWire.hxx>
#include <SelectMgr_Selection.hxx>

namespace
{
//! Compute polyline of shrunk triangle.
static occ::handle<NCollection_HSequence<gp_Pnt>> shrunkTriangle(const gp_Pnt* thePnts,
                                                                 const gp_XYZ& theCenter)
{
  const gp_XYZ                               aV1 = theCenter + (thePnts[0].XYZ() - theCenter) * 0.9;
  const gp_XYZ                               aV2 = theCenter + (thePnts[1].XYZ() - theCenter) * 0.9;
  const gp_XYZ                               aV3 = theCenter + (thePnts[2].XYZ() - theCenter) * 0.9;
  occ::handle<NCollection_HSequence<gp_Pnt>> aPoints = new NCollection_HSequence<gp_Pnt>();
  aPoints->Append(aV1);
  aPoints->Append(aV2);
  aPoints->Append(aV3);
  aPoints->Append(aV1);
  return aPoints;
}

//! Fill in triangulation polylines.
static void addTriangulation(
  NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>>& theSeqLines,
  NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>>& theSeqFree,
  const occ::handle<Select3D_SensitiveTriangulation>&           theTri,
  const gp_Trsf&                                                theLoc)
{
  gp_Trsf aTrsf = theLoc;
  if (theTri->HasInitLocation())
  {
    aTrsf = theLoc * theTri->GetInitLocation();
  }
  const occ::handle<Poly_Triangulation>& aPolyTri = theTri->Triangulation();
  for (int aTriIter = 1; aTriIter <= aPolyTri->NbTriangles(); ++aTriIter)
  {
    const Poly_Triangle& aTri     = aPolyTri->Triangle(aTriIter);
    const gp_Pnt         aPnts[3] = {aPolyTri->Node(aTri(1)).Transformed(aTrsf),
                                     aPolyTri->Node(aTri(2)).Transformed(aTrsf),
                                     aPolyTri->Node(aTri(3)).Transformed(aTrsf)};
    const gp_XYZ         aCenter  = (aPnts[0].XYZ() + aPnts[1].XYZ() + aPnts[2].XYZ()) / 3.0;
    theSeqLines.Append(shrunkTriangle(aPnts, aCenter));
  }

  occ::handle<NCollection_HSequence<gp_Pnt>> aPoints = new NCollection_HSequence<gp_Pnt>();
  Prs3d::AddFreeEdges(*aPoints, aPolyTri, aTrsf);
  if (!aPoints->IsEmpty())
  {
    theSeqFree.Append(aPoints);
  }
}

//! Fill in bounding box polylines.
static void addBoundingBox(
  NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>>& theSeqLines,
  const occ::handle<Select3D_SensitiveBox>&                     theSensBox,
  const gp_Trsf&                                                theLoc)
{
  NCollection_Vec3<double> aMin, aMax;
  theSensBox->Box().Get(aMin.x(), aMin.y(), aMin.z(), aMax.x(), aMax.y(), aMax.z());
  gp_Pnt aPnts[8] = {gp_Pnt(aMin.x(), aMin.y(), aMin.z()),
                     gp_Pnt(aMax.x(), aMin.y(), aMin.z()),
                     gp_Pnt(aMax.x(), aMax.y(), aMin.z()),
                     gp_Pnt(aMin.x(), aMax.y(), aMin.z()),
                     gp_Pnt(aMin.x(), aMin.y(), aMax.z()),
                     gp_Pnt(aMax.x(), aMin.y(), aMax.z()),
                     gp_Pnt(aMax.x(), aMax.y(), aMax.z()),
                     gp_Pnt(aMin.x(), aMax.y(), aMax.z())};
  for (int aPntIter = 0; aPntIter <= 7; ++aPntIter)
  {
    aPnts[aPntIter].Transform(theLoc);
  }

  {
    occ::handle<NCollection_HSequence<gp_Pnt>> aPoints = new NCollection_HSequence<gp_Pnt>();
    for (int i = 0; i < 4; ++i)
    {
      aPoints->Append(aPnts[i]);
    }
    aPoints->Append(aPnts[0]);
    theSeqLines.Append(aPoints);
  }
  {
    occ::handle<NCollection_HSequence<gp_Pnt>> aPoints = new NCollection_HSequence<gp_Pnt>();
    for (int i = 4; i < 8; i++)
    {
      aPoints->Append(aPnts[i]);
    }
    aPoints->Append(aPnts[4]);
    theSeqLines.Append(aPoints);
  }
  for (int i = 0; i < 4; i++)
  {
    occ::handle<NCollection_HSequence<gp_Pnt>> aPoints = new NCollection_HSequence<gp_Pnt>();
    aPoints->Append(aPnts[i]);
    aPoints->Append(aPnts[i + 4]);
    theSeqLines.Append(aPoints);
  }
}

//! Fill in circle polylines.
static void addCircle(NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>>& theSeqLines,
                      const double                                                  theRadius,
                      const gp_Trsf&                                                theTrsf,
                      const double                                                  theHeight = 0)
{
  const double anUStep = 0.1;
  gp_XYZ       aVec(0, 0, theHeight);

  occ::handle<NCollection_HSequence<gp_Pnt>> aPoints = new NCollection_HSequence<gp_Pnt>();
  Geom_Circle                                aGeom(gp_Ax2(), theRadius);
  for (double anU = 0.0f; anU < (2.0 * M_PI + anUStep); anU += anUStep)
  {
    gp_Pnt aCircPnt = aGeom.Value(anU).Coord() + aVec;
    aCircPnt.Transform(theTrsf);
    aPoints->Append(aCircPnt);
  }
  theSeqLines.Append(aPoints);
}

//! Fill in cylinder polylines.
static void addCylinder(NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>>& theSeqLines,
                        const occ::handle<Select3D_SensitiveCylinder>&                theSensCyl,
                        const gp_Trsf&                                                theLoc)
{
  occ::handle<NCollection_HSequence<gp_Pnt>> aVertLine1 = new NCollection_HSequence<gp_Pnt>();
  occ::handle<NCollection_HSequence<gp_Pnt>> aVertLine2 = new NCollection_HSequence<gp_Pnt>();

  const gp_Trsf& aTrsf   = theLoc.Multiplied(theSensCyl->Transformation());
  const double   aHeight = theSensCyl->Height();

  for (int aCircNum = 0; aCircNum < 3; aCircNum++)
  {
    double aRadius =
      0.5 * (2 - aCircNum) * theSensCyl->BottomRadius() + 0.5 * aCircNum * theSensCyl->TopRadius();
    const gp_XYZ aVec(0, 0, aHeight * 0.5 * aCircNum);

    if (aCircNum != 1)
    {
      aVertLine1->Append(gp_Pnt(gp_XYZ(aRadius, 0, 0) + aVec).Transformed(aTrsf));
      aVertLine2->Append(gp_Pnt(gp_XYZ(-aRadius, 0, 0) + aVec).Transformed(aTrsf));
    }

    if (aRadius > Precision::Confusion())
    {
      addCircle(theSeqLines, aRadius, aTrsf, aVec.Z());
    }
  }
  theSeqLines.Append(aVertLine1);
  theSeqLines.Append(aVertLine2);
}
} // namespace

//=================================================================================================

void SelectMgr::ComputeSensitivePrs(const occ::handle<Graphic3d_Structure>&     thePrs,
                                    const occ::handle<SelectMgr_Selection>&     theSel,
                                    const gp_Trsf&                              theLoc,
                                    const occ::handle<Graphic3d_TransformPers>& theTrsfPers)
{
  thePrs->SetTransformPersistence(theTrsfPers);

  NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>> aSeqLines, aSeqFree;
  NCollection_Sequence<gp_Pnt>                                 aSeqPoints;
  for (NCollection_Vector<occ::handle<SelectMgr_SensitiveEntity>>::Iterator aSelEntIter(
         theSel->Entities());
       aSelEntIter.More();
       aSelEntIter.Next())
  {
    const occ::handle<Select3D_SensitiveEntity>& anEnt = aSelEntIter.Value()->BaseSensitive();
    if (occ::handle<Select3D_SensitiveBox> aSensBox = occ::down_cast<Select3D_SensitiveBox>(anEnt))
    {
      addBoundingBox(aSeqLines, aSensBox, theLoc);
    }
    else if (occ::handle<Select3D_SensitiveCylinder> aSensCyl =
               occ::down_cast<Select3D_SensitiveCylinder>(anEnt))
    {
      addCylinder(aSeqLines, aSensCyl, theLoc);
    }
    else if (occ::handle<Select3D_SensitiveCircle> aSensCircle =
               occ::down_cast<Select3D_SensitiveCircle>(anEnt))
    {
      addCircle(aSeqLines, aSensCircle->Radius(), theLoc.Multiplied(aSensCircle->Transformation()));
    }
    else if (occ::handle<Select3D_SensitiveFace> aFace =
               occ::down_cast<Select3D_SensitiveFace>(anEnt))
    {
      occ::handle<NCollection_HArray1<gp_Pnt>> aSensPnts;
      aFace->GetPoints(aSensPnts);
      occ::handle<NCollection_HSequence<gp_Pnt>> aPoints = new NCollection_HSequence<gp_Pnt>();
      for (NCollection_HArray1<gp_Pnt>::Iterator aPntIter(*aSensPnts); aPntIter.More();
           aPntIter.Next())
      {
        aPoints->Append(aPntIter.Value().Transformed(theLoc));
      }
      aSeqLines.Append(aPoints);
    }
    else if (occ::handle<Select3D_SensitivePoly> aSensPoly =
               occ::down_cast<Select3D_SensitivePoly>(anEnt))
    {
      int aFrom = 0, aTo = 0;
      aSensPoly->ArrayBounds(aFrom, aTo);
      occ::handle<NCollection_HSequence<gp_Pnt>> aPoints = new NCollection_HSequence<gp_Pnt>();
      for (int aPntIter = aFrom; aPntIter <= aTo; ++aPntIter)
      {
        aPoints->Append(aSensPoly->GetPoint3d(aPntIter).Transformed(theLoc));
      }
      aSeqLines.Append(aPoints);
    }
    else if (occ::handle<Select3D_SensitiveWire> aWire =
               occ::down_cast<Select3D_SensitiveWire>(anEnt))
    {
      const NCollection_Vector<occ::handle<Select3D_SensitiveEntity>>& anEntities =
        aWire->GetEdges();
      for (NCollection_Vector<occ::handle<Select3D_SensitiveEntity>>::Iterator aSubIter(anEntities);
           aSubIter.More();
           aSubIter.Next())
      {
        const occ::handle<Select3D_SensitiveEntity>& aSubEnt = aSubIter.Value();
        if (occ::handle<Select3D_SensitiveSegment> aSensSeg =
              occ::down_cast<Select3D_SensitiveSegment>(aSubEnt))
        {
          occ::handle<NCollection_HSequence<gp_Pnt>> aPoints = new NCollection_HSequence<gp_Pnt>();
          aPoints->Append(aSensSeg->StartPoint().Transformed(theLoc));
          aPoints->Append(aSensSeg->EndPoint().Transformed(theLoc));
          aSeqLines.Append(aPoints);
        }
        else if (occ::handle<Select3D_SensitivePoly> aSubSensPoly =
                   occ::down_cast<Select3D_SensitivePoly>(aSubEnt))
        {
          int aFrom = 0, aTo = 0;
          aSubSensPoly->ArrayBounds(aFrom, aTo);
          occ::handle<NCollection_HSequence<gp_Pnt>> aPoints = new NCollection_HSequence<gp_Pnt>();
          for (int aPntIter = aFrom; aPntIter <= aTo; ++aPntIter)
          {
            aPoints->Append(aSubSensPoly->GetPoint3d(aPntIter).Transformed(theLoc));
          }
          aSeqLines.Append(aPoints);
        }
      }
    }
    else if (occ::handle<Select3D_SensitiveSegment> aSensSeg =
               occ::down_cast<Select3D_SensitiveSegment>(anEnt))
    {
      occ::handle<NCollection_HSequence<gp_Pnt>> aPoints = new NCollection_HSequence<gp_Pnt>();
      aPoints->Append(aSensSeg->StartPoint().Transformed(theLoc));
      aPoints->Append(aSensSeg->EndPoint().Transformed(theLoc));
      aSeqLines.Append(aPoints);
    }
    else if (occ::handle<Select3D_SensitivePoint> aSensPnt =
               occ::down_cast<Select3D_SensitivePoint>(anEnt))
    {
      aSeqPoints.Append(aSensPnt->Point().Transformed(theLoc));
    }
    else if (occ::handle<Select3D_SensitiveTriangulation> aSensTri =
               occ::down_cast<Select3D_SensitiveTriangulation>(anEnt))
    {
      addTriangulation(aSeqLines, aSeqFree, aSensTri, theLoc);
    }
    else if (occ::handle<Select3D_SensitiveTriangle> aSensTri1 =
               occ::down_cast<Select3D_SensitiveTriangle>(anEnt))
    {
      gp_Pnt aPnts[3];
      aSensTri1->Points3D(aPnts[0], aPnts[1], aPnts[2]);
      aPnts[0].Transform(theLoc);
      aPnts[1].Transform(theLoc);
      aPnts[2].Transform(theLoc);
      aSeqLines.Append(shrunkTriangle(aPnts, aSensTri1->Center3D().XYZ()));
    }
  }

  if (!aSeqPoints.IsEmpty())
  {
    occ::handle<Graphic3d_ArrayOfPoints> anArrayOfPoints =
      new Graphic3d_ArrayOfPoints(aSeqPoints.Size());
    for (NCollection_Sequence<gp_Pnt>::Iterator aPntIter(aSeqPoints); aPntIter.More();
         aPntIter.Next())
    {
      anArrayOfPoints->AddVertex(aPntIter.Value());
    }

    occ::handle<Graphic3d_Group> aSensPntGroup = thePrs->NewGroup();
    aSensPntGroup->SetPrimitivesAspect(
      new Graphic3d_AspectMarker3d(Aspect_TOM_O_PLUS, Quantity_NOC_INDIANRED3, 2.0));
    aSensPntGroup->AddPrimitiveArray(anArrayOfPoints);
  }
  if (!aSeqLines.IsEmpty())
  {
    Prs3d::AddPrimitivesGroup(thePrs,
                              new Prs3d_LineAspect(Quantity_NOC_AQUAMARINE1, Aspect_TOL_DASH, 1.0),
                              aSeqLines);
  }
  if (!aSeqFree.IsEmpty())
  {
    Prs3d::AddPrimitivesGroup(thePrs,
                              new Prs3d_LineAspect(Quantity_NOC_GREEN, Aspect_TOL_SOLID, 2.0),
                              aSeqFree);
  }
}
