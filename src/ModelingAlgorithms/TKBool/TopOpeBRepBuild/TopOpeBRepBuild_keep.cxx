// Created on: 1996-03-07
// Created by: Jean Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
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

#include <TopOpeBRepBuild_GTopo.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepBuild_PaveSet.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>

#ifdef OCCT_DEBUG
extern Standard_Integer GLOBAL_iexE;
extern Standard_Integer GLOBAL_iexF;
Standard_Boolean        STATIC_trace_iexE = Standard_False;
Standard_Boolean        STATIC_trace_iexF = Standard_False;

Standard_EXPORT void debkeep(const Standard_Integer i)
{
  std::cout << "++ debkeep " << i << std::endl;
}
#endif

//=======================================================================
// function : GKeepShape
// purpose  :
// compute position of shape <S> / shapes of list <LSclass>
// return true if LS is not empty && (position == TB)
// (return always true if LS is empty)
//=======================================================================
Standard_Boolean TopOpeBRepBuild_Builder::GKeepShape(const TopoDS_Shape&         S,
                                                     const TopTools_ListOfShape& LSclass,
                                                     const TopAbs_State          TB)
{
  TopAbs_State pos;
  return GKeepShape1(S, LSclass, TB, pos);
}

Standard_Boolean TopOpeBRepBuild_Builder::GKeepShape1(const TopoDS_Shape&         S,
                                                      const TopTools_ListOfShape& LSclass,
                                                      const TopAbs_State          TB,
                                                      TopAbs_State&               pos)
{
  Standard_Boolean keep       = Standard_True;
  pos                         = TopAbs_UNKNOWN;
  Standard_Boolean toclassify = Standard_True;
  if (S.ShapeType() == TopAbs_FACE && !myDataStructure->HasShape(S) && myClassifyDef)
  {
    toclassify = myClassifyVal;
  }

  toclassify = (toclassify && !LSclass.IsEmpty());
  if (toclassify)
  {
    pos = ShapePosition(S, LSclass);
    if (pos != TB)
      keep = Standard_False;
  }
#ifdef OCCT_DEBUG
  Standard_Integer iS;
  Standard_Boolean tSPS  = GtraceSPS(S, iS);
  Standard_Integer iface = 0, isoli = 0;
  Standard_Boolean tSPSface = Standard_False;
  Standard_Boolean tSPSsoli = Standard_False;
  if (S.ShapeType() == TopAbs_EDGE)
  {
    tSPSface = GtraceSPS(myFaceToFill, iface);
    tSPSface = tSPSface && STATIC_trace_iexE;
  }
  else if (S.ShapeType() == TopAbs_FACE)
  {
    tSPSsoli = GtraceSPS(mySolidToFill, isoli);
    tSPSsoli = tSPSsoli && STATIC_trace_iexF;
  }

  Standard_Boolean tr = tSPS || tSPSface || tSPSsoli;
  if (tr)
  {
    if (tSPS)
      GdumpSHA(S);
    else if (tSPSface)
      std::cout << "EDGE exploration " << GLOBAL_iexE;
    else if (tSPSsoli)
      std::cout << "FACE exploration " << GLOBAL_iexF;
    if (keep)
      std::cout << " is kept";
    else
      std::cout << " is NOT kept";
    std::cout << " ";
    TopAbs::Print(TB, std::cout);
    std::cout << " / ";
    if (LSclass.IsEmpty())
      std::cout << "empty list";
    else
      GdumpLS(LSclass);
    std::cout << std::endl;
    std::cout.flush();
  }
#endif

  return keep;
}

//=======================================================================
// function : GKeepShapes
// purpose  :
// select shapes to keep from list Lin located TB compared with LSclass shapes
// selected shapes are stored in list Lou
// (apply GKeepShape on Lin shapes)
// Lou is not cleared
// S is used for trace only
//=======================================================================
void TopOpeBRepBuild_Builder::GKeepShapes
#ifdef OCCT_DEBUG
  (const TopoDS_Shape& S,
#else
  (const TopoDS_Shape&,
#endif
   const TopTools_ListOfShape& LSclass,
   const TopAbs_State          TB,
   const TopTools_ListOfShape& Lin,
   TopTools_ListOfShape&       Lou)
{
#ifdef OCCT_DEBUG
  Standard_Integer iS;
  Standard_Boolean tSPS = GtraceSPS(S, iS);
  if (tSPS)
    debkeep(iS);
#endif

#ifdef OCCT_DEBUG
  Standard_Integer n = 0;
#endif
  TopTools_ListIteratorOfListOfShape it(Lin);
  for (; it.More(); it.Next())
  {
    const TopoDS_Shape& SL = it.Value();

    Standard_Boolean keep = Standard_True;
    if (!LSclass.IsEmpty())
    {
      TopAbs_State pos = ShapePosition(SL, LSclass);
      if (pos != TB)
        keep = Standard_False;
    }

#ifdef OCCT_DEBUG
    TopAbs_ShapeEnum t = SL.ShapeType();
    if (tSPS)
    {
      std::cout << "GKeepShapes : ";
    }
    if (tSPS)
    {
      std::cout << "new ";
      TopAbs::Print(t, std::cout);
      std::cout << " " << ++n;
    }
    if (tSPS)
    {
      std::cout << " from ";
      GdumpSHA(S);
      if (keep)
        std::cout << " is kept";
      else
        std::cout << " is NOT kept";
    }
    if (tSPS)
    {
      std::cout << " ";
      TopAbs::Print(TB, std::cout);
      std::cout << " / ";
    }
    if (tSPS)
    {
      if (LSclass.IsEmpty())
        std::cout << "empty list";
      else
        GdumpLS(LSclass);
      std::cout << std::endl;
    }
#endif

    if (keep)
      Lou.Append(SL);
  }
}
