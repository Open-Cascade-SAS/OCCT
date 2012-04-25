// Created by: DAUTRY Philippe
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

#ifndef TDF_LabelNode_HeaderFile
#define TDF_LabelNode_HeaderFile

#include <TCollection_AsciiString.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_LabelNodePtr.hxx>
#include <TDF_HAllocator.hxx>
#include <NCollection_DefineAlloc.hxx>

class TDF_Attribute;
class TDF_AttributeIterator;
class TDF_ChildIterator;
class TDF_Data;
class Handle_TDF_Data;
class TDF_Label;

#define KEEP_LOCAL_ROOT

enum {
  TDF_LabelNodeImportMsk = (int) 0x80000000, // Because the sign bit (HP).
  TDF_LabelNodeAttModMsk = 0x40000000,
  TDF_LabelNodeMayModMsk = 0x20000000,
  TDF_LabelNodeFlagsMsk = (TDF_LabelNodeImportMsk \
                         | TDF_LabelNodeAttModMsk \
                         | TDF_LabelNodeMayModMsk)
};

//=======================================================================
//class: TDF_LabelNode
//=======================================================================

class TDF_LabelNode {

  public :

  // Public Methods
  // --------------------------------------------------------------------------

  // Father access
  inline TDF_LabelNode* Father() const
    { return myFather; };

  // Brother access
  inline TDF_LabelNode* Brother() const
    { return myBrother; };

  // Child access
  inline TDF_LabelNode* FirstChild() const
    { return myFirstChild; };

    // Attribute access
  inline const Handle(TDF_Attribute)& FirstAttribute() const
    { return myFirstAttribute; };

  // Tag access
  inline Standard_Integer Tag() const
    { return myTag; };

  // Depth access
  inline Standard_Integer Depth() const
    { return (myFlags & ~TDF_LabelNodeFlagsMsk); };

  // IsRoot
  inline Standard_Boolean IsRoot() const
    { return myFather == NULL; };

  // Data
  Standard_EXPORT TDF_Data * Data() const;

  // Flag AttributesModified access
  inline void AttributesModified(const Standard_Boolean aStatus)
    {
      myFlags = (aStatus) ?
        (myFlags | TDF_LabelNodeAttModMsk) :
          (myFlags & ~TDF_LabelNodeAttModMsk);
      if (aStatus) AllMayBeModified();
    };

  inline Standard_Boolean AttributesModified() const
    { return ((myFlags & TDF_LabelNodeAttModMsk) != 0); };

  // Flag MayBeModified access
  inline void MayBeModified(const Standard_Boolean aStatus)
    { myFlags = (aStatus) ?
        (myFlags | TDF_LabelNodeMayModMsk) :
          (myFlags & ~TDF_LabelNodeMayModMsk); };

  inline Standard_Boolean MayBeModified() const
    { return ((myFlags & TDF_LabelNodeMayModMsk) != 0); };

  private :

  // Memory management
  DEFINE_NCOLLECTION_ALLOC

  // Constructor
  TDF_LabelNode(TDF_Data* Data);
  
  // Destructor and deallocator
  void Destroy (const TDF_HAllocator& theAllocator);

  // Public Friends
  // --------------------------------------------------------------------------

  friend class TDF_Data;
  friend class TDF_Label;

  private :

  // Private Methods
  // --------------------------------------------------------------------------

  // Constructor
  TDF_LabelNode(const Standard_Integer Tag, TDF_LabelNode* Father);

  // Others
  void AddAttribute(const Handle(TDF_Attribute)& afterAtt,
                    const Handle(TDF_Attribute)& newAtt);

  void RemoveAttribute(const Handle(TDF_Attribute)& afterAtt,
                       const Handle(TDF_Attribute)& oldAtt);

  TDF_LabelNode* RootNode ();

  const TDF_LabelNode* RootNode () const;

  Standard_EXPORT void AllMayBeModified();

  // Tag modification
  inline void Tag(const Standard_Integer aTag)
    { myTag = aTag; };

  // Depth modification
  inline void Depth(const Standard_Integer aDepth)
    { myFlags = ((myFlags & TDF_LabelNodeFlagsMsk) | aDepth); };

  // Flag Imported access
  inline void Imported(const Standard_Boolean aStatus)
    { myFlags = (aStatus) ?
        (myFlags | TDF_LabelNodeImportMsk) :
          (myFlags & ~TDF_LabelNodeImportMsk); };

  inline Standard_Boolean IsImported() const
    { return ((myFlags & TDF_LabelNodeImportMsk) != 0); };

  // Private Fields
  // --------------------------------------------------------------------------

  TDF_LabelNodePtr       myFather; 
  TDF_LabelNodePtr       myBrother; 
  TDF_LabelNodePtr       myFirstChild;
  TDF_LabelNodePtr       myLastFoundChild; //jfa 10.01.2003
  Standard_Integer       myTag;
  Standard_Integer       myFlags; // Flags & Depth
  Handle(TDF_Attribute)  myFirstAttribute;
#ifdef KEEP_LOCAL_ROOT
  TDF_Data *             myData;
#endif
#ifdef DEB
  TCollection_AsciiString myDebugEntry;
#endif
};

#endif
