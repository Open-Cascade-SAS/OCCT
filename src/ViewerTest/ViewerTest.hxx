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
#include <Graphic3d_TypeOfShadingModel.hxx>
#include <Graphic3d_ZLayerId.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_HArray1OfTransient.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_HArray1OfShape.hxx>
#include <Quantity_ColorRGBA.hxx>

class AIS_InteractiveContext;
class AIS_InteractiveObject;
class Image_PixMap;
class V3d_View;
class V3d_Viewer;
class ViewerTest_EventManager;
class TopoDS_Shape;
class WNT_WClass;

class ViewerTest
{
public:

  DEFINE_STANDARD_ALLOC

  //! Loads all Draw commands of  V2d & V3d. Used for plugin.
  Standard_EXPORT static void Factory (Draw_Interpretor& theDI);

  //! Creates view with default or custom name
  //! and adds this name in map to manage multiple views.
  //! Implemented in ViewerTest_ViewerCommands.cxx.
  //! @param thePxLeft      left position of newly created window
  //! @param thePxTop       top  position of newly created window
  //! @param thePxWidth     width of newly created window
  //! @param thePxHeight    height of newly created window
  //! @param theViewName    name of newly created View
  //! @oaram theDisplayName display name
  //! @param theViewToClone when specified, the new View will copy properties of existing one
  //! @param theIsVirtual   force creation of virtual off-screen window within interactive session
  Standard_EXPORT static TCollection_AsciiString ViewerInit (const Standard_Integer thePxLeft   = 0,
                                                             const Standard_Integer thePxTop    = 0,
                                                             const Standard_Integer thePxWidth  = 0,
                                                             const Standard_Integer thePxHeight = 0,
                                                             const TCollection_AsciiString& theViewName = "",
                                                             const TCollection_AsciiString& theDisplayName = "",
                                                             const Handle(V3d_View)& theViewToClone = Handle(V3d_View)(),
                                                             const Standard_Boolean theIsVirtual = false);

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

  //! redraws all defined views.
  Standard_EXPORT static void RedrawAllViews();

  //! Splits "parameter=value" string into separate
  //! parameter and value strings.
  //! @return TRUE if the string matches pattern "<string>=<empty or string>"
  Standard_EXPORT static Standard_Boolean SplitParameter (const TCollection_AsciiString& theString,
                                                          TCollection_AsciiString&       theName,
                                                          TCollection_AsciiString&       theValue);

  //! Returns list of selected shapes.
  Standard_EXPORT static void GetSelectedShapes (TopTools_ListOfShape& theShapes);

  //! Parses line type argument.
  //! Handles either enumeration (integer) value or string constant.
  Standard_EXPORT static Standard_Boolean ParseLineType (Standard_CString theArg,
                                                         Aspect_TypeOfLine& theType,
                                                         uint16_t& thePattern);

  //! Parses line type argument.
  //! Handles either enumeration (integer) value or string constant.
  static Standard_Boolean ParseLineType (Standard_CString theArg,
                                         Aspect_TypeOfLine& theType)
  {
    uint16_t aPattern = 0xFFFF;
    return ParseLineType (theArg, theType, aPattern);
  }

  //! Parses marker type argument.
  //! Handles either enumeration (integer) value or string constant.
  Standard_EXPORT static Standard_Boolean ParseMarkerType (Standard_CString theArg,
                                                           Aspect_TypeOfMarker& theType,
                                                           Handle(Image_PixMap)& theImage);

  //! Parses shading model argument.
  //! Handles either enumeration (integer) value or string constant.
  Standard_EXPORT static Standard_Boolean ParseShadingModel (Standard_CString              theArg,
                                                             Graphic3d_TypeOfShadingModel& theModel);

  //! Parses ZLayer name.
  //! @param theArg [in] layer name or enumeration alias
  //! @param theLayer [out] layer index
  //! @return TRUE if layer has been identified, note that Graphic3d_ZLayerId_UNKNOWN is also valid value
  static Standard_Boolean ParseZLayerName (Standard_CString theArg,
                                           Graphic3d_ZLayerId& theLayer)
  {
    return parseZLayer (theArg, false, theLayer);
  }

  //! Parses ZLayer name.
  //! @param theArg [in] layer name, enumeration alias or index (of existing Layer)
  //! @param theLayer [out] layer index
  //! @return TRUE if layer has been identified, note that Graphic3d_ZLayerId_UNKNOWN is also valid value
  static Standard_Boolean ParseZLayer (Standard_CString theArg,
                                       Graphic3d_ZLayerId& theLayer)
  {
    return parseZLayer (theArg, true, theLayer);
  }

public: //! @name deprecated methods

  //! Parses RGB(A) color argument(s) specified within theArgVec[0], theArgVec[1], theArgVec[2] and theArgVec[3].
  Standard_DEPRECATED("Method has been moved to Draw::ParseColor()")
  Standard_EXPORT static Standard_Integer ParseColor (const Standard_Integer   theArgNb,
                                                      const char* const* const theArgVec,
                                                      Quantity_ColorRGBA&      theColor);

  //! Parses RGB color argument(s).
  //! Returns number of handled arguments (1 or 3) or 0 on syntax error.
  Standard_DEPRECATED("Method has been moved to Draw::ParseColor()")
  Standard_EXPORT static Standard_Integer ParseColor (const Standard_Integer   theArgNb,
                                                      const char* const* const theArgVec,
                                                      Quantity_Color&          theColor);

  //! Parses boolean argument.
  //! Handles either flag specified by 0|1 or on|off.
  Standard_DEPRECATED("Method has been moved to Draw::ParseOnOff()")
  Standard_EXPORT static Standard_Boolean ParseOnOff (Standard_CString  theArg,
                                                      Standard_Boolean& theIsOn);

  Standard_DEPRECATED("Method has been moved to Quantity_Color::ColorFromName()")
  Standard_EXPORT static Quantity_NameOfColor GetColorFromName (const Standard_CString name);

private:

  //! Parses ZLayer name.
  //! @param theArg [in] layer name, enumeration alias or index (of existing Layer)
  //! @param theToAllowInteger [in] when TRUE, the argument will be checked for existing layer index
  //! @param theLayer [out] layer index
  //! @return TRUE if layer has been identified, note that Graphic3d_ZLayerId_UNKNOWN is also valid value
  Standard_EXPORT static Standard_Boolean parseZLayer (Standard_CString theArg,
                                                       Standard_Boolean theToAllowInteger,
                                                       Graphic3d_ZLayerId& theLayer);

  //! Returns a window class that implements standard behavior of
  //! all windows of the ViewerTest. This includes usual Open CASCADE
  //! view conventions for mouse buttons (e.g. Ctrl+MB1 for zoom,
  //! Ctrl+MB2 for pan, etc) and keyboard shortcuts.
  //! This method is relevant for MS Windows only and respectively
  //! returns WNT_WClass handle.
  static const Handle(WNT_WClass)& WClass();
};

#endif // _ViewerTest_HeaderFile
