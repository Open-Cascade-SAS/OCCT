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

#ifndef _MAT_ListOfEdge_HeaderFile
#define _MAT_ListOfEdge_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
class MAT_TListNodeOfListOfEdge;
class MAT_Edge;

class MAT_ListOfEdge : public Standard_Transient
{

public:
  Standard_EXPORT MAT_ListOfEdge();

  Standard_EXPORT ~MAT_ListOfEdge() override;

  Standard_EXPORT void First();

  Standard_EXPORT void Last();

  Standard_EXPORT void Init(const occ::handle<MAT_Edge>& aniten);

  Standard_EXPORT void Next();

  Standard_EXPORT void Previous();

  Standard_EXPORT bool More() const;

  Standard_EXPORT occ::handle<MAT_Edge> Current() const;

  Standard_EXPORT void Current(const occ::handle<MAT_Edge>& anitem) const;

  Standard_EXPORT occ::handle<MAT_Edge> FirstItem() const;

  Standard_EXPORT occ::handle<MAT_Edge> LastItem() const;

  Standard_EXPORT occ::handle<MAT_Edge> PreviousItem() const;

  Standard_EXPORT occ::handle<MAT_Edge> NextItem() const;

  int Number() const;

  int Index() const;

  Standard_EXPORT occ::handle<MAT_Edge> Brackets(const int anindex);

  occ::handle<MAT_Edge> operator()(const int anindex) { return Brackets(anindex); }

  Standard_EXPORT void Unlink();

  Standard_EXPORT void LinkBefore(const occ::handle<MAT_Edge>& anitem);

  Standard_EXPORT void LinkAfter(const occ::handle<MAT_Edge>& anitem);

  Standard_EXPORT void FrontAdd(const occ::handle<MAT_Edge>& anitem);

  Standard_EXPORT void BackAdd(const occ::handle<MAT_Edge>& anitem);

  Standard_EXPORT void Permute();

  Standard_EXPORT void Loop() const;

  bool IsEmpty() const;

  Standard_EXPORT void Dump(const int ashift, const int alevel);

  DEFINE_STANDARD_RTTI_INLINE(MAT_ListOfEdge, Standard_Transient)

private:
  occ::handle<MAT_TListNodeOfListOfEdge> thefirstnode;
  occ::handle<MAT_TListNodeOfListOfEdge> thelastnode;
  occ::handle<MAT_TListNodeOfListOfEdge> thecurrentnode;
  int                                    thecurrentindex;
  int                                    thenumberofitems;
};

#define Item occ::handle<MAT_Edge>
#define Item_hxx <MAT_Edge.hxx>
#define MAT_TListNode MAT_TListNodeOfListOfEdge
#define MAT_TListNode_hxx <MAT_TListNodeOfListOfEdge.hxx>
#define Handle_MAT_TListNode occ::handle<MAT_TListNodeOfListOfEdge>
#define MAT_TList MAT_ListOfEdge
#define MAT_TList_hxx <MAT_ListOfEdge.hxx>
#define Handle_MAT_TList occ::handle<MAT_ListOfEdge>

#include <MAT_TList.lxx>

#undef Item
#undef Item_hxx
#undef MAT_TListNode
#undef MAT_TListNode_hxx
#undef Handle_MAT_TListNode
#undef MAT_TList
#undef MAT_TList_hxx
#undef Handle_MAT_TList

#endif // _MAT_ListOfEdge_HeaderFile
