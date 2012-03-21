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

#ifndef __INCLUDE_LIBRARY_COMAND_H
# define __INCLUDE_LIBRARY_COMAND_H
			/* comand.h */

#define BUFFER_SIZE BUF_CMD_SIZ_BYT
				/* Taille des buffers input/output pour les */
				/* messages. */

#define SIZE_CMD_BUFF BUFFER_SIZE - 7 * sizeof(short)


/*------------------------ Macro definitions -------------------------------*/
/*---------- Pour entetes buffers commande et application ------------------*/
                                  

#define GETSEQ_HDR(H)	(0x03030303 & (H))
#define GETLGR_HDR(H)	((H) >> 2 & 0x3f | (H) >> 4 & 0xfc0)
#define GETNARG_HDR(H)	GETLGR_HDR(H)
#define GETLVL_HDR(H)	((H) >> 18 & 0x3f | (H) >> 20 & 0xfc0)
#define MAKE_HDR(S,LVL,X) ((S) \
	      		 | (X) << 2 & 0xfc | (X) << 4 & 0xfc00 \
			 | (LVL) << 18 & 0xfc0000 | (LVL) << 20 & 0xfc000000)


	/* Format des requetes/reponses, longueurs structures et codes */

				/* Codes des commandes serveur : */

#define COD_CMD_CTL	-6
				/* SRVCTL */
#define COD_CMD_CNX	-5		
				/* SRVCNX */
#define COD_CMD_EXI	-4		
				/* SRVEXI */
#define COD_CMD_ASS	-3		
				/* SRVASS */
#define COD_CMD_ASN	-2		
				/* SRVASN */
#define COD_CMD_DEA	-1		
				/* SRVDEA */
#define COD_CMD_DAS	0		
				/* SRVDAS */

				/* Les codes des fonctions service applicatif */
				/* vont de 1 a n (n dependant du service */
				/* applicatif). */

                                /* Codes des fonctions de service (srvctl) */
#define COD_CTL_PRT         0   /* Sortie de la sd du serveur */
#define COD_CTL_RESTART     1   /* Arret et Restart du serveur */
#define COD_CTL_ACT_RESTART 2   /* Arret et Restart du serveur s'il est actif
                                   (k_func a TRUE) */
#define COD_CTL_STOP        3   /* Arret du serveur (non fait) */
#define COD_CTL_CPU         4   /* Rend le type de CPU du serveur */
#define COD_CTL_ERRMSG      5   /* Rend le message d'erreur <--> status */
#define COD_CTL_DYNLOAD     6   /* Requete de chargement dynamique d'un
                                   partageable */
#define COD_CTL_STARTLOAD   7   /* Comme COD_CTL_DYNLOAD + Start_only ou
                                   multi-connection dans le process fils cree
                                   par Serpc */
#define COD_CTL_ETHADR      8   /* Permet de recuperer l'adresse Ethernet ou
                                   le systeme identification */
#define COD_CTL_DYNNOLOAD   9   /* Requete de chargement dynamique d'un
                                   partageable */
#define COD_CTL_STARTNOLOAD 10   /* Comme COD_CTL_DYNLOAD + Start_only ou
                                   multi-connection dans le process fils cree
                                   par Serpc */

	/* Struct entete buffer application.
	   Commence sur le 2eme long du buffer de commande. */
typedef struct _buf_acp BUF_ACP, *PBUF_ACP ;                      
struct _buf_acp {
	unsigned int level ;
	int func ;			
	unsigned int narg ;
       	unsigned int arg[1] ;
} ;

typedef struct _cmd_buff CMD_BUFF, *PCMD_BUFF ;
struct	_cmd_buff {		
				/* CMD_BUFF_OFF	*/
				/* CMD_ARG_OFF = CMD_BUFF_OFF/4 */
				/* Buffer commande/fonction SIZE_CMD_BUFF octets */
				/* Buffer des arguments user */

				/* SRVCNX/SRVASS : */
				/* ACSRV --> SERVER : Node/Taskname Destinataire */
				/* SERVR --> SERVER : Node/taskname Origine */
	int siz_nod ;		/* SIZ_NOD_OFF		12  */
				/* Longueur node destinataire        4 octets */
	char nam_nod[16] ;	/* NAM_NOD_OFF		16 */
				/* Nom node destinataire            16 octets */
	int siz_tsk ;		/* SIZ_TSK_OFF		32 */
				/* Longueur task destinatrice        4 octets */
	char nam_tsk[16] ;	/* NAM_TSK_OFF		36 */
				/* Nom task destinatrice            16 octets */
				/* Node/Username/Pid Client : */
	int siz_nod_org ; 	/* SIZ_NOD_ORG_OFF	52 */
				/* Longueur node origine             4 octets */
	char nam_nod_org[16] ;	/* NAM_NOD_ORG_OFF	56 */
 				/* Nom node origine                 16 octets */
	int siz_usr_org ; 	/* SIZ_USR_ORG_OFF	72 */
				/* Longueur Username origine         4 octets */
	char nam_usr_org[16] ;	/* NAM_USR_ORG_OFF	76 */
				/* Username origine                 16 octets */
	int siz_pid_org ;	/* SIZ_PID_ORG_OFF 	92 */
				/*                                   4 octets */
	int val_pid_org ;	/* VAL_PID_ORG_OFF	96 */
	int val_pid_org_ext ;
				/* Process identification PID        8 octets */
				/* ACSRV --> SERVER : Protocole utilise (CNX) */
				/* SERVR --> SERVER : Protocole a utiliser(ASS) */

	int siz_ptc ;		/* SIZ_PTC_OFF	104 */
				/*                                   4 octets */
	int val_ptc ;		/* VAL_PTC_OFF	108 */
				/* Protocole (DECNET/TCP-IP)         4 octets */

	char dummy[SIZE_CMD_BUFF - 100] ;
                                /* zone remplissage jusqu a SIZE_CMD_BUFF */
} ;

typedef struct _buf BUF, *PBUF ;
struct _buf {

	unsigned int lg_buff ;	/* LG_BUFF_OFF	0 */
				/* Longueur commande/reponse         4 octets */
				/* LVL_OFF2     2 */
				/* Level (apres la longueur ! ...)   	      */
	short lnkdest ;		/* LNKDEST_OFF  4 */
				/* Lnk destinataire                  2 octets */
	short lnkorg ;		/* LNKORG_OFF	6 */
				/* Lnk origine                       2 octets */
	short func ;		/* FUNC_OFF     8 */
				/* Index/numero de fonction          2 octets */
	short numcmd ;		/* NUMCMD_OFF  10 */
				/* Numero de commande                2 octets */
	CMD_BUFF cmd_buf ;

} ;

typedef struct _buf_rsp BUF_RSP, *PBUF_RSP ;
struct _buf_rsp {

	unsigned int lg_buff ;	/* LG_BUFF_OFF	 0 %%%% */
				/* Longueur commande/reponse        4 octets */
				/* LVL_OFF       0 reference dans READ_CHAN  */
				/* Level (apres la longueur ! ...)            */
				/* LVL_OFF2 	 2 */
				/* Level (apres la longueur ! ...)            */
	short lnkdest ;		/* LNKDEST_OFF 	 4 */
				/* Lnk destinataire                  2 octets */
	short lnkorg ;		/* LNKORG_OFF	 6 */
				/* Lnk origine                       2 octets */
	short func ;		/* FUNC_OFF   	 8 */
				/* Index/numero de fonction          2 octets */
	short numcmd ;		/* NUMCMD_OFF  	10 */
				/* Numero de commande                2 octets */
	STATUS status ;	        /* STATUS_OFF  	12 */
				/* Status                            8 octets */
	unsigned int rsp_buff[(BUFFER_SIZE - 20) / 4] ;
				/* RSP_BUFF_OFF	20 */
				/* Buffer reponse Fonctions         ? octets */
} ;


				/* Longueurs des commandes serveur : */
#define LG_CMD_SRV	12		
				/* Longueur entete des commandes/fonctions */
#define LG_CMD_CNX	112
				/* Longueur commande SRVCNX */
#define LG_CMD_ASS	112
				/* Longueur commande SRVASS */
#define LG_CMD_DEA	12		
				/* Longueur commande SRVDEA */
#define LG_CMD_EXI	12		
				/* Longueur commande SRVEXI */
#define LG_CMD_CTL	20		
				/* Longueur commande SRVCTL sans argument */

				/* Longueurs des reponses serveur : */
#define LG_RSP_SRV	20		
				/* Longueur minimum reponse (avec status) */
#define LG_RSP_CNX	20		
				/* Longueur reponse SRVCNX */
#define LG_RSP_ASS	20		
				/* Longueur reponse SRVASS */
#define LG_RSP_DEA	20		
				/* Longueur reponse SRVDEA */
#define LG_RSP_EXI	20		
				/* Longueur reponse SRVEXI */
#define LG_RSP_CTL	20		
				/* Longueur reponse SRVCTL */

				/* Longueur des arguments des commandes : */
#define LG_NOD		16		
				/* Nodes names */
#define HOSTNAMELEN	256		
				/* Nodes names (AccesServer-Unix-Domains) */
#define LG_TSK		80
				/* Tasknames */
#define LG_USR		16 
				/* Usernames */
#define LG_PID		8		              
				/* PID Process Identification */
#define LG_PID_DEC	4		
				/* 4 octets de PID sur VAX/VMS */
#define LG_PTC		4		
				/* Type de protocole (DECNET/TCP-IP) */

#define LVL_VAL0	0x1
				/* Level des requetes ACSRV/serveur Macro */
#define LVL_VAL		0x2
				/* Level des requetes ACSRV "C" */
				/* Le serveur Macro sans NTD refuse ce level 1*/
				/* Le serveur Macro+NTD accepte ces 2 levels*/
				/* Le serveur C accepte ces 2 levels */
				/* ACSRV "C" refuse le level 0 */
#define LVL_WRT_VAL	0x3
				/*  Level reponses serveurs Macro+NTD et C */
				/* Type de format des requetes pour serveur */
				/* Valeurs reelles des levels precedents */
				/* cadres a droite */
#define SEQ_BYT_VAL	0x03020100	
				/* Ordre des octets (VAX) */
#define MSK_SEQ_BYT	0x03030303
				/* Masque pour ordre des octets */
#define MSK_SEQ_BYT_VAL	0xFCFCFCFC	
				/* Complement masque pour ordre des octets */
/*
; LVL : Protocol level entre Client/Serveur et Serveur/Serveur 
; SEQ : Sequence des octets : pour tout ce qui n'est pas commande/reponse 
;                             (Longueur, Level, Lnkdest, Lnkorg, Cmd/Func, 
;                              Numcmd et Status pour la reponse) 

; ARG : Nombre d'arguments du buffer de commande. Il comporte aussi la
;       sequence des octets du buffer de commande qui peut etre differente
;       de celle des informations acces_serveur/serveur. Et enfin il comporte
;       le niveau de protocole applicatif (Structure des commandes/reponses
;       et type de dialogue application/service applicatif).
;       Actuellement, dans la protection, chaque valeur d'argument est
;       precedee de son type et de sa longueur et le type de protocole
;       applicatif est nul.

;       La reponse generee par le service applicatif doit suivre l'ordre des
;       octets de l'application cliente (cet ordre etant connu du service
;       applicatif dans la commande). La reponse ne comporte donc pas d'ordre
;       des octets. Si le type de protocole applicatif n'est pas compatible
;       avec le service applicatif, ce dernier rend un status. Sinon, il
;       traite la commande et fournit une reponse. Il n'y a donc pas dans la
;       reponse de type de protocole. Actuellement, dans la protection, la
;       reponse est constituee uniquement des valeurs sans nombre d'arguments.
;       Il serait souhaitable qu'elle comporte nombre d'arguments et
;       type/longueur de chaque argument avant sa valeur comme dans les
;       commandes.
*/

#endif  /* __INCLUDE_LIBRARY_COMAND_H */
