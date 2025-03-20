// Created on: 1991-04-24
// Created by: Arnaud BOUZY
// Copyright (c) 1991-1999 Matra Datavision
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

#include <Draw_Drawable3D.hxx>

#include <NCollection_DataMap.hxx>
#include <Draw_Display.hxx>
#include <Standard_CStringHasher.hxx>
#include <Standard_NotImplemented.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Draw_Drawable3D, Standard_Transient)

//! Return the map of factory functions.
static NCollection_DataMap<Standard_CString,
                           Draw_Drawable3D::FactoryFunction_t,
                           Standard_CStringHasher>&
  getFactoryMap()
{
  static NCollection_DataMap<Standard_CString,
                             Draw_Drawable3D::FactoryFunction_t,
                             Standard_CStringHasher>
    myToolMap;
  return myToolMap;
}

//=================================================================================================

void Draw_Drawable3D::RegisterFactory(const Standard_CString   theType,
                                      const FactoryFunction_t& theFactory)
{
  getFactoryMap().Bind(theType, theFactory);
}

//=================================================================================================

Handle(Draw_Drawable3D) Draw_Drawable3D::Restore(const Standard_CString theType,
                                                 Standard_IStream&      theStream)
{
  FactoryFunction_t aFactory = NULL;
  if (getFactoryMap().Find(theType, aFactory))
  {
    return aFactory(theStream);
  }
  return Handle(Draw_Drawable3D)();
}

//=================================================================================================

Draw_Drawable3D::Draw_Drawable3D()
    : myXmin(0.0),
      myXmax(0.0),
      myYmin(0.0),
      myYmax(0.0),
      myName(NULL),
      isVisible(Standard_False),
      isProtected(Standard_False)
{
}

//=================================================================================================

Standard_Boolean Draw_Drawable3D::PickReject(const Standard_Real X,
                                             const Standard_Real Y,
                                             const Standard_Real Prec) const
{
  return ((X + Prec < myXmin) || (X - Prec > myXmax) || (Y + Prec < myYmin) || (Y - Prec > myYmax));
}

//=================================================================================================

Handle(Draw_Drawable3D) Draw_Drawable3D::Copy() const
{
  return this;
}

//=================================================================================================

void Draw_Drawable3D::Dump(Standard_OStream& S) const
{
  S << myXmin << " " << myXmax << "\n";
  S << myYmin << " " << myYmax << "\n";
}

//=================================================================================================

void Draw_Drawable3D::Save(Standard_OStream&) const
{
  throw Standard_NotImplemented("Draw_Drawable3D::Save() should be redefined in sub-class");
}

//=================================================================================================

void Draw_Drawable3D::Whatis(Draw_Interpretor& S) const
{
  S << "drawable 3d";
}
