// File:      VrmlData_Sphere.hxx
// Created:   26.05.06 22:05:53
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2006


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
