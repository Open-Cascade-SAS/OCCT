#ifndef __WNT_LogFont_HeaderFile
# include <WNT_LogFont.hxx>
#endif  // __WNT_LogFont_HeaderFile

const Handle( Standard_Type )& STANDARD_TYPE( WNT_LogFont ) {

 static Handle( Standard_Type ) _aType = new Standard_Type (
                                              "WNT_LogFont", sizeof ( WNT_LogFont )
                                             );

 return _aType;

}  // end function
