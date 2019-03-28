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

#ifndef VrmlData_Node_HeaderFile
#define VrmlData_Node_HeaderFile

#include <Standard_Type.hxx>
#include <NCollection_List.hxx>
#include <Standard_Type.hxx>
#include <Standard_TypeDef.hxx>
#include <TCollection_AsciiString.hxx> 
#include <VrmlData_ErrorStatus.hxx>

#define VRMLDATA_LCOMPARE(aa, bb) \
((strncmp (aa, bb, sizeof(bb)-1)) ? 0L : (aa += sizeof(bb)-1))

struct VrmlData_InBuffer;
class VrmlData_Scene;
class gp_XY;
class gp_XYZ;
class TCollection_AsciiString;

/**
 *  Abstract VRML Node
 */
class VrmlData_Node : public Standard_Transient
{
 public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Empty constructor
   */
  Standard_EXPORT               VrmlData_Node   ();

  /**
   * Destructor
   */
  virtual                       ~VrmlData_Node  () {}

  /**
   * Query the Scene that contains this Node
   */
  inline const VrmlData_Scene&  Scene           () const
  { return * myScene; }

  /**
   * Query the name
   */
  inline const char *           Name            () const { return myName; } 

  /**
   * Read a complete node definition from VRML stream
   * @param theBuffer
   *   Buffer receiving the input data.
   * @param theNode
   *   <tt>[out]</tt> Node restored from the buffer data
   * @param Type
   *   Node type to be checked. If it is NULL(default) no type checking is done.
   *   Otherwise the created node is matched and an error is returned if
   *   no match detected.
   */
  Standard_EXPORT VrmlData_ErrorStatus
                                ReadNode    (VrmlData_InBuffer&    theBuffer,
                                             Handle(VrmlData_Node)&theNode,
                                             const Handle(Standard_Type)& Type
                                                = NULL);

  /**
   * Read the Node from input stream.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus
                                Read        (VrmlData_InBuffer& theBuffer) = 0;

  /**
   * Write the Node to output stream.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus
                                Write       (const char * thePrefix) const;

  /**
   * Returns True if the node is default, then it would not be written.
   */
  Standard_EXPORT virtual Standard_Boolean
                                IsDefault   () const;

  /**
   * Write the closing brace in the end of a node output.
   */
  Standard_EXPORT VrmlData_ErrorStatus
                                WriteClosing () const;

  /**
   * Create a copy of this node.
   * If the parameter is null, a new copied node is created. Otherwise new node
   * is not created, but rather the given one is modified.<p>
   * This method nullifies the argument node if its member myScene differs
   * from that one of the current instance.
   */
  Standard_EXPORT virtual Handle(VrmlData_Node)
                                Clone       (const Handle(VrmlData_Node)&)const;

  /**
   * Read one boolean value (TRUE or FALSE).
   */
  Standard_EXPORT static VrmlData_ErrorStatus
                                ReadBoolean (VrmlData_InBuffer& theBuffer,
                                             Standard_Boolean&  theResult);

  /**
   * Read one quoted string, the quotes are removed.
   */
  Standard_EXPORT static VrmlData_ErrorStatus
                                ReadString  (VrmlData_InBuffer& theBuffer,
                                             TCollection_AsciiString& theRes);

  /**
   * Read one quoted string, the quotes are removed.
   */
  Standard_EXPORT static VrmlData_ErrorStatus
                                ReadMultiString
                        (VrmlData_InBuffer& theBuffer,
                         NCollection_List<TCollection_AsciiString>& theRes);

  /**
   * Read one integer value.
   */
  Standard_EXPORT static VrmlData_ErrorStatus
                                ReadInteger (VrmlData_InBuffer& theBuffer,
                                             long&              theResult);

  static inline Standard_Boolean OK (const VrmlData_ErrorStatus theStat)
  { return theStat == VrmlData_StatusOK; }

  static inline Standard_Boolean OK (VrmlData_ErrorStatus&      outStat,
                                     const VrmlData_ErrorStatus theStat)
  { return (outStat = theStat) == VrmlData_StatusOK; }

  /**
   * Define the common Indent in spaces, for writing all nodes.
   */ 
  static inline Standard_Integer GlobalIndent ()
  { return 2; }

 protected:
  // ---------- PROTECTED METHODS ----------

  /**
   * Constructor
   */
  Standard_EXPORT VrmlData_Node         (const VrmlData_Scene& theScene,
                                         const char            * theName);

  /**
   * Read the closing brace. If successful, theBufrfer is incremented.
   * If no brace is found, theBuffer stays in untouched and the method returns
   * VrmlFormatError.
   */
  Standard_EXPORT static VrmlData_ErrorStatus
                                readBrace   (VrmlData_InBuffer& theBuffer);

 private:
  // ---------- PRIVATE METHODS ----------

  /**
   * Method called from VrmlData_Scene when a name should be assigned
   * automatically.
   */
  Standard_EXPORT void          setName (const char * theName,
                                         const char * theSuffix = 0L);

 private:
  // ---------- PRIVATE FIELDS ----------

  const VrmlData_Scene  * myScene; 
  const char            * myName;       ///< name of the node
#ifdef OCCT_DEBUG
  Standard_Integer      myLineCount;
#endif

  friend class VrmlData_Group;
  friend class VrmlData_Scene;

 public:
// Declaration of CASCADE RTTI
  DEFINE_STANDARD_RTTIEXT(VrmlData_Node,Standard_Transient)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (VrmlData_Node, Standard_Transient)

//! Computes a hash code for the given VRML node, in the range [1, theUpperBound]
//! @param theNode the VRML node which hash code is to be computed
//! @param theUpperBound the upper bound of the range a computing hash code must be within
//! @return a computed hash code, in the range [1, theUpperBound]
Standard_EXPORT Standard_Integer HashCode (const Handle (VrmlData_Node) & theNode, Standard_Integer theUpperBound);

Standard_EXPORT Standard_Boolean IsEqual (const Handle(VrmlData_Node)& theOne,
                                          const Handle(VrmlData_Node)& theTwo);

#endif
