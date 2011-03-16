#include <AlienImage_AlienImageData.ixx>

AlienImage_AlienImageData::AlienImage_AlienImageData()
{
#ifdef TRACE
  cout << "AlienImage_AlienImageData constructor \n" ;
#endif
}

void AlienImage_AlienImageData::SetName (const TCollection_AsciiString& aName)
{
  myName = aName + TCollection_AsciiString ("\0");
}

const TCollection_AsciiString& AlienImage_AlienImageData::Name () const
{
  return myName;
}
 
