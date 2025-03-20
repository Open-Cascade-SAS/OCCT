// Created on: 2000-01-26
// Created by: Denis PASCAL
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _TDataStd_ChildNodeIterator_HeaderFile
#define _TDataStd_ChildNodeIterator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class TDataStd_TreeNode;

//! Iterates on the  ChildStepren step of  a step, at the
//! first level  only.   It  is possible  to ask  the
//! iterator to explore all the sub step levels of the
//! given one, with the option "allLevels".
class TDataStd_ChildNodeIterator
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an empty iterator.
  Standard_EXPORT TDataStd_ChildNodeIterator();

  //! Iterates on the ChildStepren of the given Step. If
  //! <allLevels> option is set to true, it explores not
  //! only the first, but all the sub Step levels.
  Standard_EXPORT TDataStd_ChildNodeIterator(const Handle(TDataStd_TreeNode)& aTreeNode,
                                             const Standard_Boolean allLevels = Standard_False);

  //! Initializes the iteration on  the Children Step of
  //! the  given Step. If <allLevels>  option is  set to
  //! true, it explores not  only the first, but all the
  //! sub Step levels.
  Standard_EXPORT void Initialize(const Handle(TDataStd_TreeNode)& aTreeNode,
                                  const Standard_Boolean           allLevels = Standard_False);

  //! Returns True if there is a current Item in the
  //! iteration.
  Standard_Boolean More() const { return !myNode.IsNull(); }

  //! Move to the next Item
  Standard_EXPORT void Next();

  //! Move to the next Brother. If there is none, go up
  //! etc. This method is interesting only with
  //! "allLevels" behavior, because it avoids to explore
  //! the current Step ChildStepren.
  Standard_EXPORT void NextBrother();

  //! Returns the current item; a null Step if there is
  //! no one.
  const Handle(TDataStd_TreeNode)& Value() const { return myNode; }

private:
  Handle(TDataStd_TreeNode) myNode;
  Standard_Integer          myFirstLevel;
};

#endif // _TDataStd_ChildNodeIterator_HeaderFile
