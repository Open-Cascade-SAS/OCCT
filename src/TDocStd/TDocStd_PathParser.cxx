// File:	TDocStd_PathParser.cxx
// Created:	Fri Sep 17 17:51:42 1999
// Author:	Denis PASCAL
//		<dp@dingox.paris1.matra-dtv.fr>


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
