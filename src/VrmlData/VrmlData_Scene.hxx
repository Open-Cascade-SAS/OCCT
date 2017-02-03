// Created on: 2006-10-08
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

#ifndef VrmlData_Scene_HeaderFile
#define VrmlData_Scene_HeaderFile

#include <VrmlData_ListOfNode.hxx>
#include <VrmlData_MapOfNode.hxx>
#include <VrmlData_ErrorStatus.hxx>
#include <VrmlData_Geometry.hxx>
#include <VrmlData_WorldInfo.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_OStream.hxx>
#include <Standard_IStream.hxx>
#include <TCollection_ExtendedString.hxx>
#include <NCollection_IncAllocator.hxx>
#include <Standard_Mutex.hxx>
#include <VrmlData_DataMapOfShapeAppearance.hxx>

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

struct VrmlData_InBuffer;

/**
 * Block of comments describing class VrmlData_Scene
 */

class VrmlData_Scene 
{
 public:
  /**
   * Iterator type to get all contained Nodes one-by-one.
   */
  typedef VrmlData_ListOfNode::Iterator Iterator;

  // ---------- PUBLIC METHODS ----------

  /**
   * Constructor.
   */
  Standard_EXPORT VrmlData_Scene (const Handle(NCollection_IncAllocator)& = 0L);

  /**
   * Query the status of the previous operation.
   * Normally it should be equal to VrmlData_StatusOK (no error).
   */
  inline VrmlData_ErrorStatus   Status      () const
  { return myStatus; }

  /**
   * Add the given directory path to the list of VRML file search directories.
   * This method forms the list of directories ordered according to the
   * sequence of this method calls. When an Inline node is found, the URLs
   * in that node are matched with these directories.
   * The last (implicit) search directory is the current process directory
   * ("."). It takes effect if the list is empty or if there is no match with
   * exisiting directories.
   */
  Standard_EXPORT void          SetVrmlDir  (const TCollection_ExtendedString&);

  /**
   * Set the scale factor that would be further used in methods
   * ReadReal, ReadXYZ and ReadXY. All coordinates, distances and sized are
   * multiplied by this factor during reading the data.
   */
  inline void                   SetLinearScale (const Standard_Real theScale)
  { myLinearScale = theScale; }

  /**
   * Returns the directory iterator, to check the presence of requested VRML
   * file in each iterated directory.
   */
  inline NCollection_List<TCollection_ExtendedString>::Iterator
                                VrmlDirIterator () const
  { return NCollection_List<TCollection_ExtendedString>::Iterator(myVrmlDir); }

  /**
   * Iterator of Nodes
   */
  inline Iterator               GetIterator () const
  { return Iterator (myLstNodes); }

  /**
   * Get the iterator of named nodes.
   */
  inline VrmlData_MapOfNode::Iterator
                                NamedNodesIterator() const
  { return myNamedNodes; }

  /**
   * Allocator used by all nodes contained in the Scene.
   */
  inline const Handle(NCollection_IncAllocator)&
                                Allocator   () const
  { return myAllocator; }

  /**
   * Add a Node. If theN belongs to another Scene, it is cloned.
   * <p>VrmlData_WorldInfo cannot be added, in this case the method
   * returns a NULL handle.
   */
  Standard_EXPORT const Handle(VrmlData_Node)&
                                AddNode     (const Handle(VrmlData_Node)& theN,
                                             const Standard_Boolean isTopLevel
                                             = Standard_True);

  /**
   * Find a node by its name.
   * @param theName
   *   Name of the node to find.
   * @param theType
   *   Type to match. If this value is NULL, the first found node with the
   *   given name is returned. If theType is given, only the node that has
   *   that type is returned.
   */
  Standard_EXPORT Handle(VrmlData_Node)
                                FindNode    (const char * theName,
                                             const Handle(Standard_Type)&
                                                           theType = 0L) const;

  /**
   * Find a node by its name.
   * @param theName
   *   Name of the node to search for.
   * @param theLocation
   *   Location of the found node with respect to the whole VRML shape.
   */
  Standard_EXPORT Handle(VrmlData_Node)
                                FindNode(const char * theName,
                                         gp_Trsf&     theLocation) const;

  /**
   * Export to text stream (file or else).
   * This method is protected by Mutex, it is not allowed to read/write
   * two VRML streams concurrently.
   * The stream should give as the first line the VRML header:
   * <code>
   *   #VRML V2.0 <encoding type> [optional comment] <line terminator>
   * </code>
   *  
   */
  friend Standard_EXPORT Standard_OStream&
                                operator << (Standard_OStream&      theOutput,
                                             const VrmlData_Scene&  theScene);

  /**
   * Import from text stream (file or else).
   * This method is protected by Mutex, it is not allowed to read/write
   * two VRML streams concurrently.
   */
  Standard_EXPORT VrmlData_Scene& operator<<(Standard_IStream& theInput);

  /**
   * Convert the scene to a Shape.
   */
  Standard_EXPORT               operator TopoDS_Shape () const;

  /**
   * Convert the scene to a Shape, with the information on materials defined
   * for each sub-shape. This method should be used instead of TopoDS_Shape
   * explicit conversion operator when you need to retrieve the material
   * aspect for each face or edge in the returned topological object.
   * @param M
   *   Data Map that binds an Appearance instance to each created TFace or
   *   TEdge if the Appearance node is defined in VRML scene for that geometry.
   * @return
   *   TopoDS_Shape (Compound) holding all the scene, similar to the result of
   *   explicit TopoDS_Shape conversion operator.
   */
  Standard_EXPORT TopoDS_Shape  GetShape (VrmlData_DataMapOfShapeAppearance& M);

  /**
   * Query the WorldInfo member.
   */
  Standard_EXPORT const Handle(VrmlData_WorldInfo)&
                                WorldInfo() const;

  /**
   * Read a VRML line. Empty lines and comments are skipped.
   * The processing starts here from theBuffer.LinePtr; if there is at least
   * one non-empty character (neither space nor comment), this line is used
   * without reading the next one.
   * @param theLine
   *   Buffer receiving the input line
   * @param theInput
   *   Input stream
   * @param theLen
   *   Length of the input buffer (maximal line length)
   */
  Standard_EXPORT static VrmlData_ErrorStatus
                                ReadLine    (VrmlData_InBuffer& theBuffer);

  /**
   * Read a singel word from the input stream, delimited by whitespace.
   */
  Standard_EXPORT static VrmlData_ErrorStatus
                                ReadWord    (VrmlData_InBuffer&       theBuffer,
                                             TCollection_AsciiString& theStr);

  /**
   * Diagnostic dump of the contents
   */
  Standard_EXPORT void          Dump        (Standard_OStream& theStream) const;

  /**
   * Read one real value.
   */
  Standard_EXPORT VrmlData_ErrorStatus
                                ReadReal    (VrmlData_InBuffer& theBuffer,
                                             Standard_Real&     theResult,
                                             Standard_Boolean   isApplyScale,
                                             Standard_Boolean   isOnlyPositive)
                                                                        const;

  /**
   * Read one triplet of real values.
   */
  Standard_EXPORT VrmlData_ErrorStatus
                                ReadXYZ     (VrmlData_InBuffer& theBuffer,
                                             gp_XYZ&            theXYZ,
                                             Standard_Boolean   isApplyScale,
                                             Standard_Boolean   isOnlyPositive)
                                                                        const;

  /**
   * Read one doublet of real values.
   */
  Standard_EXPORT VrmlData_ErrorStatus
                                ReadXY      (VrmlData_InBuffer& theBuffer,
                                             gp_XY&             theXYZ,
                                             Standard_Boolean   isApplyScale,
                                             Standard_Boolean   isOnlyPositive)
                                                                        const;
  /**
   * Read an array of integer indices, for IndexedfaceSet and IndexedLineSet.
   */ 
  Standard_EXPORT VrmlData_ErrorStatus
                                ReadArrIndex(VrmlData_InBuffer& theBuffer,
                                             const Standard_Integer **& theArr,
                                             Standard_Size&             theNBl)
                                                                        const;

  /**
   * Query the line where the error occurred (if the status is not OK)
   */
  inline Standard_Integer       GetLineError() const { return myLineError; }

  /**
   * Store the indentation for VRML output.
   * @param nSpc
   *   number of spaces to insert at every indentation level
   */
  inline void                   SetIndent   (const Standard_Integer nSpc)
  { myIndent = nSpc; }

  /**
   * Write a triplet of real values on a separate line.
   * @param theXYZ
   *   The value to be output.
   * @param isScale
   *   If True, then each component is divided by myLinearScale.
   * @param thePostfix
   *   Optional string that is added before the end of the line.
   */
  Standard_EXPORT VrmlData_ErrorStatus
                                WriteXYZ    (const  gp_XYZ&         theXYZ,
                                             const Standard_Boolean isScale,
                                             const char           * thePostfix
                                                                    = 0L) const;
  /**
   * Write an array of integer indices, for IndexedFaceSet and IndexedLineSet.
   */
  Standard_EXPORT VrmlData_ErrorStatus
                                WriteArrIndex(const char *          thePrefix,
                                              const Standard_Integer ** theArr,
                                              const Standard_Size       theNbBl)
                                                                        const;


  /**
   * Write a string to the output stream respecting the indentation. The string
   * can be defined as two substrings that will be separated by a space.
   * Each of the substrings can be NULL, then it is ignored. If both
   * are NULL, then a single newline is output (without indent).
   * @param theLine0
   *   The first part of string to output
   * @param theLine1
   *   The second part of string to output
   * @param theIndent
   *   - 0 value ignored.
   *   - negative decreases the current indent and then outputs.
   *   - positive outputs and then increases the current indent. 
   * @return
   *   Error status of the stream, or a special error if myOutput == NULL.
   */
  Standard_EXPORT VrmlData_ErrorStatus
                                WriteLine   (const char           * theLine0,
                                             const char           * theLine1=0L,
                                             const Standard_Integer theIndent
                                                                     = 0) const;

  /**
   * Write the given node to output stream 'myOutput'.
   */
  Standard_EXPORT VrmlData_ErrorStatus
                                WriteNode   (const char * thePrefix,
                                             const Handle(VrmlData_Node)&) const;

  /**
   * Query if the current write operation is dummy, i.e., for the purpose of
   * collecting information before the real write is commenced.
   */
  inline Standard_Boolean       IsDummyWrite() const
  { return myOutput == 0L; }

 private:
  // ---------- PRIVATE METHODS (PROHIBITED) ----------
  VrmlData_Scene (const VrmlData_Scene&);
  VrmlData_Scene& operator = (const VrmlData_Scene&);

 protected:
  /**
   * Read whatever line from the input checking the istream flags.
   */ 
  Standard_EXPORT static VrmlData_ErrorStatus
                                readLine    (VrmlData_InBuffer&     theBuffer);

  /**
   * Read and verify the VRML header (the 1st line of the file)
   */ 
  Standard_EXPORT static VrmlData_ErrorStatus
                                readHeader  (VrmlData_InBuffer&     theBuffer);

  /**
   * Create the node.
   * @param theBuffer
   *   Input buffer from where the node is created
   * @param theNode
   *   Output parameter, contains the created node on exit
   * @param Type
   *   Node type to be checked. If it is NULL no type checking is done.
   *   Otherwise the created node is matched and an error is returned if
   *   no match detected.
   */
  Standard_EXPORT VrmlData_ErrorStatus 
                                createNode  (VrmlData_InBuffer&     theBuffer,
                                             Handle(VrmlData_Node)& theNode,
                                             const Handle(Standard_Type)& Type);

  /**
   * Create a single Shape object from all geometric nodes in the list.
   */
  Standard_EXPORT static void   createShape (TopoDS_Shape&          outShape,
                                             const VrmlData_ListOfNode&,
                                             VrmlData_DataMapOfShapeAppearance*);


 private:
  // ---------- PRIVATE FIELDS ----------
  Standard_Real                                 myLinearScale;
  VrmlData_ListOfNode                           myLstNodes; ///! top-level nodes
  VrmlData_ListOfNode                           myAllNodes; ///! all nodes
  VrmlData_ErrorStatus                          myStatus;
  Handle(NCollection_IncAllocator)               myAllocator;
  Handle(VrmlData_WorldInfo)                     myWorldInfo;
  VrmlData_MapOfNode                            myNamedNodes;

  // read from stream
  NCollection_List<TCollection_ExtendedString>  myVrmlDir;
  Standard_Mutex                                myMutex;
  Standard_Integer                              myLineError;///! #0 if error

  // write to stream
  Standard_OStream                              * myOutput;
  Standard_Integer                              myIndent;
  Standard_Integer                              myCurrentIndent;
  /**
   * This map is used to avoid multiple storage of the same named node: each
   * named node is added here when it is written the first time.
   */
  VrmlData_MapOfNode                            myNamedNodesOut;
  /**
   * This map allows to resolve multiple reference to any unnamed node. It
   * is used during the dummy write (myOutput == 0L). When a node is processed
   * the first time it is added to this map, the second time it is automatically
   * assigned a name.
   */
  NCollection_Map<Standard_Address>             myUnnamedNodesOut;
  Standard_Integer                              myAutoNameCounter;
  friend class VrmlData_Group;
  friend class VrmlData_Node;
};

#endif
