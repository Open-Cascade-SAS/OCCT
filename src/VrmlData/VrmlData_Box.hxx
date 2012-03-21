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



#ifndef VrmlData_Box_HeaderFile
#define VrmlData_Box_HeaderFile

#include <VrmlData_Geometry.hxx>
#include <gp_XYZ.hxx>

/**
 *  Inplementation of the Box node.
 *  This node is defined by Size vector, assumong that the box center is located
 *  in (0., 0., 0.) and that each corner is 0.5*|Size| distance from the center.
 */
class VrmlData_Box : public VrmlData_Geometry
{
 public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Empty constructor
   */
  inline VrmlData_Box           ()
    : mySize (2., 2., 2.)
  {}

  /**
   * Constructor
   */
  inline VrmlData_Box           (const VrmlData_Scene&  theScene,
                                 const char             * theName,
                                 const Standard_Real    sizeX = 2.,
                                 const Standard_Real    sizeY = 2.,
                                 const Standard_Real    sizeZ = 2.)
    : VrmlData_Geometry (theScene, theName),
      mySize            (sizeX, sizeY, sizeZ)
  {}

  /**
   * Query the Box size
   */
  inline const gp_XYZ&  Size    () const                { return mySize; }

  /**
   * Set the Box Size
   */
  inline void           SetSize (const gp_XYZ& theSize)
  { mySize = theSize; SetModified(); }

  /**
   * Query the primitive topology. This method returns a Null shape if there
   * is an internal error during the primitive creation (zero radius, etc.)
   */
  Standard_EXPORT virtual const Handle(TopoDS_TShape)&
                        TShape  ();

  /**
   * Create a copy of this node.
   * If the parameter is null, a new copied node is created. Otherwise new node
   * is not created, but rather the given one is modified.
   */
  Standard_EXPORT virtual Handle(VrmlData_Node)
                        Clone   (const Handle(VrmlData_Node)& theOther)const;

  /**
   * Fill the Node internal data from the given input stream.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus
                        Read    (VrmlData_InBuffer& theBuffer);

  /**
   * Write the Node to output stream.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus
                        Write   (const char * thePrefix) const;

 private:
  // ---------- PRIVATE FIELDS ----------

  gp_XYZ        mySize;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (VrmlData_Box)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (VrmlData_Box, VrmlData_Geometry)


#endif
