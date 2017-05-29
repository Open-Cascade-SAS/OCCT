// Created on: 2017-06-27
// Created by: Andrey Betenev
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef _TopoDS_AlertWithShape_HeaderFile
#define _TopoDS_AlertWithShape_HeaderFile

#include <Message_Alert.hxx>
#include <TopoDS_Shape.hxx>

//! Alert object storing TopoDS shape in its field
class TopoDS_AlertWithShape : public Message_Alert 
{
public:
  //! Constructor with shape argument
  Standard_EXPORT TopoDS_AlertWithShape (const TopoDS_Shape& theShape);

  //! Returns contained shape
  const TopoDS_Shape& GetShape() const { return myShape; }
  
  //! Returns false.
  virtual Standard_EXPORT Standard_Boolean SupportsMerge () const Standard_OVERRIDE;

  //! Returns false.
  virtual Standard_EXPORT Standard_Boolean Merge (const Handle(Message_Alert)& theTarget) Standard_OVERRIDE;

  // OCCT RTTI
  DEFINE_STANDARD_RTTIEXT(TopoDS_AlertWithShape, Message_Alert)

private:
  TopoDS_Shape myShape;
};

//! Helper macro allowing to define alert with shape argument in one line of code
#define DEFINE_ALERT_WITH_SHAPE(Alert) \
  class Alert : public TopoDS_AlertWithShape \
  { \
  public:\
    Alert (const TopoDS_Shape& theShape) : TopoDS_AlertWithShape(theShape) {} \
    DEFINE_STANDARD_RTTI_INLINE(Alert, TopoDS_AlertWithShape) \
  };

#endif // _TopoDS_AlertWithShape_HeaderFile
