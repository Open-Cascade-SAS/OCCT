// Created on: 1999-09-17
// Created by: Denis PASCAL
// Copyright (c) 1999-1999 Matra Datavision
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



#include <TDocStd_PathParser.ixx>

TDocStd_PathParser::TDocStd_PathParser(const TCollection_ExtendedString& path)
{
	myPath = path;
	Parse();
}

void TDocStd_PathParser::Parse()
{
	TCollection_ExtendedString temp = myPath;
	Standard_Integer PointPosition = myPath.SearchFromEnd(TCollection_ExtendedString("."));
	if (PointPosition>0)
		myExtension = temp.Split(PointPosition);
	else
		return;
	temp.Trunc(PointPosition-1);
	Standard_Boolean isFileName = (temp.Length()) ? Standard_True : Standard_False;
	Standard_Boolean isTrek = Standard_True;
#ifdef WNT
	PointPosition = temp.SearchFromEnd(TCollection_ExtendedString("\\"));
	if (!(PointPosition>0))
		PointPosition = temp.SearchFromEnd(TCollection_ExtendedString("/"));
	if (PointPosition >0) 
	  myName = temp.Split(PointPosition);
	else {
	  if(isFileName) {
	    myName = temp;
	    isTrek = Standard_False;}
	  else
	    return;
	}
#else
	PointPosition = temp.SearchFromEnd(TCollection_ExtendedString("/"));
	if (PointPosition >0) 
	  myName = temp.Split(PointPosition);
	else {
	  if(isFileName) {
	    myName = temp;
	    isTrek = Standard_False;}
	  else
	    return;
	}
#endif //WNT
	if(isTrek) {
	  temp.Trunc(PointPosition-1);
	  myTrek = temp;
	} else 
#ifdef WNT
	  myTrek = ".\\";
#else
	myTrek = "./";
#endif 
}


TCollection_ExtendedString TDocStd_PathParser::Extension() const
{
	return myExtension;
}

TCollection_ExtendedString TDocStd_PathParser::Name() const
{
	return myName;
}

TCollection_ExtendedString TDocStd_PathParser::Trek() const
{
	return myTrek;
}

TCollection_ExtendedString TDocStd_PathParser::Path() const
{
	return myPath;
}


Standard_Integer TDocStd_PathParser::Length() const
{
	return myPath.Length();
}
