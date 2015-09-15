// Created on: 1999-09-17
// Created by: Denis PASCAL
// Copyright (c) 1999-1999 Matra Datavision
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


#include <TCollection_ExtendedString.hxx>
#include <TDocStd_PathParser.hxx>

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
#ifdef _WIN32
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
#endif //_WIN32
	if(isTrek) {
	  temp.Trunc(PointPosition-1);
	  myTrek = temp;
	} else 
#ifdef _WIN32
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
