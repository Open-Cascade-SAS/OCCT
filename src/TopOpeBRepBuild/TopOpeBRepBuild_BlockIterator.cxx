// File:	TopOpeBRepBuild_BlockIterator.cxx
// Created:	Tue Mar 23 15:37:30 1993
// Author:	Jean Yves LEBEY
//		<jyl@phobox>

#include <TopOpeBRepBuild_BlockIterator.ixx>

//=======================================================================
//function : TopOpeBRepBuild_BlockIterator
//purpose  : 
//=======================================================================

TopOpeBRepBuild_BlockIterator::TopOpeBRepBuild_BlockIterator() :
myLower(0),
myUpper(0),
myValue(1)
{
}

//=======================================================================
//function : TopOpeBRepBuild_BlockIterator
//purpose  : 
//=======================================================================

TopOpeBRepBuild_BlockIterator::TopOpeBRepBuild_BlockIterator
(const Standard_Integer Lower, const Standard_Integer Upper) :
myLower(Lower),
myUpper(Upper),
myValue(Lower)
{
}

