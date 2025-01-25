// Copyright (c) 2022 OPEN CASCADE SAS
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

#ifndef _BRepFill_ThruSectionErrorStatus_HeaderFile
#define _BRepFill_ThruSectionErrorStatus_HeaderFile

//! Errors that can occur at thrusection algorithm.
enum BRepFill_ThruSectionErrorStatus
{
  BRepFill_ThruSectionErrorStatus_Done,            //!< Thrusection algorithm is done
  BRepFill_ThruSectionErrorStatus_NotDone,         //!< Thrusection algorithm is not done
  BRepFill_ThruSectionErrorStatus_NotSameTopology, //!< All profiles have not same topology (they
                                                   //!< should be all closed or all opened)
  BRepFill_ThruSectionErrorStatus_ProfilesInconsistent, //!< Profiles are inconsistent
  BRepFill_ThruSectionErrorStatus_WrongUsage,           //!< Wrong usage of punctual sections
  BRepFill_ThruSectionErrorStatus_Null3DCurve,          //!< Null 3D curve in edge
  BRepFill_ThruSectionErrorStatus_Failed                //!< Thrusection algorithm has failed
};

#endif // _BRepFill_ThruSectionErrorStatus_HeaderFile
