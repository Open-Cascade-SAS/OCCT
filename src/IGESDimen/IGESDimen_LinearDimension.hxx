// Created on: 1993-01-13
// Created by: CKY / Contract Toubro-Larsen ( Deepak PRABHU )
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

#ifndef _IGESDimen_LinearDimension_HeaderFile
#define _IGESDimen_LinearDimension_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESData_IGESEntity.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class IGESDimen_GeneralNote;
class IGESDimen_LeaderArrow;
class IGESDimen_WitnessLine;
class Standard_OutOfRange;


class IGESDimen_LinearDimension;
DEFINE_STANDARD_HANDLE(IGESDimen_LinearDimension, IGESData_IGESEntity)

//! defines LinearDimension, Type <216> Form <0>
//! in package IGESDimen
//! Used for linear dimensioning
class IGESDimen_LinearDimension : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESDimen_LinearDimension();
  
  //! This method is used to set the fields of the class
  //! LinearDimension
  //! - aNote          : General Note Entity
  //! - aLeader        : First Leader Entity
  //! - anotherLeader  : Second Leader Entity
  //! - aWitness       : First Witness Line Entity or a Null
  //! Handle
  //! - anotherWitness : Second Witness Line Entity or a Null
  //! Handle
  Standard_EXPORT void Init (const Handle(IGESDimen_GeneralNote)& aNote, const Handle(IGESDimen_LeaderArrow)& aLeader, const Handle(IGESDimen_LeaderArrow)& anotherLeader, const Handle(IGESDimen_WitnessLine)& aWitness, const Handle(IGESDimen_WitnessLine)& anotherWitness);
  
  //! Changes FormNumber (indicates the Nature of the Dimension
  //! Unspecified, Diameter or Radius)
  //! Error if not in range [0-2]
  Standard_EXPORT void SetFormNumber (const Standard_Integer form);
  
  //! returns General Note Entity
  Standard_EXPORT Handle(IGESDimen_GeneralNote) Note() const;
  
  //! returns first Leader Entity
  Standard_EXPORT Handle(IGESDimen_LeaderArrow) FirstLeader() const;
  
  //! returns second Leader Entity
  Standard_EXPORT Handle(IGESDimen_LeaderArrow) SecondLeader() const;
  
  //! returns False if no first witness line
  Standard_EXPORT Standard_Boolean HasFirstWitness() const;
  
  //! returns first Witness Line Entity or a Null Handle
  Standard_EXPORT Handle(IGESDimen_WitnessLine) FirstWitness() const;
  
  //! returns False if no second witness line
  Standard_EXPORT Standard_Boolean HasSecondWitness() const;
  
  //! returns second Witness Line Entity or a Null Handle
  Standard_EXPORT Handle(IGESDimen_WitnessLine) SecondWitness() const;




  DEFINE_STANDARD_RTTIEXT(IGESDimen_LinearDimension,IGESData_IGESEntity)

protected:




private:


  Handle(IGESDimen_GeneralNote) theNote;
  Handle(IGESDimen_LeaderArrow) theFirstLeader;
  Handle(IGESDimen_LeaderArrow) theSecondLeader;
  Handle(IGESDimen_WitnessLine) theFirstWitness;
  Handle(IGESDimen_WitnessLine) theSecondWitness;


};







#endif // _IGESDimen_LinearDimension_HeaderFile
