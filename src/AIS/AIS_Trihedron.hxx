// Created on: 1995-10-09
// Created by: Arnaud BOUZY/Odile Olivier
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

#ifndef _AIS_Trihedron_HeaderFile
#define _AIS_Trihedron_HeaderFile

#include <AIS_InteractiveObject.hxx>
#include <AIS_KindOfInteractive.hxx>

#include <Graphic3d_ArrayOfTriangles.hxx>

#include <PrsMgr_PresentationManager3d.hxx>

#include <Prs3d_DatumAspect.hxx>
#include <Prs3d_DatumMode.hxx>
#include <Prs3d_DatumParts.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PointAspect.hxx>

#include <Quantity_NameOfColor.hxx>

#include <Standard.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_Type.hxx>
#include <SelectMgr_Selection.hxx>

#include <TColgp_Array1OfPnt.hxx>

class Geom_Axis2Placement;
class AIS_InteractiveObject;
class AIS_Axis;
class AIS_Point;
class AIS_Plane;
class AIS_InteractiveContext;
class Prs3d_Presentation;
class Prs3d_Projector;
class Geom_Transformation;
class gp_Trsf;
class Quantity_Color;


class AIS_Trihedron;
DEFINE_STANDARD_HANDLE(AIS_Trihedron, AIS_InteractiveObject)

//! Create a selectable trihedron
//! The trihedron includes 1 origin, 3 axes and 3 labels.
//! Default text of labels are "X", "Y", "Z".
//! Color of origin and any axis, color of arrows and labels may be changed.
//! Visual presentation might be shown in two, shading and wireframe modes(wireframe by default)
//! There are 4 modes of selection :
//! mode = 0 to select trihedron,  priority = 1
//! mode = 1 to select its origin, priority = 5
//! mode = 2 to select its axis,   priority = 3
//! mode = 3 to select its planes, priority = 2
//! Warning
//! For the presentation of trihedron, the default unit of
//! length is the millimetre, and the default value for the
//! representation of the axes is 100. If you modify these
//! dimensions, you must temporarily recover the Drawer.
//! From inside it, you take the aspect in which the values
//! for length are stocked. For trihedron, this is
//! Prs3d_Drawer_LineAspect. You change the
//! values inside this Aspect and recalculate the presentation.
class AIS_Trihedron : public AIS_InteractiveObject
{
  DEFINE_STANDARD_RTTIEXT(AIS_Trihedron, AIS_InteractiveObject)
public:

  
  //! Initializes a trihedron entity.
  Standard_EXPORT AIS_Trihedron(const Handle(Geom_Axis2Placement)& aComponent);
  
  //! Sets Shading or Wireframe display mode, triangle or segment graphic group is used relatively.
  Standard_EXPORT void SetDatumDisplayMode(const Prs3d_DatumMode& theMode) { myDisplayMode = theMode; }

  //! Returns datum display mode.
  Standard_EXPORT Prs3d_DatumMode DatumDisplayMode() const { return myDisplayMode; }

  //! Returns the right-handed coordinate system set in SetComponent.
  Standard_EXPORT const Handle(Geom_Axis2Placement)& Component() const { return myComponent; }

  //! Constructs the right-handed coordinate system aComponent.
  Standard_EXPORT void SetComponent (const Handle(Geom_Axis2Placement)& theComponent);

  //! Returns true if the trihedron object has a size other
  //! than the default size of 100 mm. along each axis.
  Standard_Boolean HasOwnSize() const;

  //! Sets the size aValue for the trihedron object.
  //! The default value is 100 mm.
  Standard_EXPORT void SetSize (const Standard_Real aValue);
  
  //! Removes any non-default settings for size of this
  //! trihedron object.
  Standard_EXPORT void UnsetSize();
  
  Standard_EXPORT Standard_Real Size() const;
  
  //! Returns true if the display mode selected, aMode, is
  //! valid for trihedron datums.
  Standard_EXPORT Standard_Boolean AcceptDisplayMode (const Standard_Integer aMode) const Standard_OVERRIDE;

  //! Returns index 3, selection of the planes XOY, YOZ, XOZ.
  Standard_EXPORT virtual Standard_Integer Signature() const Standard_OVERRIDE;
  
  //! Indicates that the type of Interactive Object is datum.
  Standard_EXPORT virtual AIS_KindOfInteractive Type() const Standard_OVERRIDE;
  
  //! Sets the color theColor for this trihedron object, it changes color of axes.
  Standard_EXPORT void SetColor (const Quantity_NameOfColor theColor) Standard_OVERRIDE;

  //! Sets the color theColor for this trihedron object, it changes color of axes.
  Standard_EXPORT void SetColor (const Quantity_Color& theColor) Standard_OVERRIDE;
  
  //! Sets color of label of trihedron axes.
  Standard_EXPORT void SetTextColor (const Quantity_NameOfColor theColor);

  //! Sets color of label of trihedron axes.
  Standard_EXPORT void SetTextColor (const Quantity_Color& theColor);

  //! Returns true if trihedron has own text color
  Standard_EXPORT Standard_Boolean HasTextColor() const;
  
  //! Returns trihedron text color
  Standard_EXPORT Quantity_Color TextColor() const;

  //! Sets color of arrow of trihedron axes. Used only in wireframe mode
  Standard_EXPORT void SetArrowColor (const Quantity_NameOfColor theColor);
  
  //! Sets color of arrow of trihedron axes. Used only in wireframe mode
  Standard_EXPORT void SetArrowColor (const Quantity_Color& theColor);

  //! Returns true if trihedron has own arrow color
  Standard_EXPORT Standard_Boolean HasArrowColor() const;
  
  //! Returns trihedron arrow color
  Standard_EXPORT Quantity_Color ArrowColor() const;
  
  //! Removes the settings for color.
  Standard_EXPORT void UnsetColor() Standard_OVERRIDE;
  
  //! Sets color of datum part: origin or some of trihedron axes.
  //! If presentation is shading mode, this color is set for both sides of facing model
  Standard_EXPORT void SetColor (const Prs3d_DatumParts& thePart,
                                 const Quantity_Color& theColor);

  //! Returns color of datum part: origin or some of trihedron axes.
  Standard_EXPORT Quantity_Color Color (const Prs3d_DatumParts& thePart);

  //! Sets color of origin.
  //! Standard_DEPRECATED("This method is deprecated - SetColor() should be called instead")
  Standard_EXPORT void SetOriginColor (const Quantity_Color& theColor);

  //! Sets color of x-axis.
  //! Standard_DEPRECATED("This method is deprecated - SetColor() should be called instead")
  Standard_EXPORT void SetXAxisColor (const Quantity_Color& theColor);

  //! Sets color of y-axis.
  //! Standard_DEPRECATED("This method is deprecated - SetColor() should be called instead")
  Standard_EXPORT void SetYAxisColor (const Quantity_Color& theColor);

  //! Sets color of z-axis.
  //! Standard_DEPRECATED("This method is deprecated - SetColor() should be called instead")
  Standard_EXPORT void SetAxisColor (const Quantity_Color& theColor);

  //! Sets priority of selection for owner of the given type
  Standard_EXPORT void SetSelectionPriority(const Prs3d_DatumParts& thePart, const Standard_Integer thePriority)
  {
    mySelectionPriority.Bind(thePart, thePriority);
  }

  //! Sets priority of selection for owner of the given type
  Standard_EXPORT Standard_Integer SelectionPriority(const Prs3d_DatumParts& thePart)
  {
    return mySelectionPriority.Find(thePart);
  }

  //! Sets text label for trihedron axis. Parameter thePart should be XAxis, YAxis or ZAxis
  Standard_EXPORT void SetLabel(const Prs3d_DatumParts& thePart, const TCollection_ExtendedString& thePriority)
  {
    myLabel.Bind(thePart, thePriority);
  }

  //! Returns text of axis. Parameter thePart should be XAxis, YAxis or ZAxis
  Standard_EXPORT const TCollection_ExtendedString& Label(const Prs3d_DatumParts& thePart)
  {
    return myLabel.Find(thePart);
  }

public:

  //! Disables auto highlighting to use HilightSelected() and HilightOwnerWithColor() overridden methods.
  Standard_EXPORT virtual Standard_Boolean IsAutoHilight() const Standard_OVERRIDE;

  //! Method which clear all selected owners belonging
  //! to this selectable object ( for fast presentation draw ).
  Standard_EXPORT virtual void ClearSelected() Standard_OVERRIDE;

  //! Method which draws selected owners ( for fast presentation draw ).
  Standard_EXPORT virtual void HilightSelected (const Handle(PrsMgr_PresentationManager3d)& thePM,
                                    const SelectMgr_SequenceOfOwner& theOwners) Standard_OVERRIDE;

  //! Method which hilight an owner belonging to
  //! this selectable object  ( for fast presentation draw ).
  Standard_EXPORT virtual void HilightOwnerWithColor (const Handle(PrsMgr_PresentationManager3d)& thePM,
                                                      const Handle(Graphic3d_HighlightStyle)& theStyle,
                                                      const Handle(SelectMgr_EntityOwner)& theOwner);

protected:

  //! Compute trihedron presentation.
  Standard_EXPORT void Compute (const Handle(PrsMgr_PresentationManager3d)& thePrsMgr,
                                const Handle(Prs3d_Presentation)& thePrs,
                                const Standard_Integer theMode) Standard_OVERRIDE;

  //! This compute is unavailable for trihedron presentation.
  void Compute (const Handle(Prs3d_Projector)& , const Handle(Prs3d_Presentation)& ) Standard_OVERRIDE {}
  
  //! This compute is unavailable for trihedron presentation.
  Standard_EXPORT virtual void Compute (const Handle(Prs3d_Projector)& theProjector,
                                        const Handle(Geom_Transformation)& theTrsf,
                                        const Handle(Prs3d_Presentation)& thePrs) Standard_OVERRIDE;

  //! Compute selection.
  Standard_EXPORT void ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                                         const Standard_Integer theMode) Standard_OVERRIDE;

  //! Computes presentation for display mode equal 1.
  Standard_EXPORT void ComputePresentation (const Handle(PrsMgr_PresentationManager3d)& thePrsMgr,
                                            const Handle(Prs3d_Presentation)& thePrs);

  Standard_EXPORT void LoadRecomputable (const Standard_Integer TheMode);

  //! Created a sensitive entity for the datum part that will be used in selection owner creation
  Handle(SelectBasics_SensitiveEntity) CreateSensitiveEntity(const Prs3d_DatumParts& thePart,
                                        const Handle(SelectBasics_EntityOwner)& theOwner) const;

protected:
  Standard_Boolean myHasOwnSize;
  Standard_Boolean myHasOwnTextColor;
  Standard_Boolean myHasOwnArrowColor;

  Prs3d_DatumMode myDisplayMode;
  Handle(Geom_Axis2Placement) myComponent;

  NCollection_DataMap<Prs3d_DatumParts, Standard_Integer> mySelectionPriority;
  NCollection_DataMap<Prs3d_DatumParts, TCollection_ExtendedString> myLabel;

  NCollection_DataMap<Prs3d_DatumParts, Handle(Graphic3d_Group)> myPartToGroup;
  NCollection_List<Prs3d_DatumParts> mySelectedParts;

  Handle(Prs3d_PointAspect) myPointAspect;

  Handle(Prs3d_ShadingAspect) myHighlightAspect; // internal container to set group selected
  Handle(Prs3d_LineAspect) myHighlightLineAspect; // internal container to set group selected
  Handle(Prs3d_PointAspect) myHighlightPointAspect; // internal container to set group selected
};

#endif // _AIS_Trihedron_HeaderFile
