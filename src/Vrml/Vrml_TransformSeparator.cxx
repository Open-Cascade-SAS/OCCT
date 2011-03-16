#include <Vrml_TransformSeparator.ixx>

Vrml_TransformSeparator::Vrml_TransformSeparator()
{
  myFlagPrint = 0;
}

Standard_OStream& Vrml_TransformSeparator::Print(Standard_OStream& anOStream) 
{
  if ( myFlagPrint == 0 )
    {
      anOStream  << "TransformSeparator {" << endl;
      myFlagPrint = 1;
    } 
  else 
    {
      anOStream  << '}' << endl;
      myFlagPrint = 0;
    }
  return anOStream;
}
