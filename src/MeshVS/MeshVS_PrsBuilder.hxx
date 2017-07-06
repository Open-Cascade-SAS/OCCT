// Created on: 2003-10-10
// Created by: Alexander SOLOVYOV
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#ifndef _MeshVS_PrsBuilder_HeaderFile
#define _MeshVS_PrsBuilder_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <MeshVS_MeshPtr.hxx>
#include <Standard_Integer.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <Standard_Transient.hxx>
#include <MeshVS_DisplayModeFlags.hxx>
#include <MeshVS_BuilderPriority.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
class MeshVS_DataSource;
class MeshVS_Drawer;
class MeshVS_Mesh;
class Prs3d_Presentation;
class SelectBasics_SensitiveEntity;
class SelectBasics_EntityOwner;


class MeshVS_PrsBuilder;
DEFINE_STANDARD_HANDLE(MeshVS_PrsBuilder, Standard_Transient)

//! This class is parent for all builders using in MeshVS_Mesh.
//! It provides base fields and methods all buildes need.
class MeshVS_PrsBuilder : public Standard_Transient
{

public:

  
  //! Builds presentation of certain type of data.
  //! Prs is presentation object which this method constructs.
  //! IDs is set of numeric identificators forming object appearance.
  //! IDsToExclude is set of IDs to exclude from processing. If some entity
  //! has been excluded, it is not processed by other builders.
  //! IsElement indicates, IDs is identificators of nodes or elements.
  //! DisplayMode is numeric constant describing display mode (see MeshVS_DisplayModeFlags.hxx)
  Standard_EXPORT virtual void Build (const Handle(Prs3d_Presentation)& Prs, const TColStd_PackedMapOfInteger& IDs, TColStd_PackedMapOfInteger& IDsToExclude, const Standard_Boolean IsElement, const Standard_Integer DisplayMode) const = 0;
  
  //! This method is called to build presentation of custom elements (they have MeshVS_ET_0D type).
  //! IDs is set of numeric identificators of elements for custom building.
  //! IDsToExclude is set of IDs to exclude from processing. If some entity
  //! has been excluded, it is not processed by other builders.
  //! DisplayMode is numeric constant describing display mode (see MeshVS_DisplayModeFlags.hxx)
  Standard_EXPORT virtual void CustomBuild (const Handle(Prs3d_Presentation)& Prs, const TColStd_PackedMapOfInteger& IDs, TColStd_PackedMapOfInteger& IDsToExclude, const Standard_Integer DisplayMode) const;
  
  //! This method is called to build sensitive of custom elements ( they have MeshVS_ET_0D type )
  Standard_EXPORT virtual Handle(SelectBasics_SensitiveEntity) CustomSensitiveEntity (const Handle(SelectBasics_EntityOwner)& Owner, const Standard_Integer SelectMode) const;
  
  //! Returns flags, assigned with builder during creation
  Standard_EXPORT Standard_Integer GetFlags() const;
  
  //! Test whether display mode has flags assigned with this builder.
  //! This method has default implementation and can be redefined for advance behavior
  //! Returns Standard_True only if display mode is appropriate for this builder
  Standard_EXPORT virtual Standard_Boolean TestFlags (const Standard_Integer DisplayMode) const;
  
  //! Returns builder ID
  Standard_EXPORT Standard_Integer GetId() const;
  
  //! Returns priority; as priority bigger, as soon builder will be called.
  Standard_EXPORT Standard_Integer GetPriority() const;
  
  //! Returns custom data source or default ( from MeshVS_Mesh ) if custom is NULL
  Standard_EXPORT Handle(MeshVS_DataSource) GetDataSource() const;
  
  //! Change custom data source
  Standard_EXPORT void SetDataSource (const Handle(MeshVS_DataSource)& newDS);
  
  //! Returns custom drawer or default ( from MeshVS_Mesh ) if custom is NULL
  Standard_EXPORT Handle(MeshVS_Drawer) GetDrawer() const;
  
  //! Change custom drawer
  Standard_EXPORT void SetDrawer (const Handle(MeshVS_Drawer)& newDr);
  
  //! Set excluding state. If it is Standard_True, the nodes or elements, processed by current builder
  //! will be noted and next builder won't process its.
  Standard_EXPORT void SetExcluding (const Standard_Boolean state);
  
  //! Read excluding state
  Standard_EXPORT Standard_Boolean IsExcludingOn() const;
  
  //! Set presentation manager for builder
  Standard_EXPORT void SetPresentationManager (const Handle(PrsMgr_PresentationManager3d)& thePrsMgr);
  
  //! Get presentation manager of builder
  Standard_EXPORT Handle(PrsMgr_PresentationManager3d) GetPresentationManager() const;




  DEFINE_STANDARD_RTTIEXT(MeshVS_PrsBuilder,Standard_Transient)

protected:

  
  //! Constructor
  //! Parent is pointer to MeshVS_Mesh object
  //! Flags is set of display modes corresponding to this builder
  //! DS is data source object, from which builder will pick geometry and topological information
  //! Id is numeric identificator of builder. You must set it to positive integer, but if
  //! you set it to -1, constructor will select the smallest integer, not occupied by other builders
  //! Priority is numerical priority constant. As priority bigger, as sooner builder starts during
  //! presentation construction
  Standard_EXPORT MeshVS_PrsBuilder(const Handle(MeshVS_Mesh)& Parent, const MeshVS_DisplayModeFlags& Flags, const Handle(MeshVS_DataSource)& DS, const Standard_Integer Id, const MeshVS_BuilderPriority& Priority = MeshVS_BP_Default);
  
  //! Returns only custom data source
  Standard_EXPORT Handle(MeshVS_DataSource) DataSource() const;
  
  //! Returns only custom drawer
  Standard_EXPORT Handle(MeshVS_Drawer) Drawer() const;

  MeshVS_MeshPtr myParentMesh;


private:


  Standard_Boolean myIsExcluding;
  Handle(MeshVS_DataSource) myDataSource;
  Handle(MeshVS_Drawer) myDrawer;
  Standard_Integer myFlags;
  Standard_Integer myId;
  Standard_Integer myPriority;
  Handle(PrsMgr_PresentationManager3d) myPrsMgr;


};







#endif // _MeshVS_PrsBuilder_HeaderFile
