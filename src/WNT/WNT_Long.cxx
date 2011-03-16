#ifndef __WNT_Long_HeaderFile
# include <WNT_Long.hxx>
#endif  // __WNT_Long_HeaderFile

const Handle( Standard_Type )& STANDARD_TYPE( WNT_Long ) {

 static Handle( Standard_Type ) _aType = new Standard_Type (
                                              "WNT_Long", sizeof ( WNT_Long )
                                             );

 return _aType;

}  // end function
