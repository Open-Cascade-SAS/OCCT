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

/***********************************************************************

     FONCTION :
     ----------
        Classe Visual3d_ViewManager.cxx :

	Declaration of variables specific to visualisers

     HISTORIQUE DES MODIFICATIONS   :
     --------------------------------
      Mars 1992 : NW,JPB,CAL ; Creation.
      19-06-96  : FMN ; Suppression variables inutiles
      04-02-97  : FMN ; Suppression de PSOutput, XWDOutput ...
      06-05-97  : CAL ; Ajout du Clear sur les TOS_COMPUTED.
      19-09-97  : CAL ; Remplacement de Window->Position par Window->Size;
      24-10-97  : CAL ; Retrait de DownCast.
      20-11-97  : CAL ; Disparition de la dependance avec math
      01-12-97  : CAL ; Retrait du test IsActive sur l'Update et le Redraw
      31-12-97  : CAL ; Disparition de MathGra
      16-01-98  : CAL ; Ajout du SetTransform sur une TOS_COMPUTED
      11-03-98  : CAL ; Visual3d_ViewManager::Remove ()
      20-05-98  : CAL ; Perfs. Connection entre structures COMPUTED.
      10-06-98  : CAL ; Modification des signatures de xxProjectRaster.
      10-06-98  : CAL ; Modification de la signature de ViewExists.
      01-12-98  : CAL ; S4062. Ajout des layers.
      02-12-98  : CAL ; Remove () ne detruit plus les vues.

************************************************************************/

/*----------------------------------------------------------------------*/
/*
 * Constants
 */

#define NO_DOWNCAST
#define NO_DESTROY

/*----------------------------------------------------------------------*/
/*
 * Includes
 */

// for the class
#include <Visual3d_ViewManager.ixx>

#include <Standard_ErrorHandler.hxx>

#include <Aspect.hxx>
#include <Aspect_IdentDefinitionError.hxx>

#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_MapOfStructure.hxx>
#include <Graphic3d_MapIteratorOfMapOfStructure.hxx>


#if defined (_WIN32) || defined(__WIN32__)
# include <WNT_Window.hxx>
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
# include <Cocoa_Window.hxx>
#else
# include <Xw_Window.hxx>
#endif

// views identifiers : possible range
#define View_IDMIN	1
#define View_IDMAX	10000

Visual3d_ViewManager::Visual3d_ViewManager (const Handle(Graphic3d_GraphicDriver)& theDriver):
Graphic3d_StructureManager (theDriver),
MyDefinedView (),
MyViewGenId (View_IDMIN+((View_IDMIN+View_IDMAX)/(Visual3d_ViewManager::Limit ()))*(Visual3d_ViewManager::CurrentId ()-1),View_IDMIN+((View_IDMIN+View_IDMAX)/(Visual3d_ViewManager::Limit ()))*Visual3d_ViewManager::CurrentId ()-1),
MyZBufferAuto (Standard_False),
MyTransparency (Standard_False)
{
  // default layer is always presented in display layer sequence
  // it can not be removed
  myLayerIds.Add (0);
  myLayerSeq.Append (0);

  MyGraphicDriver = theDriver;
  myMapOfZLayerSettings.Bind (0, Graphic3d_ZLayerSettings());
}

//-Destructors

void Visual3d_ViewManager::Destroy () {

#ifdef DESTROY
	cout << "Visual3d_ViewManager::Destroy (" << MyId << ")\n" << flush;
#endif

	Remove ();
}

//-Methods, in order

void Visual3d_ViewManager::Remove () {

#ifdef DESTROY
	cout << "Visual3d_ViewManager::Remove (" << MyId << ")\n" << flush;
#endif

	//
	// Destroy all defined views
	//

#ifdef DESTROY
	cout << "The Manager " << MyId << " have " << Length << " defined views\n";
	cout << flush;
#endif

  // clear all structures whilst views are alive for correct GPU memory management
  MyDisplayedStructure.Clear();
  MyHighlightedStructure.Clear();
  MyPickStructure.Clear();

  // clear list of managed views
  MyDefinedView.Clear();
}

void Visual3d_ViewManager::ChangeDisplayPriority (const Handle(Graphic3d_Structure)& AStructure, const Standard_Integer OldPriority, const Standard_Integer NewPriority)
{

#ifdef TRACE
  cout << "Visual3d_ViewManager::ChangeDisplayPriority ("
    << AStructure->Identification ()
    << ", " << OldPriority << ", " << NewPriority << ")\n";
  cout << flush;
#endif

  //
  // Change structure priority in all defined views
  //
  for(int i=1; i<=MyDefinedView.Length(); i++)
  {
    (MyDefinedView.Value(i))->ChangeDisplayPriority(AStructure, OldPriority, NewPriority);
  }
}

void Visual3d_ViewManager::ReCompute (const Handle(Graphic3d_Structure)& AStructure) {

  //Standard_Integer LengthD	= MyDisplayedStructure.Extent() ();

  // Even if physically the structure cannot
  // be displayed (pb of visualisation type)
  // it has status Displayed.

  if (!MyDisplayedStructure.Contains(AStructure))
    return;

  //
  // Recompute structure in all activated views
  //
  for(int i=1; i<=MyDefinedView.Length(); i++)
  {
    (MyDefinedView.Value(i))->ReCompute(AStructure);
  }
}

void Visual3d_ViewManager::ReCompute (const Handle(Graphic3d_Structure)& AStructure,
				      const Handle(Graphic3d_DataStructureManager)& AProjector)
{

  if (! AProjector->IsKind (STANDARD_TYPE (Visual3d_View))) return;

#ifdef DOWNCAST
  Handle(Visual3d_View) theView = Handle(Visual3d_View)::DownCast (AProjector);
#else
  Handle(Visual3d_View) theView = *(Handle(Visual3d_View) *) &AProjector;
#endif
  Standard_Integer ViewId = theView->Identification ();

  // Even if physically the structure cannot
  // be displayed (pb of visualisation type)
  // it has status Displayed.
  if (!MyDisplayedStructure.Contains(AStructure))
    return;

  //
  // Recompute structure in all activated views
  //
  for(int i=1; i<=MyDefinedView.Length(); i++)
  {
    if ((MyDefinedView.Value(i))->Identification () == ViewId)
    {
      theView->ReCompute (AStructure);
    }
  }
}

void Visual3d_ViewManager::Clear (const Handle(Graphic3d_Structure)& AStructure, const Standard_Boolean WithDestruction)
{
  for(int i=1; i<=MyDefinedView.Length(); i++)
  {
    (MyDefinedView.Value(i))->Clear(AStructure, WithDestruction);
  }
}

void Visual3d_ViewManager::Connect (const Handle(Graphic3d_Structure)& AMother, const Handle(Graphic3d_Structure)& ADaughter)
{
  for(int i=1; i<=MyDefinedView.Length(); i++)
  {
    (MyDefinedView.Value(i))->Connect (AMother, ADaughter);
  }
}

void Visual3d_ViewManager::Disconnect (const Handle(Graphic3d_Structure)& AMother, const Handle(Graphic3d_Structure)& ADaughter)
{
  for(int i=1; i<=MyDefinedView.Length(); i++)
  {
    (MyDefinedView.Value(i))->Disconnect (AMother, ADaughter);
  }
}

void Visual3d_ViewManager::Display (const Handle(Graphic3d_Structure)& AStructure)
{
  // Even if physically the structure cannot
  // be displayed (pb of visualisation type)
  // it has status Displayed.

  MyDisplayedStructure.Add(AStructure);
  
  for(int i=1; i<=MyDefinedView.Length(); i++)
  {
    (MyDefinedView.Value(i))->Display(AStructure);
  }
}

void Visual3d_ViewManager::Erase (const Handle(Graphic3d_Structure)& AStructure)
{
  // Even if physically the structure cannot
  // be displayed (pb of visualisation type)
  // it has status Displayed.

  MyDisplayedStructure.Remove(AStructure);

  //
  // Erase structure in all defined views
  //
  for(int i=1; i<=MyDefinedView.Length(); i++)
  {
    (MyDefinedView.Value(i))->Erase (AStructure);
  }

  MyHighlightedStructure.Remove (AStructure);
  MyPickStructure.Remove (AStructure);
}

void Visual3d_ViewManager::Erase () {

 Graphic3d_MapIteratorOfMapOfStructure it( MyDisplayedStructure);

 for (; it.More(); it.Next()) {
   Handle(Graphic3d_Structure) SG = it.Key();
   SG->Erase();
 }

}

void Visual3d_ViewManager::Highlight (const Handle(Graphic3d_Structure)& AStructure, const Aspect_TypeOfHighlightMethod AMethod)
{
  MyHighlightedStructure.Add(AStructure);
  
  //
  // Highlight in all activated views
  //
  
  for(int i=1; i<=MyDefinedView.Length(); i++)
  {
    (MyDefinedView.Value(i))->Highlight (AStructure, AMethod);
  }
}

void Visual3d_ViewManager::SetTransform (const Handle(Graphic3d_Structure)& AStructure, const TColStd_Array2OfReal& ATrsf)
{
  for(int i=1; i<=MyDefinedView.Length(); i++)
  {
    (MyDefinedView.Value(i))->SetTransform (AStructure, ATrsf);
  }
}

void Visual3d_ViewManager::UnHighlight () {

  Graphic3d_MapIteratorOfMapOfStructure it(MyHighlightedStructure);

  for (; it.More(); it.Next()) {
    Handle(Graphic3d_Structure) SG = it.Key();
    SG->UnHighlight ();
  }


}

void Visual3d_ViewManager::UnHighlight (const Handle(Graphic3d_Structure)& AStructure)
{
  MyHighlightedStructure.Remove(AStructure);

  //
  // UnHighlight in all activated views
  //

  for(int i=1; i<=MyDefinedView.Length(); i++)
  {
    (MyDefinedView.Value(i))->UnHighlight (AStructure);
  }
}

void Visual3d_ViewManager::Redraw() const
{
  // redraw all activated views
  if (MyDefinedView.Length() == 0)
  {
    return;
  }

  if (!MyUnderLayer.IsNull() || !MyOverLayer.IsNull())
  {
    Standard_Integer aWidth = 0, aHeight = 0;
    Standard_Integer aWidthMax  = 0;
    Standard_Integer aHeightMax = 0;

    for(int i=1; i<=MyDefinedView.Length(); i++)
    {
      MyDefinedView.Value(i)->Window()->Size (aWidth, aHeight);
      aWidthMax  = Max (aWidthMax,  aWidth);
      aHeightMax = Max (aHeightMax, aHeight);
    }

    if (!MyUnderLayer.IsNull())
    {
      MyUnderLayer->SetViewport (aWidthMax, aHeightMax);
    }
    if (!MyOverLayer.IsNull())
    {
      MyOverLayer->SetViewport (aWidthMax, aHeightMax);
    }
  }

  for(int i=1; i<=MyDefinedView.Length(); i++)
  {
    MyDefinedView.Value(i)->Redraw (MyUnderLayer, MyOverLayer);
  }
}

void Visual3d_ViewManager::Update() const
{
  Redraw();
}

void Visual3d_ViewManager::RedrawImmediate() const
{
  if (MyDefinedView.Length() == 0)
  {
    return;
  }

  // update all activated views
  for(int i=1; i<=MyDefinedView.Length(); i++)
  {
    MyDefinedView.Value(i)->RedrawImmediate (MyUnderLayer, MyOverLayer);
  }
}

void Visual3d_ViewManager::Invalidate() const
{
  if (MyDefinedView.Length() == 0)
  {
    return;
  }

  // update all activated views
  for(int i=1; i<=MyDefinedView.Length(); i++)
  {
    MyDefinedView.Value(i)->Invalidate();
  }
}

Handle(Visual3d_HSequenceOfView) Visual3d_ViewManager::ActivatedView () const
{

  Handle(Visual3d_HSequenceOfView) SG = new Visual3d_HSequenceOfView();

  for(int i=1; i<=MyDefinedView.Length(); i++)
  {
    if ((MyDefinedView.Value(i))->IsActive ())
    {
      SG->Append(MyDefinedView.Value(i));
    }
  }

  return (SG);
}

#ifdef IMPLEMENTED
Standard_Boolean Visual3d_ViewManager::ContainsComputedStructure () const
{
  Standard_Boolean Result = Standard_False;

  //
  // Check all activated views
  //
  for(int i=1; (!Result) && i<=MyDefinedView.Length(); i++)
  {
    if ((MyDefinedView.Value(i))->IsActive())
    {
      Result = (MyDefinedView.Value(i))->ContainsComputedStructure();
    }
  }

  return Result;
}
#endif

Handle(Visual3d_HSequenceOfView) Visual3d_ViewManager::DefinedView () const
{
  Handle (Visual3d_HSequenceOfView) SG = new Visual3d_HSequenceOfView();

  for(int i=1; i<=MyDefinedView.Length(); i++)
  {
    SG->Append(MyDefinedView.Value(i));
  }

  return (SG);
}

Standard_Boolean Visual3d_ViewManager::ViewExists (const Handle(Aspect_Window)& AWindow, Graphic3d_CView& TheCView) const
{
  Standard_Boolean Exist = Standard_False;

  // Parse the list of views to find
  // a view with the specified window

#if defined(_WIN32)
  const Handle(WNT_Window) THEWindow = Handle(WNT_Window)::DownCast (AWindow);
  Aspect_Handle TheSpecifiedWindowId = THEWindow->HWindow ();
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
  const Handle(Cocoa_Window) THEWindow = Handle(Cocoa_Window)::DownCast (AWindow);
  NSView* TheSpecifiedWindowId = THEWindow->HView();
#elif defined(__ANDROID__)
  int TheSpecifiedWindowId = -1;
#else
  const Handle(Xw_Window) THEWindow = Handle(Xw_Window)::DownCast (AWindow);
  int TheSpecifiedWindowId = int (THEWindow->XWindow ());
#endif

  for(int i=1; (!Exist) && i<=MyDefinedView.Length(); i++)
  {
    if ( ((MyDefinedView.Value(i))->IsDefined ()) && ((MyDefinedView.Value(i))->IsActive ()) )
    {
      const Handle(Aspect_Window) AspectWindow = (MyDefinedView.Value(i))->Window();

#if defined(_WIN32)
      const Handle(WNT_Window) theWindow = Handle(WNT_Window)::DownCast (AspectWindow);
      Aspect_Handle TheWindowIdOfView = theWindow->HWindow ();
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
      const Handle(Cocoa_Window) theWindow = Handle(Cocoa_Window)::DownCast (AspectWindow);
      NSView* TheWindowIdOfView = theWindow->HView();
#elif defined(__ANDROID__)
      int TheWindowIdOfView = 0;
#else
      const Handle(Xw_Window) theWindow = Handle(Xw_Window)::DownCast (AspectWindow);
      int TheWindowIdOfView = int (theWindow->XWindow ());
#endif  // WNT
      // Comparaison on window IDs
      if (TheWindowIdOfView == TheSpecifiedWindowId)
      {
        Exist = Standard_True;
        TheCView = *(Graphic3d_CView* )(MyDefinedView.Value(i))->CView();
      }
    }
  }

  return (Exist);
}

void Visual3d_ViewManager::Activate ()
{
  //
  // Activates all deactivated views
  //
  for(int i=1; i<=MyDefinedView.Length(); i++)
  {
    if (! (MyDefinedView.Value(i))->IsActive())
    {
      (MyDefinedView.Value(i))->Activate();
    }
  }
}

void Visual3d_ViewManager::Deactivate ()
{
  //
  // Deactivates all activated views
  //
  for(int i=1; i<=MyDefinedView.Length(); i++)
  {
    if ((MyDefinedView.Value(i))->IsActive())
    {
      (MyDefinedView.Value(i))->Deactivate();
    }
  }

}

Standard_Integer Visual3d_ViewManager::MaxNumOfViews () const {

	// Retourne the planned of definable views for the current
	// Visual3d_ViewManager.
	return
(Standard_Integer ((View_IDMAX-View_IDMIN+1)/Visual3d_ViewManager::Limit ()));

}

Handle(Graphic3d_Structure) Visual3d_ViewManager::Identification (const Standard_Integer AId) const {

	return (Graphic3d_StructureManager::Identification (AId));

}

Standard_Integer Visual3d_ViewManager::Identification () const {

	return (Graphic3d_StructureManager::Identification ());

}

Standard_Integer Visual3d_ViewManager::Identification (const Handle(Visual3d_View)& AView)
{
  MyDefinedView.Append(AView);
  return (MyViewGenId.Next ());
}

void Visual3d_ViewManager::UnIdentification (const Standard_Integer aViewId)
{
  for(int i=1; i<=MyDefinedView.Length(); i++)
  {
    if ((MyDefinedView.Value(i))->Identification() == aViewId)
    {
      //remove the view from the list
      MyDefinedView.Remove(i);
      break;
    }
  }

  MyViewGenId.Free(aViewId);
}

void Visual3d_ViewManager::SetTransparency (const Standard_Boolean AFlag)
{
  if (MyTransparency && AFlag) return;
  if (! MyTransparency && ! AFlag) return;

  for(int i=1; i<=MyDefinedView.Length(); i++)
  {
    (MyDefinedView.Value(i))->SetTransparency(AFlag);
  }

  MyTransparency = AFlag;
}

Standard_Boolean Visual3d_ViewManager::Transparency () const
{
  return (MyTransparency);
}

void Visual3d_ViewManager::SetZBufferAuto (const Standard_Boolean AFlag)
{
  if (MyZBufferAuto && AFlag) return;
  if (! MyZBufferAuto && ! AFlag) return;

  // if pass from False to True :
  // no problem, at the next view update, it
  // will properly ask questions to answer (SetVisualisation)
  // if pass from True to False :
  // it is necessary to modify ZBufferActivity at each view so that
  // zbuffer could be active only if required by context.
  // In this case -1 is passed so that the view ask itself the question
  // Note : 0 forces the desactivation, 1 forces the activation
  if (! AFlag)
  {
    for(int i=1; i<=MyDefinedView.Length(); i++)
    {
      (MyDefinedView.Value(i))->SetZBufferActivity(-1);
    }
  }
  MyZBufferAuto = AFlag;
}

Standard_Boolean Visual3d_ViewManager::ZBufferAuto () const
{
  return (MyZBufferAuto);
}

void Visual3d_ViewManager::SetLayer (const Handle(Visual3d_Layer)& ALayer) {

#ifdef TRACE_LAYER
	cout << "Visual3d_ViewManager::SetLayer\n" << flush;
#endif

	if (ALayer->Type () == Aspect_TOL_OVERLAY) {
#ifdef TRACE_LAYER
		if (MyOverLayer.IsNull ())
			cout << "MyOverLayer is defined" << endl;
		else
			cout << "MyOverLayer is redefined" << endl;
#endif
		MyOverLayer = ALayer;
	}
	else {
#ifdef TRACE_LAYER
		if (MyUnderLayer.IsNull ())
			cout << "MyUnderLayer is defined" << endl;
		else
			cout << "MyUnderLayer is redefined" << endl;
#endif
		MyUnderLayer = ALayer;
	}

}

const Handle(Visual3d_Layer)& Visual3d_ViewManager::UnderLayer () const {

	return (MyUnderLayer);

}

const Handle(Visual3d_Layer)& Visual3d_ViewManager::OverLayer () const {

	return (MyOverLayer);

}

//=======================================================================
//function : ChangeZLayer
//purpose  :
//=======================================================================

void Visual3d_ViewManager::ChangeZLayer (const Handle(Graphic3d_Structure)& theStructure,
                                         const Standard_Integer theLayerId)
{
  if (!myLayerIds.Contains (theLayerId))
    return;

  // change display layer for structure in all views
  if (MyDisplayedStructure.Contains (theStructure))
  {
    for(int i=1; i<=MyDefinedView.Length(); i++)
    {
      (MyDefinedView.Value(i))->ChangeZLayer(theStructure, theLayerId);
    }
  }

  // tell graphic driver to update the structure's display layer
  MyGraphicDriver->ChangeZLayer (*(theStructure->CStructure()), theLayerId);
}

//=======================================================================
//function : GetZLayer
//purpose  :
//=======================================================================

Standard_Integer Visual3d_ViewManager::GetZLayer (const Handle(Graphic3d_Structure)& theStructure) const
{
  return MyGraphicDriver->GetZLayer (*theStructure->CStructure ());
}

//=======================================================================
//function : SetZLayerSettings
//purpose  :
//=======================================================================
void Visual3d_ViewManager::SetZLayerSettings (const Standard_Integer theLayerId,
                                              const Graphic3d_ZLayerSettings& theSettings)
{
  // tell all managed views to set zlayer settings
  for(int i=1; i<=MyDefinedView.Length(); i++)
  {
    (MyDefinedView.Value(i))->SetZLayerSettings (theLayerId, theSettings);
  }

  if (myMapOfZLayerSettings.IsBound (theLayerId))
  {
    myMapOfZLayerSettings.ChangeFind (theLayerId) = theSettings;
  }
  else
  {
    myMapOfZLayerSettings.Bind (theLayerId, theSettings);
  }
  
}

//=======================================================================
//function : ZLayerSettings
//purpose  :
//=======================================================================
Graphic3d_ZLayerSettings Visual3d_ViewManager::ZLayerSettings (const Standard_Integer theLayerId)
{
  if (!myLayerIds.Contains (theLayerId))
  {
    return Graphic3d_ZLayerSettings();
  }

  return myMapOfZLayerSettings.Find (theLayerId);
}

//=======================================================================
//function : AddZLayer
//purpose  :
//=======================================================================

Standard_Boolean Visual3d_ViewManager::AddZLayer (Standard_Integer& theLayerId)
{
  try
  {
    OCC_CATCH_SIGNALS
    theLayerId = getZLayerGenId ().Next ();
    myLayerIds.Add (theLayerId);
    myLayerSeq.Append (theLayerId);
  }
  catch (Aspect_IdentDefinitionError)
  {
    // new index can't be generated
    return Standard_False;
  }

  // default z-layer settings
  myMapOfZLayerSettings.Bind (theLayerId, Graphic3d_ZLayerSettings());

  // tell all managed views to remove display layers
  for(int i=1; i<=MyDefinedView.Length(); i++)
  {
    (MyDefinedView.Value(i))->AddZLayer(theLayerId);
  }

  return Standard_True;
}

//=======================================================================
//function : RemoveZLayer
//purpose  :
//=======================================================================

Standard_Boolean Visual3d_ViewManager::RemoveZLayer (const Standard_Integer theLayerId)
{
  if (!myLayerIds.Contains (theLayerId) || theLayerId == 0)
    return Standard_False;

  // tell all managed views to remove display layers
  for(int i=1; i<=MyDefinedView.Length(); i++)
  {
    (MyDefinedView.Value(i))->RemoveZLayer (theLayerId);
  }

  MyGraphicDriver->UnsetZLayer (theLayerId);

  // remove index
  for (int aIdx = 1; aIdx <= myLayerSeq.Length (); aIdx++)
  {
    if (myLayerSeq (aIdx) == theLayerId)
    {
      myLayerSeq.Remove (aIdx);
      break;
    }
  }

  myMapOfZLayerSettings.UnBind (theLayerId);

  myLayerIds.Remove (theLayerId);
  getZLayerGenId ().Free (theLayerId);

  return Standard_True;
}

//=======================================================================
//function : GetAllZLayers
//purpose  :
//=======================================================================

void Visual3d_ViewManager::GetAllZLayers (TColStd_SequenceOfInteger& theLayerSeq) const
{
  theLayerSeq.Assign (myLayerSeq);
}

//=======================================================================
//function : getZLayerGenId
//purpose  :
//=======================================================================

Aspect_GenId& Visual3d_ViewManager::getZLayerGenId ()
{
  static Aspect_GenId aGenId (1, IntegerLast());
  return aGenId;
}

//=======================================================================
//function : InstallZLayers
//purpose  :
//=======================================================================

void Visual3d_ViewManager::InstallZLayers(const Handle(Visual3d_View)& theView) const
{
  Standard_Boolean isContainsView = Standard_False;
  for(int i=1; i<=MyDefinedView.Length(); i++)
  {
    if(MyDefinedView.Value(i) == theView)
    {
      isContainsView = Standard_True;
      break;
    }
  }
  if (!isContainsView)
    return;

  // erase and insert layers iteratively to provide the same layer order as
  // in the view manager's sequence. This approach bases on the layer insertion
  // order: the new layers are always appended to the end of the list
  // inside of view, while layer remove operation doesn't affect the order.
  // Starting from second layer : no need to change the default z layer.
  for (Standard_Integer aSeqIdx = 2; aSeqIdx <= myLayerSeq.Length (); aSeqIdx++)
  {
    Standard_Integer aLayerID = myLayerSeq.Value (aSeqIdx);
    theView->RemoveZLayer (aLayerID);
    theView->AddZLayer (aLayerID);
  }
}
