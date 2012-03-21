/*
 Copyright (c) 1998-1999 Matra Datavision
 Copyright (c) 1999-2012 OPEN CASCADE SAS

 The content of this file is subject to the Open CASCADE Technology Public
 License Version 6.5 (the "License"). You may not use the content of this file
 except in compliance with the License. Please obtain a copy of the License
 at http://www.opencascade.org and read it completely before using this file.

 The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
 main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.

 The Original Code and all software distributed under the License is
 distributed on an "AS IS" basis, without warranty of any kind, and the
 Initial Developer hereby disclaims all such warranties, including without
 limitation, any warranties of merchantability, fitness for a particular
 purpose or non-infringement. Please see the License for the specific terms
 and conditions governing the rights and limitations under the License.

*/

/*------------- Constantes necessaires au utilisateurs des ntd -------------*/ 
	/* valeurs du parametre "reply" de ntd_confirm */
        
#define ACCEPT	1
#define REJECT	0

	/* valeurs du parametre "how" de ntd_disconnect */

#define SYNCH	0
#define ABORT	1
#define SHUT	2

	/* valeurs du parametre "prtcl" de ntd_connect */

#define DECNET	1
#define TCPIP	2
#define DUNIX	3

	/* valeur de retour d'un numero de lien inexistant */

#define	UNKNOWNLNK	-1

	/*-------------- Erreurs ----------------*/

	/* Compte rendu de soumisson ou de fin de traitement d'une fonction */

		/* valeurs du champ stat pour les erreurs internes */

#define INVALIDLNK	1
#define LNKLIMIT	2
#define INVALIDPRTCL	3
                         	/* uniquement sous unix */
#define REMOTEDISC	4
#define READINPROGRESS  5
#define LASTFILEDESC	6
#define LOCALDISC	7
#define UNKNOWNNODE	8
#define ADDRESSINUSE    9	/* tli correspond a "addr. already in use" */
				/* Messages mailbox Decnet */
#define MAILBOXWARNING	4
#define MAILBOXNETSHUT  5
#define MAILBOXBADMSG	6
