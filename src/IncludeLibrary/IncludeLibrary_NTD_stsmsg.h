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

#define MAX_OSD_VMS_MSG 6
static
char *OSDVMSMSG[MAX_OSD_VMS_MSG]=
    {
     "INVALIDLNK_NTD Invalid link value",
     "LNKLIMIT Maximum number of links reached",
     "INVALIDPRTCL",
     "DISCONNECTED",
     "READINPROGESS",
     "LASTLINK"
    };
      
#define MAX_OSD_UNIX_MSG 1
static
char *OSDUNIXMSG[MAX_OSD_UNIX_MSG]=
    {
     "XXX"
    };

#define MAX_NTD_VMS_MSG 1
static
char *NTDVMSMSG[MAX_NTD_VMS_MSG]=
    {
     "XXX"
    };

#define MAX_NTD_UNIX_MSG 1
static
char *NTDUNIXMSG[MAX_NTD_UNIX_MSG]=
    {
     "XXX"
    };

#define MAX_ACS_MSG 9
static
char *ACSMSG[MAX_ACS_MSG]=
    {
	"SEQ  Illegal sequence call.",
	"ROO  No room (queue overflow)",
	"ARG  Incorrect arguments",
	"INT  Internal error",
	"UNR  %d. Node/server currently unreachable",
	"LEV  Incompatible command level",
	"BYT  Incompatible bytes sequence",
	"LEN  Bad respons length",
	"RSP  Bad link or command number in respons"
    };

#define MAX_SRV_MBX_MSG 5
static
char *SRVMBXMSG[MAX_SRV_MBX_MSG]=
    {
     "BAD_CONF_CNEC Confirm received for not connecting link",
     "BAD_LNK Disconnect received for not CLT/LNK link",
     "BAD_MSG_TYP Undefined mailbox message type",
     "INIT_ACP Application initialization error",
     "SRV_RESTART Server restarted by itself"
    };

#define MAX_SRV_CLT_MSG 49
static
char *SRVCLTMSG[MAX_SRV_CLT_MSG]=
    {
     "ABORT_LNK_DEA Nbr_Qio/Buffer error when aborting a link",
     "ALR_ERR Unsollicited error message already queued",
     "ALR_READ_AST Read_ast buffer/status error",
     "ALR_WRT Recursive buffer write",
     "BAD_CNX_LNK Link not found in Clients list",
     "BAD_DEST Bad remote link received",
     "BAD_ERR_WRITR Read buffer overwriting",
     "BAD_ORG Bad client link number",
     "BAD_STS Bad client link status (not Client link)",
     "BAD_WRITR_COM Bad client link status (not COMputing)",
     "CLT_ALR_ABORT Abort of an aborted client link",
     "CLT_CHN_NZ Disconnect of client link with lnks",
     "COM_BUFW_FREE Overwrite of buffer",
     "DEST Inconsistent remote link received",
     "DISC_NO_DISC Inconsistent Disconnecting link status",
     "DISC_NO_EXPEC Inconsistent expected disconnect link status",
     "DISC_NO_GOUT Inconsistent initiated disconnect link status",
     "FUNC_NUL Remote link and function inconsistency",
     "ILL_CMD Illegal command code received",
     "ILL_FUNC Illegal function code received",
     "ILL_SEQ_CAL Command/function already active",
     "ILL_SEQ_CAL_EXI Command/function for disconnecting link",
     "LEN_READ Truncated command/function received",
     "LNK_EXIST Exit function while link(s) are active",
     "NODE_NOABRT Server link and node status inconsistency",
     "NODE_NOT_ABORT Server link and node status inconsistency",
     "NO_ABR_EXP Inconsistent client status while disconnecting",
     "NO_ABR_EXPC Inconsistent client status while disconnecting",
     "NO_ABR_GOUT Inconsistent client status while disconnecting",
     "NO_ABR_NBR_CLT Expected disconnect inconsistency",
     "NO_LNK Client link inconsistency",
     "READ_ABORT Normal read status on aborted link",
     "STS_ALR_READ Recursive read",
     "STS_NO Client read on not client link",
     "WRITE_BAD Inconsistent write buffer state",
     "WRITE_BAD_R Inconsistent write/read buffer state",
     "WRITE_R_BAD Inconsistent write/read buffer state",
     "WRITE_R_BAD_W Inconsistent write/read buffer state",
     "WRITR_0 Inconsistent write/read buffer state",
     "WRT_READ_FREE Error message write inconsistency",
     "BAD_NOD Local node name is unknown at remote node",
     "NO_CLT Bad client lnk received",
     "BAD_LEN_WRT Bad respons length",
     "ILL_LEV_PRO Protocol level missmatch",
     "ILL_SEQ_BYT Bad bytes sequence",
     "ILL_LG Command length inconsistency",
     "NO_ACCES_APPL Acces to application service is blocked",
     "SRV_RESTART Server restart by client",
     "SRV_DLOPEN Dynamic loading not allowed with Server."
    };

#define MAX_SRV_LNK_MSG 54
static
char *SRVLNKMSG[MAX_SRV_LNK_MSG]=
    {
     "ALR_READ_AST Buffer read_ast state inconsistency",
     "ALR_READ_CAL Buffer read state inconsistency",
     "BAD_LNK_DISC_DEALNK Node/link status inconsistency",
     "BAD_LNK_ORG Bad server link received",
     "SRV_UNREA %d. EUCLID license server currently unreachable",
     "CLT_ABRT_NO_COM Inconsistent client link status",
     "CLT_ALR_ABORT Client link already aborted",
     "CNX_NOD_NZ Inconsistent node index of disconnecting link",
     "DISC_NO_DISC Node/slavelink disconnect inconsistent status",
     "FREE_READ Read on freed slave link",
     "ILL_CMD Illegal command received",
     "ILL_FUNC Illegal function received",
     "ILL_SEQ_CAL_4 Slave/master protocol error",
     "ILL_SEQ_CAL_5 Slave/master protocol error",
     "ILL_SEQ_CAL Slave/master protocol error",
     "ILL_SEQ_CAL_NO_ASN Slave/master protocol error",
     "ILL_SEQ_CAL_NO_DAS Slave/master protocol error",
     "ILL_SEQ_CAL_NO_RSP Slave/master protocol error",
     "LESS_16_READ_FUNC Slave/master protocol error",
     "NBRLNKS_NZ_DEALNK Slave/master disconnect inconsistency",
     "NOCOM_BUFW Status/buffer_write inconsistency",
     "NODABRT_NOSTA_RD Node/slave_link status inconsistency",
     "NODE_ALR_ABORT Node/slave_link abort status inconsistency",
     "NOD_ABORT_BUFW Write buffer state inconsistency",
     "NOT_16_READ_CMD Illegal command length",
     "NO_DAS_UNDEA_CMD_Z Slave/master protocol error",
     "NO_DAS_UNDEA Slave/master protocol error",
     "NO_DISC_DEALNK Disconnecting status inconsistency",
     "NO_EXPEC_READ Inconsistent slave link status",
     "NO_GOOUT_ABORT_DEA Inconsistent slave link status",
     "PTC Protocol not yet implemented",
     "NO_STS_WRITE Inconsistent slave link status",
     "READ_AST_NO_DAS Slave/master protocol error",
     "READ_AST_NUMCMD Slave/master protocol error",
     "READ_AST_NUMCMD_Z Slave/master protocol error",
     "ROO_NOD Number of nodes overflow",
     "UNDAS_WRITE Slave/master protocol error",
     "UNDEA_COM_NZ Slave/master protocol error",
     "UNDEA_COM_Z Slave/master protocol error",
     "WRT_LESS_8 Bad command/function length",
     "BAD_LNK_IND_NUL Bad write buffer index",
     "BAD_CLT_DEST Bad Client destination link",
     "BAD_CLT Bad Client link",
     "BAD_CLT_ORG Bad origin lnk",
     "BAD_CNX_DEA Inconsistent slave links list",
     "BAD_LNK_CLT_DEA Inconsistent slave links list",
     "NO_LNK Bad slave link number",
     "BAD_CNEC Bad connect slave link",
     "LNK_FREE_TIMR Inconsistent timer link status",
     "BAD_DISC_NBR_QIO Inconsistent slave link status",
     "BAD_NBRLNKS Bad connect slave link number",
     "ILL_LEV_PRO Protocol level missmatch",
     "ILL_SEQ_BYT Bad bytes sequence",
     "ILL_LG Command length inconsistency"
    };

#define MAX_SRV_SRV_MSG 49
static
char *SRVSRVMSG[MAX_SRV_SRV_MSG]=
    {
     "ALR_READ_AST Inconsistent master link status",
     "ALR_WRT Inconsistent master link status",
     "BAD_CNX_LIST Inconsistent master link list",
     "BAD_CNX_LNK Bad master link list head",
     "BAD_ERR_WRITR Inconsistent read buffer state",
     "BAD_LNK_CLT Inconsistent master link status",
     "BAD_LNK_DEST Inconsistent master link state",
     "BAD_LNK_ORG Inconsistent master link state",
     "BAD_NBRLNKS Bad Connect/disconnect link(s)/node state",
     "BAD_NBR_DEA Inconsistent master link disconnecting state",
     "BAD_NODE_Z Inconsistent master link state",
     "BAD_ORG Slave/master protocol error",
     "BAD_READ_LEN Inconsistent master link state",
     "BAD_STS Inconsistent master link status",
     "CLT_SRV Slave/master protocol error",
     "CNX_LNK_NZ Inconsistent disconnecting master link state",
     "COM_BUFW_FREE Inconsistent master link state",
     "DISC_NO_DISC Inconsistent disconnecting master link status",
     "DISC_NO_EXPC Inconsistent disconnecting master link status",
     "DISC_NO_GOUT Inconsistent disconnecting master link status",
     "DUPL_NODE Duplicate node/taskname connect",
     "FUNC_NUL Slave/master protocol error",
     "ILL_CMD Illegal command index",
     "ILL_FUNC Illegal function index",
     "ILL_SEQ_CAL Inconsistent master link status/buffer state",
     "LEN_READ Slave/master protocol error",
     "LNK_ALR_ABRT Inconsistent aborting master link status",
     "NOD_ALR_ABORT Inconsistent aborting master link status",
     "NOLST_DISC Inconsistent disconnecting master link(s) list",
     "NO_ABRT_EXPC Inconsistent disconnecting master link status",
     "NO_ABRT_GOUT Inconsistent disconnecting master link status",
     "NO_ABRT_NBR Inconsistent disconnecting master link status",
     "NO_CNX_LNK Inconsistent disconnecting master link(s) list",
     "NO_SAME_NODE Inconsistent master link(s) list",
     "ROO_NOD Node list overflow",
     "STS_NO_IN Inconsistent master link/node index",
     "STS_NO_SAME_NODE Inconsistent master link node name",
     "STS_NO_SAME_TASK Inconsistent master link taskname",
     "WRITE_BAD_R Inconsistent master link status/buffer state",
     "WRITE_BAD Inconsistent master link status/buffer state",
     "WRITE_R_BAD Inconsistent master link status/buffer state",
     "WRITE_R_BAD_W Inconsistent master link status/buffer state",
     "WRITR_0 Inconsistent master link status/buffer state",
     "WRT_READ_FREE Inconsistent master link status/buffer state",
     "DISC_NO_EXPEC Disconnect inconsistency",
     "DISC_NO_GOOUT Disconnect inconsistency",
     "ILL_LEV_PRO Protocol level missmatch",
     "ILL_SEQ_BYT Bad bytes sequence",
     "ILL_LG Command length inconsistency"
    };

#define MAX_SRV_RSP_MSG 27
static
char *SRVRSPMSG[MAX_SRV_RSP_MSG]=
    {
     "BAD_DEST Bad client destination lnk",
     "BAD_LEN_WRT Bad respons length",
     "BAD_NUM_CMD Slave/master protocol error",
     "ILL_CMD Bad command index",
     "ILL_FUNC Bad function index",
     "ILL_SEQ_CAL Slave/master protocol error",
     "NOD_NO_ABRT_UNDAS Slave/master protocol error",
     "NO_BUF_LNK Inconsistent link status",
     "NO_DAS0_WRITE Slave/master protocol error",
     "NO_LNK_LNK Inconsistent link status",
     "BUF_NO_STS Inconsistent link status",
     "SRVDAS_NZ_WRITE Master/slave protocol error",
     "UNDAS_DAS_WRITE Master/slave protocol error",
     "UNDEA_DAS_WRITE Master/slave protocol error",
     "UNDEA_NODAS Master/slave protocol error",
     "BAD_LNK_CNEC Inconsistent connect slave link status",
     "NO_STS Inconsistent link status",
     "ALR_ERR Unsollicited error message already queued",
     "ALR_WRT Recursive buffer write",
     "BAD_WRITR_COM Bad client link status (not COMputing)",
     "COM_BUFW_FREE Overwrite of buffer",
     "WRITE_BAD Inconsistent write buffer state",
     "WRITE_BAD_R Inconsistent write/read buffer state",
     "WRITE_R_BAD Inconsistent write/read buffer state",
     "WRITE_R_BAD_W Inconsistent write/read buffer state",
     "WRITR_0 Inconsistent write/read buffer state",
     "WRT_READ_FREE Error message write inconsistency"
    };

#define MAX_SRV_OLB_MSG 2
static
char *SRVOLBMSG[MAX_SRV_OLB_MSG]=
    {
     "ROO Master links overflow",
     "UNT_OLB Unit/Channel number inconsistency"
    };

#define MAX_TLI_LIB_MSG 1
static
char *TLILIBMSG[MAX_TLI_LIB_MSG]=
    {
     "XXX"			/* Messages fournis par ntd_message */
    };


#define MAX_OSD_WNT_MSG 1
static
char *OSDWNTMSG[MAX_TLI_LIB_MSG]=
    {
     "XXX"			/* Messages fournis par ntd_message */
    };

#define MAX_NTD_WNT_MSG 1
static
char *NTDWNTMSG[MAX_TLI_LIB_MSG]=
    {
     "XXX"			/* Messages fournis par ntd_message */
    };


#define MAX_SRV_RPC_MSG 3
static
char *SRVRPCMSG[MAX_SRV_RPC_MSG]=
    {
     "Incompatible byte sequence",
     "Buffer length mismatch",
     "Error occured while Engine loading"
    };
