#include <Vrml_Separator.ixx>

 Vrml_Separator::Vrml_Separator(const Vrml_SeparatorRenderCulling aRenderCulling)
{
  myRenderCulling = aRenderCulling;
  myFlagPrint = 0;
}

 Vrml_Separator::Vrml_Separator()
{
  myRenderCulling = Vrml_AUTO;
  myFlagPrint = 0;
}

void Vrml_Separator::SetRenderCulling(const Vrml_SeparatorRenderCulling aRenderCulling)
{
  myRenderCulling = aRenderCulling;
}

Vrml_SeparatorRenderCulling Vrml_Separator::RenderCulling() const 
{
  return myRenderCulling;
}

Standard_OStream& Vrml_Separator::Print(Standard_OStream& anOStream) 
{
  if ( myFlagPrint == 0 )
    {
      anOStream  << "Separator {" << endl;
      if ( myRenderCulling != Vrml_AUTO )
	{
	 if ( myRenderCulling == Vrml_ON )
	    anOStream  << "    renderCulling" << "\tON" << endl;
	 else
	    anOStream  << "    renderCulling" << "\tOFF" << endl;
	}
      myFlagPrint = 1;
    } //End of if
  else 
    {
     anOStream  << '}' << endl;
     myFlagPrint = 0;
    }
 return anOStream;
}

