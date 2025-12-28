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

#ifndef _MAT_TListNodeOfListOfEdge_HeaderFile
#define _MAT_TListNodeOfListOfEdge_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class MAT_Edge;
class MAT_ListOfEdge;

class MAT_TListNodeOfListOfEdge : public Standard_Transient
{

public:
  MAT_TListNodeOfListOfEdge();

  MAT_TListNodeOfListOfEdge(const occ::handle<MAT_Edge>& anitem);

  occ::handle<MAT_Edge> GetItem() const;

  occ::handle<MAT_TListNodeOfListOfEdge> Next() const;

  occ::handle<MAT_TListNodeOfListOfEdge> Previous() const;

  void SetItem(const occ::handle<MAT_Edge>& anitem);

  void Next(const occ::handle<MAT_TListNodeOfListOfEdge>& atlistnode);

  void Previous(const occ::handle<MAT_TListNodeOfListOfEdge>& atlistnode);

  Standard_EXPORT void Dummy() const;

  DEFINE_STANDARD_RTTI_INLINE(MAT_TListNodeOfListOfEdge, Standard_Transient)

private:
  occ::handle<MAT_TListNodeOfListOfEdge> thenext;
  occ::handle<MAT_TListNodeOfListOfEdge> theprevious;
  occ::handle<MAT_Edge>                  theitem;
};

//=================================================================================================
// Inline implementations
//=================================================================================================

inline MAT_TListNodeOfListOfEdge::MAT_TListNodeOfListOfEdge() {}

//=================================================================================================

inline MAT_TListNodeOfListOfEdge::MAT_TListNodeOfListOfEdge(const occ::handle<MAT_Edge>& anitem)
{
  theitem = anitem;
}

//=================================================================================================

inline occ::handle<MAT_Edge> MAT_TListNodeOfListOfEdge::GetItem() const
{
  return theitem;
}

//=================================================================================================

inline occ::handle<MAT_TListNodeOfListOfEdge> MAT_TListNodeOfListOfEdge::Next() const
{
  return thenext;
}

//=================================================================================================

inline occ::handle<MAT_TListNodeOfListOfEdge> MAT_TListNodeOfListOfEdge::Previous() const
{
  return theprevious;
}

//=================================================================================================

inline void MAT_TListNodeOfListOfEdge::SetItem(const occ::handle<MAT_Edge>& anitem)
{
  theitem = anitem;
}

//=================================================================================================

inline void MAT_TListNodeOfListOfEdge::Next(const occ::handle<MAT_TListNodeOfListOfEdge>& atlistnode)
{
  thenext = atlistnode;
}

//=================================================================================================

inline void MAT_TListNodeOfListOfEdge::Previous(const occ::handle<MAT_TListNodeOfListOfEdge>& atlistnode)
{
  theprevious = atlistnode;
}

#endif // _MAT_TListNodeOfListOfEdge_HeaderFile
