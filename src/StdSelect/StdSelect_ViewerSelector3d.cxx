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

#include <StdSelect_ViewerSelector3d.ixx>
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
#include <Graphic3d_SequenceOfHClipPlane.hxx>
#include <SelectMgr_SelectableObject.hxx>
#include <SelectMgr_DataMapIteratorOfDataMapOfIntegerSensitive.hxx>
#include <SelectBasics_ListOfBox2d.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <Select3D_SensitiveCurve.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <Select3D_SensitiveFace.hxx>
#include <Select3D_SensitiveCircle.hxx>
#include <Select3D_SensitivePoint.hxx>
#include <Select3D_SensitiveTriangulation.hxx>
#include <Select3D_SensitiveTriangle.hxx>
#include <Select3D_SensitiveWire.hxx>
#include <Select3D_SensitiveEntitySequence.hxx>
#include <Select3D_ListOfSensitiveTriangle.hxx>
#include <Select3D_SensitiveBox.hxx>
#include <Select3D_ListIteratorOfListOfSensitiveTriangle.hxx>

#include <SelectMgr_DataMapIteratorOfDataMapOfSelectionActivation.hxx>
#include <Aspect_Grid.hxx>
#include <Aspect_TypeOfMarker.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <SelectBasics_ListIteratorOfListOfBox2d.hxx>
#include <Poly_Connect.hxx>
#include <TColStd_HArray1OfInteger.hxx>

#include <Poly_Array1OfTriangle.hxx>
#include <Poly_Triangulation.hxx>
#include <OSD_Environment.hxx>
#include <V3d.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <TColgp_SequenceOfPnt.hxx>

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
StdSelect_ViewerSelector3d::StdSelect_ViewerSelector3d()
: myProjector (new Select3D_Projector()),
  myPrevFOV (0.0),
  myPrevScale (0.0),
  myPrevOrthographic (Standard_True),
  mySensMode (StdSelect_SM_WINDOW),
  myPixelTolerance (2),
  myToUpdateTolerance (Standard_True)
{
  myPrevAt[0]         = 0.0;
  myPrevAt[1]         = 0.0;
  myPrevAt[2]         = 0.0;
  myPrevUp[0]         = 0.0;
  myPrevUp[1]         = 0.0;
  myPrevUp[2]         = 0.0;
  myPrevProj[0]       = 0.0;
  myPrevProj[1]       = 0.0;
  myPrevProj[2]       = 0.0;
  myPrevAxialScale[0] = 0.0;
  myPrevAxialScale[1] = 0.0;
  myPrevAxialScale[2] = 0.0;
}

//=======================================================================
// Function : Constructor
// Purpose  :
//=======================================================================
StdSelect_ViewerSelector3d::StdSelect_ViewerSelector3d (const Handle(Select3D_Projector)& theProj)
: myProjector (theProj),
  myPrevFOV (0.0),
  myPrevScale (0.0),
  myPrevOrthographic (Standard_True),
  mySensMode (StdSelect_SM_WINDOW),
  myPixelTolerance (2),
  myToUpdateTolerance (Standard_True)
{
  myPrevAt[0]         = 0.0;
  myPrevAt[1]         = 0.0;
  myPrevAt[2]         = 0.0;
  myPrevUp[0]         = 0.0;
  myPrevUp[1]         = 0.0;
  myPrevUp[2]         = 0.0;
  myPrevProj[0]       = 0.0;
  myPrevProj[1]       = 0.0;
  myPrevProj[2]       = 0.0;
  myPrevAxialScale[0] = 0.0;
  myPrevAxialScale[1] = 0.0;
  myPrevAxialScale[2] = 0.0;
}

//=======================================================================
// Function: Convert
// Purpose :
//=======================================================================
void StdSelect_ViewerSelector3d::Convert (const Handle(SelectMgr_Selection)& theSel)
{
  for (theSel->Init(); theSel->More(); theSel->Next())
  {
    if (theSel->Sensitive()->NeedsConversion())
    {
      Handle(Select3D_SensitiveEntity) aSE = *((Handle(Select3D_SensitiveEntity)*) &(theSel->Sensitive()));
      aSE->Project (myProjector);
      if (!tosort)
      {
        tosort = Standard_True;
      }
    }
  }
}

//=======================================================================
// Function: Set
// Purpose :
//=======================================================================
void StdSelect_ViewerSelector3d::Set (const Handle(Select3D_Projector)& theProj)
{
  myProjector = theProj;
  toupdate = Standard_True;
}

//=======================================================================
// Function: SetSensitivityMode
// Purpose :
//=======================================================================
void StdSelect_ViewerSelector3d::SetSensitivityMode (const StdSelect_SensitivityMode theMode)
{
  mySensMode = theMode;
  toupdate = Standard_True;
}

//=======================================================================
// Function: SetPixelTolerance
// Purpose :
//=======================================================================
void StdSelect_ViewerSelector3d::SetPixelTolerance (const Standard_Integer theTolerance)
{
  if (myPixelTolerance != theTolerance)
  {
    myPixelTolerance = theTolerance;
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
  UpdateProj (theView);
  Standard_Real aPnt3d[3];
  theView->Convert (theXPix, theYPix,
                    aPnt3d[0], aPnt3d[1], aPnt3d[2]);

  gp_Pnt2d aPnt2d;
  myProjector->Project (gp_Pnt (aPnt3d[0], aPnt3d[1], aPnt3d[2]), aPnt2d);

  InitSelect (aPnt2d.X(), aPnt2d.Y());
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
  if (myToUpdateTolerance && SensitivityMode() == StdSelect_SM_WINDOW)
  {
    SetSensitivity (theView->Convert (myPixelTolerance));
    myToUpdateTolerance = Standard_False;
  }

  UpdateProj (theView);

  Standard_Real aX1 = 0.0;
  Standard_Real aY1 = 0.0;
  Standard_Real aZ1 = 0.0;
  Standard_Real aX2 = 0.0;
  Standard_Real aY2 = 0.0;
  Standard_Real aZ2 = 0.0;
  gp_Pnt2d aP2d1;
  gp_Pnt2d aP2d2;

  theView->Convert (theXPMin, theYPMin, aX1, aY1, aZ1);
  theView->Convert (theXPMax, theYPMax, aX2, aY2, aZ2);
  myProjector->Project (gp_Pnt (aX1, aY1, aZ1), aP2d1);
  myProjector->Project (gp_Pnt (aX2, aY2, aZ2), aP2d2);

  InitSelect (Min (aP2d1.X(), aP2d2.X()),
              Min (aP2d1.Y(), aP2d2.Y()),
              Max (aP2d1.X(), aP2d2.X()),
              Max (aP2d1.Y(), aP2d2.Y()));
}

//=======================================================================
// Function: Pick
// Purpose : Selection using a polyline
//=======================================================================
void StdSelect_ViewerSelector3d::Pick (const TColgp_Array1OfPnt2d& thePolyline,
                                       const Handle(V3d_View)& theView)
{
  if (myToUpdateTolerance && SensitivityMode() == StdSelect_SM_WINDOW)
  {
    SetSensitivity (theView->Convert (myPixelTolerance));
    myToUpdateTolerance = Standard_False;
  }

  UpdateProj (theView);

  Standard_Integer aNbPix = thePolyline.Length();

  // Convert pixel
  Handle(TColgp_HArray1OfPnt2d) aP2d = new TColgp_HArray1OfPnt2d (1, aNbPix);

  for (Standard_Integer aPntIt = 1; aPntIt <= aNbPix; ++aPntIt)
  {
    Standard_Integer aXP = (Standard_Integer)(thePolyline (aPntIt).X());
    Standard_Integer aYP = (Standard_Integer)(thePolyline (aPntIt).Y());

    Standard_Real aX = 0.0;
    Standard_Real aY = 0.0;
    Standard_Real aZ = 0.0;
    gp_Pnt2d aPnt2d;

    theView->Convert (aXP, aYP, aX, aY, aZ);
    myProjector->Project (gp_Pnt (aX, aY, aZ), aPnt2d);

    aP2d->SetValue (aPntIt, aPnt2d);
  }

  const TColgp_Array1OfPnt2d& aPolyConvert = aP2d->Array1();

  InitSelect (aPolyConvert);
}

//=======================================================================
// Function: DisplayAreas
// Purpose : display the activated areas...
//=======================================================================
void StdSelect_ViewerSelector3d::DisplayAreas (const Handle(V3d_View)& theView)
{
  if (myToUpdateTolerance && SensitivityMode() == StdSelect_SM_WINDOW)
  {
    SetSensitivity (theView->Convert (myPixelTolerance));
    myToUpdateTolerance = Standard_False;
  }

  UpdateProj (theView);
  UpdateSort(); // Updates the activated areas

  if (mystruct.IsNull())
  {
    mystruct = new Graphic3d_Structure (theView->Viewer()->Viewer());
  }

  if (myareagroup.IsNull())
  {
    myareagroup  = mystruct->NewGroup();
  }

  SelectMgr_DataMapIteratorOfDataMapOfIntegerSensitive anIt (myentities);
  Handle(Select3D_Projector) aProjector = StdSelect::GetProjector (theView);
  aProjector->SetView (theView);

  Standard_Real aXmin = 0.0;
  Standard_Real aYmin = 0.0;
  Standard_Real aXmax = 0.0;
  Standard_Real aYmax = 0.0;
  gp_Pnt aPbid;
  SelectBasics_ListOfBox2d aBoxList;

  TColgp_SequenceOfPnt aSeqLines;
  for (; anIt.More(); anIt.Next())
  {
    anIt.Value()->Areas (aBoxList);

    for (SelectBasics_ListIteratorOfListOfBox2d aBoxIt (aBoxList); aBoxIt.More(); aBoxIt.Next())
    {
      aBoxIt.Value().Get (aXmin, aYmin, aXmax, aYmax);

      aPbid.SetCoord (aXmin - mytolerance, aYmin - mytolerance, 0.0);
      aProjector->Transform (aPbid, aProjector->InvertedTransformation());
      aSeqLines.Append (aPbid);

      aPbid.SetCoord (aXmax + mytolerance, aYmin - mytolerance, 0.0);
      aProjector->Transform (aPbid, aProjector->InvertedTransformation());
      aSeqLines.Append (aPbid);

      aPbid.SetCoord (aXmax + mytolerance, aYmax + mytolerance, 0.0);
      aProjector->Transform (aPbid, aProjector->InvertedTransformation());
      aSeqLines.Append (aPbid);

      aPbid.SetCoord (aXmin - mytolerance, aYmax + mytolerance, 0.0);
      aProjector->Transform (aPbid, aProjector->InvertedTransformation());
      aSeqLines.Append (aPbid);
    }
  }

  if (aSeqLines.Length())
  {
    Standard_Integer aN = 0;
    Standard_Integer aNp = 0;
    const Standard_Integer aNbl = aSeqLines.Length() / 4;

    Handle(Graphic3d_ArrayOfPolylines) aPrims = new Graphic3d_ArrayOfPolylines (5 * aNbl, aNbl);
    for (aNp = 1, aN = 0; aN < aNbl; aN++)
    {
      aPrims->AddBound (5);
      const gp_Pnt &aPnt1 = aSeqLines (aNp++);
      aPrims->AddVertex (aPnt1);
      aPrims->AddVertex (aSeqLines (aNp++));
      aPrims->AddVertex (aSeqLines (aNp++));
      aPrims->AddVertex (aSeqLines (aNp++));
      aPrims->AddVertex (aPnt1);
    }
    myareagroup->AddPrimitiveArray (aPrims);
  }

  myareagroup->SetGroupPrimitivesAspect (new Graphic3d_AspectLine3d (Quantity_NOC_AQUAMARINE1, Aspect_TOL_DASH, 1.0));
  myareagroup->Structure()->SetDisplayPriority (10);
  myareagroup->Structure()->Display();

  theView->Update();
}

//=======================================================================
// Function: ClearAreas
// Purpose :
//=======================================================================
void StdSelect_ViewerSelector3d::ClearAreas (const Handle(V3d_View)& theView)
{
  if (myareagroup.IsNull())
  {
    return;
  }

  myareagroup->Clear();

  if (!theView.IsNull())
  {
    theView->Update();
  }
}

//=======================================================================
// Function: UpdateProj
// Purpose :
//=======================================================================
Standard_Boolean StdSelect_ViewerSelector3d::UpdateProj (const Handle(V3d_View)& theView)
{
  // Check common properties of camera
  Standard_Real anUp[3];
  Standard_Real aProj[3];
  Standard_Real anAxialScale[3];
  theView->Up (anUp[0], anUp[1], anUp[2]);
  theView->Proj (aProj[0], aProj[1], aProj[2]);
  theView->AxialScale (anAxialScale[0], anAxialScale[1], anAxialScale[2]);

  Standard_Boolean isOrthographic = theView->Type() == V3d_ORTHOGRAPHIC;
  Standard_Boolean toUpdateProjector = myPrevOrthographic  != isOrthographic
                                    || myPrevUp[0]         != anUp[0]
                                    || myPrevUp[1]         != anUp[1]
                                    || myPrevUp[2]         != anUp[2]
                                    || myPrevProj[0]       != aProj[0]
                                    || myPrevProj[1]       != aProj[1]
                                    || myPrevProj[2]       != aProj[2]
                                    || myPrevAxialScale[0] != anAxialScale[0]
                                    || myPrevAxialScale[1] != anAxialScale[1]
                                    || myPrevAxialScale[2] != anAxialScale[2];

  // Check properties of perspective camera
  Standard_Real anAt[3];
  Standard_Real aScale = theView->Scale();
  Standard_Real aFOV   = theView->Camera()->FOVy();
  theView->At (anAt[0], anAt[1], anAt[2]);
  if (!isOrthographic && !toUpdateProjector)
  {
    toUpdateProjector = myPrevAt[0] != anAt[0]
                     || myPrevAt[1] != anAt[1]
                     || myPrevAt[2] != anAt[2]
                     || myPrevScale != aScale
                     || myPrevFOV   != aFOV;
  }

  myToUpdateTolerance = aScale != myPrevScale;

  // Update projector if anything changed
  if (toUpdateProjector)
  {
    toupdate = Standard_True;

    myToUpdateTolerance = Standard_True;

    if (isOrthographic)
    {
      // For orthographic view use only direction of projection and up vector
      // Panning, and zooming has no effect on 2D selection sensitives.
      Handle (Graphic3d_Camera) aCamera = new Graphic3d_Camera();

      aCamera->SetProjectionType (Graphic3d_Camera::Projection_Orthographic);
      aCamera->SetCenter (gp::Origin());
      aCamera->SetDirection (gp_Dir (-aProj[0], -aProj[1], -aProj[2]));
      aCamera->SetUp (gp_Dir (anUp[0], anUp[1], anUp[2]));
      aCamera->SetDistance (1.0);
      aCamera->SetAxialScale (gp_XYZ (anAxialScale[0], anAxialScale[1], anAxialScale[2]));

      myProjector = new Select3D_Projector (aCamera->OrientationMatrix(), Graphic3d_Mat4d());
    }
    else
    {
      // For perspective projection panning, zooming and location of view
      // has effect. Thus, use current view and projection matrices from
      // view camera. Exception is that the projection transformation
      // is scaled from NDC to size of displaying frame of view space in order
      // to maintain consistence with pixel tolerance conversion.
      const Graphic3d_Mat4d& aMVMatrix   = theView->Camera()->OrientationMatrix();
      const Graphic3d_Mat4d& aProjMatrix = theView->Camera()->ProjectionMatrix();
      gp_XYZ aViewDimensions = theView->Camera()->ViewDimensions();

      Graphic3d_Mat4d aScaledProj;
      aScaledProj.ChangeValue (0, 0) = aViewDimensions.X();
      aScaledProj.ChangeValue (1, 1) = aViewDimensions.Y();
      aScaledProj.ChangeValue (2, 2) = aViewDimensions.Z();
      Graphic3d_Mat4d aScaledProjMatrix = aScaledProj * aProjMatrix;

      myProjector = new Select3D_Projector (aMVMatrix, aScaledProjMatrix);
    }
  }

  myPrevAt[0] = anAt[0];
  myPrevAt[1] = anAt[1];
  myPrevAt[2] = anAt[2];
  myPrevUp[0] = anUp[0];
  myPrevUp[1] = anUp[1];
  myPrevUp[2] = anUp[2];
  myPrevProj[0] = aProj[0];
  myPrevProj[1] = aProj[1];
  myPrevProj[2] = aProj[2];
  myPrevAxialScale[0] = anAxialScale[0];
  myPrevAxialScale[1] = anAxialScale[1];
  myPrevAxialScale[2] = anAxialScale[2];
  myPrevFOV = aFOV;
  myPrevScale = aScale;
  myPrevOrthographic = isOrthographic;

  if (myToUpdateTolerance && SensitivityMode() == StdSelect_SM_WINDOW)
  {
    SetSensitivity (theView->Convert (myPixelTolerance));
    myToUpdateTolerance = Standard_False;
  }

  if (toupdate)
  {
    UpdateConversion();
  }

  if (tosort)
  {
    UpdateSort();
  }

  return Standard_True;
}


//=======================================================================
// Function: DisplaySensitive.
// Purpose : Display active primitives.
//=======================================================================
void StdSelect_ViewerSelector3d::DisplaySensitive (const Handle(V3d_View)& theView)
{
  if (myToUpdateTolerance && SensitivityMode() == StdSelect_SM_WINDOW)
  {
    SetSensitivity (theView->Convert (myPixelTolerance));
    myToUpdateTolerance = Standard_False;
  }

  if (toupdate)
  {
    UpdateProj (theView);
  }

  if (tosort)
  {
    UpdateSort(); // Updates the activated areas
  }

  // Preparation des structures
  if (mystruct.IsNull())
  {
    mystruct = new Graphic3d_Structure (theView->Viewer()->Viewer());
  }

  if (mysensgroup.IsNull())
  {
    mysensgroup = mystruct->NewGroup();
  }

  Quantity_Color aColor (Quantity_NOC_INDIANRED3);
  Handle(Graphic3d_AspectMarker3d) aMarkerAspect =
    new Graphic3d_AspectMarker3d (Aspect_TOM_O_PLUS, aColor, 2.0);

  mysensgroup->SetPrimitivesAspect (aMarkerAspect);
  mysensgroup->SetPrimitivesAspect (
    new Graphic3d_AspectLine3d (Quantity_NOC_GRAY40, Aspect_TOL_SOLID, 2.0));

  SelectMgr_DataMapIteratorOfDataMapOfSelectionActivation anIt (myselections);

  for (; anIt.More(); anIt.Next())
  {
    if (anIt.Value()==0)
    {
      const Handle(SelectMgr_Selection)& aSel = anIt.Key();
      ComputeSensitivePrs (aSel);
    }
  }

  mysensgroup->Structure()->SetDisplayPriority (10);
  mystruct->Display();

  theView->Update();
}

//=======================================================================
// Function: ClearSensitive
// Purpose :
//=======================================================================
void StdSelect_ViewerSelector3d::ClearSensitive (const Handle(V3d_View)& theView)
{
  if (mysensgroup.IsNull())
  {
    return;
  }

  mysensgroup->Clear();

  if (theView.IsNull())
  {
    return;
  }

  theView->Update();
}

//=======================================================================
//function : DisplaySenstive
//purpose  :
//=======================================================================
void StdSelect_ViewerSelector3d::DisplaySensitive (const Handle(SelectMgr_Selection)& theSel,
                                                   const Handle(V3d_View)& theView,
                                                   const Standard_Boolean theToClearOthers)
{
  if (mystruct.IsNull())
  {
    mystruct = new Graphic3d_Structure (theView->Viewer()->Viewer());
  }

  if (mysensgroup.IsNull())
  {
    mysensgroup = mystruct->NewGroup();
    Quantity_Color aColor (Quantity_NOC_INDIANRED3);
    Handle(Graphic3d_AspectMarker3d) aMarkerAspect =
      new Graphic3d_AspectMarker3d (Aspect_TOM_O_PLUS, aColor, 2.0);

    mysensgroup-> SetPrimitivesAspect (aMarkerAspect);
    mysensgroup->SetPrimitivesAspect (
      new Graphic3d_AspectLine3d (Quantity_NOC_GRAY40, Aspect_TOL_SOLID, 2.0));
  }

  if (theToClearOthers)
  {
    mysensgroup->Clear();
  }

  ComputeSensitivePrs (theSel);

  mystruct->SetDisplayPriority (10);
  mystruct->Display();

  theView->Update();
}

//=======================================================================
//function : DisplayAreas
//purpose  :
//=======================================================================
void StdSelect_ViewerSelector3d::DisplayAreas (const Handle(SelectMgr_Selection)& theSel,
                                               const Handle(V3d_View)& theView,
                                               const Standard_Boolean theToClearOthers)
{
  if (mystruct.IsNull())
  {
    mystruct = new Graphic3d_Structure (theView->Viewer()->Viewer());
  }

  if (mysensgroup.IsNull())
  {
    myareagroup = mystruct->NewGroup();
    myareagroup->SetGroupPrimitivesAspect (new Graphic3d_AspectLine3d (Quantity_NOC_AQUAMARINE1, Aspect_TOL_DASH, 1.0));
  }

  if (theToClearOthers)
  {
    myareagroup->Clear();
  }

  ComputeAreasPrs (theSel);

  mystruct->SetDisplayPriority (10);
  mystruct->Display();

  theView->Update();
}

//=======================================================================
//function : ComputeSensitivePrs
//purpose  :
//=======================================================================
void StdSelect_ViewerSelector3d::ComputeSensitivePrs (const Handle(SelectMgr_Selection)& theSel)
{
  TColgp_SequenceOfPnt aSeqLines, aSeqFree;
  TColStd_SequenceOfInteger aSeqBnds;

  for (theSel->Init(); theSel->More(); theSel->Next())
  {
    Handle(Select3D_SensitiveEntity) Ent = Handle(Select3D_SensitiveEntity)::DownCast(theSel->Sensitive());
    const Standard_Boolean hasloc = (Ent.IsNull()? Standard_False : Ent->HasLocation());

    TopLoc_Location theloc;
    if(hasloc)
      theloc = Ent->Location();

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
          theboxpoint[i].Transform (theloc.Transformation());
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
      aFace->Points3D(TheHPts);
      const TColgp_Array1OfPnt& ThePts = TheHPts->Array1();

      aSeqBnds.Append(ThePts.Length());
      for (Standard_Integer I = ThePts.Lower(); I <= ThePts.Upper(); I++)
      {
        if (hasloc)
          aSeqLines.Append(ThePts(I).Transformed (theloc.Transformation()));
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
          aSeqLines.Append(ThePts(I).Transformed (theloc.Transformation()));
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
      Select3D_SensitiveEntitySequence EntitySeq;
      aWire->GetEdges (EntitySeq);

      for (int i = 1; i <= EntitySeq.Length(); i++)
      {
        Handle(Select3D_SensitiveEntity) SubEnt = Handle(Select3D_SensitiveEntity)::DownCast(EntitySeq.Value(i));

        //Segment
        if (SubEnt->DynamicType()==STANDARD_TYPE(Select3D_SensitiveSegment))
        {
          gp_Pnt P1 (Handle(Select3D_SensitiveSegment)::DownCast(SubEnt)->StartPoint().XYZ());
          gp_Pnt P2 (Handle(Select3D_SensitiveSegment)::DownCast(SubEnt)->EndPoint().XYZ());
          if (hasloc)
          {
            P1.Transform(theloc.Transformation());
            P2.Transform(theloc.Transformation());
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
              aPnts[0].Transform (theloc.Transformation());
              aPnts[1].Transform (theloc.Transformation());
              aPnts[2].Transform (theloc.Transformation());
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
              aSeqLines.Append(ThePts(I).Transformed (theloc.Transformation()));
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
        P1.Transform (theloc.Transformation());
        P2.Transform (theloc.Transformation());
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
          aPnts[0].Transform (theloc.Transformation());
          aPnts[1].Transform (theloc.Transformation());
          aPnts[2].Transform (theloc.Transformation());
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
        Handle(Select3D_SensitivePoint)::DownCast(Ent)->Point().Transformed (theloc.Transformation());
      Handle(Graphic3d_ArrayOfPoints) anArrayOfPoints = new Graphic3d_ArrayOfPoints (1);
      anArrayOfPoints->AddVertex (P.X(), P.Y(), P.Z());
      mysensgroup->AddPrimitiveArray (anArrayOfPoints);
    }
    //============================================================
    // Triangulation : On met un petit offset ves l'interieur...
    //==========================================================
    else if (Ent->DynamicType()==STANDARD_TYPE(Select3D_SensitiveTriangulation))
    {
      const Handle(Poly_Triangulation)& PT =
        (*((Handle(Select3D_SensitiveTriangulation)*) &Ent))->Triangulation();

      const Poly_Array1OfTriangle& triangles = PT->Triangles();
      const TColgp_Array1OfPnt& Nodes = PT->Nodes();
      Standard_Integer n[3];

      TopLoc_Location iloc, bidloc;
      if ((*((Handle(Select3D_SensitiveTriangulation)*) &Ent))->HasInitLocation())
        bidloc = (*((Handle(Select3D_SensitiveTriangulation)*) &Ent))->GetInitLocation();

      if (bidloc.IsIdentity())
        iloc = theloc;
      else
        iloc = theloc * bidloc;

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
    myareagroup->AddPrimitiveArray(aPrims);
  }

  if (aSeqFree.Length())
  {
    mysensgroup->SetPrimitivesAspect (new Graphic3d_AspectLine3d (Quantity_NOC_GREEN, Aspect_TOL_SOLID, 2.0));
    Handle(Graphic3d_ArrayOfPolylines) aPrims = new Graphic3d_ArrayOfPolylines(aSeqFree.Length(),aSeqFree.Length()/2);
    for (i = 1; i <= aSeqFree.Length(); i++)
    {
      aPrims->AddBound(2);
      aPrims->AddVertex(aSeqLines(i++));
      aPrims->AddVertex(aSeqLines(i));
    }
    mysensgroup->AddPrimitiveArray(aPrims);
    mysensgroup->SetPrimitivesAspect (new Graphic3d_AspectLine3d (Quantity_NOC_GRAY40, Aspect_TOL_SOLID, 2.0));
  }
}

//=======================================================================
//function : ComputeAreaPrs
//purpose  :
//=======================================================================
void StdSelect_ViewerSelector3d::ComputeAreasPrs (const Handle(SelectMgr_Selection)& theSel)
{
  Standard_Real aXmin = 0.0;
  Standard_Real aYmin = 0.0;
  Standard_Real aXmax = 0.0;
  Standard_Real aYmax = 0.0;

  gp_Pnt aPbid;
  SelectBasics_ListOfBox2d aBoxList;

  TColgp_SequenceOfPnt aSeqLines;
  for (theSel->Init(); theSel->More(); theSel->Next())
  {
    theSel->Sensitive()->Areas (aBoxList);
    for (SelectBasics_ListIteratorOfListOfBox2d aBoxIt (aBoxList); aBoxIt.More(); aBoxIt.Next())
    {
      aBoxIt.Value().Get (aXmin, aYmin, aXmax, aYmax);

      aPbid.SetCoord (aXmin - mytolerance, aYmin - mytolerance, 0.0);
      myProjector->Transform (aPbid, myProjector->InvertedTransformation());
      aSeqLines.Append (aPbid);

      aPbid.SetCoord (aXmax + mytolerance, aYmin - mytolerance, 0.0);
      myProjector->Transform (aPbid, myProjector->InvertedTransformation());
      aSeqLines.Append (aPbid);

      aPbid.SetCoord (aXmax + mytolerance, aYmax + mytolerance, 0.0);
      myProjector->Transform (aPbid, myProjector->InvertedTransformation());
      aSeqLines.Append (aPbid);

      aPbid.SetCoord (aXmin - mytolerance, aYmax + mytolerance, 0.0);
      myProjector->Transform (aPbid, myProjector->InvertedTransformation());
      aSeqLines.Append (aPbid);
    }
  }

  if (aSeqLines.Length())
  {
    Standard_Integer aN = 0;
    Standard_Integer aNP = 0;
    const Standard_Integer aNBL = aSeqLines.Length() / 4;
    Handle(Graphic3d_ArrayOfPolylines) aPrims = new Graphic3d_ArrayOfPolylines (5 * aNBL, aNBL);
    for (aNP = 1, aN = 0; aN < aNBL; aN++)
    {
      aPrims->AddBound (5);
      const gp_Pnt &aP1 = aSeqLines (aNP++);
      aPrims->AddVertex (aP1);
      aPrims->AddVertex (aSeqLines (aNP++));
      aPrims->AddVertex (aSeqLines (aNP++));
      aPrims->AddVertex (aSeqLines (aNP++));
      aPrims->AddVertex (aP1);
    }
    myareagroup->AddPrimitiveArray (aPrims);
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
//function : ComputeClipRange
//purpose  :
//=======================================================================
void StdSelect_ViewerSelector3d::ComputeClipRange (const Graphic3d_SequenceOfHClipPlane& thePlanes,
                                                   const gp_Lin& thePickLine,
                                                   Standard_Real& theDepthMin,
                                                   Standard_Real& theDepthMax) const
{
  theDepthMin = RealFirst();
  theDepthMax = RealLast();
  Standard_Real aPlaneA, aPlaneB, aPlaneC, aPlaneD;

  Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt (thePlanes);
  for (; aPlaneIt.More(); aPlaneIt.Next())
  {
    const Handle(Graphic3d_ClipPlane)& aClipPlane = aPlaneIt.Value();
    if (!aClipPlane->IsOn())
      continue;

    gp_Pln aGeomPlane = aClipPlane->ToPlane();

    aGeomPlane.Coefficients (aPlaneA, aPlaneB, aPlaneC, aPlaneD);

    const gp_Dir& aPlaneDir = aGeomPlane.Axis().Direction();
    const gp_Dir& aPickDir  = thePickLine.Direction();
    const gp_XYZ& aPntOnLine = thePickLine.Location().XYZ();
    const gp_XYZ& aPlaneDirXYZ = aPlaneDir.XYZ();

    Standard_Real aDotProduct = aPickDir.Dot (aPlaneDir);
    Standard_Real aDistance = -(aPntOnLine.Dot (aPlaneDirXYZ) + aPlaneD);

    // check whether the pick line is parallel to clip plane
    if (Abs (aDotProduct) < Precision::Angular())
    {
      if (aDistance > 0.0)
      {
        // line lies above the plane, thus no selection is possible
        theDepthMin = 0.0;
        theDepthMax = 0.0;
        return;
      }

      // line lies below the plane and is not clipped, skip
      continue;
    }

    // compute distance to point of pick line intersection with the plane
    Standard_Real aIntDist = aDistance / aDotProduct;

    // change depth limits for case of opposite and directed planes
    if (aDotProduct < 0.0)
    {
      theDepthMax = Min (aIntDist, theDepthMax);
    }
    else if (aIntDist > theDepthMin)
    {
      theDepthMin = Max (aIntDist, theDepthMin);
    }
  }
}

//=======================================================================
//function : PickingLine
//purpose  :
//=======================================================================
gp_Lin StdSelect_ViewerSelector3d::PickingLine(const Standard_Real theX, const Standard_Real theY) const
{
  return myProjector->Shoot (theX, theY);
}

//=======================================================================
//function : DepthClipping
//purpose  :
//=======================================================================
void StdSelect_ViewerSelector3d::DepthClipping (const Standard_Real theX,
                                                const Standard_Real theY,
                                                Standard_Real& theDepthMin,
                                                Standard_Real& theDepthMax) const
{
  return ComputeClipRange (myClipPlanes, PickingLine (theX, theY), theDepthMin, theDepthMax);
}

//=======================================================================
//function : DepthClipping
//purpose  :
//=======================================================================
void StdSelect_ViewerSelector3d::DepthClipping (const Standard_Real theX,
                                                const Standard_Real theY,
                                                const Handle(SelectMgr_EntityOwner)& theOwner,
                                                Standard_Real& theDepthMin,
                                                Standard_Real& theDepthMax) const
{
  return ComputeClipRange (theOwner->Selectable()->GetClipPlanes(),
                           PickingLine (theX, theY),
                           theDepthMin, theDepthMax);
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
