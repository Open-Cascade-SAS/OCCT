// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2015 OPEN CASCADE SAS
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

#ifndef _ViewerTest_HeaderFile
#define _ViewerTest_HeaderFile

#include <AIS_KindOfInteractive.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Aspect_TypeOfMarker.hxx>
#include <Draw_Interpretor.hxx>
#include <Standard_Integer.hxx>
#include <Standard_CString.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>
#include <TColStd_HArray1OfTransient.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_HArray1OfShape.hxx>
#include <Quantity_Color.hxx>

class AIS_InteractiveContext;
class AIS_InteractiveObject;
class TCollection_AsciiString;
class Standard_Transient;
class Image_PixMap;
class V3d_View;
class V3d_Viewer;
class ViewerTest_EventManager;
class TopoDS_Shape;

class ViewerTest
{
public:

  DEFINE_STANDARD_ALLOC

  //! Loads all Draw commands of  V2d & V3d. Used for plugin.
  Standard_EXPORT static void Factory (Draw_Interpretor& theDI);

  //! Creates view with default or custom name
  //! and add this name in map to manage muliple views
  //! implemented in ViewerTest_ViewerCommands.cxx
  Standard_EXPORT static TCollection_AsciiString ViewerInit (const Standard_Integer thePxLeft   = 0,
                                                             const Standard_Integer thePxTop    = 0,
                                                             const Standard_Integer thePxWidth  = 0,
                                                             const Standard_Integer thePxHeight = 0,
                                                             const Standard_CString theViewName = "",
                                                             const Standard_CString theDisplayName = "");

  Standard_EXPORT static void RemoveViewName (const TCollection_AsciiString& theName);

  Standard_EXPORT static void InitViewName (const TCollection_AsciiString& theName,
                                            const Handle(V3d_View)&        theView);

  Standard_EXPORT static TCollection_AsciiString GetCurrentViewName();

  //! Removes view and clear all maps
  //! with information about its resources if necessary
  Standard_EXPORT static void RemoveView (const TCollection_AsciiString& theViewName,
                                          const Standard_Boolean         theToRemoveContext = Standard_True);

  //! Removes view and clear all maps
  //! with information about its resources if necessary
  Standard_EXPORT static void RemoveView (const Handle(V3d_View)& theView,
                                          const Standard_Boolean  theToRemoveContext = Standard_True);

  //! Display AIS object in active Viewer and register it in the map of Interactive Objects with specified name.
  //! @param theName            key to be associated to displayed interactive object
  //! @param theObject          object to display
  //! @param theToUpdate        redraw viewer after displaying object
  //! @param theReplaceIfExists replace the object assigned to specified key
  //! @return true if new object has been displayed
  Standard_EXPORT static Standard_Boolean Display (const TCollection_AsciiString&       theName,
                                                   const Handle(AIS_InteractiveObject)& theObject,
                                                   const Standard_Boolean               theToUpdate = Standard_True,
                                                   const Standard_Boolean               theReplaceIfExists = Standard_True);

  //! waits until a shape of type <aType> is picked in the AIS Viewer and returns it.
  //! if <aType> == TopAbs_Shape, any shape can be picked...
  //! MaxPick  is the Max number before exiting, if no pick is successful
  Standard_EXPORT static TopoDS_Shape PickShape (const TopAbs_ShapeEnum aType,
                                                 const Standard_Integer MaxPick = 5);

  //! wait until the array is filled with picked shapes.
  //! returns True if the array is filled.
  //! exit if number of unsuccessful picks =  <MaxPick>
  Standard_EXPORT static Standard_Boolean PickShapes (const TopAbs_ShapeEnum           aType,
                                                      Handle(TopTools_HArray1OfShape)& thepicked,
                                                      const Standard_Integer           MaxPick = 5);

  //! waits until an interactive object of a given Type
  //! and signature is picked (default values authorize
  //! selection of any Interactive Object)
  //! exit if number of unsuccessful picks =  <MaxPick>
  Standard_EXPORT static Handle(AIS_InteractiveObject) PickObject (const AIS_KindOfInteractive Type      = AIS_KOI_None,
                                                                   const Standard_Integer      Signature = -1,
                                                                   const Standard_Integer      MaxPick   = 5);

  //! selection of several interactive objects.
  //! Number is given by the size of <thepicked>
  //! exit if number of unsuccesfull picks =  <MaxPick>
  Standard_EXPORT static Standard_Boolean PickObjects (Handle(TColStd_HArray1OfTransient)& thepicked,
                                                       const AIS_KindOfInteractive         Type = AIS_KOI_None,
                                                       const Standard_Integer              Signature = -1,
                                                       const Standard_Integer              MaxPick = 5);

  Standard_EXPORT static void Commands (Draw_Interpretor& theCommands);

  Standard_EXPORT static void ViewerCommands (Draw_Interpretor& theCommands);

  Standard_EXPORT static void MyCommands (Draw_Interpretor& theCommands);

  Standard_EXPORT static void RelationCommands (Draw_Interpretor& theCommands);

  Standard_EXPORT static void ObjectCommands (Draw_Interpretor& theCommands);

  Standard_EXPORT static void FilletCommands (Draw_Interpretor& theCommands);

  Standard_EXPORT static void OpenGlCommands (Draw_Interpretor& theCommands);

  Standard_EXPORT static void GetMousePosition (Standard_Integer& xpix, Standard_Integer& ypix);

  Standard_EXPORT static Handle(V3d_Viewer) GetViewerFromContext();

  Standard_EXPORT static Handle(V3d_Viewer) GetCollectorFromContext();

  Standard_EXPORT static const Handle(AIS_InteractiveContext)& GetAISContext();

  Standard_EXPORT static void SetAISContext (const Handle(AIS_InteractiveContext)& aContext);

  Standard_EXPORT static const Handle(V3d_View)& CurrentView();

  Standard_EXPORT static void CurrentView (const Handle(V3d_View)& aViou);

  Standard_EXPORT static void Clear();

  //! puts theMgr as current eventmanager (the move,select,...will be applied to theMgr)
  Standard_EXPORT static void SetEventManager (const Handle(ViewerTest_EventManager)& theMgr);

  //! removes the last EventManager from the list.
  Standard_EXPORT static void UnsetEventManager();

  //! clear the list of EventManagers and
  //! sets the default EventManager as current
  Standard_EXPORT static void ResetEventManager();

  Standard_EXPORT static Handle(ViewerTest_EventManager) CurrentEventManager();

  Standard_EXPORT static void RemoveSelected();

  Standard_EXPORT static void StandardModeActivation (const Standard_Integer Mode);

  Standard_EXPORT static Quantity_NameOfColor GetColorFromName (const Standard_CString name);

  //! Parses color argument(s) specified within theArgVec[0], theArgVec[1] and theArgVec[2].
  //! Handles either color specified by name (single argument)
  //! or by RGB components (3 arguments) in range 0..1.
  //! The result is stored in theColor on success.
  //! Returns number of handled arguments (1 or 3) or 0 on syntax error.
  Standard_EXPORT static Standard_Integer ParseColor (Standard_Integer theArgNb,
                                                      const char**     theArgVec,
                                                      Quantity_Color&  theColor);

  //! redraws all defined views.
  Standard_EXPORT static void RedrawAllViews();

  //! Splits "parameter=value" string into separate
  //! parameter and value strings.
  //! @return TRUE if the string matches pattern "<string>=<empty or string>"
  Standard_EXPORT static Standard_Boolean SplitParameter (const TCollection_AsciiString& theString,
                                                          TCollection_AsciiString&       theName,
                                                          TCollection_AsciiString&       theValue);

  //! Parses boolean argument.
  //! Handles either flag specified by 0|1 or on|off.
  Standard_EXPORT static Standard_Boolean ParseOnOff (Standard_CString  theArg,
                                                      Standard_Boolean& theIsOn);

  //! Returns list of selected shapes.
  Standard_EXPORT static void GetSelectedShapes (TopTools_ListOfShape& theShapes);

  //! Parses line type argument.
  //! Handles either enumeration (integer) value or string constant.
  Standard_EXPORT static Standard_Boolean ParseLineType (Standard_CString   theArg,
                                                         Aspect_TypeOfLine& theType);

  //! Parses marker type argument.
  //! Handles either enumeration (integer) value or string constant.
  Standard_EXPORT static Standard_Boolean ParseMarkerType (Standard_CString theArg,
                                                           Aspect_TypeOfMarker& theType,
                                                           Handle(Image_PixMap)& theImage);

private:

  //! Returns a window class that implements standard behavior of
  //! all windows of the ViewerTest. This includes usual Open CASCADE
  //! view conventions for mouse buttons (e.g. Ctrl+MB1 for zoom,
  //! Ctrl+MB2 for pan, etc) and keyboard shortcuts.
  //! This method is relevant for MS Windows only and respectively
  //! returns WNT_WClass handle.
  static const Handle(Standard_Transient)& WClass();
};

#endif // _ViewerTest_HeaderFile
