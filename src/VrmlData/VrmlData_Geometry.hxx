// Created on: 2006-05-25
// Created by: Alexander GRIGORIEV
// Copyright (c) 2006-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



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
