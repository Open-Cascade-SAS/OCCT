// Created on: 1991-09-05
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

#ifndef _Graphic3d_StructureManager_HeaderFile
#define _Graphic3d_StructureManager_HeaderFile

#include <Aspect_GenId.hxx>
#include <Graphic3d_CView.hxx>
#include <Graphic3d_ViewAffinity.hxx>
#include <NCollection_DataMap.hxx>
#include <Graphic3d_MapOfStructure.hxx>
#include <Graphic3d_ViewAffinity.hxx>
#include <Graphic3d_ZLayerId.hxx>
#include <Graphic3d_ZLayerSettings.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_IndexedMap.hxx>
#include <Standard.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Type.hxx>
#include <NCollection_Array2.hxx>

class Graphic3d_GraphicDriver;
class Graphic3d_Structure;
class Graphic3d_DataStructureManager;

//! This class allows the definition of a manager to
//! which the graphic objects are associated.
//! It allows them to be globally manipulated.
//! It defines the global attributes.
//! Keywords: Structure, Structure Manager, Update Mode,
//! Destroy, Highlight, Visible
class Graphic3d_StructureManager : public Standard_Transient
{
  friend class Graphic3d_Structure;
  DEFINE_STANDARD_RTTIEXT(Graphic3d_StructureManager, Standard_Transient)
public:
  //! Initializes the ViewManager.
  //! Currently creating of more than 100 viewer instances
  //! is not supported and leads to InitializationError and
  //! initialization failure.
  //! This limitation might be addressed in some future OCCT releases.
  //! Warning: Raises InitialisationError if the initialization
  //! of the ViewManager failed.
  Standard_EXPORT Graphic3d_StructureManager(const occ::handle<Graphic3d_GraphicDriver>& theDriver);

  //! Deletes the manager <me>.
  Standard_EXPORT ~Graphic3d_StructureManager();

  //! Invalidates bounding box of specified ZLayerId.
  Standard_EXPORT virtual void Update(
    const Graphic3d_ZLayerId theLayerId = Graphic3d_ZLayerId_UNKNOWN) const;

  //! Deletes and erases the 3D structure manager.
  Standard_EXPORT virtual void Remove();

  //! Erases all the structures.
  Standard_EXPORT virtual void Erase();

  //! Returns the set of structures displayed in
  //! visualiser <me>.
  Standard_EXPORT void DisplayedStructures(NCollection_Map<occ::handle<Graphic3d_Structure>>& SG) const;

  //! Returns the set of highlighted structures
  //! in a visualiser <me>.
  Standard_EXPORT void HighlightedStructures(NCollection_Map<occ::handle<Graphic3d_Structure>>& SG) const;

  //! Forces a new construction of the structure.
  //! if <theStructure> is displayed and TOS_COMPUTED.
  Standard_EXPORT virtual void ReCompute(const occ::handle<Graphic3d_Structure>& theStructure);

  //! Forces a new construction of the structure.
  //! if <theStructure> is displayed in <theProjector> and TOS_COMPUTED.
  Standard_EXPORT virtual void ReCompute(
    const occ::handle<Graphic3d_Structure>&            theStructure,
    const occ::handle<Graphic3d_DataStructureManager>& theProjector);

  //! Clears the structure.
  Standard_EXPORT virtual void Clear(Graphic3d_Structure*   theStructure,
                                     const bool theWithDestruction);

  //! Connects the structures.
  Standard_EXPORT virtual void Connect(const Graphic3d_Structure* theMother,
                                       const Graphic3d_Structure* theDaughter);

  //! Disconnects the structures.
  Standard_EXPORT virtual void Disconnect(const Graphic3d_Structure* theMother,
                                          const Graphic3d_Structure* theDaughter);

  //! Display the structure.
  Standard_EXPORT virtual void Display(const occ::handle<Graphic3d_Structure>& theStructure);

  //! Erases the structure.
  Standard_EXPORT virtual void Erase(const occ::handle<Graphic3d_Structure>& theStructure);

  //! Highlights the structure.
  Standard_EXPORT virtual void Highlight(const occ::handle<Graphic3d_Structure>& theStructure);

  //! Transforms the structure.
  Standard_EXPORT virtual void SetTransform(const occ::handle<Graphic3d_Structure>& theStructure,
                                            const occ::handle<TopLoc_Datum3D>&      theTrsf);

  //! Changes the display priority of the structure <AStructure>.
  Standard_EXPORT virtual void ChangeDisplayPriority(
    const occ::handle<Graphic3d_Structure>& theStructure,
    const Graphic3d_DisplayPriority    theOldPriority,
    const Graphic3d_DisplayPriority    theNewPriority);

  //! Change Z layer for structure. The Z layer mechanism allows to display structures in higher
  //! layers in overlay of structures in lower layers.
  Standard_EXPORT virtual void ChangeZLayer(const occ::handle<Graphic3d_Structure>& theStructure,
                                            const Graphic3d_ZLayerId           theLayerId);

  //! Returns the graphic driver of <me>.
  Standard_EXPORT const occ::handle<Graphic3d_GraphicDriver>& GraphicDriver() const;

  //! Attaches the view to this structure manager and sets its identification number within the
  //! manager.
  Standard_EXPORT int Identification(Graphic3d_CView* theView);

  //! Detach the view from this structure manager and release its identification.
  Standard_EXPORT void UnIdentification(Graphic3d_CView* theView);

  //! Returns the group of views defined in the structure manager.
  Standard_EXPORT const NCollection_IndexedMap<Graphic3d_CView*>& DefinedViews() const;

  //! Returns the theoretical maximum number of definable views in the manager.
  //! Warning: It's not possible to accept an infinite number of definable views because each
  //! view must have an identification and we have different managers.
  Standard_EXPORT int MaxNumOfViews() const;

  //! Returns the structure with the identification number <AId>.
  Standard_EXPORT virtual occ::handle<Graphic3d_Structure> Identification(
    const int AId) const;

  //! Suppress the highlighting on the structure <AStructure>.
  Standard_EXPORT virtual void UnHighlight(const occ::handle<Graphic3d_Structure>& AStructure);

  //! Suppresses the highlighting on all the structures in <me>.
  Standard_EXPORT virtual void UnHighlight();

  //! Recomputes all structures in the manager.
  //! Resets Device Lost flag.
  Standard_EXPORT void RecomputeStructures();

  //! Recomputes all structures from theStructures.
  Standard_EXPORT void RecomputeStructures(
    const NCollection_Map<Graphic3d_Structure*>& theStructures);

  Standard_EXPORT void RegisterObject(const occ::handle<Standard_Transient>&     theObject,
                                      const occ::handle<Graphic3d_ViewAffinity>& theAffinity);

  Standard_EXPORT void UnregisterObject(const occ::handle<Standard_Transient>& theObject);

  Standard_EXPORT const occ::handle<Graphic3d_ViewAffinity>& ObjectAffinity(
    const occ::handle<Standard_Transient>& theObject) const;

  //! Returns TRUE if Device Lost flag has been set and presentation data should be reuploaded onto
  //! graphics driver.
  bool IsDeviceLost() const { return myDeviceLostFlag; }

  //! Sets Device Lost flag.
  void SetDeviceLost() { myDeviceLostFlag = true; }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const;

protected:
  //! Returns the number of structures displayed in
  //! visualizer <me>.
  //! Returns the structure displayed in visualizer <me>.
  Standard_EXPORT int NumberOfDisplayedStructures() const;

protected:
  Aspect_GenId                    myViewGenId;
  NCollection_Map<occ::handle<Graphic3d_Structure>>        myDisplayedStructure;
  NCollection_Map<occ::handle<Graphic3d_Structure>>        myHighlightedStructure;
  NCollection_DataMap<const Standard_Transient*, occ::handle<Graphic3d_ViewAffinity>>           myRegisteredObjects;
  occ::handle<Graphic3d_GraphicDriver> myGraphicDriver;
  NCollection_IndexedMap<Graphic3d_CView*>      myDefinedViews;
  bool                myDeviceLostFlag;
};

#endif // _Graphic3d_StructureManager_HeaderFile
