// Created on: 1995-03-15
// Created by: Robert COUBLANC
// Copyright (c) 1995-1999 Matra Datavision
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

#include <StdSelect_ViewerSelector3d.hxx>
#include <StdSelect.hxx>
#include <SelectBasics_SensitiveEntity.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax3.hxx>
#include <gp_GTrsf.hxx>
#include <gp_Pln.hxx>
#include <Select3D_SensitiveEntity.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_SequenceOfHClipPlane.hxx>
#include <Graphic3d_Structure.hxx>
#include <SelectMgr_SelectableObject.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <Select3D_SensitiveCurve.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <Select3D_SensitiveFace.hxx>
#include <Select3D_SensitiveCircle.hxx>
#include <Select3D_SensitivePoint.hxx>
#include <Select3D_SensitiveTriangulation.hxx>
#include <Select3D_SensitiveTriangle.hxx>
#include <Select3D_SensitiveWire.hxx>
#include <Select3D_SensitiveBox.hxx>
#include <SelectMgr_Selection.hxx>
#include <SelectMgr_EntityOwner.hxx>

#include <Aspect_Grid.hxx>
#include <Aspect_TypeOfMarker.hxx>
#include <Aspect_Window.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <math_BullardGenerator.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Quantity_ColorHasher.hxx>
#include <Poly_Connect.hxx>
#include <TColStd_HArray1OfInteger.hxx>

#include <Poly_Array1OfTriangle.hxx>
#include <Poly_Triangulation.hxx>
#include <OSD_Environment.hxx>
#include <V3d.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <TColgp_SequenceOfPnt.hxx>

#include <OSD_Timer.hxx>


IMPLEMENT_STANDARD_RTTIEXT(StdSelect_ViewerSelector3d,SelectMgr_ViewerSelector)

static Standard_Integer StdSel_NumberOfFreeEdges (const Handle(Poly_Triangulation)& Trg)
{
  Standard_Integer nFree = 0;
  Poly_Connect pc(Trg);
  Standard_Integer t[3];
  Standard_Integer i, j;
  for (i = 1; i <= Trg->NbTriangles(); i++)
  {
    pc.Triangles (i, t[0], t[1], t[2]);
    for (j = 0; j < 3; j++)
      if (t[j] == 0) nFree++;
  }
  return nFree;
}

//=======================================================================
// Function : Constructor
// Purpose  :
//=======================================================================
StdSelect_ViewerSelector3d::StdSelect_ViewerSelector3d() {}

//=======================================================================
// Function: SetPixelTolerance
// Purpose :
//=======================================================================
void StdSelect_ViewerSelector3d::SetPixelTolerance (const Standard_Integer theTolerance)
{
  if (myTolerances.Tolerance() != theTolerance)
  {
    if (theTolerance < 0)
      myTolerances.ResetDefaults();
    else
      myTolerances.SetCustomTolerance (theTolerance);
    myToUpdateTolerance = Standard_True;
  }
}

//=======================================================================
// Function: Pick
// Purpose :
//=======================================================================
void StdSelect_ViewerSelector3d::Pick (const Standard_Integer theXPix,
                                       const Standard_Integer theYPix,
                                       const Handle(V3d_View)& theView)
{
  updateZLayers (theView);
  if(myToUpdateTolerance)
  {
    mySelectingVolumeMgr.SetPixelTolerance (myTolerances.Tolerance());
    myToUpdateTolerance = Standard_False;
  }

  mySelectingVolumeMgr.SetCamera (theView->Camera());
  mySelectingVolumeMgr.SetActiveSelectionType (SelectMgr_SelectingVolumeManager::Point);
  Standard_Integer aWidth = 0, aHeight = 0;
  theView->Window()->Size (aWidth, aHeight);
  mySelectingVolumeMgr.SetWindowSize (aWidth, aHeight);
  gp_Pnt2d aMousePos (static_cast<Standard_Real> (theXPix),
                      static_cast<Standard_Real> (theYPix));
  mySelectingVolumeMgr.BuildSelectingVolume (aMousePos);
  mySelectingVolumeMgr.SetViewClipping (theView->ClipPlanes());

  TraverseSensitives();
}

//=======================================================================
// Function: Pick
// Purpose :
//=======================================================================
void StdSelect_ViewerSelector3d::Pick (const Standard_Integer theXPMin,
                                       const Standard_Integer theYPMin,
                                       const Standard_Integer theXPMax,
                                       const Standard_Integer theYPMax,
                                       const Handle(V3d_View)& theView)
{
  updateZLayers (theView);
  mySelectingVolumeMgr.SetCamera (theView->Camera());
  mySelectingVolumeMgr.SetActiveSelectionType (SelectMgr_SelectingVolumeManager::Box);
  Standard_Integer aWidth = 0, aHeight = 0;
  theView->Window()->Size (aWidth, aHeight);
  mySelectingVolumeMgr.SetWindowSize (aWidth, aHeight);
  gp_Pnt2d aMinMousePos (static_cast<Standard_Real> (theXPMin),
                         static_cast<Standard_Real> (theYPMin));
  gp_Pnt2d aMaxMousePos (static_cast<Standard_Real> (theXPMax),
                         static_cast<Standard_Real> (theYPMax));
  mySelectingVolumeMgr.BuildSelectingVolume (aMinMousePos,
                                             aMaxMousePos);

  TraverseSensitives();
}

//=======================================================================
// Function: Pick
// Purpose : Selection using a polyline
//=======================================================================
void StdSelect_ViewerSelector3d::Pick (const TColgp_Array1OfPnt2d& thePolyline,
                                       const Handle(V3d_View)& theView)
{
  updateZLayers (theView);
  mySelectingVolumeMgr.SetCamera (theView->Camera());
  mySelectingVolumeMgr.SetActiveSelectionType (SelectMgr_SelectingVolumeManager::Polyline);
  Standard_Integer aWidth = 0, aHeight = 0;
  theView->Window()->Size (aWidth, aHeight);
  mySelectingVolumeMgr.SetWindowSize (aWidth, aHeight);
  mySelectingVolumeMgr.BuildSelectingVolume (thePolyline);

  TraverseSensitives();
}

//=======================================================================
// Function: DisplaySensitive.
// Purpose : Display active primitives.
//=======================================================================
void StdSelect_ViewerSelector3d::DisplaySensitive (const Handle(V3d_View)& theView)
{
  SelectMgr_SelectableObjectSet::Iterator aSelectableIt (mySelectableObjects);

  for (; aSelectableIt.More(); aSelectableIt.Next())
  {
    const Handle (SelectMgr_SelectableObject)& anObj = aSelectableIt.Value();

    Handle(Graphic3d_Structure) aStruct = new Graphic3d_Structure (theView->Viewer()->StructureManager());

    for (anObj->Init(); anObj->More(); anObj->Next())
    {
      if (anObj->CurrentSelection()->GetSelectionState() == SelectMgr_SOS_Activated)
      {
        computeSensitivePrs (aStruct, anObj->CurrentSelection(), anObj->Transformation(), Handle(Graphic3d_TransformPers)());
      }
    }

    myStructs.Append (aStruct);
  }

  for (Standard_Integer aStructIdx = 1; aStructIdx <= myStructs.Length(); ++aStructIdx)
  {
    Handle(Graphic3d_Structure)& aStruct = myStructs.ChangeValue (aStructIdx);
    aStruct->SetDisplayPriority (10);
    aStruct->Display();
  }

  theView->Update();
}

//=======================================================================
// Function: ClearSensitive
// Purpose :
//=======================================================================
void StdSelect_ViewerSelector3d::ClearSensitive (const Handle(V3d_View)& theView)
{
  for (Standard_Integer aStructIdx = 1; aStructIdx <= myStructs.Length(); ++aStructIdx)
  {
    myStructs.Value (aStructIdx)->Remove();
  }

  myStructs.Clear();

  if (!theView.IsNull())
  {
    theView->Update();
  }
}

//=======================================================================
//function : DisplaySenstive
//purpose  :
//=======================================================================
void StdSelect_ViewerSelector3d::DisplaySensitive (const Handle(SelectMgr_Selection)& theSel,
                                                   const gp_Trsf& theTrsf,
                                                   const Handle(V3d_View)& theView,
                                                   const Standard_Boolean theToClearOthers)
{
  if (theToClearOthers)
  {
    ClearSensitive (theView);
  }

  Handle(Graphic3d_Structure) aStruct = new Graphic3d_Structure (theView->Viewer()->StructureManager());

  computeSensitivePrs (aStruct, theSel, theTrsf, Handle(Graphic3d_TransformPers)());

  myStructs.Append (aStruct);
  myStructs.Last()->SetDisplayPriority (10);
  myStructs.Last()->Display();

  theView->Update();
}

//=======================================================================
//function : computeSensitivePrs
//purpose  :
//=======================================================================
void StdSelect_ViewerSelector3d::computeSensitivePrs (const Handle(Graphic3d_Structure)& theStructure,
                                                      const Handle(SelectMgr_Selection)& theSel,
                                                      const gp_Trsf& theLoc,
                                                      const Handle(Graphic3d_TransformPers)& theTrsfPers)
{
  theStructure->SetTransformPersistence (theTrsfPers);

  Handle(Graphic3d_Group) aSensGroup  = theStructure->NewGroup();

  Quantity_Color aColor (Quantity_NOC_INDIANRED3);
  Handle(Graphic3d_AspectMarker3d) aMarkerAspect =
    new Graphic3d_AspectMarker3d (Aspect_TOM_O_PLUS, aColor, 2.0);

  aSensGroup->SetPrimitivesAspect (aMarkerAspect);
  aSensGroup->SetPrimitivesAspect (
    new Graphic3d_AspectLine3d (Quantity_NOC_GRAY40, Aspect_TOL_SOLID, 2.0));

  Handle(Graphic3d_Group) anAreaGroup = theStructure->NewGroup();

  anAreaGroup->SetPrimitivesAspect (
    new Graphic3d_AspectLine3d (Quantity_NOC_AQUAMARINE1, Aspect_TOL_DASH, 1.0));

  TColgp_SequenceOfPnt aSeqLines, aSeqFree;
  TColStd_SequenceOfInteger aSeqBnds;

  for (theSel->Init(); theSel->More(); theSel->Next())
  {
    Handle(Select3D_SensitiveEntity) Ent =
      Handle(Select3D_SensitiveEntity)::DownCast(theSel->Sensitive()->BaseSensitive());
    const Standard_Boolean hasloc = theLoc.Form() != gp_Identity;

    //==============
    // Box
    //=============

    if (Ent->DynamicType()==STANDARD_TYPE(Select3D_SensitiveBox))
    {
      const Bnd_Box& B = Handle(Select3D_SensitiveBox)::DownCast (Ent)->Box();
      Standard_Real xmin, ymin, zmin, xmax, ymax, zmax;
      B.Get (xmin, ymin, zmin, xmax, ymax, zmax);
      Standard_Integer i;
      gp_Pnt theboxpoint[8] =
      {
        gp_Pnt(xmin,ymin,zmin),
        gp_Pnt(xmax,ymin,zmin),
        gp_Pnt(xmax,ymax,zmin),
        gp_Pnt(xmin,ymax,zmin),
        gp_Pnt(xmin,ymin,zmax),
        gp_Pnt(xmax,ymin,zmax),
        gp_Pnt(xmax,ymax,zmax),
        gp_Pnt(xmin,ymax,zmax)
      };
      if(hasloc)
      {
        for (i = 0; i <= 7; i++)
          theboxpoint[i].Transform (theLoc);
      }

      aSeqBnds.Append(5);
      for (i = 0; i < 4; i++)
        aSeqLines.Append(theboxpoint[i]);
      aSeqLines.Append(theboxpoint[0]);

      aSeqBnds.Append(5);
      for (i = 4; i < 8; i++)
        aSeqLines.Append(theboxpoint[i]);
      aSeqLines.Append(theboxpoint[4]);

      for (i = 0; i < 4; i++)
      {
        aSeqBnds.Append(2);
        aSeqLines.Append(theboxpoint[i]);
        aSeqLines.Append(theboxpoint[i+4]);
      }
    }
    //==============
    // Face
    //=============
    else if (Ent->DynamicType()==STANDARD_TYPE(Select3D_SensitiveFace))
    {
      Handle(Select3D_SensitiveFace) aFace = Handle(Select3D_SensitiveFace)::DownCast(Ent);
      Handle(TColgp_HArray1OfPnt) TheHPts;
      aFace->GetPoints(TheHPts);
      const TColgp_Array1OfPnt& ThePts = TheHPts->Array1();

      aSeqBnds.Append(ThePts.Length());
      for (Standard_Integer I = ThePts.Lower(); I <= ThePts.Upper(); I++)
      {
        if (hasloc)
          aSeqLines.Append(ThePts(I).Transformed (theLoc));
        else
          aSeqLines.Append(ThePts(I));
      }
    }
    //==============
    // Curve
    //=============
    else if (Ent->DynamicType()==STANDARD_TYPE(Select3D_SensitiveCurve))
    {
      Handle(Select3D_SensitiveCurve) aCurve = Handle(Select3D_SensitiveCurve)::DownCast(Ent);
      Handle(TColgp_HArray1OfPnt) TheHPts;
      aCurve->Points3D(TheHPts);
      const TColgp_Array1OfPnt& ThePts = TheHPts->Array1();

      aSeqBnds.Append(ThePts.Length());
      for (Standard_Integer I = ThePts.Lower(); I <= ThePts.Upper(); I++)
      {
        if (hasloc)
          aSeqLines.Append(ThePts(I).Transformed (theLoc));
        else
          aSeqLines.Append(ThePts(I));
      }
    }
    //==============
    // Wire
    //=============
    else if (Ent->DynamicType()==STANDARD_TYPE(Select3D_SensitiveWire))
    {
      Handle(Select3D_SensitiveWire) aWire = Handle(Select3D_SensitiveWire)::DownCast(Ent);
      const NCollection_Vector<Handle(Select3D_SensitiveEntity)>& anEntities = aWire->GetEdges();

      for (int i = 0; i < anEntities.Length(); i++)
      {
        Handle(Select3D_SensitiveEntity) SubEnt = anEntities.Value(i);

        //Segment
        if (SubEnt->DynamicType()==STANDARD_TYPE(Select3D_SensitiveSegment))
        {
          gp_Pnt P1 (Handle(Select3D_SensitiveSegment)::DownCast(SubEnt)->StartPoint().XYZ());
          gp_Pnt P2 (Handle(Select3D_SensitiveSegment)::DownCast(SubEnt)->EndPoint().XYZ());
          if (hasloc)
          {
            P1.Transform(theLoc);
            P2.Transform(theLoc);
          }
          aSeqBnds.Append(2);
          aSeqLines.Append(P1);
          aSeqLines.Append(P2);
        }

        //circle
        if (SubEnt->DynamicType()==STANDARD_TYPE(Select3D_SensitiveCircle))
        {
          Handle(Select3D_SensitiveCircle) aCircle = Handle(Select3D_SensitiveCircle)::DownCast(SubEnt);
          Standard_Integer aFrom, aTo;
          aCircle->ArrayBounds (aFrom, aTo);
          aTo -= 2;
          for (Standard_Integer aPntIter = aFrom; aPntIter <= aTo; aPntIter += 2)
          {
            gp_Pnt aPnts[3] =
            {
              gp_Pnt (aCircle->GetPoint3d (aPntIter + 0).XYZ()),
              gp_Pnt (aCircle->GetPoint3d (aPntIter + 1).XYZ()),
              gp_Pnt (aCircle->GetPoint3d (aPntIter + 2).XYZ())
            };

            if (hasloc)
            {
              aPnts[0].Transform (theLoc);
              aPnts[1].Transform (theLoc);
              aPnts[2].Transform (theLoc);
            }

            aSeqBnds.Append (4);
            aSeqLines.Append (aPnts[0]);
            aSeqLines.Append (aPnts[1]);
            aSeqLines.Append (aPnts[2]);
            aSeqLines.Append (aPnts[0]);
          }
        }

        //curve
        if (SubEnt->DynamicType()==STANDARD_TYPE(Select3D_SensitiveCurve))
        {
          Handle(Select3D_SensitiveCurve) aCurve = Handle(Select3D_SensitiveCurve)::DownCast(SubEnt);
          Handle(TColgp_HArray1OfPnt) TheHPts;
          aCurve->Points3D (TheHPts);
          const TColgp_Array1OfPnt& ThePts = TheHPts->Array1();

          aSeqBnds.Append(ThePts.Length());
          for (Standard_Integer I = ThePts.Lower(); I <= ThePts.Upper(); I++)
          {
            if (hasloc)
              aSeqLines.Append(ThePts(I).Transformed (theLoc));
            else
              aSeqLines.Append(ThePts(I));
          }
        }
      }
    }
    //==============
    // Segment
    //=============
    else if (Ent->DynamicType()==STANDARD_TYPE(Select3D_SensitiveSegment))
    {
      gp_Pnt P1 (Handle(Select3D_SensitiveSegment)::DownCast(Ent)->StartPoint().XYZ());
      gp_Pnt P2 (Handle(Select3D_SensitiveSegment)::DownCast(Ent)->EndPoint().XYZ());
      if (hasloc)
      {
        P1.Transform (theLoc);
        P2.Transform (theLoc);
      }
      aSeqBnds.Append(2);
      aSeqLines.Append(P1);
      aSeqLines.Append(P2);
    }
    //==============
    // Circle
    //=============
    else if (Ent->DynamicType()==STANDARD_TYPE(Select3D_SensitiveCircle))
    {
      Handle(Select3D_SensitiveCircle) aCircle = Handle(Select3D_SensitiveCircle)::DownCast(Ent);
      Standard_Integer aFrom, aTo;
      aCircle->ArrayBounds (aFrom, aTo);
      aTo -= 2;
      for (Standard_Integer aPntIter = aFrom; aPntIter <= aTo; aPntIter += 2)
      {
        gp_Pnt aPnts[3] =
        {
          gp_Pnt (aCircle->GetPoint3d (aPntIter + 0).XYZ()),
          gp_Pnt (aCircle->GetPoint3d (aPntIter + 1).XYZ()),
          gp_Pnt (aCircle->GetPoint3d (aPntIter + 2).XYZ())
        };

        if (hasloc)
        {
          aPnts[0].Transform (theLoc);
          aPnts[1].Transform (theLoc);
          aPnts[2].Transform (theLoc);
        }

        aSeqBnds.Append (4);
        aSeqLines.Append (aPnts[0]);
        aSeqLines.Append (aPnts[1]);
        aSeqLines.Append (aPnts[2]);
        aSeqLines.Append (aPnts[0]);
      }
    }
    //==============
    // Point
    //=============
    else if (Ent->DynamicType()==STANDARD_TYPE(Select3D_SensitivePoint))
    {
      gp_Pnt P = hasloc ?
        Handle(Select3D_SensitivePoint)::DownCast(Ent)->Point() :
        Handle(Select3D_SensitivePoint)::DownCast(Ent)->Point().Transformed (theLoc);
      Handle(Graphic3d_ArrayOfPoints) anArrayOfPoints = new Graphic3d_ArrayOfPoints (1);
      anArrayOfPoints->AddVertex (P.X(), P.Y(), P.Z());
      aSensGroup->AddPrimitiveArray (anArrayOfPoints);
    }
    //============================================================
    // Triangulation : On met un petit offset ves l'interieur...
    //==========================================================
    else if (Ent->DynamicType()==STANDARD_TYPE(Select3D_SensitiveTriangulation))
    {
      Handle(Poly_Triangulation) PT (Handle(Select3D_SensitiveTriangulation)::DownCast (Ent)->Triangulation());

      const Poly_Array1OfTriangle& triangles = PT->Triangles();
      const TColgp_Array1OfPnt& Nodes = PT->Nodes();
      Standard_Integer n[3];

      TopLoc_Location iloc, bidloc;
      if (Handle(Select3D_SensitiveTriangulation)::DownCast (Ent)->HasInitLocation())
        bidloc = Handle(Select3D_SensitiveTriangulation)::DownCast (Ent)->GetInitLocation();

      if (bidloc.IsIdentity())
        iloc = theLoc;
      else
        iloc = theLoc * bidloc;

      Standard_Integer i;
      for (i = 1; i <= PT->NbTriangles(); i++)
      {
        triangles (i).Get (n[0], n[1], n[2]);
        gp_Pnt P1 (Nodes (n[0]).Transformed (iloc));
        gp_Pnt P2 (Nodes (n[1]).Transformed (iloc));
        gp_Pnt P3 (Nodes (n[2]).Transformed (iloc));
        gp_XYZ V1 (P1.XYZ());
        gp_XYZ V2 (P2.XYZ());
        gp_XYZ V3 (P3.XYZ());
        gp_XYZ CDG (P1.XYZ()); CDG += (P2.XYZ()); CDG += (P3.XYZ()); CDG /= 3.0;
        V1 -= CDG; V2 -= CDG; V3 -= CDG;
        V1 *= 0.9; V2 *= 0.9; V3 *= 0.9;
        V1 += CDG; V2 += CDG; V3 += CDG;

        aSeqBnds.Append(4);
        aSeqLines.Append(gp_Pnt(V1));
        aSeqLines.Append(gp_Pnt(V2));
        aSeqLines.Append(gp_Pnt(V3));
        aSeqLines.Append(gp_Pnt(V1));
      }

      // recherche des bords libres...

      Handle(TColStd_HArray1OfInteger) FreeEdges = new TColStd_HArray1OfInteger (1, 2 * StdSel_NumberOfFreeEdges (PT));
      TColStd_Array1OfInteger& FreeE = FreeEdges->ChangeArray1();
      Poly_Connect pc (PT);
      Standard_Integer t[3];
      Standard_Integer j;
      Standard_Integer fr (1);
      for (i = 1; i <= PT->NbTriangles(); i++)
      {
        pc.Triangles (i, t[0], t[1], t[2]);
        triangles (i).Get (n[0], n[1], n[2]);
        for (j = 0; j < 3; j++)
        {
          Standard_Integer k = (j + 1) % 3;
          if (t[j] == 0)
          {
            FreeE (fr)    = n[j];
            FreeE (fr + 1)= n[k];
            fr += 2;
          }
        }
      }
      for (Standard_Integer ifri = 1; ifri <= FreeE.Length(); ifri += 2)
      {
        gp_Pnt pe1 (Nodes (FreeE (ifri)).Transformed (iloc)), pe2 (Nodes (FreeE (ifri + 1)).Transformed (iloc));
        aSeqFree.Append(pe1);
        aSeqFree.Append(pe2);
      }
    }
    else if (Ent->DynamicType()==STANDARD_TYPE(Select3D_SensitiveTriangle))
    {
      Handle(Select3D_SensitiveTriangle) Str = Handle(Select3D_SensitiveTriangle)::DownCast(Ent);
      gp_Pnt P1, P2, P3;
      Str->Points3D (P1, P2, P3);
      gp_Pnt CDG = Str->Center3D();

      gp_XYZ V1 (P1.XYZ()); V1 -= (CDG.XYZ());
      gp_XYZ V2 (P2.XYZ()); V2 -= (CDG.XYZ());
      gp_XYZ V3 (P3.XYZ()); V3 -= (CDG.XYZ());
      V1 *= 0.9; V2 *= 0.9; V3 *= 0.9;
      V1 += CDG.XYZ(); V2 += CDG.XYZ(); V3 += CDG.XYZ();

      aSeqBnds.Append(4);
      aSeqLines.Append(gp_Pnt(V1));
      aSeqLines.Append(gp_Pnt(V2));
      aSeqLines.Append(gp_Pnt(V3));
      aSeqLines.Append(gp_Pnt(V1));
    }
  }

  Standard_Integer i;

  if (aSeqLines.Length())
  {
    Handle(Graphic3d_ArrayOfPolylines) aPrims = new Graphic3d_ArrayOfPolylines(aSeqLines.Length(),aSeqBnds.Length());
    for (i = 1; i <= aSeqLines.Length(); i++)
      aPrims->AddVertex(aSeqLines(i));
    for (i = 1; i <= aSeqBnds.Length(); i++)
      aPrims->AddBound(aSeqBnds(i));
    anAreaGroup->AddPrimitiveArray(aPrims);
  }

  if (aSeqFree.Length())
  {
    aSensGroup->SetPrimitivesAspect (new Graphic3d_AspectLine3d (Quantity_NOC_GREEN, Aspect_TOL_SOLID, 2.0));
    Handle(Graphic3d_ArrayOfPolylines) aPrims = new Graphic3d_ArrayOfPolylines(aSeqFree.Length(),aSeqFree.Length()/2);
    for (i = 1; i <= aSeqFree.Length(); i++)
    {
      aPrims->AddBound(2);
      aPrims->AddVertex(aSeqLines(i++));
      aPrims->AddVertex(aSeqLines(i));
    }
    aSensGroup->AddPrimitiveArray(aPrims);
    aSensGroup->SetPrimitivesAspect (new Graphic3d_AspectLine3d (Quantity_NOC_GRAY40, Aspect_TOL_SOLID, 2.0));
  }
}

//=======================================================================
//function : HasDepthClipping
//purpose  :
//=======================================================================
Standard_Boolean StdSelect_ViewerSelector3d::HasDepthClipping (const Handle(SelectMgr_EntityOwner)& theOwner) const
{
  if (!theOwner->HasSelectable())
  {
    return Standard_False;
  }

  const Handle(SelectMgr_SelectableObject)& aSelectable = theOwner->Selectable();
  return !aSelectable->ClipPlanes().IsNull()
      && !aSelectable->ClipPlanes()->IsEmpty();
}

//=======================================================================
// Function: updateZLayers
// Purpose :
//=======================================================================
void StdSelect_ViewerSelector3d::updateZLayers (const Handle(V3d_View)& theView)
{
  myZLayerOrderMap.Clear();
  TColStd_SequenceOfInteger aZLayers;
  theView->Viewer()->GetAllZLayers (aZLayers);
  Standard_Integer aPos = 0;
  Standard_Boolean isPrevDepthWrite = true;
  for (TColStd_SequenceOfInteger::Iterator aLayerIter (aZLayers); aLayerIter.More(); aLayerIter.Next())
  {
    Graphic3d_ZLayerSettings aSettings = theView->Viewer()->ZLayerSettings (aLayerIter.Value());
    if (aSettings.ToClearDepth()
     || isPrevDepthWrite != aSettings.ToEnableDepthWrite())
    {
      ++aPos;
    }
    isPrevDepthWrite = aSettings.ToEnableDepthWrite();
    myZLayerOrderMap.Bind (aLayerIter.Value(), aPos);
  }
}

namespace
{
  //! Abstract class for filling pixel with color.
  class BaseFiller : public Standard_Transient
  {
    DEFINE_STANDARD_RTTI_INLINE(BaseFiller, Standard_Transient)
  public:

    //! Main constructor.
    BaseFiller (Image_PixMap&               thePixMap,
                StdSelect_ViewerSelector3d* theSelector)
    : myImage  (&thePixMap),
      myMainSel(theSelector) {}

    //! Fill pixel at specified position.
    virtual void Fill (const Standard_Integer theCol,
                       const Standard_Integer theRow,
                       const Standard_Integer thePicked) = 0;

    //! Flush results into final image.
    virtual void Flush() {}

  protected:

    //! Find the new unique random color.
    void randomPastelColor (Quantity_Color& theColor)
    {
      for (;;)
      {
        nextRandomPastelColor (theColor);
        if (myUniqueColors.Add (theColor))
        {
          return;
        }
      }
    }

    //! Fills the given color as random.
    void nextRandomPastelColor (Quantity_Color& theColor)
    {
      theColor = Quantity_Color (Standard_Real(myBullardGenerator.NextInt() % 256) / 255.0,
                                 Standard_Real(myBullardGenerator.NextInt() % 256) / 255.0,
                                 Standard_Real(myBullardGenerator.NextInt() % 256) / 255.0,
                                 Quantity_TOC_RGB);
    }

  protected:
    Image_PixMap*               myImage;
    StdSelect_ViewerSelector3d* myMainSel;
    math_BullardGenerator       myBullardGenerator;
    NCollection_Map<Quantity_Color, Quantity_ColorHasher> myUniqueColors;
  };

  //! Help class for filling pixel with random color.
  class GeneratedEntityColorFiller : public BaseFiller
  {
    DEFINE_STANDARD_RTTI_INLINE(GeneratedEntityColorFiller, BaseFiller)
  public:
    GeneratedEntityColorFiller (Image_PixMap& thePixMap,
                                StdSelect_ViewerSelector3d* theSelector,
                                const SelectMgr_SelectableObjectSet& theSelObjects)
    : BaseFiller (thePixMap, theSelector)
    {
      // generate per-entity colors in the order as they have been activated
      for (SelectMgr_SelectableObjectSet::Iterator anObjIter (theSelObjects); anObjIter.More(); anObjIter.Next())
      {
        const Handle(SelectMgr_SelectableObject)& anObj = anObjIter.Value();
        for (anObj->Init(); anObj->More(); anObj->Next())
        {
          const Handle(SelectMgr_Selection)& aSel = anObj->CurrentSelection();
          for (aSel->Init(); aSel->More(); aSel->Next())
          {
            const Handle(SelectMgr_SensitiveEntity)& aSens = aSel->Sensitive();
            if (!myMapEntityColors.IsBound (aSens->BaseSensitive()))
            {
              Quantity_Color aColor;
              randomPastelColor (aColor);
              myMapEntityColors.Bind (aSens->BaseSensitive(), aColor);
            }
          }
        }
      }
    }

    virtual void Fill (const Standard_Integer theCol,
                       const Standard_Integer theRow,
                       const Standard_Integer thePicked) Standard_OVERRIDE
    {
      if (thePicked < 1
       || thePicked > myMainSel->NbPicked())
      {
        myImage->SetPixelColor (theCol, theRow, Quantity_Color(Quantity_NOC_BLACK));
        return;
      }

      const Handle(SelectBasics_SensitiveEntity)& aPickedEntity = myMainSel->PickedEntity (thePicked);
      Quantity_Color aColor (Quantity_NOC_BLACK);
      myMapEntityColors.Find (aPickedEntity, aColor);
      myImage->SetPixelColor (theCol, theRow, aColor);
    }

  protected:
    NCollection_DataMap<Handle(SelectBasics_SensitiveEntity), Quantity_Color> myMapEntityColors;
  };

  //! Help class for filling pixel with normalized depth of ray.
  class NormalizedDepthFiller : public BaseFiller
  {
    DEFINE_STANDARD_RTTI_INLINE(NormalizedDepthFiller, BaseFiller)
  public:
    NormalizedDepthFiller (Image_PixMap& thePixMap,
                           StdSelect_ViewerSelector3d* theSelector,
                           const Standard_Boolean theToInverse)
    : BaseFiller (thePixMap, theSelector),
      myDepthMin ( RealLast()),
      myDepthMax (-RealLast()),
      myToInverse(theToInverse)
    {
      myUnnormImage.InitZero (Image_Format_GrayF, thePixMap.SizeX(), thePixMap.SizeY());
    }

    //! Accumulate the data.
    virtual void Fill (const Standard_Integer theCol,
                       const Standard_Integer theRow,
                       const Standard_Integer thePicked) Standard_OVERRIDE
    {
      if (myUnnormImage.IsEmpty())
      {
        return;
      }

      if (thePicked < 1
       || thePicked > myMainSel->NbPicked())
      {
        myUnnormImage.ChangeValue<float> (theRow, theCol) = ShortRealLast();
        return;
      }

      const SelectMgr_SortCriterion& aSortCriterion = myMainSel->PickedData (thePicked);
      myUnnormImage.ChangeValue<float> (theRow, theCol) = float(aSortCriterion.Depth);
      myDepthMin = Min (myDepthMin, aSortCriterion.Depth);
      myDepthMax = Max (myDepthMax, aSortCriterion.Depth);
    }

    //! Normalize the depth values.
    virtual void Flush() Standard_OVERRIDE
    {
      Standard_Real aFrom  = 0.0;
      Standard_Real aDelta = 1.0;
      if (myDepthMin <= myDepthMax)
      {
        aFrom = myDepthMin;
        if (myDepthMin != myDepthMax)
        {
          aDelta = myDepthMax - myDepthMin;
        }
      }
      for (Standard_Size aRowIter = 0; aRowIter < myUnnormImage.SizeY(); ++aRowIter)
      {
        for (Standard_Size aColIter = 0; aColIter < myUnnormImage.SizeX(); ++aColIter)
        {
          float aDepth = myUnnormImage.Value<float> (aRowIter, aColIter);
          if (aDepth <= -ShortRealLast()
           || aDepth >=  ShortRealLast())
          {
            myImage->SetPixelColor (Standard_Integer(aColIter), Standard_Integer(aRowIter),
                                    Quantity_ColorRGBA (0.0f, 0.0f, 0.0f, 1.0f));
            continue;
          }

          float aNormDepth = float((Standard_Real(aDepth) - aFrom) / aDelta);
          if (myToInverse)
          {
            aNormDepth = 1.0f - aNormDepth;
          }
          myImage->SetPixelColor (Standard_Integer(aColIter), Standard_Integer(aRowIter),
                                  Quantity_ColorRGBA (aNormDepth, aNormDepth, aNormDepth, 1.0f));
        }
      }
    }

  private:
    Image_PixMap     myUnnormImage;
    Standard_Real    myDepthMin;
    Standard_Real    myDepthMax;
    Standard_Boolean myToInverse;
  };

  //! Help class for filling pixel with unnormalized depth of ray.
  class UnnormalizedDepthFiller : public BaseFiller
  {
    DEFINE_STANDARD_RTTI_INLINE(UnnormalizedDepthFiller, BaseFiller)
  public:
    UnnormalizedDepthFiller (Image_PixMap&               thePixMap,
                             StdSelect_ViewerSelector3d* theSelector)
    : BaseFiller (thePixMap, theSelector) {}

    virtual void Fill (const Standard_Integer theCol,
                       const Standard_Integer theRow,
                       const Standard_Integer thePicked) Standard_OVERRIDE
    {
      if (thePicked < 1
       || thePicked > myMainSel->NbPicked())
      {
        myImage->SetPixelColor (theCol, theRow, Quantity_ColorRGBA (0.0f, 0.0f, 0.0f, 1.0f));
        return;
      }

      const SelectMgr_SortCriterion& aSortCriterion = myMainSel->PickedData (thePicked);
      const float aDepth = float(aSortCriterion.Depth);
      myImage->SetPixelColor (theCol, theRow, Quantity_ColorRGBA (aDepth, aDepth, aDepth, 1.0f));
    }
  };

  //! Help class for filling pixel with color of detected object.
  class GeneratedOwnerColorFiller : public BaseFiller
  {
    DEFINE_STANDARD_RTTI_INLINE(GeneratedOwnerColorFiller, BaseFiller)
  public:
    GeneratedOwnerColorFiller (Image_PixMap& thePixMap,
                               StdSelect_ViewerSelector3d* theSelector,
                               const SelectMgr_SelectableObjectSet& theSelObjects)
    : BaseFiller (thePixMap, theSelector)
    {
      // generate per-owner colors in the order as they have been activated
      for (SelectMgr_SelectableObjectSet::Iterator anObjIter (theSelObjects); anObjIter.More(); anObjIter.Next())
      {
        const Handle(SelectMgr_SelectableObject)& anObj = anObjIter.Value();
        for (anObj->Init(); anObj->More(); anObj->Next())
        {
          const Handle(SelectMgr_Selection)& aSel = anObj->CurrentSelection();
          for (aSel->Init(); aSel->More(); aSel->Next())
          {
            const Handle(SelectMgr_SensitiveEntity)& aSens   = aSel->Sensitive();
            const Handle(SelectBasics_EntityOwner)&  anOwner = aSens->BaseSensitive()->OwnerId();
            if (!myMapOwnerColors.IsBound (anOwner))
            {
              Quantity_Color aColor;
              randomPastelColor (aColor);
              myMapOwnerColors.Bind (anOwner, aColor);
            }
          }
        }
      }
    }

    virtual void Fill (const Standard_Integer theCol,
                       const Standard_Integer theRow,
                       const Standard_Integer thePicked) Standard_OVERRIDE
    {
      if (thePicked < 1
       || thePicked > myMainSel->NbPicked())
      {
        myImage->SetPixelColor (theCol, theRow, Quantity_Color(Quantity_NOC_BLACK));
        return;
      }

      const Handle(SelectMgr_EntityOwner)& aPickedOwner = myMainSel->Picked (thePicked);
      Quantity_Color aColor (Quantity_NOC_BLACK);
      myMapOwnerColors.Find (aPickedOwner, aColor);
      myImage->SetPixelColor (theCol, theRow, aColor);
    }

  protected:
    NCollection_DataMap<Handle(SelectBasics_EntityOwner), Quantity_Color> myMapOwnerColors;
  };

  //! Help class for filling pixel with random color for each selection mode.
  class GeneratedSelModeColorFiller : public BaseFiller
  {
    DEFINE_STANDARD_RTTI_INLINE(GeneratedSelModeColorFiller, BaseFiller)
  public:
    GeneratedSelModeColorFiller (Image_PixMap&               thePixMap,
                                 StdSelect_ViewerSelector3d* theSelector)
    : BaseFiller (thePixMap, theSelector)
    {
      // generate standard modes in proper order, consider custom objects would use similar scheme
      myMapSelectionModeColors.Bind (     0, Quantity_NOC_WHITE);          // default (entire object selection)
      myMapSelectionModeColors.Bind (     1, Quantity_NOC_YELLOW);         // TopAbs_VERTEX
      myMapSelectionModeColors.Bind (     2, Quantity_NOC_GREEN);          // TopAbs_EDGE
      myMapSelectionModeColors.Bind (     3, Quantity_NOC_RED);            // TopAbs_WIRE
      myMapSelectionModeColors.Bind (     4, Quantity_NOC_BLUE1);          // TopAbs_FACE
      myMapSelectionModeColors.Bind (     5, Quantity_NOC_CYAN1);          // TopAbs_SHELL
      myMapSelectionModeColors.Bind (     6, Quantity_NOC_PURPLE);         // TopAbs_SOLID
      myMapSelectionModeColors.Bind (     7, Quantity_NOC_MAGENTA1);       // TopAbs_COMPSOLID
      myMapSelectionModeColors.Bind (     8, Quantity_NOC_BROWN);          // TopAbs_COMPOUND
      myMapSelectionModeColors.Bind (0x0010, Quantity_NOC_PINK);           // MeshVS_SMF_Volume
      myMapSelectionModeColors.Bind (0x001E, Quantity_NOC_LIMEGREEN);      // MeshVS_SMF_Element
      myMapSelectionModeColors.Bind (0x001F, Quantity_NOC_DARKOLIVEGREEN); // MeshVS_SMF_All
      myMapSelectionModeColors.Bind (0x0100, Quantity_NOC_GOLD);           // MeshVS_SMF_Group
    }

    virtual void Fill (const Standard_Integer theCol,
                       const Standard_Integer theRow,
                       const Standard_Integer thePicked) Standard_OVERRIDE
    {
      if (thePicked < 1
       || thePicked > myMainSel->NbPicked())
      {
        myImage->SetPixelColor (theCol, theRow, Quantity_Color (Quantity_NOC_BLACK));
        return;
      }

      Standard_Integer aSelectionMode = -1;
      const Handle(SelectMgr_SelectableObject)&   aSelectable = myMainSel->Picked       (thePicked)->Selectable();
      const Handle(SelectBasics_SensitiveEntity)& anEntity    = myMainSel->PickedEntity (thePicked);
      for (aSelectable->Init(); aSelectable->More(); aSelectable->Next())
      {
        const Handle(SelectMgr_Selection)& aSelection = aSelectable->CurrentSelection();
        for (aSelection->Init(); aSelection->More(); aSelection->Next())
        {
          if (aSelection->Sensitive()->BaseSensitive() == anEntity)
          {
            aSelectionMode = aSelection->Mode();
            break;
          }
        }
      }
      if (aSelectionMode == -1)
      {
        myImage->SetPixelColor (theCol, theRow, Quantity_Color (Quantity_NOC_BLACK));
        return;
      }

      if (!myMapSelectionModeColors.IsBound (aSelectionMode))
      {
        Quantity_Color aColor;
        randomPastelColor (aColor);
        myMapSelectionModeColors.Bind (aSelectionMode, aColor);
      }

      const Quantity_Color& aColor = myMapSelectionModeColors.Find (aSelectionMode);
      myImage->SetPixelColor (theCol, theRow, aColor);
    }

  protected:
    NCollection_DataMap<Standard_Integer, Quantity_Color> myMapSelectionModeColors;
  };

  //! Help class for filling pixel with color of detected shape.
  class DetectedObjectColorFiller : public BaseFiller
  {
    DEFINE_STANDARD_RTTI_INLINE(DetectedObjectColorFiller, BaseFiller)
  public:
    DetectedObjectColorFiller (Image_PixMap&               thePixMap,
                               StdSelect_ViewerSelector3d* theSelector)
    : BaseFiller (thePixMap, theSelector) {}

    virtual void Fill (const Standard_Integer theCol,
                       const Standard_Integer theRow,
                       const Standard_Integer thePicked) Standard_OVERRIDE
    {
      Quantity_Color aColor (Quantity_NOC_BLACK);
      if (thePicked > 0
       && thePicked <= myMainSel->NbPicked())
      {
        const Handle(SelectMgr_SelectableObject)& aSelectable = myMainSel->Picked (thePicked)->Selectable();
        aColor = aSelectable->Attributes()->Color();
      }
      myImage->SetPixelColor (theCol, theRow, aColor);
    }
  };

}

//=======================================================================
//function : ToPixMap
//purpose  :
//=======================================================================
Standard_Boolean StdSelect_ViewerSelector3d::ToPixMap (Image_PixMap&                        theImage,
                                                       const Handle(V3d_View)&              theView,
                                                       const StdSelect_TypeOfSelectionImage theType,
                                                       const Standard_Integer               thePickedIndex)
{
  if (theImage.IsEmpty())
  {
    throw Standard_ProgramError("StdSelect_ViewerSelector3d::ToPixMap() has been called with empty image");
  }

  Handle(BaseFiller) aFiller;
  switch (theType)
  {
    case StdSelect_TypeOfSelectionImage_NormalizedDepth:
    case StdSelect_TypeOfSelectionImage_NormalizedDepthInverted:
    {
      aFiller = new NormalizedDepthFiller (theImage, this,
                                           theType == StdSelect_TypeOfSelectionImage_NormalizedDepthInverted);
      break;
    }
    case StdSelect_TypeOfSelectionImage_UnnormalizedDepth:
    {
      aFiller = new UnnormalizedDepthFiller (theImage, this);
      break;
    }
    case StdSelect_TypeOfSelectionImage_ColoredDetectedObject:
    {
      aFiller = new DetectedObjectColorFiller (theImage, this);
      break;
    }
    case StdSelect_TypeOfSelectionImage_ColoredEntity:
    {
      aFiller = new GeneratedEntityColorFiller (theImage, this, mySelectableObjects);
      break;
    }
    case StdSelect_TypeOfSelectionImage_ColoredOwner:
    {
      aFiller = new GeneratedOwnerColorFiller (theImage, this, mySelectableObjects);
      break;
    }
    case StdSelect_TypeOfSelectionImage_ColoredSelectionMode:
    {
      aFiller = new GeneratedSelModeColorFiller (theImage, this);
      break;
    }
  }
  if (aFiller.IsNull())
  {
    return Standard_False;
  }

  const Standard_Integer aSizeX = static_cast<Standard_Integer> (theImage.SizeX());
  const Standard_Integer aSizeY = static_cast<Standard_Integer> (theImage.SizeY());
  for (Standard_Integer aRowIter = 0; aRowIter < aSizeY; ++aRowIter)
  {
    for (Standard_Integer aColIter = 0; aColIter < aSizeX; ++aColIter)
    {
      Pick (aColIter, aRowIter, theView);
      aFiller->Fill (aColIter, aRowIter, thePickedIndex);
    }
  }
  aFiller->Flush();
  return Standard_True;
}
