// Created on: 1999-09-24
// Created by: Sergey ZARITCHNY
// Copyright (c) 1999-1999 Matra Datavision
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

#ifndef _QANewBRepNaming_Box_HeaderFile
#define _QANewBRepNaming_Box_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <QANewBRepNaming_TopNaming.hxx>
#include <QANewBRepNaming_TypeOfPrimitive3D.hxx>
class TDF_Label;
class BRepPrimAPI_MakeBox;


//! To load the Box results
class QANewBRepNaming_Box  : public QANewBRepNaming_TopNaming
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT QANewBRepNaming_Box();
  
  Standard_EXPORT QANewBRepNaming_Box(const TDF_Label& ResultLabel);
  
  Standard_EXPORT void Init (const TDF_Label& ResultLabel);
  
  //! Load  the box in  the data  framework
  Standard_EXPORT void Load (BRepPrimAPI_MakeBox& MakeShape, const QANewBRepNaming_TypeOfPrimitive3D Type) const;
  
  //! Returns the label of the back face of a box .
  Standard_EXPORT TDF_Label Back() const;
  
  //! Returns the label of the  bottom face of a box .
  Standard_EXPORT TDF_Label Bottom() const;
  
  //! Returns the label of the  front face of a box .
  Standard_EXPORT TDF_Label Front() const;
  
  //! Returns the label of the  left face of a box .
  Standard_EXPORT TDF_Label Left() const;
  
  //! Returns the  label of the  right face of a box .
  Standard_EXPORT TDF_Label Right() const;
  
  //! Returns the  label of the  top face of a box .
  Standard_EXPORT TDF_Label Top() const;




protected:





private:





};







#endif // _QANewBRepNaming_Box_HeaderFile
