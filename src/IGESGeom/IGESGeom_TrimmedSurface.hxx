// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( Kiran )
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

#ifndef _IGESGeom_TrimmedSurface_HeaderFile
#define _IGESGeom_TrimmedSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <IGESGeom_HArray1OfCurveOnSurface.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Boolean.hxx>
class IGESData_IGESEntity;
class IGESGeom_CurveOnSurface;
class Standard_OutOfRange;


class IGESGeom_TrimmedSurface;
DEFINE_STANDARD_HANDLE(IGESGeom_TrimmedSurface, IGESData_IGESEntity)

//! defines IGESTrimmedSurface, Type <144> Form <0>
//! in package IGESGeom
//! A simple closed curve  in Euclidean plane  divides the
//! plane in to two disjoint, open connected components; one
//! bounded, one unbounded. The bounded one is called the
//! interior region to the curve. Unbounded component is called
//! exterior region to the curve. The domain of the trimmed
//! surface is defined as the interior of the outer boundaries
//! and exterior of the inner boundaries and includes the
//! boundary curves.
class IGESGeom_TrimmedSurface : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESGeom_TrimmedSurface();
  
  //! This method is used to set the fields of the class
  //! TrimmedSurface
  //! - aSurface  : Surface to be trimmed
  //! - aFlag     : Outer boundary type
  //! False = The outer boundary is the boundary of
  //! rectangle D which is the domain of the
  //! surface to be trimmed
  //! True  = otherwise
  //! - anOuter   : Closed curve which constitutes outer boundary
  //! - allInners : Array of closed curves which constitute the
  //! inner boundary
  Standard_EXPORT void Init (const Handle(IGESData_IGESEntity)& aSurface, const Standard_Integer aFlag, const Handle(IGESGeom_CurveOnSurface)& anOuter, const Handle(IGESGeom_HArray1OfCurveOnSurface)& allInners);
  
  //! returns the surface to be trimmed
  Standard_EXPORT Handle(IGESData_IGESEntity) Surface() const;
  
  //! returns True if outer contour exists
  Standard_EXPORT Standard_Boolean HasOuterContour() const;
  
  //! returns the outer contour of the trimmed surface
  Standard_EXPORT Handle(IGESGeom_CurveOnSurface) OuterContour() const;
  
  //! returns the outer contour type of the trimmed surface
  //! 0  : The outer boundary is the boundary of D
  //! 1  : otherwise
  Standard_EXPORT Standard_Integer OuterBoundaryType() const;
  
  //! returns the number of inner boundaries
  Standard_EXPORT Standard_Integer NbInnerContours() const;
  
  //! returns the Index'th inner contour
  //! raises exception if Index <= 0 or Index > NbInnerContours()
  Standard_EXPORT Handle(IGESGeom_CurveOnSurface) InnerContour (const Standard_Integer Index) const;




  DEFINE_STANDARD_RTTIEXT(IGESGeom_TrimmedSurface,IGESData_IGESEntity)

protected:




private:


  Handle(IGESData_IGESEntity) theSurface;
  Standard_Integer theFlag;
  Handle(IGESGeom_CurveOnSurface) theOuterCurve;
  Handle(IGESGeom_HArray1OfCurveOnSurface) theInnerCurves;


};







#endif // _IGESGeom_TrimmedSurface_HeaderFile
