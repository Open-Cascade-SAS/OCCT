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

#ifndef _BOPDS_Pave_HeaderFile
#define _BOPDS_Pave_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>



//! The class BOPDS_Pave is to store
//! information about vertex on an edge
class BOPDS_Pave 
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! Empty constructor
    BOPDS_Pave();

  //! Modifier
  //! Sets the index of vertex <theIndex>
    void SetIndex (const Standard_Integer theIndex);
  

  //! Selector
  //! Returns the index of vertex
    Standard_Integer Index() const;
  

  //! Modifier
  //! Sets the parameter of vertex <theParameter>
    void SetParameter (const Standard_Real theParameter);
  

  //! Selector
  //! Returns the parameter of vertex
    Standard_Real Parameter() const;
  

  //! Selector
  //! Returns the index of vertex <theIndex>
  //! Returns the parameter of vertex <theParameter>
    void Contents (Standard_Integer& theIndex, Standard_Real& theParameter) const;
  

  //! Query
  //! Returns true if thr parameter od this is less
  //! than the parameter of  <theOther>
    Standard_Boolean IsLess (const BOPDS_Pave& theOther) const;
  Standard_Boolean operator < (const BOPDS_Pave& theOther) const
{
  return IsLess(theOther);
}
  

  //! Query
  //! Returns true if thr parameter od this is equal
  //! to the parameter of  <theOther>
    Standard_Boolean IsEqual (const BOPDS_Pave& theOther) const;
  Standard_Boolean operator == (const BOPDS_Pave& theOther) const
{
  return IsEqual(theOther);
}
  
  Standard_EXPORT void Dump() const;




protected:



  Standard_Integer myIndex;
  Standard_Real myParameter;


private:





};


#include <BOPDS_Pave.lxx>





#endif // _BOPDS_Pave_HeaderFile
