// Created on: 1998-02-04
// Created by: Christian CAILLET
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

#ifndef _MoniTool_SignShape_HeaderFile
#define _MoniTool_SignShape_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <MoniTool_SignText.hxx>
#include <Standard_CString.hxx>
class TCollection_AsciiString;
class Standard_Transient;


class MoniTool_SignShape;
DEFINE_STANDARD_HANDLE(MoniTool_SignShape, MoniTool_SignText)

//! Signs HShape according to its real content (type of Shape)
//! Context is not used
class MoniTool_SignShape : public MoniTool_SignText
{

public:

  
  Standard_EXPORT MoniTool_SignShape();
  
  //! Returns "SHAPE"
  Standard_EXPORT Standard_CString Name() const Standard_OVERRIDE;
  
  //! Returns for a HShape, the string of its ShapeEnum
  //! The Model is absolutely useless (may be null)
  Standard_EXPORT TCollection_AsciiString Text (const Handle(Standard_Transient)& ent, const Handle(Standard_Transient)& context) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(MoniTool_SignShape,MoniTool_SignText)

protected:




private:




};







#endif // _MoniTool_SignShape_HeaderFile
