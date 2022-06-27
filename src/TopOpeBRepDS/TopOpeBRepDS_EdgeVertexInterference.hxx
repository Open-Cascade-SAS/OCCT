// Created on: 1994-10-28
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

#ifndef _TopOpeBRepDS_EdgeVertexInterference_HeaderFile
#define _TopOpeBRepDS_EdgeVertexInterference_HeaderFile

#include <Standard.hxx>

#include <Standard_Real.hxx>
#include <TopOpeBRepDS_ShapeShapeInterference.hxx>
#include <TopOpeBRepDS_Kind.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <TopOpeBRepDS_Config.hxx>
#include <Standard_OStream.hxx>
class TopOpeBRepDS_Transition;


class TopOpeBRepDS_EdgeVertexInterference;
DEFINE_STANDARD_HANDLE(TopOpeBRepDS_EdgeVertexInterference, TopOpeBRepDS_ShapeShapeInterference)

//! An interference with a parameter (ShapeShapeInterference).
class TopOpeBRepDS_EdgeVertexInterference : public TopOpeBRepDS_ShapeShapeInterference
{

public:

  
  //! Create an interference of VERTEX <G> on a crossed EDGE E.
  //!
  //! if support type <ST> == EDGE : <S> is edge E
  //! FACE : <S> is the face with bound E.
  //! <T> is the transition along the edge, crossing the crossed edge.
  //! E  is the crossed edge.
  //! <GIsBound> indicates if <G> is a bound of the edge.
  //! <P> is the parameter of <G> on the edge.
  //!
  //! interference is stored in the list of interfs of the edge.
  Standard_EXPORT TopOpeBRepDS_EdgeVertexInterference(const TopOpeBRepDS_Transition& T, const TopOpeBRepDS_Kind ST, const Standard_Integer S, const Standard_Integer G, const Standard_Boolean GIsBound, const TopOpeBRepDS_Config C, const Standard_Real P);
  
  //! Create an interference of VERTEX <G> on crossed EDGE <S>.
  //!
  //! <T> is the transition along the edge, crossing the crossed edge.
  //! <S> is the crossed edge.
  //! <GIsBound> indicates if <G> is a bound of the edge.
  //! <C> indicates the geometric configuration between
  //! the edge and the crossed edge.
  //! <P> is the parameter of <G> on the edge.
  //!
  //! interference is stored in the list of interfs of the edge.
  Standard_EXPORT TopOpeBRepDS_EdgeVertexInterference(const TopOpeBRepDS_Transition& T, const Standard_Integer S, const Standard_Integer G, const Standard_Boolean GIsBound, const TopOpeBRepDS_Config C, const Standard_Real P);
  
  Standard_EXPORT Standard_Real Parameter() const;
  
  Standard_EXPORT void Parameter (const Standard_Real P);





  DEFINE_STANDARD_RTTIEXT(TopOpeBRepDS_EdgeVertexInterference,TopOpeBRepDS_ShapeShapeInterference)

protected:




private:


  Standard_Real myParam;


};







#endif // _TopOpeBRepDS_EdgeVertexInterference_HeaderFile
