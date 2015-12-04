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
                        Clone   (const Handle(VrmlData_Node)& theOther)const Standard_OVERRIDE;

  /**
   * Read the Node from input stream.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus
                        Read    (VrmlData_InBuffer& theBuffer) Standard_OVERRIDE;

  /**
   * Write the Node to output stream.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus
                        Write   (const char * thePrefix) const Standard_OVERRIDE;

 protected:
  // ---------- PROTECTED METHODS ----------



 private:
  // ---------- PRIVATE FIELDS ----------

  NCollection_List<TCollection_AsciiString> myURL;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI_INLINE(VrmlData_ImageTexture,VrmlData_Texture)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (VrmlData_ImageTexture, VrmlData_Texture)


#endif
