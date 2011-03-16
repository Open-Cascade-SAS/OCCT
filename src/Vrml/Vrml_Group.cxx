#include <Vrml_Group.ixx>

 Vrml_Group::Vrml_Group()
{
  myFlagPrint = 0;
}

Standard_OStream& Vrml_Group::Print(Standard_OStream& anOStream)
{
  if ( myFlagPrint == 0 )
    {
      anOStream  << "Group {" << endl;
      myFlagPrint = 1;
    } //End of if
  else 
    {
     anOStream  << '}' << endl;
     myFlagPrint = 0;
    }
 return anOStream;
}

