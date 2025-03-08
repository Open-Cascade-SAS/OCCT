// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( TCD )
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

#ifndef _IGESGraph_DrawingSize_HeaderFile
#define _IGESGraph_DrawingSize_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <IGESData_IGESEntity.hxx>

class IGESGraph_DrawingSize;
DEFINE_STANDARD_HANDLE(IGESGraph_DrawingSize, IGESData_IGESEntity)

//! defines IGESDrawingSize, Type <406> Form <16>
//! in package IGESGraph
//!
//! Specifies the drawing size in drawing units. The
//! origin of the drawing is defined to be (0,0) in
//! drawing space
class IGESGraph_DrawingSize : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESGraph_DrawingSize();

  //! This method is used to set the fields of the class
  //! DrawingSize
  //! - nbProps : Number of property values (NP = 2)
  //! - aXSize  : Extent of Drawing along positive XD axis
  //! - aYSize  : Extent of Drawing along positive YD axis
  Standard_EXPORT void Init(const Standard_Integer nbProps,
                            const Standard_Real    aXSize,
                            const Standard_Real    aYSize);

  //! returns the number of property values in <me> (NP = 2)
  Standard_EXPORT Standard_Integer NbPropertyValues() const;

  //! returns the extent of Drawing along positive XD axis
  Standard_EXPORT Standard_Real XSize() const;

  //! returns the extent of Drawing along positive YD axis
  Standard_EXPORT Standard_Real YSize() const;

  DEFINE_STANDARD_RTTIEXT(IGESGraph_DrawingSize, IGESData_IGESEntity)

protected:
private:
  Standard_Integer theNbPropertyValues;
  Standard_Real    theXSize;
  Standard_Real    theYSize;
};

#endif // _IGESGraph_DrawingSize_HeaderFile
