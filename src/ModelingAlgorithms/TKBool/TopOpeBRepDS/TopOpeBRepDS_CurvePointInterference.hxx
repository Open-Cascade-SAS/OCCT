// Created on: 1993-06-23
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

#ifndef _TopOpeBRepDS_CurvePointInterference_HeaderFile
#define _TopOpeBRepDS_CurvePointInterference_HeaderFile

#include <Standard.hxx>

#include <Standard_Real.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_Kind.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>
class TopOpeBRepDS_Transition;

class TopOpeBRepDS_CurvePointInterference;
DEFINE_STANDARD_HANDLE(TopOpeBRepDS_CurvePointInterference, TopOpeBRepDS_Interference)

//! An interference with a parameter.
class TopOpeBRepDS_CurvePointInterference : public TopOpeBRepDS_Interference
{

public:
  Standard_EXPORT TopOpeBRepDS_CurvePointInterference(const TopOpeBRepDS_Transition& T,
                                                      const TopOpeBRepDS_Kind        ST,
                                                      const Standard_Integer         S,
                                                      const TopOpeBRepDS_Kind        GT,
                                                      const Standard_Integer         G,
                                                      const Standard_Real            P);

  Standard_EXPORT Standard_Real Parameter() const;

  Standard_EXPORT void Parameter(const Standard_Real P);

  DEFINE_STANDARD_RTTIEXT(TopOpeBRepDS_CurvePointInterference, TopOpeBRepDS_Interference)

protected:
private:
  Standard_Real myParam;
};

#endif // _TopOpeBRepDS_CurvePointInterference_HeaderFile
