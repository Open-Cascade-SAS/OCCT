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

#ifndef _TopOpeBRepDS_FaceEdgeInterference_HeaderFile
#define _TopOpeBRepDS_FaceEdgeInterference_HeaderFile

#include <Standard.hxx>

#include <TopOpeBRepDS_ShapeShapeInterference.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <TopOpeBRepDS_Config.hxx>
#include <Standard_OStream.hxx>
class TopOpeBRepDS_Transition;


class TopOpeBRepDS_FaceEdgeInterference;
DEFINE_STANDARD_HANDLE(TopOpeBRepDS_FaceEdgeInterference, TopOpeBRepDS_ShapeShapeInterference)

//! ShapeShapeInterference
class TopOpeBRepDS_FaceEdgeInterference : public TopOpeBRepDS_ShapeShapeInterference
{

public:

  
  //! Create an interference of EDGE <G> on FACE <S>.
  Standard_EXPORT TopOpeBRepDS_FaceEdgeInterference(const TopOpeBRepDS_Transition& T, const Standard_Integer S, const Standard_Integer G, const Standard_Boolean GIsBound, const TopOpeBRepDS_Config C);



  DEFINE_STANDARD_RTTIEXT(TopOpeBRepDS_FaceEdgeInterference,TopOpeBRepDS_ShapeShapeInterference)

protected:




private:




};







#endif // _TopOpeBRepDS_FaceEdgeInterference_HeaderFile
