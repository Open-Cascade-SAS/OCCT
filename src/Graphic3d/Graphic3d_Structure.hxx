// Created on: 1991-06-12
// Created by: NW,JPB,CAL
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Graphic3d_Structure_HeaderFile
#define _Graphic3d_Structure_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Graphic3d_CStructure.hxx>
#include <Graphic3d_IndexedMapOfAddress.hxx>
#include <Standard_Address.hxx>
#include <Graphic3d_TypeOfStructure.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Graphic3d_ZLayerId.hxx>
#include <Graphic3d_SequenceOfHClipPlane.hxx>
#include <Standard_Real.hxx>
#include <Graphic3d_SequenceOfGroup.hxx>
#include <Graphic3d_TypeOfConnection.hxx>
#include <Graphic3d_MapOfStructure.hxx>
#include <Graphic3d_TypeOfComposition.hxx>
#include <Graphic3d_TransformPers.hxx>
#include <Graphic3d_TransModeFlags.hxx>
#include <Graphic3d_BndBox4f.hxx>
#include <Graphic3d_BndBox4d.hxx>
#include <Graphic3d_Vertex.hxx>
class Graphic3d_PriorityDefinitionError;
class Graphic3d_StructureDefinitionError;
class Graphic3d_TransformError;
class Graphic3d_Group;
class Graphic3d_StructureManager;
class Graphic3d_DataStructureManager;
class Bnd_Box;
class gp_Pnt;
class Graphic3d_Vector;


class Graphic3d_Structure;
DEFINE_STANDARD_HANDLE(Graphic3d_Structure, Standard_Transient)

//! This class allows the definition a graphic object.
//! This graphic structure can be displayed,
//! erased, or highlighted.
//! This graphic structure can be connected with
//! another graphic structure.
//! Keywords: Structure, StructureManager, Display, Erase, Highlight,
//! UnHighlight, Visible, Priority, Selectable, Visible,
//! Visual, Connection, Ancestors, Descendants, Transformation
class Graphic3d_Structure : public Standard_Transient
{

public:

  
  //! Creates a graphic object in the manager theManager.
  //! It will appear in all the views of the visualiser.
  //! The structure is not displayed when it is created.
  Standard_EXPORT Graphic3d_Structure(const Handle(Graphic3d_StructureManager)& theManager);
  
  //! Creates a shadow link to existing graphic object.
  Standard_EXPORT Graphic3d_Structure(const Handle(Graphic3d_StructureManager)& theManager, const Handle(Graphic3d_Structure)& thePrs);
  
  //! if WithDestruction == Standard_True then
  //! suppress all the groups of primitives in the structure.
  //! and it is mandatory to create a new group in <me>.
  //! if WithDestruction == Standard_False then
  //! clears all the groups of primitives in the structure.
  //! and all the groups are conserved and empty.
  //! They will be erased at the next screen update.
  //! The structure itself is conserved.
  //! The transformation and the attributes of <me> are conserved.
  //! The childs of <me> are conserved.
  Standard_EXPORT virtual void Clear (const Standard_Boolean WithDestruction = Standard_True);
  
  //! Suppresses the structure <me>.
  //! It will be erased at the next screen update.
  Standard_EXPORT ~Graphic3d_Structure();
  
  //! Displays the structure <me> in all the views of the visualiser.
  Standard_EXPORT virtual void Display();
  
  //! Returns the current display priority for the
  //! structure <me>.
  Standard_EXPORT Standard_Integer DisplayPriority() const;
  
  //! Erases the structure <me> in all the views
  //! of the visualiser.
  Standard_EXPORT virtual void Erase();
  
  //! Highlights the structure in all the views with the given style
  //! @param theStyle [in] the style (type of highlighting: box/color, color and opacity)
  //! @param theToUpdateMgr [in] defines whether related computed structures will be
  //! highlighted via structure manager or not
  Standard_EXPORT void Highlight (const Handle(Graphic3d_PresentationAttributes)& theStyle, const Standard_Boolean theToUpdateMgr = Standard_True);
  
  //! Suppress the structure <me>.
  //! It will be erased at the next screen update.
  //! Warning: No more graphic operations in <me> after this call.
  //! Category: Methods to modify the class definition
  Standard_EXPORT void Remove();
  
  //! Computes axis-aligned bounding box of a structure.
  Standard_EXPORT virtual void CalculateBoundBox();
  
  //! If <theToSet> is Standard_True then <me> is infinite and
  //! the MinMaxValues method method return :
  //! theXMin = theYMin = theZMin = RealFirst().
  //! theXMax = theYMax = theZMax = RealLast().
  //! By default, <me> is not infinite but empty.
  Standard_EXPORT void SetInfiniteState (const Standard_Boolean theToSet);
  
  //! Modifies the order of displaying the structure.
  //! Values are between 0 and 10.
  //! Structures are drawn according to their display priorities
  //! in ascending order.
  //! A structure of priority 10 is displayed the last and appears over the others.
  //! The default value is 5.
  //! Category: Methods to modify the class definition
  //! Warning: If <me> is displayed then the SetDisplayPriority
  //! method erase <me> and display <me> with the
  //! new priority.
  //! Raises PriorityDefinitionError if <Priority> is
  //! greater than 10 or a negative value.
  Standard_EXPORT void SetDisplayPriority (const Standard_Integer Priority);
  
  //! Reset the current priority of the structure to the
  //! previous priority.
  //! Category: Methods to modify the class definition
  //! Warning: If <me> is displayed then the SetDisplayPriority
  //! method erase <me> and display <me> with the
  //! previous priority.
  Standard_EXPORT void ResetDisplayPriority();
  
  //! Set Z layer ID for the structure. The Z layer mechanism
  //! allows to display structures presented in higher layers in overlay
  //! of structures in lower layers by switching off z buffer depth
  //! test between layers
  Standard_EXPORT void SetZLayer (const Graphic3d_ZLayerId theLayerId);
  
  //! Get Z layer ID of displayed structure. The method
  //! returns -1 if the structure has no ID (deleted from graphic driver).
  Standard_EXPORT Graphic3d_ZLayerId GetZLayer() const;
  
  //! Changes a sequence of clip planes slicing the structure on rendering.
  //! @param thePlanes [in] the set of clip planes.
  Standard_EXPORT void SetClipPlanes (const Handle(Graphic3d_SequenceOfHClipPlane)& thePlanes);
  
  //! Get clip planes slicing the structure on rendering.
  //! @return set of clip planes.
  Standard_EXPORT const Handle(Graphic3d_SequenceOfHClipPlane)& ClipPlanes() const;

  //! Modifies the visibility indicator to Standard_True or
  //! Standard_False for the structure <me>.
  //! The default value at the definition of <me> is
  //! Standard_True.
  Standard_EXPORT void SetVisible (const Standard_Boolean AValue);
  
  //! Modifies the visualisation mode for the structure <me>.
  Standard_EXPORT virtual void SetVisual (const Graphic3d_TypeOfStructure AVisual);
  
  //! Modifies the minimum and maximum zoom coefficients
  //! for the structure <me>.
  //! The default value at the definition of <me> is unlimited.
  //! Category: Methods to modify the class definition
  //! Warning: Raises StructureDefinitionError if <LimitInf> is
  //! greater than <LimitSup> or if <LimitInf> or
  //! <LimitSup> is a negative value.
  Standard_EXPORT void SetZoomLimit (const Standard_Real LimitInf, const Standard_Real LimitSup);
  
  //! marks the structure <me> representing wired structure needed for
  //! highlight only so it won't be added to BVH tree.
  //! Category: Methods to modify the class definition
  Standard_EXPORT void SetIsForHighlight (const Standard_Boolean isForHighlight);
  
  //! Suppresses the highlight for the structure <me>
  //! in all the views of the visualiser.
  Standard_EXPORT void UnHighlight();
  
  Standard_EXPORT virtual void Compute();
  
  //! Returns the new Structure defined for the new visualization
  Standard_EXPORT virtual Handle(Graphic3d_Structure) Compute (const Handle(Graphic3d_DataStructureManager)& aProjector);
  
  //! Returns the new Structure defined for the new visualization
  Standard_EXPORT virtual Handle(Graphic3d_Structure) Compute (const Handle(Graphic3d_DataStructureManager)& theProjector,
                                                               const Handle(Geom_Transformation)& theTrsf);
  
  //! Returns the new Structure defined for the new visualization
  Standard_EXPORT virtual void Compute (const Handle(Graphic3d_DataStructureManager)& aProjector, Handle(Graphic3d_Structure)& aStructure);
  
  //! Returns the new Structure defined for the new visualization
  Standard_EXPORT virtual void Compute (const Handle(Graphic3d_DataStructureManager)& theProjector,
                                        const Handle(Geom_Transformation)& theTrsf,
                                        Handle(Graphic3d_Structure)& theStructure);

  //! Forces a new construction of the structure <me>
  //! if <me> is displayed and TOS_COMPUTED.
  Standard_EXPORT void ReCompute();
  
  //! Forces a new construction of the structure <me>
  //! if <me> is displayed in <aProjetor> and TOS_COMPUTED.
  Standard_EXPORT void ReCompute (const Handle(Graphic3d_DataStructureManager)& aProjector);
  
  //! Returns Standard_True if the structure <me> contains
  //! Polygons, Triangles or Quadrangles.
  Standard_EXPORT Standard_Boolean ContainsFacet() const;

  //! Returns the groups sequence included in the structure <me> (internal storage).
  Standard_EXPORT const Graphic3d_SequenceOfGroup& Groups() const;
  
  //! Returns the current number of groups in the
  //! structure <me>.
  Standard_EXPORT Standard_Integer NumberOfGroups() const;
  
  //! Append new group to this structure.
  Standard_EXPORT Handle(Graphic3d_Group) NewGroup();
  
  //! Returns the highlight color for the Highlight method
  //! with the highlight method TOHM_COLOR or TOHM_BOUNDBOX.
  Standard_EXPORT const Handle(Graphic3d_PresentationAttributes)& HighlightStyle() const;
  
  //! Returns Standard_True if the structure <me> is deleted.
  //! <me> is deleted after the call Remove (me).
  Standard_EXPORT Standard_Boolean IsDeleted() const;
  
  //! Returns the display indicator for the structure <me>.
  Standard_EXPORT virtual Standard_Boolean IsDisplayed() const;
  
  //! Returns Standard_True if the structure <me> is empty.
  //! Warning: A structure is empty if :
  //! it do not have group or all the groups are empties
  //! and it do not have descendant or all the descendants
  //! are empties.
  Standard_EXPORT Standard_Boolean IsEmpty() const;
  
  //! Returns Standard_True if the structure <me> is infinite.
  Standard_EXPORT Standard_Boolean IsInfinite() const;
  
  //! Returns the highlight indicator for the structure <me>.
  Standard_EXPORT virtual Standard_Boolean IsHighlighted() const;
  
  //! Returns Standard_True if the structure <me> is transformed.
  //! <=> The transformation != Identity.
  Standard_EXPORT Standard_Boolean IsTransformed() const;
  
  //! Returns the visibility indicator for the structure <me>.
  Standard_EXPORT Standard_Boolean IsVisible() const;

  //! Returns the coordinates of the boundary box of the structure <me>.
  //! If <theToIgnoreInfiniteFlag> is TRUE, the method returns actual graphical
  //! boundaries of the Graphic3d_Group components. Otherwise, the
  //! method returns boundaries taking into account infinite state
  //! of the structure. This approach generally used for application
  //! specific fit operation (e.g. fitting the model into screen,
  //! not taking into accout infinite helper elements).
  //! Warning: If the structure <me> is empty then the empty box is returned,
  //! If the structure <me> is infinite then the whole box is returned.
  Standard_EXPORT Bnd_Box MinMaxValues (const Standard_Boolean theToIgnoreInfiniteFlag = Standard_False) const;

  //! Returns the visualisation mode for the structure <me>.
  Standard_EXPORT Graphic3d_TypeOfStructure Visual() const;
  
  //! Returns Standard_True if the connection is possible between
  //! <AStructure1> and <AStructure2> without a creation
  //! of a cycle.
  //!
  //! It's not possible to call the method
  //! AStructure1->Connect (AStructure2, TypeOfConnection)
  //! if
  //! - the set of all ancestors of <AStructure1> contains
  //! <AStructure1> and if the
  //! TypeOfConnection == TOC_DESCENDANT
  //! - the set of all descendants of <AStructure1> contains
  //! <AStructure2> and if the
  //! TypeOfConnection == TOC_ANCESTOR
  Standard_EXPORT static Standard_Boolean AcceptConnection (const Handle(Graphic3d_Structure)& AStructure1, const Handle(Graphic3d_Structure)& AStructure2, const Graphic3d_TypeOfConnection AType);
  
  //! Returns the group of structures to which <me> is connected.
  Standard_EXPORT void Ancestors (Graphic3d_MapOfStructure& SG) const;
  
  //! If Atype is TOC_DESCENDANT then add <AStructure>
  //! as a child structure of  <me>.
  //! If Atype is TOC_ANCESTOR then add <AStructure>
  //! as a parent structure of <me>.
  //! The connection propagates Display, Highlight, Erase,
  //! Remove, and stacks the transformations.
  //! No connection if the graph of the structures
  //! contains a cycle and <WithCheck> is Standard_True;
  Standard_EXPORT void Connect (const Handle(Graphic3d_Structure)& AStructure, const Graphic3d_TypeOfConnection AType, const Standard_Boolean WithCheck = Standard_False);
  
  //! Returns the group of structures connected to <me>.
  Standard_EXPORT void Descendants (Graphic3d_MapOfStructure& SG) const;
  
  //! Suppress the connection between <AStructure> and <me>.
  Standard_EXPORT void Disconnect (const Handle(Graphic3d_Structure)& AStructure);
  
  //! If Atype is TOC_DESCENDANT then suppress all
  //! the connections with the child structures of <me>.
  //! If Atype is TOC_ANCESTOR then suppress all
  //! the connections with the parent structures of <me>.
  Standard_EXPORT void DisconnectAll (const Graphic3d_TypeOfConnection AType);
  
  //! Returns <ASet> the group of structures :
  //! - directly or indirectly connected to <AStructure> if the
  //! TypeOfConnection == TOC_DESCENDANT
  //! - to which <AStructure> is directly or indirectly connected
  //! if the TypeOfConnection == TOC_ANCESTOR
  Standard_EXPORT static void Network (const Handle(Graphic3d_Structure)& AStructure, const Graphic3d_TypeOfConnection AType, Graphic3d_MapOfStructure& ASet);
  
  Standard_EXPORT void SetOwner (const Standard_Address Owner);
  
  Standard_EXPORT Standard_Address Owner() const;
  
  Standard_EXPORT void SetHLRValidation (const Standard_Boolean AFlag);
  
  Standard_EXPORT Standard_Boolean HLRValidation() const;

  //! Return local transformation.
  const Handle(Geom_Transformation)& Transformation() const { return myCStructure->Transformation(); }

  //! Modifies the current local transformation
  Standard_EXPORT void SetTransformation (const Handle(Geom_Transformation)& theTrsf);

  Standard_DEPRECATED("This method is deprecated - SetTransformation() should be called instead")
  void Transform (const Handle(Geom_Transformation)& theTrsf) { SetTransformation (theTrsf); }

  //! Modifies the current transform persistence (pan, zoom or rotate)
  Standard_EXPORT void SetTransformPersistence (const Handle(Graphic3d_TransformPers)& theTrsfPers);

  //! @return transform persistence of the presentable object.
  const Handle(Graphic3d_TransformPers)& TransformPersistence() const { return myCStructure->TransformPersistence(); }

  //! Sets if the structure location has mutable nature (content or location will be changed regularly).
  Standard_EXPORT void SetMutable (const Standard_Boolean theIsMutable);
  
  //! Returns true if structure has mutable nature (content or location are be changed regularly).
  //! Mutable structure will be managed in different way than static onces.
  Standard_EXPORT Standard_Boolean IsMutable() const;
  
  Standard_EXPORT Graphic3d_TypeOfStructure ComputeVisual() const;
  
  //! Clears the structure <me>.
  Standard_EXPORT void GraphicClear (const Standard_Boolean WithDestruction);
  
  Standard_EXPORT void GraphicConnect (const Handle(Graphic3d_Structure)& ADaughter);
  
  Standard_EXPORT void GraphicDisconnect (const Handle(Graphic3d_Structure)& ADaughter);

  //! Internal method which sets new transformation without calling graphic manager callbacks.
  Standard_EXPORT void GraphicTransform (const Handle(Geom_Transformation)& theTrsf);

  //! Returns the identification number of the structure <me>.
  Standard_EXPORT Standard_Integer Identification() const;
  
  //! Prints informations about the network associated
  //! with the structure <AStructure>.
  Standard_EXPORT static void PrintNetwork (const Handle(Graphic3d_Structure)& AStructure, const Graphic3d_TypeOfConnection AType);
  
  //! Suppress the adress <APtr> in the list
  //! of descendants or in the list of ancestors.
  Standard_EXPORT void Remove (const Standard_Address APtr, const Graphic3d_TypeOfConnection AType);
  
  Standard_EXPORT void SetComputeVisual (const Graphic3d_TypeOfStructure AVisual);
  
  //! Transforms theX, theY, theZ with the transformation theTrsf.
  Standard_EXPORT static void Transforms (const gp_Trsf& theTrsf,
                                          const Standard_Real theX, const Standard_Real theY, const Standard_Real theZ,
                                          Standard_Real& theNewX, Standard_Real& theNewY, Standard_Real& theNewZ);

  //! Returns the low-level structure
  const Handle(Graphic3d_CStructure)& CStructure() const { return myCStructure; }

friend class Graphic3d_Group;


  DEFINE_STANDARD_RTTIEXT(Graphic3d_Structure,Standard_Transient)

protected:

  //! Transforms boundaries with <theTrsf> transformation.
  Standard_EXPORT static void TransformBoundaries (const gp_Trsf& theTrsf,
                                                   Standard_Real& theXMin, Standard_Real& theYMin, Standard_Real& theZMin,
                                                   Standard_Real& theXMax, Standard_Real& theYMax, Standard_Real& theZMax);

  //! Appends new descendant structure.
  Standard_EXPORT Standard_Boolean AppendDescendant (const Standard_Address theDescendant);
  
  //! Removes the given descendant structure.
  Standard_EXPORT Standard_Boolean RemoveDescendant (const Standard_Address theDescendant);
  
  //! Appends new ancestor structure.
  Standard_EXPORT Standard_Boolean AppendAncestor (const Standard_Address theAncestor);
  
  //! Removes the given ancestor structure.
  Standard_EXPORT Standard_Boolean RemoveAncestor (const Standard_Address theAncestor);

private:

  //! Suppress in the structure <me>, the group theGroup.
  //! It will be erased at the next screen update.
  Standard_EXPORT void Remove (const Handle(Graphic3d_Group)& theGroup);
  
  //! Manages the number of groups in the structure <me>
  //! which contains facet.
  //! Polygons, Triangles or Quadrangles.
  //! <ADelta> = +1 or -1
  Standard_EXPORT void GroupsWithFacet (const Standard_Integer ADelta);
  
  //! Returns the extreme coordinates found in the structure <me> without transformation applied.
  Standard_EXPORT Graphic3d_BndBox4f minMaxCoord() const;
  
  //! Gets untransformed bounding box from structure.
  Standard_EXPORT void getBox (Graphic3d_BndBox3d& theBox, const Standard_Boolean theToIgnoreInfiniteFlag = Standard_False) const;
  
  //! Adds transformed (with myCStructure->Transformation) bounding box of structure to theBox.
  Standard_EXPORT void addTransformed (Graphic3d_BndBox3d& theBox, const Standard_Boolean theToIgnoreInfiniteFlag = Standard_False) const;
  
  //! Returns the manager to which <me> is associated.
  Standard_EXPORT Handle(Graphic3d_StructureManager) StructureManager() const;
  
  //! Calls the Update method of the StructureManager which contains the Structure <me>.
  //! If theUpdateLayer is true then invalidates bounding box of ZLayer.
  Standard_EXPORT void Update (const bool theUpdateLayer = false) const;

protected:

  Graphic3d_StructureManager* myStructureManager;
  Graphic3d_TypeOfStructure   myComputeVisual;

  Handle(Graphic3d_CStructure) myCStructure;
  Graphic3d_IndexedMapOfAddress myAncestors;
  Graphic3d_IndexedMapOfAddress myDescendants;
  Standard_Address myOwner;
  Graphic3d_TypeOfStructure myVisual;

};

#endif // _Graphic3d_Structure_HeaderFile
