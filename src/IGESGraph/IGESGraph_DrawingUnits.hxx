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

#ifndef _IGESGraph_DrawingUnits_HeaderFile
#define _IGESGraph_DrawingUnits_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Real.hxx>
class TCollection_HAsciiString;


class IGESGraph_DrawingUnits;
DEFINE_STANDARD_HANDLE(IGESGraph_DrawingUnits, IGESData_IGESEntity)

//! defines IGESDrawingUnits, Type <406> Form <17>
//! in package IGESGraph
//!
//! Specifies the drawing space units as outlined
//! in the Drawing entity
class IGESGraph_DrawingUnits : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESGraph_DrawingUnits();
  
  //! This method is used to set the fields of the class
  //! DrawingUnits
  //! - nbProps : Number of property values (NP = 2)
  //! - aFlag   : DrawingUnits Flag
  //! - aUnit   : DrawingUnits Name
  Standard_EXPORT void Init (const Standard_Integer nbProps, const Standard_Integer aFlag, const Handle(TCollection_HAsciiString)& aUnit);
  
  //! returns the number of property values in <me>
  Standard_EXPORT Standard_Integer NbPropertyValues() const;
  
  //! returns the drawing space units of <me>
  Standard_EXPORT Standard_Integer Flag() const;
  
  //! returns the name of the drawing space units of <me>
  Standard_EXPORT Handle(TCollection_HAsciiString) Unit() const;
  
  //! Computes the value of the unit, in meters, according Flag
  //! (same values as for GlobalSection from IGESData)
  Standard_EXPORT Standard_Real UnitValue() const;




  DEFINE_STANDARD_RTTIEXT(IGESGraph_DrawingUnits,IGESData_IGESEntity)

protected:




private:


  Standard_Integer theNbPropertyValues;
  Standard_Integer theFlag;
  Handle(TCollection_HAsciiString) theUnit;


};







#endif // _IGESGraph_DrawingUnits_HeaderFile
