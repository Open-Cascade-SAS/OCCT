// File:	RWStl.cxx
// Created:	Thu Oct 13 13:41:29 1994
// Author:	Marc LEGAY
//		<mle@bourdon>

// Copyright:    Matra Datavision	

#include <RWStl.ixx>	
#include <OSD_Protection.hxx>
#include <OSD_File.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_NoMoreObject.hxx>
#include <Standard_TypeMismatch.hxx>
#include <Precision.hxx>
#include <StlMesh_MeshExplorer.hxx>
#include <OSD.hxx>
#include <OSD_Host.hxx>
#include <gp_XYZ.hxx>
#include <gp.hxx>
#include <stdio.h>
#include <gp_Vec.hxx>


// constants
static const int HEADER_SIZE           =  84; 
static const int SIZEOF_STL_FACET      =  50; 
static const int STL_MIN_FILE_SIZE     = 284; 
static const int ASCII_LINES_PER_FACET =   7; 

//=============== =======================================================
//function : WriteInteger
//purpose  : writing a Little Endian 32 bits integer
//=======================================================================

inline static void WriteInteger(OSD_File& ofile,const Standard_Integer value)
{
  union {
    Standard_Integer i;// don't be afraid, this is just an unsigned int
    char c[4];
  } bidargum;

  bidargum.i = value;

  Standard_Integer entier;
  
  entier  =  bidargum.c[0] & 0xFF;
  entier |= (bidargum.c[1] & 0xFF) << 0x08;
  entier |= (bidargum.c[2] & 0xFF) << 0x10;
  entier |= (bidargum.c[3] & 0xFF) << 0x18;

  ofile.Write((char *)&entier,sizeof(bidargum.c));
}

//=======================================================================
//function : WriteDouble2Float
//purpose  : writing a Little Endian 32 bits float
//=======================================================================

inline static void WriteDouble2Float(OSD_File& ofile,Standard_Real value)
{
  union {
    Standard_ShortReal f;
    char c[4];
  } bidargum;

  bidargum.f = (Standard_ShortReal)value;

  Standard_Integer entier;
  
  entier  =  bidargum.c[0] & 0xFF;
  entier |= (bidargum.c[1] & 0xFF) << 0x08;
  entier |= (bidargum.c[2] & 0xFF) << 0x10;
  entier |= (bidargum.c[3] & 0xFF) << 0x18;

  ofile.Write((char *)&entier,sizeof(bidargum.c));
}


//=======================================================================
//function : readFloat2Double
//purpose  : reading a Little Endian 32 bits float
//=======================================================================

inline static Standard_Real ReadFloat2Double(OSD_File &aFile)
{
  union {
    Standard_Boolean i; // don't be afraid, this is just an unsigned int
    Standard_ShortReal f;
  }bidargum;

  char c[4];
  Standard_Address adr;
  adr = (Standard_Address)c;
  Standard_Integer lread;
  aFile.Read(adr,4,lread);
  bidargum.i  =  c[0] & 0xFF;
  bidargum.i |=  (c[1] & 0xFF) << 0x08;
  bidargum.i |=  (c[2] & 0xFF) << 0x10;
  bidargum.i |=  (c[3] & 0xFF) << 0x18;

  return (Standard_Real)(bidargum.f);
}



//=======================================================================
//function : WriteBinary
//purpose  : write a binary STL file in Little Endian format
//=======================================================================

Standard_Boolean RWStl::WriteBinary(const Handle(StlMesh_Mesh)& aMesh, const OSD_Path& aPath)
{
  
  OSD_File theFile = OSD_File (aPath);
  theFile.Build(OSD_WriteOnly,OSD_Protection());

  Standard_Real x1, y1, z1;
  Standard_Real x2, y2, z2;
  Standard_Real x3, y3, z3;
  
  //pgo  Standard_Real x,y,z;
  
  char sval[80];
  Standard_Integer NBTRIANGLES=0;
  unsigned int NBT;
  NBTRIANGLES = aMesh->NbTriangles();
  NBT = NBTRIANGLES ;
  theFile.Write ((Standard_Address)sval,80);
  WriteInteger(theFile,NBT);  
//  theFile.Write ((Standard_Address)&NBT,4);
  int dum=0;
  StlMesh_MeshExplorer aMexp (aMesh);
  
  
  for (Standard_Integer nbd=1;nbd<=aMesh->NbDomains();nbd++) {
    for (aMexp.InitTriangle (nbd); aMexp.MoreTriangle (); aMexp.NextTriangle ()) {
	aMexp.TriangleVertices (x1,y1,z1,x2,y2,z2,x3,y3,z3);
	//pgo	  aMexp.TriangleOrientation (x,y,z);
	gp_XYZ Vect12 ((x2-x1), (y2-y1), (z2-z1));
	gp_XYZ Vect13 ((x3-x1), (y3-y1), (z3-z1));
	gp_XYZ Vnorm = Vect12 ^ Vect13;
	Standard_Real Vmodul = Vnorm.Modulus ();
	if (Vmodul > gp::Resolution()) {
	  Vnorm.Divide(Vmodul);
	}
	else {
	  // si Vnorm est quasi-nul, on le charge a 0 explicitement
	  Vnorm.SetCoord (0., 0., 0.);
	}
	
	WriteDouble2Float (theFile,Vnorm.X());
	WriteDouble2Float (theFile,Vnorm.Y());
	WriteDouble2Float (theFile,Vnorm.Z());
	
	WriteDouble2Float (theFile,x1);
	WriteDouble2Float (theFile,y1);
	WriteDouble2Float (theFile,z1);

	WriteDouble2Float (theFile,x2);
	WriteDouble2Float (theFile,y2);
	WriteDouble2Float (theFile,z2);
	
	WriteDouble2Float (theFile,x3);
	WriteDouble2Float (theFile,y3);
	WriteDouble2Float (theFile,z3);

	theFile.Write (&dum,2);
	
      }
  }
  
  theFile.Close ();
  return Standard_True;
}
//=======================================================================
//function : WriteAscii
//purpose  : write an ASCII STL file
//=======================================================================

Standard_Boolean RWStl::WriteAscii(const Handle(StlMesh_Mesh)& aMesh, const OSD_Path& aPath)
{
  OSD_File theFile = OSD_File (aPath);
  theFile.Build(OSD_WriteOnly,OSD_Protection());
  TCollection_AsciiString buf = TCollection_AsciiString ("solid\n");
  theFile.Write (buf,buf.Length());buf.Clear();
  
  Standard_Real x1, y1, z1;
  Standard_Real x2, y2, z2;
  Standard_Real x3, y3, z3;
  
  //pgo  Standard_Real x,y,z;
  
  char sval[16];
  
  StlMesh_MeshExplorer aMexp (aMesh);
  
  for (Standard_Integer nbd=1;nbd<=aMesh->NbDomains();nbd++) {
    for (aMexp.InitTriangle (nbd); aMexp.MoreTriangle (); aMexp.NextTriangle ()) {
      aMexp.TriangleVertices (x1,y1,z1,x2,y2,z2,x3,y3,z3);
      
//      Standard_Real x, y, z;
//      aMexp.TriangleOrientation (x,y,z);
      
      gp_XYZ Vect12 ((x2-x1), (y2-y1), (z2-z1));
      gp_XYZ Vect23 ((x3-x2), (y3-y2), (z3-z2));
	gp_XYZ Vnorm = Vect12 ^ Vect23;
      Standard_Real Vmodul = Vnorm.Modulus ();
      if (Vmodul > gp::Resolution()){
	Vnorm.Divide (Vmodul);
      }
      else {
	// si Vnorm est quasi-nul, on le charge a 0 explicitement
	Vnorm.SetCoord (0., 0., 0.);
      }
      buf += " facet normal "; 
      sprintf (sval,"% 12e",Vnorm.X());
      buf += sval;
      buf += " "; 
      sprintf (sval,"% 12e",Vnorm.Y());
      buf += sval;
      buf += " "; 
      sprintf (sval,"% 12e",Vnorm.Z());
      buf += sval;
      buf += '\n';
      theFile.Write (buf,buf.Length());buf.Clear();
      buf += "   outer loop\n"; 
      theFile.Write (buf,buf.Length());buf.Clear();
      
      buf += "     vertex "; 
      sprintf (sval,"% 12e",x1);
      buf += sval;
      buf += " "; 
      sprintf (sval,"% 12e",y1);
      buf += sval;
      buf += " "; 
      sprintf (sval,"% 12e",z1);
      buf += sval;
      buf += '\n';
      theFile.Write (buf,buf.Length());buf.Clear();
      
      buf += "     vertex "; 
      sprintf (sval,"% 12e",x2);
      buf += sval;
      buf += " "; 
      sprintf (sval,"% 12e",y2);
      buf += sval;
      buf += " "; 
      sprintf (sval,"% 12e",z2);
      buf += sval;
      buf += '\n';
      theFile.Write (buf,buf.Length());buf.Clear();
      
      buf += "     vertex "; 
      sprintf (sval,"% 12e",x3);
      buf += sval;
      buf += " "; 
      sprintf (sval,"% 12e",y3);
      buf += sval;
      buf += " "; 
      sprintf (sval,"% 12e",z3);
      buf += sval;
      buf += '\n';
      theFile.Write (buf,buf.Length());buf.Clear();
      buf += "   endloop\n"; 
      theFile.Write (buf,buf.Length());buf.Clear();
      buf += " endfacet\n"; 
      theFile.Write (buf,buf.Length());buf.Clear();
    }
  }
  
  buf += "endsolid\n";
  theFile.Write (buf,buf.Length());buf.Clear();
  
  theFile.Close ();
  return Standard_True;
}
//=======================================================================
//function : ReadFile
//Design   : 
//Warning  : 
//=======================================================================

Handle_StlMesh_Mesh RWStl::ReadFile(const OSD_Path& aPath)
{
  OSD_File file = OSD_File (aPath);
  file.Open(OSD_ReadOnly,OSD_Protection(OSD_RWD,OSD_RWD,OSD_RWD,OSD_RWD));
  Standard_Boolean IsAscii;
  unsigned char str[128];
  Standard_Integer lread,i;
  Standard_Address ach;
  ach = (Standard_Address)str;
  
  // we skip the header which is in Ascii for both modes
  file.Read(ach,HEADER_SIZE,lread);

  // we read 128 characters to detect if we have a non-ascii char
  file.Read(ach,sizeof(str),lread);

  IsAscii = Standard_True;
  for (i = 0; i< lread && IsAscii; ++i) {
    if (str[i] > '~') {
      IsAscii = Standard_False;
    }
  }

  printf("%s\n",(IsAscii?"ascii":"binary"));
  
  file.Close();
  
  if (IsAscii) {
    return RWStl::ReadAscii (aPath);
  } else {
    return RWStl::ReadBinary (aPath);
  }
}

//=======================================================================
//function : ReadBinary
//Design   : 
//Warning  : 
//=======================================================================

Handle_StlMesh_Mesh RWStl::ReadBinary(const OSD_Path& aPath)
{
  Standard_Integer NBFACET;
  Standard_Integer ifacet;
  Standard_Real fx,fy,fz,fx1,fy1,fz1,fx2,fy2,fz2,fx3,fy3,fz3;
  Standard_Integer i1,i2,i3,lread;
  char buftest[5];
  Standard_Address adr;
  adr = (Standard_Address)buftest;

  // Open the file 
  OSD_File theFile = OSD_File(aPath);
  theFile.Open(OSD_ReadOnly,OSD_Protection(OSD_RWD,OSD_RWD,OSD_RWD,OSD_RWD));

  // the size of the file (minus the header size)
  // must be a multiple of SIZEOF_STL_FACET

  // compute file size
  Standard_Integer filesize = theFile.Size();

  if ( (filesize - HEADER_SIZE) % SIZEOF_STL_FACET !=0 
	|| (filesize < STL_MIN_FILE_SIZE)) {
    Standard_NoMoreObject::Raise("RWStl::ReadBinary (wrong file size)");
  }

  // don't trust the number of triangles which is coded in the file
  // sometimes it is wrong, and with this technique we don't need to swap endians for integer
  NBFACET = ((filesize - HEADER_SIZE) / SIZEOF_STL_FACET);

  // skip the header
  theFile.Seek(HEADER_SIZE,OSD_FromBeginning);

  // create the StlMesh_Mesh object
  Handle(StlMesh_Mesh) ReadMesh = new StlMesh_Mesh ();
  ReadMesh->AddDomain ();

  for (ifacet=1; ifacet<=NBFACET; ++ifacet) {
    // read normal coordinates
    fx = ReadFloat2Double(theFile);
    fy = ReadFloat2Double(theFile);
    fz = ReadFloat2Double(theFile);

    // read vertex 1
    fx1 = ReadFloat2Double(theFile);
    fy1 = ReadFloat2Double(theFile);
    fz1 = ReadFloat2Double(theFile);

    // read vertex 2
    fx2 = ReadFloat2Double(theFile);
    fy2 = ReadFloat2Double(theFile);
    fz2 = ReadFloat2Double(theFile);

    // read vertex 3
    fx3 = ReadFloat2Double(theFile);
    fy3 = ReadFloat2Double(theFile);
    fz3 = ReadFloat2Double(theFile);

    i1 = ReadMesh->AddOnlyNewVertex (fx1,fy1,fz1);
    i2 = ReadMesh->AddOnlyNewVertex (fx2,fy2,fz2);
    i3 = ReadMesh->AddOnlyNewVertex (fx3,fy3,fz3);
    ReadMesh->AddTriangle (i1,i2,i3,fx,fy,fz);

    // skip extra bytes
    theFile.Read(adr,2,lread);
  }

  theFile.Close ();
  return ReadMesh;
  
}
//=======================================================================
//function : ReadAscii
//Design   : 
//Warning  : 
//=======================================================================

Handle_StlMesh_Mesh RWStl::ReadAscii(const OSD_Path& aPath)
{
  TCollection_AsciiString filename;
  long ipos;
  Standard_Integer nbLines = 0;
  Standard_Integer nbTris = 0;
  Standard_Integer iTri;
  Standard_ShortReal x[4],y[4],z[4];
  Standard_Integer i1,i2,i3;
  Handle(StlMesh_Mesh) ReadMesh;

  aPath.SystemName( filename);

  // Open the file 
  FILE* file = fopen(filename.ToCString(),"r");

  fseek(file,0L,SEEK_END);

  long filesize = ftell(file);

  fclose(file);
  file = fopen(filename.ToCString(),"r");



  // count the number of lines
  for (ipos = 0; ipos < filesize; ++ipos) {
	  if (getc(file) == '\n')
        nbLines++;
  }

  // compute number of triangles
  nbTris = (nbLines / ASCII_LINES_PER_FACET);

  // go back to the beginning of the file
//  fclose(file);
//  file = fopen(filename.ToCString(),"r");
  rewind(file);

  // skip header
  while (getc(file) != '\n');

  cout<< "start mesh\n";
  ReadMesh = new StlMesh_Mesh(); 
  ReadMesh->AddDomain();

  // main reading
  for (iTri = 0; iTri < nbTris; ++iTri) {
    // reading the facet normal
    fscanf(file,"%*s %*s %f %f %f\n",&x[0],&y[0],&z[0]);

    // skip the keywords "outer loop"
    fscanf(file,"%*s %*s");

    // reading vertex
    fscanf(file,"%*s %f %f %f\n",&x[1],&y[1],&z[1]);
    fscanf(file,"%*s %f %f %f\n",&x[2],&y[2],&z[2]);
    fscanf(file,"%*s %f %f %f\n",&x[3],&y[3],&z[3]);

    // here the facet must be built and put in the mesh datastructure		  

    i1 = ReadMesh->AddOnlyNewVertex ((Standard_Real)x[1],(Standard_Real)y[1],(Standard_Real)z[1]);
    i2 = ReadMesh->AddOnlyNewVertex ((Standard_Real)x[2],(Standard_Real)y[2],(Standard_Real)z[2]);
    i3 = ReadMesh->AddOnlyNewVertex ((Standard_Real)x[3],(Standard_Real)y[3],(Standard_Real)z[3]);
    ReadMesh->AddTriangle (i1,i2,i3,(Standard_Real)x[0],(Standard_Real)y[0],(Standard_Real)z[0]);

    // skip the keywords "endloop"
    fscanf(file,"%*s");

    // skip the keywords "endfacet"
    fscanf(file,"%*s");

  }

  cout<< "end mesh\n"<<endl;
  fclose(file);
  return ReadMesh;
  
}
