// Created on: 1994-03-25
// Created by: model
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

#ifndef _Adaptor3d_HVertex_HeaderFile
#define _Adaptor3d_HVertex_HeaderFile

#include <Adaptor2d_Curve2d.hxx>
#include <gp_Pnt2d.hxx>
#include <TopAbs_Orientation.hxx>

class Adaptor3d_HVertex;
DEFINE_STANDARD_HANDLE(Adaptor3d_HVertex, Standard_Transient)

class Adaptor3d_HVertex : public Standard_Transient
{

public:
  Standard_EXPORT Adaptor3d_HVertex();

  Standard_EXPORT Adaptor3d_HVertex(const gp_Pnt2d&          P,
                                    const TopAbs_Orientation Ori,
                                    const Standard_Real      Resolution);

  Standard_EXPORT virtual gp_Pnt2d Value();

  Standard_EXPORT virtual Standard_Real Parameter(const Handle(Adaptor2d_Curve2d)& C);

  //! Parametric resolution (2d).
  Standard_EXPORT virtual Standard_Real Resolution(const Handle(Adaptor2d_Curve2d)& C);

  Standard_EXPORT virtual TopAbs_Orientation Orientation();

  Standard_EXPORT virtual Standard_Boolean IsSame(const Handle(Adaptor3d_HVertex)& Other);

  DEFINE_STANDARD_RTTIEXT(Adaptor3d_HVertex, Standard_Transient)

protected:
private:
  gp_Pnt2d           myPnt;
  Standard_Real      myTol;
  TopAbs_Orientation myOri;
};

#endif // _Adaptor3d_HVertex_HeaderFile
