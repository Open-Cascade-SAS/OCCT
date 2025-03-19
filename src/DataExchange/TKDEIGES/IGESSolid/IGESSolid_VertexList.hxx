// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( SIVA )
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

#ifndef _IGESSolid_VertexList_HeaderFile
#define _IGESSolid_VertexList_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColgp_HArray1OfXYZ.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Integer.hxx>
class gp_Pnt;

class IGESSolid_VertexList;
DEFINE_STANDARD_HANDLE(IGESSolid_VertexList, IGESData_IGESEntity)

//! defines VertexList, Type <502> Form Number <1>
//! in package IGESSolid
//! A vertex is a point in R3. A vertex is the bound of an
//! edge and can participate in the bounds of a face.
//! It contains one or more vertices.
class IGESSolid_VertexList : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESSolid_VertexList();

  //! This method is used to set the fields of the class
  //! VertexList
  //! - vertices : the vertices in the list
  Standard_EXPORT void Init(const Handle(TColgp_HArray1OfXYZ)& vertices);

  //! return the number of vertices in the list
  Standard_EXPORT Standard_Integer NbVertices() const;

  //! returns the num'th vertex in the list
  //! raises exception if num  <= 0 or num > NbVertices()
  Standard_EXPORT gp_Pnt Vertex(const Standard_Integer num) const;

  DEFINE_STANDARD_RTTIEXT(IGESSolid_VertexList, IGESData_IGESEntity)

protected:
private:
  Handle(TColgp_HArray1OfXYZ) theVertices;
};

#endif // _IGESSolid_VertexList_HeaderFile
