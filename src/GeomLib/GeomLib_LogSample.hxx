// Created on: 1998-09-23
// Created by: Philippe MANGIN
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _GeomLib_LogSample_HeaderFile
#define _GeomLib_LogSample_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <math_FunctionSample.hxx>


class GeomLib_LogSample  : public math_FunctionSample
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT GeomLib_LogSample(const Standard_Real A, const Standard_Real B, const Standard_Integer N);
  
  //! Returns the value of parameter of the point of
  //! range Index : A + ((Index-1)/(NbPoints-1))*B.
  //! An exception is raised if Index<=0 or Index>NbPoints.
  Standard_EXPORT virtual Standard_Real GetParameter (const Standard_Integer Index) const Standard_OVERRIDE;




protected:





private:



  Standard_Real myF;
  Standard_Real myexp;


};







#endif // _GeomLib_LogSample_HeaderFile
