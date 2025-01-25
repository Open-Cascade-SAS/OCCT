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

class MAT_ListOfEdge;
DEFINE_STANDARD_HANDLE(MAT_ListOfEdge, Standard_Transient)

class MAT_ListOfEdge : public Standard_Transient
{

public:
  Standard_EXPORT MAT_ListOfEdge();

  Standard_EXPORT ~MAT_ListOfEdge();

  Standard_EXPORT void First();

  Standard_EXPORT void Last();

  Standard_EXPORT void Init(const Handle(MAT_Edge)& aniten);

  Standard_EXPORT void Next();

  Standard_EXPORT void Previous();

  Standard_EXPORT Standard_Boolean More() const;

  Standard_EXPORT Handle(MAT_Edge) Current() const;

  Standard_EXPORT void Current(const Handle(MAT_Edge)& anitem) const;

  Standard_EXPORT Handle(MAT_Edge) FirstItem() const;

  Standard_EXPORT Handle(MAT_Edge) LastItem() const;

  Standard_EXPORT Handle(MAT_Edge) PreviousItem() const;

  Standard_EXPORT Handle(MAT_Edge) NextItem() const;

  Standard_Integer Number() const;

  Standard_Integer Index() const;

  Standard_EXPORT Handle(MAT_Edge) Brackets(const Standard_Integer anindex);

  Handle(MAT_Edge) operator()(const Standard_Integer anindex) { return Brackets(anindex); }

  Standard_EXPORT void Unlink();

  Standard_EXPORT void LinkBefore(const Handle(MAT_Edge)& anitem);

  Standard_EXPORT void LinkAfter(const Handle(MAT_Edge)& anitem);

  Standard_EXPORT void FrontAdd(const Handle(MAT_Edge)& anitem);

  Standard_EXPORT void BackAdd(const Handle(MAT_Edge)& anitem);

  Standard_EXPORT void Permute();

  Standard_EXPORT void Loop() const;

  Standard_Boolean IsEmpty() const;

  Standard_EXPORT void Dump(const Standard_Integer ashift, const Standard_Integer alevel);

  DEFINE_STANDARD_RTTI_INLINE(MAT_ListOfEdge, Standard_Transient)

protected:
private:
  Handle(MAT_TListNodeOfListOfEdge) thefirstnode;
  Handle(MAT_TListNodeOfListOfEdge) thelastnode;
  Handle(MAT_TListNodeOfListOfEdge) thecurrentnode;
  Standard_Integer                  thecurrentindex;
  Standard_Integer                  thenumberofitems;
};

#define Item Handle(MAT_Edge)
#define Item_hxx <MAT_Edge.hxx>
#define MAT_TListNode MAT_TListNodeOfListOfEdge
#define MAT_TListNode_hxx <MAT_TListNodeOfListOfEdge.hxx>
#define Handle_MAT_TListNode Handle(MAT_TListNodeOfListOfEdge)
#define MAT_TList MAT_ListOfEdge
#define MAT_TList_hxx <MAT_ListOfEdge.hxx>
#define Handle_MAT_TList Handle(MAT_ListOfEdge)

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
