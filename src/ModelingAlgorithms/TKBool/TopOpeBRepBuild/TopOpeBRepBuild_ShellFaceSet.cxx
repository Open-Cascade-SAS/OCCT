// Created on: 1993-06-17
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepBuild_define.hxx>
#include <TopOpeBRepBuild_ShellFaceSet.hxx>

#ifdef OCCT_DEBUG
extern bool TopOpeBRepBuild_GettraceCHK();
#endif

//=================================================================================================

TopOpeBRepBuild_ShellFaceSet::TopOpeBRepBuild_ShellFaceSet()
    : TopOpeBRepBuild_ShapeSet(TopAbs_EDGE)
{
#ifdef OCCT_DEBUG
  myDEBName = "SFS";
#endif
}

//=================================================================================================

TopOpeBRepBuild_ShellFaceSet::TopOpeBRepBuild_ShellFaceSet
#ifdef OCCT_DEBUG
  (const TopoDS_Shape& S, void* const A)
    : // DEB
#else
  (const TopoDS_Shape& S, void* const)
    : // DEB
#endif
      TopOpeBRepBuild_ShapeSet(TopAbs_EDGE)
{
  mySolid = TopoDS::Solid(S);

#ifdef OCCT_DEBUG
  myDEBName = "SFS";
  if (A != NULL)
  {
    TopOpeBRepBuild_Builder* pB = ((TopOpeBRepBuild_Builder*)((void*)A));
    myDEBNumber                 = pB->GdumpSHASETindex();
    int  iS;
    bool tSPS = pB->GtraceSPS(S, iS);
    if (tSPS)
    {
      std::cout << "creation SFS " << myDEBNumber << " on ";
    }
    if (tSPS)
    {
      pB->GdumpSHA(S, NULL);
      std::cout << std::endl;
    }
  }

  if (TopOpeBRepBuild_GettraceCHK() && !myCheckShape)
  {
    DumpName(std::cout, "no checkshape in creation of ");
    std::cout << std::endl;
  }
#endif
}

//=================================================================================================

void TopOpeBRepBuild_ShellFaceSet::AddShape(const TopoDS_Shape& S)
{
  TopOpeBRepBuild_ShapeSet::AddShape(S);
}

//=================================================================================================

void TopOpeBRepBuild_ShellFaceSet::AddStartElement(const TopoDS_Shape& S)
{
  TopOpeBRepBuild_ShapeSet::ProcessAddStartElement(S);
}

//=================================================================================================

void TopOpeBRepBuild_ShellFaceSet::AddElement(const TopoDS_Shape& S)
{
  TopOpeBRepBuild_ShapeSet::AddElement(S);
}

//=================================================================================================

const TopoDS_Solid& TopOpeBRepBuild_ShellFaceSet::Solid() const
{
  return mySolid;
}

//=================================================================================================

void TopOpeBRepBuild_ShellFaceSet::DumpSS()
{
#ifdef OCCT_DEBUG
  TopOpeBRepBuild_ShapeSet::DumpSS();
#endif
}

//=================================================================================================

TCollection_AsciiString TopOpeBRepBuild_ShellFaceSet::SName(const TopoDS_Shape&,
                                                            const TCollection_AsciiString& sb,
                                                            const TCollection_AsciiString&) const
{
  TCollection_AsciiString str = sb;
  return str;
}

//=================================================================================================

TCollection_AsciiString TopOpeBRepBuild_ShellFaceSet::SNameori(const TopoDS_Shape&,
                                                               const TCollection_AsciiString& sb,
                                                               const TCollection_AsciiString&) const
{
  TCollection_AsciiString str = sb;
  return str;
}

//=================================================================================================

TCollection_AsciiString TopOpeBRepBuild_ShellFaceSet::SName(const NCollection_List<TopoDS_Shape>&,
                                                            const TCollection_AsciiString&,
                                                            const TCollection_AsciiString&) const
{
  TCollection_AsciiString str;
  return str;
}

//=================================================================================================

TCollection_AsciiString TopOpeBRepBuild_ShellFaceSet::SNameori(
  const NCollection_List<TopoDS_Shape>& /*L*/,
  const TCollection_AsciiString& /*sb*/,
  const TCollection_AsciiString& /*sa*/) const
{
  TCollection_AsciiString str;
  return str;
}
