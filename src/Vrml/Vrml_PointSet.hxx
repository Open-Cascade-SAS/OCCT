// Created on: 1997-02-04
// Created by: Alexander BRIVIN
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

#ifndef _Vrml_PointSet_HeaderFile
#define _Vrml_PointSet_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_OStream.hxx>

//! defines a PointSet node of VRML specifying geometry shapes.
class Vrml_PointSet
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT Vrml_PointSet(const Standard_Integer aStartIndex = 0,
                                const Standard_Integer aNumPoints  = -1);

  Standard_EXPORT void SetStartIndex(const Standard_Integer aStartIndex);

  Standard_EXPORT Standard_Integer StartIndex() const;

  Standard_EXPORT void SetNumPoints(const Standard_Integer aNumPoints);

  Standard_EXPORT Standard_Integer NumPoints() const;

  Standard_EXPORT Standard_OStream& Print(Standard_OStream& anOStream) const;

protected:
private:
  Standard_Integer myStartIndex;
  Standard_Integer myNumPoints;
};

#endif // _Vrml_PointSet_HeaderFile
