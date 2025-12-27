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

#ifndef _IGESDimen_CurveDimension_HeaderFile
#define _IGESDimen_CurveDimension_HeaderFile

#include <Standard.hxx>

#include <IGESData_IGESEntity.hxx>
class IGESDimen_GeneralNote;
class IGESDimen_LeaderArrow;
class IGESDimen_WitnessLine;

//! defines CurveDimension, Type <204> Form <0>
//! in package IGESDimen
//! Used to dimension curves
//! Consists of one tail segment of nonzero length
//! beginning with an arrowhead and which serves to define
//! the orientation
class IGESDimen_CurveDimension : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESDimen_CurveDimension();

  //! This method is used to set the fields of the class
  //! CurveDimension
  //! - aNote         : General Note Entity
  //! - aCurve        : First Curve Entity
  //! - anotherCurve  : Second Curve Entity or a Null Handle
  //! - aLeader       : First Leader Entity
  //! - anotherLeader : Second Leader Entity
  //! - aLine         : First Witness Line Entity or a Null
  //! Handle
  //! - anotherLine   : Second Witness Line Entity or a Null
  //! Handle
  Standard_EXPORT void Init(const occ::handle<IGESDimen_GeneralNote>& aNote,
                            const occ::handle<IGESData_IGESEntity>&   aCurve,
                            const occ::handle<IGESData_IGESEntity>&   anotherCurve,
                            const occ::handle<IGESDimen_LeaderArrow>& aLeader,
                            const occ::handle<IGESDimen_LeaderArrow>& anotherLeader,
                            const occ::handle<IGESDimen_WitnessLine>& aLine,
                            const occ::handle<IGESDimen_WitnessLine>& anotherLine);

  //! returns the General Note Entity
  Standard_EXPORT occ::handle<IGESDimen_GeneralNote> Note() const;

  //! returns the First curve Entity
  Standard_EXPORT occ::handle<IGESData_IGESEntity> FirstCurve() const;

  //! returns False if theSecondCurve is a Null Handle.
  Standard_EXPORT bool HasSecondCurve() const;

  //! returns the Second curve Entity or a Null Handle.
  Standard_EXPORT occ::handle<IGESData_IGESEntity> SecondCurve() const;

  //! returns the First Leader Entity
  Standard_EXPORT occ::handle<IGESDimen_LeaderArrow> FirstLeader() const;

  //! returns the Second Leader Entity
  Standard_EXPORT occ::handle<IGESDimen_LeaderArrow> SecondLeader() const;

  //! returns False if theFirstWitnessLine is a Null Handle.
  Standard_EXPORT bool HasFirstWitnessLine() const;

  //! returns the First Witness Line Entity or a Null Handle.
  Standard_EXPORT occ::handle<IGESDimen_WitnessLine> FirstWitnessLine() const;

  //! returns False if theSecondWitnessLine is a Null Handle.
  Standard_EXPORT bool HasSecondWitnessLine() const;

  //! returns the Second Witness Line Entity or a Null Handle.
  Standard_EXPORT occ::handle<IGESDimen_WitnessLine> SecondWitnessLine() const;

  DEFINE_STANDARD_RTTIEXT(IGESDimen_CurveDimension, IGESData_IGESEntity)

private:
  occ::handle<IGESDimen_GeneralNote> theNote;
  occ::handle<IGESData_IGESEntity>   theFirstCurve;
  occ::handle<IGESData_IGESEntity>   theSecondCurve;
  occ::handle<IGESDimen_LeaderArrow> theFirstLeader;
  occ::handle<IGESDimen_LeaderArrow> theSecondLeader;
  occ::handle<IGESDimen_WitnessLine> theFirstWitnessLine;
  occ::handle<IGESDimen_WitnessLine> theSecondWitnessLine;
};

#endif // _IGESDimen_CurveDimension_HeaderFile
