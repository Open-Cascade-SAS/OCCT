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
