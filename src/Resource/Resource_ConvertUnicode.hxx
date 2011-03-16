// Copyright: 	Matra-Datavision 1996
// File:	Resource_ConvertUnicode.hxx
// Created:	Mon Sep 30 10:03:04 1996
// Author:	Arnaud BOUZY
//		<adn>


#ifndef Resource_ConvertUnicode_HeaderFile
#define Resource_ConvertUnicode_HeaderFile

extern "C" {

void Resource_sjis_to_unicode (unsigned int *ph, unsigned int *pl);
void Resource_unicode_to_sjis (unsigned int *ph, unsigned int *pl);
void Resource_unicode_to_euc (unsigned int *ph, unsigned int *pl);
void Resource_euc_to_unicode (unsigned int *ph, unsigned int *pl);
void Resource_gb_to_unicode(unsigned int *ph, unsigned int *pl);
void Resource_unicode_to_gb(unsigned int *ph, unsigned int *pl);

}

#endif
