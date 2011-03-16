// File:      VrmlData_ImageTexture.hxx
// Created:   25.05.06 16:08:10
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2006


#ifndef VrmlData_ImageTexture_HeaderFile
#define VrmlData_ImageTexture_HeaderFile

#include <VrmlData_Texture.hxx>

/**
 *  Implementation of the ImageTexture node
 */
class VrmlData_ImageTexture : public VrmlData_Texture
{
 public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Empty constructor
   */
  inline VrmlData_ImageTexture ()
  {}

  /**
   * Constructor
   */
  Standard_EXPORT VrmlData_ImageTexture
                            (const VrmlData_Scene&  theScene,
                             const char             * theName,
                             const char             * theURL = 0L,
                             const Standard_Boolean theRepS = Standard_False,
                             const Standard_Boolean theRepT = Standard_False);

  /**
   * Query the associated URL.
   */
  inline const NCollection_List<TCollection_AsciiString>&
                        URL     () const
  { return myURL; }

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

 protected:
  // ---------- PROTECTED METHODS ----------



 private:
  // ---------- PRIVATE FIELDS ----------

  NCollection_List<TCollection_AsciiString> myURL;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (VrmlData_ImageTexture)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (VrmlData_ImageTexture, VrmlData_Texture)


#endif
