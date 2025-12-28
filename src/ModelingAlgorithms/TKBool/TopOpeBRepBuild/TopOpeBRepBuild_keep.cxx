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
extern int GLOBAL_iexE;
extern int GLOBAL_iexF;
bool        STATIC_trace_iexE = false;
bool        STATIC_trace_iexF = false;

Standard_EXPORT void debkeep(const int i)
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
bool TopOpeBRepBuild_Builder::GKeepShape(const TopoDS_Shape&         S,
                                                     const NCollection_List<TopoDS_Shape>& LSclass,
                                                     const TopAbs_State          TB)
{
  TopAbs_State pos;
  return GKeepShape1(S, LSclass, TB, pos);
}

bool TopOpeBRepBuild_Builder::GKeepShape1(const TopoDS_Shape&         S,
                                                      const NCollection_List<TopoDS_Shape>& LSclass,
                                                      const TopAbs_State          TB,
                                                      TopAbs_State&               pos)
{
  bool keep       = true;
  pos                         = TopAbs_UNKNOWN;
  bool toclassify = true;
  if (S.ShapeType() == TopAbs_FACE && !myDataStructure->HasShape(S) && myClassifyDef)
  {
    toclassify = myClassifyVal;
  }

  toclassify = (toclassify && !LSclass.IsEmpty());
  if (toclassify)
  {
    pos = ShapePosition(S, LSclass);
    if (pos != TB)
      keep = false;
  }
#ifdef OCCT_DEBUG
  int iS;
  bool tSPS  = GtraceSPS(S, iS);
  int iface = 0, isoli = 0;
  bool tSPSface = false;
  bool tSPSsoli = false;
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

  bool tr = tSPS || tSPSface || tSPSsoli;
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
   const NCollection_List<TopoDS_Shape>& LSclass,
   const TopAbs_State          TB,
   const NCollection_List<TopoDS_Shape>& Lin,
   NCollection_List<TopoDS_Shape>&       Lou)
{
#ifdef OCCT_DEBUG
  int iS;
  bool tSPS = GtraceSPS(S, iS);
  if (tSPS)
    debkeep(iS);
#endif

#ifdef OCCT_DEBUG
  int n = 0;
#endif
  NCollection_List<TopoDS_Shape>::Iterator it(Lin);
  for (; it.More(); it.Next())
  {
    const TopoDS_Shape& SL = it.Value();

    bool keep = true;
    if (!LSclass.IsEmpty())
    {
      TopAbs_State pos = ShapePosition(SL, LSclass);
      if (pos != TB)
        keep = false;
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
