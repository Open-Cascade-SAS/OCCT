#ifndef WNT
# include <X11/Xlib.h>
#endif  // WNT
#include <AlienImage_X11XWDFileHeader.hxx>

const Handle(Standard_Type)& STANDARD_TYPE(AlienImage_X11XWDFileHeader)
{
  static Handle(Standard_Type) _atype = 
    new Standard_Type ("AlienImage_X11XWDFileHeader", sizeof (AlienImage_X11XWDFileHeader));
  return _atype;
}

Standard_Boolean  operator == ( 
		const AlienImage_X11XWDFileHeader& AnObject,
		const AlienImage_X11XWDFileHeader& AnotherObject)

{ Standard_Boolean _result = Standard_True; 

  _result = _result && (AnObject.header_size==
			AnotherObject.header_size) ; 
  _result = _result && (AnObject.file_version==
			AnotherObject.file_version) ; 
  _result = _result && (AnObject.pixmap_format==
			AnotherObject.pixmap_format) ; 
  _result = _result && (AnObject.pixmap_depth==
			AnotherObject.pixmap_depth) ; 
  _result = _result && (AnObject.pixmap_width==
			AnotherObject.pixmap_width) ; 
  _result = _result && (AnObject.pixmap_height==
			AnotherObject.pixmap_height) ; 
  _result = _result && (AnObject.xoffset==
			AnotherObject.xoffset) ; 
  _result = _result && (AnObject.byte_order==
			AnotherObject.byte_order) ; 
  _result = _result && (AnObject.bitmap_unit==
			AnotherObject.bitmap_unit) ; 
  _result = _result && (AnObject.bitmap_bit_order==
			AnotherObject.bitmap_bit_order) ; 
  _result = _result && (AnObject.bitmap_pad==
			AnotherObject.bitmap_pad) ; 
  _result = _result && (AnObject.bits_per_pixel==
			AnotherObject.bits_per_pixel) ; 
  _result = _result && (AnObject.bytes_per_line==
			AnotherObject.bytes_per_line) ; 
  _result = _result && (AnObject.visual_class==
			AnotherObject.visual_class) ; 
  _result = _result && (AnObject.red_mask==
			AnotherObject.red_mask) ; 
  _result = _result && (AnObject.green_mask==
			AnotherObject.green_mask) ; 
  _result = _result && (AnObject.blue_mask==
			AnotherObject.blue_mask) ; 
  _result = _result && (AnObject.bits_per_rgb==
			AnotherObject.bits_per_rgb) ; 
  _result = _result && (AnObject.colormap_entries==
			AnotherObject.colormap_entries) ; 
  _result = _result && (AnObject.ncolors==
			AnotherObject.ncolors) ; 
  _result = _result && (AnObject.window_width==
			AnotherObject.window_width) ; 
  _result = _result && (AnObject.window_height==
			AnotherObject.window_height) ; 
  _result = _result && (AnObject.window_x==
			AnotherObject.window_x) ; 
  _result = _result && (AnObject.window_y==
			AnotherObject.window_y) ; 
  _result = _result && (AnObject.window_bdrwidth==
			AnotherObject.window_bdrwidth) ; 
  
 return _result;
}

//============================================================================
//==== ShallowDump : Writes a CString value.
//============================================================================
void ShallowDump ( 
	const AlienImage_X11XWDFileHeader& AnObject,Standard_OStream& s) {
  s << "AlienImage_X11XWDFileHeader" << "\n";
  s << "\theader_size\t:" 	<< AnObject.header_size  << "\n" ;
  s << "\tfile_version\t:" 	<< AnObject.file_version  << "\n" ;
  s << "\tpixmap_format\t:" ; switch( AnObject.pixmap_format ) {
	case XYBitmap :
  	  s << "XYBitmap" ; break ;
	case XYPixmap :
  	  s << "XYPixmap"  ; break ;
	case ZPixmap  :
  	  s << "ZPixmap" ; break ;
	default	      :
  	  s << AnObject.pixmap_format  ; break ;
  } ; s << "\n"  ;
  s << "\tpixmap_depth\t:" 	<< AnObject.pixmap_depth  << "\n" ;
  s << "\tpixmap_width\t:" 	<< AnObject.pixmap_width  << "\n" ;
  s << "\tpixmap_height\t:" 	<< AnObject.pixmap_height  << "\n" ;
  s << "\txoffset\t:" 		<< AnObject.xoffset  << "\n" ;
  s << "\tbyte_order\t:" ; 
	if ( AnObject.byte_order == LSBFirst ) s << "LSBFirst" ;
	else				       s << "MSBFirst" ;
  s << "\n" ;
  s << "\tbitmap_unit\t:" 	<< AnObject.bitmap_unit  << "\n" ;
  s << "\tbitmap_bit_order\t:" ;
	if ( AnObject.bitmap_bit_order == LSBFirst ) s << "LSBFirst" ;
	else					     s << "MSBFirst" ;
  s << "\n" ;
  s << "\tbitmap_pad\t:" 	<< AnObject.bitmap_pad  << "\n" ;
  s << "\tbits_per_pixel\t:" 	<< AnObject.bits_per_pixel  << "\n" ;
  s << "\tbytes_per_line\t:" 	<< AnObject.bytes_per_line  << "\n" ;
  s << "\tvisual_class\t:" ; switch( AnObject.visual_class ) {
	case StaticGray  : s << "StaticGray" ; break ;
	case GrayScale   : s << "GrayScale" ; break ;
	case StaticColor : s << "StaticColor" ; break ;
	case PseudoColor : s << "PseudoColor" ; break ;
	case TrueColor   : s << "TrueColor" ; break ;
	case DirectColor : s << "DirectColor" ; break ;
	default          : s <<  AnObject.visual_class ; break ;
  } ; s << "\n" ;
  s << "\tred_mask\t:" 		<< AnObject.red_mask  << "\n" ;
  s << "\tgreen_mask\t:" 	<< AnObject.green_mask  << "\n" ;
  s << "\tblue_mask\t:" 	<< AnObject.blue_mask  << "\n" ;
  s << "\tbits_per_rgb\t:" 	<< AnObject.bits_per_rgb  << "\n" ;
  s << "\tcolormap_entries\t:" 	<< AnObject.colormap_entries  << "\n" ;
  s << "\tncolors\t:" 		<< AnObject.ncolors  << "\n" ;
  s << "\twindow_width\t:" 	<< AnObject.window_width  << "\n" ;
  s << "\twindow_height\t:" 	<< AnObject.window_height  << "\n" ;
  s << "\twindow_x\t:" 		<< AnObject.window_x  << "\n" ;
  s << "\twindow_y\t:" 		<< AnObject.window_y  << "\n" ;
  s << "\twindow_bdrwidth\t:" 	<< AnObject.window_bdrwidth  << "\n" << flush ;
}

ostream& operator << ( ostream& s,  const AlienImage_X11XWDFileHeader& h )

{	::ShallowDump( h, s ) ;
	return( s ) ;
}
 
