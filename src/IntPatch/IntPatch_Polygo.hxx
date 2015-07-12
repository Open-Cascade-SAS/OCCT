// Created on: 1993-05-06
// Created by: Jacques GOUSSARD
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

#ifndef _IntPatch_Polygo_HeaderFile
#define _IntPatch_Polygo_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Intf_Polygon2d.hxx>
#include <Standard_Integer.hxx>
class Standard_OutOfRange;
class gp_Pnt2d;



class IntPatch_Polygo  : public Intf_Polygon2d
{
public:

  DEFINE_STANDARD_ALLOC

  
    Standard_Real Error() const;
  
  Standard_EXPORT virtual Standard_Integer NbPoints() const = 0;
  
  Standard_EXPORT virtual gp_Pnt2d Point (const Standard_Integer Index) const = 0;
  
  //! Returns the tolerance of the polygon.
    virtual Standard_Real DeflectionOverEstimation() const Standard_OVERRIDE;
  
  //! Returns the number of Segments in the polyline.
    virtual Standard_Integer NbSegments() const Standard_OVERRIDE;
  
  //! Returns the points of the segment <Index> in the Polygon.
    virtual void Segment (const Standard_Integer theIndex, gp_Pnt2d& theBegin, gp_Pnt2d& theEnd) const Standard_OVERRIDE;
  
  Standard_EXPORT void Dump() const;




protected:

  
  Standard_EXPORT IntPatch_Polygo(const Standard_Real theError = 0.0);


  Standard_Real myError;


private:





};


#include <IntPatch_Polygo.lxx>





#endif // _IntPatch_Polygo_HeaderFile
