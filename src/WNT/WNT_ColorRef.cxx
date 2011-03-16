#ifndef __WNT_ColorRef_HeaderFile
# include <WNT_ColorRef.hxx>
#endif  // __WNT_ColorRef_HeaderFile

const Handle( Standard_Type )& STANDARD_TYPE( WNT_ColorRef ) {

 static Handle( Standard_Type ) _aType = new Standard_Type (
                                              "WNT_ColorRef", sizeof ( WNT_ColorRef )
                                             );

 return _aType;

}  // end function
