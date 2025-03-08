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

#ifdef DRAW
  #include <DBRep.hxx>
static TCollection_AsciiString PRODINS("dins ");
#endif

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepBuild_define.hxx>
#include <TopOpeBRepBuild_ShellFaceSet.hxx>

#ifdef OCCT_DEBUG
extern Standard_Boolean TopOpeBRepBuild_GettraceCHK();
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
  (const TopoDS_Shape& S, const Standard_Address A)
    : // DEB
#else
  (const TopoDS_Shape& S, const Standard_Address)
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
    Standard_Integer iS;
    Standard_Boolean tSPS = pB->GtraceSPS(S, iS);
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

#ifdef DRAW
TCollection_AsciiString TopOpeBRepBuild_ShellFaceSet::SName(const TopoDS_Shape&            S,
                                                            const TCollection_AsciiString& sb,
                                                            const TCollection_AsciiString& sa) const
{
  TCollection_AsciiString str = sb;

  str = str + TopOpeBRepBuild_ShapeSet::SName(S);
  str = str + sa;
  DBRep::Set(str.ToCString(), S);

  return str;
}
#else
TCollection_AsciiString TopOpeBRepBuild_ShellFaceSet::SName(const TopoDS_Shape&,
                                                            const TCollection_AsciiString& sb,
                                                            const TCollection_AsciiString&) const
{
  TCollection_AsciiString str = sb;
  return str;
}
#endif

//=================================================================================================

#ifdef DRAW
TCollection_AsciiString TopOpeBRepBuild_ShellFaceSet::SNameori(
  const TopoDS_Shape&            S,
  const TCollection_AsciiString& sb,
  const TCollection_AsciiString& sa) const
#else
TCollection_AsciiString TopOpeBRepBuild_ShellFaceSet::SNameori(const TopoDS_Shape&,
                                                               const TCollection_AsciiString& sb,
                                                               const TCollection_AsciiString&) const
#endif
{
  TCollection_AsciiString str = sb;
#ifdef DRAW
  str = str + TopOpeBRepBuild_ShapeSet::SNameori(S);
  if (S.ShapeType() == TopAbs_FACE)
  {
    const TopoDS_Shape& F = TopoDS::Face(S);
    DBRep::Set(str.ToCString(), S);
  }
#endif
  return str;
}

//=================================================================================================

#ifdef DRAW
TCollection_AsciiString TopOpeBRepBuild_ShellFaceSet::SName(const TopTools_ListOfShape&    L,
                                                            const TCollection_AsciiString& sb,
                                                            const TCollection_AsciiString& sa) const
{
  TCollection_AsciiString str;

  for (TopTools_ListIteratorOfListOfShape it(L); it.More(); it.Next())
    str = str + sb + SName(it.Value()) + sa + " ";

  return str;
}
#else
TCollection_AsciiString TopOpeBRepBuild_ShellFaceSet::SName(const TopTools_ListOfShape&,
                                                            const TCollection_AsciiString&,
                                                            const TCollection_AsciiString&) const
{
  TCollection_AsciiString str;
  return str;
}
#endif

//=================================================================================================

TCollection_AsciiString TopOpeBRepBuild_ShellFaceSet::SNameori(
  const TopTools_ListOfShape& /*L*/,
  const TCollection_AsciiString& /*sb*/,
  const TCollection_AsciiString& /*sa*/) const
{
  TCollection_AsciiString str;
#ifdef DRAW
  for (TopTools_ListIteratorOfListOfShape it(L); it.More(); it.Next())
    str = str + sb + SNameori(it.Value()) + sa + " ";
#endif
  return str;
}
