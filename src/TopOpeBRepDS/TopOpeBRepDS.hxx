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

#ifndef _TopOpeBRepDS_HeaderFile
#define _TopOpeBRepDS_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopAbs_State.hxx>
#include <TopOpeBRepDS_Kind.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopOpeBRepDS_Config.hxx>
class TCollection_AsciiString;


//! This package provides services used by the TopOpeBRepBuild
//! package performing topological operations on the BRep
//! data structure.
class TopOpeBRepDS 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! IN OU ON UN
  Standard_EXPORT static TCollection_AsciiString SPrint (const TopAbs_State S);
  
  Standard_EXPORT static Standard_OStream& Print (const TopAbs_State S, Standard_OStream& OS);
  
  //! <K>
  Standard_EXPORT static TCollection_AsciiString SPrint (const TopOpeBRepDS_Kind K);
  
  //! S1(<K>,<I>)S2
  Standard_EXPORT static TCollection_AsciiString SPrint (const TopOpeBRepDS_Kind K, const Standard_Integer I, const TCollection_AsciiString& B = "", const TCollection_AsciiString& A = "");
  
  Standard_EXPORT static Standard_OStream& Print (const TopOpeBRepDS_Kind K, Standard_OStream& S);
  
  Standard_EXPORT static Standard_OStream& Print (const TopOpeBRepDS_Kind K, const Standard_Integer I, Standard_OStream& S, const TCollection_AsciiString& B = "", const TCollection_AsciiString& A = "");
  
  Standard_EXPORT static TCollection_AsciiString SPrint (const TopAbs_ShapeEnum T);
  
  //! (<T>,<I>)
  Standard_EXPORT static TCollection_AsciiString SPrint (const TopAbs_ShapeEnum T, const Standard_Integer I);
  
  Standard_EXPORT static Standard_OStream& Print (const TopAbs_ShapeEnum T, const Standard_Integer I, Standard_OStream& S);
  
  Standard_EXPORT static TCollection_AsciiString SPrint (const TopAbs_Orientation O);
  
  Standard_EXPORT static TCollection_AsciiString SPrint (const TopOpeBRepDS_Config C);
  
  Standard_EXPORT static Standard_OStream& Print (const TopOpeBRepDS_Config C, Standard_OStream& S);
  
  Standard_EXPORT static Standard_Boolean IsGeometry (const TopOpeBRepDS_Kind K);
  
  Standard_EXPORT static Standard_Boolean IsTopology (const TopOpeBRepDS_Kind K);
  
  Standard_EXPORT static TopAbs_ShapeEnum KindToShape (const TopOpeBRepDS_Kind K);
  
  Standard_EXPORT static TopOpeBRepDS_Kind ShapeToKind (const TopAbs_ShapeEnum S);

};

#endif // _TopOpeBRepDS_HeaderFile
