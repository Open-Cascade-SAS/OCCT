// Created on: 1994-11-17
// Created by: Marie Jose MARTZ
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _GeomToIGES_GeomSurface_HeaderFile
#define _GeomToIGES_GeomSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomToIGES_GeomEntity.hxx>
class IGESData_IGESEntity;
class Geom_Surface;
class Geom_BoundedSurface;
class Geom_BSplineSurface;
class Geom_BezierSurface;
class Geom_RectangularTrimmedSurface;
class Geom_ElementarySurface;
class Geom_Plane;
class Geom_CylindricalSurface;
class Geom_ConicalSurface;
class Geom_SphericalSurface;
class Geom_ToroidalSurface;
class Geom_SweptSurface;
class Geom_SurfaceOfLinearExtrusion;
class Geom_SurfaceOfRevolution;
class Geom_OffsetSurface;


//! This class implements the transfer of the Surface Entity from Geom
//! To IGES. These can be :
//! . BoundedSurface
//! * BSplineSurface
//! * BezierSurface
//! * RectangularTrimmedSurface
//! . ElementarySurface
//! * Plane
//! * CylindricalSurface
//! * ConicalSurface
//! * SphericalSurface
//! * ToroidalSurface
//! . SweptSurface
//! * SurfaceOfLinearExtrusion
//! * SurfaceOfRevolution
//! . OffsetSurface
class GeomToIGES_GeomSurface  : public GeomToIGES_GeomEntity
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT GeomToIGES_GeomSurface();
  
  //! Creates a tool GeomSurface ready to run and sets its
  //! fields as GE's.
  Standard_EXPORT GeomToIGES_GeomSurface(const GeomToIGES_GeomEntity& GE);
  
  //! Transfert  a  GeometryEntity which  answer True  to  the
  //! member : BRepToIGES::IsGeomSurface(Geometry).  If this
  //! Entity could not be converted, this member returns a NullEntity.
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferSurface (const Handle(Geom_Surface)& start, const Standard_Real Udeb, const Standard_Real Ufin, const Standard_Real Vdeb, const Standard_Real Vfin);
  
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferSurface (const Handle(Geom_BoundedSurface)& start, const Standard_Real Udeb, const Standard_Real Ufin, const Standard_Real Vdeb, const Standard_Real Vfin);
  
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferSurface (const Handle(Geom_BSplineSurface)& start, const Standard_Real Udeb, const Standard_Real Ufin, const Standard_Real Vdeb, const Standard_Real Vfin);
  
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferSurface (const Handle(Geom_BezierSurface)& start, const Standard_Real Udeb, const Standard_Real Ufin, const Standard_Real Vdeb, const Standard_Real Vfin);
  
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferSurface (const Handle(Geom_RectangularTrimmedSurface)& start, const Standard_Real Udeb, const Standard_Real Ufin, const Standard_Real Vdeb, const Standard_Real Vfin);
  
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferSurface (const Handle(Geom_ElementarySurface)& start, const Standard_Real Udeb, const Standard_Real Ufin, const Standard_Real Vdeb, const Standard_Real Vfin);
  
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferSurface (const Handle(Geom_Plane)& start, const Standard_Real Udeb, const Standard_Real Ufin, const Standard_Real Vdeb, const Standard_Real Vfin);
  
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferSurface (const Handle(Geom_CylindricalSurface)& start, const Standard_Real Udeb, const Standard_Real Ufin, const Standard_Real Vdeb, const Standard_Real Vfin);
  
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferSurface (const Handle(Geom_ConicalSurface)& start, const Standard_Real Udeb, const Standard_Real Ufin, const Standard_Real Vdeb, const Standard_Real Vfin);
  
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferSurface (const Handle(Geom_SphericalSurface)& start, const Standard_Real Udeb, const Standard_Real Ufin, const Standard_Real Vdeb, const Standard_Real Vfin);
  
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferSurface (const Handle(Geom_ToroidalSurface)& start, const Standard_Real Udeb, const Standard_Real Ufin, const Standard_Real Vdeb, const Standard_Real Vfin);
  
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferSurface (const Handle(Geom_SweptSurface)& start, const Standard_Real Udeb, const Standard_Real Ufin, const Standard_Real Vdeb, const Standard_Real Vfin);
  
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferSurface (const Handle(Geom_SurfaceOfLinearExtrusion)& start, const Standard_Real Udeb, const Standard_Real Ufin, const Standard_Real Vdeb, const Standard_Real Vfin);
  
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferSurface (const Handle(Geom_SurfaceOfRevolution)& start, const Standard_Real Udeb, const Standard_Real Ufin, const Standard_Real Vdeb, const Standard_Real Vfin);
  
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferSurface (const Handle(Geom_OffsetSurface)& start, const Standard_Real Udeb, const Standard_Real Ufin, const Standard_Real Vdeb, const Standard_Real Vfin);
  
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferPlaneSurface (const Handle(Geom_Plane)& start, const Standard_Real Udeb, const Standard_Real Ufin, const Standard_Real Vdeb, const Standard_Real Vfin);
  
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferCylindricalSurface (const Handle(Geom_CylindricalSurface)& start, const Standard_Real Udeb, const Standard_Real Ufin, const Standard_Real Vdeb, const Standard_Real Vfin);
  
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferConicalSurface (const Handle(Geom_ConicalSurface)& start, const Standard_Real Udeb, const Standard_Real Ufin, const Standard_Real Vdeb, const Standard_Real Vfin);
  
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferSphericalSurface (const Handle(Geom_SphericalSurface)& start, const Standard_Real Udeb, const Standard_Real Ufin, const Standard_Real Vdeb, const Standard_Real Vfin);
  
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferToroidalSurface (const Handle(Geom_ToroidalSurface)& start, const Standard_Real Udeb, const Standard_Real Ufin, const Standard_Real Vdeb, const Standard_Real Vfin);
  
  //! Returns the value of "TheLength"
  Standard_EXPORT Standard_Real Length() const;
  
  //! Returns Brep mode flag.
  Standard_EXPORT Standard_Boolean GetBRepMode() const;
  
  //! Sets BRep mode flag.
  Standard_EXPORT void SetBRepMode (const Standard_Boolean flag);
  
  //! Returns flag for writing elementary surfaces
  Standard_EXPORT Standard_Boolean GetAnalyticMode() const;
  
  //! Setst flag for writing elementary surfaces
  Standard_EXPORT void SetAnalyticMode (const Standard_Boolean flag);




protected:





private:



  Standard_Real TheLength;
  Standard_Boolean myBRepMode;
  Standard_Boolean myAnalytic;


};







#endif // _GeomToIGES_GeomSurface_HeaderFile
