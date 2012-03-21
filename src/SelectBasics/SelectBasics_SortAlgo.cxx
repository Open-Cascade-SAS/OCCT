// Created on: 1994-04-18
// Created by: Didier PIFFAULT
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <SelectBasics_SortAlgo.ixx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <gp_Pnt2d.hxx>

//=======================================================================
//function : SelectBasics_SortAlgo
//purpose  : 
//=======================================================================
SelectBasics_SortAlgo::SelectBasics_SortAlgo()
     : sizeArea(0.)
{}

//=======================================================================
//function : SelectBasics_SortAlgo
//purpose  : 
//=======================================================================
SelectBasics_SortAlgo::SelectBasics_SortAlgo 
  (const Bnd_Box2d& ClippingRectangle,
   const Standard_Real sizeOfSensitiveArea, 
   const Handle_Bnd_HArray1OfBox2d& theRectangles)
: clipRect(ClippingRectangle), sizeArea(sizeOfSensitiveArea)
{
  sortedRect.Initialize(clipRect, theRectangles);
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================
void SelectBasics_SortAlgo::Initialize(const Bnd_Box2d& ClippingRectangle, 
				  const Standard_Real sizeOfSensitiveArea, 
				  const Handle_Bnd_HArray1OfBox2d& theRectangles)
{
  clipRect=ClippingRectangle;
  sizeArea=sizeOfSensitiveArea;
  sortedRect.Initialize(clipRect, theRectangles);
}

//=======================================================================
//function : Select
//purpose  : 
//=======================================================================
void SelectBasics_SortAlgo::InitSelect(const Standard_Real x,
				  const Standard_Real y)
{
  Bnd_Box2d rep;
  rep.Set(gp_Pnt2d(x, y));
  rep.Enlarge(sizeArea);
  myMap.Clear() ;
  TColStd_ListIteratorOfListOfInteger It(sortedRect.Compare(rep));
  for(;It.More();It.Next()){
    myMap.Add(It.Value());
  }
  curResult.Initialize(myMap);
}

//=======================================================================
//function : Select
//purpose  : 
//=======================================================================
void SelectBasics_SortAlgo::InitSelect(const Bnd_Box2d& rect)
{
  myMap.Clear() ;
  TColStd_ListIteratorOfListOfInteger It(sortedRect.Compare(rect));
  for(;It.More();It.Next()){
    myMap.Add(It.Value());
  }
  curResult.Initialize(myMap);

}

//=======================================================================
//function : More
//purpose  : 
//=======================================================================
Standard_Boolean SelectBasics_SortAlgo::More()  const
{
  return curResult.More();
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================
void SelectBasics_SortAlgo::Next() 
{
  curResult.Next();
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================
Standard_Integer SelectBasics_SortAlgo::Value() const
{
  return curResult.Key();
}

