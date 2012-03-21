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
