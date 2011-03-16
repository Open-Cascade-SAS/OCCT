// File:      VrmlData_Normal.hxx
// Created:   26.05.06 18:34:13
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2006


#ifndef VrmlData_Normal_HeaderFile
#define VrmlData_Normal_HeaderFile

#include <VrmlData_ArrayVec3d.hxx>

/**
 *  Implementation of the node Normal
 */
class VrmlData_Normal : public VrmlData_ArrayVec3d
{
 public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Empty constructor
   */
  inline VrmlData_Normal () {}

  /**
   * Constructor
   */
  inline VrmlData_Normal (const VrmlData_Scene& theScene,
                          const char            * theName,
                          const size_t          nVec     = 0,
                          const gp_XYZ          * arrVec = 0L)
    : VrmlData_ArrayVec3d (theScene, theName, nVec, arrVec)
  {}

  /**
   * Query one normal
   * @param i
   *   index in the array of normals [0 .. N-1]
   * @return
   *   the normal value for the index. If index irrelevant, returns (0., 0., 0.)
   */
  inline const gp_XYZ& Normal   (const Standard_Integer i) const
  { return Value(i); }

  /**
   * Create a copy of this node.
   * If the parameter is null, a new copied node is created. Otherwise new node
   * is not created, but rather the given one is modified.
   */
  Standard_EXPORT virtual Handle(VrmlData_Node)
                        Clone   (const Handle(VrmlData_Node)& theOther)const;

  /**
   * Read the Node from input stream.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus
                        Read    (VrmlData_InBuffer& theBuffer);

  /**
   * Write the Node to the Scene output.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus
                        Write   (const char * thePrefix) const;


 private:
  // ---------- PRIVATE FIELDS ----------


 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (VrmlData_Normal)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (VrmlData_Normal, VrmlData_ArrayVec3d)


#endif
