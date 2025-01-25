// Created on: 1997-02-11
// Created by: Alexander BRIVIN and Dmitry TARASOV
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _Vrml_Normal_HeaderFile
#define _Vrml_Normal_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColgp_HArray1OfVec.hxx>
#include <Standard_Transient.hxx>
#include <Standard_OStream.hxx>

class Vrml_Normal;
DEFINE_STANDARD_HANDLE(Vrml_Normal, Standard_Transient)

//! defines a Normal node of VRML specifying properties of geometry
//! and its appearance.
//! This node defines a set of 3D surface normal vectors to be used by vertex-based shape
//! nodes (IndexedFaceSet, IndexedLineSet, PointSet) that follow it in the scene graph. This
//! node does not produce a visible result during rendering; it simply replaces the current
//! normals in the rendering state for subsequent nodes to use. This node contains one
//! multiple-valued field that contains the normal vectors.
class Vrml_Normal : public Standard_Transient
{

public:
  Standard_EXPORT Vrml_Normal(const Handle(TColgp_HArray1OfVec)& aVector);

  Standard_EXPORT Vrml_Normal();

  Standard_EXPORT void SetVector(const Handle(TColgp_HArray1OfVec)& aVector);

  Standard_EXPORT Handle(TColgp_HArray1OfVec) Vector() const;

  Standard_EXPORT Standard_OStream& Print(Standard_OStream& anOStream) const;

  DEFINE_STANDARD_RTTIEXT(Vrml_Normal, Standard_Transient)

protected:
private:
  Handle(TColgp_HArray1OfVec) myVector;
};

#endif // _Vrml_Normal_HeaderFile
