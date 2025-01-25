// Author: Kirill Gavrilov
// Copyright (c) 2017-2019 OPEN CASCADE SAS
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

#ifndef _XCAFPrs_DocumentNode_HeaderFile
#define _XCAFPrs_DocumentNode_HeaderFile

#include <XCAFPrs_Style.hxx>

#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <TopLoc_Location.hxx>

//! Structure defining document node.
struct XCAFPrs_DocumentNode
{
  TCollection_AsciiString Id;         //!< string identifier
  TDF_Label               Label;      //!< label in the document
  TDF_Label               RefLabel;   //!< reference label in the document
  XCAFPrs_Style           Style;      //!< node style
  TopLoc_Location         Location;   //!< node global transformation
  TopLoc_Location         LocalTrsf;  //!< node transformation relative to parent
  TDF_ChildIterator       ChildIter;  //!< child iterator
  Standard_Boolean        IsAssembly; //!< flag indicating that this label is assembly

  XCAFPrs_DocumentNode()
      : IsAssembly(Standard_False)
  {
  }

public: // Methods for hash map
  bool operator==(const XCAFPrs_DocumentNode& theOther) const { return Id == theOther.Id; }
};

namespace std
{
template <>
struct hash<XCAFPrs_DocumentNode>
{
  size_t operator()(const XCAFPrs_DocumentNode& theDocumentNode) const
  {
    return std::hash<TCollection_AsciiString>{}(theDocumentNode.Id);
  }
};
} // namespace std

#endif // _XCAFPrs_DocumentNode_HeaderFile
