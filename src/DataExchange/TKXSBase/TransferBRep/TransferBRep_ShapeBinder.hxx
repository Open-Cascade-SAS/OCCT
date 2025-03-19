// Created on: 1994-10-03
// Created by: Assim
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

#ifndef _TransferBRep_ShapeBinder_HeaderFile
#define _TransferBRep_ShapeBinder_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TransferBRep_BinderOfShape.hxx>
#include <TopAbs_ShapeEnum.hxx>
class TopoDS_Shape;
class TopoDS_Vertex;
class TopoDS_Edge;
class TopoDS_Wire;
class TopoDS_Face;
class TopoDS_Shell;
class TopoDS_Solid;
class TopoDS_CompSolid;
class TopoDS_Compound;

class TransferBRep_ShapeBinder;
DEFINE_STANDARD_HANDLE(TransferBRep_ShapeBinder, TransferBRep_BinderOfShape)

//! A ShapeBinder is a BinderOfShape with some additional services
//! to cast the Result under various kinds of Shapes
class TransferBRep_ShapeBinder : public TransferBRep_BinderOfShape
{

public:
  //! Creates an empty ShapeBinder
  Standard_EXPORT TransferBRep_ShapeBinder();

  //! Creates a ShapeBinder with a result
  Standard_EXPORT TransferBRep_ShapeBinder(const TopoDS_Shape& res);

  //! Returns the Type of the Shape Result (under TopAbs form)
  Standard_EXPORT TopAbs_ShapeEnum ShapeType() const;

  Standard_EXPORT TopoDS_Vertex Vertex() const;

  Standard_EXPORT TopoDS_Edge Edge() const;

  Standard_EXPORT TopoDS_Wire Wire() const;

  Standard_EXPORT TopoDS_Face Face() const;

  Standard_EXPORT TopoDS_Shell Shell() const;

  Standard_EXPORT TopoDS_Solid Solid() const;

  Standard_EXPORT TopoDS_CompSolid CompSolid() const;

  Standard_EXPORT TopoDS_Compound Compound() const;

  DEFINE_STANDARD_RTTIEXT(TransferBRep_ShapeBinder, TransferBRep_BinderOfShape)

protected:
private:
};

#endif // _TransferBRep_ShapeBinder_HeaderFile
