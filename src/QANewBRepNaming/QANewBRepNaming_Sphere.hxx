// Created on: 1997-08-22
// Created by: VAUTHIER Jean-Claude
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

#ifndef _QANewBRepNaming_Sphere_HeaderFile
#define _QANewBRepNaming_Sphere_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <QANewBRepNaming_TopNaming.hxx>
#include <QANewBRepNaming_TypeOfPrimitive3D.hxx>
class TDF_Label;
class BRepPrimAPI_MakeSphere;


//! To load the Sphere results
class QANewBRepNaming_Sphere  : public QANewBRepNaming_TopNaming
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT QANewBRepNaming_Sphere();
  
  Standard_EXPORT QANewBRepNaming_Sphere(const TDF_Label& ResultLabel);
  
  Standard_EXPORT void Init (const TDF_Label& ResultLabel);
  
  Standard_EXPORT void Load (BRepPrimAPI_MakeSphere& mkSphere, const QANewBRepNaming_TypeOfPrimitive3D Type) const;
  
  //! Returns the label of the bottom
  //! face of the Sphere.
  Standard_EXPORT TDF_Label Bottom() const;
  
  //! Returns the label of the top
  //! face of the Sphere.
  Standard_EXPORT TDF_Label Top() const;
  
  //! Returns the label of the lateral
  //! face of the Sphere.
  Standard_EXPORT TDF_Label Lateral() const;
  
  //! Returns the label of the first
  //! side of the Sphere.
  Standard_EXPORT TDF_Label StartSide() const;
  
  //! Returns the label of the second
  //! side of the Sphere.
  Standard_EXPORT TDF_Label EndSide() const;
  
  //! Returns the label of the meridian
  //! edges of the Sphere.
  Standard_EXPORT TDF_Label Meridian() const;
  
  //! Returns the label of the degenerated
  //! edges of the Sphere.
  Standard_EXPORT TDF_Label Degenerated() const;




protected:





private:





};







#endif // _QANewBRepNaming_Sphere_HeaderFile
