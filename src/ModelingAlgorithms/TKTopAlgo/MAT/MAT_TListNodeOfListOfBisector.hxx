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

//=================================================================================================
// Inline implementations
//=================================================================================================

inline MAT_TListNodeOfListOfBisector::MAT_TListNodeOfListOfBisector() {}

//=================================================================================================

inline MAT_TListNodeOfListOfBisector::MAT_TListNodeOfListOfBisector(
  const Handle(MAT_Bisector)& anitem)
{
  theitem = anitem;
}

//=================================================================================================

inline Handle(MAT_Bisector) MAT_TListNodeOfListOfBisector::GetItem() const
{
  return theitem;
}

//=================================================================================================

inline Handle(MAT_TListNodeOfListOfBisector) MAT_TListNodeOfListOfBisector::Next() const
{
  return thenext;
}

//=================================================================================================

inline Handle(MAT_TListNodeOfListOfBisector) MAT_TListNodeOfListOfBisector::Previous() const
{
  return theprevious;
}

//=================================================================================================

inline void MAT_TListNodeOfListOfBisector::SetItem(const Handle(MAT_Bisector)& anitem)
{
  theitem = anitem;
}

//=================================================================================================

inline void MAT_TListNodeOfListOfBisector::Next(
  const Handle(MAT_TListNodeOfListOfBisector)& atlistnode)
{
  thenext = atlistnode;
}

//=================================================================================================

inline void MAT_TListNodeOfListOfBisector::Previous(
  const Handle(MAT_TListNodeOfListOfBisector)& atlistnode)
{
  theprevious = atlistnode;
}

#endif // _MAT_TListNodeOfListOfBisector_HeaderFile
