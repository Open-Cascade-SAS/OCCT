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

	/* stsdef.h */
        
	/* ACS Error status */

				/* Status non systeme de l'interface d'acces */

/* Error numbers */

#define STS_ACS_SEQ 	1		
				/* Illegal call (sequences d'appel aux syntaxes) */
#define STS_ACS_ROO 	2		
				/* No room (overflow) */
#define STS_ACS_ARG 	3		
				/* Arguments incorrects */
#define STS_ACS_INT 	4		
				/* Erreur interne */
#define STS_ACS_UNR	5		
				/* Node/server currently unreachable */
#define STS_ACS_LEV	6		
				/* Command level incompatible */
#define STS_ACS_BYT	7		
				/* Bytes pas dans l'ordre */
#define STS_ACS_LEN	8		
				/* Longueur incorrecte */
#define STS_ACS_RSP	9		
				/* numero de lien ou de commande incorrect
				   dans une reponse */
        
	/* STSDEF_SRV : Valeurs des status SERVER */

#define STS_SRV_OK       1			
					/* Normal successful completion */

#define STS_SRV_BAD_CONF_CNEC_MBX         1		
						/* Fatal CONFIRM_MBX */
#define STS_SRV_BAD_LNK_MBX               2		
						/* Fatal DISCONNECT_MBX */
#define STS_SRV_BAD_MSG_TYP_MBX           3		
						/* Fatal READ_MBX */
#define STS_SRV_INIT_ACP_MBX              4		
						/* Fatal SERVER */
#define STS_SRV_RESTART_MBX               5
						/* Fatal exit wanted */

#define STS_SRV_ABORT_LNK_DEA_CLT       1
						/* Fatal SRVVMS_CLT */
#define STS_SRV_ALR_ERR_CLT             2		
						/* Fatal WRITE_CLT */
#define STS_SRV_ALR_READ_CLT_AST        3		
						/* Fatal READ_CLT */
#define STS_SRV_ALR_WRT_CLT             4		
						/* Fatal WRITE_CLT */
#define STS_SRV_BAD_CNX_LNK_CLT         5		
						/* Fatal DEALNK_CLT */
#define STS_SRV_BAD_DEST_CLT            6	
						/* Error ---> Client SRVCOM_CLT */
#define STS_SRV_BAD_ERR_WRITR_CLT       7		
						/* Fatal READ_CLT */
#define STS_SRV_BAD_ORG_CLT             8	
						/* Error ---> Client SRVCOM_CLT */
#define STS_SRV_BAD_STS_CLT             9		
						/* Fatal SRVCOM_CLT */
#define STS_SRV_BAD_WRITR_COM_CLT       10		
						/* Fatal WRITE_CLT */
#define STS_SRV_CLT_ALR_ABORT_CLT       11		
						/* Fatal SRVVMS_CLT */
#define STS_SRV_CLT_CHN_NZ_CLT          12		
						/* Fatal DEALNK_CLT */
#define STS_SRV_COM_BUFW_FREE_CLT       13		
						/* Fatal WRITE_CLT */
#define STS_SRV_DEST_CLT                14	
						/* Error ---> Client SRVCOM_CLT */
#define STS_SRV_DISC_NO_DISC_CLT        15		
						/* Fatal DISCONNECT_CLT */
#define STS_SRV_DISC_NO_EXPEC_CLT       16		
						/* Fatal DISCONNECT_CLT */
#define STS_SRV_DISC_NO_GOUT_CLT        17		
						/* Fatal DISCONNECT_CLT */
#define STS_SRV_FUNC_NUL_CLT            18	
						/* Error ---> Client READ_CLT */
#define STS_SRV_ILL_CMD_CLT             19	
						/* Error ---> Client SRVCOM_CLT */
#define STS_SRV_ILL_FUNC_CLT            20	
						/* Error ---> Client READ_CLT */
#define STS_SRV_ILL_SEQ_CAL_CLT         21	
						/* Error ---> Client READ_CLT */
#define STS_SRV_ILL_SEQ_CAL_EXI_CLT     22	
						/* Error ---> Client SRVCOM_CLT */
#define STS_SRV_LEN_READ_CLT            23	
						/* Error ---> Client READ_CLT */
#define STS_SRV_LNK_EXIST_CLT           24	
						/* Error ---> Client SRVEXI_CLT */
#define STS_SRV_NODE_NOABRT_CLT         25		
						/* Fatal SRVVMS_CLT */
#define STS_SRV_NODE_NOT_ABORT_CLT      26		
						/* Fatal SRVVMS_CLT */
#define STS_SRV_NO_ABR_EXP_CLT          27		
						/* Fatal DEALNK_CLT */
#define STS_SRV_NO_ABR_EXPC_CLT         28		
						/* Fatal DEALNK_CLT */
#define STS_SRV_NO_ABR_GOUT_CLT         29		
						/* Fatal DEALNK_CLT */
#define STS_SRV_NO_ABR_NBR_CLT          30		
						/* Fatal DEALNK_CLT */
#define STS_SRV_NO_LNK_CLT              31		
						/* Fatal SRVCOM_CLT */
#define STS_SRV_READ_ABORT_CLT          32	
						/* Informational READ_CLT */
#define STS_SRV_STS_ALR_READ_CLT        33		
						/* Fatal READ_CLT */
#define STS_SRV_STS_NO_CLT              34		
						/* Fatal READ_CLT */
#define STS_SRV_WRITE_BAD_CLT           35		
						/* Fatal WRITE_CLT */
#define STS_SRV_WRITE_BAD_R_CLT         36		
						/* Fatal WRITE_CLT */
#define STS_SRV_WRITE_R_BAD_CLT         37		
						/* Fatal WRITE_CLT */
#define STS_SRV_WRITE_R_BAD_W_CLT       38		
						/* Fatal WRITE_CLT */
#define STS_SRV_WRITR_0_CLT             39		
						/* Fatal WRITE_CLT */
#define STS_SRV_WRT_READ_FREE_CLT       40		
						/* Fatal WRITE_CLT */
#define STS_SRV_BAD_NOD_CLT             41	
						/* Informational CONNECT_CLT */
#define STS_SRV_NO_CLT                  42	
						/* Error ---> Client READ_CLT */
#define STS_SRV_BAD_LEN_WRT_CLT         43		
						/* Fatal WRITE_CLT */
#define STS_SRV_ILL_LEV_PRO_CLT         44	
						/* Error ---> Client READ_CLT */
#define STS_SRV_ILL_SEQ_BYT_CLT         45 
						/* Error ---> Client SRVCOM_CLT */
#define STS_SRV_ILL_LG_CLT              46	
						/* Error ---> Client READ_CLT */
#define STS_SRV_NO_ACCES_APPL_CLT       47
						/* Error ---> Client SRVCMD_CLT */
#define STS_SRV_RESTART_CLT             48
						/* Fatal exit wanted */
#define STS_SRV_DLOPEN_CLT              49
                                                /* Dlopen not allowed */
#define STS_SRV_ALR_READ_AST_LNK        1		
						/* Fatal READ_LNK */
#define STS_SRV_ALR_READ_CAL_LNK        2 		
						/* Fatal READ_LNK */
#define STS_SRV_BAD_LNK_DISC_DEALNK_LNK 3 		
						/* Fatal DEALNK_LNK */
#define STS_SRV_BAD_LNK_ORG_LNK         4 	
						/* Error ---> Client SRVCOM_LNK */
#define STS_SRV_NOD_SRV_UNREACHABLE_LNK 5 	
						/* Informational SRVVMS_LNK */
#define STS_SRV_CLT_ABRT_NO_COM_LNK     6 		
						/* Fatal WRITE_LNK */
#define STS_SRV_CLT_ALR_ABORT_LNK       7 		
						/* Fatal SRVVMS_LNK */
#define STS_SRV_CNX_NOD_NZ_LNK          8 		
						/* Fatal DEALNK_LNK */
#define STS_SRV_DISC_NO_DISC_LNK        9 		
						/* Fatal DEALNK_LNK */
#define STS_SRV_FREE_READ_LNK           10		
						/* Fatal READ_LNK */
#define STS_SRV_ILL_CMD_LNK             11	
						/* Error ---> Client SRVCOM_LNK */
#define STS_SRV_ILL_FUNC_LNK            12	
						/* Error ---> Client SRVCOM_LNK */
#define STS_SRV_ILL_SEQ_CAL_4_LNK       13		
						/* Fatal READ_LNK */
#define STS_SRV_ILL_SEQ_CAL_5_LNK       14		
						/* Fatal READ_LNK */
#define STS_SRV_ILL_SEQ_CAL_LNK         15		
						/* Fatal READ_LNK */
#define STS_SRV_ILL_SEQ_CAL_NO_ASN_LNK  16		
						/* Fatal READ_LNK */
#define STS_SRV_ILL_SEQ_CAL_NO_DAS_LNK  17		
						/* Fatal READ_LNK */
#define STS_SRV_ILL_SEQ_CAL_NO_RSP_LNK  18		
						/* Fatal READ_LNK */
#define STS_SRV_LESS_16_READ_FUNC_LNK   19		
						/* Fatal READ_LNK */
#define STS_SRV_NBRLNKS_NZ_DEALNK_LNK   20		
						/* Fatal DEALNK_LNK */
#define STS_SRV_NOCOM_BUFW_LNK          21		
						/* Fatal SRVCOM_LNK */
#define STS_SRV_NODABRT_NOSTA_RD_LNK    22	
						/* Informational READ_LNK */
#define STS_SRV_NODE_ALR_ABORT_LNK      23		
						/* Fatal SRVVMS_LNK */
#define STS_SRV_NOD_ABORT_BUFW_LNK      24		
						/* Fatal SRVVMS_LNK */
#define STS_SRV_NOT_16_READ_CMD_LNK     25		
						/* Fatal READ_LNK */
#define STS_SRV_NO_DAS_UNDEA_CMD_Z_LNK  26		
						/* Fatal READ_LNK */
#define STS_SRV_NO_DAS_UNDEA_LNK        27		
						/* Fatal READ_LNK */
#define STS_SRV_NO_DISC_DEALNK_LNK      28		
						/* Fatal DEALNK_LNK */
#define STS_SRV_NO_EXPEC_READ_LNK       29		
						/* Fatal DEALNK_LNK */
#define STS_SRV_NO_GOOUT_ABORT_DEA_LNK  30		
						/* Fatal DEALNK_LNK */
#define STS_SRV_PTC_LNK                 31	
						/* Error ---> Client SRVASS_LNK */
#define STS_SRV_NO_STS_WRITE_LNK        32		
						/* Fatal WRITE_LNK */
#define STS_SRV_READ_AST_NO_DAS_LNK     33		
						/* Fatal READ_LNK */
#define STS_SRV_READ_AST_NUMCMD_LNK     34		
						/* Fatal READ_LNK */
#define STS_SRV_READ_AST_NUMCMD_Z_LNK   35		
						/* Fatal READ_LNK */
#define STS_SRV_ROO_NOD_LNK             36	
						/* Error ---> Client GETNODE_LNK */
#define STS_SRV_UNDAS_WRITE_LNK         37		
						/* Fatal WRITE_LNK */
#define STS_SRV_UNDEA_COM_NZ_LNK        38		
						/* Fatal WRITE_LNK */
#define STS_SRV_UNDEA_COM_Z_LNK         39			
						/* Fatal WRITE_LNK */
#define STS_SRV_WRT_LESS_8_LNK          40		
						/* Fatal WRITE_LNK */
#define STS_SRV_BAD_LNK_IND_NUL_LNK     41		
						/* Fatal WRITE_LNK */
#define STS_SRV_BAD_CLT_DEST_LNK        42	
					/* Error ---> Client SRVCOM_LNK */
#define STS_SRV_BAD_CLT_LNK             43	
						/* Error ---> Client SRVCOM_LNK */
#define STS_SRV_BAD_CLT_ORG_LNK         44	
						/* Error ---> Client SRVCOM_LNK */
#define STS_SRV_BAD_CNX_DEA_LNK         45		
						/* Fatal DEALNK_LNK */
#define STS_SRV_BAD_LNK_CLT_DEA_LNK     46		
						/* Fatal DEALNK_LNK */
#define STS_SRV_NO_LNK                  47	
						/* Error ---> Client SRVCOM_LNK */
#define STS_SRV_BAD_CNEC_LNK            48		
						/* Fatal SRVASS_LNK */
#define STS_SRV_LNK_FREE_TIMR_LNK       49		
						/* Fatal READ/WRITE_LNK */
#define STS_SRV_BAD_DISC_NBR_QIO_LNK    50		
						/* Fatal DEALNK_LNK */
#define STS_SRV_BAD_NBRLNKS_LNK         51	
						/* Fatal ASSLNK_LNK */
#define STS_SRV_ILL_LEV_PRO_LNK         52 
						/* Error ---> Client READ_LNK */
#define STS_SRV_ILL_SEQ_BYT_LNK         53 
						/* Error ---> Client SRVCOM_LNK */
#define STS_SRV_ILL_LG_LNK              54 
						/* Error ---> Client READ_LNK */

#define STS_SRV_BAD_DEST_RSP            1
							/* Fatal SRVCOM_RSP */
#define STS_SRV_BAD_LEN_WRT_RSP         2		
							/* Fatal WRITE_RSP */
#define STS_SRV_BAD_NUM_CMD_RSP         3		
							/* Fatal SRVCOM_RSP */
#define STS_SRV_ILL_CMD_RSP             4		
							/* Fatal SRVCOM_RSP */
#define STS_SRV_ILL_FUNC_RSP            5		
							/* Fatal SRVCOM_RSP */
#define STS_SRV_ILL_SEQ_CAL_RSP         6		
							/* Fatal SRVCOM_RSP */
#define STS_SRV_NOD_NO_ABRT_UNDAS_RSP   7 		
							/* Fatal WRITE_RSP */
#define STS_SRV_NO_BUF_LNK_RSP          8 		
							/* Fatal SRVCOM_RSP */
#define STS_SRV_NO_DAS0_WRITE_RSP       9 		
							/* Fatal WRITE_RSP */
#define STS_SRV_NO_LNK_LNK_RSP          10		
							/* Fatal SRVCOM_RSP */
#define STS_SRV_BUF_NO_STS_RSP          11		
							/* Fatal SRVCOM_RSP */
#define STS_SRV_SRVDAS_NZ_WRITE_RSP     12		
							/* Fatal WRITE_RSP */
#define STS_SRV_UNDAS_DAS_WRITE_RSP     13		
							/* Fatal WRITE_RSP */
#define STS_SRV_UNDEA_DAS_WRITE_RSP     14		
							/* Fatal WRITE_RSP */
#define STS_SRV_UNDEA_NODAS_RSP         15		
							/* Fatal WRITE_RSP */
#define STS_SRV_BAD_LNK_CNEC_RSP        16		
							/* Fatal CONFIRM_RSP */
#define STS_SRV_NO_STS_RSP              17		
							/* Fatal SRVCOM_RSP */
#define STS_SRV_ALR_ERR_RSP             18		
							/* Fatal WRITE_RSP */
#define STS_SRV_ALR_WRT_RSP             19		
							/* Fatal WRITE_RSP */
#define STS_SRV_BAD_WRITR_COM_RSP       20		
							/* Fatal WRITE_RSP */
#define STS_SRV_COM_BUFW_FREE_RSP       21		
							/* Fatal WRITE_RSP */
#define STS_SRV_WRITE_BAD_RSP           22		
							/* Fatal WRITE_RSP */
#define STS_SRV_WRITE_BAD_R_RSP         23		
							/* Fatal WRITE_RSP */
#define STS_SRV_WRITE_R_BAD_RSP         24		
							/* Fatal WRITE_RSP */
#define STS_SRV_WRITE_R_BAD_W_RSP       25		
							/* Fatal WRITE_RSP */
#define STS_SRV_WRITR_0_RSP             26		
							/* Fatal WRITE_RSP */
#define STS_SRV_WRT_READ_FREE_RSP       27		
							/* Fatal WRITE_RSP */

#define STS_SRV_ALR_READ_SRV_AST        1		
							/* Fatal READ_SRV */
#define STS_SRV_ALR_WRT_SRV             2 		
							/* Fatal WRITE_SRV */
#define STS_SRV_BAD_CNX_LIST_SRV        3 		
							/* Fatal DEALNK_SRV */
#define STS_SRV_BAD_CNX_LNK_SRV         4 		
							/* Fatal DEALNK_SRV */
#define STS_SRV_BAD_ERR_WRITR_SRV       5 		
							/* Fatal READ_SRV */
#define STS_SRV_BAD_LNK_CLT_SRV         6 		
							/* Fatal SRVCOM_SRV */
#define STS_SRV_BAD_LNK_DEST_SRV        7 		
							/* Fatal SRVCOM_SRV */
#define STS_SRV_BAD_LNK_ORG_SRV         8 		
							/* Fatal SRVCOM_SRV */
#define STS_SRV_BAD_NBRLNKS_SRV         9 		
						/* Fatal DEALNK_SRV */
						/*Error --> Client GETNODE_SRV */
#define STS_SRV_BAD_NBR_DEA_SRV         10		
							/* Fatal DEALNK_SRV */
#define STS_SRV_BAD_NODE_Z_SRV          11		
							/* Fatal SRVCOM_SRV */
#define STS_SRV_BAD_ORG_SRV             12		
							/* Fatal READ_SRV */
#define STS_SRV_BAD_READ_LEN_SRV        13		
							/* Fatal READ_SRV */
#define STS_SRV_BAD_STS_SRV             14		
							/* Fatal READ_SRV */
#define STS_SRV_CLT_SRV                 15		
							/* Fatal READ_SRV */
#define STS_SRV_CNX_LNK_NZ_SRV          16		
							/* Fatal DEALNK_SRV */
#define STS_SRV_COM_BUFW_FREE_SRV       17		
							/* Fatal WRITE_SRV */
#define STS_SRV_DISC_NO_DISC_SRV        18		
							/* Fatal DEALNK_SRV */
#define STS_SRV_DISC_NO_EXPC_SRV        19		
							/* Fatal DEALNK_SRV */
#define STS_SRV_DISC_NO_GOUT_SRV        20		
							/* Fatal DEALNK_SRV */
#define STS_SRV_DUPL_NODE_SRV           21	
							/* Informational GETNODE_SRV */
#define STS_SRV_FUNC_NUL_SRV            22		
							/* Fatal READ_SRV */
#define STS_SRV_ILL_CMD_SRV             23		
							/* Fatal SRVCOM_SRV */
#define STS_SRV_ILL_FUNC_SRV            24		
							/* Fatal SRVCOM_SRV */
#define STS_SRV_ILL_SEQ_CAL_SRV         25		
							/* Fatal SRVCOM_SRV */
#define STS_SRV_LEN_READ_SRV            26		
							/* Fatal READ_SRV */
#define STS_SRV_LNK_ALR_ABRT_SRV        27		
							/* Fatal SRVVMS_SRV */
#define STS_SRV_NOD_ALR_ABORT_SRV       28		
							/* Fatal SRVVMS_SRV */
#define STS_SRV_NOLST_DISC_SRV          29		
							/* Fatal SRVDEA_SRV */
#define STS_SRV_NO_ABRT_EXPC_SRV        30		
							/* Fatal DEALNK_SRV */
#define STS_SRV_NO_ABRT_GOUT_SRV        31		
							/* Fatal DEALNK_SRV */
#define STS_SRV_NO_ABRT_NBR_SRV         32		
							/* Fatal DEALNK_SRV */
#define STS_SRV_NO_CNX_LNK_SRV          33		
							/* Fatal SRVDEA_SRV */
#define STS_SRV_NO_SAME_NODE_SRV        34		
							/* Fatal GETNODE_SRV */
							/* Fatal SRVCOM_SRV */
#define STS_SRV_ROO_NOD_SRV             35	
					/* Error ---> Slave GETNODE_SRV */
#define STS_SRV_STS_NO_IN_SRV           36		
					/* Fatal GETNODE_SRV */
#define STS_SRV_STS_NO_SAME_NODE_SRV    37		
					/* Fatal GETNODE_SRV */
#define STS_SRV_STS_NO_SAME_TASK_SRV    38		
					/* Fatal GETNODE_SRV */
#define STS_SRV_WRITE_BAD_R_SRV         39		
					/* Fatal WRITE_SRV */
#define STS_SRV_WRITE_BAD_SRV           40		
					/* Fatal WRITE_SRV */
#define STS_SRV_WRITE_R_BAD_SRV         41		
					/* Fatal WRITE_SRV */
#define STS_SRV_WRITE_R_BAD_W_SRV       42		
					/* Fatal WRITE_SRV */
#define STS_SRV_WRITR_0_SRV             43		
					/* Fatal WRITE_SRV */
#define STS_SRV_WRT_READ_FREE_SRV       44		
					/* Fatal WRITE_SRV */
#define STS_SRV_DISC_NO_EXPEC_SRV       45 
					
#define STS_SRV_DISC_NO_GOOUT_SRV       46
					
#define STS_SRV_ILL_LEV_PRO_SRV         47	
					/* Error ---> Client READ_SRV */
#define STS_SRV_ILL_SEQ_BYT_SRV         48 
					/* Error ---> Client SRVCOM_SRV */
#define STS_SRV_ILL_LG_SRV              49	
					/* Error ---> Client READ_SRV */

					/* GET_CHAN */
#define STS_SRV_ROO_OLB                 1
					/* No room (overflow) Lnks */
#define STS_SRV_UNT_OLB                 2
					/* Unknown unit number. Fatal */

#define STS_RPC_BYT 1
#define STS_RPC_LEN 2
#define STS_RPC_DL  3
