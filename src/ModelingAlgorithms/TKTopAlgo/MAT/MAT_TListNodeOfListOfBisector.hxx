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

class MAT_TListNodeOfListOfBisector : public Standard_Transient
{

public:
  MAT_TListNodeOfListOfBisector();

  MAT_TListNodeOfListOfBisector(const occ::handle<MAT_Bisector>& anitem);

  occ::handle<MAT_Bisector> GetItem() const;

  occ::handle<MAT_TListNodeOfListOfBisector> Next() const;

  occ::handle<MAT_TListNodeOfListOfBisector> Previous() const;

  void SetItem(const occ::handle<MAT_Bisector>& anitem);

  void Next(const occ::handle<MAT_TListNodeOfListOfBisector>& atlistnode);

  void Previous(const occ::handle<MAT_TListNodeOfListOfBisector>& atlistnode);

  Standard_EXPORT void Dummy() const;

  DEFINE_STANDARD_RTTI_INLINE(MAT_TListNodeOfListOfBisector, Standard_Transient)

private:
  occ::handle<MAT_TListNodeOfListOfBisector> thenext;
  occ::handle<MAT_TListNodeOfListOfBisector> theprevious;
  occ::handle<MAT_Bisector>                  theitem;
};

//=================================================================================================
// Inline implementations
//=================================================================================================

inline MAT_TListNodeOfListOfBisector::MAT_TListNodeOfListOfBisector() = default;

//=================================================================================================

inline MAT_TListNodeOfListOfBisector::MAT_TListNodeOfListOfBisector(
  const occ::handle<MAT_Bisector>& anitem)
{
  theitem = anitem;
}

//=================================================================================================

inline occ::handle<MAT_Bisector> MAT_TListNodeOfListOfBisector::GetItem() const
{
  return theitem;
}

//=================================================================================================

inline occ::handle<MAT_TListNodeOfListOfBisector> MAT_TListNodeOfListOfBisector::Next() const
{
  return thenext;
}

//=================================================================================================

inline occ::handle<MAT_TListNodeOfListOfBisector> MAT_TListNodeOfListOfBisector::Previous() const
{
  return theprevious;
}

//=================================================================================================

inline void MAT_TListNodeOfListOfBisector::SetItem(const occ::handle<MAT_Bisector>& anitem)
{
  theitem = anitem;
}

//=================================================================================================

inline void MAT_TListNodeOfListOfBisector::Next(
  const occ::handle<MAT_TListNodeOfListOfBisector>& atlistnode)
{
  thenext = atlistnode;
}

//=================================================================================================

inline void MAT_TListNodeOfListOfBisector::Previous(
  const occ::handle<MAT_TListNodeOfListOfBisector>& atlistnode)
{
  theprevious = atlistnode;
}

#endif // _MAT_TListNodeOfListOfBisector_HeaderFile
