// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen ( Arun MENON )
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

#ifndef _IGESAppli_DrilledHole_HeaderFile
#define _IGESAppli_DrilledHole_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <IGESData_IGESEntity.hxx>


class IGESAppli_DrilledHole;
DEFINE_STANDARD_HANDLE(IGESAppli_DrilledHole, IGESData_IGESEntity)

//! defines DrilledHole, Type <406> Form <6>
//! in package IGESAppli
//! Identifies an entity representing a drilled hole
//! through a printed circuit board.
class IGESAppli_DrilledHole : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESAppli_DrilledHole();
  
  //! This method is used to set the fields of the class
  //! DrilledHole
  //! - nbPropVal    : Number of property values = 5
  //! - aSize        : Drill diameter size
  //! - anotherSize  : Finish diameter size
  //! - aPlating     : Plating indication flag
  //! False = not plating
  //! True  = is plating
  //! - aLayer       : Lower numbered layer
  //! - anotherLayer : Higher numbered layer
  Standard_EXPORT void Init (const Standard_Integer nbPropVal, const Standard_Real aSize, const Standard_Real anotherSize, const Standard_Integer aPlating, const Standard_Integer aLayer, const Standard_Integer anotherLayer);
  
  //! is always 5
  Standard_EXPORT Standard_Integer NbPropertyValues() const;
  
  //! returns the drill diameter size
  Standard_EXPORT Standard_Real DrillDiaSize() const;
  
  //! returns the finish diameter size
  Standard_EXPORT Standard_Real FinishDiaSize() const;
  
  //! Returns Plating Status :
  //! False = not plating  /  True  = is plating
  Standard_EXPORT Standard_Boolean IsPlating() const;
  
  //! returns the lower numbered layer
  Standard_EXPORT Standard_Integer NbLowerLayer() const;
  
  //! returns the higher numbered layer
  Standard_EXPORT Standard_Integer NbHigherLayer() const;




  DEFINE_STANDARD_RTTIEXT(IGESAppli_DrilledHole,IGESData_IGESEntity)

protected:




private:


  Standard_Integer theNbPropertyValues;
  Standard_Real theDrillDiaSize;
  Standard_Real theFinishDiaSize;
  Standard_Integer thePlatingFlag;
  Standard_Integer theNbLowerLayer;
  Standard_Integer theNbHigherLayer;


};







#endif // _IGESAppli_DrilledHole_HeaderFile
