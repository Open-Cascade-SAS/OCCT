// Created on: 1996-12-18
// Created by: Robert COUBLANC
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _AIS_InteractiveContext_HeaderFile
#define _AIS_InteractiveContext_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <AIS_DataMapOfIOStatus.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <StdSelect_ViewerSelector3d.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_Boolean.hxx>
#include <Prs3d_Drawer.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Standard_Integer.hxx>
#include <AIS_DataMapOfILC.hxx>
#include <AIS_SequenceOfInteractive.hxx>
#include <MMgt_TShared.hxx>
#include <AIS_DisplayStatus.hxx>
#include <AIS_KindOfInteractive.hxx>
#include <Standard_Real.hxx>
#include <Aspect_TypeOfFacingModel.hxx>
#include <Graphic3d_NameOfMaterial.hxx>
#include <Standard_ShortReal.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <AIS_DisplayMode.hxx>
#include <AIS_TypeOfIso.hxx>
#include <AIS_StatusOfDetection.hxx>
#include <AIS_StatusOfPick.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <SelectMgr_IndexedMapOfOwner.hxx>
#include <AIS_ClearMode.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <SelectMgr_ListOfFilter.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <Standard_CString.hxx>
class SelectMgr_SelectionManager;
class V3d_Viewer;
class AIS_InteractiveObject;
class SelectMgr_OrFilter;
class V3d_View;
class AIS_LocalContext;
class TopLoc_Location;
class Quantity_Color;
class TCollection_ExtendedString;
class Prs3d_LineAspect;
class Prs3d_BasicAspect;
class TopoDS_Shape;
class SelectMgr_EntityOwner;
class Standard_Transient;
class SelectMgr_Filter;
class TCollection_AsciiString;


class AIS_InteractiveContext;
DEFINE_STANDARD_HANDLE(AIS_InteractiveContext, MMgt_TShared)

//! The Interactive Context allows you to manage
//! graphic behavior and selection of Interactive Objects
//! in one or more viewers. Class methods make this
//! highly transparent.
//! It is essential to remember that an Interactive Object
//! which is already known by the Interactive Context
//! must be modified using Context methods. You can
//! only directly call the methods available for an
//! Interactive Object if it has not been loaded into an
//! Interactive Context.
//! You must distinguish two states in the Interactive Context:
//! -   No Open Local Context, also known as the Neutral Point.
//! -   One or several open local contexts, each
//! representing a temporary state of selection and presentation.
//! Some methods can only be used in open Local
//! Context; others in closed Local Context; others do
//! not have the same behavior in one state as in the other.
//! The possiblities of use for local contexts are
//! numerous depending on the type of operation that
//! you want to perform, for example:
//! -   working on all visualized interactive objects,
//! -   working on only a few objects,
//! -   working on a single object.
//! 1. When you want ot work on one type of entity, you
//! may open a local context with the option
//! UseDisplayedObjects set to false. DisplayedObjects
//! allows you to recover the visualized Interactive
//! Objects which have a given Type and
//! Signature   from Neutral Point.
//! 2. You must keep in mind the fact that when you open
//! a Local Context with default options:
//! -   The Interactive Objects visualized at Neutral Point
//! are activated with their default selection mode. You
//! must deactivate those which you do not want ot use.
//! -   The Shape type Interactive Objects are
//! automatically decomposed into sub-shapes when
//! standard activation modes are launched.
//! -   The "temporary" Interactive Objects present in the
//! Local Contexts are not automatically taken into
//! account. You have to load them manually if you
//! want to use them.
//! -   The stages could be the following:
//! -   Open a Local Context with the right options;
//! -   Load/Visualize the required complementary
//! objects with the desired activation modes.
//! -   Activate Standard modes if necessary
//! - Create its filters and add them to the Local Context
//! -   Detect/Select/recover the desired entities
//! -   Close the Local Context with the adequate index.
//! -   It is useful to create an interactive editor, to which
//! you pass the Interactive Context. This will take care
//! of setting up the different contexts of
//! selection/presentation according to the operation
//! which you want to perform.

//! Selection of parts of the objects can also be done without opening a local context.
//! Interactive context itself supports decomposed object selection with selection filters
//! support. Note that each selectable object must specify the selection mode that is
//! responsible for selection of object as a whole (global selection mode). By default, global
//! selection mode is equal to 0, but it might be redefined if needed. Sub-part selection
//! of the objects without using local context provides a possibility to activate part
//! selection modes along with global selection mode.
class AIS_InteractiveContext : public MMgt_TShared
{

public:

  

  //! Constructs the interactive context object defined by
  //! the principal viewer MainViewer.
  Standard_EXPORT AIS_InteractiveContext(const Handle(V3d_Viewer)& MainViewer);
  
  Standard_EXPORT virtual void Delete() const Standard_OVERRIDE;
  
  Standard_EXPORT void SetAutoActivateSelection (const Standard_Boolean Auto);
  
  Standard_EXPORT Standard_Boolean GetAutoActivateSelection() const;
  
  //! Controls the choice between the using the display
  //! and selection modes of open local context which you
  //! have defined and activating those available by default.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the Interactive Object anIobj is
  //! displayed in the default active mode. This will be the
  //! object's default display mode, if there is one.
  //! Otherwise, it will be the context mode. The Interactive
  //! Object's default selection mode is activated. In
  //! general, this is 0.
  //! This syntax has the same behavior as local context,
  //! open or closed. If you want to view the object in open
  //! local context without selection, use the syntax below,
  //! setting aSelectionMode to -1.
  Standard_EXPORT void Display (const Handle(AIS_InteractiveObject)& anIobj, const Standard_Boolean updateviewer = Standard_True);
  
  //! Controls the choice between the using the display
  //! and selection modes of open local context which you
  //! have defined and activating those available by default.
  //! If no Local Context is opened. and the Interactive
  //! Object theIObj has no display mode of its own, the
  //! default display mode, 0, is used. Likewise, if theIObj
  //! has no selection mode of its own, the default one, 0, is used.
  //! If a local context is open and if theToUpdateViewer equals
  //! Standard_False, the presentation of the Interactive
  //! Object activates the selection mode; the object is
  //! displayed but no viewer will be updated.
  //! If theSelectionMode equals -1, theIObj will not be
  //! activated: it will be displayed but will not be selectable.
  //! Use this if you want to view the object in open local
  //! context without selection. Note: This option is only
  //! available in Local Context.
  //! If theToAllowDecomposition equals true, theIObj can have
  //! subshapes detected by selection mechanisms. theIObj
  //! must be able to give a shape selection modes which
  //! fit the AIS_Shape selection modes:
  //! -   vertices: 1
  //! -   edges: 2
  //! -   wires: 3.
  Standard_EXPORT void Display (const Handle(AIS_InteractiveObject)& theIObj, const Standard_Integer theDispMode, const Standard_Integer theSelectionMode, const Standard_Boolean theToUpdateViewer = Standard_True, const Standard_Boolean theToAllowDecomposition = Standard_True, const AIS_DisplayStatus theDispStatus = AIS_DS_None);
  
  //! Allows you to load the Interactive Object aniobj
  //! with a given selection mode SelectionMode, and/or
  //! with the desired decomposition option, whether the
  //! object is visualized or not. If AllowDecomp =
  //! Standard_True and, if the interactive object is of
  //! the "Shape" type, these "standard" selection
  //! modes will be automatically activated as a function
  //! of the modes present in the Local Context.
  //! The loaded objects will be selectable but
  //! displayable in highlighting only when detected by the Selector.
  //! This method is available only when Local Contexts are open.
  Standard_EXPORT void Load (const Handle(AIS_InteractiveObject)& aniobj, const Standard_Integer SelectionMode = -1, const Standard_Boolean AllowDecomp = Standard_False);
  
  //! Hides the object. The object's presentations are simply
  //! flagged as invisible and therefore excluded from redrawing.
  //! To show hidden objects, use Display().
  Standard_EXPORT void Erase (const Handle(AIS_InteractiveObject)& aniobj, const Standard_Boolean updateviewer = Standard_True);
  
  //! Hides all objects. The object's presentations are simply
  //! flagged as invisible and therefore excluded from redrawing.
  //! To show all hidden objects, use DisplayAll().
  Standard_EXPORT void EraseAll (const Standard_Boolean updateviewer = Standard_True);
  
  //! Displays all hidden objects.
  Standard_EXPORT void DisplayAll (const Standard_Boolean updateviewer = Standard_True);
  

  //! Hides selected objects. The object's presentations are simply
  //! flagged as invisible and therefore excluded from redrawing.
  //! To show hidden objects, use Display().
  Standard_EXPORT void EraseSelected (const Standard_Boolean updateviewer = Standard_True);
  
  //! Displays selected objects if a local context is open.
  //! Displays current objects if there is no active local context.
  //! Objects selected when there is no open local context
  //! are called current objects; those selected in open
  //! local context, selected objects.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the presentation   of the Interactive
  //! Object activates   the   selection   mode; the   object is
  //! displayed but no viewer will be updated.
  Standard_EXPORT void DisplaySelected (const Standard_Boolean updateviewer = Standard_True);
  
  //! Changes the status of a temporary object. It will be
  //! kept at the neutral point, i.e. put in the list of
  //! displayed   objects along withwith   its temporary
  //! attributes. These include display mode and
  //! selection   mode, for example.
  //! Returns true if done.
  //! inWhichLocal gives the local context in which anIObj
  //! is displayed. By default, the index -1 refers to the last
  //! Local Context opened.
  Standard_EXPORT Standard_Boolean KeepTemporary (const Handle(AIS_InteractiveObject)& anIObj, const Standard_Integer InWhichLocal = -1);
  
  //! Empties the graphic presentation of the mode
  //! indexed by aMode.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the presentation of the Interactive
  //! Object activates the selection mode; the object is
  //! displayed but no viewer will be updated.
  //! Warning
  //! Removes anIobj. anIobj is still active if it was
  //! previously activated.
  Standard_EXPORT void ClearPrs (const Handle(AIS_InteractiveObject)& aniobj, const Standard_Integer aMode = 0, const Standard_Boolean updateviewer = Standard_True);
  
  //! Removes aniobj from every viewer. aniobj is no
  //! longer referenced in the Context.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the presentation   of the Interactive
  //! Object activates the selection mode; the object is
  //! displayed but no viewer will be updated.
  Standard_EXPORT void Remove (const Handle(AIS_InteractiveObject)& aniobj, const Standard_Boolean updateviewer = Standard_True);
  
  //! Removes all the objects from all opened Local Contexts
  //! and from the Neutral Point
  Standard_EXPORT void RemoveAll (const Standard_Boolean updateviewer = Standard_True);
  

  //! Updates the display in the viewer to take dynamic
  //! detection into account. On dynamic detection by the
  //! mouse cursor, sensitive primitives are highlighted.
  //! The highlight color of entities detected by mouse
  //! movement is white by default.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the presentation of the Interactive
  //! Object activates the selection mode; the object is
  //! displayed but no viewer will be updated.
  Standard_EXPORT void Hilight (const Handle(AIS_InteractiveObject)& aniobj, const Standard_Boolean updateviewer = Standard_True);
  

  //! Changes the color of all the lines of the object in view,
  //! aniobj. It paints these lines the color passed as the
  //! argument, aCol.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the presentation of the Interactive
  //! Object activates the selection mode; the object is
  //! displayed but no viewer will be updated.
  Standard_EXPORT void HilightWithColor (const Handle(AIS_InteractiveObject)& aniobj, const Quantity_NameOfColor aCol, const Standard_Boolean updateviewer = Standard_True);
  

  //! Removes hilighting from the entity aniobj. Updates the viewer.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the presentation of the Interactive
  //! Object activates the selection mode; the object is
  //! displayed but no viewer will be updated.
  Standard_EXPORT void Unhilight (const Handle(AIS_InteractiveObject)& aniobj, const Standard_Boolean updateviewer = Standard_True);
  
  //! Sets the display priority aPriority of the seen parts
  //! presentation of the entity anIobj.
  Standard_EXPORT void SetDisplayPriority (const Handle(AIS_InteractiveObject)& anIobj, const Standard_Integer aPriority);
  
  //! Set Z layer id for interactive object.
  //! The Z layers can be used to display temporarily presentations of some object in front of the other objects in the scene.
  //! The ids for Z layers are generated by V3d_Viewer.
  Standard_EXPORT void SetZLayer (const Handle(AIS_InteractiveObject)& theIObj, const Standard_Integer theLayerId);
  
  //! Get Z layer id set for displayed interactive object.
  Standard_EXPORT Standard_Integer GetZLayer (const Handle(AIS_InteractiveObject)& theIObj) const;
  
  //! Recomputes the seen parts presentation of the entity
  //! aniobj. If allmodes equals true, all presentations are
  //! present in the object even if unseen.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the presentation of the Interactive
  //! Object activates the selection mode; the object is
  //! displayed but no viewer will be updated.
  Standard_EXPORT void Redisplay (const Handle(AIS_InteractiveObject)& aniobj, const Standard_Boolean updateviewer = Standard_True, const Standard_Boolean allmodes = Standard_False);
  
  //! Recomputes the Prs/Selection of displayed objects of
  //! a given type and a given signature.
  //! if signature = -1  doesnt take signature criterion.
  Standard_EXPORT void Redisplay (const AIS_KindOfInteractive aTypeOfObject, const Standard_Integer Signature = -1, const Standard_Boolean updateviewer = Standard_True);
  
  //! Recomputes the displayed presentations, flags the others
  //! Doesn't update presentations
  Standard_EXPORT void RecomputePrsOnly (const Handle(AIS_InteractiveObject)& anIobj, const Standard_Boolean updateviewer = Standard_True, const Standard_Boolean allmodes = Standard_False);
  
  //! Recomputes the active selections, flags the others
  //! Doesn't update presentations
  Standard_EXPORT void RecomputeSelectionOnly (const Handle(AIS_InteractiveObject)& anIObj);
  
  //! Updates displayed interactive object by checking and
  //! recomputing its flagged as "to be recomputed" presentation
  //! and selection structures. This method does not force any
  //! recomputation on its own. The method recomputes selections
  //! even if they are loaded without activation in particular selector.
  Standard_EXPORT void Update (const Handle(AIS_InteractiveObject)& theIObj, const Standard_Boolean theUpdateViewer = Standard_True);
  

  //! Sets the display mode of seen Interactive Objects.
  //! aMode provides the display mode index of the entity aniobj.
  //! If updateviewer equals Standard_True, the
  //! predominant mode aMode will overule the context mode.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the presentation of the Interactive
  //! Object returns to the default selection mode; the
  //! object is displayed but no viewer will be updated.
  //! Note that display mode 3 is only used if you have an
  //! AIS_Textured Shape.
  Standard_EXPORT void SetDisplayMode (const Handle(AIS_InteractiveObject)& aniobj, const Standard_Integer aMode, const Standard_Boolean updateviewer = Standard_True);
  

  //! Unsets the display mode of seen Interactive Objects.
  //! aMode provides the display mode index of the entity aniobj.
  //! If updateviewer equals Standard_True, the
  //! predominant mode aMode will overule the context mode.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the presentation of the Interactive
  //! Object returns to the default selection mode; the
  //! object is displayed but no viewer will be updated.
  Standard_EXPORT void UnsetDisplayMode (const Handle(AIS_InteractiveObject)& aniobj, const Standard_Boolean updateviewer = Standard_True);

  //! Disables the mechanism of adaptive tolerance calculation in SelectMgr_ViewerSelector and
  //! sets the given tolerance for ALL sensitive entities activated. For more information, see
  //! SelectMgr_ViewerSelector documentation
  //! Warning: When a local context is open the sensitivity is apply on it
  //! instead on the main context.
  Standard_EXPORT void SetPixelTolerance (const Standard_Integer aPrecision = 2);
  
  //! Returns the pixel tolerance.
  Standard_EXPORT Standard_Integer PixelTolerance() const;

  //! Allows to manage sensitivity of a particular selection of interactive object theObject
  //! and changes previous sensitivity value of all sensitive entities in selection with theMode
  //! to the given theNewSensitivity.
  Standard_EXPORT void SetSelectionSensitivity (const Handle(AIS_InteractiveObject)& theObject,
                                                const Standard_Integer theMode,
                                                const Standard_Integer theNewSensitivity);
  
  //! Puts the location aLocation on the initial graphic
  //! representation and the selection for the entity aniobj.
  //! In other words, aniobj is visible and selectable at a
  //! position other than initial position.
  //! Graphic and selection primitives are not recomputed.
  //! To clean the view correctly, you must reset the previous location.
  Standard_EXPORT void SetLocation (const Handle(AIS_InteractiveObject)& aniobj, const TopLoc_Location& aLocation);
  
  //! Puts the entity aniobj back into its initial position.
  Standard_EXPORT void ResetLocation (const Handle(AIS_InteractiveObject)& aniobj);
  

  //! Returns true if the entity aniobj has a location.
  Standard_EXPORT Standard_Boolean HasLocation (const Handle(AIS_InteractiveObject)& aniobj) const;
  

  //! Returns the location of the entity aniobj.
  Standard_EXPORT TopLoc_Location Location (const Handle(AIS_InteractiveObject)& aniobj) const;
  
  //! change the current facing model apply on polygons for
  //! SetColor(), SetTransparency(), SetMaterial() methods
  //! default facing model is Aspect_TOFM_TWO_SIDE. This mean that attributes is
  //! applying both on the front and back face.
  Standard_EXPORT void SetCurrentFacingModel (const Handle(AIS_InteractiveObject)& aniobj, const Aspect_TypeOfFacingModel aModel = Aspect_TOFM_BOTH_SIDE);
  
  Standard_EXPORT void SetColor (const Handle(AIS_InteractiveObject)& aniobj, const Quantity_NameOfColor aColor, const Standard_Boolean updateviewer = Standard_True);
  

  //! Sets the color of the selected entity.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the presentation   of the Interactive
  //! Object activates the selection mode; the object is
  //! displayed but no viewer will be updated.
  Standard_EXPORT void SetColor (const Handle(AIS_InteractiveObject)& aniobj, const Quantity_Color& aColor, const Standard_Boolean updateviewer = Standard_True);
  
  //! Removes the color selection for the selected entity.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the presentation of the Interactive
  //! Object activates the selection mode; the object is
  //! displayed but no viewer will be updated.
  Standard_EXPORT void UnsetColor (const Handle(AIS_InteractiveObject)& aniobj, const Standard_Boolean updateviewer = Standard_True);
  

  //! Sets the width of the entity aniobj.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the presentation of the Interactive
  //! Object activates the selection mode; the object is
  //! displayed but no viewer will be updated.
  Standard_EXPORT virtual void SetWidth (const Handle(AIS_InteractiveObject)& aniobj, const Standard_Real aValue, const Standard_Boolean updateviewer = Standard_True);
  

  //! Removes the width setting of the entity aniobj.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the presentation of the Interactive
  //! Object activates the selection mode; the object is
  //! displayed but no viewer will be updated.
  Standard_EXPORT virtual void UnsetWidth (const Handle(AIS_InteractiveObject)& aniobj, const Standard_Boolean updateviewer = Standard_True);
  

  //! Provides the type of material setting for the view of
  //! the entity aniobj.
  //! The range of settings includes: BRASS, BRONZE,
  //! GOLD, PEWTER, SILVER, STONE.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the presentation of the Interactive
  //! Object activates the selection mode; the object is
  //! displayed but no viewer will be updated.
  Standard_EXPORT void SetMaterial (const Handle(AIS_InteractiveObject)& aniobj, const Graphic3d_NameOfMaterial aName, const Standard_Boolean updateviewer = Standard_True);
  

  //! Removes the type of material setting for viewing the
  //! entity aniobj.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the presentation of the Interactive
  //! Object activates the selection mode; the object is
  //! displayed but no viewer will be updated.
  Standard_EXPORT void UnsetMaterial (const Handle(AIS_InteractiveObject)& anObj, const Standard_Boolean updateviewer = Standard_True);
  
  //! Provides the transparency settings for viewing the
  //! entity aniobj. The transparency value aValue may be
  //! between 0.0, opaque, and 1.0, fully transparent.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the presentation of the Interactive
  //! Object activates the selection mode; the object is
  //! displayed but no viewer will be updated.
  Standard_EXPORT void SetTransparency (const Handle(AIS_InteractiveObject)& aniobj, const Standard_Real aValue = 0.6, const Standard_Boolean updateviewer = Standard_True);
  

  //! Removes   the transparency settings for viewing the
  //! entity aniobj. The transparency value aValue may be
  //! between 0.0, opaque, and 1.0, fully transparent.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the presentation of the Interactive
  //! Object activates the selection mode; the object is
  //! displayed but no viewer will be updated.
  Standard_EXPORT void UnsetTransparency (const Handle(AIS_InteractiveObject)& aniobj, const Standard_Boolean updateviewer = Standard_True);
  

  //! Sets the attributes of the interactive object aniobj by
  //! plugging the attribute manager aDrawer into the local
  //! context. The graphic attributes of aDrawer such as
  //! visualization mode, color, and material, are then used
  //! to display aniobj.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the presentation of the Interactive
  //! Object activates the selection mode; the object is
  //! displayed but no viewer will be updated.
  Standard_EXPORT void SetLocalAttributes (const Handle(AIS_InteractiveObject)& aniobj, const Handle(Prs3d_Drawer)& aDrawer, const Standard_Boolean updateviewer = Standard_True);
  

  //! Removes the settings for local attributes of the entity
  //! anObj   and returns to the Neutral Point attributes or
  //! those of the previous local context.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the presentation of the Interactive
  //! Object activates the selection mode; the object is
  //! displayed but no viewer will be updated.
  Standard_EXPORT void UnsetLocalAttributes (const Handle(AIS_InteractiveObject)& anObj, const Standard_Boolean updateviewer = Standard_True);
  
  //! Sets up polygon offsets for the given AIS_InteractiveObject.
  //! It simply calls anObj->SetPolygonOffsets()
  Standard_EXPORT void SetPolygonOffsets (const Handle(AIS_InteractiveObject)& anObj, const Standard_Integer aMode, const Standard_ShortReal aFactor = 1.0, const Standard_ShortReal aUnits = 0.0, const Standard_Boolean updateviewer = Standard_True);
  
  //! simply calls anObj->HasPolygonOffsets()
  Standard_EXPORT Standard_Boolean HasPolygonOffsets (const Handle(AIS_InteractiveObject)& anObj) const;
  
  //! Retrieves current polygon offsets settings for <anObj>.
  Standard_EXPORT void PolygonOffsets (const Handle(AIS_InteractiveObject)& anObj, Standard_Integer& aMode, Standard_ShortReal& aFactor, Standard_ShortReal& aUnits) const;
  
  //! Sets the size aSize of the trihedron.
  //! Is used to change the default value 100 mm for
  //! display of trihedra.
  //! Use of this function in one of your own interactive
  //! objects requires a call to the Compute function of the
  //! new class. This will recalculate the presentation for
  //! every trihedron displayed.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the presentation of the Interactive
  //! Object activates the selection mode; the object is
  //! displayed but no viewer will be updated.
  Standard_EXPORT void SetTrihedronSize (const Standard_Real aSize, const Standard_Boolean updateviewer = Standard_True);
  
  //! returns the current value of trihedron size.
  Standard_EXPORT Standard_Real TrihedronSize() const;
  

  //! Sets the plane size defined by the length in the X
  //! direction XSize and that in the Y direction YSize.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the presentation of the Interactive
  //! Object activates the selection mode; the object is
  //! displayed but no viewer will be updated.
  Standard_EXPORT void SetPlaneSize (const Standard_Real aSizeX, const Standard_Real aSizeY, const Standard_Boolean updateviewer = Standard_True);
  

  //! Sets the plane size aSize.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the presentation of the Interactive
  //! Object activates the selection mode; the object is
  //! displayed but no viewer will be updated.
  //! May be used if PlaneSize returns true.
  Standard_EXPORT void SetPlaneSize (const Standard_Real aSize, const Standard_Boolean updateviewer = Standard_True);
  
  //! Returns true if the length in the X direction XSize is
  //! the same as that in the Y direction YSize.
  Standard_EXPORT Standard_Boolean PlaneSize (Standard_Real& XSize, Standard_Real& YSize) const;
  
  //! Returns the display status of the entity anIobj.
  //! This will be one of the following:
  //! -   DS_Displayed   displayed in main viewer
  //! -   DS_Erased   hidden in main viewer
  //! -   DS_Temporary   temporarily displayed
  //! -   DS_None   nowhere displayed.
  Standard_EXPORT AIS_DisplayStatus DisplayStatus (const Handle(AIS_InteractiveObject)& anIobj) const;
  

  //! Returns the list of active display modes for the entity aniobj.
  Standard_EXPORT const TColStd_ListOfInteger& DisplayedModes (const Handle(AIS_InteractiveObject)& aniobj) const;
  
  //! Returns true if anIobj is displayed in the interactive context.
  Standard_EXPORT Standard_Boolean IsDisplayed (const Handle(AIS_InteractiveObject)& anIobj) const;
  
  Standard_EXPORT Standard_Boolean IsDisplayed (const Handle(AIS_InteractiveObject)& aniobj, const Standard_Integer aMode) const;
  
  Standard_EXPORT Standard_Boolean IsHilighted (const Handle(AIS_InteractiveObject)& aniobj) const;
  
  //! if <anIObj> is hilighted with a specific color
  //! <WithColor> will be returned TRUE
  //! <theHiCol> gives the name of the hilightcolor
  Standard_EXPORT Standard_Boolean IsHilighted (const Handle(AIS_InteractiveObject)& anIobj, Standard_Boolean& WithColor, Quantity_NameOfColor& theHiCol) const;

  //! if <theOwner> is hilighted with a specific color, than <theIsCustomColor> will be set
  //! to true and <theCustomColorName> will have the name of the color stored
  Standard_EXPORT Standard_Boolean IsHilighted (const Handle(SelectMgr_EntityOwner)& theOwner,
                                                Standard_Boolean& theIsCustomColor,
                                                Quantity_NameOfColor& theCustomColorName) const;

  //! Returns the display priority of the entity anIobj. This
  //! will be display   mode of anIobj if it is in the main
  //! viewer.
  Standard_EXPORT Standard_Integer DisplayPriority (const Handle(AIS_InteractiveObject)& anIobj) const;
  

  //! Returns true if a view of the Interactive Object aniobj has color.
  Standard_EXPORT Standard_Boolean HasColor (const Handle(AIS_InteractiveObject)& aniobj) const;
  
  Standard_EXPORT Quantity_NameOfColor Color (const Handle(AIS_InteractiveObject)& aniobj) const;
  

  //! Returns the color Color of the entity aniobj in the interactive context.
  Standard_EXPORT void Color (const Handle(AIS_InteractiveObject)& aniobj, Quantity_Color& acolor) const;
  

  //! Returns the width of the Interactive Object aniobj in
  //! the interactive context.
  Standard_EXPORT virtual Standard_Real Width (const Handle(AIS_InteractiveObject)& aniobj) const;
  

  //! Returns the status astatus of the Interactive Context
  //! for the view of the Interactive Object anObj.
  Standard_EXPORT void Status (const Handle(AIS_InteractiveObject)& anObj, TCollection_ExtendedString& astatus) const;
  

  //! Updates the current viewer, the viewer in Neutral Point.
  //! Objects selected when there is no open local context
  //! are called current objects; those selected in open
  //! local context, selected objects.
  Standard_EXPORT void UpdateCurrentViewer();
  
  //! Returns the display mode setting.
  //! Note that mode 3 is only used.
    Standard_Integer DisplayMode() const;
  

  //! Returns the name of the color used to show
  //! highlighted entities, that is, entities picked out by the mouse.
    Quantity_NameOfColor HilightColor() const;
  

  //! Returns the name of the color used to show selected entities.
  //! By default, this is Quantity_NOC_GRAY80.
    Quantity_NameOfColor SelectionColor() const;
  
  //! Returns the name of the color used to show preselection.
  //! By default, this is Quantity_NOC_GREEN.
    Quantity_NameOfColor PreSelectionColor() const;
  

  //! Returns the name of the color used by default.
  //! By default, this is Quantity_NOC_GOLDENROD.
    Quantity_NameOfColor DefaultColor() const;
  

  //! Returns the name of the color used to show that an
  //! object is not currently selected.
  //! By default, this is Quantity_NOC_GRAY40.
    Quantity_NameOfColor SubIntensityColor() const;
  

  //! Sets the color used to show highlighted entities, that
  //! is, entities picked by the mouse.
  //! By default, this is Quantity_NOC_CYAN1.
    void SetHilightColor (const Quantity_NameOfColor aHiCol);
  

  //! Sets the color used to show selected entities.
  //! By default, this is Quantity_NOC_GRAY80.
    void SelectionColor (const Quantity_NameOfColor aCol);
  

  //! Allows you to set the color used to show preselection.
  //! By default, this is Quantity_NOC_GREEN.
  //! A preselected entity is one which has been selected
  //! as the domain of application of a function such as a fillet.
    void SetPreselectionColor (const Quantity_NameOfColor aCol);
  

  //! Sets the color used to show that an object is not currently selected.
  //! By default, this is Quantity_NOC_GRAY40.
    void SetSubIntensityColor (const Quantity_NameOfColor aCol);
  

  //! Sets the display mode of seen Interactive Objects.
  //! aMode provides the display mode index of the entity aniobj.
  //! If updateviewer equals Standard_True, the
  //! predominant mode aMode will overule the context mode.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the presentation of the Interactive
  //! Object returns to the default selection mode; the
  //! object is displayed but no viewer will be updated.
  //! Note that display mode 3 is only used if you have an
  //! AIS_Textured Shape.
  Standard_EXPORT void SetDisplayMode (const AIS_DisplayMode AMode, const Standard_Boolean updateviewer = Standard_True);
  

  //! Sets the deviation coefficient aCoefficient.
  //! Drawings of curves or patches are made with respect
  //! to a maximal chordal deviation. A Deviation coefficient
  //! is used in the shading display mode. The shape is
  //! seen decomposed into triangles. These are used to
  //! calculate reflection of light from the surface of the
  //! object. The triangles are formed from chords of the
  //! curves in the shape. The deviation coefficient
  //! aCoefficient gives the highest value of the angle with
  //! which a chord can deviate from a tangent to a   curve.
  //! If this limit is reached, a new triangle is begun.
  //! This deviation is absolute and is set through the
  //! method: SetMaximalChordialDeviation. The default
  //! value is 0.001.
  //! In drawing shapes, however, you are allowed to ask
  //! for a relative deviation. This deviation will be:
  //! SizeOfObject * DeviationCoefficient.
  //! default 0.001
  Standard_EXPORT void SetDeviationCoefficient (const Handle(AIS_InteractiveObject)& aniobj, const Standard_Real aCoefficient, const Standard_Boolean updateviewer = Standard_True);
  
  Standard_EXPORT void SetDeviationAngle (const Handle(AIS_InteractiveObject)& aniobj, const Standard_Real anAngle, const Standard_Boolean updateviewer = Standard_True);
  
  //! Calls the AIS_Shape SetAngleAndDeviation to set
  //! both Angle and Deviation coefficients
  Standard_EXPORT void SetAngleAndDeviation (const Handle(AIS_InteractiveObject)& aniobj, const Standard_Real anAngle, const Standard_Boolean updateviewer = Standard_True);
  

  //! Sets the deviation coefficient aCoefficient for
  //! removal of hidden lines created by different
  //! viewpoints in different presentations. The Default value is 0.02.
  Standard_EXPORT void SetHLRDeviationCoefficient (const Handle(AIS_InteractiveObject)& aniobj, const Standard_Real aCoefficient, const Standard_Boolean updateviewer = Standard_True);
  
  Standard_EXPORT void SetHLRDeviationAngle (const Handle(AIS_InteractiveObject)& aniobj, const Standard_Real anAngle, const Standard_Boolean updateviewer = Standard_True);
  
  //! Computes a HLRAngle and a
  //! HLRDeviationCoefficient by means of the angle
  //! anAngle and sets the corresponding methods in the
  //! default drawing tool with these values.
  Standard_EXPORT void SetHLRAngleAndDeviation (const Handle(AIS_InteractiveObject)& aniobj, const Standard_Real anAngle, const Standard_Boolean updateviewer = Standard_True);
  
  //! Sets the deviation coefficient aCoefficient.
  //! Drawings of curves or patches are made with respect
  //! to a maximal chordal deviation. A Deviation coefficient
  //! is used in the shading display mode. The shape is
  //! seen decomposed into triangles. These are used to
  //! calculate reflection of light from the surface of the
  //! object. The triangles are formed from chords of the
  //! curves in the shape. The deviation coefficient
  //! aCoefficient gives the highest value of the angle with
  //! which a chord can deviate from a tangent to a   curve.
  //! If this limit is reached, a new triangle is begun.
  //! This deviation is absolute and is set through the
  //! method: SetMaximalChordialDeviation. The default
  //! value is 0.001.
  //! In drawing shapes, however, you are allowed to ask
  //! for a relative deviation. This deviation will be:
  //! SizeOfObject * DeviationCoefficient.
  //! default 0.001
  Standard_EXPORT void SetDeviationCoefficient (const Standard_Real aCoefficient);
  
  //! Returns the deviation coefficient.
  //! Drawings of curves or patches are made with respect
  //! to a maximal chordal deviation. A Deviation coefficient
  //! is used in the shading display mode. The shape is
  //! seen decomposed into triangles. These are used to
  //! calculate reflection of light from the surface of the
  //! object. The triangles are formed from chords of the
  //! curves in the shape. The deviation coefficient gives
  //! the highest value of the angle with which a chord can
  //! deviate from a tangent to a   curve. If this limit is
  //! reached, a new triangle is begun.
  //! This deviation is absolute and is set through
  //! Prs3d_Drawer::SetMaximalChordialDeviation. The
  //! default value is 0.001.
  //! In drawing shapes, however, you are allowed to ask
  //! for a relative deviation. This deviation will be:
  //! SizeOfObject * DeviationCoefficient.
  Standard_EXPORT Standard_Real DeviationCoefficient() const;
  
  //! default 6degrees
  Standard_EXPORT void SetDeviationAngle (const Standard_Real anAngle);
  
  Standard_EXPORT Standard_Real DeviationAngle() const;
  
  //! Sets the deviation coefficient aCoefficient for
  //! removal of hidden lines created by different
  //! viewpoints in different presentations. The Default value is 0.02.
  Standard_EXPORT void SetHLRDeviationCoefficient (const Standard_Real aCoefficient);
  

  //! Returns the real number value of the hidden line
  //! removal deviation coefficient.
  //! A Deviation coefficient is used in the shading display
  //! mode. The shape is seen decomposed into triangles.
  //! These are used to calculate reflection of light from the
  //! surface of the object.
  //! The triangles are formed from chords of the curves in
  //! the shape. The deviation coefficient give the highest
  //! value of the angle with which a chord can deviate
  //! from a tangent to a curve. If this limit is reached, a
  //! new triangle is begun.
  //! To find the hidden lines, hidden line display mode
  //! entails recalculation of the view at each different
  //! projector perspective.
  //! Because hidden lines entail calculations of more than
  //! usual complexity to decompose them into these
  //! triangles, a deviation coefficient allowing greater
  //! tolerance is used. This increases efficiency in calculation.
  //! The Default value is 0.02.
  Standard_EXPORT Standard_Real HLRDeviationCoefficient() const;
  
  //! Sets the HLR angle anAngle.
  Standard_EXPORT void SetHLRAngle (const Standard_Real anAngle);
  

  //! Returns the real number value of the deviation angle
  //! in hidden line removal views in this interactive context.
  //! The default value is 20*PI/180.
  Standard_EXPORT Standard_Real HLRAngle() const;
  
  //! compute with anangle a HLRAngle and a HLRDeviationCoefficient
  //! and set them in myHLRAngle and in myHLRDeviationCoefficient
  //! of myDefaultDrawer ;
  //! anAngle is in radian ; ( 1 deg < angle in deg < 20 deg)
  Standard_EXPORT void SetHLRAngleAndDeviation (const Standard_Real anAngle);
  
  //! Initializes hidden line aspect in the default drawing tool, or Drawer.
  //! The default values are:
  //! Color: Quantity_NOC_YELLOW
  //! Type of line: Aspect_TOL_DASH
  //! Width: 1.
  Standard_EXPORT Handle(Prs3d_LineAspect) HiddenLineAspect() const;
  

  //! Sets the hidden line aspect anAspect.
  //! anAspect defines display attributes for hidden lines in
  //! HLR projections.
  Standard_EXPORT void SetHiddenLineAspect (const Handle(Prs3d_LineAspect)& anAspect) const;
  
  //! returns Standard_True if the hidden lines are to be drawn.
  //! By default the hidden lines are not drawn.
  Standard_EXPORT Standard_Boolean DrawHiddenLine() const;
  
  Standard_EXPORT void EnableDrawHiddenLine() const;
  
  Standard_EXPORT void DisableDrawHiddenLine() const;
  
  //! Sets the number of U and V isoparameters displayed.
  Standard_EXPORT void SetIsoNumber (const Standard_Integer NbIsos, const AIS_TypeOfIso WhichIsos = AIS_TOI_Both);
  
  //! Returns the number of U and V isoparameters displayed.
  Standard_EXPORT Standard_Integer IsoNumber (const AIS_TypeOfIso WhichIsos = AIS_TOI_Both);
  
  //! Returns True if drawing isoparameters on planes is enabled.
  Standard_EXPORT void IsoOnPlane (const Standard_Boolean SwitchOn);
  
  //! Returns True if drawing isoparameters on planes is enabled.
  //! if <forUIsos> = False,
  Standard_EXPORT Standard_Boolean IsoOnPlane() const;

  //! Enables or disables on-triangulation build for isolines for a particular object.
  //! In case if on-triangulation builder is disabled, default on-plane
  //! builder will compute isolines for the object given.
  Standard_EXPORT void IsoOnTriangulation (const Standard_Boolean theIsEnabled,
                                           const Handle(AIS_InteractiveObject)& theObject);

  //! Enables or disables on-triangulation build for isolines for default drawer.
  //! In case if on-triangulation builder is disabled, default on-plane
  //! builder will compute isolines for the object given.
  Standard_EXPORT void IsoOnTriangulation (const Standard_Boolean theToSwitchOn);

  //! Returns true if drawing isolines on triangulation algorithm is enabled.
  Standard_EXPORT Standard_Boolean IsoOnTriangulation() const;

  //! Sets the graphic basic aspect to the current presentation of
  //! ALL selected objects.
  //! When <globalChange> is TRUE , the full object presentation
  //! is changed.
  //! When <globalChange> is FALSE , only the current group
  //! of the object presentation is changed.
  //! Updates the viewer when <updateViewer> is TRUE
  Standard_EXPORT void SetSelectedAspect (const Handle(Prs3d_BasicAspect)& anAspect, const Standard_Boolean globalChange = Standard_True, const Standard_Boolean updateViewer = Standard_True);
  
  //! Relays mouse position in pixels theXPix and theYPix to the interactive context selectors.
  //! This is done by the view theView passing this position to the main viewer and updating it.
  //! Functions in both Neutral Point and local contexts.
  //! If theToRedrawOnUpdate is set to false, callee should call RedrawImmediate() to highlight detected object.
  Standard_EXPORT AIS_StatusOfDetection MoveTo (const Standard_Integer theXPix, const Standard_Integer theYPix, const Handle(V3d_View)& theView, const Standard_Boolean theToRedrawOnUpdate = Standard_True);
  
  //! returns True  if other entities  were detected  in the
  //! last mouse detection
  Standard_EXPORT Standard_Boolean HasNextDetected() const;
  
  //! if more than 1 object is detected by the selector,
  //! only the "best" owner is hilighted at the mouse position.
  //! This Method allows the user to hilight one after another
  //! the other detected entities.
  //! if The method select is called, the selected entity
  //! will be the hilighted one!
  //! returns the Rank of hilighted entity
  //! WARNING : Loop Method. When all the detected entities
  //! have been hilighted , the next call will hilight
  //! the first one again
  Standard_EXPORT Standard_Integer HilightNextDetected (const Handle(V3d_View)& theView, const Standard_Boolean theToRedrawImmediate = Standard_True);
  
  //! Same as previous methods in reverse direction...
  Standard_EXPORT Standard_Integer HilightPreviousDetected (const Handle(V3d_View)& theView, const Standard_Boolean theToRedrawImmediate = Standard_True);
  
  //! Selects everything found in the bounding rectangle
  //! defined by the pixel minima and maxima, XPMin,
  //! YPMin, XPMax, and YPMax in the view, aView
  //! The objects detected are passed to the main viewer,
  //! which is then updated.
  Standard_EXPORT AIS_StatusOfPick Select (const Standard_Integer XPMin, const Standard_Integer YPMin, const Standard_Integer XPMax, const Standard_Integer YPMax, const Handle(V3d_View)& aView, const Standard_Boolean updateviewer = Standard_True);
  
  //! polyline selection; clears the previous picked list
  Standard_EXPORT AIS_StatusOfPick Select (const TColgp_Array1OfPnt2d& Polyline, const Handle(V3d_View)& aView, const Standard_Boolean updateviewer = Standard_True);
  
  //! Stores  and hilights the previous detected; Unhilights
  //! the previous picked.
  Standard_EXPORT AIS_StatusOfPick Select (const Standard_Boolean updateviewer = Standard_True);
  
  //! adds the last detected to the list of previous picked.
  //! if the last detected was already declared as picked,
  //! removes it from the Picked List.
  Standard_EXPORT AIS_StatusOfPick ShiftSelect (const Standard_Boolean updateviewer = Standard_True);
  
  //! adds the last detected to the list of previous picked.
  //! if the last detected was already declared as picked,
  //! removes it from the Picked List.
  Standard_EXPORT AIS_StatusOfPick ShiftSelect (const TColgp_Array1OfPnt2d& Polyline, const Handle(V3d_View)& aView, const Standard_Boolean updateviewer = Standard_True);
  
  //! rectangle  of selection  ; adds new detected entities into the
  //! picked list, removes the detected entities that were already stored...
  Standard_EXPORT AIS_StatusOfPick ShiftSelect (const Standard_Integer XPMin, const Standard_Integer YPMin, const Standard_Integer XPMax, const Standard_Integer YPMax, const Handle(V3d_View)& aView, const Standard_Boolean updateviewer = Standard_True);
  
  //! Specify whether selected object must be hilighted when mouse cursor
  //! is moved above it (in MoveTo method). By default this value is false and
  //! selected object is not hilighted in this case.
    void SetToHilightSelected (const Standard_Boolean toHilight);
  
  //! Return value specified whether selected object must be hilighted
  //! when mouse cursor is moved above it
    Standard_Boolean ToHilightSelected() const;


  //! @name OBSOLETE METHODS THAT ARE VALID FOR LOCAL CONTEXT ONLY

  //! Updates the view of the current object in open context.
  //! Objects selected when there is no open local context
  //! are called current objects; those selected in open
  //! local context, selected objects.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the presentation of the Interactive
  //! Object activates the selection mode; the object is
  //! displayed but no viewer will be updated.
  Standard_EXPORT void SetCurrentObject (const Handle(AIS_InteractiveObject)& aniobj, const Standard_Boolean updateviewer = Standard_True);
  

  //! Allows to add or remove the object given to the list of current and highlight/unhighlight it
  //! correspondingly. Is valid for global context only; for local context use method AddOrRemoveSelected.
  //! Since this method makes sence only for neutral point selection of a whole object, if 0 selection
  //! of the object is empty this method simply does nothing.
  Standard_EXPORT void AddOrRemoveCurrentObject (const Handle(AIS_InteractiveObject)& theObj,
                                                 const Standard_Boolean theIsToUpdateViewer = Standard_True);
  
  //! Updates the list of current objects, i.e. hilights new
  //! current objects, removes hilighting from former current objects.
  //! Objects selected when there is no open local context
  //! are called current objects; those selected in open
  //! local context, selected objects.
  Standard_EXPORT void UpdateCurrent();
  

  //! Returns the current selection touched by the cursor.
  //! Objects selected when there is no open local context
  //! are called current objects; those selected in open
  //! local context, selected objects.
    Standard_Boolean WasCurrentTouched() const;
  
    void SetOkCurrent();
  

  //! Returns true if there is a non-null interactive object in Neutral Point.
  //! Objects selected when there is no open local context are called current objects;
  //! those selected in open local context, selected objects.
  Standard_EXPORT Standard_Boolean IsCurrent (const Handle(AIS_InteractiveObject)& theObject) const;
  

  //! Initializes a scan of the current selected objects in
  //! Neutral Point.
  //! Objects selected when there is no open local context
  //! are called current objects; those selected in open
  //! local context, selected objects.
  Standard_EXPORT void InitCurrent();
  

  //! Returns true if there is another object found by the
  //! scan of the list of current objects.
  //! Objects selected when there is no open local context
  //! are called current objects; those selected in open
  //! local context, selected objects.
  Standard_EXPORT Standard_Boolean MoreCurrent() const;
  

  //! Continues the scan to the next object in the list of
  //! current objects.
  //! Objects selected when there is no open local context
  //! are called current objects; those selected in open
  //! local context, selected objects.
  Standard_EXPORT void NextCurrent();
  

  //! Returns the current interactive object.
  //! Objects selected when there is no open local context
  //! are called current objects; those selected in open
  //! local context, selected objects.
  Standard_EXPORT Handle(AIS_InteractiveObject) Current() const;
  
  Standard_EXPORT Standard_Integer NbCurrents();

  //! Highlights current objects.
  //! Objects selected when there is no open local context
  //! are called current objects; those selected in open
  //! local context, selected objects.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the presentation of the Interactive
  //! Object activates the selection mode; the object is
  //! displayed but no viewer will be updated.
  Standard_EXPORT void HilightCurrents (const Standard_Boolean theToUpdateViewer = Standard_True);

  //! Removes highlighting from current objects.
  //! Objects selected when there is no open local context
  //! are called current objects; those selected in open
  //! local context, selected objects.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the presentation of the Interactive
  //! Object activates the selection mode; the object is
  //! displayed but no viewer will be updated.
  Standard_EXPORT void UnhilightCurrents (const Standard_Boolean updateviewer = Standard_True);

  //! Empties previous current objects in order to get the
  //! current objects detected by the selector using
  //! UpdateCurrent.
  //! Objects selected when there is no open local context
  //! are called current objects; those selected in open
  //! local context, selected objects.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the presentation of the Interactive
  //! Object activates the selection mode; the object is
  //! displayed but no viewer will be updated.
  Standard_EXPORT void ClearCurrents (const Standard_Boolean theToUpdateViewer = Standard_True);

  //! @return current mouse-detected shape or empty (null) shape, if current interactive object
  //! is not a shape (AIS_Shape) or there is no current mouse-detected interactive object at all.
  Standard_EXPORT const TopoDS_Shape& DetectedCurrentShape() const;
  
  //! @return current mouse-detected interactive object or null object, if there is no
  //! currently detected interactives
  Standard_EXPORT Handle(AIS_InteractiveObject) DetectedCurrentObject() const;

  //! @name COMMON SELECTION METHODS VALID FOR BOTH GLOBAL AND LOCAL CONTEXT

  //! Unhighlights previously selected owners and marks them as not selected.
  //! Marks owner given as selected and highlights it.
  //! Performs selection filters check.
  Standard_EXPORT void SetSelected (const Handle(SelectMgr_EntityOwner)& theOwners,
                                    const Standard_Boolean theToUpdateViewer = Standard_True);

  //! Puts the interactive object aniObj in the list of
  //! selected objects.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the presentation of the Interactive
  //! Object activates the selection mode; the object is
  //! displayed but no viewer will be updated.
  //! Performs selection filters check.
  Standard_EXPORT void SetSelected (const Handle(AIS_InteractiveObject)& theObject,
                                    const Standard_Boolean theToUpdateViewer = Standard_True);

  //! Updates the list of selected objects:
  //! i.e. highlights the newely selected ones and unhighlights previously selected objects.
  Standard_EXPORT void UpdateSelected (const Standard_Boolean theToUpdateViewer = Standard_True);
  
  //! Allows to highlight or unhighlight the owner given depending on its selection status
  Standard_EXPORT void AddOrRemoveSelected (const Handle(AIS_InteractiveObject)& theObject,
                                            const Standard_Boolean theToUpdateViewer = Standard_True);

  //! Highlights selected objects.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the presentation of the Interactive
  //! Object activates the selection mode; the object is
  //! displayed but no viewer will be updated.
  Standard_EXPORT void HilightSelected (const Standard_Boolean theToUpdateViewer = Standard_True);

  //! Removes highlighting from selected objects.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the presentation of the Interactive
  //! Object activates the selection mode; the object is
  //! displayed but no viewer will be updated.
  Standard_EXPORT void UnhilightSelected (const Standard_Boolean theToUpdateViewer = Standard_True);

  //! Empties previous selected objects in order to get the
  //! selected objects detected by the selector using
  //! UpdateSelected.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the presentation of the Interactive
  //! Object activates the selection mode; the object is
  //! displayed but no viewer will be updated.
  Standard_EXPORT void ClearSelected (const Standard_Boolean theToUpdateViewer = Standard_True);
  
  //! No right to Add a selected Shape (Internal Management
  //! of shape Selection).
  //! A Previous selected shape may only be removed.
  Standard_EXPORT void AddOrRemoveSelected (const TopoDS_Shape& aShape, const Standard_Boolean updateviewer = Standard_True);

  //! Allows to highlight or unhighlight the owner given depending on its selection status
  Standard_EXPORT void AddOrRemoveSelected (const Handle(SelectMgr_EntityOwner)& theOwner,
                                            const Standard_Boolean theToUpdateViewer = Standard_True);

  //! Returns true is the owner given is selected
  Standard_EXPORT Standard_Boolean IsSelected (const Handle(SelectMgr_EntityOwner)& theOwner) const;

  //! Returns true is the object given is selected
  Standard_EXPORT Standard_Boolean IsSelected (const Handle(AIS_InteractiveObject)& theObj) const;

  //! Returns the first selected object in the list of current selected.
  Standard_EXPORT Handle(AIS_InteractiveObject) FirstSelectedObject();

  //! Initializes a scan of the selected objects.
  Standard_EXPORT void InitSelected();

  //! Returns true if there is another object found by the
  //! scan of the list of selected objects.
  Standard_EXPORT Standard_Boolean MoreSelected() const;

  //! Continues the scan to the next object in the list of
  //! selected objects.
  Standard_EXPORT void NextSelected();

  Standard_EXPORT Standard_Integer NbSelected();

  //! Returns true if the interactive context has a shape
  //! selected in it which results from the decomposition of
  //! another entity.
  //! If HasSelectedShape returns true, SelectedShape
  //! returns the shape which has been shown to be
  //! selected. Interactive returns the Interactive Object
  //! from which the shape has been selected.
  //! If HasSelectedShape returns false, Interactive
  //! returns the interactive entity selected by the click of the mouse.
  Standard_EXPORT Standard_Boolean HasSelectedShape() const;

  //! Returns the selected shape.
  Standard_EXPORT TopoDS_Shape SelectedShape() const;

  //! Returns the owner of the selected entity resulting
  //! from the decomposition of another entity.
  Standard_EXPORT Handle(SelectMgr_EntityOwner) SelectedOwner() const;

  //! Returns a collection containing all entity owners
  //! created for the interactive object <theIObj> in
  //! the selection mode theMode (in all active modes
  //! if the Mode == -1)
  Standard_EXPORT void EntityOwners (Handle(SelectMgr_IndexedMapOfOwner)& theOwners,
                                     const Handle(AIS_InteractiveObject)& theIObj,
                                     const Standard_Integer theMode = -1) const;

  Standard_EXPORT Handle(AIS_InteractiveObject) SelectedInteractive() const;

  //! Returns true if the applicative object has an owner
  //! from Interactive attributed to it.
  Standard_EXPORT Standard_Boolean HasApplicative() const;
  

  //! Returns the owner of the applicative entity detected
  //! in interactive context. The owner can be a shape for
  //! a set of sub-shapes or a sub-shape for sub-shapes
  //! which it is composed of.
  Standard_EXPORT Handle(Standard_Transient) Applicative() const;
  

  //! Returns true if there is a mouse-detected entity in local context.
  //! If there is no open local context, the objects selected
  //! are called current objects; selected objects if there is
  //! one. Iterators allow entities to be recovered in either
  //! case. This method is one of a set which allows you to
  //! manipulate the objects which have been placed in these two lists.
  Standard_EXPORT Standard_Boolean HasDetected() const;
  

  //! Returns true if there is a detected shape in local context.
  //! If there is no open local context, the objects selected
  //! are called current objects; selected objects if there is
  //! one. Iterators allow entities to be recovered in either
  //! case. This method is one of a set which allows you to
  //! manipulate the objects which have been placed in these two lists.
  Standard_EXPORT Standard_Boolean HasDetectedShape() const;
  

  //! Returns the shape detected in local context.
  //! If there is no open local context, the objects selected
  //! are called current objects; selected objects if there is
  //! one. Iterators allow entities to be recovered in either
  //! case. This method is one of a set which allows you to
  //! manipulate the objects which have been placed in these two lists.
  Standard_EXPORT const TopoDS_Shape& DetectedShape() const;
  

  //! Returns the interactive objects last detected in open context.
  //! If there is no open local context, the objects selected
  //! are called current objects; selected objects if there is
  //! one. Iterators allow entities to be recovered in either
  //! case. This method is one of a set which allows you to
  //! manipulate the objects which have been placed in these two lists.
  Standard_EXPORT Handle(AIS_InteractiveObject) DetectedInteractive() const;
  
  //! returns the owner of the detected sensitive primitive.
  Standard_EXPORT Handle(SelectMgr_EntityOwner) DetectedOwner() const;
  

  //! Initialization for iteration through mouse-detected objects in
  //! interactive context or in local context if it is opened.
  Standard_EXPORT void InitDetected();
  

  //! @return true if there is more mouse-detected objects after the current one
  //! during iteration through mouse-detected interactive objects.
  Standard_EXPORT Standard_Boolean MoreDetected() const;
  

  //! Gets next current object during iteration through mouse-detected
  //! interactive objects.
  Standard_EXPORT void NextDetected();

  //! Opens local contexts and specifies how this is to be
  //! done. The options listed above function in the following manner:
  //! -   UseDisplayedObjects -allows you to load or not
  //! load the interactive objects visualized at Neutral
  //! Point in the local context which you open. If false,
  //! the local context is empty after being opened. If
  //! true, the objects at Neutral Point are loaded by their
  //! default selection mode.
  //! -   AllowShapeDecomposition -AIS_Shape allows or
  //! prevents decomposition in standard shape location
  //! mode of objects at Neutral Point which are
  //! type-"privileged". This Flag is only taken into
  //! account when UseDisplayedObjects is true.
  //! -   AcceptEraseOfObjects -authorises other local
  //! contexts to erase the interactive objects present in
  //! this context. This option is rarely used.
  //! -   BothViewers - Has no use currently defined.
  //! This method returns the index of the created local
  //! context. It should be kept and used to close the context.
  //! Opening a local context allows you to prepare an
  //! environment for temporary presentations and
  //! selections which will disappear once the local context is closed.
  //! You can open several local contexts, but only the last
  //! one will be active.
  Standard_EXPORT Standard_Integer OpenLocalContext (const Standard_Boolean UseDisplayedObjects = Standard_True, const Standard_Boolean AllowShapeDecomposition = Standard_True, const Standard_Boolean AcceptEraseOfObjects = Standard_False, const Standard_Boolean BothViewers = Standard_False);
  
  //! Allows you to close local contexts. For greater
  //! security, you should close the context with the
  //! index Index given on opening.
  //! When you close a local context, the one before,
  //! which is still on the stack,   reactivates. If none is
  //! left, you return to Neutral Point.
  //! If a local context is open and if updateviewer
  //! equals Standard_False, the presentation of the
  //! Interactive Object activates the selection mode; the
  //! object is displayed but no viewer will be updated.
  //! Warning
  //! When the index isn't specified, the current context
  //! is closed. This option can be dangerous, as other
  //! Interactive Functions can open local contexts
  //! without necessarily warning the user.
  Standard_EXPORT void CloseLocalContext (const Standard_Integer Index = -1, const Standard_Boolean updateviewer = Standard_True);
  
  //! returns -1 if no opened local context.
  Standard_EXPORT Standard_Integer IndexOfCurrentLocal() const;
  

  //! Allows you to close all local contexts at one go and
  //! return to Neutral Point.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the presentation of the Interactive
  //! Object activates the selection mode; the object is
  //! displayed but no viewer will be updated.
  Standard_EXPORT void CloseAllContexts (const Standard_Boolean updateviewer = Standard_True);
  
  //! to   be  used only with no  opened
  //! local context..  displays and activates objects in their
  //! original state before local contexts were opened...
  Standard_EXPORT void ResetOriginalState (const Standard_Boolean updateviewer = Standard_True);
  
  //! clears Objects/Filters/Activated Modes list in the current opened
  //! local context.
  Standard_EXPORT void ClearLocalContext (const AIS_ClearMode TheMode = AIS_CM_All);
  
  Standard_EXPORT void UseDisplayedObjects();
  
  //! when a local Context is opened, one is able to
  //! use/not use the displayed objects at neutral point
  //! at anytime.
  Standard_EXPORT void NotUseDisplayedObjects();
  
  //! initializes the list of presentations to be displayed
  //! returns False if no local context is opened.
  Standard_EXPORT Standard_Boolean BeginImmediateDraw();
  
  //! returns True if <anIObj> has been stored in the list.
  Standard_EXPORT Standard_Boolean ImmediateAdd (const Handle(AIS_InteractiveObject)& theObj, const Standard_Integer theMode = 0);
  
  //! returns True if the immediate display has been done.
  Standard_EXPORT Standard_Boolean EndImmediateDraw (const Handle(V3d_View)& theView);
  
  //! Uses the First Active View of Main Viewer!
  //! returns True if the immediate display has been done.
  Standard_EXPORT Standard_Boolean EndImmediateDraw();
  
  Standard_EXPORT Standard_Boolean IsImmediateModeOn() const;
  

  //! Sets the highlighting status aStatus of detected and
  //! selected entities.
  //! Whether you are in Neutral Point or local context, this
  //! is automatically managed by the Interactive Context.
  //! This function allows you to disconnect the automatic mode.
  Standard_EXPORT void SetAutomaticHilight (const Standard_Boolean aStatus);
  

  //! Returns true if the automatic highlight mode is active
  //! in an open context.
  Standard_EXPORT Standard_Boolean AutomaticHilight() const;
  
  //! Enables/Disables the Z detection.
  //! If TRUE the detection echo can be partially hidden by the
  //! detected object.
  Standard_EXPORT void SetZDetection (const Standard_Boolean aStatus = Standard_False);
  
  //! Retrieves the Z detection state.
  Standard_EXPORT Standard_Boolean ZDetection() const;
  
  //! Activates the selection mode aMode whose index is
  //! given, for the given interactive entity anIobj.
  Standard_EXPORT void Activate (const Handle(AIS_InteractiveObject)& anIobj, const Standard_Integer aMode = 0, const Standard_Boolean theIsForce = Standard_False);
  
  //! Deactivates all the activated selection modes
  //! of an object.
  Standard_EXPORT void Deactivate (const Handle(AIS_InteractiveObject)& anIObj);
  

  //! Deactivates all the activated selection modes of the
  //! interactive object anIobj with a given selection mode aMode.
  Standard_EXPORT void Deactivate (const Handle(AIS_InteractiveObject)& anIobj, const Standard_Integer aMode);
  

  //! Returns the list of activated selection modes in an open context.
  Standard_EXPORT void ActivatedModes (const Handle(AIS_InteractiveObject)& anIobj, TColStd_ListOfInteger& theList) const;
  
  //! to be Used only with opened local context and
  //! if <anIobj> is of type shape...
  //! if <aStatus> = True <anIobj> will be sensitive to
  //! shape selection modes activation.
  //! = False, <anIobj> will not be senstive
  //! any more.
  Standard_EXPORT void SetShapeDecomposition (const Handle(AIS_InteractiveObject)& anIobj, const Standard_Boolean aStatus);
  

  //! Sets the temporary graphic attributes of the entity
  //! anObj. These are provided by the attribute manager
  //! aDrawer and are valid for a particular local context only.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the presentation of the Interactive
  //! Object activates the selection mode; the object is
  //! displayed but no viewer will be updated.
  Standard_EXPORT void SetTemporaryAttributes (const Handle(AIS_InteractiveObject)& anObj, const Handle(Prs3d_Drawer)& aDrawer, const Standard_Boolean updateviewer = Standard_True);
  

  //! Highlights, and removes highlights from, the displayed
  //! object aniobj which is displayed at Neutral Point with
  //! subintensity color; available only for active local
  //! context. There is no effect if there is no local context.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the presentation of the Interactive
  //! Object activates the selection mode; the object is
  //! displayed but no viewer will be updated.
  Standard_EXPORT void SubIntensityOn (const Handle(AIS_InteractiveObject)& aniobj, const Standard_Boolean updateviewer = Standard_True);
  

  //! Removes the subintensity option for the entity aniobj.
  //! If a local context is open and if updateviewer equals
  //! Standard_False, the presentation of the Interactive
  //! Object activates the selection mode; the object is
  //! displayed but no viewer will be updated.
  Standard_EXPORT void SubIntensityOff (const Handle(AIS_InteractiveObject)& aniobj, const Standard_Boolean updateviewer = Standard_True);
  
  //! hilights/unhilights displayed objects which are displayed at
  //! neutral state with subintensity color;
  //! available only for active local context.
  //! No effect if no local context.
  Standard_EXPORT void SubIntensityOn (const Standard_Boolean updateviewer = Standard_True);
  
  //! removes subintensity option for all objects.
  Standard_EXPORT void SubIntensityOff (const Standard_Boolean updateviewer = Standard_True);
  
  //! Allows you to add the filter aFilter to Neutral Point or
  //! to a local context if one or more selection modes have been activated.
  //! Only type filters may be active in Neutral Point.
  Standard_EXPORT void AddFilter (const Handle(SelectMgr_Filter)& aFilter);
  

  //! Removes a filter from Neutral Point or a local context
  //! if one or more selection modes have been activated.
  //! Only type filters are activated in Neutral Point.
  Standard_EXPORT void RemoveFilter (const Handle(SelectMgr_Filter)& aFilter);
  
  //! Remove a filter to Neutral Point or a local context if
  //! one or more selection modes have been activated.
  //! Only type filters are active in Neutral Point.
  Standard_EXPORT void RemoveFilters();
  
  //! Provides an alternative to the Display methods when
  //! activating specific selection modes. This has the
  //! effect of activating the corresponding selection mode
  //! aStandardActivation for all objects in Local Context
  //! which accept decomposition into sub-shapes.
  //! Every new Object which has been loaded into the
  //! interactive context and which answers these
  //! decomposition criteria is automatically activated
  //! according to these modes.
  //! Warning
  //! If you have opened a local context by loading an
  //! object with the default options
  //! (<AllowShapeDecomposition >= Standard_True), all
  //! objects of the "Shape" type are also activated with
  //! the same modes. You can act on the state of these
  //! "Standard" objects by using SetShapeDecomposition(Status).
  Standard_EXPORT void ActivateStandardMode (const TopAbs_ShapeEnum aStandardActivation);
  

  //! Provides an alternative to the Display methods when
  //! deactivating specific selection modes. This has the
  //! effect of deactivating the corresponding selection
  //! mode aStandardActivation for all objects in Local
  //! Context which accept decomposition into sub-shapes.
  Standard_EXPORT void DeactivateStandardMode (const TopAbs_ShapeEnum aStandardActivation);
  

  //! Returns the list of activated standard selection modes
  //! available in a local context.
  Standard_EXPORT const TColStd_ListOfInteger& ActivatedStandardModes() const;
  

  //! Returns the list of filters active in a local context.
  Standard_EXPORT const SelectMgr_ListOfFilter& Filters() const;
  

  //! Returns the default attribute manager.
  //! This contains all the color and line attributes which
  //! can be used by interactive objects which do not have
  //! their own attributes.
    const Handle(Prs3d_Drawer)& DefaultDrawer() const;
  
  //! Returns the current viewer.
    const Handle(V3d_Viewer)& CurrentViewer() const;
  
  //! Returns the list of displayed objects of a particular
  //! Type WhichKind and Signature WhichSignature. By
  //! Default, WhichSignature equals -1. This means that
  //! there is a check on type only.
  Standard_EXPORT void DisplayedObjects (AIS_ListOfInteractive& aListOfIO, const Standard_Boolean OnlyFromNeutral = Standard_False) const;
  
  //! gives the list of displayed objects of a particular
  //! Type and signature.
  //! by Default, <WhichSignature> = -1 means
  //! control only on <WhichKind>.
  Standard_EXPORT void DisplayedObjects (const AIS_KindOfInteractive WhichKind, const Standard_Integer WhichSignature, AIS_ListOfInteractive& aListOfIO, const Standard_Boolean OnlyFromNeutral = Standard_False) const;
  

  //! Returns the list theListOfIO of erased objects (hidden objects)
  //! particular Type WhichKind and Signature WhichSignature.
  //! By Default, WhichSignature equals 1. This means
  //! that there is a check on type only.
  Standard_EXPORT void ErasedObjects (AIS_ListOfInteractive& theListOfIO) const;
  
  //! gives the list of erased objects (hidden objects)
  //! Type and signature
  //! by Default, <WhichSignature> = -1 means
  //! control only on <WhichKind>.
  Standard_EXPORT void ErasedObjects (const AIS_KindOfInteractive WhichKind, const Standard_Integer WhichSignature, AIS_ListOfInteractive& theListOfIO) const;
  

  //! Returns the list theListOfIO of objects with indicated display status
  //! particular Type WhichKind and Signature WhichSignature.
  //! By Default, WhichSignature equals 1. This means
  //! that there is a check on type only.
  Standard_EXPORT void ObjectsByDisplayStatus (const AIS_DisplayStatus theStatus, AIS_ListOfInteractive& theListOfIO) const;
  
  //! gives the list of objects with indicated display status
  //! Type and signature
  //! by Default, <WhichSignature> = -1 means
  //! control only on <WhichKind>.
  Standard_EXPORT void ObjectsByDisplayStatus (const AIS_KindOfInteractive WhichKind, const Standard_Integer WhichSignature, const AIS_DisplayStatus theStatus, AIS_ListOfInteractive& theListOfIO) const;
  
  //! fills <aListOfIO> with objects of a particular
  //! Type and Signature with no consideration of display status.
  //! by Default, <WhichSignature> = -1 means
  //! control only on <WhichKind>.
  //! if <WhichKind> = AIS_KOI_None and <WhichSignature> = -1,
  //! all the objects are put into the list.
  Standard_EXPORT void ObjectsInside (AIS_ListOfInteractive& aListOfIO, const AIS_KindOfInteractive WhichKind = AIS_KOI_None, const Standard_Integer WhichSignature = -1) const;
  
  //! Returns true if there is an open context.
    Standard_Boolean HasOpenedContext() const;
  

  //! Returns the name of the current selected entity in Neutral Point.
  //! Objects selected when there is no open local context
  //! are called current objects; those selected in open
  //! local context, selected objects.
    const TCollection_AsciiString& CurrentName() const;
  

  //! Returns the name of the current selected entity in
  //! open local context.
  //! Objects selected when there is no open local context
  //! are called current objects; those selected in open
  //! local context, selected objects.
  Standard_EXPORT const TCollection_AsciiString& SelectionName() const;
  
  //! Returns the domain name of the main viewer.
  Standard_EXPORT Standard_CString DomainOfMainViewer() const;
  

  //! This method is only intended for advanced operation, particularly with
  //! the aim to improve performance when many objects have to be selected
  //! together. Otherwise, you should use other (non-internal) methods of
  //! class AIS_InteractiveContext without trying to obtain an instance of
  //! AIS_LocalContext.
    Handle(AIS_LocalContext) LocalContext() const;
  
    const Handle(SelectMgr_SelectionManager)& SelectionManager() const;
  
    const Handle(PrsMgr_PresentationManager3d)& MainPrsMgr() const;
  
    const Handle(StdSelect_ViewerSelector3d)& MainSelector() const;
  
  Standard_EXPORT Handle(StdSelect_ViewerSelector3d) LocalSelector() const;
  
  //! Clears all the structures which don't
  //! belong to objects displayed at neutral point
  //! only effective when no Local Context is opened...
  //! returns the number of removed  structures from the viewers.
  Standard_EXPORT Standard_Integer PurgeDisplay();
  
  Standard_EXPORT Standard_Integer HighestIndex() const;
  
  Standard_EXPORT void DisplayActiveSensitive (const Handle(V3d_View)& aView);
  
  Standard_EXPORT void ClearActiveSensitive (const Handle(V3d_View)& aView);
  
  //! Fits the view correspondingly to the bounds of selected objects.
  //! Infinite objects are ignored if infinite state of AIS_InteractiveObject
  //! is set to true.
  Standard_EXPORT void FitSelected (const Handle(V3d_View)& theView, const Standard_Real theMargin = 0.01, const Standard_Boolean theToUpdate = Standard_True);
  
  Standard_EXPORT void DisplayActiveSensitive (const Handle(AIS_InteractiveObject)& anObject, const Handle(V3d_View)& aView);
  
  //! returns if possible,
  //! the first local context where the object is seen
  Standard_EXPORT Standard_Boolean IsInLocal (const Handle(AIS_InteractiveObject)& anObject, Standard_Integer& TheIndex) const;
  
  //! Rebuilds 1st level of BVH selection forcibly
  Standard_EXPORT void RebuildSelectionStructs();
  
  //! setup object visibility in specified view,
  //! has no effect if object is not disaplyed in this context.
  Standard_EXPORT void SetViewAffinity (const Handle(AIS_InteractiveObject)& theIObj, const Handle(V3d_View)& theView, const Standard_Boolean theIsVisible);
  
  //! Disconnects theObjToDisconnect from theAssembly and removes dependent selection structures
  Standard_EXPORT void Disconnect (const Handle(AIS_InteractiveObject)& theAssembly, const Handle(AIS_InteractiveObject)& theObjToDisconnect = NULL);
  
  //! Query objects visible or hidden in specified view due to affinity mask.
  Standard_EXPORT void ObjectsForView (AIS_ListOfInteractive& theListOfIO, const Handle(V3d_View)& theView, const Standard_Boolean theIsVisibleInView, const AIS_DisplayStatus theStatus = AIS_DS_None) const;

  //! Redraws immediate structures in all views of the viewer given taking into account its visibility.
  Standard_EXPORT void RedrawImmediate (const Handle(V3d_Viewer)& theViewer);


friend class AIS_LocalContext;


  DEFINE_STANDARD_RTTI(AIS_InteractiveContext,MMgt_TShared)

protected:




private:

  
  Standard_EXPORT void GetDefModes (const Handle(AIS_InteractiveObject)& anIobj, Standard_Integer& Dmode, Standard_Integer& HiMod, Standard_Integer& SelMode) const;
  
  Standard_EXPORT void EraseGlobal (const Handle(AIS_InteractiveObject)& anObj, const Standard_Boolean updateviewer = Standard_True);
  
  Standard_EXPORT void ClearGlobal (const Handle(AIS_InteractiveObject)& anObj, const Standard_Boolean updateviewer = Standard_True);
  
  Standard_EXPORT void ClearGlobalPrs (const Handle(AIS_InteractiveObject)& anObj, const Standard_Integer aMode, const Standard_Boolean updateviewer = Standard_True);
  
  Standard_EXPORT void InitAttributes();
  
  Standard_EXPORT Standard_Integer PurgeViewer (const Handle(V3d_Viewer)& Vwr);
  
  //! UNKNOWN
  Standard_EXPORT void redisplayPrsModes (const Handle(AIS_InteractiveObject)& theIObj, const Standard_Boolean theToUpdateViewer = Standard_True);
  
  //! UNKNOWN
  Standard_EXPORT void redisplayPrsRecModes (const Handle(AIS_InteractiveObject)& theIObj, const Standard_Boolean theToUpdateViewer = Standard_True);

  //! Helper function to unhighlight all entity owners currently highlighted with seleciton color.
  void unhighlightOwners (const Handle(AIS_InteractiveObject)& theObject);

  //! Helper function that highlights the owner given with <theColor> without
  //! performing AutoHighlight checks, e.g. is used for dynamic highlight.
  //! If the parameter <theViewer> is set and <theIsImmediate> is true, highlight will be synchronized
  //! automatically in all views of the viewer.
  void highlightWithColor (const Handle(SelectMgr_EntityOwner)& theOwner,
                           const Quantity_NameOfColor theColor,
                           const Handle(V3d_Viewer)& theViewer = NULL);

  //! Helper function that highlights the owner given with <theColor> with check
  //! for AutoHighlight, e.g. is used for selection.
  //! If the parameter <theViewer> is set and <theIsImmediate> is true, selection color will be synchronized
  //! automatically in all views of the viewer.
  void highlightSelected (const Handle(SelectMgr_EntityOwner)& theOwner,
                          const Quantity_NameOfColor theSelColor);

  //! Helper function that unhighlights all owners that are stored in current AIS_Selection.
  //! The function updates global status and selection state of owner and interactive object.
  //! If the parameter <theIsToHilightSubIntensity> is set to true, interactive objects with sub-intensity
  //! switched on in AIS_GlobalStatus will be highlighted with context's sub-intensity color.
  void unhighlightSelected (const Standard_Boolean theIsToHilightSubIntensity = Standard_False);

  AIS_DataMapOfIOStatus myObjects;
  Handle(SelectMgr_SelectionManager) mgrSelector;
  Handle(PrsMgr_PresentationManager3d) myMainPM;
  Handle(V3d_Viewer) myMainVwr;
  Handle(StdSelect_ViewerSelector3d) myMainSel;
  TCollection_AsciiString mySelectionName;
  TCollection_AsciiString myCurrentName;
  Handle(SelectMgr_EntityOwner) myLastPicked;
  Handle(SelectMgr_EntityOwner) myLastinMain;
  Standard_Boolean myWasLastMain;
  Standard_Boolean myCurrentTouched;
  Standard_Boolean mySelectedTouched;
  Standard_Boolean myToHilightSelected;
  Handle(SelectMgr_OrFilter) myFilters;
  Handle(Prs3d_Drawer) myDefaultDrawer;
  Quantity_NameOfColor myDefaultColor;
  Quantity_NameOfColor myHilightColor;
  Quantity_NameOfColor mySelectionColor;
  Quantity_NameOfColor myPreselectionColor;
  Quantity_NameOfColor mySubIntensity;
  Standard_Integer myDisplayMode;
  AIS_DataMapOfILC myLocalContexts;
  Standard_Integer myCurLocalIndex;
  Handle(V3d_View) mylastmoveview;
  AIS_SequenceOfInteractive myAISDetectedSeq;
  Standard_Integer myAISCurDetected;
  Standard_Boolean myZDetectionFlag;
  Standard_Boolean myIsAutoActivateSelMode;


};


#include <AIS_InteractiveContext.lxx>





#endif // _AIS_InteractiveContext_HeaderFile
