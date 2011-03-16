#include <Vrml_PointSet.ixx>

 Vrml_PointSet::Vrml_PointSet(const Standard_Integer aStartIndex, 
			      const Standard_Integer aNumPoints)
{
 myStartIndex = aStartIndex;
 myNumPoints  = aNumPoints;
}

void Vrml_PointSet::SetStartIndex(const Standard_Integer aStartIndex)
{
 myStartIndex = aStartIndex;
}

Standard_Integer Vrml_PointSet::StartIndex() const 
{
 return myStartIndex;
}

void Vrml_PointSet::SetNumPoints(const Standard_Integer aNumPoints)
{
 myNumPoints  = aNumPoints;
}

Standard_Integer Vrml_PointSet::NumPoints() const 
{
 return myNumPoints;
}

Standard_OStream& Vrml_PointSet::Print(Standard_OStream& anOStream) const 
{
 anOStream  << "PointSet {" << endl;
 if ( myStartIndex != 0 || myNumPoints !=-1 )
  {
    if ( myStartIndex != 0)
      {
	anOStream  << "    startIndex" << '\t';
	anOStream << myStartIndex << endl;
      }
    if ( myNumPoints != 0)
      {
	anOStream  << "    numPoints" << '\t';
	anOStream << myNumPoints << endl;
      }
  }
 anOStream  << '}' << endl;
 return anOStream;
}

