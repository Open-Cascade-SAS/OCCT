// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <SWDRAW_ToVRML.ixx>

#include <Standard_Stream.hxx>
#include <BRepMesh_FastDiscret.hxx>
#include <BRepMesh_Triangle.hxx>
#include <BRepMesh_Edge.hxx>
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>

#include <gp_Pnt.hxx> //ied_modif_for_compil_Nov-20-1998

SWDRAW_ToVRML::SWDRAW_ToVRML  ()
{
  myEmissiveColorRed   = 0.3;
  myEmissiveColorGreen = 0.3;
  myEmissiveColorBlue  = 0.3;
  myDiffuseColorRed    = 0.3;
  myDiffuseColorGreen  = 0.3;
  myDiffuseColorBlue   = 0.5;
  myTransparency       = 0.0;
  myAmbientIntensity   = 0.3;
  mySpecularColorRed   = 0.7;
  mySpecularColorGreen = 0.7;
  mySpecularColorBlue  = 0.8;
  myShininess   = 0.1;
  myTexture     = " [] " ;
  myCreaseAngle = 1.57;
  myDeflection  = 0.005;
}

Standard_Real&  SWDRAW_ToVRML::EmissiveColorRed  ()  {  return myEmissiveColorRed  ;  }
Standard_Real&  SWDRAW_ToVRML::EmissiveColorGreen()  {  return myEmissiveColorGreen;  }
Standard_Real&  SWDRAW_ToVRML::EmissiveColorBlue ()  {  return myEmissiveColorBlue ;  }
Standard_Real&  SWDRAW_ToVRML::DiffuseColorRed   ()  {  return myDiffuseColorRed   ;  }
Standard_Real&  SWDRAW_ToVRML::DiffuseColorGreen ()  {  return myDiffuseColorGreen ;  }
Standard_Real&  SWDRAW_ToVRML::DiffuseColorBlue  ()  {  return myDiffuseColorBlue  ;  }
Standard_Real&  SWDRAW_ToVRML::Transparency      ()  {  return myTransparency      ;  }
Standard_Real&  SWDRAW_ToVRML::AmbientIntensity  ()  {  return myAmbientIntensity  ;  }
Standard_Real&  SWDRAW_ToVRML::SpecularColorRed  ()  {  return mySpecularColorRed  ;  }
Standard_Real&  SWDRAW_ToVRML::SpecularColorGreen()  {  return mySpecularColorGreen;  }
Standard_Real&  SWDRAW_ToVRML::SpecularColorBlue ()  {  return mySpecularColorBlue ;  }
Standard_Real&  SWDRAW_ToVRML::Shininess  ()         {  return myShininess  ;  }
TCollection_AsciiString&  SWDRAW_ToVRML::Texture ()  {  return myTexture    ;  }
Standard_Real&  SWDRAW_ToVRML::CreaseAngle()         {  return myCreaseAngle;  }
Standard_Real&  SWDRAW_ToVRML::Deflection ()         {  return myDeflection ;  }

//=======================================================================
// function : ToVRML::Write
// purpose  : conversion of a Shape into VRML format for 3d visualisation
//=======================================================================

Standard_Boolean  SWDRAW_ToVRML::Write
  (const TopoDS_Shape& aShape, const Standard_CString filename) const
{
  filebuf thefile;
  ostream TheFileOut(&thefile);

  if (thefile.open(filename,ios::out))
    {

      // Creates facets from the shape
//    	Create (defle     : Real    from Standard;
//       	shape     : Shape   from TopoDS;
//    	    	angl      : Real    from Standard= 0.17;
//    	    	withShare : Boolean from Standard=Standard_True;
//    	    	inshape   : Boolean from Standard=Standard_False;
//    	    	relative  : Boolean from Standard=Standard_False;
//    	    	shapetrigu: Boolean from Standard=Standard_False)
//	    returns mutable Discret from BRepMesh;
      Bnd_Box B;
      BRepBndLib::Add(aShape, B);

      Handle(BRepMesh_FastDiscret) 	TheDiscret = 
	new BRepMesh_FastDiscret(myDeflection,
			     aShape,
                             B,
			     0.17,
			     Standard_True,
			     Standard_False,
			     Standard_True,
			     Standard_True);

      Standard_Integer i,j;

      // header of the VRML file
      TheFileOut << "#VRML V2.0 utf8" << endl;
      TheFileOut << "Group {" << endl;
      TheFileOut << " children [ " << endl;
      TheFileOut << " NavigationInfo {" << endl;
      TheFileOut << "       type \"EXAMINE\" " << endl;
      TheFileOut << " }," << endl;
      TheFileOut << "Shape {" << endl;

      TheFileOut << "   appearance Appearance {" << endl;
      TheFileOut << "     texture ImageTexture {" << endl;
      TheFileOut << "         url " << myTexture.ToCString() << endl;
      TheFileOut << "         }" << endl;
      TheFileOut << "     material Material { " << endl;
      TheFileOut << "  diffuseColor " << myDiffuseColorRed << " " << myDiffuseColorGreen << " " << myDiffuseColorBlue << " " << endl;
      TheFileOut << " emissiveColor " << myEmissiveColorRed << " "
	<< myEmissiveColorGreen << " " << myEmissiveColorBlue << " " << endl;
      TheFileOut << " transparency " << myTransparency << endl;
      TheFileOut << " ambientIntensity " << myAmbientIntensity << " " << endl;
      TheFileOut << " specularColor " << mySpecularColorRed << " " << mySpecularColorGreen << " " << mySpecularColorBlue << " " << endl;
      TheFileOut << " shininess " <<myShininess << " " << endl;
      TheFileOut << "         }" << endl;
      TheFileOut << "     }" << endl;

      TheFileOut << "   geometry IndexedFaceSet {" << endl;
      TheFileOut << "     coord Coordinate {" << endl;
      TheFileOut << "       point [" << endl;
      
      // puts the coordinates of all the vertices using the order
      // given during the discretisation
      for (i=1;i<=TheDiscret->NbVertices();i++)
	{
	  gp_Pnt TheVertex=TheDiscret->Pnt(i);
	  TheFileOut << "          " 
	    <<  TheVertex.Coord().X() << " " 
	      << TheVertex.Coord().Y() <<  " " 
		<< TheVertex.Coord().Z() << "," << endl;
	}
      TheFileOut << "       ]" << endl;
      TheFileOut << "     }" << endl;
      
      TheFileOut << "     coordIndex [" << endl;
      
      // retrieves all the triangles in order to draw the facets
      for (j=1; j <= TheDiscret->NbTriangles(); j++)
	{
	  BRepMesh_Triangle TheTri=TheDiscret->Triangle(j);
	  Standard_Integer e1,e2,e3,i1,i2,i3;
	  Standard_Boolean b1,b2,b3;
	  
	  TheTri.Edges(e1,e2,e3,b1,b2,b3);
	  
	  if (b1)
	    {
	      i1 = TheDiscret->Edge(e1).FirstNode()-1;
	      i2 = TheDiscret->Edge(e1).LastNode()-1;
	    }
	  else
	    {
	      i2 = TheDiscret->Edge(e1).FirstNode()-1;
	      i1 = TheDiscret->Edge(e1).LastNode()-1;
	    }

	  if (b2)
	    {
	      i3 = TheDiscret->Edge(e2).LastNode()-1;
	    }
	  else
	    {
	      i3 = TheDiscret->Edge(e2).FirstNode()-1;
	    }
	  
	  TheFileOut << "          " << i1  << ", " << i2 << ", "  << i3  << ", -1, " << endl;
	}
      
      TheFileOut << "          ]" << endl;
      TheFileOut << "     solid FALSE" << endl;      // it is not a closed solid
      TheFileOut << " creaseAngle " << myCreaseAngle << " " << endl; // for smooth shading
      TheFileOut << "     }" << endl;
      TheFileOut << "   }" << endl;
	  TheFileOut << " ]" << endl;
	  TheFileOut << "} " << endl;
      
    }
  else return Standard_False;  // failure when opening file

  thefile.close();

  return Standard_True;
}

