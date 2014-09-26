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

#include <XSDRAWSTLVRML_ToVRML.ixx>

#include <Standard_Stream.hxx>
#include <BRepMesh_FastDiscret.hxx>
#include <BRepMesh_Triangle.hxx>
#include <BRepMesh_Edge.hxx>
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

#include <gp_Pnt.hxx> //ied_modif_for_compil_Nov-20-1998

XSDRAWSTLVRML_ToVRML::XSDRAWSTLVRML_ToVRML  ()
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

Standard_Real&  XSDRAWSTLVRML_ToVRML::EmissiveColorRed  ()  {  return myEmissiveColorRed  ;  }
Standard_Real&  XSDRAWSTLVRML_ToVRML::EmissiveColorGreen()  {  return myEmissiveColorGreen;  }
Standard_Real&  XSDRAWSTLVRML_ToVRML::EmissiveColorBlue ()  {  return myEmissiveColorBlue ;  }
Standard_Real&  XSDRAWSTLVRML_ToVRML::DiffuseColorRed   ()  {  return myDiffuseColorRed   ;  }
Standard_Real&  XSDRAWSTLVRML_ToVRML::DiffuseColorGreen ()  {  return myDiffuseColorGreen ;  }
Standard_Real&  XSDRAWSTLVRML_ToVRML::DiffuseColorBlue  ()  {  return myDiffuseColorBlue  ;  }
Standard_Real&  XSDRAWSTLVRML_ToVRML::Transparency      ()  {  return myTransparency      ;  }
Standard_Real&  XSDRAWSTLVRML_ToVRML::AmbientIntensity  ()  {  return myAmbientIntensity  ;  }
Standard_Real&  XSDRAWSTLVRML_ToVRML::SpecularColorRed  ()  {  return mySpecularColorRed  ;  }
Standard_Real&  XSDRAWSTLVRML_ToVRML::SpecularColorGreen()  {  return mySpecularColorGreen;  }
Standard_Real&  XSDRAWSTLVRML_ToVRML::SpecularColorBlue ()  {  return mySpecularColorBlue ;  }
Standard_Real&  XSDRAWSTLVRML_ToVRML::Shininess  ()         {  return myShininess  ;  }
TCollection_AsciiString&  XSDRAWSTLVRML_ToVRML::Texture ()  {  return myTexture    ;  }
Standard_Real&  XSDRAWSTLVRML_ToVRML::CreaseAngle()         {  return myCreaseAngle;  }
Standard_Real&  XSDRAWSTLVRML_ToVRML::Deflection ()         {  return myDeflection ;  }

//=======================================================================
// function : ToVRML::Write
// purpose  : conversion of a Shape into VRML format for 3d visualisation
//=======================================================================

Standard_Boolean  XSDRAWSTLVRML_ToVRML::Write
  (const TopoDS_Shape& theShape, const Standard_CString theFileName) const
{
  filebuf aFile;
  ostream anOut(&aFile);

  if ( aFile.open(theFileName,ios::out) )
  {
    // Creates facets from the shape
    // Create (defle     : Real    from Standard;
    //         shape     : Shape   from TopoDS;
    //         angl      : Real    from Standard = 0.17;
    //         withShare : Boolean from Standard = Standard_True;
    //         inshape   : Boolean from Standard = Standard_False;
    //         relative  : Boolean from Standard = Standard_False;
    //         shapetrigu: Boolean from Standard = Standard_False)
    //         returns mutable Discret from BRepMesh;

    Bnd_Box aBox;
    BRepBndLib::Add(theShape, aBox);

    Handle(BRepMesh_FastDiscret) aDiscret =
      new BRepMesh_FastDiscret( theShape,
                                myDeflection,
                                0.17,
                                aBox,
                                Standard_True,
                                Standard_False,
                                Standard_True,
                                Standard_True );

      // Header of the VRML file
      anOut << "#VRML V2.0 utf8" << endl;
      anOut << "Group {" << endl;
      anOut << "  children [ " << endl;
      anOut << "    NavigationInfo {" << endl;
      anOut << "      type \"EXAMINE\" " << endl;
      anOut << "    }," << endl;
      anOut << "    Shape {" << endl;

      anOut << "      appearance Appearance {" << endl;
      anOut << "        texture ImageTexture {" << endl;
      anOut << "          url " << myTexture.ToCString() << endl;
      anOut << "        }" << endl;
      anOut << "        material Material {" << endl;
      anOut << "          diffuseColor " << myDiffuseColorRed << " "
                                         << myDiffuseColorGreen << " "
                                         << myDiffuseColorBlue << " " << endl;
      anOut << "          emissiveColor " << myEmissiveColorRed << " "
                                          << myEmissiveColorGreen << " "
                                          << myEmissiveColorBlue << " " << endl;
      anOut << "          transparency " << myTransparency << endl;
      anOut << "          ambientIntensity " << myAmbientIntensity << " " << endl;
      anOut << "          specularColor " << mySpecularColorRed << " "
                                          << mySpecularColorGreen << " "
                                          << mySpecularColorBlue << " " << endl;
      anOut << "          shininess " << myShininess << " " << endl;
      anOut << "        }" << endl;
      anOut << "      }" << endl;

      anOut << "      geometry IndexedFaceSet {" << endl;
      anOut << "        coord Coordinate {" << endl;
      anOut << "          point [" << endl;

      // Puts the coordinates of all the vertices using the order
      // given during the discretisation
      TopExp_Explorer aFaceIt(theShape, TopAbs_FACE);
      for (; aFaceIt.More(); aFaceIt.Next())
      {
        Handle(BRepMesh_FaceAttribute) anAttribute;
        const TopoDS_Face& aFace = TopoDS::Face(aFaceIt.Current());
        if (!aDiscret->GetFaceAttribute(aFace, anAttribute) || !anAttribute->IsValid())
          continue;

        Handle(BRepMesh_DataStructureOfDelaun)& aStructure =
          anAttribute->ChangeStructure();

        const Standard_Integer aNbVertices = aStructure->NbNodes();
        for (Standard_Integer i = 1; i <= aNbVertices; ++i)
        {
          const BRepMesh_Vertex& aVertex = aStructure->GetNode(i);
          const gp_Pnt&          aPoint  = anAttribute->GetPoint(aVertex);

          anOut << "          "
            << aPoint.Coord().X() << " "
            << aPoint.Coord().Y() << " "
            << aPoint.Coord().Z() << "," << endl;
        }
      }

      anOut << "          ]" << endl;
      anOut << "        }" << endl;
      anOut << "        coordIndex [" << endl;

      // Retrieves all the triangles in order to draw the facets
      for (aFaceIt.Init(theShape, TopAbs_FACE); aFaceIt.More(); aFaceIt.Next())
      {
        Handle(BRepMesh_FaceAttribute) anAttribute;
        const TopoDS_Face& aFace = TopoDS::Face(aFaceIt.Current());

        if (!aDiscret->GetFaceAttribute(aFace, anAttribute) || !anAttribute->IsValid())
          continue;

        Handle(BRepMesh_DataStructureOfDelaun)& aStructure =
          anAttribute->ChangeStructure();

        const Standard_Integer aNbTriangles = aStructure->NbElements();
        for ( Standard_Integer i = 1; i <= aNbTriangles; ++i )
        {
          const BRepMesh_Triangle& aTriangle = aStructure->GetElement(i);

          Standard_Integer v[3];
          aStructure->ElementNodes(aTriangle, v);

          anOut << "          "
                << v[0] - 1 << ", "
                << v[1] - 1 << ", "
                << v[2] - 1 << ", -1," << endl;
        }
      }

      anOut << "        ]" << endl;
      anOut << "        solid FALSE" << endl; // it is not a closed solid
      anOut << "        creaseAngle " << myCreaseAngle << " " << endl; // for smooth shading
      anOut << "      }" << endl;
      anOut << "    }" << endl;
      anOut << "  ]" << endl;
      anOut << "}" << endl;
  }
  else
  {
    // Failure when opening file
    return Standard_False;
  }

  aFile.close();
  return Standard_True;
}

