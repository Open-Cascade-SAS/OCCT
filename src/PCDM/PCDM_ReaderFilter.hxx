// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef _PCDM_ReaderFilter_HeaderFile
#define _PCDM_ReaderFilter_HeaderFile

#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_List.hxx>

class PCDM_ReaderFilter;
DEFINE_STANDARD_HANDLE(PCDM_ReaderFilter, Standard_Transient)

//! Class represents a document reading filter.
//!
//! It allows to set attributes (by class names) that must be skipped during the document reading
//! or attributes that must be retrieved only.
//! In addition it is possible to define one or several subtrees (by entry) which must be
//! retrieved during the reading. Other labels are created, but no one attribute on them.
class PCDM_ReaderFilter : public Standard_Transient
{
public:
  //! Supported modes of appending the file content into existing document
  enum AppendMode
  {
    AppendMode_Forbid    = 0, //!< do not allow append, default mode
    AppendMode_Protect   = 1, //!< keeps existing attributes, reads only new ones
    AppendMode_Overwrite = 2, //!< overwrites the existing attributes by the loaded ones
  };

  //! Creates an empty filter, so, all will be retrieved if nothing else is defined.
  inline PCDM_ReaderFilter()
      : myAppend(AppendMode_Forbid)
  {
  }

  //! Creates a filter to skip only one type of attributes.
  Standard_EXPORT PCDM_ReaderFilter(const Handle(Standard_Type)& theSkipped);

  //! Creates a filter to read only sub-labels of a label-path.
  //! Like, for "0:2" it will read all attributes for labels "0:2", "0:2:1", etc.
  Standard_EXPORT PCDM_ReaderFilter(const TCollection_AsciiString& theEntryToRead);

  //! Creates a filter to append the content of file to open to existing document.
  Standard_EXPORT PCDM_ReaderFilter(const AppendMode theAppend);

  //! Destructor for the filter content
  Standard_EXPORT ~PCDM_ReaderFilter();

  //! Adds skipped attribute by type.
  Standard_EXPORT void AddSkipped(const Handle(Standard_Type)& theSkipped)
  {
    mySkip.Add(theSkipped->Name());
  }

  //! Adds skipped attribute by type name.
  Standard_EXPORT void AddSkipped(const TCollection_AsciiString& theSkipped)
  {
    mySkip.Add(theSkipped);
  }

  //! Adds attribute to read by type. Disables the skipped attributes added.
  Standard_EXPORT void AddRead(const Handle(Standard_Type)& theRead)
  {
    myRead.Add(theRead->Name());
  }

  //! Adds attribute to read by type name. Disables the skipped attributes added.
  Standard_EXPORT void AddRead(const TCollection_AsciiString& theRead) { myRead.Add(theRead); }

  //! Adds sub-tree path (like "0:2").
  Standard_EXPORT void AddPath(const TCollection_AsciiString& theEntryToRead)
  {
    mySubTrees.Append(theEntryToRead);
  }

  //! Makes filter pass all data.
  Standard_EXPORT void Clear();

  //! Returns true if attribute must be read.
  Standard_EXPORT virtual Standard_Boolean IsPassed(
    const Handle(Standard_Type)& theAttributeID) const;
  //! Returns true if attribute must be read.
  Standard_EXPORT virtual Standard_Boolean IsPassedAttr(
    const TCollection_AsciiString& theAttributeType) const;
  //! Returns true if content of the label must be read.
  Standard_EXPORT virtual Standard_Boolean IsPassed(const TCollection_AsciiString& theEntry) const;
  //! Returns true if some sub-label of the given label is passed.
  Standard_EXPORT virtual Standard_Boolean IsSubPassed(
    const TCollection_AsciiString& theEntry) const;
  //! Returns true if only part of the document tree will be retrieved.
  Standard_EXPORT virtual Standard_Boolean IsPartTree();

  //! Returns the append mode.
  Standard_EXPORT AppendMode& Mode() { return myAppend; }

  //! Returns true if appending to the document is performed.
  Standard_EXPORT Standard_Boolean IsAppendMode()
  {
    return myAppend != PCDM_ReaderFilter::AppendMode_Forbid;
  }

  //! Starts the tree iterator. It is used for fast searching of passed labels if the whole tree of
  //! labels is parsed. So, on each iteration step the methods Up and Down must be called after the
  //! iteration start.
  Standard_EXPORT virtual void StartIteration();
  //! Iteration to the child label.
  Standard_EXPORT virtual void Up();
  //! Iteration to the child with defined tag.
  Standard_EXPORT virtual void Down(const int& theTag);
  //! Returns true if content of the currently iterated label must be read.
  Standard_EXPORT virtual Standard_Boolean IsPassed() const;
  //! Returns true if some sub-label of the currently iterated label is passed.
  Standard_EXPORT virtual Standard_Boolean IsSubPassed() const;

  DEFINE_STANDARD_RTTIEXT(PCDM_ReaderFilter, Standard_Transient)

private:
  //! Clears the iteration tree
  Standard_EXPORT void ClearTree();
  //! Clears the iteration sub-tree
  Standard_EXPORT static void ClearSubTree(const Standard_Address theMap);

protected:
  //! Append mode for reading files into existing document
  AppendMode myAppend;
  //! Class names of attributes that must be skipped during the read
  NCollection_Map<TCollection_AsciiString> mySkip;
  //! Class names of only attributes to read (if it is not empty, mySkip is unused)
  NCollection_Map<TCollection_AsciiString> myRead;
  //! Paths to the labels that must be read. If it is empty, read all.
  NCollection_List<TCollection_AsciiString> mySubTrees;

  //! Map from tag of a label to sub-tree of this tag. Used for fast browsing the tree
  //! and compare with entities that must be read.
  typedef NCollection_DataMap<Standard_Integer, Standard_Address> TagTree;
  //! Whole tree that correspond to retrieved document.
  TagTree myTree;
  //! Pointer to the current node of the iterator.
  TagTree* myCurrent;
  //! If a node does not described in the read-entries, the iterator goes inside of this subtree
  //! just by keeping the depth of iteration.
  Standard_Integer myCurrentDepth;
};

#endif // _PCDM_ReaderFilter_HeaderFile
