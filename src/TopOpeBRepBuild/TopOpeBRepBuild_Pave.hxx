// Created on: 1994-11-14
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

#ifndef _TopOpeBRepBuild_Pave_HeaderFile
#define _TopOpeBRepBuild_Pave_HeaderFile

#include <Standard.hxx>

#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <TopOpeBRepDS_Kind.hxx>
#include <TopOpeBRepBuild_Loop.hxx>

class TopOpeBRepBuild_Pave;
DEFINE_STANDARD_HANDLE(TopOpeBRepBuild_Pave, TopOpeBRepBuild_Loop)

class TopOpeBRepBuild_Pave : public TopOpeBRepBuild_Loop
{

public:
  //! V = vertex, P = parameter of vertex <V>
  //! bound = True if <V> is an old vertex
  //! bound = False if <V> is a new vertex
  Standard_EXPORT TopOpeBRepBuild_Pave(const TopoDS_Shape&    V,
                                       const Standard_Real    P,
                                       const Standard_Boolean bound);

  Standard_EXPORT void HasSameDomain(const Standard_Boolean b);

  Standard_EXPORT void SameDomain(const TopoDS_Shape& VSD);

  Standard_EXPORT Standard_Boolean HasSameDomain() const;

  Standard_EXPORT const TopoDS_Shape& SameDomain() const;

  Standard_EXPORT const TopoDS_Shape& Vertex() const;

  Standard_EXPORT TopoDS_Shape& ChangeVertex();

  Standard_EXPORT Standard_Real Parameter() const;

  Standard_EXPORT void Parameter(const Standard_Real Par);

  Standard_EXPORT TopOpeBRepDS_Kind& InterferenceType();

  Standard_EXPORT virtual Standard_Boolean IsShape() const Standard_OVERRIDE;

  Standard_EXPORT virtual const TopoDS_Shape& Shape() const Standard_OVERRIDE;

  Standard_EXPORT virtual void Dump() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(TopOpeBRepBuild_Pave, TopOpeBRepBuild_Loop)

protected:
private:
  TopoDS_Shape      myVertex;
  Standard_Real     myParam;
  Standard_Boolean  myIsShape;
  Standard_Boolean  myHasSameDomain;
  TopoDS_Shape      mySameDomain;
  TopOpeBRepDS_Kind myIntType;
};

#endif // _TopOpeBRepBuild_Pave_HeaderFile
