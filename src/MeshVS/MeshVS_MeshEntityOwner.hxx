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

#ifndef _MeshVS_MeshEntityOwner_HeaderFile
#define _MeshVS_MeshEntityOwner_HeaderFile

#include <MeshVS_EntityType.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <PrsMgr_PresentationManager.hxx>
#include <Quantity_NameOfColor.hxx>
class PrsMgr_PresentationManager;


class MeshVS_MeshEntityOwner;
DEFINE_STANDARD_HANDLE(MeshVS_MeshEntityOwner, SelectMgr_EntityOwner)

//! The custom owner. This class provides methods to store owner information:
//! 1) An address of element or node data structure
//! 2) Type of node or element owner assigned
//! 3) ID of node or element owner assigned
class MeshVS_MeshEntityOwner : public SelectMgr_EntityOwner
{

public:

  
  Standard_EXPORT MeshVS_MeshEntityOwner(const SelectMgr_SelectableObject* SelObj, const Standard_Integer ID, const Standard_Address MeshEntity, const MeshVS_EntityType& Type, const Standard_Integer Priority = 0, const Standard_Boolean IsGroup = Standard_False);
  
  //! Returns an address of element or node data structure
  Standard_EXPORT Standard_Address Owner() const;
  
  //! Returns type of element or node data structure
  Standard_EXPORT MeshVS_EntityType Type() const;
  
  //! Returns ID of element or node data structure
  Standard_EXPORT Standard_Integer ID() const;
  
  //! Returns true if owner represents group of nodes or elements
  Standard_EXPORT Standard_Boolean IsGroup() const;
  
  //! Returns true if owner is hilighted
  Standard_EXPORT virtual Standard_Boolean IsHilighted (const Handle(PrsMgr_PresentationManager)& PM, const Standard_Integer Mode = 0) const Standard_OVERRIDE;
  
  //! Hilights owner with the certain color
  Standard_EXPORT virtual void HilightWithColor (const Handle(PrsMgr_PresentationManager)& thePM,
                                                 const Handle(Prs3d_Drawer)& theStyle,
                                                 const Standard_Integer theMode) Standard_OVERRIDE;
  
  //! Strip hilight of owner
  Standard_EXPORT virtual void Unhilight (const Handle(PrsMgr_PresentationManager)& PM, const Standard_Integer Mode = 0) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Clear (const Handle(PrsMgr_PresentationManager)& PM, const Standard_Integer Mode = 0) Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(MeshVS_MeshEntityOwner,SelectMgr_EntityOwner)

protected:




private:


  Standard_Address myAddr;
  MeshVS_EntityType myType;
  Standard_Integer myID;
  Standard_Boolean myIsGroup;


};







#endif // _MeshVS_MeshEntityOwner_HeaderFile
