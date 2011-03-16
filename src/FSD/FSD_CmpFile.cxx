#include <FSD_CmpFile.ixx>
#include <OSD.hxx>

#include <OSD_Real2String.hxx>

#include <Storage_StreamModeError.hxx>
#include <Storage_StreamUnknownTypeError.hxx>
#include <Standard_PCharacter.hxx>

// Propagate the improvement (BUC60808) on all platforms
// #ifdef WNT
#define BUC60808
// BUC60808 : standard output in MS Visual C++ limites the quantity of figures (digits)
// in part of number after fixed point. The quantity of figures after fixed point 
// could not be more than 15 when we output double number using standard MS VS output operator '<<'.
// For example, the maximum double value DBL_MAX = 1.7976931348623157e+308 (see float.h) after 
// cout<<setprecision(17)<<DBL_MAX  becomes 1.79769313486232e+308  in stream of output 
// (it could be a file stream). When the number 1.79769313486232e+308 is read from the
// i/o streame (file, for example) it occurs more than allowed maximum value of double 
// type - 1.7976931348623157e+308.  Such value is considered as infinite. When it is written 
// to file (for example)  again it is written as 1.#INF. Such value can't be read from file next time.
//#endif

const Standard_CString MAGICNUMBER = "CMPFILE";
const Standard_CString ENDOFNORMALEXTENDEDSECTION = "BEGIN_REF_SECTION";
const Standard_Integer SIZEOFNORMALEXTENDEDSECTION = 16;

//=======================================================================
//function : FSD_CmpFile
//purpose  : 
//=======================================================================

FSD_CmpFile::FSD_CmpFile()
{

}

//=======================================================================
//function : IsGoodFileType
//purpose  : INFO SECTION
//           write
//=======================================================================

Storage_Error FSD_CmpFile::IsGoodFileType(const TCollection_AsciiString& aName)
{
  FSD_CmpFile      f;
  Storage_Error s;

  s = f.Open(aName,Storage_VSRead);

  if (s == Storage_VSOk) {
    TCollection_AsciiString l;
    Standard_Integer        len = strlen(FSD_CmpFile::MagicNumber());

    f.ReadChar(l,len);

    f.Close();

    if (strncmp(FSD_CmpFile::MagicNumber(),l.ToCString(),len) != 0) {
      s = Storage_VSFormatError;
    }
  }

  return s;
}

//=======================================================================
//function : Open
//purpose  : 
//=======================================================================

Storage_Error FSD_CmpFile::Open(const TCollection_AsciiString& aName,const Storage_OpenMode aMode)
{
  Storage_Error result = Storage_VSOk;

  SetName(aName);

  if (OpenMode() == Storage_VSNone) {

#if !defined(IRIX) && !defined(DECOSF1)
    if (aMode == Storage_VSRead) {
      myStream.open(aName.ToCString(),ios::in|ios::binary); // ios::nocreate is not portable
    }
    else if (aMode == Storage_VSWrite) {
      myStream.open(aName.ToCString(),ios::out|ios::binary);
    }
    else if (aMode == Storage_VSReadWrite) {
      myStream.open(aName.ToCString(),ios::in|ios::out|ios::binary);
    }
#else
    if (aMode == Storage_VSRead) {
      myStream.open(aName.ToCString(),ios::in); // ios::nocreate is not portable
    }
    else if (aMode == Storage_VSWrite) {
      myStream.open(aName.ToCString(),ios::out);
    }
    else if (aMode == Storage_VSReadWrite) {
      myStream.open(aName.ToCString(),ios::in|ios::out);
    }
#endif

    if (myStream.fail()) {
      result = Storage_VSOpenError;
    }
    else {
      myStream.precision(17);
      SetOpenMode(aMode);
    }
  }
  else {
    result = Storage_VSAlreadyOpen;
  }
  return result;
}

//=======================================================================
//function : IsEnd
//purpose  : 
//=======================================================================

Standard_Boolean FSD_CmpFile::IsEnd()
{
  return myStream.eof();
}

//=======================================================================
//function : Close
//purpose  : 
//=======================================================================

Storage_Error FSD_CmpFile::Close()
{
  Storage_Error result = Storage_VSOk;

  if (OpenMode() != Storage_VSNone) {
    myStream.close();
    SetOpenMode(Storage_VSNone);
  }
  else {
    result = Storage_VSNotOpen;
  }

  return result;
}

//=======================================================================
//function : MagicNumber
//purpose  : ------------------ PROTECTED
//=======================================================================

const Standard_CString FSD_CmpFile::MagicNumber()
{
  return MAGICNUMBER;
}

//=======================================================================
//function : FlushEndOfLine
//purpose  : 
//=======================================================================

void FSD_CmpFile::FlushEndOfLine()
{
  TCollection_AsciiString aDummy;
  ReadLine (aDummy); // flush is nothing more than to read till the line-break
  /*
  static char Buffer[8192];
  char c;
  Standard_Boolean IsEnd = Standard_False;

  while (!IsEnd && !FSD_CmpFile::IsEnd()) {
    Buffer[0] = '\0';
    myStream.get(Buffer,8192,'\n');

    if (myStream.get(c) && c != '\r' && c != '\n') {
    }
    else {
      IsEnd = Standard_True;
    }
  }
  */
}

//=======================================================================
//function : ReadLine
//purpose  : read from the current position to the end of line.
//=======================================================================

void FSD_CmpFile::ReadLine(TCollection_AsciiString& buffer)
{
  char Buffer[8193];
  //char c;
  Standard_Boolean IsEnd = Standard_False;
  
  buffer.Clear();

  while (!IsEnd && !FSD_CmpFile::IsEnd()) {
    Buffer[0] = '\0';
    //myStream.get(Buffer,8192,'\n');
    myStream.getline(Buffer,8192,'\n');
    for (Standard_Integer lv = (strlen(Buffer)- 1); lv > 1 && (Buffer[lv] == '\r' || Buffer[lv] == '\n') ;lv--) {
      Buffer[lv] = '\0';
    }  
    
//     if (myStream.get(c) && c != '\r' && c != '\n') {
//       buffer += Buffer;
//       buffer += c;
//     }
//     else {
      buffer += Buffer;
      IsEnd = Standard_True;
//     }
  }
}

//=======================================================================
//function : WriteExtendedLine
//purpose  : write from the current position to the end of line.
//=======================================================================

void FSD_CmpFile::WriteExtendedLine(const TCollection_ExtendedString& buffer)
{
#if 0
  Standard_ExtString extBuffer;
  Standard_Integer   i,c,d;

  extBuffer = buffer.ToExtString();

  for (i = 0; i < buffer.Length(); i++) {
    c = (extBuffer[i] & 0x0000FF00 ) >> 8 ;
    d = extBuffer[i] & 0x000000FF;

    myStream << (char)c << (char)d;
  }

  myStream << (char)0 << "\n";
#endif
  Standard_ExtString extBuffer;
  Standard_Integer   i;

  extBuffer = buffer.ToExtString();
  PutInteger(buffer.Length());
  for (i = 0; i < buffer.Length(); i++) {
    PutExtCharacter(extBuffer[i]);
  }

  myStream << "\n";
}

//=======================================================================
//function : ReadExtendedLine
//purpose  : 
//=======================================================================

void FSD_CmpFile::ReadExtendedLine(TCollection_ExtendedString& buffer)
{
  Standard_ExtCharacter c;
  Standard_Integer i;

  GetInteger(i);

  for (i = 0; i < buffer.Length(); i++) {
    GetExtCharacter(c);
    buffer += c;
  }

  FlushEndOfLine();
#if 0
  char c = '\0';
  Standard_ExtCharacter i = 0,j,count = 0;
  Standard_Boolean fin = Standard_False;
  Standard_CString tg = ENDOFNORMALEXTENDEDSECTION;
 
  buffer.Clear();

  while (!fin && !IsEnd()) {
    myStream.get(c);

    if (c == tg[count]) count++;
    else count = 0;
    if (count < SIZEOFNORMALEXTENDEDSECTION) {
      i = 0; j = 0;
      i += (Standard_ExtCharacter)c;
      if (c == '\0') fin = Standard_True;
      i = (i << 8);
      
      myStream.get(c);
      if (c == tg[count]) count++;
      else count = 0;
      if (count < SIZEOFNORMALEXTENDEDSECTION) {
	if ( c != '\r') {
	  j += (Standard_ExtCharacter)c;
	  if (c != '\n' && c != '\r') {
	    fin = Standard_False;
	    i |= (0x00FF & j);
	    buffer += (Standard_ExtCharacter)i;
	  }

	}
      }
      else {
	Storage_StreamExtCharParityError::Raise();	
      }
    }
    else {
      Storage_StreamExtCharParityError::Raise();
    }
  }
#endif

}

//=======================================================================
//function : ReadChar
//purpose  : read <rsize> character from the current position.
//=======================================================================

void FSD_CmpFile::ReadChar(TCollection_AsciiString& buffer, const Standard_Integer rsize)
{
  char             c;
  Standard_Integer ccount = 0;

  buffer.Clear();

  while (!IsEnd() && (ccount < rsize)) {
    myStream.get(c);
    buffer += c;
    ccount++;
  }
}

//=======================================================================
//function : ReadString
//purpose  : read from the first none space character position to the end of line.
//=======================================================================

void FSD_CmpFile::ReadString(TCollection_AsciiString& buffer)
{
  char Buffer[8193];
  char *bpos;
  Standard_Boolean IsEnd = Standard_False,isFirstTime = Standard_True;
  
  buffer.Clear();
  
  while (!IsEnd && !FSD_CmpFile::IsEnd()) {
    Buffer[0] = '\0';
    //myStream.get(Buffer,8192,'\n');
    myStream.getline(Buffer,8192,'\n');
    for (Standard_Integer lv = (strlen(Buffer)- 1); lv > 1 && (Buffer[lv] == '\r' || Buffer[lv] == '\n') ;lv--) {
      Buffer[lv] = '\0';
    }  
    bpos = Buffer;

    // LeftAdjust
    //
    if (isFirstTime) {
      isFirstTime = Standard_False;
      while (*bpos == '\n' || *bpos == ' ') bpos++;
    }
//     char c;
//     if (myStream.get(c) && c != '\n') {
//       buffer += bpos;
//       buffer += c;
//     }
//     else {
      buffer += bpos;
      IsEnd = Standard_True;
//     }
  }
}

//=======================================================================
//function : ReadWord
//purpose  : read from the current position to the next white space or end of line.
//=======================================================================

void FSD_CmpFile::ReadWord(TCollection_AsciiString& buffer)
{
  char c;
  char b[8193],*tmpb;
  Standard_Boolean IsEnd = Standard_False;
  Standard_Integer i;

  tmpb = b;
  memset(b,'\0',8193);
  buffer.Clear();

  while (!IsEnd && !FSD_CmpFile::IsEnd()) {
    myStream.get(c);
    if ((c != ' ') && (c != '\n')) IsEnd = Standard_True;
  }

  IsEnd = Standard_False;
  i = 0;

  while (!IsEnd && !FSD_CmpFile::IsEnd()) {
    if (i == 8192) {
      buffer += b;
      tmpb = b;
      memset(b,'\0',8193);
      i = 0;
    }
    *tmpb = c;
    tmpb++; i++;
    myStream.get(c);
    if ((c == '\n') || (c == ' ')) IsEnd = Standard_True;
  }

  buffer += b;
}

//=======================================================================
//function : FindTag
//purpose  : 
//=======================================================================

Storage_Error FSD_CmpFile::FindTag(const Standard_CString aTag)
{
  TCollection_AsciiString l;
  
  ReadString(l);

  while ((strcmp(l.ToCString(),aTag) != 0) && !IsEnd()) {
    ReadString(l);
  }

  if (IsEnd()) {
    return Storage_VSSectionNotFound;
  }
  else {
    return Storage_VSOk;
  }
}

//=======================================================================
//function : SkipObject
//purpose  : 
//=======================================================================

void FSD_CmpFile::SkipObject()
{
  FlushEndOfLine();
}

//=======================================================================
//function : PutReference
//purpose  : ---------------------- PUBLIC : PUT
//=======================================================================

Storage_BaseDriver& FSD_CmpFile::PutReference(const Standard_Integer aValue)
{
  myStream << aValue << " ";
  if (myStream.bad()) Storage_StreamWriteError::Raise();
  return *this;
}

//=======================================================================
//function : PutCharacter
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_CmpFile::PutCharacter(const Standard_Character aValue)
{
  unsigned short i;

  i = aValue;
  myStream << i << " ";
  if (myStream.bad()) Storage_StreamWriteError::Raise();
  return *this;
}

//=======================================================================
//function : PutExtCharacter
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_CmpFile::PutExtCharacter(const Standard_ExtCharacter aValue)
{
  myStream << aValue << " ";
  if (myStream.bad()) Storage_StreamWriteError::Raise();
  return *this;
}

//=======================================================================
//function : PutInteger
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_CmpFile::PutInteger(const Standard_Integer aValue)
{
  myStream << aValue << " ";
  if (myStream.bad()) Storage_StreamWriteError::Raise();
  return *this;
}

//=======================================================================
//function : PutBoolean
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_CmpFile::PutBoolean(const Standard_Boolean aValue)
{
  myStream << ((Standard_Integer)aValue) << " ";
  if (myStream.bad()) Storage_StreamWriteError::Raise();
  return *this;
}

//=======================================================================
//function : PutReal
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_CmpFile::PutReal(const Standard_Real aValue)
{
#ifdef BUC60808
  char realbuffer[100];
  Standard_PCharacter pChar;
  //
  pChar=realbuffer;
  realbuffer[0] = '\0';
  //
  if (myRealConv.RealToCString(aValue,pChar)) {
    myStream << realbuffer << " ";
  }
  else {
    Storage_StreamWriteError::Raise();
  }
  if (myStream.bad()) Storage_StreamWriteError::Raise();

  return *this;

#else
  myStream << ((Standard_Real)aValue) << " ";

  if (myStream.bad()) Storage_StreamWriteError::Raise();
  return *this;
#endif
}

//=======================================================================
//function : PutShortReal
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_CmpFile::PutShortReal(const Standard_ShortReal aValue)
{
#ifdef BUC60808
  char realbuffer[100];
  Standard_PCharacter pStr;
  //
  pStr=realbuffer;
  realbuffer[0] = '\0';
  //
  if (myRealConv.RealToCString(aValue,pStr)) {
    myStream << realbuffer << " ";
  }
  else {
    Storage_StreamWriteError::Raise();
  }
  if (myStream.bad()) Storage_StreamWriteError::Raise();

  return *this;
#else
  myStream << aValue << " ";

  if (myStream.bad()) Storage_StreamWriteError::Raise();
  return *this;
#endif
}

//=======================================================================
//function : GetReference
//purpose  : ----------------- PUBLIC : GET
//=======================================================================

Storage_BaseDriver& FSD_CmpFile::GetReference(Standard_Integer& aValue)
{
  if (!(myStream >> aValue)) Storage_StreamTypeMismatchError::Raise();

  return *this;
}

//=======================================================================
//function : GetCharacter
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_CmpFile::GetCharacter(Standard_Character& aValue)
{
  unsigned short i = 0;
  if (!(myStream >> i)) {
    // SGI : donne une erreur mais a une bonne valeur pour les caracteres ecrits
    //       signes (-80 fait ios::badbit, mais la variable i est initialisee)
    //
    if (i == 0) Storage_StreamTypeMismatchError::Raise();
    myStream.clear(ios::goodbit);
  }
  aValue = (char)i;

  return *this;
}

//=======================================================================
//function : GetExtCharacter
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_CmpFile::GetExtCharacter(Standard_ExtCharacter& aValue)
{
  if (!(myStream >> aValue)) Storage_StreamTypeMismatchError::Raise();
  
  return *this;
}

//=======================================================================
//function : GetInteger
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_CmpFile::GetInteger(Standard_Integer& aValue)
{
  if (!(myStream >> aValue)) Storage_StreamTypeMismatchError::Raise();

  return *this;
}

//=======================================================================
//function : GetBoolean
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_CmpFile::GetBoolean(Standard_Boolean& aValue)
{
  if (!(myStream >> aValue)) Storage_StreamTypeMismatchError::Raise();

  return *this;
}

//=======================================================================
//function : GetReal
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_CmpFile::GetReal(Standard_Real& aValue)
{
#ifdef BUC60808
  char realbuffer[100];

  realbuffer[0] = '\0';
  if (!(myStream >> realbuffer)) {
    cerr << "%%%ERROR: read error of double at offset " << myStream.tellg() << endl;
    cerr << "\t buffer is" << realbuffer<< endl;
    Storage_StreamTypeMismatchError::Raise();
  }
  if (!myRealConv.CStringToReal(realbuffer,aValue)) {
    cerr << "%%%ERROR: read error of double at offset " << myStream.tellg() << endl;
    cerr << "\t buffer is" << realbuffer<< endl;
    //if (!OSD::CStringToReal(realbuffer,aValue))
    Storage_StreamTypeMismatchError::Raise();
  }

  return *this;
#else
  if (!(myStream >> aValue)) Storage_StreamTypeMismatchError::Raise();

  return *this;
#endif
}

//=======================================================================
//function : GetShortReal
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_CmpFile::GetShortReal(Standard_ShortReal& aValue)
{
#ifdef BUC60808
  char realbuffer[100];
  Standard_Real r = 0.0;

  realbuffer[0] = '\0';
  if (!(myStream >> realbuffer)) Storage_StreamTypeMismatchError::Raise();
  // if (!OSD::CStringToReal(realbuffer,r)) 
  if (!myRealConv.CStringToReal(realbuffer,r))  Storage_StreamTypeMismatchError::Raise();

  aValue = (Standard_ShortReal)r;

  return *this;
#else
  if (!(myStream >> aValue)) Storage_StreamTypeMismatchError::Raise();
 return *this;
#endif
}

//=======================================================================
//function : Destroy
//purpose  : 
//=======================================================================

void FSD_CmpFile::Destroy()
{
  if (OpenMode() != Storage_VSNone) {
    Close();
  }
}

//=======================================================================
//function : BeginWriteInfoSection
//purpose  : -------------------------- INFO : WRITE
//=======================================================================

Storage_Error FSD_CmpFile::BeginWriteInfoSection() 
{
  myStream << FSD_CmpFile::MagicNumber() << '\n';
  myStream << "BEGIN_INFO_SECTION\n";
  if (myStream.bad()) Storage_StreamWriteError::Raise();

  return Storage_VSOk;
}

//=======================================================================
//function : WriteInfo
//purpose  : 
//=======================================================================

void FSD_CmpFile::WriteInfo(const Standard_Integer nbObj,
			 const TCollection_AsciiString& dbVersion,
			 const TCollection_AsciiString& date,
			 const TCollection_AsciiString& schemaName,
			 const TCollection_AsciiString& schemaVersion,
			 const TCollection_ExtendedString& appName,
			 const TCollection_AsciiString& appVersion,
			 const TCollection_ExtendedString& dataType,
			 const TColStd_SequenceOfAsciiString& userInfo) 
{
  Standard_Integer i;

  myStream << nbObj;
  myStream << "\n";
  myStream << dbVersion.ToCString() << "\n";
  myStream << date.ToCString() << "\n";
  myStream << schemaName.ToCString() << "\n";
  myStream << schemaVersion.ToCString() << "\n";
  WriteExtendedLine(appName);
  myStream << appVersion.ToCString() << "\n";
  WriteExtendedLine(dataType);
  myStream << userInfo.Length() << "\n";

  if (myStream.bad()) Storage_StreamWriteError::Raise();

  for (i = 1; i <= userInfo.Length(); i++) {
    myStream << userInfo.Value(i).ToCString() << "\n";
    if (myStream.bad()) Storage_StreamWriteError::Raise();
  }
}

//=======================================================================
//function : EndWriteInfoSection
//purpose  : read
//=======================================================================

Storage_Error FSD_CmpFile::EndWriteInfoSection() 
{
  myStream << "END_INFO_SECTION\n";
  if (myStream.bad())  Storage_StreamWriteError::Raise();
  return Storage_VSOk;
}

//=======================================================================
//function : BeginReadInfoSection
//purpose  : 
//=======================================================================

Storage_Error FSD_CmpFile::BeginReadInfoSection() 
{
  Storage_Error s;
  TCollection_AsciiString l;
  Standard_Integer        len = strlen(FSD_CmpFile::MagicNumber());

  ReadChar(l,len);
  
  if (strncmp(FSD_CmpFile::MagicNumber(),l.ToCString(),len) != 0) {
    s = Storage_VSFormatError;
  }
  else {
    s = FindTag("BEGIN_INFO_SECTION");
  }

  return s;
}

//=======================================================================
//function : ReadInfo
//purpose  : ------------------- INFO : READ
//=======================================================================

void FSD_CmpFile::ReadInfo(Standard_Integer& nbObj,
			TCollection_AsciiString& dbVersion,
			TCollection_AsciiString& date,
			TCollection_AsciiString& schemaName,
			TCollection_AsciiString& schemaVersion,
			TCollection_ExtendedString& appName,
			TCollection_AsciiString& appVersion,
			TCollection_ExtendedString& dataType,
			TColStd_SequenceOfAsciiString& userInfo) 
{
  if (!(myStream >> nbObj)) Storage_StreamTypeMismatchError::Raise();

  FlushEndOfLine();

  ReadLine(dbVersion);
  ReadLine(date);
  ReadLine(schemaName);
  ReadLine(schemaVersion);
  ReadExtendedLine(appName);
  ReadLine(appVersion);
  ReadExtendedLine(dataType);

  Standard_Integer i,len = 0;

  if (!(myStream >> len)) Storage_StreamTypeMismatchError::Raise();

  FlushEndOfLine();

  TCollection_AsciiString line;

  for (i = 1; i <= len && !IsEnd(); i++) {
    ReadLine(line);
    userInfo.Append(line);
    line.Clear();
  }
}

//=======================================================================
//function : EndReadInfoSection
//purpose  : COMMENTS SECTION
//           write
//=======================================================================

Storage_Error FSD_CmpFile::EndReadInfoSection() 
{
  return FindTag("END_INFO_SECTION");
}

//=======================================================================
//function : BeginWriteCommentSection
//purpose  : ---------------- COMMENTS : WRITE
//=======================================================================

Storage_Error FSD_CmpFile::BeginWriteCommentSection() 
{
  myStream << "BEGIN_COMMENT_SECTION\n";
  if (myStream.bad()) Storage_StreamWriteError::Raise();
  return Storage_VSOk;
}

//=======================================================================
//function : WriteComment
//purpose  : 
//=======================================================================

void FSD_CmpFile::WriteComment(const TColStd_SequenceOfExtendedString& aCom)
{
 Standard_Integer i,aSize;

 aSize = aCom.Length();
 myStream << aSize << "\n";
 if (myStream.bad()) Storage_StreamWriteError::Raise();

 for (i = 1; i <= aSize; i++) {
   WriteExtendedLine(aCom.Value(i));
   if (myStream.bad()) Storage_StreamWriteError::Raise();
 }
}

//=======================================================================
//function : EndWriteCommentSection
//purpose  : read
//=======================================================================

Storage_Error FSD_CmpFile::EndWriteCommentSection() 
{
  myStream << "END_COMMENT_SECTION\n";
  if (myStream.bad()) Storage_StreamWriteError::Raise();
  return Storage_VSOk;
}

//=======================================================================
//function : BeginReadCommentSection
//purpose  : ---------------- COMMENTS : READ
//=======================================================================

Storage_Error FSD_CmpFile::BeginReadCommentSection() 
{
  return FindTag("BEGIN_COMMENT_SECTION");
}

//=======================================================================
//function : ReadComment
//purpose  : 
//=======================================================================

void FSD_CmpFile::ReadComment(TColStd_SequenceOfExtendedString& aCom)
{
  TCollection_ExtendedString line;
  Standard_Integer           len,i;

  if (!(myStream >> len)) Storage_StreamTypeMismatchError::Raise();
  
  FlushEndOfLine();  

  for (i = 1; i <= len && !IsEnd(); i++) {
    ReadExtendedLine(line);
    aCom.Append(line);
    line.Clear();
  }
}

//=======================================================================
//function : EndReadCommentSection
//purpose  : 
//=======================================================================

Storage_Error FSD_CmpFile::EndReadCommentSection() 
{
  return FindTag("END_COMMENT_SECTION");
}

//=======================================================================
//function : BeginWriteTypeSection
//purpose  : --------------- TYPE : WRITE
//=======================================================================

Storage_Error FSD_CmpFile::BeginWriteTypeSection() 
{
  myStream << "BEGIN_TYPE_SECTION\n";
  if (myStream.bad()) Storage_StreamWriteError::Raise();
  return Storage_VSOk;
}

//=======================================================================
//function : SetTypeSectionSize
//purpose  : 
//=======================================================================

void FSD_CmpFile::SetTypeSectionSize(const Standard_Integer aSize) 
{
  myStream << aSize << "\n";
  if (myStream.bad()) Storage_StreamWriteError::Raise();
}

//=======================================================================
//function : WriteTypeInformations
//purpose  : 
//=======================================================================

void FSD_CmpFile::WriteTypeInformations(const Standard_Integer typeNum,
				      const TCollection_AsciiString& typeName) 
{
  myStream << typeNum << " " << typeName.ToCString() << "\n";
  if (myStream.bad()) Storage_StreamWriteError::Raise();
}

//=======================================================================
//function : EndWriteTypeSection
//purpose  : read
//=======================================================================

Storage_Error FSD_CmpFile::EndWriteTypeSection() 
{
  myStream << "END_TYPE_SECTION\n";
  if (myStream.bad()) Storage_StreamWriteError::Raise();
  return Storage_VSOk;
}

//=======================================================================
//function : BeginReadTypeSection
//purpose  : ------------------- TYPE : READ
//=======================================================================

Storage_Error FSD_CmpFile::BeginReadTypeSection() 
{
  return FindTag("BEGIN_TYPE_SECTION");
}

//=======================================================================
//function : TypeSectionSize
//purpose  : 
//=======================================================================

Standard_Integer FSD_CmpFile::TypeSectionSize() 
{
  Standard_Integer i;

  if (!(myStream >> i)) Storage_StreamTypeMismatchError::Raise();

  FlushEndOfLine();

  return i;
}

//=======================================================================
//function : ReadTypeInformations
//purpose  : 
//=======================================================================

void FSD_CmpFile::ReadTypeInformations(Standard_Integer& typeNum,
				    TCollection_AsciiString& typeName) 
{
  if (!(myStream >> typeNum)) Storage_StreamTypeMismatchError::Raise();
  if (!(myStream >> typeName)) Storage_StreamTypeMismatchError::Raise();
  FlushEndOfLine();
}

//=======================================================================
//function : EndReadTypeSection
//purpose  : ROOT SECTION
//           write
//=======================================================================

Storage_Error FSD_CmpFile::EndReadTypeSection() 
{
  return FindTag("END_TYPE_SECTION");
}

//=======================================================================
//function : BeginWriteRootSection
//purpose  : -------------------- ROOT : WRITE
//=======================================================================

Storage_Error FSD_CmpFile::BeginWriteRootSection() 
{
  myStream << "BEGIN_ROOT_SECTION\n";
  if (myStream.bad()) Storage_StreamWriteError::Raise();
  return Storage_VSOk;
}

//=======================================================================
//function : SetRootSectionSize
//purpose  : 
//=======================================================================

void FSD_CmpFile::SetRootSectionSize(const Standard_Integer aSize) 
{
  myStream << aSize << "\n";
  if (myStream.bad()) Storage_StreamWriteError::Raise();
}

//=======================================================================
//function : WriteRoot
//purpose  : 
//=======================================================================

void FSD_CmpFile::WriteRoot(const TCollection_AsciiString& rootName, const Standard_Integer aRef, const TCollection_AsciiString& rootType) 
{
  myStream << aRef << " " << rootName.ToCString() << " " << rootType.ToCString() << "\n";
  if (myStream.bad()) Storage_StreamWriteError::Raise();
}

//=======================================================================
//function : EndWriteRootSection
//purpose  : read
//=======================================================================

Storage_Error FSD_CmpFile::EndWriteRootSection() 
{
  myStream << "END_ROOT_SECTION\n";
  if (myStream.bad()) Storage_StreamWriteError::Raise();
  return Storage_VSOk;
}

//=======================================================================
//function : BeginReadRootSection
//purpose  : ----------------------- ROOT : READ
//=======================================================================

Storage_Error FSD_CmpFile::BeginReadRootSection() 
{
  return FindTag("BEGIN_ROOT_SECTION");
}

//=======================================================================
//function : RootSectionSize
//purpose  : 
//=======================================================================

Standard_Integer FSD_CmpFile::RootSectionSize() 
{
  Standard_Integer i;

  if (!(myStream >> i)) Storage_StreamTypeMismatchError::Raise();
  
  FlushEndOfLine();
  
  return i;
}

//=======================================================================
//function : ReadRoot
//purpose  : 
//=======================================================================

void FSD_CmpFile::ReadRoot(TCollection_AsciiString& rootName, Standard_Integer& aRef,TCollection_AsciiString& rootType) 
{
  if (!(myStream >> aRef)) Storage_StreamTypeMismatchError::Raise();
  ReadWord(rootName);
  ReadWord(rootType);
}

//=======================================================================
//function : EndReadRootSection
//purpose  : REF SECTION
//           write
//=======================================================================

Storage_Error FSD_CmpFile::EndReadRootSection() 
{
  return FindTag("END_ROOT_SECTION");
}

//=======================================================================
//function : BeginWriteRefSection
//purpose  : -------------------------- REF : WRITE
//=======================================================================

Storage_Error FSD_CmpFile::BeginWriteRefSection() 
{
  myStream << "BEGIN_REF_SECTION\n";
  if (myStream.bad()) Storage_StreamWriteError::Raise();
  return Storage_VSOk;
}

//=======================================================================
//function : SetRefSectionSize
//purpose  : 
//=======================================================================

void FSD_CmpFile::SetRefSectionSize(const Standard_Integer aSize) 
{
  myStream << aSize << "\n";
  if (myStream.bad()) Storage_StreamWriteError::Raise();
}

//=======================================================================
//function : WriteReferenceType
//purpose  : 
//=======================================================================

void FSD_CmpFile::WriteReferenceType(const Standard_Integer reference,
				  const Standard_Integer typeNum) 
{
  myStream << reference << " " << typeNum << "\n";
  if (myStream.bad()) Storage_StreamWriteError::Raise();
}

//=======================================================================
//function : EndWriteRefSection
//purpose  : read
//=======================================================================

Storage_Error FSD_CmpFile::EndWriteRefSection() 
{
  myStream << "END_REF_SECTION\n";
  if (myStream.bad()) Storage_StreamWriteError::Raise();
  return Storage_VSOk;
}

//=======================================================================
//function : BeginReadRefSection
//purpose  : ----------------------- REF : READ
//=======================================================================

Storage_Error FSD_CmpFile::BeginReadRefSection() 
{
  return FindTag("BEGIN_REF_SECTION");
}

//=======================================================================
//function : RefSectionSize
//purpose  : 
//=======================================================================

Standard_Integer FSD_CmpFile::RefSectionSize() 
{
  Standard_Integer i;

  if (!(myStream >> i)) Storage_StreamTypeMismatchError::Raise();
  FlushEndOfLine();

  return i;
}

//=======================================================================
//function : ReadReferenceType
//purpose  : 
//=======================================================================

void FSD_CmpFile::ReadReferenceType(Standard_Integer& reference,
				 Standard_Integer& typeNum) 
{
  if (!(myStream >> reference)) Storage_StreamTypeMismatchError::Raise();
  if (!(myStream >> typeNum)) Storage_StreamTypeMismatchError::Raise();
  FlushEndOfLine();
}

//=======================================================================
//function : EndReadRefSection
//purpose  : DATA SECTION
//           write
//=======================================================================

Storage_Error FSD_CmpFile::EndReadRefSection() 
{
  return FindTag("END_REF_SECTION");
}

//=======================================================================
//function : BeginWriteDataSection
//purpose  : -------------------- DATA : WRITE
//=======================================================================

Storage_Error FSD_CmpFile::BeginWriteDataSection() 
{
  myStream << "BEGIN_DATA_SECTION";
  if (myStream.bad()) Storage_StreamWriteError::Raise();
  return Storage_VSOk;
}

//=======================================================================
//function : WritePersistentObjectHeader
//purpose  : 
//=======================================================================

void FSD_CmpFile::WritePersistentObjectHeader(const Standard_Integer aRef,
					   const Standard_Integer aType) 
{
  myStream << "\n#" << aRef << "%" << aType << " ";
  if (myStream.bad()) Storage_StreamWriteError::Raise();
}

//=======================================================================
//function : BeginWritePersistentObjectData
//purpose  : 
//=======================================================================

void FSD_CmpFile::BeginWritePersistentObjectData() 
{
  if (myStream.bad()) Storage_StreamWriteError::Raise();
}

//=======================================================================
//function : BeginWriteObjectData
//purpose  : 
//=======================================================================

void FSD_CmpFile::BeginWriteObjectData() 
{
  if (myStream.bad()) Storage_StreamWriteError::Raise();
}

//=======================================================================
//function : EndWriteObjectData
//purpose  : 
//=======================================================================

void FSD_CmpFile::EndWriteObjectData() 
{
  if (myStream.bad()) Storage_StreamWriteError::Raise();
}

//=======================================================================
//function : EndWritePersistentObjectData
//purpose  : 
//=======================================================================

void FSD_CmpFile::EndWritePersistentObjectData() 
{
  if (myStream.bad()) Storage_StreamWriteError::Raise();
}

//=======================================================================
//function : EndWriteDataSection
//purpose  : read
//=======================================================================

Storage_Error FSD_CmpFile::EndWriteDataSection() 
{
  myStream << "\nEND_DATA_SECTION\n";
  if (myStream.bad()) Storage_StreamWriteError::Raise();
  return Storage_VSOk;
}

//=======================================================================
//function : BeginReadDataSection
//purpose  : ---------------------- DATA : READ
//=======================================================================

Storage_Error FSD_CmpFile::BeginReadDataSection() 
{
  return FindTag("BEGIN_DATA_SECTION");
}

//=======================================================================
//function : ReadPersistentObjectHeader
//purpose  : 
//=======================================================================

void FSD_CmpFile::ReadPersistentObjectHeader(Standard_Integer& aRef,
					  Standard_Integer& aType) 
{
  char c;

  myStream.get(c);

  while (c != '#') {
    if (IsEnd() || (c != ' ') || (c == '\r')|| (c == '\n')) {
      Storage_StreamFormatError::Raise();
    }
    myStream.get(c);
  }

  if (!(myStream >> aRef)) Storage_StreamTypeMismatchError::Raise();

  myStream.get(c);

  while (c != '%') {
    if (IsEnd() || (c != ' ') || (c == '\r')|| (c == '\n')) {
      Storage_StreamFormatError::Raise();
    }
    myStream.get(c);
  }

  if (!(myStream >> aType)) Storage_StreamTypeMismatchError::Raise();
//  cout << "REF:" << aRef << " TYPE:"<< aType << endl;
}

//=======================================================================
//function : BeginReadPersistentObjectData
//purpose  : 
//=======================================================================

void FSD_CmpFile::BeginReadPersistentObjectData() 
{
//cout << "BeginReadPersistentObjectData" << endl;
}

//=======================================================================
//function : BeginReadObjectData
//purpose  : 
//=======================================================================

void FSD_CmpFile::BeginReadObjectData() 
{
//  cout << "BeginReadObjectData" << endl;
}

//=======================================================================
//function : EndReadObjectData
//purpose  : 
//=======================================================================

void FSD_CmpFile::EndReadObjectData() 
{
//  cout << "EndReadObjectData" << endl;
}

//=======================================================================
//function : EndReadPersistentObjectData
//purpose  : 
//=======================================================================

void FSD_CmpFile::EndReadPersistentObjectData() 
{
  char c;

  myStream.get(c);
  while (c != '\n' && (c != '\r')) {
    if (IsEnd() || (c != ' ')) {
      Storage_StreamFormatError::Raise();
    }
    myStream.get(c);
  }
 if (c == '\r') {
   myStream.get(c);
 }
//  cout << "EndReadPersistentObjectData" << endl;
}

//=======================================================================
//function : EndReadDataSection
//purpose  : 
//=======================================================================

Storage_Error FSD_CmpFile::EndReadDataSection() 
{
  return FindTag("END_DATA_SECTION");
}

//=======================================================================
//function : Tell
//purpose  : return position in the file. Return -1 upon error.
//=======================================================================

Storage_Position FSD_CmpFile::Tell()
{
  switch (OpenMode()) {
  case Storage_VSRead:
    return (Storage_Position) myStream.tellp();
  case Storage_VSWrite:
    return (Storage_Position) myStream.tellg();
  case Storage_VSReadWrite: {
    Storage_Position aPosR  = (Storage_Position) myStream.tellp();
    Storage_Position aPosW  = (Storage_Position) myStream.tellg();
    if (aPosR < aPosW)
      return aPosW;
    else
      return aPosR;
  }
  default: return -1;
  }
  return -1;
}
