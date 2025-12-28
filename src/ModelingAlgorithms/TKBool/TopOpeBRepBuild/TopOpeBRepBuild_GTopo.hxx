// Created on: 1996-02-13
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

#ifndef _TopOpeBRepBuild_GTopo_HeaderFile
#define _TopOpeBRepBuild_GTopo_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopAbs_ShapeEnum.hxx>
#include <TopOpeBRepDS_Config.hxx>
#include <Standard_Integer.hxx>
#include <TopAbs_State.hxx>
#include <Standard_OStream.hxx>

class TopOpeBRepBuild_GTopo
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopOpeBRepBuild_GTopo();

  Standard_EXPORT TopOpeBRepBuild_GTopo(const bool                II,
                                        const bool                IN,
                                        const bool                IO,
                                        const bool                NI,
                                        const bool                NN,
                                        const bool                NO,
                                        const bool                OI,
                                        const bool                ON,
                                        const bool                OO,
                                        const TopAbs_ShapeEnum    t1,
                                        const TopAbs_ShapeEnum    t2,
                                        const TopOpeBRepDS_Config C1,
                                        const TopOpeBRepDS_Config C2);

  Standard_EXPORT void Reset();

  Standard_EXPORT void Set(const bool II,
                           const bool IN,
                           const bool IO,
                           const bool NI,
                           const bool NN,
                           const bool NO,
                           const bool OI,
                           const bool ON,
                           const bool OO);

  Standard_EXPORT void Type(TopAbs_ShapeEnum& t1, TopAbs_ShapeEnum& t2) const;

  Standard_EXPORT void ChangeType(const TopAbs_ShapeEnum t1, const TopAbs_ShapeEnum t2);

  Standard_EXPORT TopOpeBRepDS_Config Config1() const;

  Standard_EXPORT TopOpeBRepDS_Config Config2() const;

  Standard_EXPORT void ChangeConfig(const TopOpeBRepDS_Config C1, const TopOpeBRepDS_Config C2);

  Standard_EXPORT bool Value(const TopAbs_State s1, const TopAbs_State s2) const;

  Standard_EXPORT bool Value(const int I1, const int I2) const;

  Standard_EXPORT bool Value(const int II) const;

  Standard_EXPORT void ChangeValue(const int i1, const int i2, const bool b);

  Standard_EXPORT void ChangeValue(const TopAbs_State s1, const TopAbs_State s2, const bool b);

  Standard_EXPORT int GIndex(const TopAbs_State S) const;

  Standard_EXPORT TopAbs_State GState(const int I) const;

  Standard_EXPORT void Index(const int II, int& i1, int& i2) const;

  Standard_EXPORT void DumpVal(Standard_OStream&  OS,
                               const TopAbs_State s1,
                               const TopAbs_State s2) const;

  Standard_EXPORT void DumpType(Standard_OStream& OS) const;

  Standard_EXPORT static void DumpSSB(Standard_OStream&  OS,
                                      const TopAbs_State s1,
                                      const TopAbs_State s2,
                                      const bool         b);

  Standard_EXPORT virtual void Dump(Standard_OStream& OS, void* const s = nullptr) const;

  Standard_EXPORT void StatesON(TopAbs_State& s1, TopAbs_State& s2) const;

  Standard_EXPORT bool IsToReverse1() const;

  Standard_EXPORT bool IsToReverse2() const;

  Standard_EXPORT void SetReverse(const bool rev);

  Standard_EXPORT bool Reverse() const;

  Standard_EXPORT TopOpeBRepBuild_GTopo CopyPermuted() const;

private:
  bool                mycases[3][3];
  TopAbs_ShapeEnum    myt1;
  TopAbs_ShapeEnum    myt2;
  TopOpeBRepDS_Config myConfig1;
  TopOpeBRepDS_Config myConfig2;
  bool                myReverseForce;
  bool                myReverseValue;
};

#endif // _TopOpeBRepBuild_GTopo_HeaderFile
