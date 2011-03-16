// File:      VrmlData_Geometry.hxx
// Created:   25.05.06 16:42:51
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2006


#ifndef VrmlData_Geometry_HeaderFile
#define VrmlData_Geometry_HeaderFile

#include <VrmlData_Node.hxx>
#include <Handle_TopoDS_TShape.hxx>

/**
 *  Implementation of the Geometry node.
 *  Contains the topological representation (TopoDS_Shell) of the VRML geometry
 */

class VrmlData_Geometry : public VrmlData_Node
{
 public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Empty constructor
   */
  inline VrmlData_Geometry ()
    : myIsModified      (Standard_True)
  {}

  /**
   * Constructor
   */
  inline VrmlData_Geometry (const VrmlData_Scene& theScene,
                            const char            * theName)
    : VrmlData_Node     (theScene, theName),
      myIsModified      (Standard_True)
  {}

  /**
   * Query the shape. This method checks the flag myIsModified; if True it
   * should rebuild the shape presentation.
   */
  Standard_EXPORT virtual const Handle(TopoDS_TShape)&  TShape () = 0;

 protected:
  // ---------- PROTECTED METHODS ----------

  /**
   * Set the TShape.
   */
  inline void   SetTShape       (const Handle(TopoDS_TShape)& theTShape)
  { myTShape = theTShape; }

  /**
   * Mark modification
   */
  inline void   SetModified     ()      { myIsModified= Standard_True; }


 protected:
  // ---------- PROTECTED FIELDS ----------

  Handle(TopoDS_TShape)  myTShape;
  Standard_Boolean       myIsModified;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (VrmlData_Geometry)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (VrmlData_Geometry, VrmlData_Node)


#endif
