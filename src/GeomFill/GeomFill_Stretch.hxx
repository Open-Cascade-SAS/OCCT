// Created on: 1993-09-28
// Created by: Bruno DUMORTIER
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

#ifndef _GeomFill_Stretch_HeaderFile
#define _GeomFill_Stretch_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomFill_Filling.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>



class GeomFill_Stretch  : public GeomFill_Filling
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT GeomFill_Stretch();
  
  Standard_EXPORT GeomFill_Stretch(const TColgp_Array1OfPnt& P1, const TColgp_Array1OfPnt& P2, const TColgp_Array1OfPnt& P3, const TColgp_Array1OfPnt& P4);
  
  Standard_EXPORT GeomFill_Stretch(const TColgp_Array1OfPnt& P1, const TColgp_Array1OfPnt& P2, const TColgp_Array1OfPnt& P3, const TColgp_Array1OfPnt& P4, const TColStd_Array1OfReal& W1, const TColStd_Array1OfReal& W2, const TColStd_Array1OfReal& W3, const TColStd_Array1OfReal& W4);
  
  Standard_EXPORT void Init (const TColgp_Array1OfPnt& P1, const TColgp_Array1OfPnt& P2, const TColgp_Array1OfPnt& P3, const TColgp_Array1OfPnt& P4);
  
  Standard_EXPORT void Init (const TColgp_Array1OfPnt& P1, const TColgp_Array1OfPnt& P2, const TColgp_Array1OfPnt& P3, const TColgp_Array1OfPnt& P4, const TColStd_Array1OfReal& W1, const TColStd_Array1OfReal& W2, const TColStd_Array1OfReal& W3, const TColStd_Array1OfReal& W4);




protected:





private:





};







#endif // _GeomFill_Stretch_HeaderFile
