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

#include <Graphic3d_ArrayOfPoints.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Structure.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <Poly_Triangulation.hxx>
#include <Prs3d.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_NListOfSequenceOfPnt.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <Select3D_SensitiveBox.hxx>
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
  static Handle(TColgp_HSequenceOfPnt) shrunkTriangle (const gp_Pnt* thePnts,
                                                       const gp_XYZ& theCenter)
  {
    const gp_XYZ aV1 = theCenter + (thePnts[0].XYZ() - theCenter) * 0.9;
    const gp_XYZ aV2 = theCenter + (thePnts[1].XYZ() - theCenter) * 0.9;
    const gp_XYZ aV3 = theCenter + (thePnts[2].XYZ() - theCenter) * 0.9;
    Handle(TColgp_HSequenceOfPnt) aPoints = new TColgp_HSequenceOfPnt();
    aPoints->Append (aV1);
    aPoints->Append (aV2);
    aPoints->Append (aV3);
    aPoints->Append (aV1);
    return aPoints;
  }

  //! Fill in triangulation polylines.
  static void addTriangulation (Prs3d_NListOfSequenceOfPnt& theSeqLines,
                                Prs3d_NListOfSequenceOfPnt& theSeqFree,
                                const Handle(Select3D_SensitiveTriangulation)& theTri,
                                const gp_Trsf& theLoc)
  {
    gp_Trsf aTrsf = theLoc;
    if (theTri->HasInitLocation())
    {
      aTrsf = theLoc * theTri->GetInitLocation();
    }
    const Handle(Poly_Triangulation)& aPolyTri = theTri->Triangulation();
    for (Standard_Integer aTriIter = 1; aTriIter <= aPolyTri->NbTriangles(); ++aTriIter)
    {
      const Poly_Triangle& aTri = aPolyTri->Triangle (aTriIter);
      const gp_Pnt aPnts[3] =
      {
        aPolyTri->Node (aTri (1)).Transformed (aTrsf),
        aPolyTri->Node (aTri (2)).Transformed (aTrsf),
        aPolyTri->Node (aTri (3)).Transformed (aTrsf)
      };
      const gp_XYZ aCenter = (aPnts[0].XYZ() + aPnts[1].XYZ() + aPnts[2].XYZ()) / 3.0;
      theSeqLines.Append (shrunkTriangle (aPnts, aCenter));
    }

    Handle(TColgp_HSequenceOfPnt) aPoints = new TColgp_HSequenceOfPnt();
    Prs3d::AddFreeEdges (*aPoints, aPolyTri, aTrsf);
    if (!aPoints->IsEmpty())
    {
      theSeqFree.Append (aPoints);
    }
  }

  //! Fill in bounding box polylines.
  static void addBoundingBox (Prs3d_NListOfSequenceOfPnt& theSeqLines,
                              const Handle(Select3D_SensitiveBox)& theSensBox,
                              const gp_Trsf& theLoc)
  {
    Graphic3d_Vec3d aMin, aMax;
    theSensBox->Box().Get (aMin.x(), aMin.y(), aMin.z(), aMax.x(), aMax.y(), aMax.z());
    gp_Pnt aPnts[8] =
    {
      gp_Pnt(aMin.x(), aMin.y(), aMin.z()),
      gp_Pnt(aMax.x(), aMin.y(), aMin.z()),
      gp_Pnt(aMax.x(), aMax.y(), aMin.z()),
      gp_Pnt(aMin.x(), aMax.y(), aMin.z()),
      gp_Pnt(aMin.x(), aMin.y(), aMax.z()),
      gp_Pnt(aMax.x(), aMin.y(), aMax.z()),
      gp_Pnt(aMax.x(), aMax.y(), aMax.z()),
      gp_Pnt(aMin.x(), aMax.y(), aMax.z())
    };
    for (Standard_Integer aPntIter = 0; aPntIter <= 7; ++aPntIter)
    {
      aPnts[aPntIter].Transform (theLoc);
    }

    {
      Handle(TColgp_HSequenceOfPnt) aPoints = new TColgp_HSequenceOfPnt();
      for (Standard_Integer i = 0; i < 4; ++i)
      {
        aPoints->Append (aPnts[i]);
      }
      aPoints->Append (aPnts[0]);
      theSeqLines.Append (aPoints);
    }
    {
      Handle(TColgp_HSequenceOfPnt) aPoints = new TColgp_HSequenceOfPnt();
      for (Standard_Integer i = 4; i < 8; i++)
      {
        aPoints->Append (aPnts[i]);
      }
      aPoints->Append (aPnts[4]);
      theSeqLines.Append (aPoints);
    }
    for (Standard_Integer i = 0; i < 4; i++)
    {
      Handle(TColgp_HSequenceOfPnt) aPoints = new TColgp_HSequenceOfPnt();
      aPoints->Append (aPnts[i]);
      aPoints->Append (aPnts[i+4]);
      theSeqLines.Append (aPoints);
    }
  }
}

//=======================================================================
//function : ComputeSensitivePrs
//purpose  :
//=======================================================================
void SelectMgr::ComputeSensitivePrs (const Handle(Graphic3d_Structure)& thePrs,
                                     const Handle(SelectMgr_Selection)& theSel,
                                     const gp_Trsf& theLoc,
                                     const Handle(Graphic3d_TransformPers)& theTrsfPers)
{
  thePrs->SetTransformPersistence (theTrsfPers);

  Prs3d_NListOfSequenceOfPnt aSeqLines, aSeqFree;
  TColgp_SequenceOfPnt aSeqPoints;
  for (NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>::Iterator aSelEntIter (theSel->Entities()); aSelEntIter.More(); aSelEntIter.Next())
  {
    const Handle(Select3D_SensitiveEntity)& anEnt = aSelEntIter.Value()->BaseSensitive();
    if (Handle(Select3D_SensitiveBox) aSensBox = Handle(Select3D_SensitiveBox)::DownCast (anEnt))
    {
      addBoundingBox (aSeqLines, aSensBox, theLoc);
    }
    else if (Handle(Select3D_SensitiveFace) aFace = Handle(Select3D_SensitiveFace)::DownCast(anEnt))
    {
      Handle(TColgp_HArray1OfPnt) aSensPnts;
      aFace->GetPoints (aSensPnts);
      Handle(TColgp_HSequenceOfPnt) aPoints = new TColgp_HSequenceOfPnt();
      for (TColgp_HArray1OfPnt::Iterator aPntIter (*aSensPnts); aPntIter.More(); aPntIter.Next())
      {
        aPoints->Append (aPntIter.Value().Transformed (theLoc));
      }
      aSeqLines.Append (aPoints);
    }
    else if (Handle(Select3D_SensitivePoly) aSensPoly = Handle(Select3D_SensitivePoly)::DownCast (anEnt))
    {
      Standard_Integer aFrom = 0, aTo = 0;
      aSensPoly->ArrayBounds (aFrom, aTo);
      Handle(TColgp_HSequenceOfPnt) aPoints = new TColgp_HSequenceOfPnt();
      for (Standard_Integer aPntIter = aFrom; aPntIter <= aTo; ++aPntIter)
      {
        aPoints->Append (aSensPoly->GetPoint3d (aPntIter).Transformed (theLoc));
      }
      aSeqLines.Append (aPoints);
    }
    else if (Handle(Select3D_SensitiveWire) aWire = Handle(Select3D_SensitiveWire)::DownCast(anEnt))
    {
      const NCollection_Vector<Handle(Select3D_SensitiveEntity)>& anEntities = aWire->GetEdges();
      for (NCollection_Vector<Handle(Select3D_SensitiveEntity)>::Iterator aSubIter (anEntities); aSubIter.More(); aSubIter.Next())
      {
        const Handle(Select3D_SensitiveEntity)& aSubEnt = aSubIter.Value();
        if (Handle(Select3D_SensitiveSegment) aSensSeg = Handle(Select3D_SensitiveSegment)::DownCast (aSubEnt))
        {
          Handle(TColgp_HSequenceOfPnt) aPoints = new TColgp_HSequenceOfPnt();
          aPoints->Append (aSensSeg->StartPoint().Transformed (theLoc));
          aPoints->Append (aSensSeg->EndPoint()  .Transformed (theLoc));
          aSeqLines.Append (aPoints);
        }
        else if (Handle(Select3D_SensitivePoly) aSubSensPoly = Handle(Select3D_SensitivePoly)::DownCast (aSubEnt))
        {
          Standard_Integer aFrom = 0, aTo = 0;
          aSubSensPoly->ArrayBounds (aFrom, aTo);
          Handle(TColgp_HSequenceOfPnt) aPoints = new TColgp_HSequenceOfPnt();
          for (Standard_Integer aPntIter = aFrom; aPntIter <= aTo; ++aPntIter)
          {
            aPoints->Append (aSubSensPoly->GetPoint3d (aPntIter).Transformed (theLoc));
          }
          aSeqLines.Append (aPoints);
        }
      }
    }
    else if (Handle(Select3D_SensitiveSegment) aSensSeg = Handle(Select3D_SensitiveSegment)::DownCast(anEnt))
    {
      Handle(TColgp_HSequenceOfPnt) aPoints = new TColgp_HSequenceOfPnt();
      aPoints->Append (aSensSeg->StartPoint().Transformed (theLoc));
      aPoints->Append (aSensSeg->EndPoint()  .Transformed (theLoc));
      aSeqLines.Append (aPoints);
    }
    else if (Handle(Select3D_SensitivePoint) aSensPnt = Handle(Select3D_SensitivePoint)::DownCast(anEnt))
    {
      aSeqPoints.Append (aSensPnt->Point().Transformed (theLoc));
    }
    else if (Handle(Select3D_SensitiveTriangulation) aSensTri = Handle(Select3D_SensitiveTriangulation)::DownCast (anEnt))
    {
      addTriangulation (aSeqLines, aSeqFree, aSensTri, theLoc);
    }
    else if (Handle(Select3D_SensitiveTriangle) aSensTri1 = Handle(Select3D_SensitiveTriangle)::DownCast(anEnt))
    {
      gp_Pnt aPnts[3];
      aSensTri1->Points3D (aPnts[0], aPnts[1], aPnts[2]);
      aPnts[0].Transform (theLoc);
      aPnts[1].Transform (theLoc);
      aPnts[2].Transform (theLoc);
      aSeqLines.Append (shrunkTriangle (aPnts, aSensTri1->Center3D().XYZ()));
    }
  }

  if (!aSeqPoints.IsEmpty())
  {
    Handle(Graphic3d_ArrayOfPoints) anArrayOfPoints = new Graphic3d_ArrayOfPoints (aSeqPoints.Size());
    for (TColgp_SequenceOfPnt::Iterator aPntIter (aSeqPoints); aPntIter.More(); aPntIter.Next())
    {
      anArrayOfPoints->AddVertex (aPntIter.Value());
    }

    Handle(Graphic3d_Group) aSensPntGroup = thePrs->NewGroup();
    aSensPntGroup->SetPrimitivesAspect (new Graphic3d_AspectMarker3d (Aspect_TOM_O_PLUS, Quantity_NOC_INDIANRED3, 2.0));
    aSensPntGroup->AddPrimitiveArray (anArrayOfPoints);
  }
  if (!aSeqLines.IsEmpty())
  {
    Prs3d::AddPrimitivesGroup (thePrs, new Prs3d_LineAspect (Quantity_NOC_AQUAMARINE1, Aspect_TOL_DASH, 1.0), aSeqLines);
  }
  if (!aSeqFree.IsEmpty())
  {
    Prs3d::AddPrimitivesGroup (thePrs, new Prs3d_LineAspect (Quantity_NOC_GREEN, Aspect_TOL_SOLID, 2.0), aSeqFree);
  }
}
