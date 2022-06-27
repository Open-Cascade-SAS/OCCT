// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
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

#ifndef _StepShape_OrientedPath_HeaderFile
#define _StepShape_OrientedPath_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepShape_Path.hxx>
#include <StepShape_HArray1OfOrientedEdge.hxx>
#include <Standard_Integer.hxx>
class StepShape_EdgeLoop;
class TCollection_HAsciiString;
class StepShape_OrientedEdge;


class StepShape_OrientedPath;
DEFINE_STANDARD_HANDLE(StepShape_OrientedPath, StepShape_Path)


class StepShape_OrientedPath : public StepShape_Path
{

public:

  
  //! Returns a OrientedPath
  Standard_EXPORT StepShape_OrientedPath();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(StepShape_EdgeLoop)& aPathElement, const Standard_Boolean aOrientation);
  
  Standard_EXPORT void SetPathElement (const Handle(StepShape_EdgeLoop)& aPathElement);
  
  Standard_EXPORT Handle(StepShape_EdgeLoop) PathElement() const;
  
  Standard_EXPORT void SetOrientation (const Standard_Boolean aOrientation);
  
  Standard_EXPORT Standard_Boolean Orientation() const;
  
  Standard_EXPORT virtual void SetEdgeList (const Handle(StepShape_HArray1OfOrientedEdge)& aEdgeList) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Handle(StepShape_HArray1OfOrientedEdge) EdgeList() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Handle(StepShape_OrientedEdge) EdgeListValue (const Standard_Integer num) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Integer NbEdgeList() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(StepShape_OrientedPath,StepShape_Path)

protected:




private:


  Handle(StepShape_EdgeLoop) pathElement;
  Standard_Boolean orientation;


};







#endif // _StepShape_OrientedPath_HeaderFile
