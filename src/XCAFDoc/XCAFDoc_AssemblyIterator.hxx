// Created on: 2022-05-11
// Copyright (c) 2022 OPEN CASCADE SAS
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

#ifndef _XCAFDoc_AssemblyIterator_HeaderFile
#define _XCAFDoc_AssemblyIterator_HeaderFile

#include <NCollection_Sequence.hxx>
#include <XCAFDoc_AssemblyItemId.hxx>

class TDF_Label;
class TDocStd_Document;
class XCAFDoc_ShapeTool;

//! Iterator in depth along the assembly tree.
class XCAFDoc_AssemblyIterator
{
public:
  //! Constructs iterator starting from assembly roots.
  //! \param[in]       theDoc   - document to iterate.
  //! \param [in, opt] theLevel - max level of hierarchy to reach (INT_MAX is for no limit).
  Standard_EXPORT XCAFDoc_AssemblyIterator(const Handle(TDocStd_Document)& theDoc,
                                           const Standard_Integer          theLevel = INT_MAX);

  //! Constructs iterator starting from the specified position in the assembly tree.
  //! \param[in]       theDoc   - document to iterate.
  //! \param[in]       theRoot  - assembly item to start iterating from.
  //! \param [in, opt] theLevel - max level of hierarchy to reach (INT_MAX is for no limit).
  Standard_EXPORT XCAFDoc_AssemblyIterator(const Handle(TDocStd_Document)& theDoc,
                                           const XCAFDoc_AssemblyItemId&   theRoot,
                                           const Standard_Integer          theLevel = INT_MAX);

  //! \return true if there is still something to iterate, false -- otherwise.
  Standard_EXPORT Standard_Boolean More() const;

  //! Moves depth-first iterator to the next position.
  Standard_EXPORT void Next();

  //! \return current item.
  Standard_EXPORT XCAFDoc_AssemblyItemId Current() const;

private:
  struct AuxAssemblyItem
  {
    TDF_Label              myLabel;
    XCAFDoc_AssemblyItemId myItem;
  };

  void createItem(const TDF_Label&                 theLabel,
                  const TColStd_ListOfAsciiString& theParentPath,
                  AuxAssemblyItem&                 theAuxItem) const;

private:
  Handle(XCAFDoc_ShapeTool)             myShapeTool; //!< Document shape tool.
  NCollection_Sequence<AuxAssemblyItem> myFringe;    //!< Items pending for iteration.
  Standard_Integer                      myMaxLevel;  //!< Limit on max depth of iteration.
  Standard_Integer                      mySeedLevel; //!< Level of hierarchy where we start.
};

#endif // _XCAFDoc_AssemblyIterator_HeaderFile
