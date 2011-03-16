// File:      NIS_View.cxx
// Created:   06.07.07 19:51
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2007

#include <NIS_View.hxx>
#include <NIS_InteractiveContext.hxx>
#include <NIS_InteractiveObject.hxx>
#include <gp_Ax1.hxx>
#include <Visual3d_View.hxx>
#ifdef WNT
#include <Windows.h>
#endif
#include <GL/gl.h>

IMPLEMENT_STANDARD_HANDLE  (NIS_View, V3d_OrthographicView)
IMPLEMENT_STANDARD_RTTIEXT (NIS_View, V3d_OrthographicView)

//=======================================================================
//function : NIS_View()
//purpose  : Constructor
//=======================================================================

NIS_View::NIS_View (const Handle(V3d_Viewer)&    theViewer,
                    const Handle(Aspect_Window)& theWindow)
  : V3d_OrthographicView (theViewer)
{
  if (!theWindow.IsNull()) {
    const Aspect_GraphicCallbackProc aCallback = &MyCallback;
    V3d_View::SetWindow (theWindow, NULL, aCallback, this);
  }
}

//=======================================================================
//function : SetWindow
//purpose  : 
//=======================================================================

void NIS_View::SetWindow(const Handle(Aspect_Window) &theWindow)
{
  const Aspect_GraphicCallbackProc aCallback = &MyCallback;
  V3d_View::SetWindow (theWindow, NULL, aCallback, this);
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
}

//=======================================================================
//function : FitAll3d
//purpose  : 
//=======================================================================

void NIS_View::FitAll3d ()
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

  if (aBox.IsVoid()) {
    // No NIS objects displays, run the compatible method of V3d_View
    FitAll();
    return;
  }

  // Take the bounding box of AIS objects displayed in the view
  Standard_Real aVal[6];
  View()->MinMaxValues(aVal[0], aVal[1], aVal[2], aVal[3], aVal[4], aVal[5]);
  if (aVal[3] < 0.5 * RealLast()) {
    aBox.Add (gp_XYZ (aVal[0], aVal[1], aVal[2]));
    aBox.Add (gp_XYZ (aVal[3], aVal[4], aVal[5]));
  }

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
      WindowFit (aLimp[0], aLimp[2], aLimp[1], aLimp[3]);
      return;
    } 
  }
}


//=======================================================================
//function : MyCallback
//purpose  : 
//=======================================================================

int NIS_View::MyCallback (Aspect_Drawable                /* Window ID */,
                          void*                          ptrData, 
                          Aspect_GraphicCallbackStruct*  /* call data */)
{
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

//  glEnable(GL_COLOR_MATERIAL);
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

  for (anIter = thisView->myContexts; anIter.More(); anIter.Next())
    anIter.Value()->redraw (thisView, NIS_Drawer::Draw_Normal);
  for (anIter = thisView->myContexts; anIter.More(); anIter.Next())
    anIter.Value()->redraw (thisView, NIS_Drawer::Draw_Transparent);
  for (anIter = thisView->myContexts; anIter.More(); anIter.Next())
    anIter.Value()->redraw (thisView, NIS_Drawer::Draw_Hilighted);
  for (anIter = thisView->myContexts; anIter.More(); anIter.Next())
    anIter.Value()->redraw (thisView, NIS_Drawer::Draw_DynHilighted);

  return 0;
}

//=======================================================================
//function : DynamicHilight
//purpose  : 
//=======================================================================

void NIS_View::DynamicHilight  (const Standard_Integer theX,
                                const Standard_Integer theY)
{
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
      myDynHilighted->GetDrawer()->SetDynamicHilighted (Standard_False,
                                                        myDynHilighted, aView);
    if (aSelected.IsNull())
      myDynHilighted.Nullify();
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
                       const Standard_Boolean isForceMultiple)
{
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
  Redraw();
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
                       const Standard_Boolean  isFullyIncluded)
{
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
  Redraw();
}


//=======================================================================
//function : Pick
//purpose  : 
//=======================================================================

Handle_NIS_InteractiveObject NIS_View::Pick
                                (const Standard_Integer theX,
                                 const Standard_Integer theY) const
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
                                 const Standard_Boolean isOnlySelectable) const
{
  Standard_Real                 aDistance (0.1 * RealLast());
  Handle(NIS_InteractiveObject) aSelected, aTmpSel;

  NCollection_List<NIS_InteractiveContext *>::Iterator anIterC (myContexts);
  for (; anIterC.More(); anIterC.Next()) {
    const Standard_Real aDist =
      anIterC.Value()->selectObject (aTmpSel, theAxis, theOver,
                                     isOnlySelectable);
    if (aDist < aDistance) {
      aDistance = aDist;
      aSelected = aTmpSel;
    }
  }
  return aSelected;
}
