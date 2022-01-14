// Created on: 1993-10-07
// Created by: Yves FRICAUD
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _MAT2d_Connexion_HeaderFile
#define _MAT2d_Connexion_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_Transient.hxx>


class MAT2d_Connexion;
DEFINE_STANDARD_HANDLE(MAT2d_Connexion, Standard_Transient)

//! A Connexion links two lines of items  in a set
//! of  lines. It s contains two  points and their paramatric
//! definitions on the lines.
//! The items can be points or curves.
class MAT2d_Connexion : public Standard_Transient
{

public:

  
  Standard_EXPORT MAT2d_Connexion();
  
  Standard_EXPORT MAT2d_Connexion(const Standard_Integer LineA, const Standard_Integer LineB, const Standard_Integer ItemA, const Standard_Integer ItemB, const Standard_Real Distance, const Standard_Real ParameterOnA, const Standard_Real ParameterOnB, const gp_Pnt2d& PointA, const gp_Pnt2d& PointB);
  
  //! Returns the Index on the first line.
  Standard_EXPORT Standard_Integer IndexFirstLine() const;
  
  //! Returns the Index on the Second line.
  Standard_EXPORT Standard_Integer IndexSecondLine() const;
  
  //! Returns the Index of the item on the first line.
  Standard_EXPORT Standard_Integer IndexItemOnFirst() const;
  
  //! Returns the Index of the item on the second line.
  Standard_EXPORT Standard_Integer IndexItemOnSecond() const;
  
  //! Returns the parameter of the point on the firstline.
  Standard_EXPORT Standard_Real ParameterOnFirst() const;
  
  //! Returns the parameter of the point on the secondline.
  Standard_EXPORT Standard_Real ParameterOnSecond() const;
  
  //! Returns the point on the firstline.
  Standard_EXPORT gp_Pnt2d PointOnFirst() const;
  
  //! Returns the point on the secondline.
  Standard_EXPORT gp_Pnt2d PointOnSecond() const;
  
  //! Returns the distance between the two points.
  Standard_EXPORT Standard_Real Distance() const;
  
  Standard_EXPORT void IndexFirstLine (const Standard_Integer anIndex);
  
  Standard_EXPORT void IndexSecondLine (const Standard_Integer anIndex);
  
  Standard_EXPORT void IndexItemOnFirst (const Standard_Integer anIndex);
  
  Standard_EXPORT void IndexItemOnSecond (const Standard_Integer anIndex);
  
  Standard_EXPORT void ParameterOnFirst (const Standard_Real aParameter);
  
  Standard_EXPORT void ParameterOnSecond (const Standard_Real aParameter);
  
  Standard_EXPORT void PointOnFirst (const gp_Pnt2d& aPoint);
  
  Standard_EXPORT void PointOnSecond (const gp_Pnt2d& aPoint);
  
  Standard_EXPORT void Distance (const Standard_Real aDistance);
  
  //! Returns the reverse connexion of <me>.
  //! the firstpoint  is the secondpoint.
  //! the secondpoint is the firstpoint.
  Standard_EXPORT Handle(MAT2d_Connexion) Reverse() const;
  
  //! Returns <True> if my firstPoint is on the same line
  //! than the firstpoint of <aConnexion> and my firstpoint
  //! is after the firstpoint of <aConnexion> on the line.
  //! <aSense> = 1 if <aConnexion> is on the Left of its
  //! firstline, else <aSense> = -1.
  Standard_EXPORT Standard_Boolean IsAfter (const Handle(MAT2d_Connexion)& aConnexion, const Standard_Real aSense) const;
  
  //! Print <me>.
  Standard_EXPORT void Dump (const Standard_Integer Deep = 0, const Standard_Integer Offset = 0) const;




  DEFINE_STANDARD_RTTIEXT(MAT2d_Connexion,Standard_Transient)

protected:




private:


  Standard_Integer lineA;
  Standard_Integer lineB;
  Standard_Integer itemA;
  Standard_Integer itemB;
  Standard_Real distance;
  Standard_Real parameterOnA;
  Standard_Real parameterOnB;
  gp_Pnt2d pointA;
  gp_Pnt2d pointB;


};







#endif // _MAT2d_Connexion_HeaderFile
