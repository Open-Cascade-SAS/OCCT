// File:      VrmlData_WorldInfo.hxx
// Created:   01.08.07 07:24
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2007


#ifndef VrmlData_WorldInfo_HeaderFile
#define VrmlData_WorldInfo_HeaderFile

#include <VrmlData_Node.hxx>

/**
 * Data type for WorldInfo node
 */

class VrmlData_WorldInfo : public VrmlData_Node
{
 public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Empty Constructor.
   */
  inline VrmlData_WorldInfo () : myTitle (0L) {}

  /**
   * Constructor.
   */
  Standard_EXPORT VrmlData_WorldInfo (const VrmlData_Scene&  theScene,
                                      const char             * theName = 0L,
                                      const char             * theTitle = 0L);

  /**
   * Set or modify the title.
   */
  Standard_EXPORT void SetTitle (const char * theString);

  /**
   * Add a string to the list of info strings.
   */
  Standard_EXPORT void  AddInfo (const char * theString);

  /**
   * Query the title string.
   */
  inline const char *   Title   () const
  { return myTitle; }

  /**
   * Return the iterator of Info strings.
   */
  inline NCollection_List <const char *>::Iterator
                        InfoIterator () const
  { return myInfo; }

  /**
   * Create a copy of this node.
   * If the parameter is null, a new copied node is created. Otherwise new node
   * is not created, but rather the given one is modified.
   */
  Standard_EXPORT virtual Handle(VrmlData_Node)
                        Clone   (const Handle(VrmlData_Node)& theOther) const;

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

  /**
   * Returns True if the node is default, then it would not be written.
   */
  Standard_EXPORT virtual Standard_Boolean
                        IsDefault() const;

 private:
  // ---------- PRIVATE FIELDS ----------

  const char                    * myTitle;
  NCollection_List <const char *> myInfo;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (VrmlData_WorldInfo)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (VrmlData_WorldInfo, VrmlData_Node)


#endif
