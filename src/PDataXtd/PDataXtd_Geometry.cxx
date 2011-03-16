// File:	PDataStd_Geometry.cxx
// Created:	Wed Nov 19 15:50:37 1997
// Author:	Denis PASCAL
//		<dp@dingox.paris1.matra-dtv.fr>


#include <PDataXtd_Geometry.ixx>

PDataXtd_Geometry::PDataXtd_Geometry () { }

PDataXtd_Geometry::PDataXtd_Geometry (const Standard_Integer Type) : myType (Type) {}

Standard_Integer PDataXtd_Geometry::GetType() const 
{ return myType; }

void PDataXtd_Geometry::SetType(const Standard_Integer Type) 
{ myType = Type; }
