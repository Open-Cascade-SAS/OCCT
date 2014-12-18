// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef AppCont_Function_HeaderFile
#define AppCont_Function_HeaderFile

#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>

//! Class describing a continous 3d and/or function f(u).
//! This class must be provided by the user to use the approximation algorithm FittingCurve.
class AppCont_Function
{
public:

  Standard_EXPORT AppCont_Function()
  {
    myNbPnt = -1;
    myNbPnt2d = -1;
  }

  //! Get number of 3d and 2d points returned by "Value" and "D1" functions.
  Standard_EXPORT void GetNumberOfPoints(Standard_Integer& theNbPnt,
                                         Standard_Integer& theNbPnt2d) const
  {
    theNbPnt = myNbPnt;
    theNbPnt2d = myNbPnt2d;
  }

  //! Get number of 3d points returned by "Value" and "D1" functions.
  Standard_EXPORT Standard_Integer GetNbOf3dPoints() const
  {
    return myNbPnt;
  }

  //! Get number of 2d points returned by "Value" and "D1" functions.
  Standard_EXPORT Standard_Integer GetNbOf2dPoints() const
  {
    return myNbPnt2d;
  }

  //! Destructor
  Standard_EXPORT virtual ~AppCont_Function() {}

  //! Returns the first parameter of the function.
  Standard_EXPORT virtual Standard_Real FirstParameter() const = 0;

  //! Returns the last parameter of the function.
  Standard_EXPORT virtual Standard_Real LastParameter() const = 0;

  //! Returns the point at parameter <theU>.
  Standard_EXPORT virtual Standard_Boolean Value(const Standard_Real   theU,
                                                 NCollection_Array1<gp_Pnt2d>& thePnt2d,
                                                 NCollection_Array1<gp_Pnt>&   thePnt) const = 0;

  //! Returns the derivative at parameter <theU>.
  Standard_EXPORT virtual Standard_Boolean D1(const Standard_Real   theU,
                                              NCollection_Array1<gp_Vec2d>& theVec2d,
                                              NCollection_Array1<gp_Vec>&   theVec) const = 0;

  //! Return information about peridicity in output paramateters space. 
  //! @param theDimIdx Defines index in output parameters space. 1 <= theDimIdx <= 3 * myNbPnt + 2 * myNbPnt2d.
  Standard_EXPORT virtual void PeriodInformation(const Standard_Integer /*theDimIdx*/,
                                                 Standard_Boolean&      IsPeriodic,
                                                 Standard_Real&         thePeriod) const
  {
    IsPeriodic = Standard_False;
    thePeriod = 0.0;
  };


protected:
  Standard_Integer myNbPnt;
  Standard_Integer myNbPnt2d;
};

#endif
