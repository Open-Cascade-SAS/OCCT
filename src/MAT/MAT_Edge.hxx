// Created on: 1992-10-14
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _MAT_Edge_HeaderFile
#define _MAT_Edge_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
class MAT_Bisector;


class MAT_Edge;
DEFINE_STANDARD_HANDLE(MAT_Edge, Standard_Transient)


class MAT_Edge : public Standard_Transient
{

public:

  
  Standard_EXPORT MAT_Edge();
  
  Standard_EXPORT void EdgeNumber (const Standard_Integer anumber);
  
  Standard_EXPORT void FirstBisector (const Handle(MAT_Bisector)& abisector);
  
  Standard_EXPORT void SecondBisector (const Handle(MAT_Bisector)& abisector);
  
  Standard_EXPORT void Distance (const Standard_Real adistance);
  
  Standard_EXPORT void IntersectionPoint (const Standard_Integer apoint);
  
  Standard_EXPORT Standard_Integer EdgeNumber() const;
  
  Standard_EXPORT Handle(MAT_Bisector) FirstBisector() const;
  
  Standard_EXPORT Handle(MAT_Bisector) SecondBisector() const;
  
  Standard_EXPORT Standard_Real Distance() const;
  
  Standard_EXPORT Standard_Integer IntersectionPoint() const;
  
  Standard_EXPORT void Dump (const Standard_Integer ashift, const Standard_Integer alevel) const;




  DEFINE_STANDARD_RTTIEXT(MAT_Edge,Standard_Transient)

protected:




private:


  Standard_Integer theedgenumber;
  Handle(MAT_Bisector) thefirstbisector;
  Handle(MAT_Bisector) thesecondbisector;
  Standard_Real thedistance;
  Standard_Integer theintersectionpoint;


};







#endif // _MAT_Edge_HeaderFile
