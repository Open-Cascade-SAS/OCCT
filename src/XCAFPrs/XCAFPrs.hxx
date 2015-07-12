// Created on: 2000-08-11
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _XCAFPrs_HeaderFile
#define _XCAFPrs_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <XCAFPrs_DataMapOfShapeStyle.hxx>
#include <Standard_Boolean.hxx>
class TDF_Label;
class TopLoc_Location;
class XCAFPrs_Driver;
class XCAFPrs_Style;


//! Presentation (visualiation, selection etc.) tools for
//! DECAF documents
class XCAFPrs 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Collect styles defined for shape on label L
  //! and its components and subshapes and fills a map of
  //! shape - style correspondence
  //! The location <loc> is for internal use, it
  //! should be Null location for external call
  Standard_EXPORT static void CollectStyleSettings (const TDF_Label& L, const TopLoc_Location& loc, XCAFPrs_DataMapOfShapeStyle& settings);
  
  //! Set ViewNameMode for indicate display names or not.
  Standard_EXPORT static void SetViewNameMode (const Standard_Boolean viewNameMode);
  
  Standard_EXPORT static Standard_Boolean GetViewNameMode();




protected:





private:




friend class XCAFPrs_Driver;
friend class XCAFPrs_Style;

};







#endif // _XCAFPrs_HeaderFile
