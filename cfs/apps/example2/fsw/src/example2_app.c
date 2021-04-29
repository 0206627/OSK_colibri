/*******************************************************************************
** File: example2_app.c
**
** Purpose:
**   Provide functions for the EXAMPLE2 application used in cFE training.
**
*******************************************************************************/

/*
**   Include Files:
*/

#include "example2_app.h"
#include "example2_app_perfids.h"
#include "example2_app_msgids.h"


/*
** System Header files
*/
#include <string.h>

/*
** EXAMPLE2 global data
*/
EXAMPLE2_AppData_t EXAMPLE2_AppData;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* EXAMPLE2_AppMain() -- Application entry point and main process loop   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void EXAMPLE2_AppMain(void)
{
    int32 Status;
    
    /*
    ** Register the Application with Executive Services
    */
    CFE_ES_RegisterApp();

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(EXAMPLE2_APPMAIN_PERF_ID);

    /*
    ** Perform application specific initialization
    ** If the Intialization fails, set the RunStatus to CFE_ES_APP_ERROR
    ** and the App will not enter the RunLoop.
    */
    Status = EXAMPLE2_AppInit();
    if ( Status != CFE_SUCCESS )
    {
        EXAMPLE2_AppData.RunStatus = CFE_ES_APP_ERROR;
    }

    /*
    ** Application Main Loop. Call CFE_ES_RunLoop to check for changes
    ** in the Application's status. If there is a request to kill this
    ** App, it will be passed in through the RunLoop call.
    */
    while ( CFE_ES_RunLoop(&EXAMPLE2_AppData.RunStatus) == TRUE )
    {
    
        /*
        ** Performance Log Exit Stamp.
        */
        CFE_ES_PerfLogExit(EXAMPLE2_APPMAIN_PERF_ID);
   
        /*
        ** Pend on the arrival of the next Software Bus message.
        */
        Status = CFE_SB_RcvMsg(&EXAMPLE2_AppData.MsgPtr, EXAMPLE2_AppData.CmdPipe, CFE_SB_PEND_FOREVER);
        
        /*
        ** Performance Log Entry Stamp.
        */      
        CFE_ES_PerfLogEntry(EXAMPLE2_APPMAIN_PERF_ID);

        /*
        ** Check the return status from the Software Bus. If a pend with timeout is 
        ** used in the CFE_SB_RcvMsg() call then this check needs to change.
        */        
        if (Status == CFE_SUCCESS)
        {
            /*
            ** Process Software Bus message. If there are fatal errors
            ** in command processing the command can alter the global 
            ** RunStatus variable to exit the main event loop.
            */
            EXAMPLE2_AppPipe(EXAMPLE2_AppData.MsgPtr);
            
            /* 
            ** Depending on the app you may want to add calls to functions
            ** that execute each loop.
            */

        }
        else
        {
            /*
            ** This is an example2 of exiting on an error.
            ** Note that a SB read error is not always going to 
            ** result in an app quitting. 
            */
            CFE_EVS_SendEvent(EXAMPLE2_PIPE_ERR_EID, CFE_EVS_ERROR,
                               "EXAMPLE2: SB Pipe Read Error, EXAMPLE2 App Will Exit.");
            
            EXAMPLE2_AppData.RunStatus = CFE_ES_APP_ERROR;
         }
                    
    } /* end while */

    /*
    ** Performance Log Exit Stamp.
    */
    CFE_ES_PerfLogExit(EXAMPLE2_APPMAIN_PERF_ID);

    /*
    ** Exit the Application.
    */
    CFE_ES_ExitApp(EXAMPLE2_AppData.RunStatus);
    
} /* End of EXAMPLE2_AppMain() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* EXAMPLE2_AppInit() -- EXAMPLE2 initialization                               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 EXAMPLE2_AppInit(void)
{
    int32  Status;
    int32  ResetType;
    uint32 ResetSubType;

    /* 
    ** Determine Reset Type
    */
    ResetType = CFE_ES_GetResetType(&ResetSubType);
        
    /*
    ** Setup the RunStatus variable
    */
    EXAMPLE2_AppData.RunStatus = CFE_ES_APP_RUN;
    
    /*
    ** Initialize app command execution counters.
    */
    EXAMPLE2_AppData.CmdCounter = 0;
    EXAMPLE2_AppData.ErrCounter = 0;

    /*
    ** Initialize app configuration data.
    */
    strcpy(EXAMPLE2_AppData.PipeName, "EXAMPLE2_CMD_PIPE");
    EXAMPLE2_AppData.PipeDepth = EXAMPLE2_PIPE_DEPTH;

    EXAMPLE2_AppData.LimitHK   = EXAMPLE2_LIMIT_HK;
    EXAMPLE2_AppData.LimitCmd  = EXAMPLE2_LIMIT_CMD;

    /*
    ** Initialize event filter table for envents we want to filter.
    */
    
    EXAMPLE2_AppData.EventFilters[0].EventID = EXAMPLE2_RESET_INF_EID;
    EXAMPLE2_AppData.EventFilters[0].Mask    = CFE_EVS_NO_FILTER;
    
    EXAMPLE2_AppData.EventFilters[1].EventID = EXAMPLE2_CC1_ERR_EID;
    EXAMPLE2_AppData.EventFilters[1].Mask    = CFE_EVS_EVERY_OTHER_TWO;
    
    EXAMPLE2_AppData.EventFilters[2].EventID = EXAMPLE2_LEN_ERR_EID;
    EXAMPLE2_AppData.EventFilters[2].Mask    = CFE_EVS_FIRST_8_STOP;

    // EXAMPLE2_AppData.EventFilters[3].EventID = EXAMPLE2_NEW_CMD_INF_EID;
    // EXAMPLE2_AppData.EventFilters[3].Mask    = CFE_EVS_EVERY_FOURTH_ONE;

    /*
    ** Register event filter table.
    */
    Status = CFE_EVS_Register(EXAMPLE2_AppData.EventFilters, 4,
                               CFE_EVS_BINARY_FILTER);
    
    if ( Status != CFE_SUCCESS )
    {
       CFE_ES_WriteToSysLog("EXAMPLE2 App: Error Registering Events, RC = 0x%08X\n", Status);
       return ( Status );
    }
             
    /*
    ** Initialize housekeeping packet (clear user data area).
    */
    CFE_SB_InitMsg(&EXAMPLE2_AppData.HkPacket, EXAMPLE2_HK_TLM_MID, sizeof(EXAMPLE2_HkPacket_t), TRUE);
   
    /*
    ** Create Software Bus message pipe.
    */
    Status = CFE_SB_CreatePipe(&EXAMPLE2_AppData.CmdPipe, EXAMPLE2_AppData.PipeDepth, EXAMPLE2_AppData.PipeName);
    if ( Status != CFE_SUCCESS )
    {
       /*
       ** Could use an event at this point
       */
       CFE_ES_WriteToSysLog("EXAMPLE2 App: Error Creating SB Pipe, RC = 0x%08X\n", Status);
       return ( Status );
    }

    /*
    ** Subscribe to Housekeeping request commands
    */
    Status = CFE_SB_Subscribe(EXAMPLE2_SEND_HK_MID,EXAMPLE2_AppData.CmdPipe);
    if ( Status != CFE_SUCCESS )
    {
       CFE_ES_WriteToSysLog("EXAMPLE2 App: Error Subscribing to HK Request, RC = 0x%08X\n", Status);
       return ( Status );
    }

    /*
    ** Subscribe to EXAMPLE2 ground command packets
    */
    Status = CFE_SB_Subscribe(EXAMPLE2_CMD_MID,EXAMPLE2_AppData.CmdPipe);
    if ( Status != CFE_SUCCESS )
    {
       CFE_ES_WriteToSysLog("EXAMPLE2 App: Error Subscribing to EXAMPLE2 Command, RC = 0x%08X\n", Status);
       return ( Status );
    }

    Status = CFE_SB_Subscribe(EXAMPLE2_APPCOMM_MID,EXAMPLE2_AppData.CmdPipe);
    if ( Status != CFE_SUCCESS )
    {
       CFE_ES_WriteToSysLog("EXAMPLE2 App: Error Subscribing to APPCOMM EXAMPLE2 Command, RC = 0x%08X\n", Status);
       return ( Status );
    }
                 
    /*
    ** Application startup event message.
    */
    CFE_EVS_SendEvent(EXAMPLE2_INIT_INF_EID,CFE_EVS_INFORMATION, "EXAMPLE2: Application Initialized");
                         
    return(CFE_SUCCESS);

} /* End of EXAMPLE2_AppInit() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* EXAMPLE2_AppPipe() -- Process command pipe message                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void EXAMPLE2_AppPipe(CFE_SB_MsgPtr_t msg)
{
    CFE_SB_MsgId_t MessageID;
    uint16 CommandCode;

    MessageID = CFE_SB_GetMsgId(msg);
    switch (MessageID)
    {
        /*
        ** Housekeeping telemetry request.
        */
        case EXAMPLE2_SEND_HK_MID:
            EXAMPLE2_HousekeepingCmd(msg);
            break;

        /*
        ** EXAMPLE2 ground commands.
        */
        case EXAMPLE2_CMD_MID:

            CommandCode = CFE_SB_GetCmdCode(msg);
            switch (CommandCode)
            {
                case EXAMPLE2_NOOP_CC:
                    EXAMPLE2_NoopCmd(msg);
                    break;

                case EXAMPLE2_RESET_CC:
                    EXAMPLE2_ResetCmd(msg);
                    break;
                    
                case EXAMPLE2_NEW_CMD_CC:
                    EXAMPLE2_NewCmd(msg);
                    break;

                default:
                    CFE_EVS_SendEvent(EXAMPLE2_CC1_ERR_EID, CFE_EVS_ERROR,
                     "Invalid ground command code: ID = 0x%X, CC = %d",
                                      MessageID, CommandCode);
                    break;
            }
            break;
        case EXAMPLE2_APPCOMM_MID:
             printf("Example2");
             CFE_EVS_SendEvent(EXAMPLE2_NEW_CMD_INF_EID, CFE_EVS_INFORMATION, "Funciona:)");
             break;

        default:

            CFE_EVS_SendEvent(EXAMPLE2_MID_ERR_EID, CFE_EVS_ERROR,
                             "Invalid command pipe message ID: 0x%X",
                              MessageID);
            break;
    }

    return;

} /* End of EXAMPLE2_AppPipe() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* EXAMPLE2_HousekeepingCmd() -- On-board command (HK request)           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void EXAMPLE2_HousekeepingCmd(CFE_SB_MsgPtr_t msg)
{
    uint16 ExpectedLength = sizeof(EXAMPLE2_NoArgsCmd_t);
    uint16 i;

    /*
    ** Verify command packet length
    */
    if (EXAMPLE2_VerifyCmdLength(msg, ExpectedLength))
    {
        /*
        ** Get command execution counters
        */
        EXAMPLE2_AppData.HkPacket.CmdCounter = EXAMPLE2_AppData.CmdCounter;
        EXAMPLE2_AppData.HkPacket.ErrCounter = EXAMPLE2_AppData.ErrCounter;

        /*
        ** Send housekeeping telemetry packet
        */
        CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &EXAMPLE2_AppData.HkPacket);
        CFE_SB_SendMsg((CFE_SB_Msg_t *) &EXAMPLE2_AppData.HkPacket);

        /*
        ** This command does not affect the command execution counter
        */
        
    } /* end if */

    return;

} /* End of EXAMPLE2_HousekeepingCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* EXAMPLE2_NoopCmd() -- EXAMPLE2 ground command (NO-OP)                       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void EXAMPLE2_NoopCmd(CFE_SB_MsgPtr_t msg)
{
    uint16 ExpectedLength = sizeof(EXAMPLE2_NoArgsCmd_t);

    /*
    ** Verify command packet length...
    */
    if (EXAMPLE2_VerifyCmdLength(msg, ExpectedLength))
    {
        EXAMPLE2_AppData.CmdCounter+=2;

        CFE_EVS_SendEvent(EXAMPLE2_NOOP_INF_EID, CFE_EVS_INFORMATION,
                         "EXAMPLE2 Version 1.0.0: No-op command");
    }

    return;

} /* End of EXAMPLE2_NoopCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* EXAMPLE2_ResetCmd() -- EXAMPLE2 ground command (reset counters)             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void EXAMPLE2_ResetCmd(CFE_SB_MsgPtr_t msg)
{
    uint16 ExpectedLength = sizeof(EXAMPLE2_NoArgsCmd_t);

    /*
    ** Verify command packet length...
    */
    if (EXAMPLE2_VerifyCmdLength(msg, ExpectedLength))
    {
        EXAMPLE2_AppData.CmdCounter = 0;
        EXAMPLE2_AppData.ErrCounter = 0;

        CFE_EVS_SendEvent(EXAMPLE2_RESET_INF_EID, CFE_EVS_INFORMATION,
                         "EXAMPLE2: Reset Counters command");
    }

    return;

} /* End of EXAMPLE2_ResetCmd() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* EXAMPLE2_NewtCmd() -- EXAMPLE2 ground command (Send a string)   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void    EXAMPLE2_NewCmd(CFE_SB_MsgPtr_t msg){
    char return_val[30];

    EXAMPLE2_ArgsCmd_t* data = (EXAMPLE2_ArgsCmd_t*)CFE_SB_GetUserData(msg);
    
    switch(data->num){
        case 1:
            strcpy(return_val,"Hola");
            break;
        case 2:
            strcpy(return_val,"Como estas?");
            break;
        case 3:
            strcpy(return_val,"Bien, y tu?");
            break;
        case 4:
            strcpy(return_val,"Pues tambien...");
            break;
        case 5:
            strcpy(return_val,"Ah bueno te me cuidas :)");
            break;
        default:
            strcpy(return_val,"Camara");
    }
    CFE_EVS_SendEvent(EXAMPLE2_NEW_CMD_INF_EID, CFE_EVS_INFORMATION, return_val);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* EXAMPLE2_VerifyCmdLength() -- Verify command packet length            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean EXAMPLE2_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength)
{
    boolean result = TRUE;
    uint16 ActualLength = CFE_SB_GetTotalMsgLength(msg);

    /*
    ** Verify the command packet length...
    */
    if (ExpectedLength != ActualLength)
    {
        CFE_SB_MsgId_t MessageID = CFE_SB_GetMsgId(msg);
        uint16 CommandCode = CFE_SB_GetCmdCode(msg);

        CFE_EVS_SendEvent(EXAMPLE2_LEN_ERR_EID, CFE_EVS_ERROR,
           "EXAMPLE2: Invalid cmd pkt: ID = 0x%X,  CC = %d, Len = %d",
                          MessageID, CommandCode, ActualLength);
        result = FALSE;
        EXAMPLE2_AppData.ErrCounter++;
    }

    return(result);

} /* End of EXAMPLE2_VerifyCmdLength() */

/************************/
/*  End of File Comment */
/************************/
