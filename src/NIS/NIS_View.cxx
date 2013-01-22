// Created on: 2007-07-06
// Created by: Alexander GRIGORIEV
// Copyright (c) 2007-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <NIS_View.hxx>
#include <NIS_InteractiveContext.hxx>
#include <NIS_InteractiveObject.hxx>
#include <gp_Ax1.hxx>
#include <Visual3d_View.hxx>
#include <Bnd_B2f.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

#include <OpenGl_GlCore11.hxx>

IMPLEMENT_STANDARD_HANDLE  (NIS_View, V3d_OrthographicView)
IMPLEMENT_STANDARD_RTTIEXT (NIS_View, V3d_OrthographicView)

//=======================================================================
//function : NIS_View()
//purpose  : Constructor
//=======================================================================

NIS_View::NIS_View (const Handle(V3d_Viewer)&    theViewer,
                    const Handle(Aspect_Window)& theWindow)
  : V3d_OrthographicView (theViewer),
    myIsTopHilight(Standard_False),
    myDoHilightSelected(Standard_True)
{
  if (!theWindow.IsNull())
    V3d_View::SetWindow (theWindow, NULL, &MyCallback, this);
}

//=======================================================================
//function : SetWindow
//purpose  :
//=======================================================================

void NIS_View::SetWindow(const Handle(Aspect_Window) &theWindow)
{
  V3d_View::SetWindow (theWindow, NULL, &MyCallback, this);
}

// //=======================================================================
// //function : ~NIS_View
// //purpose  : Destructor
// //=======================================================================

// NIS_View::~NIS_View()
// {}


//=======================================================================
//function : AddContext
//purpose  :
//=======================================================================

void  NIS_View::AddContext (NIS_InteractiveContext * theCtx)
{
  // Check that the given context is not among already attached contexts
  NCollection_List<NIS_InteractiveContext *>::Iterator anIter (myContexts);
  for (; anIter.More(); anIter.Next())
    if (anIter.Value() == theCtx)
      break;
  if (anIter.More() == Standard_False)
    myContexts.Append (theCtx);
}

//=======================================================================
//function : RemoveContext
//purpose  :
//=======================================================================

void NIS_View::RemoveContext (NIS_InteractiveContext * theCtx)
{
  NCollection_List<NIS_InteractiveContext *>::Iterator anIter (myContexts);
  for (; anIter.More(); anIter.Next())
    if (anIter.Value() == theCtx) {
      myContexts.Remove (anIter);
      break;
    }

  NCollection_Map<Handle_NIS_Drawer>::Iterator anIterD (theCtx->GetDrawers ());
  for (; anIterD.More(); anIterD.Next()) {
    const Handle(NIS_Drawer)& aDrawer = anIterD.Value();
    if (aDrawer.IsNull() == Standard_False) {
      aDrawer->UpdateExListId(this);
    }
  }
}

//=======================================================================
//function : FitAll3d
//purpose  :
//=======================================================================

Standard_Boolean NIS_View::FitAll3d (const Quantity_Coefficient theCoef)
{
  Standard_Boolean aResult(Standard_False);
  /*
  Standard_Integer aLimp[4] = { 1000000, -1000000, 1000000, -1000000 };
  GetBndBox( aLimp[0], aLimp[1], aLimp[2], aLimp[3] );
  if (aLimp[1] > -1000000 && aLimp[3] > -1000000 &&
      aLimp[0] < aLimp[1] && aLimp[2] < aLimp[3])
  {
    // Scale the view
    WindowFit (aLimp[0], aLimp[2], aLimp[1], aLimp[3]);
    aResult = Standard_True;
  }
  */

  Bnd_B3f aBox = GetBndBox();

  // Check that the box is not empty
  if (aBox.IsVoid() == Standard_False && MyView->IsDefined() == Standard_True) {
    // Convert the 3D box to 2D representation in view coordinates
    Standard_Real Umin,Umax,Vmin,Vmax,U,V,W;
    gp_XYZ aCoord;

    const gp_XYZ aCorner[2] = { aBox.CornerMin(), aBox.CornerMax() };

    Standard_Boolean doFit = Standard_True;
    while (doFit) {

    for (Standard_Integer i = 0; i < 8; i++) {
      if (i & 0x1) aCoord.SetX (aCorner[0].X());
      else         aCoord.SetX (aCorner[1].X());
      if (i & 0x2) aCoord.SetY (aCorner[0].Y());
      else         aCoord.SetY (aCorner[1].Y());
      if (i & 0x4) aCoord.SetZ (aCorner[0].Z());
      else         aCoord.SetZ (aCorner[1].Z());

      MyView->Projects(aCoord.X(), aCoord.Y(), aCoord.Z(), U, V, W);
      if (i) {
        Umin = Min(Umin, U); Umax = Max(Umax, U);
        Vmin = Min(Vmin, V); Vmax = Max(Vmax, V);
      }
      else {
        Umin = Umax = U;
        Vmin = Vmax = V;
      }
    }

    if ( (Umax > Umin) && (Vmax > Vmin) ) {
      Standard_Real OldUmin,OldUmax,OldVmin,OldVmax;
      MyViewMapping.WindowLimit(OldUmin, OldVmin, OldUmax, OldVmax);
      Standard_Real DxvOld = Abs(OldUmax - OldUmin);

      // make a margin
      Standard_Real Xrp, Yrp, DxvNew, DyvNew;

      DxvNew = Abs(Umax - Umin); DyvNew = Abs(Vmax - Vmin);
      DxvNew *= (1. + theCoef);
      DyvNew *= (1. + theCoef);

      Standard_Real aRatio = DxvNew / DxvOld;

      Xrp = (Umin + Umax)/2. ; Yrp = (Vmin + Vmax)/2. ;
      Umin = Xrp - DxvNew/2. ; Umax = Xrp + DxvNew/2. ;
      Vmin = Yrp - DyvNew/2. ; Vmax = Yrp + DyvNew/2. ;

      // fit view
      FitAll(Umin, Vmin, Umax, Vmax);

      // ratio 1e+6 often gives calculation error(s), reduce it
      // if (aRatio < 1e+6) doFit = Standard_False;
      if (aRatio < 100) doFit = Standard_False;
      aResult = Standard_True;
    }
    else doFit = Standard_False;

    }
  }

  return aResult;
}

//=======================================================================
//function : GetBndBox
//purpose  :
//=======================================================================

Bnd_B3f NIS_View::GetBndBox() const
{
  // Calculate the 3D bounding box of visible objects
  // in all interactive contexts
  Bnd_B3f aBox;
  NCollection_List<NIS_InteractiveContext *>::Iterator anIterC (myContexts);
  for (; anIterC.More(); anIterC.Next()) {
    NCollection_Map<Handle_NIS_Drawer>::Iterator anIterD
      (anIterC.Value()->myDrawers);
    for (; anIterD.More(); anIterD.Next()) {
      const Handle(NIS_Drawer)& aDrawer = anIterD.Value();
      Bnd_B3f aBoxD = aDrawer->GetBox (this);
      aBox.Add (aBoxD);
    }
  }

  // Take the bounding box of AIS objects displayed in the view
  Standard_Real aVal[6];
  View()->MinMaxValues(aVal[0], aVal[1], aVal[2], aVal[3], aVal[4], aVal[5]);
  if (aVal[3] < 0.5 * RealLast()) {
    aBox.Add (gp_XYZ (aVal[0], aVal[1], aVal[2]));
    aBox.Add (gp_XYZ (aVal[3], aVal[4], aVal[5]));
  }

  return aBox;
}

//=======================================================================
//function : GetBndBox
//purpose  :
//=======================================================================

void NIS_View::GetBndBox( Standard_Integer& theXMin, Standard_Integer& theXMax,
                          Standard_Integer& theYMin, Standard_Integer& theYMax ) const
{
  theXMin = theYMin = 0;
  theXMax = theYMax = -1;

  Bnd_B3f aBox = GetBndBox();

  // Check that the box is not empty
  if (aBox.IsVoid() == Standard_False) {
    // Convert the 3D box to 2D representation in pixel coordinates
    gp_XYZ aCoord;
    Standard_Integer anXp, anYp;
    const gp_XYZ aCorner[2] = { aBox.CornerMin(), aBox.CornerMax() };
    Standard_Integer aLimp[4] = { 1000000, -1000000, 1000000, -1000000 };
    for (Standard_Integer i = 0; i < 8; i++) {
      if (i & 0x1) aCoord.SetX (aCorner[0].X());
      else         aCoord.SetX (aCorner[1].X());
      if (i & 0x2) aCoord.SetY (aCorner[0].Y());
      else         aCoord.SetY (aCorner[1].Y());
      if (i & 0x4) aCoord.SetZ (aCorner[0].Z());
      else         aCoord.SetZ (aCorner[1].Z());
      Convert( aCoord.X(), aCoord.Y(), aCoord.Z(), anXp, anYp );
      if (aLimp[0] > anXp) aLimp[0] = anXp;
      if (aLimp[1] < anXp) aLimp[1] = anXp;
      if (aLimp[2] > anYp) aLimp[2] = anYp;
      if (aLimp[3] < anYp) aLimp[3] = anYp;
    }
    if (aLimp[0] < aLimp[1] && aLimp[2] < aLimp[3])
    {
      // Scale the view
      // WindowFit (aLimp[0], aLimp[2], aLimp[1], aLimp[3]);
      theXMin = aLimp[0];
      theXMax = aLimp[1];
      theYMin = aLimp[2];
      theYMax = aLimp[3];
    }
  }
}


//=======================================================================
//function : MyCallback
//purpose  :
//=======================================================================

int NIS_View::MyCallback (Aspect_Drawable                /* Window ID */,
                          void*                          ptrData,
                          Aspect_GraphicCallbackStruct*  callData /* call data */)
{
  // Avoid multiple rendering of the scene ( accordingly with update of
  // callback mechanism, that invokes additional callbacks before
  // underlay and overlay redrawing with OCC_PRE_REDRAW and OCC_PRE_OVERLAY
  // bits added to the "reason" value of the callback data structure;
  // see comments to OCC_REDRAW_ADDITIONAL_CALLBACKS definition )
  if (callData->reason & OCC_REDRAW_ADDITIONAL_CALLBACKS)
    return 0;

  const Handle(NIS_View) thisView (static_cast<NIS_View *> (ptrData));
  NCollection_List<NIS_InteractiveContext *>::Iterator anIter;
#ifdef CLIP
  // Find the bounding box of all displayed objects by summing the boxes stored
  // in the relevant DrawList instances.
  Bnd_B3f aBndBox;
  for (anIter = thisView->myContexts; anIter.More(); anIter.Next())
    anIter.Value()->GetBox (aBndBox, pView);

  if (aBndBox.IsVoid() == Standard_False) {
    const gp_XYZ aBoxSize   = 0.5 * (aBndBox.CornerMax() - aBndBox.CornerMin());
    const gp_XYZ aBoxCenter = 0.5 * (aBndBox.CornerMax() + aBndBox.CornerMin());

    // Find the ray passing through the clicked point in the view window.
    Standard_Real anX, anY, aZ;
    thisView->Convert(0, 0, anX, anY, aZ);  // 3D point for the 3D coordinates
    const gp_Pnt anEye (anX, anY, aZ);
    thisView->Proj (anX, anY, aZ);  // vector orthogonal to the view plane
    const gp_Dir aProj (anX, anY, aZ);
    const gp_Ax1 anAxis (anEye, aProj);

    const Standard_Real aCenterDist = (anEye.XYZ() - aBoxCenter) * aProj.XYZ();
    const Standard_Real aBoxExtent /*(fabs(aBoxSize.X() * anX) +
                                    fabs(aBoxSize.Y() * anY) +
                                    fabs(aBoxSize.Z() * aZ))*/(100.);

#define FRONT_CLIPPING_PLANE (GL_CLIP_PLANE0 + 0)
#define BACK_CLIPPING_PLANE  (GL_CLIP_PLANE0 + 1)
    Standard_Real arr[4] = {
      0.0,  /* Nx */
      0.0,  /* Ny */
      1.0,  /* Nz */
      0.
    };
    arr[3] = aBoxExtent + 1.;
    glClipPlane (BACK_CLIPPING_PLANE, arr);
    glEnable (BACK_CLIPPING_PLANE);
    arr[2] = -1.0;
    arr[3] = aBoxExtent + 1.;
    glClipPlane (FRONT_CLIPPING_PLANE, arr);
    glEnable (FRONT_CLIPPING_PLANE);
  }
#endif //IS_DISABLED

  GLboolean isDepthWriteMask, isDepthTest;
  glGetBooleanv(GL_DEPTH_WRITEMASK,&isDepthWriteMask);
  glGetBooleanv(GL_DEPTH_TEST,&isDepthTest);
//   printf ("GlDepthMask=%d; GlDepthTest=%d\n", depthwritemask, depthtest);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_EDGE_FLAG_ARRAY);
  glDisableClientState(GL_INDEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  if (!isDepthTest) {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearDepth(1.);
    glClear(GL_DEPTH_BUFFER_BIT);
  }

  TColStd_MapIteratorOfPackedMapOfInteger anIterM(thisView->myExListId);
  for (; anIterM.More(); anIterM.Next())
    if (anIterM.Key() != 0) {
#ifdef ARRAY_LISTS
      glDeleteLists (anIterM.Key(), 5);
#else
      glDeleteLists (anIterM.Key(), 1);
    }
#endif
  thisView->myExListId.Clear();

  for (anIter = thisView->myContexts; anIter.More(); anIter.Next())
    anIter.Value()->redraw (thisView, NIS_Drawer::Draw_Normal);

  // #818151 - selected object is hidden by covered unselected one
  // display hilighted objects always above the rest ones
  if (thisView->myIsTopHilight == Standard_True) {
    glDepthFunc(GL_ALWAYS);
  }

  for (anIter = thisView->myContexts; anIter.More(); anIter.Next())
    anIter.Value()->redraw (thisView, NIS_Drawer::Draw_Hilighted);
  for (anIter = thisView->myContexts; anIter.More(); anIter.Next())
    anIter.Value()->redraw (thisView, NIS_Drawer::Draw_DynHilighted);
  for (anIter = thisView->myContexts; anIter.More(); anIter.Next())
    anIter.Value()->redraw (thisView, NIS_Drawer::Draw_Transparent);

  // draw top objects always above
  if (thisView->myIsTopHilight == Standard_False) {
    glDepthFunc(GL_ALWAYS);
  }

  for (anIter = thisView->myContexts; anIter.More(); anIter.Next())
    anIter.Value()->redraw (thisView, NIS_Drawer::Draw_Top);

  return 0;
}

//=======================================================================
//function : DynamicHilight
//purpose  :
//=======================================================================

void NIS_View::DynamicHilight  (const Standard_Integer theX,
                                const Standard_Integer theY)
{
  myDetected.Clear();
  const Handle(NIS_InteractiveObject) aSelected = Pick (theX, theY);

  // ASV: if at least one Context returns IsSelectable()==False,
  // hilight is canceled, this method returns
  if (aSelected.IsNull() == Standard_False) {
    if (aSelected->IsSelectable() == Standard_False)
      return;
  }
  if (aSelected != myDynHilighted) {
    const Handle(NIS_View) aView (this);
    if (myDynHilighted.IsNull() == Standard_False)
      if (myDynHilighted->GetDrawer().IsNull() == Standard_False)
        myDynHilighted->GetDrawer()->SetDynamicHilighted(Standard_False,
                                                         myDynHilighted, aView);

    // 30.07.10 - NKV - synchronize behaviour with AIS interactive context (if need)
    if (aSelected.IsNull() ||
        (myDoHilightSelected == Standard_False &&
         aSelected->GetDrawer()->GetContext()->IsSelected(aSelected)))
    {
      myDynHilighted.Nullify();
    }
    else {
      aSelected->GetDrawer()->SetDynamicHilighted (Standard_True,
                                                   aSelected, aView);
      myDynHilighted = aSelected;
    }
    Redraw();
  }
}

//=======================================================================
//function : DynamicUnhilight
//purpose  :
//=======================================================================

void NIS_View::DynamicUnhilight(const Handle_NIS_InteractiveObject& theObj)
{
  if (theObj == myDynHilighted && theObj.IsNull() == Standard_False) {
    const Handle(NIS_View) aView (this);
    if (myDynHilighted->GetDrawer().IsNull() == Standard_False)
      myDynHilighted->GetDrawer()->SetDynamicHilighted (Standard_False,
                                                        myDynHilighted, aView);
    myDynHilighted.Nullify();
    Redraw();
  }
}

//=======================================================================
//function : Select
//purpose  : selection by single click
//=======================================================================

void NIS_View::Select (const Standard_Integer theX,
                       const Standard_Integer theY,
                       const Standard_Boolean isForceMultiple,
                       const Standard_Boolean theRedraw)
{
  myDetected.Clear();
  const Handle(NIS_InteractiveObject) aSelected = Pick (theX, theY);
  NCollection_List<NIS_InteractiveContext *>::Iterator anIter (myContexts);
  for (; anIter.More(); anIter.Next())
    anIter.Value()->ProcessSelection (aSelected, isForceMultiple);
  if (aSelected == myDynHilighted && aSelected.IsNull() == Standard_False)
  {
    myDynHilighted.Nullify();
    const Handle(NIS_Drawer)& aDrawer = aSelected->GetDrawer();
    aDrawer->SetDynamicHilighted (Standard_False, aSelected, this);
  }
  if (theRedraw) Redraw();
}

//=======================================================================
//function : Select
//purpose  : selection by rectange
//=======================================================================

void NIS_View::Select (const Standard_Integer  theXmin,
                       const Standard_Integer  theYmin,
                       const Standard_Integer  theXmax,
                       const Standard_Integer  theYmax,
                       const Standard_Boolean  isForceMult,
                       const Standard_Boolean  isFullyIncluded,
                       const Standard_Boolean  theRedraw)
{
  myDetected.Clear();
  Standard_Real anX, anY, aZ;
  if (theXmin == theXmax || theYmin == theYmax)
    return;

  //Transformed box corresponding to the selected rectangle
  Proj (anX, anY, aZ);                  // vector orthogonal to the view plane
  const gp_Dir aProj (anX, anY, aZ);

  Convert(theXmin, theYmin, anX, anY, aZ); // 3D point for the 3D coordinates
  const gp_Pnt anEye (anX, anY, aZ);

  Convert(theXmax, theYmin, anX, anY, aZ); // 3D point for the 3D coordinates
  const gp_XYZ anXdir (gp_XYZ(anX, anY, aZ) - anEye.XYZ());
  const gp_Ax3 anAx3 (anEye, aProj, anXdir);
  gp_Trsf aTrf;
  aTrf.SetTransformation (anAx3);
  const gp_Trsf aTrfInv = aTrf.Inverted();

  Convert(theXmax, theYmax, anX, anY, aZ); // 3D point for the 3D coordinates
  gp_XYZ anUpperCorner (anX, anY, aZ);
  aTrf.Transforms(anUpperCorner);

  // Selecting box
  Bnd_B3f aBoxSel;
  aBoxSel.Add (gp_XYZ(0., 0., -10000.));
  aBoxSel.Add (anUpperCorner);

  TColStd_PackedMapOfInteger mapSelected;
  NCollection_List<NIS_InteractiveContext *>::Iterator anIterC (myContexts);
  for (; anIterC.More(); anIterC.Next()) {
    NIS_InteractiveContext * pCtx = anIterC.Value();
    mapSelected.Clear();
    pCtx->selectObjects (mapSelected, aBoxSel, aTrfInv, aTrf, isFullyIncluded);
    pCtx->ProcessSelection (mapSelected, isForceMult);
  }
  if (theRedraw) Redraw();
}

//=======================================================================
//function : Select
//purpose  : Selection by polygon
//=======================================================================

void  NIS_View::Select (const NCollection_List<gp_XY> &thePolygon,
                        const Standard_Boolean         isForceMult,
                        const Standard_Boolean         isFullyIncluded,
                        const Standard_Boolean         theRedraw)
{
  myDetected.Clear();
  if (thePolygon.IsEmpty())
    return;

  Standard_Real anX, anY, aZ;

  //Transformed box corresponding to the selected rectangle
  Proj (anX, anY, aZ);                  // vector orthogonal to the view plane
  const gp_Dir aProj (anX, anY, aZ);

  const gp_XY &aPf = thePolygon.First();
  // 3D point for the 3D coordinates
  Convert((Standard_Integer) aPf.X(), (Standard_Integer) aPf.Y(), anX, anY, aZ);
  const gp_Pnt anEye (anX, anY, aZ);

  // 3D point for the 3D coordinates
  const gp_XY &aPl = thePolygon.Last();

  Convert((Standard_Integer) aPl.X(), (Standard_Integer) aPl.Y(), anX, anY, aZ);

  // Compute transformation.
  const gp_XYZ anXdir (gp_XYZ(anX, anY, aZ) - anEye.XYZ());
  if (anXdir.Modulus() <= gp::Resolution())
  {
    return;
  }

  const gp_Ax3 anAx3 (anEye, aProj, anXdir);
  gp_Trsf aTrf;
  aTrf.SetTransformation (anAx3);
  const gp_Trsf aTrfInv = aTrf.Inverted();

  // Prepare list of 2d points of selection polygon.
  NCollection_List<gp_XY>           aPoints;
  NCollection_List<gp_XY>::Iterator anIter(thePolygon);
  Bnd_B2f                           aPolyBox;

  for (; anIter.More(); anIter.Next()) {
    const gp_XY &aP = anIter.Value();

    Convert((Standard_Integer) aP.X(), (Standard_Integer) aP.Y(), anX, anY, aZ);
    gp_XYZ aP3d(anX, anY, aZ);

    aTrf.Transforms(aP3d);

    gp_XY aP2d(aP3d.X(), aP3d.Y());

    aPoints.Append(aP2d);
    aPolyBox.Add(aP2d);
  }

  TColStd_PackedMapOfInteger                           mapSelected;
  NCollection_List<NIS_InteractiveContext *>::Iterator anIterC(myContexts);

  for (; anIterC.More(); anIterC.Next()) {
    NIS_InteractiveContext * pCtx = anIterC.Value();
    mapSelected.Clear();
    pCtx->selectObjects (mapSelected, aPoints, aPolyBox, aTrf, isFullyIncluded);
    pCtx->ProcessSelection (mapSelected, isForceMult);
  }

  if (theRedraw) Redraw();
}

//=======================================================================
//function : Pick
//purpose  :
//=======================================================================

Handle_NIS_InteractiveObject NIS_View::Pick (const Standard_Integer theX,
                                             const Standard_Integer theY)
{
  // Find the ray passing through the clicked point in the view window.
  Standard_Real anX, anY, aZ, anOver;
  Convert(theX, theY, anX, anY, aZ);  // 3D point for the 3D coordinates
  const gp_Pnt anEye (anX, anY, aZ);
  Proj (anX, anY, aZ);                // vector orthogonal to the view plane
  const gp_Dir aProj (-anX, -anY, -aZ);
  const gp_Ax1 anAxis (anEye, aProj);

  Convert (theX+1, theY+1, anX, anY, aZ);
  anOver = ((gp_XYZ(anX, anY, aZ) - anEye.XYZ()) ^ aProj.XYZ()).Modulus() * 1.5;

  return Pick(anAxis, anOver, Standard_True);
}

//=======================================================================
//function : Pick
//purpose  :
//=======================================================================

Handle_NIS_InteractiveObject NIS_View::Pick
                                (const gp_Ax1&          theAxis,
                                 const Standard_Real    theOver,
                                 const Standard_Boolean isOnlySelectable)
{
  typedef NCollection_List<NIS_InteractiveContext::DetectedEnt> LstDetected;
  Standard_Real                 aDistance (0.1 * RealLast());
  Handle(NIS_InteractiveObject) aSelected, aTmpSel;
  LstDetected aDetected;

  NCollection_List<NIS_InteractiveContext *>::Iterator anIterC (myContexts);
  for (; anIterC.More(); anIterC.Next()) {
    const Standard_Real aDist =
      anIterC.Value()->selectObject (aTmpSel, aDetected, theAxis, theOver,
                                     isOnlySelectable);
    if (aDist < aDistance) {
      aDistance = aDist;
      aSelected = aTmpSel;
    }
  }

  // simple iterating is enough to create list of detected objects
  // in the order of increasing distance
  myDetected.Clear();
  for (LstDetected::Iterator anIt(aDetected); anIt.More(); anIt.Next())
    myDetected.Append(anIt.Value().PObj);

  return aSelected;
}

