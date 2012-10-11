// Created on: 2008-01-20
// Created by: Alexander A. BORODIN
// Copyright (c) 2008-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <Font_FontMgr.ixx>
#ifdef WNT
# include <windows.h>
# include <stdlib.h>
#else  //WNT
# include <dirent.h>
# include <X11/Xlib.h>
#endif //WNT

#include <OSD_Environment.hxx>
#include <NCollection_List.hxx>
#include <TCollection_HAsciiString.hxx>  
#include <Standard_Stream.hxx>


#ifndef WNT
#include <TCollection_AsciiString.hxx>

#include <NCollection_DefineList.hxx>
#include <NCollection_List.hxx>

#include <OSD_Path.hxx>
#include <OSD_FileIterator.hxx>
#include <OSD_DirectoryIterator.hxx>
#include <OSD_File.hxx>
#include <OSD_FileNode.hxx>
#include <OSD_OpenMode.hxx>
#include <OSD_Protection.hxx>
#include <Font_NListOfSystemFont.hxx>

const  Standard_Integer font_service_conf_size = 3;
static Standard_Character font_service_conf[font_service_conf_size][64] = { {"/etc/X11/fs/config"},
                                                                            {"/usr/X11R6/lib/X11/fs/config"},
                                                                            {"/usr/X11/lib/X11/fs/config"}
                                                                           };

DEFINE_LIST( StringList, NCollection_List, TCollection_HAsciiString );

void find_path_with_font_dir( const TCollection_AsciiString& dir,StringList& dirs )
{  
  if( !dir.IsEmpty() )
  {
    TCollection_AsciiString PathName( dir );

    Standard_Integer rem = PathName.Length();

    if ( PathName.SearchFromEnd("/") == rem )
      PathName.Remove( rem, 1 );

    Standard_Boolean need_to_append = Standard_True;
       
    StringList::Iterator it( dirs );
    for( ; it.More(); it.Next() )
    {
      if ( PathName.IsEqual(it.Value().ToCString()) ) {
        need_to_append = Standard_False;
        break;
      }
    }
    if ( need_to_append )
      dirs.Append( PathName );

    OSD_DirectoryIterator osd_dir(PathName,"*");
    while(osd_dir.More())
    {
      OSD_Path path_file;
      osd_dir.Values().Path( path_file );
      if( path_file.Name().Length() < 1 )
      {
        osd_dir.Next();
        continue;
      }
      
      TCollection_AsciiString full_path_name = PathName + "/" + path_file.Name();
      rem = full_path_name.Length();
      if ( full_path_name.SearchFromEnd("/") == rem )
        full_path_name.Remove( rem, 1 );
      find_path_with_font_dir( full_path_name, dirs );
      osd_dir.Next();
    }
  }
}

#endif //WNT


Handle(Font_FontMgr) Font_FontMgr::GetInstance() {

  static Handle(Font_FontMgr) _mgr;
  if ( _mgr.IsNull() )
    _mgr = new Font_FontMgr();

  return _mgr;

}

Font_FontMgr::Font_FontMgr() {

  InitFontDataBase();

}

void Font_FontMgr::InitFontDataBase() {

  MyListOfFonts.Clear();

#ifdef WNT
  //detect font directory

  OSD_Environment env("windir");
  TCollection_AsciiString windir_str = env.Value();
  if ( windir_str.IsEmpty() )
  {
	  return;
  }
  Handle(TCollection_HAsciiString) HFontDir = new TCollection_HAsciiString( windir_str );
  HFontDir->AssignCat( "\\Fonts\\" );
  #ifdef TRACE
    cout << "System font directory: " << HFontDir->ToCString() << "\n";
  #endif TRACE

  //read registry
  HKEY fonts_hkey;
  if( RegOpenKeyEx( HKEY_LOCAL_MACHINE,
      TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts"),
      0,
      KEY_READ,
      &fonts_hkey )
      != ERROR_SUCCESS ) 
  {
    return;
  }           
  Standard_Integer           id = 0;
  Standard_Character         buf_name[100];
  Standard_Byte buf_data[100];
  DWORD size_name = 100,
        size_data = 100;

  while ( true )
  {
    //detect file name    
    DWORD type;
    size_name = 100,
      size_data = 100;
    Font_FontAspect aspect;
    if( RegEnumValue( fonts_hkey,
                      id,
                      buf_name,
                      &size_name,
                      NULL,
                      &type,
                      buf_data,
                      &size_data) == ERROR_NO_MORE_ITEMS ) {
        break;
      }
      Handle(TCollection_HAsciiString) fname = 
        new TCollection_HAsciiString(buf_name);
      fname->RightAdjust();
      fname->LeftAdjust();
      //remove construction like (TrueType....
      Standard_Integer anIndexTT = fname->SearchFromEnd( new TCollection_HAsciiString( " (" ) );
      Standard_Boolean aTruncate = Standard_False;
      if ( anIndexTT > 1 )
        fname->Trunc( anIndexTT );
      Standard_Integer anIndex = 0;
      fname->RightAdjust();
      if ( ( anIndex = fname->SearchFromEnd( new TCollection_HAsciiString("Bold Italic") ) ) > 0 ) {
        aTruncate = ( anIndex > 1 ) && ( fname->Value(anIndex - 1 ) == ' ' );
        aspect = Font_FA_BoldItalic;
      } else if ( ( anIndex = fname->SearchFromEnd( new TCollection_HAsciiString("Bold") ) ) > 0 ) {
        aTruncate = ( anIndex > 1 ) && ( fname->Value(anIndex - 1 ) == ' ' );
        aspect = Font_FA_Bold;
      } else if ( ( anIndex = fname->SearchFromEnd( new TCollection_HAsciiString("Italic") ) ) > 0 ) {
        aTruncate = ( anIndex > 1 ) && ( fname->Value(anIndex - 1 ) == ' ' );
        aspect = Font_FA_Italic;
      } else {
        aspect = Font_FA_Regular;
      }
      if( aTruncate )
        fname->Trunc( anIndex - 1 );
      fname->RightAdjust();
      Handle(TCollection_HAsciiString) file_path =
      new TCollection_HAsciiString( (Standard_Character*)buf_data );
      if ( strchr( (Standard_Character*)buf_data, '\\' ) == NULL ) {
        file_path->Insert( 1, HFontDir );
      }

      if( ( ( file_path->Search(".ttf") > 0 ) || ( file_path->Search(".TTF") > 0 ) ||
            ( file_path->Search(".otf") > 0 ) || ( file_path->Search(".OTF") > 0 ) ||
            ( file_path->Search(".ttc") > 0 ) || ( file_path->Search(".TTC") > 0 ) ) ){
        MyListOfFonts.Append( new Font_SystemFont( fname, aspect, file_path ) );
#ifdef TRACE
        cout  << "Adding font...\n"
              << "  font name: " << fname->ToCString() << "\n"
              << "  font file: " << file_path->ToCString() << "\n"
              << "  font aspect: ";
        switch( aspect ) {
        case Font_FA_Bold:
          cout << "Font_FA_Bold\n";
          break;
        case Font_FA_BoldItalic:
          cout << "Font_FA_BoldItalic\n";
          break;
        case Font_FA_Italic:
          cout << "Font_FA_Italic\n";
          break;
        default:
          cout << "Font_FA_Regular\n";
          break;
        }
#endif
      }
  id++; 
  }
  //close registry
  RegCloseKey( fonts_hkey );
#endif //WNT

#ifndef WNT
  StringList dirs;
  Handle(TCollection_HAsciiString) str = new TCollection_HAsciiString;
  Display * disp = XOpenDisplay("localhost:0.0");

  if (!disp) 
  {
    // let the X server find the available connection
    disp = XOpenDisplay(":0.0");
    if (!disp)
    {
      cout << "Display is NULL!" << endl;
      return ;
    }
  }

  Standard_Integer npaths = 0;

  Standard_Character** fontpath = XGetFontPath(disp, &npaths);
#ifdef TRACE
  cout << "NPATHS = " << npaths << endl ;
#endif
  for (Standard_Integer i = 0; i < npaths; i++  ) 
  {  
#ifdef TRACE         
    cout << "Font Path: " << fontpath[i] << endl;
#endif
    if ( fontpath[i][0] == '/' ) {
      TCollection_AsciiString aFontPath( fontpath[i] );
      find_path_with_font_dir( aFontPath, dirs );
    }
    else
    {
      TCollection_AsciiString aFontPath( fontpath[i] );
      TCollection_AsciiString aCutFontPath;        
      Standard_Integer location = -1 ;
       location = aFontPath.Location( "/",1,aFontPath.Length() );
      if( location > 0 )
        aCutFontPath.AssignCat( aFontPath.SubString(location, aFontPath.Length() ) );
      find_path_with_font_dir( aCutFontPath, dirs ); 
    }
  }
  XFreeFontPath(fontpath);


  OSD_OpenMode aMode =  OSD_ReadOnly;
  OSD_Protection  aProtect( OSD_R, OSD_R, OSD_R, OSD_R );

  for( Standard_Integer j = 0 ; j < font_service_conf_size; j++ )
  {
    TCollection_AsciiString fileOfFontServiceName( font_service_conf[j] );
    OSD_File aFile( fileOfFontServiceName );

    if( aFile.Exists() ) 
      aFile.Open( aMode, aProtect );
  
    if( aFile.IsOpen() )//font service
    {
      Standard_Integer aNByte = 256;
      Standard_Integer aNbyteRead;
      TCollection_AsciiString aStr( aNByte );//read string with information
      TCollection_AsciiString aStrCut( aNByte );//cut of string
      TCollection_AsciiString endStr;//cutting string

      Standard_Boolean read_dirs = Standard_False;
      Standard_Integer location =- 1; //disposition of necessary literals
      Standard_Integer begin =- 1; //first left entry in string
      Standard_Integer end =- 1; //first right entry in string
      while( !aFile.IsAtEnd() )
      {
        aFile.ReadLine( aStr, aNByte, aNbyteRead );//reading 1 lines(256 bytes) 
        location = aStr.Location( "catalogue = ", 1, aStr.Length() );
        if(location == 0)
          location = aStr.Location( "catalogue=", 1, aStr.Length() );
        if(location == 0)
          location = aStr.Location( "catalogue= ", 1, aStr.Length() );
        if(location == 0)
          location = aStr.Location( "catalogue = ", 1, aStr.Length() );
        if( location > 0 )  
        {
#ifdef TRACE
          cout << " Font config find!!" << endl;
#endif
          read_dirs = Standard_True;
        }
  
        if( read_dirs )
        {  
          begin = aStr.Location( "/", 1, aStr.Length() );//begin of path name
          end = aStr.Location( ":", 1, aStr.Length() );//end of path name
          if( end < 1 )
            end = aStr.Location( ",", 1, aStr.Length() );//also end of path name
          end -= 1;
          if( begin > 0 && end > 0 )
          {
            if( ( end - begin ) > 0 )
              endStr.AssignCat( aStr.SubString ( begin, end ) );//cutting necessary literals for string
            dirs.Append( TCollection_HAsciiString ( endStr ) );
            endStr.Clear();
          }
          else 
            if( begin > 0 && end == -1 )
            { 
              //if end of string don't have "," or ":"
              //it is possible last sentence in block of word
              endStr.AssignCat( aStr.SubString( begin, aStr.Length() - 1 ) );
              dirs.Append( TCollection_HAsciiString( endStr ) );   
              endStr.Clear();
            }
        } 
 
      }
      aFile.Close();
    }
  }

  if( dirs.Size() > 0 )
  {   
    //if dirs list contains elements
    OSD_OpenMode aModeRead =  OSD_ReadOnly;
    OSD_Protection  aProtectRead( OSD_R, OSD_R , OSD_R , OSD_R );

    TCollection_AsciiString fileFontsDir;
    StringList::Iterator it( dirs ); 
    for( ; it.More(); it.Next() ) 
    {     
      fileFontsDir.AssignCat( it.Value().ToCString() );   
      fileFontsDir.AssignCat( "/fonts.dir" );//append file name in path way

      OSD_File readFile( fileFontsDir );  
      readFile.Open( aModeRead, aProtectRead );

      Standard_Integer aNbyteRead, aNByte = 256;
      if( readFile.IsOpen ( ) )
      {
        TCollection_AsciiString aLine( aNByte );
        Standard_Integer countOfString = 0 ;
        while( ! readFile.IsAtEnd() )//return true if EOF
        {
          if( countOfString > 1 )
          {
            readFile.ReadLine( aLine , aNByte , aNbyteRead );
            if( ( ( aLine.Search(".pfa") > 0 ) || ( aLine.Search(".PFA") > 0 ) ||
                  ( aLine.Search(".pfb") > 0 ) || ( aLine.Search(".PFB") > 0 ) ||
                  ( aLine.Search(".ttf") > 0 ) || ( aLine.Search(".TTF") > 0 ) ||
                  ( aLine.Search(".otf") > 0 ) || ( aLine.Search(".OTF") > 0 ) ||
                  ( aLine.Search(".ttc") > 0 ) || ( aLine.Search(".TTC") > 0 ) )
                  && ( aLine.Search( "iso8859-1\n" ) > 0 ) )    
            { 

              // In current implementation use fonts with ISO-8859-1 coding page.
              // OCCT not give to manage coding page by means of programm interface. 
              // TODO: make high level interface for 
              // choosing necessary coding page.  
              TCollection_AsciiString aXLFD;
              Standard_Integer leftXLFD = aLine.SearchFromEnd(" ");
              Standard_Integer rightXLFD = aLine.Length();
              if( leftXLFD && rightXLFD )
                aXLFD.AssignCat(aLine.SubString( leftXLFD + 1, rightXLFD ) );

              TCollection_AsciiString aPath;
              TCollection_AsciiString aTemp( it.Value().ToCString() );
              if ( aTemp.SearchFromEnd("/") == aTemp.Length() )
              {
                //this branch intend to SUN
                aPath.AssignCat( aTemp.ToCString() );
                aPath.AssignCat( aLine.Token( " ", 1 ) );
              }
              else {
                //this branch intend to Linux       
                aPath.AssignCat( aTemp.ToCString( ) );
                aPath.AssignCat( "/" );
                aPath.AssignCat( aLine.Token( " ", 1 ) );
              }
              MyListOfFonts.Append( new Font_SystemFont( new TCollection_HAsciiString( aXLFD ),
                new TCollection_HAsciiString( aPath ) ) );
            }
            
          }
          else
            readFile.ReadLine( aLine, aNByte, aNbyteRead ); 
          countOfString++;
        }
        readFile.Close();
      } 
      fileFontsDir.Clear(); 
    }
  }
#endif
}

Font_NListOfSystemFont Font_FontMgr::GetAvalableFonts() const
{
  return MyListOfFonts;
}


