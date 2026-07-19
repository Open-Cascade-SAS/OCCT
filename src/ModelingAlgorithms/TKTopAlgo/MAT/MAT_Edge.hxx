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

class MAT_Edge : public Standard_Transient
{

public:
  Standard_EXPORT MAT_Edge();

  Standard_EXPORT void EdgeNumber(const int anumber);

  Standard_EXPORT void FirstBisector(const occ::handle<MAT_Bisector>& abisector);

  Standard_EXPORT void SecondBisector(const occ::handle<MAT_Bisector>& abisector);

  Standard_EXPORT void Distance(const double adistance);

  Standard_EXPORT void IntersectionPoint(const int apoint);

  Standard_EXPORT int EdgeNumber() const;

  Standard_EXPORT occ::handle<MAT_Bisector> FirstBisector() const;

  Standard_EXPORT occ::handle<MAT_Bisector> SecondBisector() const;

  Standard_EXPORT double Distance() const;

  Standard_EXPORT int IntersectionPoint() const;

  Standard_EXPORT void Dump(const int ashift, const int alevel) const;

  DEFINE_STANDARD_RTTIEXT(MAT_Edge, Standard_Transient)

private:
  int                       theedgenumber;
  occ::handle<MAT_Bisector> thefirstbisector;
  occ::handle<MAT_Bisector> thesecondbisector;
  double                    thedistance;
  int                       theintersectionpoint;
};

#endif // _MAT_Edge_HeaderFile
