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
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopOpeBRepDS_Config.hxx>
#include <Standard_Integer.hxx>
#include <TopAbs_State.hxx>
#include <Standard_OStream.hxx>
#include <Standard_Address.hxx>



class TopOpeBRepBuild_GTopo 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRepBuild_GTopo();
  
  Standard_EXPORT TopOpeBRepBuild_GTopo (const Standard_Boolean II, const Standard_Boolean IN, const Standard_Boolean IO,
                                         const Standard_Boolean NI, const Standard_Boolean NN, const Standard_Boolean NO,
                                         const Standard_Boolean OI, const Standard_Boolean ON, const Standard_Boolean OO,
                                         const TopAbs_ShapeEnum t1, const TopAbs_ShapeEnum t2,
                                         const TopOpeBRepDS_Config C1, const TopOpeBRepDS_Config C2);

  Standard_EXPORT void Reset();
  
  Standard_EXPORT void Set (const Standard_Boolean II, const Standard_Boolean IN, const Standard_Boolean IO, const Standard_Boolean NI, const Standard_Boolean NN, const Standard_Boolean NO, const Standard_Boolean OI, const Standard_Boolean ON, const Standard_Boolean OO);
  
  Standard_EXPORT void Type (TopAbs_ShapeEnum& t1, TopAbs_ShapeEnum& t2) const;
  
  Standard_EXPORT void ChangeType (const TopAbs_ShapeEnum t1, const TopAbs_ShapeEnum t2);
  
  Standard_EXPORT TopOpeBRepDS_Config Config1() const;
  
  Standard_EXPORT TopOpeBRepDS_Config Config2() const;
  
  Standard_EXPORT void ChangeConfig (const TopOpeBRepDS_Config C1, const TopOpeBRepDS_Config C2);
  
  Standard_EXPORT Standard_Boolean Value (const TopAbs_State s1, const TopAbs_State s2) const;
  
  Standard_EXPORT Standard_Boolean Value (const Standard_Integer I1, const Standard_Integer I2) const;
  
  Standard_EXPORT Standard_Boolean Value (const Standard_Integer II) const;
  
  Standard_EXPORT void ChangeValue (const Standard_Integer i1, const Standard_Integer i2, const Standard_Boolean b);
  
  Standard_EXPORT void ChangeValue (const TopAbs_State s1, const TopAbs_State s2, const Standard_Boolean b);
  
  Standard_EXPORT Standard_Integer GIndex (const TopAbs_State S) const;
  
  Standard_EXPORT TopAbs_State GState (const Standard_Integer I) const;
  
  Standard_EXPORT void Index (const Standard_Integer II, Standard_Integer& i1, Standard_Integer& i2) const;
  
  Standard_EXPORT void DumpVal (Standard_OStream& OS, const TopAbs_State s1, const TopAbs_State s2) const;
  
  Standard_EXPORT void DumpType (Standard_OStream& OS) const;
  
  Standard_EXPORT static void DumpSSB (Standard_OStream& OS, const TopAbs_State s1, const TopAbs_State s2, const Standard_Boolean b);
  
  Standard_EXPORT virtual void Dump (Standard_OStream& OS, const Standard_Address s = NULL) const;
  
  Standard_EXPORT void StatesON (TopAbs_State& s1, TopAbs_State& s2) const;
  
  Standard_EXPORT Standard_Boolean IsToReverse1() const;
  
  Standard_EXPORT Standard_Boolean IsToReverse2() const;
  
  Standard_EXPORT void SetReverse (const Standard_Boolean rev);
  
  Standard_EXPORT Standard_Boolean Reverse() const;
  
  Standard_EXPORT TopOpeBRepBuild_GTopo CopyPermuted() const;




protected:





private:



  Standard_Boolean mycases[3][3];
  TopAbs_ShapeEnum myt1;
  TopAbs_ShapeEnum myt2;
  TopOpeBRepDS_Config myConfig1;
  TopOpeBRepDS_Config myConfig2;
  Standard_Boolean myReverseForce;
  Standard_Boolean myReverseValue;


};







#endif // _TopOpeBRepBuild_GTopo_HeaderFile
