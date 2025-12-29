// Created on: 2006-05-29
// Created by: Alexander GRIGORIEV
// Copyright (c) 2006-2014 OPEN CASCADE SAS
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

#ifndef VrmlData_Group_HeaderFile
#define VrmlData_Group_HeaderFile

#include <NCollection_List.hxx>
#include <VrmlData_Node.hxx>
#include <NCollection_DataMap.hxx>
#include <VrmlData_Appearance.hxx>
#include <TopoDS_TShape.hxx>
#include <Bnd_B3.hxx>
#include <gp_Trsf.hxx>

class TopoDS_Shape;

/**
 * Implementation of node "Group"
 */

class VrmlData_Group : public VrmlData_Node
{
public:
  typedef NCollection_List<occ::handle<VrmlData_Node>>::Iterator Iterator;

  // ---------- PUBLIC METHODS ----------

  /**
   * Empty constructor.
   * @param isTransform
   *   True if the group of type Transform is defined
   * @param theAlloc
   *   Allocator used for the list of children
   */
  VrmlData_Group(const bool isTransform = false)
      : myIsTransform(isTransform)
  {
  }

  /**
   * Constructor.
   * @param theName
   *   Name of the Group node
   * @param isTransform
   *   True if the group of type Transform is defined
   * @param theAlloc
   *   Allocator used for the list of children
   */
  Standard_EXPORT VrmlData_Group(const VrmlData_Scene& theScene,
                                 const char*           theName,
                                 const bool            isTransform = false);

  /**
   *  Add one node to the Group.
   */
  inline occ::handle<VrmlData_Node>& AddNode(const occ::handle<VrmlData_Node>& theNode)
  {
    return myNodes.Append(theNode);
  }

  /**
   * Remove one node from the Group.
   * @return
   *   True if the node was located and removed, False if none removed.
   */
  Standard_EXPORT bool RemoveNode(const occ::handle<VrmlData_Node>& theNode);

  /**
   * Create iterator on nodes belonging to the Group.
   */
  inline Iterator NodeIterator() const { return Iterator(myNodes); }

  /**
   * Query the bounding box.
   */
  inline const Bnd_B3f& Box() const { return myBox; }

  /**
   * Set the bounding box.
   */
  inline void SetBox(const Bnd_B3f& theBox) { myBox = theBox; }

  /**
   * Set the transformation. Returns True if the group is Transform type,
   * otherwise do nothing and return False.
   */
  Standard_EXPORT bool SetTransform(const gp_Trsf& theTrsf);

  /**
   * Query the transform value.
   * For group without transformation this always returns Identity
   */
  inline const gp_Trsf& GetTransform() const { return myTrsf; }

  /**
   * Query if the node is Transform type.
   */
  inline bool IsTransform() const { return myIsTransform; }

  /**
   * Create a copy of this node.
   * If the parameter is null, a new copied node is created. Otherwise new node
   * is not created, but rather the given one is modified.
   */
  Standard_EXPORT occ::handle<VrmlData_Node> Clone(
    const occ::handle<VrmlData_Node>& theOther) const override;

  /**
   * Fill the Node internal data from the given input stream.
   */
  Standard_EXPORT VrmlData_ErrorStatus Read(VrmlData_InBuffer& theBuffer) override;

  /**
   * Write the Node to output stream.
   */
  Standard_EXPORT VrmlData_ErrorStatus Write(const char* thePrefix) const override;

  /**
   * Find a node by its name, inside this Group
   * @param theName
   *   Name of the node to search for.
   * @param theLocation
   *   Location of the found node with respect to this Group.
   */
  Standard_EXPORT occ::handle<VrmlData_Node> FindNode(const char* theName,
                                                      gp_Trsf&    theLocation) const;

  /**
   * Get the shape representing the group geometry.
   */
  Standard_EXPORT void Shape(
    TopoDS_Shape&                                                                      theShape,
    NCollection_DataMap<occ::handle<TopoDS_TShape>, occ::handle<VrmlData_Appearance>>* pMapApp);

protected:
  // ---------- PROTECTED METHODS ----------

  /**
   * Try to open a file by the given filename, using the search directories
   * list myVrmlDir of the Scene.
   */
  Standard_EXPORT VrmlData_ErrorStatus openFile(Standard_IStream&              theStream,
                                                const TCollection_AsciiString& theFilename);

private:
  // ---------- PRIVATE FIELDS ----------

  bool                                         myIsTransform;
  NCollection_List<occ::handle<VrmlData_Node>> myNodes;
  Bnd_B3f                                      myBox;
  gp_Trsf                                      myTrsf;

public:
  // Declaration of CASCADE RTTI
  DEFINE_STANDARD_RTTIEXT(VrmlData_Group, VrmlData_Node)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
#endif
