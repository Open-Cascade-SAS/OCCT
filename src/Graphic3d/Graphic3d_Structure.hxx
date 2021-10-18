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

#include <Graphic3d_BndBox4f.hxx>
#include <Graphic3d_BndBox4d.hxx>
#include <Graphic3d_CStructure.hxx>
#include <Graphic3d_MapOfStructure.hxx>
#include <Graphic3d_SequenceOfGroup.hxx>
#include <Graphic3d_SequenceOfHClipPlane.hxx>
#include <Graphic3d_TypeOfConnection.hxx>
#include <Graphic3d_TypeOfStructure.hxx>
#include <Graphic3d_TransformPers.hxx>
#include <Graphic3d_TransModeFlags.hxx>
#include <Graphic3d_ZLayerId.hxx>
#include <NCollection_IndexedMap.hxx>

class Graphic3d_StructureManager;
class Graphic3d_DataStructureManager;
class Bnd_Box;

DEFINE_STANDARD_HANDLE(Graphic3d_Structure, Standard_Transient)

//! This class allows the definition a graphic object.
//! This graphic structure can be displayed, erased, or highlighted.
//! This graphic structure can be connected with another graphic structure.
class Graphic3d_Structure : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_Structure, Standard_Transient)
  friend class Graphic3d_Group;
public:

  //! Creates a graphic object in the manager theManager.
  //! It will appear in all the views of the visualiser.
  //! The structure is not displayed when it is created.
  //! @param theManager structure manager holding this structure
  //! @param theLinkPrs another structure for creating a shadow (linked) structure
  Standard_EXPORT Graphic3d_Structure (const Handle(Graphic3d_StructureManager)& theManager,
                                       const Handle(Graphic3d_Structure)& theLinkPrs = Handle(Graphic3d_Structure)());
  
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
  virtual void Clear (const Standard_Boolean WithDestruction = Standard_True)
  {
    clear (WithDestruction);
  }
  
  //! Suppresses the structure <me>.
  //! It will be erased at the next screen update.
  Standard_EXPORT virtual ~Graphic3d_Structure();
  
  //! Displays the structure <me> in all the views of the visualiser.
  Standard_EXPORT virtual void Display();

  //! Returns the current display priority for this structure.
  Standard_Integer DisplayPriority() const { return myCStructure->Priority; }
  
  //! Erases this structure in all the views of the visualiser.
  virtual void Erase() { erase(); }
  
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
  
  //! Sets infinite flag.
  //! When TRUE, the MinMaxValues method returns:
  //! theXMin = theYMin = theZMin = RealFirst().
  //! theXMax = theYMax = theZMax = RealLast().
  //! By default, structure is created not infinite but empty.
  void SetInfiniteState (const Standard_Boolean theToSet)
  {
    if (!myCStructure.IsNull()) { myCStructure->IsInfinite = theToSet ? 1 : 0; }
  }

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
  
  //! Get Z layer ID of displayed structure.
  //! The method returns -1 if the structure has no ID (deleted from graphic driver).
  Graphic3d_ZLayerId GetZLayer() const { return myCStructure->ZLayer(); }
  
  //! Changes a sequence of clip planes slicing the structure on rendering.
  //! @param thePlanes [in] the set of clip planes.
  void SetClipPlanes (const Handle(Graphic3d_SequenceOfHClipPlane)& thePlanes)
  {
    if (!myCStructure.IsNull()) { myCStructure->SetClipPlanes (thePlanes); }
  }
  
  //! Get clip planes slicing the structure on rendering.
  //! @return set of clip planes.
  const Handle(Graphic3d_SequenceOfHClipPlane)& ClipPlanes() const { return myCStructure->ClipPlanes(); }

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

  //! Marks the structure <me> representing wired structure needed for highlight only so it won't be added to BVH tree.
  void SetIsForHighlight (const Standard_Boolean isForHighlight)
  {
    if (!myCStructure.IsNull()) { myCStructure->IsForHighlight = isForHighlight; }
  }
  
  //! Suppresses the highlight for the structure <me>
  //! in all the views of the visualiser.
  Standard_EXPORT void UnHighlight();
  
  virtual void Compute()
  {
    //
  }

  //! Returns the new Structure defined for the new visualization
  virtual void computeHLR (const Handle(Graphic3d_Camera)& theProjector,
                           Handle(Graphic3d_Structure)& theStructure)
  {
    (void )theProjector;
    (void )theStructure;
  }

  //! Forces a new construction of the structure <me>
  //! if <me> is displayed and TOS_COMPUTED.
  Standard_EXPORT void ReCompute();
  
  //! Forces a new construction of the structure <me>
  //! if <me> is displayed in <aProjetor> and TOS_COMPUTED.
  Standard_EXPORT void ReCompute (const Handle(Graphic3d_DataStructureManager)& aProjector);
  
  //! Returns Standard_True if the structure <me> contains
  //! Polygons, Triangles or Quadrangles.
  Standard_EXPORT Standard_Boolean ContainsFacet() const;

  //! Returns the groups sequence included in this structure.
  const Graphic3d_SequenceOfGroup& Groups() const { return myCStructure->Groups(); }

  //! Returns the current number of groups in this structure.
  Standard_Integer NumberOfGroups() const { return myCStructure->Groups().Length(); }
  
  //! Append new group to this structure.
  Standard_EXPORT Handle(Graphic3d_Group) NewGroup();

  //! Returns the last created group or creates new one if list is empty.
  Handle(Graphic3d_Group) CurrentGroup()
  {
    if (Groups().IsEmpty())
    {
      return NewGroup();
    }
    return Groups().Last();
  }

  //! Returns the highlight attributes.
  const Handle(Graphic3d_PresentationAttributes)& HighlightStyle() const { return myCStructure->HighlightStyle(); }

  //! Returns TRUE if this structure is deleted (after Remove() call).
  Standard_Boolean IsDeleted() const { return myCStructure.IsNull(); }
  
  //! Returns the display indicator for this structure.
  virtual Standard_Boolean IsDisplayed() const
  {
    return !myCStructure.IsNull()
        && myCStructure->stick != 0;
  }
  
  //! Returns Standard_True if the structure <me> is empty.
  //! Warning: A structure is empty if :
  //! it do not have group or all the groups are empties
  //! and it do not have descendant or all the descendants
  //! are empties.
  Standard_EXPORT Standard_Boolean IsEmpty() const;
  
  //! Returns Standard_True if the structure <me> is infinite.
  Standard_Boolean IsInfinite() const
  {
    return IsDeleted()
        || myCStructure->IsInfinite;
  }
  
  //! Returns the highlight indicator for this structure.
  virtual Standard_Boolean IsHighlighted() const
  {
    return !myCStructure.IsNull()
        && myCStructure->highlight != 0;
  }
  
  //! Returns TRUE if the structure is transformed.
  Standard_Boolean IsTransformed() const
  {
    return !myCStructure.IsNull()
        && !myCStructure->Transformation().IsNull()
        && myCStructure->Transformation()->Form() != gp_Identity;
  }
  
  //! Returns the visibility indicator for this structure.
  Standard_Boolean IsVisible() const
  {
    return !myCStructure.IsNull()
        && myCStructure->visible != 0;
  }

  //! Returns the coordinates of the boundary box of the structure <me>.
  //! If <theToIgnoreInfiniteFlag> is TRUE, the method returns actual graphical
  //! boundaries of the Graphic3d_Group components. Otherwise, the
  //! method returns boundaries taking into account infinite state
  //! of the structure. This approach generally used for application
  //! specific fit operation (e.g. fitting the model into screen,
  //! not taking into account infinite helper elements).
  //! Warning: If the structure <me> is empty then the empty box is returned,
  //! If the structure <me> is infinite then the whole box is returned.
  Standard_EXPORT Bnd_Box MinMaxValues (const Standard_Boolean theToIgnoreInfiniteFlag = Standard_False) const;

  //! Returns the visualisation mode for the structure <me>.
  Graphic3d_TypeOfStructure Visual() const { return myVisual; }
  
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
  Standard_EXPORT static Standard_Boolean AcceptConnection (Graphic3d_Structure* theStructure1,
                                                            Graphic3d_Structure* theStructure2,
                                                            Graphic3d_TypeOfConnection theType);
  
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
  Standard_EXPORT void Connect (Graphic3d_Structure* theStructure,
                                Graphic3d_TypeOfConnection theType,
                                Standard_Boolean theWithCheck = Standard_False);

  Standard_DEPRECATED("Deprecated short-cut")
  void Connect (const Handle(Graphic3d_Structure)& thePrs)
  {
    Connect (thePrs.get(), Graphic3d_TOC_DESCENDANT);
  }
  
  //! Returns the group of structures connected to <me>.
  Standard_EXPORT void Descendants (Graphic3d_MapOfStructure& SG) const;
  
  //! Suppress the connection between <AStructure> and <me>.
  Standard_EXPORT void Disconnect (Graphic3d_Structure* theStructure);

  Standard_DEPRECATED("Deprecated alias for Disconnect()")
  void Remove (const Handle(Graphic3d_Structure)& thePrs) { Disconnect (thePrs.get()); }
  
  //! If Atype is TOC_DESCENDANT then suppress all
  //! the connections with the child structures of <me>.
  //! If Atype is TOC_ANCESTOR then suppress all
  //! the connections with the parent structures of <me>.
  Standard_EXPORT void DisconnectAll (const Graphic3d_TypeOfConnection AType);

  Standard_DEPRECATED("Deprecated alias for DisconnectAll()")
  void RemoveAll() { DisconnectAll (Graphic3d_TOC_DESCENDANT); }
  
  //! Returns <ASet> the group of structures :
  //! - directly or indirectly connected to <AStructure> if the
  //! TypeOfConnection == TOC_DESCENDANT
  //! - to which <AStructure> is directly or indirectly connected
  //! if the TypeOfConnection == TOC_ANCESTOR
  Standard_EXPORT static void Network (Graphic3d_Structure* theStructure,
                                       const Graphic3d_TypeOfConnection theType,
                                       NCollection_Map<Graphic3d_Structure*>& theSet);
  
  void SetOwner (const Standard_Address theOwner) { myOwner = theOwner; }
  
  Standard_Address Owner() const { return myOwner; }
  
  void SetHLRValidation (const Standard_Boolean theFlag)
  {
    if (!myCStructure.IsNull()) { myCStructure->HLRValidation = theFlag ? 1 : 0; }
  }

  //! Hidden parts stored in this structure are valid if:
  //! 1) the owner is defined.
  //! 2) they are not invalid.
  Standard_Boolean HLRValidation() const
  {
    return myOwner != NULL
        && myCStructure->HLRValidation != 0;
  }

  //! Return local transformation.
  const Handle(TopLoc_Datum3D)& Transformation() const { return myCStructure->Transformation(); }

  //! Modifies the current local transformation
  Standard_EXPORT void SetTransformation (const Handle(TopLoc_Datum3D)& theTrsf);

  //! Modifies the current transform persistence (pan, zoom or rotate)
  Standard_EXPORT void SetTransformPersistence (const Handle(Graphic3d_TransformPers)& theTrsfPers);

  //! @return transform persistence of the presentable object.
  const Handle(Graphic3d_TransformPers)& TransformPersistence() const { return myCStructure->TransformPersistence(); }

  //! Sets if the structure location has mutable nature (content or location will be changed regularly).
  void SetMutable (const Standard_Boolean theIsMutable)
  {
    if (!myCStructure.IsNull()) { myCStructure->IsMutable = theIsMutable; }
  }

  //! Returns true if structure has mutable nature (content or location are be changed regularly).
  //! Mutable structure will be managed in different way than static onces.
  Standard_Boolean IsMutable() const
  {
    return !myCStructure.IsNull()
        && myCStructure->IsMutable;
  }

  Graphic3d_TypeOfStructure ComputeVisual() const { return myComputeVisual; }

  //! Clears the structure <me>.
  Standard_EXPORT void GraphicClear (const Standard_Boolean WithDestruction);

  void GraphicConnect (const Handle(Graphic3d_Structure)& theDaughter)
  {
    if (!myCStructure.IsNull()) { myCStructure->Connect (*theDaughter->myCStructure); }
  }

  void GraphicDisconnect (const Handle(Graphic3d_Structure)& theDaughter)
  {
    if (!myCStructure.IsNull()) { myCStructure->Disconnect (*theDaughter->myCStructure); }
  }

  //! Internal method which sets new transformation without calling graphic manager callbacks.
  void GraphicTransform (const Handle(TopLoc_Datum3D)& theTrsf)
  {
    if (!myCStructure.IsNull()) { myCStructure->SetTransformation (theTrsf); }
  }

  //! Returns the identification number of this structure.
  Standard_Integer Identification() const { return myCStructure->Id; }
  
  //! Prints information about the network associated
  //! with the structure <AStructure>.
  Standard_EXPORT static void PrintNetwork (const Handle(Graphic3d_Structure)& AStructure, const Graphic3d_TypeOfConnection AType);
  
  //! Suppress the structure in the list of descendants or in the list of ancestors.
  Standard_EXPORT void Remove (Graphic3d_Structure* thePtr,
                               const Graphic3d_TypeOfConnection theType);
  
  void SetComputeVisual (const Graphic3d_TypeOfStructure theVisual)
  {
    // The ComputeVisual is saved only if the structure is declared TOS_ALL, TOS_WIREFRAME or TOS_SHADING.
    // This declaration permits to calculate proper representation of the structure calculated by Compute instead of passage to TOS_COMPUTED.
    if (theVisual != Graphic3d_TOS_COMPUTED)
    {
      myComputeVisual = theVisual;
    }
  }
  
  //! Transforms theX, theY, theZ with the transformation theTrsf.
  Standard_EXPORT static void Transforms (const gp_Trsf& theTrsf,
                                          const Standard_Real theX, const Standard_Real theY, const Standard_Real theZ,
                                          Standard_Real& theNewX, Standard_Real& theNewY, Standard_Real& theNewZ);

  //! Returns the low-level structure
  const Handle(Graphic3d_CStructure)& CStructure() const { return myCStructure; }

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

protected:

  //! Transforms boundaries with <theTrsf> transformation.
  Standard_EXPORT static void TransformBoundaries (const gp_Trsf& theTrsf,
                                                   Standard_Real& theXMin, Standard_Real& theYMin, Standard_Real& theZMin,
                                                   Standard_Real& theXMax, Standard_Real& theYMax, Standard_Real& theZMax);

  //! Appends new descendant structure.
  Standard_EXPORT Standard_Boolean AppendDescendant (Graphic3d_Structure* theDescendant);
  
  //! Removes the given descendant structure.
  Standard_EXPORT Standard_Boolean RemoveDescendant (Graphic3d_Structure* theDescendant);
  
  //! Appends new ancestor structure.
  Standard_EXPORT Standard_Boolean AppendAncestor (Graphic3d_Structure* theAncestor);
  
  //! Removes the given ancestor structure.
  Standard_EXPORT Standard_Boolean RemoveAncestor (Graphic3d_Structure* theAncestor);

  //! Clears all the groups of primitives in the structure.
  Standard_EXPORT void clear (const Standard_Boolean WithDestruction);

  //! Erases this structure in all the views of the visualiser.
  Standard_EXPORT void erase();

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

  Graphic3d_StructureManager*   myStructureManager;
  Handle(Graphic3d_CStructure)  myCStructure;
  NCollection_IndexedMap<Graphic3d_Structure*> myAncestors;
  NCollection_IndexedMap<Graphic3d_Structure*> myDescendants;
  Standard_Address              myOwner;
  Graphic3d_TypeOfStructure     myVisual;
  Graphic3d_TypeOfStructure     myComputeVisual;

};

#endif // _Graphic3d_Structure_HeaderFile
