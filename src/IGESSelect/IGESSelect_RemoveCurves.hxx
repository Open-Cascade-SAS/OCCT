// Created on: 1999-02-24
// Created by: Christian CAILLET
// Copyright (c) 1999 Matra Datavision
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

#ifndef _IGESSelect_RemoveCurves_HeaderFile
#define _IGESSelect_RemoveCurves_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <IGESSelect_ModelModifier.hxx>
class IFSelect_ContextModif;
class IGESData_IGESModel;
class Interface_CopyTool;
class TCollection_AsciiString;


class IGESSelect_RemoveCurves;
DEFINE_STANDARD_HANDLE(IGESSelect_RemoveCurves, IGESSelect_ModelModifier)

//! Removes Curves UV or 3D (not both !) from Faces, those
//! designated by the Selection. No Selection means all the file
class IGESSelect_RemoveCurves : public IGESSelect_ModelModifier
{

public:

  
  //! Creates a RemoveCurves from Faces (141/142/143/144)
  //! UV True  : Removes UV Curves (pcurves)
  //! UV False : Removes 3D Curves
  Standard_EXPORT IGESSelect_RemoveCurves(const Standard_Boolean UV);
  
  //! Specific action : Removes the Curves
  Standard_EXPORT void Performing (IFSelect_ContextModif& ctx, const Handle(IGESData_IGESModel)& target, Interface_CopyTool& TC) const Standard_OVERRIDE;
  
  //! Returns a text which is
  //! "Remove Curves UV on Face"  or  "Remove Curves 3D on Face"
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IGESSelect_RemoveCurves,IGESSelect_ModelModifier)

protected:




private:


  Standard_Boolean theUV;


};







#endif // _IGESSelect_RemoveCurves_HeaderFile
