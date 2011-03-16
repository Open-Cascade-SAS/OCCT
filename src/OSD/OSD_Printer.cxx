
#ifndef WNT

#include <Standard_ConstructionError.hxx>
#include <OSD_Printer.ixx>
#include <OSD_WhoAmI.hxx>

//const OSD_WhoAmI Iam = OSD_WPrinter;

#include <errno.h>

// -------------------------------------------------------------
// -------------------------------------------------------------
OSD_Printer::OSD_Printer (const TCollection_AsciiString& Name){
 SetName(Name);
}


// -------------------------------------------------------------
// -------------------------------------------------------------
void OSD_Printer::SetName (const TCollection_AsciiString& Name){

 if (!Name.IsAscii())
  Standard_ConstructionError::Raise("OSD_Printer::SetName");

 myName = Name;
}


// -------------------------------------------------------------
// -------------------------------------------------------------
void OSD_Printer::Name (TCollection_AsciiString &Name)const{
 Name = myName;
}


// -------------------------------------------------------------
// -------------------------------------------------------------
void OSD_Printer::Reset(){
 myError.Reset();
}

// -------------------------------------------------------------
// -------------------------------------------------------------
Standard_Boolean OSD_Printer::Failed()const{
 return( myError.Failed());
}

// -------------------------------------------------------------
// -------------------------------------------------------------
void OSD_Printer::Perror() {
 myError.Perror();
}


// -------------------------------------------------------------
// -------------------------------------------------------------
Standard_Integer OSD_Printer::Error()const{
 return( myError.Error());
}
#else

//------------------------------------------------------------------------
//-------------------  Windows NT sources for OSD_Printer ----------------
//------------------------------------------------------------------------

#include <OSD_Printer.ixx>

OSD_Printer :: OSD_Printer ( const TCollection_AsciiString& Name ) {

 SetName ( Name );

}  // end constructor

void OSD_Printer :: SetName ( const TCollection_AsciiString& Name ) {

 myName = Name;

}  // end OSD_Printer :: SetName

void OSD_Printer :: Name ( TCollection_AsciiString& Name ) const {

 Name = myName;

}  // end OSD_Printer :: Name

Standard_Boolean OSD_Printer :: Failed () const {

 return myError.Failed ();

}  // end OSD_Printer :: Failed

void OSD_Printer :: Reset () {

 myError.Reset ();

}  // end OSD_Printer :: Reset

void OSD_Printer :: Perror () {

 myError.Perror ();

}  // end OSD_Printer :: Perror

Standard_Integer OSD_Printer :: Error () const {

 return myError.Error ();

}  // end OSD_Printer :: Error


#endif
