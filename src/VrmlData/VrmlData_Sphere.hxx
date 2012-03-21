// Created on: 2006-05-26
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



#ifndef VrmlData_Sphere_HeaderFile
#define VrmlData_Sphere_HeaderFile

#include <VrmlData_Geometry.hxx>

/**
 *  Implementation of the Sphere node.
 */
class VrmlData_Sphere : public VrmlData_Geometry
{
 public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Empty constructor
   */
  inline VrmlData_Sphere ()
    : myRadius (1.)
  {}

  /**
   * Constructor
   */
  inline VrmlData_Sphere (const VrmlData_Scene& theScene,
                          const char            * theName,
                          const Standard_Real   theRadius = 1.)
    : VrmlData_Geometry (theScene, theName),
      myRadius          (theRadius)
  {}

  /**
   * Query the sphere radius
   */
  inline Standard_Real  Radius          () const        { return myRadius; } 

  /**
   * Set the spere radius
   */
  inline void           SetRadius       (const Standard_Real  theRadius)
  { myRadius = theRadius; SetModified(); }

  /**
   * Query the primitive topology. This method returns a Null shape if there
   * is an internal error during the primitive creation (zero radius, etc.)
   */
  Standard_EXPORT virtual const Handle(TopoDS_TShape)&  TShape ();

  /**
   * Create a copy of this node.
   * If the parameter is null, a new copied node is created. Otherwise new node
   * is not created, but rather the given one is modified.
   */
  Standard_EXPORT virtual Handle(VrmlData_Node)
                        Clone     (const Handle(VrmlData_Node)& theOther)const;

  /**
   * Fill the Node internal data from the given input stream.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus
                        Read            (VrmlData_InBuffer& theBuffer);

  /**
   * Write the Node to output stream.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus
                        Write           (const char * thePrefix) const;

 private:
  // ---------- PRIVATE FIELDS ----------

  Standard_Real         myRadius;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (VrmlData_Sphere)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (VrmlData_Sphere, VrmlData_Geometry)


#endif
