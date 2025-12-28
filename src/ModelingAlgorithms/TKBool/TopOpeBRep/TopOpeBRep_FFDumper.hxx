// Created on: 1996-10-23
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

#ifndef _TopOpeBRep_FFDumper_HeaderFile
#define _TopOpeBRep_FFDumper_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopOpeBRep_PFacesFiller.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_Integer.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
#include <TopOpeBRepDS_Kind.hxx>
class TopOpeBRep_LineInter;
class TopOpeBRep_VPointInter;
class TopoDS_Shape;

class TopOpeBRep_FFDumper : public Standard_Transient
{

public:
  Standard_EXPORT TopOpeBRep_FFDumper(const TopOpeBRep_PFacesFiller& PFF);

  Standard_EXPORT void Init(const TopOpeBRep_PFacesFiller& PFF);

  Standard_EXPORT void DumpLine(const int I);

  Standard_EXPORT void DumpLine(const TopOpeBRep_LineInter& L);

  Standard_EXPORT void DumpVP(const TopOpeBRep_VPointInter& VP);

  Standard_EXPORT void DumpVP(const TopOpeBRep_VPointInter& VP, const int ISI);

  Standard_EXPORT int ExploreIndex(const TopoDS_Shape&    S,
                                                const int ISI) const;

  Standard_EXPORT void DumpDSP(const TopOpeBRep_VPointInter& VP,
                               const TopOpeBRepDS_Kind       GK,
                               const int        G,
                               const bool        newinDS) const;

  Standard_EXPORT TopOpeBRep_PFacesFiller PFacesFillerDummy() const;

  DEFINE_STANDARD_RTTIEXT(TopOpeBRep_FFDumper, Standard_Transient)

private:
  TopOpeBRep_PFacesFiller        myPFF;
  TopoDS_Face                    myF1;
  TopoDS_Face                    myF2;
  NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher> myEM1;
  NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher> myEM2;
  int               myEn1;
  int               myEn2;
  int               myLineIndex;
};

#endif // _TopOpeBRep_FFDumper_HeaderFile
