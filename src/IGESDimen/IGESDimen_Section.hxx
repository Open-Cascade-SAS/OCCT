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

#ifndef _IGESDimen_Section_HeaderFile
#define _IGESDimen_Section_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <TColgp_HArray1OfXY.hxx>
#include <IGESData_IGESEntity.hxx>
class gp_Pnt;


class IGESDimen_Section;
DEFINE_STANDARD_HANDLE(IGESDimen_Section, IGESData_IGESEntity)

//! defines Section, Type <106> Form <31-38>
//! in package IGESDimen
//! Contains information to display sectioned sides
class IGESDimen_Section : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESDimen_Section();
  
  //! This method is used to set the fields of the class
  //! Section
  //! - dataType   : Interpretation Flag, always = 1
  //! - aDisp      : Common z displacement
  //! - dataPoints : Data points
  Standard_EXPORT void Init (const Standard_Integer dataType, const Standard_Real aDisp, const Handle(TColgp_HArray1OfXY)& dataPoints);
  
  //! Changes FormNumber (indicates the Type of the Hatches)
  //! Error if not in range [31-38]
  Standard_EXPORT void SetFormNumber (const Standard_Integer form);
  
  //! returns Interpretation Flag, always = 1
  Standard_EXPORT Standard_Integer Datatype() const;
  
  //! returns number of Data Points
  Standard_EXPORT Standard_Integer NbPoints() const;
  
  //! returns common Z displacement
  Standard_EXPORT Standard_Real ZDisplacement() const;
  
  //! returns Index'th data point
  //! raises exception if Index <= 0 or Index > NbPoints()
  Standard_EXPORT gp_Pnt Point (const Standard_Integer Index) const;
  
  //! returns Index'th data point after Transformation
  //! raises exception if Index <= 0 or Index > NbPoints()
  Standard_EXPORT gp_Pnt TransformedPoint (const Standard_Integer Index) const;




  DEFINE_STANDARD_RTTIEXT(IGESDimen_Section,IGESData_IGESEntity)

protected:




private:


  Standard_Integer theDatatype;
  Standard_Real theZDisplacement;
  Handle(TColgp_HArray1OfXY) theDataPoints;


};







#endif // _IGESDimen_Section_HeaderFile
