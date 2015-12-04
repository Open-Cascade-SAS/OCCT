// Created on: 2004-02-04
// Created by: Sergey KUUL
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

#ifndef _ShapeFix_SplitCommonVertex_HeaderFile
#define _ShapeFix_SplitCommonVertex_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Shape.hxx>
#include <Standard_Integer.hxx>
#include <ShapeFix_Root.hxx>
class TopoDS_Shape;


class ShapeFix_SplitCommonVertex;
DEFINE_STANDARD_HANDLE(ShapeFix_SplitCommonVertex, ShapeFix_Root)

//! Two wires have common vertex - this case is valid in BRep model
//! and isn't valid in STEP => before writing into STEP it is necessary
//! to split this vertex (each wire must has one vertex)
class ShapeFix_SplitCommonVertex : public ShapeFix_Root
{

public:

  
  Standard_EXPORT ShapeFix_SplitCommonVertex();
  
  Standard_EXPORT void Init (const TopoDS_Shape& S);
  
  Standard_EXPORT void Perform();
  
  Standard_EXPORT TopoDS_Shape Shape();




  DEFINE_STANDARD_RTTIEXT(ShapeFix_SplitCommonVertex,ShapeFix_Root)

protected:




private:


  TopoDS_Shape myShape;
  TopoDS_Shape myResult;
  Standard_Integer myStatus;


};







#endif // _ShapeFix_SplitCommonVertex_HeaderFile
