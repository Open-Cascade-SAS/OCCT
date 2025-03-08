// Created on: 1992-09-22
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _MAT_TListNodeOfListOfBisector_HeaderFile
#define _MAT_TListNodeOfListOfBisector_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class MAT_Bisector;
class MAT_ListOfBisector;

class MAT_TListNodeOfListOfBisector;
DEFINE_STANDARD_HANDLE(MAT_TListNodeOfListOfBisector, Standard_Transient)

class MAT_TListNodeOfListOfBisector : public Standard_Transient
{

public:
  MAT_TListNodeOfListOfBisector();

  MAT_TListNodeOfListOfBisector(const Handle(MAT_Bisector)& anitem);

  Handle(MAT_Bisector) GetItem() const;

  Handle(MAT_TListNodeOfListOfBisector) Next() const;

  Handle(MAT_TListNodeOfListOfBisector) Previous() const;

  void SetItem(const Handle(MAT_Bisector)& anitem);

  void Next(const Handle(MAT_TListNodeOfListOfBisector)& atlistnode);

  void Previous(const Handle(MAT_TListNodeOfListOfBisector)& atlistnode);

  Standard_EXPORT void Dummy() const;

  DEFINE_STANDARD_RTTI_INLINE(MAT_TListNodeOfListOfBisector, Standard_Transient)

protected:
private:
  Handle(MAT_TListNodeOfListOfBisector) thenext;
  Handle(MAT_TListNodeOfListOfBisector) theprevious;
  Handle(MAT_Bisector)                  theitem;
};

#define Item Handle(MAT_Bisector)
#define Item_hxx <MAT_Bisector.hxx>
#define MAT_TListNode MAT_TListNodeOfListOfBisector
#define MAT_TListNode_hxx <MAT_TListNodeOfListOfBisector.hxx>
#define Handle_MAT_TListNode Handle(MAT_TListNodeOfListOfBisector)
#define MAT_TList MAT_ListOfBisector
#define MAT_TList_hxx <MAT_ListOfBisector.hxx>
#define Handle_MAT_TList Handle(MAT_ListOfBisector)

#include <MAT_TListNode.lxx>

#undef Item
#undef Item_hxx
#undef MAT_TListNode
#undef MAT_TListNode_hxx
#undef Handle_MAT_TListNode
#undef MAT_TList
#undef MAT_TList_hxx
#undef Handle_MAT_TList

#endif // _MAT_TListNodeOfListOfBisector_HeaderFile
