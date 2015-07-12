// Created on: 2002-02-07
// Created by: Igor FEOKTISTOV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _QANewModTopOpe_ReShaper_HeaderFile
#define _QANewModTopOpe_ReShaper_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Shape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <MMgt_TShared.hxx>
#include <TopTools_HSequenceOfShape.hxx>
class TopoDS_Shape;


class QANewModTopOpe_ReShaper;
DEFINE_STANDARD_HANDLE(QANewModTopOpe_ReShaper, MMgt_TShared)

//! to remove  "floating" objects from compound.
//! "floating" objects are wires, edges, vertices that do not belong
//! solids, shells or faces.
class QANewModTopOpe_ReShaper : public MMgt_TShared
{

public:

  
  Standard_EXPORT QANewModTopOpe_ReShaper(const TopoDS_Shape& TheInitialShape);
  
  Standard_EXPORT QANewModTopOpe_ReShaper(const TopoDS_Shape& TheInitialShape, const TopTools_MapOfShape& TheMap);
  
  Standard_EXPORT QANewModTopOpe_ReShaper(const TopoDS_Shape& TheInitialShape, const Handle(TopTools_HSequenceOfShape)& TheShapeToBeRemoved);
  
  Standard_EXPORT void Remove (const TopoDS_Shape& TheS);
  
  Standard_EXPORT void Perform();
  
  //! to  clear  all  added  for  removing  shapes  from  inner  map.
  Standard_EXPORT void Clear();
  
  Standard_EXPORT const TopoDS_Shape& GetResult() const;
Standard_EXPORT operator TopoDS_Shape() const;




  DEFINE_STANDARD_RTTI(QANewModTopOpe_ReShaper,MMgt_TShared)

protected:




private:


  TopoDS_Shape myInitShape;
  TopoDS_Shape myResult;
  TopTools_MapOfShape myMap;


};







#endif // _QANewModTopOpe_ReShaper_HeaderFile
