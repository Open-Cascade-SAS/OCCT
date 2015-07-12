// Created on: 1991-06-24
// Created by: Didier PIFFAULT
// Copyright (c) 1991-1999 Matra Datavision
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


#include <Intf_SectionPoint.hxx>
#include <Intf_TangentZone.hxx>
#include <Standard_OutOfRange.hxx>

#define  DEBUG_TANGENTZONE 0 
//=======================================================================
//function : Intf_TangentZone
//purpose  : Constructor of an empty tangent zone.
//=======================================================================

Intf_TangentZone::Intf_TangentZone       ()
{
  ParamOnFirstMin   = ParamOnSecondMin  = RealLast();
  ParamOnFirstMax   = ParamOnSecondMax  = RealFirst();
}


//=======================================================================
//function : Intf_TangentZone
//purpose  : Copy
//=======================================================================

Intf_TangentZone::Intf_TangentZone (const Intf_TangentZone& Other)
{
  Result=Other.Result;
  ParamOnFirstMin   = Other.ParamOnFirstMin;
  ParamOnFirstMax   = Other.ParamOnFirstMax;
  ParamOnSecondMin  = Other.ParamOnSecondMin;
  ParamOnSecondMax  = Other.ParamOnSecondMax;
}
//=======================================================================
//function : Append
//purpose  : Append the section point to the tangent zone.
//=======================================================================

void Intf_TangentZone::Append            (const Intf_SectionPoint& Pi)
{
  Result.Append(Pi);
  if(ParamOnFirstMin  > Pi.ParamOnFirst())   ParamOnFirstMin  = Pi.ParamOnFirst();
  if(ParamOnSecondMin > Pi.ParamOnSecond())  ParamOnSecondMin = Pi.ParamOnSecond();

  if(ParamOnFirstMax  < Pi.ParamOnFirst())   ParamOnFirstMax  = Pi.ParamOnFirst();
  if(ParamOnSecondMax < Pi.ParamOnSecond())  ParamOnSecondMax = Pi.ParamOnSecond();
}


//=======================================================================
//function : Append
//purpose  : Merge the TangentZone to the tangent zone.
//=======================================================================

void Intf_TangentZone::Append            (const Intf_TangentZone& Tzi)
{
  Standard_Integer Tzi_NumberOfPoints = Tzi.NumberOfPoints();
  for (Standard_Integer ipi=1; ipi<=Tzi_NumberOfPoints; ipi++) {
    PolygonInsert(Tzi.GetPoint(ipi));
  }
}


//=======================================================================
//function : Insert
//purpose  : Insert the section point at his place in the tangent zone.
//=======================================================================

//Standard_Boolean Intf_TangentZone::Insert (const Intf_SectionPoint& Pi)
Standard_Boolean Intf_TangentZone::Insert (const Intf_SectionPoint& )
{
#if DEBUG_TANGENTZONE
  cout<<" Standard_Boolean Intf_TangentZone::Insert (const Intf_SectionPoint& Pi) ???? "<<endl;
#endif
  Standard_Boolean Inserted=Standard_False;
/*
  Standard_Integer lpon=0;

  if (Result.Length()<1) {
    Append(Pi);
    Inserted=Standard_True;
  }
  else if (Result.Length()==1) {
    if (Pi.IsOnSameEdge(Result(1))) {
      InsertAfter(1, Pi);
      Inserted=Standard_True;
    }
  }
  else {
    Standard_Integer lp1, lp2;
    Standard_Integer nbptz=NumberOfPoints();
    for (lp1=1; lp1<=nbptz; lp1++) {
      lp2=(lp1%nbptz)+1;
      if (Pi.IsOnSameEdge(Result(lp1))) {
	lpon=lp1;
	if (Pi.IsOnSameEdge(Result(lp2))) {
	  InsertAfter(lp1, Pi);
	  Inserted=Standard_True;
	  break;
	}
      }
    }
  }
  if (!Inserted && lpon>0) {
    InsertAfter(lpon, Pi);
    Inserted=Standard_True;
  }
*/
  return Inserted;
}

//=======================================================================
//function : PolygonInsert
//purpose  : Insert the point at his place in the polygonal tangent zone.
//=======================================================================

void Intf_TangentZone::PolygonInsert (const Intf_SectionPoint& Pi)
{
//  Standard_Boolean  Inserted=Standard_False;
  Standard_Integer nbpTz=NumberOfPoints();
//  Standard_Integer lpi;
  if(nbpTz==0) { 
    Append(Pi);
    return;
  }
  if(Pi.ParamOnFirst() >= ParamOnFirstMax) { 
    Append(Pi);
  }
  else if(Pi.ParamOnFirst() >= ParamOnFirstMin) { 
    InsertBefore(1,Pi);
  }
  else {
/*----- Trop Long lbr le 13 mai 97  
    Standard_Real PiParamOnFirst  = Pi.ParamOnFirst();
    Standard_Real PiParamOnSecond = Pi.ParamOnSecond();
    for (lpi=1; lpi<=nbpTz; lpi++) {
      const Intf_SectionPoint& Resultlpi = Result(lpi);
      if (PiParamOnFirst<Resultlpi.ParamOnFirst()) {
	InsertBefore(lpi, Pi);
	Inserted=Standard_True;
	break;
      }
      else if (PiParamOnFirst==Resultlpi.ParamOnFirst()) {
	if (PiParamOnSecond==Resultlpi.ParamOnSecond()) {
	  Inserted=Standard_True;
	  break;
	}
      }
    }
    if (!Inserted) {
      Append(Pi);
    }
------ */
    Append(Pi); //-- On met les points sans les classer 
                //-- si on veut on pourra les reclasser 
                //-- ensuite avec un TRI. 
  }
}

//=======================================================================
//function : InsertAfter
//purpose  : 
//=======================================================================

void  Intf_TangentZone::InsertAfter(const Standard_Integer Index,
				    const Intf_SectionPoint& Pi)
{
  Result.InsertAfter(Index, Pi);
  if(ParamOnFirstMin  > Pi.ParamOnFirst())   ParamOnFirstMin  = Pi.ParamOnFirst();
  if(ParamOnSecondMin > Pi.ParamOnSecond())  ParamOnSecondMin = Pi.ParamOnSecond();

  if(ParamOnFirstMax  < Pi.ParamOnFirst())   ParamOnFirstMax  = Pi.ParamOnFirst();
  if(ParamOnSecondMax < Pi.ParamOnSecond())  ParamOnSecondMax = Pi.ParamOnSecond();
}

//=======================================================================
//function : InsertBefore
//purpose  : 
//=======================================================================

void  Intf_TangentZone::InsertBefore(const Standard_Integer Index,
				     const Intf_SectionPoint& Pi)
{
  Result.InsertBefore(Index, Pi);
  if(ParamOnFirstMin  > Pi.ParamOnFirst())   ParamOnFirstMin  = Pi.ParamOnFirst();
  if(ParamOnSecondMin > Pi.ParamOnSecond())  ParamOnSecondMin = Pi.ParamOnSecond();
  
  if(ParamOnFirstMax  < Pi.ParamOnFirst())   ParamOnFirstMax  = Pi.ParamOnFirst();
  if(ParamOnSecondMax < Pi.ParamOnSecond())  ParamOnSecondMax = Pi.ParamOnSecond();
}


//=======================================================================
//function : GetPoint
//purpose  : Return the section point of range index in the tangent zone.
//=======================================================================

const Intf_SectionPoint& Intf_TangentZone::GetPoint 
  (const Standard_Integer Index) const
{
  return Result(Index);
}



//=======================================================================
//function : IsEqual
//purpose  : Compare two tangent zone.
//=======================================================================

Standard_Boolean Intf_TangentZone::IsEqual
  (const Intf_TangentZone& Other) const
{
  if (Result.Length() != Other.Result.Length())
    return Standard_False;
  Standard_Integer i;
  for (i = 1; i <= Result.Length(); i++) {
    if (!Result(i).IsEqual(Other.Result(i)))
      return Standard_False;
  }
  return Standard_True;
}


//=======================================================================
//function : Contains
//purpose  : 
//=======================================================================

Standard_Boolean Intf_TangentZone::Contains
 (const Intf_SectionPoint& ThePI) const
{
  Standard_Integer i;
  for (i = 1; i <= Result.Length(); i++)
    if (ThePI.IsEqual(Result(i)))
      return Standard_True;
  return Standard_False;
}
//=======================================================================
//function : InfoFirst
//purpose  : 
//=======================================================================

void Intf_TangentZone::InfoFirst(Standard_Integer& segMin,
				 Standard_Real& paraMin, 
				 Standard_Integer& segMax,
				 Standard_Real& paraMax) const
{
  ParamOnFirst(paraMin, paraMax);
  segMin  = (Standard_Integer)(IntegerPart(paraMin));
  paraMin = paraMin-(Standard_Real)segMin;
  segMax  = (Standard_Integer)(IntegerPart(paraMax));
  paraMax = paraMax-(Standard_Real)segMax;
}


//=======================================================================
//function : InfoSecond
//purpose  : 
//=======================================================================

void Intf_TangentZone::InfoSecond(Standard_Integer& segMin,
				  Standard_Real& paraMin, 
				  Standard_Integer& segMax,
				  Standard_Real& paraMax) const
{
  ParamOnSecond(paraMin, paraMax);
  segMin  = (Standard_Integer)(IntegerPart(paraMin));
  paraMin = paraMin-(Standard_Real)segMin;
  segMax  = (Standard_Integer)(IntegerPart(paraMax));
  paraMax = paraMax-(Standard_Real)segMax;
}


//=======================================================================
//function : RangeContains
//purpose  : 
//=======================================================================

Standard_Boolean Intf_TangentZone::RangeContains
 (const Intf_SectionPoint& ThePI) const
{
  Standard_Real a, b, c, d;
  ParamOnFirst(a, b);
  ParamOnSecond(c, d);
  if (a<=ThePI.ParamOnFirst() && ThePI.ParamOnFirst()<=b &&
      c<=ThePI.ParamOnSecond()&& ThePI.ParamOnSecond()<=d )
    return Standard_True;
  return Standard_False;
}


//=======================================================================
//function : HasCommonRange
//purpose  : 
//=======================================================================

Standard_Boolean Intf_TangentZone::HasCommonRange
 (const Intf_TangentZone& Other) const
{
  Standard_Real a1,b1,c1,d1;
  Standard_Real a2,b2,c2,d2;
  ParamOnFirst(a1, b1);
  ParamOnSecond(a2, b2);
  Other.ParamOnFirst(c1, d1);
  Other.ParamOnSecond(c2, d2);

  if (((c1<=a1 && a1<=d1) || (c1<=b1 && b1<=d1) || (a1<=c1 && c1<=b1)) &&
      ((c2<=a2 && a2<=d2) || (c2<=b2 && b2<=d2) || (a2<=c2 && c2<=b2)))
    return Standard_True;
  return Standard_False;
}


//=======================================================================
//function : Dump
//purpose  : Dump the TangentZone.
//=======================================================================

void Intf_TangentZone::Dump (const Standard_Integer /*Indent*/) const
{
#if DEBUG_TANGENTZONE
  for (Standard_Integer id=0; id<Indent; id++) cout << " ";
  cout << "TZ \n" ;
  cout<<"  ParamOnFirstMin Max    : "<<ParamOnFirstMin<<" "<<ParamOnFirstMax<<endl;
  cout<<"  ParamOnSecondMin Max   : "<<ParamOnSecondMin<<" "<<ParamOnSecondMax<<endl;
  for (Standard_Integer p=1; p<=Result.Length(); p++) {
    Result(p).Dump(Indent+2);
  }
#endif
}
