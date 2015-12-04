// Created on: 2006-08-07
// Created by: Galina KULIKOVA
// Copyright (c) 2006-2014 OPEN CASCADE SAS
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

#ifndef _ShapeUpgrade_FaceDivideArea_HeaderFile
#define _ShapeUpgrade_FaceDivideArea_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <ShapeUpgrade_FaceDivide.hxx>
#include <Standard_Boolean.hxx>
class TopoDS_Face;


class ShapeUpgrade_FaceDivideArea;
DEFINE_STANDARD_HANDLE(ShapeUpgrade_FaceDivideArea, ShapeUpgrade_FaceDivide)

//! Divides face by max area criterium.
class ShapeUpgrade_FaceDivideArea : public ShapeUpgrade_FaceDivide
{

public:

  
  //! Creates empty  constructor.
  Standard_EXPORT ShapeUpgrade_FaceDivideArea();
  
  Standard_EXPORT ShapeUpgrade_FaceDivideArea(const TopoDS_Face& F);
  
  //! Performs splitting and computes the resulting shell
  //! The context is used to keep track of former splittings
  Standard_EXPORT virtual Standard_Boolean Perform() Standard_OVERRIDE;
  
  //! Set max area allowed for faces
    Standard_Real& MaxArea();




  DEFINE_STANDARD_RTTIEXT(ShapeUpgrade_FaceDivideArea,ShapeUpgrade_FaceDivide)

protected:




private:


  Standard_Real myMaxArea;


};


#include <ShapeUpgrade_FaceDivideArea.lxx>





#endif // _ShapeUpgrade_FaceDivideArea_HeaderFile
