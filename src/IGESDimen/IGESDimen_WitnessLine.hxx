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

#ifndef _IGESDimen_WitnessLine_HeaderFile
#define _IGESDimen_WitnessLine_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <TColgp_HArray1OfXY.hxx>
#include <IGESData_IGESEntity.hxx>
class gp_Pnt;


class IGESDimen_WitnessLine;
DEFINE_STANDARD_HANDLE(IGESDimen_WitnessLine, IGESData_IGESEntity)

//! defines WitnessLine, Type <106> Form <40>
//! in package IGESDimen
//! Contains one or more straight line segments associated
//! with drafting entities of various types
class IGESDimen_WitnessLine : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESDimen_WitnessLine();
  
  //! This method is used to set the fields of the class
  //! WitnessLine
  //! - dataType   : Interpretation Flag, always = 1
  //! - aDispl     : Common z displacement
  //! - dataPoints : Data points
  Standard_EXPORT void Init (const Standard_Integer dataType, const Standard_Real aDisp, const Handle(TColgp_HArray1OfXY)& dataPoints);
  
  //! returns Interpretation Flag, always = 1
  Standard_EXPORT Standard_Integer Datatype() const;
  
  //! returns number of Data Points
  Standard_EXPORT Standard_Integer NbPoints() const;
  
  //! returns common Z displacement
  Standard_EXPORT Standard_Real ZDisplacement() const;
  
  //! returns Index'th. data point
  //! raises exception if Index <= 0 or Index > NbPoints
  Standard_EXPORT gp_Pnt Point (const Standard_Integer Index) const;
  
  //! returns data point after Transformation.
  //! raises exception if Index <= 0 or Index > NbPoints
  Standard_EXPORT gp_Pnt TransformedPoint (const Standard_Integer Index) const;




  DEFINE_STANDARD_RTTIEXT(IGESDimen_WitnessLine,IGESData_IGESEntity)

protected:




private:


  Standard_Integer theDatatype;
  Standard_Real theZDisplacement;
  Handle(TColgp_HArray1OfXY) theDataPoints;


};







#endif // _IGESDimen_WitnessLine_HeaderFile
