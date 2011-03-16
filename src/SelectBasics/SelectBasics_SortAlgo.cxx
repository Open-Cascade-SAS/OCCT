// File:	SelectBasics_SortAlgo.cxx
// Created:	Mon Apr 18 14:30:38 1994
// Author:	Didier PIFFAULT
//		<dpf@zerox>

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

