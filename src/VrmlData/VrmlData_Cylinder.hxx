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

#ifndef VrmlData_Cylinder_HeaderFile
#define VrmlData_Cylinder_HeaderFile

#include <VrmlData_Geometry.hxx>

/**
 *  Implementation of the Cylinder node
 */
class VrmlData_Cylinder : public VrmlData_Geometry
{
 public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Empty constructor
   */
  inline VrmlData_Cylinder ()
    : myRadius          (1.),
      myHeight          (2.),
      myHasBottom       (Standard_True),
      myHasSide         (Standard_True),
      myHasTop          (Standard_True)
  {}
  /**
   * Constructor
   */
  inline VrmlData_Cylinder (const VrmlData_Scene& theScene,
                            const char          * theName,
                            const Standard_Real   theRadius = 1.,
                            const Standard_Real   theHeight = 2.)
    : VrmlData_Geometry (theScene, theName),
      myRadius          (theRadius),
      myHeight          (theHeight),
      myHasBottom       (Standard_True),
      myHasSide         (Standard_True),
      myHasTop          (Standard_True)
  {}

  /**
   * Query the Radius
   */
  inline Standard_Real    Radius        () const    { return myRadius; }

  /**
   * Query the Height
   */
  inline Standard_Real    Height        () const    { return myHeight; }

  /**
   * Query if the bottom circle is included
   */
  inline Standard_Boolean HasBottom     () const    { return myHasBottom; }

  /**
   * Query if the side surface is included
   */
  inline Standard_Boolean HasSide       () const    { return myHasSide; }

  /**
   * Query if the top surface is included
   */
  inline Standard_Boolean HasTop        () const    { return myHasTop; }

  /**
   * Set the Radius
   */
  inline void             SetRadius     (const Standard_Real theRadius)
  { myRadius = theRadius; SetModified(); }

  /**
   * Set the Height
   */
  inline void             SetHeight     (const Standard_Real theHeight)
  { myHeight = theHeight; SetModified(); }

  /**
   * Set which faces are included
   */
  inline void             SetFaces      (const Standard_Boolean hasBottom,
                                         const Standard_Boolean hasSide,
                                         const Standard_Boolean hasTop)
  { myHasBottom = hasBottom; myHasSide = hasSide;
    myHasTop = hasTop; SetModified(); }

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
                          Read          (VrmlData_InBuffer& theBuffer);

  /**
   * Write the Node to output stream.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus
                          Write         (const char * thePrefix) const;

 protected:
  // ---------- PROTECTED METHODS ----------



 private:
  // ---------- PRIVATE FIELDS ----------
  Standard_Real         myRadius;
  Standard_Real         myHeight;
  Standard_Boolean      myHasBottom : 1;
  Standard_Boolean      myHasSide   : 1;
  Standard_Boolean      myHasTop    : 1;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (VrmlData_Cylinder)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (VrmlData_Cylinder, VrmlData_Geometry)


#endif
