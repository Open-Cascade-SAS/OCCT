// Created on: 1996-12-11
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

#ifndef _AIS_InteractiveObject_HeaderFile
#define _AIS_InteractiveObject_HeaderFile

#include <Aspect_TypeOfFacingModel.hxx>
#include <AIS_KindOfInteractive.hxx>
#include <Graphic3d_NameOfMaterial.hxx>
#include <PrsMgr_TypeOfPresentation3d.hxx>
#include <SelectMgr_SelectableObject.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_ListOfTransient.hxx>
#include <Quantity_Color.hxx>

class AIS_InteractiveContext;
class Graphic3d_MaterialAspect;
class Prs3d_Presentation;
class Prs3d_BasicAspect;
class Bnd_Box;

//! Defines a class of objects with display and selection services.
//! Entities which are visualized and selected are
//! Interactive Objects. You can make use of classes of
//! standard Interactive Objects for which all necessary
//! methods have already been programmed, or you can
//! implement your own classes of Interactive Objects.
//! Specific attributes of entities such as arrow aspect for
//! dimensions must be loaded in a Drawer. This Drawer
//! is then applied to the Interactive Object in view.
//! There are four types of Interactive Object in AIS: the
//! construction element or Datum, the Relation, which
//! includes both dimensions and constraints, the Object,
//! and finally, when the object is of an unknown type, the None type.
//! Inside these categories, a signature, or index,
//! provides the possibility of additional characterization.
//! By default, the Interactive Object has a None type
//! and a signature of 0. If you want to give a particular
//! type and signature to your interactive object, you must
//! redefine the methods, Signature and Type.
//! Warning
//! In the case of attribute methods, methods for
//! standard attributes are virtual. They must be
//! redefined   by the inheriting classes. Setcolor for a
//! point and Setcolor for a plane, for example, do not
//! affect the same attributes in the Drawer.
class AIS_InteractiveObject : public SelectMgr_SelectableObject
{
  friend class AIS_InteractiveContext;
  DEFINE_STANDARD_RTTIEXT(AIS_InteractiveObject, SelectMgr_SelectableObject)
public:

  //! Returns the kind of Interactive Object:
  //! -   None
  //! -   Datum
  //! -   Relation
  //! -   Object
  //! By default, the   interactive object has a None type.
  //! Because specific shapes entail different behavior
  //! according to their sub-shapes, you may need to
  //! create a Local Context. This will allow you to
  //! specify the additional characteristics which you
  //! need to handle these shapes.
  Standard_EXPORT virtual AIS_KindOfInteractive Type() const;
  
  //! Specifies additional characteristics of Interactive
  //! Objects. A signature is, in fact, an index with integer
  //! values assigned different properties.
  //! This method is frequently used in conjuction with
  //! Type to give a particular type and signature to an
  //! Interactive Object. By default, the Interactive Object
  //! has a None type and a signature of 0. Among the
  //! datums, this signature is attributed to the shape
  //! The remaining datums have the following default signatures:
  //! -   Point                   signature 1
  //! -   Axis                     signature 2
  //! -   Trihedron                signature 3
  //! -   PlaneTrihedron            signature 4
  //! -   Line                     signature 5
  //! -   Circle                  signature 6
  //! -   Plane                   signature 7.
  Standard_EXPORT virtual Standard_Integer Signature() const;
  
  //! Informs the graphic context that the interactive Object
  //! may be decomposed into sub-shapes for dynamic selection.
  //! The most used Interactive Object is AIS_Shape.
  //! Activation methods for standard selection modes are
  //! proposed in the Interactive Context. These include
  //! selection by vertex or by edges. For datums with the
  //! same behavior as AIS_Shape, such as vetices and
  //! edges, we must redefine the virtual method so that
  //! AcceptShapeDecomposition returns false.
  //! Rule for selection :
  //! Mode 0 :  Selection of  the interactive Object itself
  //! Mode 1 :  Selection of vertices
  //! Mode 2 :  Selection Of Edges
  //! Mode 3 :  Selection Of Wires
  //! Mode 4 :  Selection Of Faces ...
  virtual Standard_Boolean AcceptShapeDecomposition() const { return Standard_False; }

  //! change the current facing model apply on polygons for
  //! SetColor(), SetTransparency(), SetMaterial() methods
  //! default facing model is Aspect_TOFM_TWO_SIDE. This mean that attributes is
  //! applying both on the front and back face.
  Standard_EXPORT void SetCurrentFacingModel (const Aspect_TypeOfFacingModel aModel = Aspect_TOFM_BOTH_SIDE);
  
  //! Returns the current facing model which is in effect.
  Standard_EXPORT Aspect_TypeOfFacingModel CurrentFacingModel() const;

  //! Only the interactive object knowns which Drawer attribute is affected by the color, if any
  //! (ex: for a wire,it's the wireaspect field of the drawer, but for a vertex, only the point aspect field is affected by the color).
  //! WARNING : Do not forget to set the corresponding fields here (hasOwnColor and myDrawer->SetColor())
  Standard_EXPORT virtual void SetColor (const Quantity_Color& theColor);

  //! Removes color settings. Only the Interactive Object
  //! knows which Drawer attribute is   affected by the color
  //! setting. For a wire, for example, wire aspect is the
  //! attribute affected. For a vertex, however, only point
  //! aspect is affected by the color setting.
  Standard_EXPORT virtual void UnsetColor();
  
  //! Allows you to provide the setting aValue for width.
  //! Only the Interactive Object knows which Drawer
  //! attribute is affected by the width setting.
  Standard_EXPORT virtual void SetWidth (const Standard_Real aValue);
  
  Standard_EXPORT virtual void UnsetWidth();
  
  //! Returns true if the class of objects accepts the display mode aMode.
  //! The interactive context can have a default mode of
  //! representation for the set of Interactive Objects. This
  //! mode may not be accepted by a given class of
  //! objects. Consequently, this virtual method allowing us
  //! to get information about the class in question must be implemented.
  Standard_EXPORT virtual Standard_Boolean AcceptDisplayMode (const Standard_Integer aMode) const;
  
  //! Returns the default display mode. This method is to
  //! be implemented when the main mode is not mode 0.
  Standard_EXPORT virtual Standard_Integer DefaultDisplayMode() const;
  
  //! Updates the active presentation; if <AllModes> = Standard_True
  //! all the presentations inside are recomputed.
  //! IMPORTANT: It is preferable to call Redisplay method of
  //! corresponding AIS_InteractiveContext instance for cases when it
  //! is accessible. This method just redirects call to myCTXPtr,
  //! so this class field must be up to date for proper result.
  Standard_EXPORT void Redisplay (const Standard_Boolean AllModes = Standard_False);
  
  //! Sets the infinite state flag aFlage.
  //! if   <aFlag>   = True  ,  the   interactiveObject  is
  //! considered as infinite, i.e. its graphic presentations
  //! are not taken in account for View FitAll...
  Standard_EXPORT void SetInfiniteState (const Standard_Boolean aFlag = Standard_True);

  //! Returns true if the interactive object is infinite. In this
  //! case, its graphic presentations are not taken into
  //! account in the fit-all view.
  Standard_Boolean IsInfinite() const { return myInfiniteState; }

  //! Indicates whether the Interactive Object has a pointer to an interactive context.
  Standard_Boolean HasInteractiveContext() const { return myCTXPtr != NULL; }

  //! Returns the context pointer to the interactive context.
  Standard_EXPORT Handle(AIS_InteractiveContext) GetContext() const;
  
  //! Sets the interactive context aCtx and provides a link
  //! to the default drawing tool or "Drawer" if there is none.
  Standard_EXPORT virtual void SetContext (const Handle(AIS_InteractiveContext)& aCtx);
  
  //! Returns true if the object has an owner attributed to it.
  //! The owner can be a shape for a set of sub-shapes or
  //! a sub-shape for sub-shapes which it is composed of,
  //! and takes the form of a transient.
  Standard_EXPORT Standard_Boolean HasOwner() const;
  
  //! Returns the owner of the Interactive Object.
  //! The owner can be a shape for a set of sub-shapes or
  //! a sub-shape for sub-shapes which it is composed of,
  //! and takes the form of a transient.
  //! There are two types of owners:
  //! -   Direct owners, decomposition shapes such as
  //! edges, wires, and faces.
  //! -   Users, presentable objects connecting to sensitive
  //! primitives, or a shape which has been decomposed.
  const Handle(Standard_Transient)& GetOwner() const { return myOwner; }

  //! Allows you to attribute the owner theApplicativeEntity to
  //! an Interactive Object. This can be a shape for a set of
  //! sub-shapes or a sub-shape for sub-shapes which it
  //! is composed of. The owner takes the form of a transient.
  void SetOwner (const Handle(Standard_Transient)& theApplicativeEntity) { myOwner = theApplicativeEntity; }

  //! Each Interactive Object has methods which allow us
  //! to attribute an Owner to it in the form of a Transient.
  //! This method removes the owner from the graphic entity.
  Standard_EXPORT void ClearOwner();

  //! Returns true if the Interactive Object has a display
  //! mode setting. Otherwise, it is displayed in Neutral Point.
  Standard_Boolean HasDisplayMode() const { return myDrawer->DisplayMode() != -1; }

  //! Sets the display mode aMode for the interactive object.
  //! An object can have its own temporary display mode,
  //! which is different from that proposed by the interactive context.
  //! The range of possibilities currently proposed is the following:
  //! -   AIS_WireFrame
  //! -   AIS_Shaded
  //! This range can, however, be extended through the creation of new display modes.
  Standard_EXPORT void SetDisplayMode (const Standard_Integer aMode);

  //! Removes display mode settings from the interactive object.
  void UnsetDisplayMode() { myDrawer->SetDisplayMode (-1); }

  //! Returns the display mode setting of the Interactive Object.
  //! The range of possibilities is the following:
  //! -   AIS_WireFrame
  //! -   AIS_Shaded
  //! This range can, however, be extended through the
  //! creation of new display modes.
  Standard_Integer DisplayMode() const { return myDrawer->DisplayMode(); }

  //! Returns true if the Interactive Object is in highlight mode.
  Standard_Boolean HasHilightMode() const { return !myHilightDrawer.IsNull() && myHilightDrawer->DisplayMode() != -1; }

  //! Returns highlight display mode.
  //! This is obsolete method for backward compatibility - use ::HilightAttributes() and ::DynamicHilightAttributes() instead.
  Standard_Integer HilightMode() const { return !myHilightDrawer.IsNull() ? myHilightDrawer->DisplayMode() : -1; }

  //! Sets highlight display mode.
  //! This is obsolete method for backward compatibility - use ::HilightAttributes() and ::DynamicHilightAttributes() instead.
  void SetHilightMode (const Standard_Integer theMode)
  {
    if (myHilightDrawer.IsNull())
    {
      myHilightDrawer = new Prs3d_Drawer();
      myHilightDrawer->Link (myDrawer);
      myHilightDrawer->SetAutoTriangulation (Standard_False);
      myHilightDrawer->SetColor (Quantity_NOC_GRAY80);
      myHilightDrawer->SetZLayer(Graphic3d_ZLayerId_UNKNOWN);
    }
    if (myDynHilightDrawer.IsNull())
    {
      myDynHilightDrawer = new Prs3d_Drawer();
      myDynHilightDrawer->Link (myDrawer);
      myDynHilightDrawer->SetColor (Quantity_NOC_CYAN1);
      myDynHilightDrawer->SetAutoTriangulation (Standard_False);
      myDynHilightDrawer->SetZLayer(Graphic3d_ZLayerId_Top);
    }
    myHilightDrawer   ->SetDisplayMode (theMode);
    myDynHilightDrawer->SetDisplayMode (theMode);
  }

  //! Unsets highlight display mode.
  void UnsetHilightMode()
  {
    if (!myHilightDrawer.IsNull())
    {
      myHilightDrawer->SetDisplayMode (-1);
    }
    if (!myDynHilightDrawer.IsNull())
    {
      myDynHilightDrawer->SetDisplayMode (-1);
    }
  }

  //! Returns true if the Interactive Object has color.
  Standard_Boolean HasColor() const { return hasOwnColor; }

  //! Returns the color setting of the Interactive Object.
  virtual void Color (Quantity_Color& theColor) const { theColor = myDrawer->Color(); }

  //! Returns true if the Interactive Object has width.
  Standard_Boolean HasWidth() const { return myOwnWidth != 0.0; }

  //! Returns the width setting of the Interactive Object.
  Standard_Real Width() const { return myOwnWidth; }

  //! Returns true if the Interactive Object has a setting for material.
  Standard_Boolean HasMaterial() const { return hasOwnMaterial; }

  //! Returns the current material setting.
  //! This will be on of the following materials:
  //! -   Brass
  //! -   Bronze
  //! -   Gold
  //! -   Pewter
  //! -   Silver
  //! -   Stone.
  Standard_EXPORT virtual Graphic3d_NameOfMaterial Material() const;

  //! Sets the material aMat defining this display attribute
  //! for the interactive object.
  //! Material aspect determines shading aspect, color and
  //! transparency of visible entities.
  Standard_EXPORT virtual void SetMaterial (const Graphic3d_MaterialAspect& aName);
  
  //! Removes the setting for material.
  Standard_EXPORT virtual void UnsetMaterial();
  
  //! Attributes a setting aValue for transparency.
  //! The transparency value should be between 0.0 and 1.0.
  //! At 0.0 an object will be totally opaque, and at 1.0, fully transparent.
  //! Warning At a value of 1.0, there may be nothing visible.
  Standard_EXPORT virtual void SetTransparency (const Standard_Real aValue = 0.6);
  
  //! Returns true if there is a transparency setting.
  Standard_Boolean IsTransparent() const { return myDrawer->Transparency() > 0.005f; }

  //! Returns the transparency setting.
  //! This will be between 0.0 and 1.0.
  //! At 0.0 an object will be totally opaque, and at 1.0, fully transparent.
  Standard_EXPORT virtual Standard_Real Transparency() const;
  
  //! Removes the transparency setting. The object is opaque by default.
  Standard_EXPORT virtual void UnsetTransparency();
  
  //! Clears settings provided by the drawing tool aDrawer.
  Standard_EXPORT virtual void UnsetAttributes() Standard_OVERRIDE;

  //! Returns TRUE when this object has a presentation
  //! in the current DisplayMode()
  Standard_EXPORT Standard_Boolean HasPresentation() const;
  
  //! Returns the current presentation of this object
  //! according to the current DisplayMode()
  Standard_EXPORT Handle(Prs3d_Presentation) Presentation() const;

  //! Sets the graphic basic aspect to the current presentation.
  Standard_EXPORT void SetAspect (const Handle(Prs3d_BasicAspect)& anAspect);
  
  //! Sets up polygon offsets for this object.
  //! It modifies all existing presentations of <anObj> (if any),
  //! so it is reasonable to call this method after <anObj> has been displayed.
  //! Otherwise, Compute() method should pass Graphic3d_AspectFillArea3d
  //! aspect from <myDrawer> to Graphic3d_Group to make polygon offsets work.
  //!
  //! <aMode> parameter can contain various combinations of
  //! Aspect_PolygonOffsetMode enumeration elements (Aspect_POM_None means
  //! that polygon offsets are not changed).
  //! If <aMode> is different from Aspect_POM_Off and Aspect_POM_None, then <aFactor> and <aUnits>
  //! arguments are used by graphic renderer to calculate a depth offset value:
  //!
  //! offset = <aFactor> * m + <aUnits> * r, where
  //! m - maximum depth slope for the polygon currently being displayed,
  //! r - minimum window coordinates depth resolution (implementation-specific).
  //!
  //! Deafult settings for OCC 3D viewer: mode = Aspect_POM_Fill, factor = 1., units = 0.
  //!
  //! Negative offset values move polygons closer to the viewport,
  //! while positive values shift polygons away.
  //! Consult OpenGL reference for details (glPolygonOffset function description).
  //!
  //! NOTE: This method has a side effect - it creates own shading aspect
  //! if not yet created, so it is better to set up object material,
  //! color, etc. first.
  Standard_EXPORT virtual void SetPolygonOffsets (const Standard_Integer aMode, const Standard_ShortReal aFactor = 1.0, const Standard_ShortReal aUnits = 0.0);
  
  //! Returns Standard_True if <myDrawer> has non-null shading aspect
  Standard_EXPORT virtual Standard_Boolean HasPolygonOffsets() const;
  
  //! Retrieves current polygon offsets settings from <myDrawer>.
  Standard_EXPORT virtual void PolygonOffsets (Standard_Integer& aMode, Standard_ShortReal& aFactor, Standard_ShortReal& aUnits) const;
  
  //! Returns bounding box of object correspondingly to its current display mode.
  Standard_EXPORT virtual void BoundingBox (Bnd_Box& theBndBox) Standard_OVERRIDE;

  //! Enables or disables on-triangulation build of isolines according to the flag given.
  Standard_EXPORT void SetIsoOnTriangulation (const Standard_Boolean theIsEnabled);

  //! Synchronize presentation aspects after their modification.
  //!
  //! This method should be called after modifying primitive aspect properties (material, texture, shader)
  //! so that modifications will take effect on already computed presentation groups (thus avoiding re-displaying the object).
  Standard_EXPORT void SynchronizeAspects();

private:

  Standard_EXPORT virtual Standard_Boolean RecomputeEveryPrs() const;

  Standard_EXPORT void MustRecomputePrs (const Standard_Integer aMode) const;

  Standard_EXPORT const TColStd_ListOfInteger& ListOfRecomputeModes() const;

  Standard_EXPORT void SetRecomputeOk();

protected:

  //! The TypeOfPresention3d means that the interactive object
  //! may have a presentation dependant of the view of Display.
  Standard_EXPORT AIS_InteractiveObject(const PrsMgr_TypeOfPresentation3d aTypeOfPresentation3d = PrsMgr_TOP_AllView);

private:

  AIS_InteractiveContext* myCTXPtr;
  Handle(Standard_Transient) myOwner;

protected:

  TColStd_ListOfInteger myToRecomputeModes;
  Standard_Real myOwnWidth;
  Aspect_TypeOfFacingModel myCurrentFacingModel;
  Standard_Boolean myInfiniteState;
  Standard_Boolean hasOwnColor;
  Standard_Boolean hasOwnMaterial;
  Standard_Boolean myRecomputeEveryPrs;

};

DEFINE_STANDARD_HANDLE(AIS_InteractiveObject, SelectMgr_SelectableObject)

#endif // _AIS_InteractiveObject_HeaderFile
