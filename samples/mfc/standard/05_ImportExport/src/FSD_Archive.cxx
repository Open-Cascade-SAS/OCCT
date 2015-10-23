
#include "stdafx.h"

#include "FSD_CArchive.hxx"
#include "FSD_Archive.ixx"

#include <FSD_CFile.hxx>

#define FSD_REF 'R'
#define FSD_CHA 'c'
#define FSD_EXT 'C'
#define FSD_INT 'I'
#define FSD_BOO 'B'
#define FSD_REA 'F'
#define FSD_SHO 'f'
#define FSD_END 'E'

const Standard_CString MAGICNUMBER = "FSDARCH";

FSD_Archive::FSD_Archive()
{
  myStream = NULL;
  myEof = Standard_False;
  myExternFlag = Standard_False;
  myCFile = NULL;
  myFormat = Standard_False;
}

FSD_Archive::FSD_Archive(const FSD_CArchive& anArchive)
{
  myStream = anArchive;
  myEof = Standard_False;
  myExternFlag = Standard_True;
  myCFile = NULL;
  myFormat = Standard_False;
  if (myStream->IsLoading()) SetOpenMode(Storage_VSRead);
  else SetOpenMode(Storage_VSWrite);
}

Storage_Error FSD_Archive::IsGoodFileType(const TCollection_AsciiString& aName)
{
  FSD_Archive      f;
  Storage_Error s;
  s = f.Open(aName,Storage_VSRead);

  if (s == Storage_VSOk) {
    TCollection_AsciiString l;
    Standard_Integer len = (int)strlen(FSD_Archive::MagicNumber());

    f.ReadChar(l,len);

    f.Close();

    if (strncmp(FSD_Archive::MagicNumber(),l.ToCString(),len) != 0) {
      s = Storage_VSFormatError;
    }
  }
  return s;
}

Storage_Error FSD_Archive::Open(const TCollection_AsciiString& theName,
                                const Storage_OpenMode aMode)
{
  Storage_Error result = Storage_VSOk;
  CString aName = theName.ToCString();
  SetName (theName);
  if (OpenMode() == Storage_VSNone) {
	if (aMode == Storage_VSRead) {
		if (!((FSD_CFile*)myCFile)->Open(aName, CFile::modeRead))
				result = Storage_VSOpenError;
	}
	else if (aMode == Storage_VSWrite) {
		if (!((FSD_CFile*)myCFile)->Open(aName, CFile::modeCreate | CFile::modeWrite))
				result = Storage_VSOpenError;
	}
	else if (aMode == Storage_VSReadWrite) {
		if (!((FSD_CFile*)myCFile)->Open(aName, CFile::modeReadWrite))
				result = Storage_VSOpenError;
	}
	if (result == Storage_VSOk) {
		if (aMode == Storage_VSRead) {
			myStream = new CArchive( ((FSD_CFile*)myCFile), CArchive::load);
		}
		else {
			myStream = new CArchive( ((FSD_CFile*)myCFile), CArchive::store);
		}

		SetOpenMode(aMode);
	}

	myEof = Standard_False;

  }
  else {
	  result = Storage_VSAlreadyOpen;
  }
  return result;
}

Standard_Boolean FSD_Archive::IsEnd()
{
	return myEof;
}

Storage_Error FSD_Archive::Close()
{
  Storage_Error result = Storage_VSOk;

  myEof = Standard_False;

  if (OpenMode() != Storage_VSNone) {
      if (!myExternFlag) {
  		(*myStream).Close();
		((FSD_CFile*)myCFile)->Close();
		delete myStream;
      }
      myExternFlag = Standard_False;
	myStream = NULL;
      SetOpenMode(Storage_VSNone);
  }
  else {
    result = Storage_VSNotOpen;
  }

  return result;
}

// ------------------ PROTECTED

const Standard_CString FSD_Archive::MagicNumber()
{
  return MAGICNUMBER;
}


void FSD_Archive::ReadLine(TCollection_AsciiString& buffer)
{
  TRY {
	char c;
	Standard_Boolean IsEnd = Standard_False;
  
	buffer.Clear();

	while (!IsEnd) {
	    (*myStream) >> c;

	    if (c != '\0') {
			buffer += c;
		}
		else {
			buffer += c;
			IsEnd = Standard_True;
		}
	}
  }
  CATCH(CArchiveException,e) {
	if (e->m_cause == CArchiveException::endOfFile) myEof = Standard_True;
  }
  END_CATCH
}

void FSD_Archive::WriteLine(const TCollection_AsciiString& buffer)
{
	Standard_Integer i;
  for (i = 1; i <= buffer.Length(); i++) {
	  (*myStream) << buffer.Value(i);
	}
  (*myStream) << (Standard_Character)0;
}

void FSD_Archive::WriteExtendedLine(const TCollection_ExtendedString& buffer)
{
  Standard_ExtString extBuffer;
  Standard_Integer   i;

  extBuffer = buffer.ToExtString(); 

  for (i = 0; i < buffer.Length(); i++) {
    (*myStream) << extBuffer[i];
  }

  (*myStream) << (Standard_ExtCharacter)0;
}

void FSD_Archive::ReadExtendedLine(TCollection_ExtendedString& buffer)
{
  TRY {
	Standard_ExtCharacter i = 0;
	Standard_Boolean fin = Standard_False;

	buffer.Clear();

	while (!fin) {
	    (*myStream) >> i;
		if (i == 0) fin = Standard_True;
	    else buffer += (Standard_ExtCharacter)i;
	}
  }
  CATCH(CArchiveException,e) {
	if (e->m_cause == CArchiveException::endOfFile) myEof = Standard_True;
  }
  END_CATCH
}

void FSD_Archive::ReadChar(TCollection_AsciiString& buffer, const Standard_Integer rsize)
{
  TRY {
	char             c;
	Standard_Integer ccount = 0;

	buffer.Clear();

	while (ccount < rsize) {
	    (*myStream) >> c;
	    buffer += c;
	    ccount++;
	}
  }
  CATCH(CArchiveException,e) {
	if (e->m_cause == CArchiveException::endOfFile) myEof = Standard_True;
  }
  END_CATCH
}


Storage_Error FSD_Archive::FindTag(const Standard_CString aTag)
{
  TCollection_AsciiString l;
  Standard_CString str;

  ReadLine(l);
  str = l.ToCString();

  Standard_Integer aNum = 0;

  if(!myFormat)
    aNum = 1;

  while ((strcmp(&str[aNum],aTag) != 0) && !IsEnd()) {
//  while ((strcmp(&str[1],aTag) != 0) && !IsEnd()) {
    ReadLine(l);
    str = l.ToCString();
  }

  if (IsEnd()) {
    return Storage_VSSectionNotFound;
  }
  else {
    return Storage_VSOk;
  }
}

void FSD_Archive::SkipObject()
{
	Standard_Boolean errorStatus = Standard_False;
	TRY {
		char c;
		Standard_Boolean IsEnd = Standard_False;
		Standard_Integer rtmp,itmp;
		Standard_Real    ftmp;
		Standard_ShortReal stmp;
		Standard_Integer btmp;
		Standard_Character ctmp;
		Standard_ExtCharacter etmp;

		ReadPersistentObjectHeader(rtmp,itmp);

		while (!IsEnd) {
			(*myStream) >> c;

			switch (c) {
				case FSD_REF:
					(*myStream) >> rtmp;
					break;
				case FSD_CHA:
					(*myStream) >> ctmp;
					break;
				case FSD_EXT:
					(*myStream) >> etmp;
					break;
				case FSD_INT:
					(*myStream) >> itmp;
					break;
				case FSD_BOO:
					(*myStream) >> btmp;
					break;
				case FSD_REA:
					(*myStream) >> ftmp;
					break;
				case FSD_SHO:
					(*myStream) >> stmp;
					break;
				case FSD_END: 
					IsEnd = Standard_True;
					break;
				default: 
					errorStatus = Standard_True;
					IsEnd = Standard_True;
					break;
			}
		}
	}
	CATCH(CArchiveException,e) {
		if ((e->m_cause != CArchiveException::endOfFile) &&
			(e->m_cause != CArchiveException::none)) errorStatus = Standard_True;
		else if (e->m_cause == CArchiveException::endOfFile) myEof = Standard_True;
	}
	END_CATCH
}

// ---------------------- PUBLIC : PUT

Storage_BaseDriver& FSD_Archive::PutReference(const Standard_Integer aValue)
{
  Standard_Boolean errorStatus = Standard_False;

  TRY {
	(*myStream) << FSD_REF << aValue;
  }
  CATCH(CArchiveException,e) {
	errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) Storage_StreamWriteError::Raise();
  return *this;
}

Storage_BaseDriver& FSD_Archive::PutCharacter(const Standard_Character aValue)
{
  Standard_Boolean errorStatus = Standard_False;

  TRY {
	//unsigned short i = aValue;
	//(*myStream) << FSD_CHA << i;
	(*myStream) << FSD_CHA << aValue;
  }
  CATCH(CArchiveException,e) {
	errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) Storage_StreamWriteError::Raise();
  return *this;
}

Storage_BaseDriver& FSD_Archive::PutExtCharacter(const Standard_ExtCharacter aValue)
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	(*myStream) << FSD_EXT << aValue;
  }
  CATCH(CArchiveException,e) {
	errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) Storage_StreamWriteError::Raise();
  return *this;
}

Storage_BaseDriver& FSD_Archive::PutInteger(const Standard_Integer aValue)
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	(*myStream) << FSD_INT << aValue;
  }
  CATCH(CArchiveException,e) {
	errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) Storage_StreamWriteError::Raise();
  return *this;
}

Storage_BaseDriver& FSD_Archive::PutBoolean(const Standard_Boolean aValue)
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	(*myStream) << FSD_BOO << ((Standard_Integer)aValue);
  }
  CATCH(CArchiveException,e) {
	errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) Storage_StreamWriteError::Raise();
  return *this;
}

Storage_BaseDriver& FSD_Archive::PutReal(const Standard_Real aValue)
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	(*myStream) << FSD_REA << ((Standard_Real)aValue);
  }
  CATCH(CArchiveException,e) {
	errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) Storage_StreamWriteError::Raise();
  return *this;
}

Storage_BaseDriver& FSD_Archive::PutShortReal(const Standard_ShortReal aValue)
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	(*myStream) << FSD_SHO << aValue;
  }
  CATCH(CArchiveException,e) {
	errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) Storage_StreamWriteError::Raise();
  return *this;
}

// ----------------- PUBLIC : GET

Storage_BaseDriver& FSD_Archive::GetReference(Standard_Integer& aValue)
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	char ptype;
	(*myStream) >> ptype;
	if (ptype == FSD_REF)
		(*myStream) >> aValue;
	else errorStatus = Standard_True;
  }
  CATCH(CArchiveException,e) {
	if (e->m_cause == CArchiveException::endOfFile) myEof = Standard_True;
	else errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) Storage_StreamTypeMismatchError::Raise();
  return *this;
}

Storage_BaseDriver& FSD_Archive::GetCharacter(Standard_Character& aValue)
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	//unsigned short i;
	Standard_Character i;
	char ptype;
	(*myStream) >> ptype;
	if (ptype == FSD_CHA) {
		(*myStream) >> i;
		aValue = (char)i;
	}
	else errorStatus = Standard_True;
  }
  CATCH(CArchiveException,e) {
	if (e->m_cause == CArchiveException::endOfFile) myEof = Standard_True;
	else errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) Storage_StreamTypeMismatchError::Raise();
  return *this;
}

Storage_BaseDriver& FSD_Archive::GetExtCharacter(Standard_ExtCharacter& aValue)
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	char ptype;
	(*myStream) >> ptype;
	if (ptype == FSD_EXT)
		(*myStream) >> aValue;
	else errorStatus = Standard_True;
  }
  CATCH(CArchiveException,e) {
	if (e->m_cause == CArchiveException::endOfFile) myEof = Standard_True;
	else errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) Storage_StreamTypeMismatchError::Raise();
  return *this;
}

Storage_BaseDriver& FSD_Archive::GetInteger(Standard_Integer& aValue)
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	char ptype;
	(*myStream) >> ptype;
	if (ptype == FSD_INT)
		(*myStream) >> aValue;
	else errorStatus = Standard_True;
  }
  CATCH(CArchiveException,e) {
	if (e->m_cause == CArchiveException::endOfFile) myEof = Standard_True;
	else errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) Storage_StreamTypeMismatchError::Raise();
  return *this;
}

Storage_BaseDriver& FSD_Archive::GetBoolean(Standard_Boolean& aValue)
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	char ptype;
	(*myStream) >> ptype;
	if (ptype == FSD_BOO)
		(*myStream) >> aValue;
	else errorStatus = Standard_True;
  }
  CATCH(CArchiveException,e) {
	if (e->m_cause == CArchiveException::endOfFile) myEof = Standard_True;
	else errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) Storage_StreamTypeMismatchError::Raise();
  return *this;
}

Storage_BaseDriver& FSD_Archive::GetReal(Standard_Real& aValue)
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	char ptype;
	(*myStream) >> ptype;
	if (ptype == FSD_REA)
		(*myStream) >> aValue;
	else errorStatus = Standard_True;
  }
  CATCH(CArchiveException,e) {
	if (e->m_cause == CArchiveException::endOfFile) myEof = Standard_True;
	else errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) Storage_StreamTypeMismatchError::Raise();
  return *this;
}

Storage_BaseDriver& FSD_Archive::GetShortReal(Standard_ShortReal& aValue)
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
char ptype;
	(*myStream) >> ptype;
	if (ptype == FSD_SHO)
		(*myStream) >> aValue;
	else errorStatus = Standard_True;
  }
  CATCH(CArchiveException,e) {
	if (e->m_cause == CArchiveException::endOfFile) myEof = Standard_True;
	else errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) Storage_StreamTypeMismatchError::Raise();
  return *this;
}

// -------------------------- DESTROY

void FSD_Archive::Destroy()
{
  if (OpenMode() != Storage_VSNone) {
    Close();
    if (myCFile) {
      delete ((FSD_CFile*)myCFile);
      myCFile = NULL;
    }

  }
}

// -------------------------- INFO : WRITE

Storage_Error FSD_Archive::BeginWriteInfoSection() 
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	  //(*myStream) << FSD_Archive::MagicNumber();
  WriteLine(TCollection_AsciiString(FSD_Archive::MagicNumber()));
		//(*myStream) << "BEGIN_INFO_SECTION" << '\0';
  WriteLine(TCollection_AsciiString("BEGIN_INFO_SECTION"));
  }
  CATCH(CArchiveException,e) {
	errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) Storage_StreamWriteError::Raise();
  return Storage_VSOk;
}

void FSD_Archive::WriteInfo(const Standard_Integer nbObj,
                            const TCollection_AsciiString& dbVersion,
                            const TCollection_AsciiString& date,
                            const TCollection_AsciiString& schemaName,
                            const TCollection_AsciiString& schemaVersion,
                            const TCollection_ExtendedString& appName,
                            const TCollection_AsciiString& appVersion,
                            const TCollection_ExtendedString& dataType,
                            const TColStd_SequenceOfAsciiString& userInfo)
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	Standard_Integer i;

	(*myStream) << nbObj;
	//(*myStream) << dbVersion.ToCString() << '\0';
  WriteLine(dbVersion);
	//(*myStream) << date.ToCString() << '\0';
  WriteLine(date);
	//(*myStream) << schemaName.ToCString() << '\0';
  WriteLine(schemaName);
	//(*myStream) << schemaVersion.ToCString() << '\0';
  WriteLine(schemaVersion);
	WriteExtendedLine(appName);
	//(*myStream) << appVersion.ToCString() << '\0';
  WriteLine(appVersion);
	WriteExtendedLine(dataType);
	(*myStream) << userInfo.Length();
	
	for (i = 1; i <= userInfo.Length(); i++) {
	    //(*myStream) << userInfo.Value(i).ToCString() << '\0';
    WriteLine(userInfo.Value(i));
	}
  }
  CATCH(CArchiveException,e) {
	errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) Storage_StreamWriteError::Raise();
}


Storage_Error FSD_Archive::EndWriteInfoSection() 
{

  Standard_Boolean errorStatus = Standard_False;
  TRY {
	//(*myStream) << "END_INFO_SECTION" << '\0';
  WriteLine(TCollection_AsciiString("END_INFO_SECTION"));
 }
  CATCH(CArchiveException,e) {
	errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) Storage_StreamWriteError::Raise();
  return Storage_VSOk;
}

Storage_Error FSD_Archive::BeginReadInfoSection() 
{
  Storage_Error s;

  TCollection_AsciiString l;
  Standard_Integer len = (int)strlen(FSD_Archive::MagicNumber());

  // first character is length of the magic number
  ReadChar(l,1);

  if(l.ToCString()[0] != (Standard_Character) len) 
    myFormat = Standard_True;

  if(myFormat) {
    TCollection_AsciiString ll;
    ReadLine(ll);
    l += ll;
  } else {
    ReadChar(l,len);
  }
  if (strncmp(FSD_Archive::MagicNumber(),l.ToCString(),len) != 0) {
    s = Storage_VSFormatError;
  }
  else {
    s = FindTag("BEGIN_INFO_SECTION");
  }
  return s;
}

// ------------------- INFO : READ

void FSD_Archive::ReadInfo(Standard_Integer& nbObj,
                           TCollection_AsciiString& dbVersion,
                           TCollection_AsciiString& date,
                           TCollection_AsciiString& schemaName,
                           TCollection_AsciiString& schemaVersion,
                           TCollection_ExtendedString& appName,
                           TCollection_AsciiString& appVersion,
                           TCollection_ExtendedString& dataType,
                           TColStd_SequenceOfAsciiString& userInfo) 
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	(*myStream) >> nbObj;

	ReadLine(dbVersion);
	ReadLine(date);
	ReadLine(schemaName);
	ReadLine(schemaVersion);
	ReadExtendedLine(appName);
	ReadLine(appVersion);
	ReadExtendedLine(dataType);

	Standard_Integer i,len = 0;

	(*myStream) >> len;

	TCollection_AsciiString line;

	for (i = 1; i <= len && !IsEnd(); i++) {
		ReadLine(line);
		userInfo.Append(line);
		line.Clear();
	}
  }
  CATCH(CArchiveException,e) {
	if (e->m_cause == CArchiveException::endOfFile) myEof = Standard_True;
	else errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) 
	  Storage_StreamTypeMismatchError::Raise();	
}

Storage_Error FSD_Archive::EndReadInfoSection() 
{
  return FindTag("END_INFO_SECTION");
}

// ---------------- COMMENTS : WRITE

Storage_Error FSD_Archive::BeginWriteCommentSection() 
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	//(*myStream) << "BEGIN_COMMENT_SECTION" << '\0';
  WriteLine(TCollection_AsciiString("BEGIN_COMMENT_SECTION"));
  }
  CATCH(CArchiveException,e) {
	errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) Storage_StreamWriteError::Raise();  
  return Storage_VSOk;
}

void FSD_Archive::WriteComment(const TColStd_SequenceOfExtendedString& aCom)
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	Standard_Integer i,aSize;

	aSize = aCom.Length();
	(*myStream) << aSize; 
	for (i = 1; i <= aSize; i++) {
		WriteExtendedLine(aCom.Value(i));
	}
  }
  CATCH(CArchiveException,e) {
	errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) 
	  Storage_StreamWriteError::Raise();
}

Storage_Error FSD_Archive::EndWriteCommentSection() 
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	//(*myStream) << "END_COMMENT_SECTION" << '\0';
  WriteLine(TCollection_AsciiString("END_COMMENT_SECTION"));
  }
  CATCH(CArchiveException,e) {
	errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) Storage_StreamWriteError::Raise();
  return Storage_VSOk;
}

// ---------------- COMMENTS : READ

Storage_Error FSD_Archive::BeginReadCommentSection() 
{
  return FindTag("BEGIN_COMMENT_SECTION");
}

void FSD_Archive::ReadComment(TColStd_SequenceOfExtendedString& aCom)
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	TCollection_ExtendedString line;
	Standard_Integer           len,i;

	(*myStream) >> len;
  
	for (i = 1; i <= len && !IsEnd(); i++) {
		ReadExtendedLine(line);
		aCom.Append(line);
		line.Clear();
	}
  }
  CATCH(CArchiveException,e) {
	if (e->m_cause == CArchiveException::endOfFile) myEof = Standard_True;
	else errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) 
	  Storage_StreamTypeMismatchError::Raise();
}

Storage_Error FSD_Archive::EndReadCommentSection() 
{
  return FindTag("END_COMMENT_SECTION");
}

// --------------- TYPE : WRITE

Storage_Error FSD_Archive::BeginWriteTypeSection() 
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	//(*myStream) << "BEGIN_TYPE_SECTION" << '\0';
  WriteLine(TCollection_AsciiString("BEGIN_TYPE_SECTION"));
  }
  CATCH(CArchiveException,e) {
	errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) Storage_StreamWriteError::Raise();
  return Storage_VSOk;
}

void FSD_Archive::SetTypeSectionSize(const Standard_Integer aSize)
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	(*myStream) << aSize;
  }
  CATCH(CArchiveException,e) {
	errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) 
	  Storage_StreamWriteError::Raise();
}

void FSD_Archive::WriteTypeInformations (const Standard_Integer typeNum,
                                         const TCollection_AsciiString& typeName)
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	//(*myStream) << typeNum << typeName.ToCString() << '\0';
  (*myStream) << typeNum;
  WriteLine(typeName);
  }
  CATCH(CArchiveException,e) {
	errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) 
	  Storage_StreamWriteError::Raise();
}

Storage_Error FSD_Archive::EndWriteTypeSection() 
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	//(*myStream) << "END_TYPE_SECTION" << '\0';
  WriteLine(TCollection_AsciiString("END_TYPE_SECTION"));
  }
  CATCH(CArchiveException,e) {
	errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) Storage_StreamWriteError::Raise();
  return Storage_VSOk;
}

// ------------------- TYPE : READ

Storage_Error FSD_Archive::BeginReadTypeSection() 
{
  return FindTag("BEGIN_TYPE_SECTION");
}

Standard_Integer FSD_Archive::TypeSectionSize() 
{
  Standard_Integer i = 0;
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	(*myStream) >> i;
  }
  CATCH(CArchiveException,e) {
	if (e->m_cause == CArchiveException::endOfFile) myEof = Standard_True;
	else errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) Storage_StreamTypeMismatchError::Raise();
  return i;
}

void FSD_Archive::ReadTypeInformations(Standard_Integer& typeNum, TCollection_AsciiString& typeName)
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	unsigned char c;
	(*myStream) >> typeNum;
	(*myStream) >> c;
	if(myFormat) {
	  TCollection_AsciiString tn;
	  typeName = (Standard_Character) c;
	  ReadLine(tn);
	  typeName += tn;
	} else {
	  ReadLine(typeName);
	}
  }
  CATCH(CArchiveException,e) {
	if (e->m_cause == CArchiveException::endOfFile) myEof = Standard_True;
	else errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) 
	  Storage_StreamTypeMismatchError::Raise();
}

Storage_Error FSD_Archive::EndReadTypeSection() 
{
  return FindTag("END_TYPE_SECTION");
}

// -------------------- ROOT : WRITE

Storage_Error FSD_Archive::BeginWriteRootSection() 
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	//(*myStream) << "BEGIN_ROOT_SECTION" << '\0';
  WriteLine(TCollection_AsciiString("BEGIN_ROOT_SECTION"));
  }
  CATCH(CArchiveException,e) {
	errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) Storage_StreamWriteError::Raise();
  return Storage_VSOk;
}

void FSD_Archive::SetRootSectionSize(const Standard_Integer aSize)
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	(*myStream) << aSize;
  }
  CATCH(CArchiveException,e) {
	errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) 
	  Storage_StreamWriteError::Raise();
}

void FSD_Archive::WriteRoot (const TCollection_AsciiString& rootName,
                             const Standard_Integer aRef,
                             const TCollection_AsciiString& rootType)
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	//(*myStream) << aRef << rootName.ToCString() << '\0' << rootType.ToCString() << '\0';
  (*myStream) << aRef;
  WriteLine(rootName);
  WriteLine(rootType);

  }
  CATCH(CArchiveException,e) {
	errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) 
	  Storage_StreamWriteError::Raise();
}

Storage_Error FSD_Archive::EndWriteRootSection() 
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	//(*myStream) << "END_ROOT_SECTION" << '\0';
  WriteLine(TCollection_AsciiString("END_ROOT_SECTION"));
  }
  CATCH(CArchiveException,e) {
	errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) Storage_StreamWriteError::Raise();
  return Storage_VSOk;
}

// ----------------------- ROOT : READ

Storage_Error FSD_Archive::BeginReadRootSection() 
{
  return FindTag("BEGIN_ROOT_SECTION");
}

Standard_Integer FSD_Archive::RootSectionSize() 
{
  Standard_Integer i = 0;
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	(*myStream) >> i;
  }
  CATCH(CArchiveException,e) {
	if (e->m_cause == CArchiveException::endOfFile) myEof = Standard_True;
	else errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) Storage_StreamTypeMismatchError::Raise();
  return i;
}

void FSD_Archive::ReadRoot(TCollection_AsciiString& rootName,
                           Standard_Integer& aRef,
                           TCollection_AsciiString& rootType)
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	(*myStream) >> aRef;
	ReadLine(rootName);
	ReadLine(rootType);
  }
  CATCH(CArchiveException,e) {
	if (e->m_cause == CArchiveException::endOfFile) myEof = Standard_True;
	else errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) 
	  Storage_StreamTypeMismatchError::Raise();
}

Storage_Error FSD_Archive::EndReadRootSection() 
{
  return FindTag("END_ROOT_SECTION");
}

// -------------------------- REF : WRITE

Storage_Error FSD_Archive::BeginWriteRefSection() 
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	//(*myStream) << "BEGIN_REF_SECTION" << '\0';
  WriteLine(TCollection_AsciiString("BEGIN_REF_SECTION"));
  }
  CATCH(CArchiveException,e) {
	errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) Storage_StreamWriteError::Raise();
  return Storage_VSOk;
}

void FSD_Archive::SetRefSectionSize(const Standard_Integer aSize)
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	(*myStream) << aSize; 
  }
  CATCH(CArchiveException,e) {
	errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) 
	  Storage_StreamWriteError::Raise();
}

void FSD_Archive::WriteReferenceType(const Standard_Integer reference,
                                     const Standard_Integer typeNum)
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	(*myStream) << reference << typeNum;
  }
  CATCH(CArchiveException,e) {
	errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) 
	  Storage_StreamWriteError::Raise();
}

Storage_Error FSD_Archive::EndWriteRefSection() 
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	//(*myStream) << "END_REF_SECTION" << '\0';
  WriteLine(TCollection_AsciiString("END_REF_SECTION"));
  }
  CATCH(CArchiveException,e) {
	errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) Storage_StreamWriteError::Raise();
  return Storage_VSOk;
}

// ----------------------- REF : READ

Storage_Error FSD_Archive::BeginReadRefSection() 
{
  return FindTag("BEGIN_REF_SECTION");
}

Standard_Integer FSD_Archive::RefSectionSize() 
{
  Standard_Integer i = 0;
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	(*myStream) >> i;
  }
  CATCH(CArchiveException,e) {
	if (e->m_cause == CArchiveException::endOfFile) myEof = Standard_True;
	else errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) Storage_StreamTypeMismatchError::Raise();
  return i;
}

void FSD_Archive::ReadReferenceType(Standard_Integer& reference,
                                    Standard_Integer& typeNum)
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	(*myStream) >> reference;
	(*myStream) >> typeNum;
  }
  CATCH(CArchiveException,e) {
	if (e->m_cause == CArchiveException::endOfFile) myEof = Standard_True;
	else errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) 
	  Storage_StreamTypeMismatchError::Raise();
}

Storage_Error FSD_Archive::EndReadRefSection() 
{
  return FindTag("END_REF_SECTION");
}

// -------------------- DATA : WRITE

Storage_Error FSD_Archive::BeginWriteDataSection() 
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	//(*myStream) << "BEGIN_DATA_SECTION" << '\0';
  WriteLine(TCollection_AsciiString("BEGIN_DATA_SECTION"));
  }
  CATCH(CArchiveException,e) {
	errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) Storage_StreamWriteError::Raise();
  return Storage_VSOk;
}

void FSD_Archive::WritePersistentObjectHeader(const Standard_Integer aRef,
                                              const Standard_Integer aType)
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	(*myStream) << aRef << aType;
  }
  CATCH(CArchiveException,e) {
	errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) 
	  Storage_StreamWriteError::Raise();
}

void FSD_Archive::BeginWritePersistentObjectData() 
{
}

void FSD_Archive::BeginWriteObjectData() 
{
}

void FSD_Archive::EndWriteObjectData() 
{
}

void FSD_Archive::EndWritePersistentObjectData() 
{	
	(*myStream) << 'E';
}

Storage_Error FSD_Archive::EndWriteDataSection() 
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	//(*myStream) << "END_DATA_SECTION" << '\0';
  WriteLine(TCollection_AsciiString("END_DATA_SECTION"));
  }
  CATCH(CArchiveException,e) {
	errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) Storage_StreamWriteError::Raise();
  return Storage_VSOk;
}

// ---------------------- DATA : READ

Storage_Error FSD_Archive::BeginReadDataSection() 
{
  return FindTag("BEGIN_DATA_SECTION");
}

void FSD_Archive::ReadPersistentObjectHeader(Standard_Integer& aRef, Standard_Integer& aType)
{
  Standard_Boolean errorStatus = Standard_False;
  TRY {
	(*myStream) >> aRef;
	(*myStream) >> aType;
  }
  CATCH(CArchiveException,e) {
	if (e->m_cause == CArchiveException::endOfFile) myEof = Standard_True;
	else errorStatus = Standard_True;
  }
  END_CATCH

  if (errorStatus) 
	  Storage_StreamTypeMismatchError::Raise();
}

void FSD_Archive::BeginReadPersistentObjectData() 
{
}

void FSD_Archive::BeginReadObjectData() 
{
}

void FSD_Archive::EndReadObjectData() 
{
}

void FSD_Archive::EndReadPersistentObjectData() 
{
	char theEnd;

	(*myStream) >> theEnd;

	if (theEnd != 'E') {
		Storage_StreamFormatError::Raise();
	}
}

Storage_Error FSD_Archive::EndReadDataSection() 
{
  return FindTag("END_DATA_SECTION");
}

Storage_Position FSD_Archive::Tell() 
{
	return 0; //just a stub
}
