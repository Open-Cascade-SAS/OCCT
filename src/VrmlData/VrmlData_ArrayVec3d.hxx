// File:      VrmlData_ArrayVec3d.hxx
// Created:   26.05.06 18:12:55
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2006


#ifndef VrmlData_ArrayVec3d_HeaderFile
#define VrmlData_ArrayVec3d_HeaderFile

#include <VrmlData_Node.hxx>
class gp_XYZ;

/**
 * Implementatioon of basic node for Coordinate, Normal and Color
 * (array of triplets).
 */
class VrmlData_ArrayVec3d : public VrmlData_Node
{
 public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Empty constructor
   */
  inline VrmlData_ArrayVec3d ()
    : myArray (0L), myLength (0)
  {}

  /**
   * Constructor
   */
  inline VrmlData_ArrayVec3d (const VrmlData_Scene& theScene,
                              const char            * theName,
                              const size_t          nVec,
                              const gp_XYZ          * arrVec)
    : VrmlData_Node     (theScene, theName),
      myArray           (arrVec),
      myLength          (nVec)
  {}

  /**
   * Query the number of vectors
   */
  inline size_t         Length          () const { return myLength; }

  /**
   * Query the array
   */
  inline const gp_XYZ * Values          () const { return myArray; }

  /**
   * Create a data array and assign the field myArray.
   * @return
   *   True if allocation was successful.
   */ 
  Standard_EXPORT Standard_Boolean
                        AllocateValues  (const Standard_Size theLength);

  /**
   * Set the array data
   */
  inline void           SetValues (const size_t nValues,
                                   const gp_XYZ * arrValues)
  { myLength = nValues; myArray = arrValues; }

  /**
   * Create a copy of this node.
   * If the parameter is null, a new copied node is created. Otherwise new node
   * is not created, but rather the given one is modified.
   */
//   Standard_EXPORT virtual Handle(VrmlData_Node)
//                         Clone     (const Handle(VrmlData_Node)& theOther)const;

  /**
   * Read the Node from input stream.
   */
  Standard_EXPORT VrmlData_ErrorStatus
                        ReadArray (VrmlData_InBuffer&     theBuffer,
                                   const char *           theName,
                                   const Standard_Boolean isScale);

  /**
   * Write the Node to the output stream currently opened in Scene.
   */
  Standard_EXPORT VrmlData_ErrorStatus
                        WriteArray(const char *           theName,
                                   const Standard_Boolean isScale) const;

  /**
   * Returns True if the node is default, so that it should not be written.
   */
  Standard_EXPORT virtual Standard_Boolean
                        IsDefault () const;

 protected:
  // ---------- PROTECTED METHODS ----------
  /**
   * Query one vector
   * @param i
   *   index in the array of vectors [0 .. N-1]
   * @return
   *   the vector for the index. If index irrelevant, returns (0., 0., 0.)
   */
  Standard_EXPORT const gp_XYZ& Value (const Standard_Integer i) const;

 protected:
  // ---------- PROTECTED FIELDS ----------

  const gp_XYZ  * myArray;
  Standard_Size   myLength;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (VrmlData_ArrayVec3d)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (VrmlData_ArrayVec3d, VrmlData_Node)


#endif
