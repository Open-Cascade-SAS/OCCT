// Created on: 1999-04-06
// Created by: Fabrice SERVANT
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

#ifndef _IntPolyh_SectionLine_HeaderFile
#define _IntPolyh_SectionLine_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <IntPolyh_SeqOfStartPoints.hxx>
class IntPolyh_StartPoint;



class IntPolyh_SectionLine 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT IntPolyh_SectionLine();
  
  Standard_EXPORT IntPolyh_SectionLine(const Standard_Integer nn);

  IntPolyh_SectionLine(const IntPolyh_SectionLine& theOther)
  {
    Copy(theOther);
  }
  
  Standard_EXPORT void Init (const Standard_Integer nn);
  
  Standard_EXPORT const IntPolyh_StartPoint& Value (const Standard_Integer nn) const;
const IntPolyh_StartPoint& operator [] (const Standard_Integer nn) const
{
  return Value(nn);
}
  
  Standard_EXPORT IntPolyh_StartPoint& ChangeValue (const Standard_Integer nn);
IntPolyh_StartPoint& operator [] (const Standard_Integer nn)
{
  return ChangeValue(nn);
}
  
  Standard_EXPORT IntPolyh_SectionLine& Copy (const IntPolyh_SectionLine& Other);
IntPolyh_SectionLine& operator = (const IntPolyh_SectionLine& Other)
{
  return Copy(Other);
}
  
  Standard_EXPORT Standard_Integer GetN() const;
  
  Standard_EXPORT Standard_Integer NbStartPoints() const;
  
  Standard_EXPORT void IncrementNbStartPoints();
  
  Standard_EXPORT void Destroy();
~IntPolyh_SectionLine()
{
  Destroy();
}
  
  Standard_EXPORT void Dump() const;
  
  Standard_EXPORT void Prepend (const IntPolyh_StartPoint& SP);




protected:





private:



  IntPolyh_SeqOfStartPoints mySeqOfSPoints;


};







#endif // _IntPolyh_SectionLine_HeaderFile
