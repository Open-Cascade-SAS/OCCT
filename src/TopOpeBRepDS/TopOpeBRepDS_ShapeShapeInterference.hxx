// Created on: 1994-08-30
// Created by: Jean Yves LEBEY
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _TopOpeBRepDS_ShapeShapeInterference_HeaderFile
#define _TopOpeBRepDS_ShapeShapeInterference_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopOpeBRepDS_Config.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_Kind.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>
class TopOpeBRepDS_Transition;


class TopOpeBRepDS_ShapeShapeInterference;
DEFINE_STANDARD_HANDLE(TopOpeBRepDS_ShapeShapeInterference, TopOpeBRepDS_Interference)

//! Interference
class TopOpeBRepDS_ShapeShapeInterference : public TopOpeBRepDS_Interference
{

public:

  
  //! a shape interfers on shape <G> with shape <S>.
  //! examples :
  //! create a ShapeShapeInterference describing :
  //! vertex V of edge E1 found on edge E2 :
  //! ST,S,GT,G = TopOpeBRepDS_EDGE,E2,TopOpeBRepDS_VERTEX,V
  //!
  //! create a ShapeShapeInterference describing
  //! vertex V of edge E found on face F :
  //! ST,S,GT,G = TopOpeBRepDS_FACE,F,TopOpeBRepDS_VERTEX,V
  //!
  //! <GBound> indicates if shape <G> is a bound of shape <S>.
  //!
  //! <SCC> :
  //! UNSH_GEOMETRY :
  //! <S> and <Ancestor> have any types,
  //! <S> and <Ancestor> don't share the same geometry
  //! SAME_ORIENTED :
  //! <S> and <Ancestor> have identical types,
  //! <S> and <Ancestor> orientations are IDENTICAL.
  //! DIFF_ORIENTED :
  //! <S> and <Ancestor> have identical types,
  //! <S> and <Ancestor> orientations are DIFFERENT.
  Standard_EXPORT TopOpeBRepDS_ShapeShapeInterference(const TopOpeBRepDS_Transition& T, const TopOpeBRepDS_Kind ST, const Standard_Integer S, const TopOpeBRepDS_Kind GT, const Standard_Integer G, const Standard_Boolean GBound, const TopOpeBRepDS_Config C);
  
  Standard_EXPORT TopOpeBRepDS_Config Config() const;
  
  Standard_EXPORT Standard_Boolean GBound() const;
  
  Standard_EXPORT void SetGBound (const Standard_Boolean b);
  



  DEFINE_STANDARD_RTTIEXT(TopOpeBRepDS_ShapeShapeInterference,TopOpeBRepDS_Interference)

protected:




private:


  Standard_Boolean myGBound;
  TopOpeBRepDS_Config myC;


};







#endif // _TopOpeBRepDS_ShapeShapeInterference_HeaderFile
