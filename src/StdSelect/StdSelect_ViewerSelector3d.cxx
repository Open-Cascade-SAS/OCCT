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
  SetClipping (theView->GetClipPlanes());

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
  for (Standard_Integer anObjectIdx = 0; anObjectIdx <= mySelectableObjects.Size(); ++anObjectIdx)
  {
    const Handle (SelectMgr_SelectableObject)& anObj = mySelectableObjects.GetObjectById (anObjectIdx);

    Handle(Graphic3d_Structure) aStruct = new Graphic3d_Structure (theView->Viewer()->StructureManager());

    for (anObj->Init(); anObj->More(); anObj->Next())
    {
      if (anObj->CurrentSelection()->GetSelectionState() == SelectMgr_SOS_Activated)
      {
        ComputeSensitivePrs (aStruct, anObj->CurrentSelection(), anObj->Transformation(), Graphic3d_TransformPers());
      }
    }

    myStructs.Append (aStruct);
  }

  for (Standard_Integer anObjectIdx = 0; anObjectIdx <= mySelectableObjectsTrsfPers.Size(); ++anObjectIdx)
  {
    const Handle (SelectMgr_SelectableObject)& anObj = mySelectableObjectsTrsfPers.GetObjectById (anObjectIdx);

    Handle(Graphic3d_Structure) aStruct = new Graphic3d_Structure (theView->Viewer()->StructureManager());

    if (!anObj->TransformPersistence().Flags || (anObj->TransformPersistence().Flags & Graphic3d_TMF_2d))
    {
      continue;
    }

    for (anObj->Init(); anObj->More(); anObj->Next())
    {
      if (anObj->CurrentSelection()->GetSelectionState() == SelectMgr_SOS_Activated)
      {
        ComputeSensitivePrs (aStruct, anObj->CurrentSelection(), anObj->Transformation(), anObj->TransformPersistence());
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

  ComputeSensitivePrs (aStruct, theSel, theTrsf, Graphic3d_TransformPers());

  myStructs.Append (aStruct);
  myStructs.Last()->SetDisplayPriority (10);
  myStructs.Last()->Display();

  theView->Update();
}

//=======================================================================
//function : ComputeSensitivePrs
//purpose  :
//=======================================================================
void StdSelect_ViewerSelector3d::ComputeSensitivePrs (const Handle(Graphic3d_Structure)& theStructure,
                                                      const Handle(SelectMgr_Selection)& theSel,
                                                      const gp_Trsf& theLoc,
                                                      const Graphic3d_TransformPers& theTransPers)
{
  theStructure->SetTransformPersistence (theTransPers.Flags, gp_Pnt (theTransPers.Point.x(),
                                                                     theTransPers.Point.y(),
                                                                     theTransPers.Point.z()));

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
        Handle(Select3D_SensitiveEntity) SubEnt = Handle(Select3D_SensitiveEntity)::DownCast(anEntities.Value(i));

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
//function : SetClipping
//purpose  :
//=======================================================================
void StdSelect_ViewerSelector3d::SetClipping (const Graphic3d_SequenceOfHClipPlane& thePlanes)
{
  myClipPlanes = thePlanes;
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
  return (aSelectable->GetClipPlanes().Size() > 0);
}
