// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( SIVA )
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

#ifndef _IGESSolid_SolidOfRevolution_HeaderFile
#define _IGESSolid_SolidOfRevolution_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <gp_XYZ.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Boolean.hxx>
class IGESData_IGESEntity;
class gp_XYZ;
class gp_Pnt;
class gp_Dir;


class IGESSolid_SolidOfRevolution;
DEFINE_STANDARD_HANDLE(IGESSolid_SolidOfRevolution, IGESData_IGESEntity)

//! defines SolidOfRevolution, Type <162> Form Number <0,1>
//! in package IGESSolid
//! This entity is defined by revolving the area determined
//! by a planar curve about a specified axis through a given
//! fraction of full rotation.
class IGESSolid_SolidOfRevolution : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESSolid_SolidOfRevolution();
  
  //! This method is used to set the fields of the class
  //! SolidOfRevolution
  //! - aCurve     : the curve entity that is to be revolved
  //! - aFract     : the fraction of full rotation (default 1.0)
  //! - aAxisPnt   : the point on the axis
  //! - aDirection : the direction of the axis
  Standard_EXPORT void Init (const Handle(IGESData_IGESEntity)& aCurve, const Standard_Real aFract, const gp_XYZ& aAxisPnt, const gp_XYZ& aDirection);
  
  //! Sets the Curve to be by default, Closed to Axis (Form 0)
  //! if <mode> is True, Closed to Itself (Form 1) else
  Standard_EXPORT void SetClosedToAxis (const Standard_Boolean mode);
  
  //! Returns True if Form Number = 0
  //! if Form no is 0, then the curve is closed to axis
  //! if 1, the curve is closed to itself.
  Standard_EXPORT Standard_Boolean IsClosedToAxis() const;
  
  //! returns the curve entity that is to be revolved
  Standard_EXPORT Handle(IGESData_IGESEntity) Curve() const;
  
  //! returns the fraction of full rotation that the curve is to
  //! be rotated
  Standard_EXPORT Standard_Real Fraction() const;
  
  //! returns the point on the axis
  Standard_EXPORT gp_Pnt AxisPoint() const;
  
  //! returns the point on the axis after applying Trans.Matrix
  Standard_EXPORT gp_Pnt TransformedAxisPoint() const;
  
  //! returns the direction of the axis
  Standard_EXPORT gp_Dir Axis() const;
  
  //! returns the direction of the axis after applying
  //! TransformationMatrix
  Standard_EXPORT gp_Dir TransformedAxis() const;




  DEFINE_STANDARD_RTTIEXT(IGESSolid_SolidOfRevolution,IGESData_IGESEntity)

protected:




private:


  Handle(IGESData_IGESEntity) theCurve;
  Standard_Real theFraction;
  gp_XYZ theAxisPoint;
  gp_XYZ theAxis;


};







#endif // _IGESSolid_SolidOfRevolution_HeaderFile
