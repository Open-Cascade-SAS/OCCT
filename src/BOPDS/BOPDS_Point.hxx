// Created by: Peter KURNEV
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

#ifndef _BOPDS_Point_HeaderFile
#define _BOPDS_Point_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_Integer.hxx>
class gp_Pnt;
class gp_Pnt2d;



//! The class BOPDS_Point is to store
//! the information about intersection point
class BOPDS_Point 
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! Empty contructor
    BOPDS_Point();
virtual ~BOPDS_Point();
  

  //! Modifier
  //! Sets 3D point <thePnt>
    void SetPnt (const gp_Pnt& thePnt);
  

  //! Selector
  //! Returns 3D point
    const gp_Pnt& Pnt() const;
  

  //! Modifier
  //! Sets 2D point on the first face <thePnt>
    void SetPnt2D1 (const gp_Pnt2d& thePnt);
  

  //! Selector
  //! Returns 2D point on the first face <thePnt>
    const gp_Pnt2d& Pnt2D1() const;
  

  //! Modifier
  //! Sets 2D point on the second face <thePnt>
    void SetPnt2D2 (const gp_Pnt2d& thePnt);
  

  //! Selector
  //! Returns 2D point on the second face <thePnt>
    const gp_Pnt2d& Pnt2D2() const;
  

  //! Modifier
  //! Sets the index of the vertex <theIndex>
    void SetIndex (const Standard_Integer theIndex);
  

  //! Selector
  //! Returns index of the vertex
    Standard_Integer Index() const;




protected:



  gp_Pnt myPnt;
  gp_Pnt2d myPnt2D1;
  gp_Pnt2d myPnt2D2;
  Standard_Integer myIndex;


private:





};


#include <BOPDS_Point.lxx>





#endif // _BOPDS_Point_HeaderFile
