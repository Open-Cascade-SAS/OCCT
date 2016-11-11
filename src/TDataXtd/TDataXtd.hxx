// Created on: 2008-05-29
// Created by: Sergey ZARITCHNY
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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

#ifndef _TDataXtd_HeaderFile
#define _TDataXtd_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TDF_IDList.hxx>
#include <Standard_OStream.hxx>
#include <TDataXtd_GeometryEnum.hxx>
#include <TDataXtd_ConstraintEnum.hxx>
class TDataXtd_Position;
class TDataXtd_Constraint;
class TDataXtd_Placement;
class TDataXtd_Geometry;
class TDataXtd_Point;
class TDataXtd_Axis;
class TDataXtd_Plane;
class TDataXtd_Pattern;
class TDataXtd_PatternStd;
class TDataXtd_Shape;
class TDataXtd_Triangulation;

//! This  package  defines  extension of standard attributes for
//! modelling  (mainly for work with geometry).
class TDataXtd 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Appends to <anIDList> the list of the attributes
  //! IDs of this package. CAUTION: <anIDList> is NOT
  //! cleared before use.
  //! Print of TDataExt enumeration
  //! =============================
  Standard_EXPORT static void IDList (TDF_IDList& anIDList);
  
  //! Prints the name of the geometry dimension <GEO> as a String on
  //! the Stream <S> and returns <S>.
  Standard_EXPORT static Standard_OStream& Print (const TDataXtd_GeometryEnum GEO, Standard_OStream& S);
  
  //! Prints the name of the constraint <CTR> as a String on
  //! the Stream <S> and returns <S>.
  Standard_EXPORT static Standard_OStream& Print (const TDataXtd_ConstraintEnum CTR, Standard_OStream& S);




protected:





private:




friend class TDataXtd_Position;
friend class TDataXtd_Constraint;
friend class TDataXtd_Placement;
friend class TDataXtd_Geometry;
friend class TDataXtd_Point;
friend class TDataXtd_Axis;
friend class TDataXtd_Plane;
friend class TDataXtd_Pattern;
friend class TDataXtd_PatternStd;
friend class TDataXtd_Shape;
friend class TDataXtd_Triangulation;

};







#endif // _TDataXtd_HeaderFile
