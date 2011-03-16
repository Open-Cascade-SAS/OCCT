#include <Vrml_Switch.ixx>

Vrml_Switch::Vrml_Switch(const Standard_Integer aWhichChild)
{
  myWhichChild = aWhichChild;
}

 void Vrml_Switch::SetWhichChild(const Standard_Integer aWhichChild) 
{
  myWhichChild = aWhichChild;
}

 Standard_Integer Vrml_Switch::WhichChild() const
{
  return myWhichChild;
}

 Standard_OStream& Vrml_Switch::Print(Standard_OStream& anOStream) const
{
  anOStream  << "Switch {" << endl;
  if ( myWhichChild != -1 )
    {
      anOStream  << "    whichChild" << '\t';
      anOStream << myWhichChild << endl;
    }
 anOStream  << '}' << endl;
 return anOStream;
}
