// Created by: DAUTRY Philippe
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _TDF_ClosureMode_HeaderFile
#define _TDF_ClosureMode_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>


//! This class provides options closure management.
class TDF_ClosureMode 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates an object with all modes set to <aMode>.
  Standard_EXPORT TDF_ClosureMode(const Standard_Boolean aMode = Standard_True);
  
  //! Sets the mode "Descendants" to <aStatus>.
  //!
  //! "Descendants" mode means we add to the data set
  //! the children labels of each USER GIVEN label. We
  //! do not do that with the labels found applying
  //! UpToFirstLevel option.
    void Descendants (const Standard_Boolean aStatus);
  
  //! Returns true if the mode "Descendants" is set.
    Standard_Boolean Descendants() const;
  
  //! Sets the mode "References" to <aStatus>.
  //!
  //! "References" mode means we add to the data set
  //! the descendants of an attribute, by calling the
  //! attribute method Descendants().
    void References (const Standard_Boolean aStatus);
  
  //! Returns true if the mode "References" is set.
    Standard_Boolean References() const;




protected:





private:



  Standard_Integer myFlags;


};


#include <TDF_ClosureMode.lxx>





#endif // _TDF_ClosureMode_HeaderFile
