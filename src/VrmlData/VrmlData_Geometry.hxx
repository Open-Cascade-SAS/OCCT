// Created on: 2006-05-25
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

#ifndef VrmlData_Geometry_HeaderFile
#define VrmlData_Geometry_HeaderFile

#include <VrmlData_Node.hxx>
#include <TopoDS_TShape.hxx>

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
