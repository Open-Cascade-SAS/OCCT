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



#ifndef VrmlData_Cone_HeaderFile
#define VrmlData_Cone_HeaderFile

#include <VrmlData_Geometry.hxx>

/**
 *  Implementation of the Cone node.
 *  The cone is located with its middle of the height segment in (0., 0., 0.) 
 *  The height is oriented along OY.
 */
class VrmlData_Cone : public VrmlData_Geometry
{
 public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Empty constructor
   */
  inline VrmlData_Cone ()
    : myBottomRadius    (1.),
      myHeight          (2.),
      myHasSide         (Standard_True),
      myHasBottom       (Standard_True)
  {}

  /**
   * Constructor
   */
  inline VrmlData_Cone (const VrmlData_Scene&   theScene,
                        const char              * theName,
                        const Standard_Real     theBottomRadius = 1.,
                        const Standard_Real     theHeight = 2.)
    : VrmlData_Geometry (theScene, theName),
      myBottomRadius    (theBottomRadius),
      myHeight          (theHeight),
      myHasSide         (Standard_True),
      myHasBottom       (Standard_True)
  {}

  /**
   * Query the Bottom Radius
   */
  inline Standard_Real    BottomRadius  () const    { return myBottomRadius; }

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
   * Set the Bottom Radius
   */
  inline void             SetBottomRadius (const Standard_Real theRadius)
  { myBottomRadius = theRadius; SetModified(); }

  /**
   * Set the Height
   */
  inline void             SetHeight     (const Standard_Real theHeight)
  { myHeight = theHeight; SetModified(); }

  /**
   * Set which faces are included
   */
  inline void             SetFaces      (const Standard_Boolean hasBottom,
                                         const Standard_Boolean hasSide)
  { myHasBottom = hasBottom; myHasSide = hasSide; SetModified(); }

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
                        Clone   (const Handle(VrmlData_Node)& theOther) const;

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


 protected:
  // ---------- PROTECTED METHODS ----------



 private:
  // ---------- PRIVATE FIELDS ----------

  Standard_Real         myBottomRadius;
  Standard_Real         myHeight;
  Standard_Boolean      myHasSide       : 1;
  Standard_Boolean      myHasBottom     : 1;


 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (VrmlData_Cone)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (VrmlData_Cone, VrmlData_Geometry)


#endif
