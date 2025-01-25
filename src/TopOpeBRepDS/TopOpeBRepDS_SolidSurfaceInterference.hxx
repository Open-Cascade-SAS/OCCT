// Created on: 1994-05-26
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

#ifndef _TopOpeBRepDS_SolidSurfaceInterference_HeaderFile
#define _TopOpeBRepDS_SolidSurfaceInterference_HeaderFile

#include <Standard.hxx>

#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_Kind.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>
class TopOpeBRepDS_Transition;

class TopOpeBRepDS_SolidSurfaceInterference;
DEFINE_STANDARD_HANDLE(TopOpeBRepDS_SolidSurfaceInterference, TopOpeBRepDS_Interference)

//! Interference
class TopOpeBRepDS_SolidSurfaceInterference : public TopOpeBRepDS_Interference
{

public:
  Standard_EXPORT TopOpeBRepDS_SolidSurfaceInterference(const TopOpeBRepDS_Transition& Transition,
                                                        const TopOpeBRepDS_Kind        SupportType,
                                                        const Standard_Integer         Support,
                                                        const TopOpeBRepDS_Kind        GeometryType,
                                                        const Standard_Integer         Geometry);

  DEFINE_STANDARD_RTTIEXT(TopOpeBRepDS_SolidSurfaceInterference, TopOpeBRepDS_Interference)

protected:
private:
};

#endif // _TopOpeBRepDS_SolidSurfaceInterference_HeaderFile
