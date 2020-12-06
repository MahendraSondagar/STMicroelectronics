#include "TargetFeatures.h"
#include "main.h"
#include "cli_commands.h"
#if SENSING1_USE_DATALOG
#include "ff.h"
#include "DataLog_Manager.h"
#endif /* SENSING1_USE_DATALOG */

extern osSemaphoreId semRun;

extern volatile uint32_t MultiNN;

#ifdef STM32_SENSORTILEBOX
extern volatile int PowerButtonPressed;
#endif /* STM32_SENSORTILEBOX */

extern uint8_t bdaddr[6];
extern uint8_t NodeName[8];
extern uint8_t set_connectable;

extern char DefaultDataFileName[12];

static BaseType_t prvInfoCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t prvUidCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t prvNameCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t prvDateCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t prvBdaddrCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

static BaseType_t prvHarCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t prvAscCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t prvMultiCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t prvGetAllAIAlgoCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t prvGetAIAlgoCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t prvSetAIAlgoCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

#if SENSING1_USE_DATALOG
static BaseType_t prvSdnameCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t prvDatalogCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t prvLSCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t prvCATCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t prvRMCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t prvFORMATCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
#endif /* SENSING1_USE_DATALOG */

#if SENSING1_USE_USB_MSC
static BaseType_t prvUsbCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
#endif /* SENSING1_USE_USB_MSC */

static BaseType_t prvResetCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

#ifdef STM32_SENSORTILEBOX
static BaseType_t prvOffCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
#endif /* STM32_SENSORTILEBOX */

static const CLI_Command_Definition_t xNameCommand =
{
    "name", /* The command string to type */
    "\r\nname [name]:\r\n Show or set node name (7 Chars Max).\r\n"\
                        " When called with no arguments, display node name.\r\n",
    prvNameCommand, /* The function to run */
    -1 /* The user can enter any number of commands. */
};

static const CLI_Command_Definition_t xInfoCommand =
{
    "info", /* The command string to type */
    "\r\ninfo:\r\n Show firmware details and version.\r\n",
    prvInfoCommand, /* The function to run */
    0 /* No parameters are expected. */
};

static const CLI_Command_Definition_t xUidCommand =
{
    "uid", /* The command string to type */
    "\r\nuid:\r\n Show STM32 UID.\r\n",
    prvUidCommand, /* The function to run */
    0 /* No parameters are expected. */
};

static const CLI_Command_Definition_t xDateCommand =
{
    "date", /* The command string to type. */
    "\r\ndate [wd/dd/mm/yy hh:mm:ss]:\r\n Show or set the device date and time.\r\n"\
                                        " When called with no arguments, display date and time.\r\n",
    prvDateCommand, /* The function to run. */
    -1 /* The user can enter any number of commands. */
};

static const CLI_Command_Definition_t xBdaddrCommand =
{
    "bdaddr", /* The command string to type */
    "\r\nbdaddr:\r\n Show Bluetooth Device Address.\r\n",
    prvBdaddrCommand, /* The function to run */
    0 /* No parameters are expected. */
};

static const CLI_Command_Definition_t xHarCommand =
{
    "har", /* The command string to type */
    "\r\nhar [start | stop] <name>:\r\n Start or stop Human Activity Recognition (HAR).\r\n",
    prvHarCommand, /* The function to run */
    2 /* Two parameters are expected. Valid name values are "gmp", "ign" and "ign_wsdm". */
};

static const CLI_Command_Definition_t xAscCommand =
{
    "asc", /* The command string to type */
    "\r\nasc [start | stop]:\r\n Start or stop Acoustic Scene Classification (ASC).\r\n",
    prvAscCommand, /* The function to run */
    1 /* One parameter is expected. */
};

static const CLI_Command_Definition_t xMultiCommand =
{
    "multi", /* The command string to type */
    "\r\nmulti [start | stop]:\r\n Start or stop Multi NN (ASC & HAR).\r\n",
    prvMultiCommand, /* The function to run */
    1 /* One parameter is expected. */
};

static const CLI_Command_Definition_t xGetAllAIAlgoCommand =
{
    "getAllAIAlgo", /* The command string to type */
    "\r\ngetAllAIAlgo :\r\n Get all Available AI Algorithms\r\n",
    prvGetAllAIAlgoCommand, /* The function to run */
    0 /* No parameters are expected. */
};

static const CLI_Command_Definition_t xSetAIAlgoCommand =
{
    "setAIAlgo", /* The command string to type */
    "\r\nsetAIAlgo :\r\n Set AI Algorithm\r\n",
    prvSetAIAlgoCommand, /* The function to run */
    1 /* One parameter is expected. */
};

static const CLI_Command_Definition_t xGetAIAlgoCommand =
{
    "getAIAlgo", /* The command string to type */
    "\r\ngetAllAIAlgo :\r\n Get current AI Algorithm\r\n",
    prvGetAIAlgoCommand, /* The function to run */
    0 /* No parameters are expected. */
};

#if SENSING1_USE_DATALOG
static const CLI_Command_Definition_t xSdnameCommand =
{
    "sdname", /* The command string to type */
    "\r\nsdname [name]:\r\n Show or set datalog filename prefix.\r\n"\
                          " When called with no arguments, display sdname.\r\n",
    prvSdnameCommand, /* The function to run */
    -1 /* The user can enter any number of commands. */
};

static const CLI_Command_Definition_t xDatalogCommand =
{
    "datalog", /* The command string to type */
    "\r\ndatalog [start | stop] <type>:\r\n Start or stop AI Datalog.\r\n"\
                                           " Valid type values are \"audio\" and \"mems\".\r\n",
    prvDatalogCommand, /* The function to run */
    2 /* Two parameters are expected. */
};

/* Structure that defines the ls command line command, which lists all the
files in the current directory. */
static const CLI_Command_Definition_t xLSCommand =
{
    // TODO: Optional paramenter for directory path
    "ls", /* The command string to type. */
    "\r\nls:\r\n List directory contents.\r\n",
    prvLSCommand, /* The function to run. */
    0 /* No parameters are expected. */
};

/* Structure that defines the cat command line command, which lists all the
files in the current directory. */
static const CLI_Command_Definition_t xCATCommand =
{
    "cat", /* The command string to type. */
    "\r\ncat <filename>:\r\n Read file to the standard output.\r\n",
    prvCATCommand, /* The function to run. */
    1 /* One parameter is expected. */
};

static const CLI_Command_Definition_t xRMCommand =
{
    "rm",
    "\r\nrm <filename>:\r\n Delete file from the volume.\r\n",
    prvRMCommand,
    1 /* One parameter is expected. */
};

/* Structure that defines the FORMAT command line command, which re-formats the
file system. */
static const CLI_Command_Definition_t xFORMATCommand =
{
    "format", /* The command string to type. */
    "\r\nformat:\r\n Re-format volume. ALL FILES WILL BE DELETED!\r\n",
    prvFORMATCommand, /* The function to run. */
    0 /* No parameters are expected. */
};
#endif /* SENSING1_USE_DATALOG */

#if SENSING1_USE_USB_MSC
static const CLI_Command_Definition_t xUsbCommand =
{
    "usb", /* The command string to type */
    "\r\nusb [start | stop]:\r\n Start or stop USB Mass Storage Device mode.\r\n",
    prvUsbCommand, /* The function to run */
    1 /* One parameter is expected. */
};
#endif /* SENSING1_USE_USB_MSC */

static const CLI_Command_Definition_t xResetCommand =
{
    "reset",
    "\r\nreset:\r\n MCU System reset.\r\n",
    prvResetCommand,
    0 /* No parameters are expected. */
};

#ifdef STM32_SENSORTILEBOX
static const CLI_Command_Definition_t xOffCommand =
{
    "Off",
    "\r\nOff:\r\n Power off the device.\r\n",
    prvOffCommand,
    0 /* No parameters are expected. */
};
#endif /* STM32_SENSORTILEBOX */

void RegisterCLICommands(void)
{
    /* Register all the command line commands defined immediately above. */
    FreeRTOS_CLIRegisterCommand(&xInfoCommand);
    FreeRTOS_CLIRegisterCommand(&xUidCommand);
    FreeRTOS_CLIRegisterCommand(&xNameCommand);
    FreeRTOS_CLIRegisterCommand(&xDateCommand);
    FreeRTOS_CLIRegisterCommand(&xBdaddrCommand);

    FreeRTOS_CLIRegisterCommand(&xAscCommand);
    FreeRTOS_CLIRegisterCommand(&xHarCommand);
    FreeRTOS_CLIRegisterCommand(&xMultiCommand);
    FreeRTOS_CLIRegisterCommand(&xGetAllAIAlgoCommand);
    FreeRTOS_CLIRegisterCommand(&xSetAIAlgoCommand);
    FreeRTOS_CLIRegisterCommand(&xGetAIAlgoCommand);

#if SENSING1_USE_DATALOG
    FreeRTOS_CLIRegisterCommand(&xSdnameCommand);
    FreeRTOS_CLIRegisterCommand(&xDatalogCommand);
    FreeRTOS_CLIRegisterCommand(&xLSCommand);
    FreeRTOS_CLIRegisterCommand(&xCATCommand);
    FreeRTOS_CLIRegisterCommand(&xRMCommand);
    FreeRTOS_CLIRegisterCommand(&xFORMATCommand);
#endif /* SENSING1_USE_DATALOG */

#if SENSING1_USE_USB_MSC
    FreeRTOS_CLIRegisterCommand(&xUsbCommand);
#endif /* SENSING1_USE_USB_AUDIO */

    FreeRTOS_CLIRegisterCommand(&xResetCommand);

#ifdef STM32_SENSORTILEBOX
    FreeRTOS_CLIRegisterCommand(&xOffCommand);
#endif /* STM32_SENSORTILEBOX */
}


static BaseType_t prvInfoCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    // TODO: Create shared function?

    (void) xWriteBufferLen;
    static BaseType_t xReturn = 0;

    if (xReturn == 0)
    {
        sprintf(pcWriteBuffer,"\r\nSTMicroelectronics %s:\r\n"
            "\tVersion %c.%c.%c\r\n"
    #if defined(STM32_SENSORTILE)
            "\tSTM32476RG-SensorTile board"
    #elif defined(USE_STM32L4XX_NUCLEO)
            "\tSTM32L476RG-Nucleo board"
    #elif defined(USE_STM32L475E_IOT01)
            "\tSTM32L475R-IoT01A1 board"
    #elif defined(STM32_SENSORTILEBOX)
            "\tSTM32L4R9ZI-SensorTile.box board"
    #else
    #error "Write Platform name"
    #endif /* STM32_SENSORTILE */
            "\r\n",
            SENSING1_PACKAGENAME,
            SENSING1_VERSION_MAJOR,SENSING1_VERSION_MINOR,SENSING1_VERSION_PATCH);
        xReturn = 1; /* There is more to output */
    }
    else
    {
        sprintf(pcWriteBuffer,
            "\t(HAL %ld.%ld.%ld_%ld)\r\n\tCompiled %s %s"
    #if defined (__IAR_SYSTEMS_ICC__)
            " (IAR)\r\n",
    #elif defined (__CC_ARM)
            " (KEIL)\r\n",
    #elif defined (__GNUC__)
            " (openstm32)\r\n",
    #endif
            HAL_GetHalVersion() >>24,
            (HAL_GetHalVersion() >>16)&0xFF,
            (HAL_GetHalVersion() >> 8)&0xFF,
            HAL_GetHalVersion()      &0xFF,
            __DATE__,__TIME__);
        xReturn = 0; /* done */
    }

    return xReturn;
}

static BaseType_t prvUidCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    /* Write back the STM32 UID */
    uint8_t *uid = (uint8_t *)UID_BASE;
    uint32_t mcu_dev_id = HAL_GetDEVID();
    sprintf(pcWriteBuffer, "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X_%.3lX\n",
            uid[3], uid[2], uid[1], uid[0],
            uid[7], uid[6], uid[5], uid[4],
            uid[11], uid[10], uid[9], uid[8],
            mcu_dev_id);

    return 0;
}

static BaseType_t prvNameCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    const char *pcParameter;
    BaseType_t xParameterStringLength;

    (void) pcCommandString;
    (void) xWriteBufferLen;
    configASSERT(pcWriteBuffer);

    /* Obtain the parameter string. */
    pcParameter = FreeRTOS_CLIGetParameter(
        pcCommandString,        /* The command string itself. */
        1,                      /* Return the first parameter. */
        &xParameterStringLength /* Store the parameter string length. */
    );

    if (pcParameter != NULL)
    {
        /* Set node name (up to the first 7 chars) */
        strncpy((char *)NodeName + 1, pcParameter, 7);

        MDM_SaveGMD(GMD_NODE_NAME, (void *)&NodeName);
        NecessityToSaveMetaDataManager = 1;
        set_connectable = TRUE;

        /* Signal ProcessThread to update MetaDataManager */
        if (semRun) {
            osSemaphoreRelease(semRun);
        }

        /* No output */
        sprintf(pcWriteBuffer, "\r\n");
    }
    else
    {
        /* Display node name */
        sprintf(pcWriteBuffer, "%s\r\n", NodeName+1);
    }

    return 0;
}

static BaseType_t prvDateCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    const char *pcParameter1;
    BaseType_t xParameterStringLength1;
    const char *pcParameter2;
    BaseType_t xParameterStringLength2;
    RTC_DateTypeDef date;
    RTC_TimeTypeDef time;
    int32_t date_valid = -1;
    int32_t time_valid = -1;

    (void) pcCommandString;
    (void) xWriteBufferLen;
    configASSERT(pcWriteBuffer);

    /* Obtain the parameter string. */
    pcParameter1 = FreeRTOS_CLIGetParameter(
        pcCommandString,         /* The command string itself. */
        1,                       /* Return the first parameter. */
        &xParameterStringLength1 /* Store the parameter string length. */
    );

    /* Obtain the parameter string. */
    pcParameter2 = FreeRTOS_CLIGetParameter(
        pcCommandString,         /* The command string itself. */
        2,                       /* Return the first parameter. */
        &xParameterStringLength2 /* Store the parameter string length. */
    );

    if (pcParameter1 != NULL)
    {
        /* Set date and time ------------- */

        /* check format */
        /* wd/dd/mm/yy hh:mm:ss */
        if (pcParameter2 == NULL)
        {
            sprintf(pcWriteBuffer, "missing time information\r\n");
        }
        else if ((xParameterStringLength1 != 11) || (xParameterStringLength2 != 8))
        {
            sprintf(pcWriteBuffer, "Invalid date \'%s\'\r\n", pcParameter1);
        }
        else
        {
            date.WeekDay = pcParameter1[1] - 48;
            date.Date    =  ((pcParameter1[3] - 48) * 16) + (pcParameter1[4] - 48);
            date.Month   = ((pcParameter1[6] - 48) * 16) + (pcParameter1[7] - 48);
            date.Year    =  ((pcParameter1[9] - 48) * 16) + (pcParameter1[10] - 48);

            time.Hours=   ((pcParameter2[0]  - 48) * 16) + (pcParameter2[1]  - 48);
            time.Minutes= ((pcParameter2[3] - 48) * 16) + (pcParameter2[4] - 48);
            time.Seconds= ((pcParameter2[6] - 48) * 16) + (pcParameter2[7] - 48);

            date_valid =  (((date.WeekDay > 0x00) && (date.WeekDay < 0x08)) &&
                           ((date.Date > 0x00) && (date.Date < 0x32)) &&
                           ((date.Month > 0x00) && (date.Month < 0x13)) &&
                            (date.Year < 0x99));

            time_valid = ((time.Hours   < 0x24) &&
                          (time.Minutes < 0x60) &&
                          (time.Seconds < 0x60));

            if (date_valid && time_valid)
            {
                /* Configure RTC Data */
                RTC_DateConfig(date.WeekDay, date.Date, date.Month, date.Year);
                RTC_TimeConfig(time.Hours, time.Minutes, time.Seconds);

                sprintf(pcWriteBuffer, "\r\n");
            }
            else
            {
                sprintf(pcWriteBuffer, "Invalid date \'%s\'\r\n", pcParameter1);
            }
        }
    }
    else
    {
        /* Show date and time ------------ */
        RTC_GetCurrentDateTime();

        sprintf(pcWriteBuffer,
            "RTC Date & time: %02d/%02d/%02d/%02d %02d:%02d:%02d\r\n",
            CurrentDate.WeekDay,
            CurrentDate.Date,
            CurrentDate.Month,
            CurrentDate.Year,
            CurrentTime.Hours,
            CurrentTime.Minutes,
            CurrentTime.Seconds);
    }

    return 0;
}

static BaseType_t prvBdaddrCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    /* Show the Bluetooth Device Address */
    sprintf(pcWriteBuffer, "%02x:%02x:%02x:%02x:%02x:%02x\r\n",
            bdaddr[5],bdaddr[4],bdaddr[3],bdaddr[2],bdaddr[1],bdaddr[0]);

    return 0;
}

static BaseType_t prvHarCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    const char *pcParameter;
    BaseType_t lParameterStringLength;

    msgType_t process_type = ACTIVITY_GMP;
    uint32_t process_period = INERTIAL_ACQ_ACTIVITY_GMP_MS;

    /* Clear write buffer if there is nothing to return */
    sprintf(pcWriteBuffer, "\r\n");

    pcParameter = FreeRTOS_CLIGetParameter(
        pcCommandString,        /* The command string itself. */
        2,                      /* Return the second parameter. */
        &lParameterStringLength /* Store the parameter string length. */
    );

    /* Sanity check something was returned. */
    configASSERT(pcParameter);

    if (strncmp(pcParameter, "gmp", strlen("gmp")) == 0)
    {
        process_type = ACTIVITY_GMP;
        process_period = INERTIAL_ACQ_ACTIVITY_GMP_MS;
    }
    else if (strncmp(pcParameter, "ign_wsdm", strlen("ign_wsdm")) == 0)
    {
        process_type = ACTIVITY_IGN_WSDM;
        process_period = INERTIAL_ACQ_ACTIVITY_IGN_WSDM_MS;
    }
    else if (strncmp(pcParameter, "ign", strlen("ign")) == 0)
    {
        process_type = ACTIVITY_IGN;
        process_period = INERTIAL_ACQ_ACTIVITY_IGN_MS;
    }
    else
    {
        sprintf(pcWriteBuffer, "Valid parameters are \"gmp\", \"ign\" and \"ign_wsdm\".\r\n");
    }

    /* Obtain the parameter string. */
    pcParameter = FreeRTOS_CLIGetParameter(
        pcCommandString,        /* The command string itself. */
        1,                      /* Return the first parameter. */
        &lParameterStringLength /* Store the parameter string length. */
    );

    if (strncmp(pcParameter, "start", strlen("start")) == 0)
    {
        W2ST_ON_CONNECTION(W2ST_CONNECT_AR);
        startProc(process_type, process_period);
    }
    else if (strncmp(pcParameter, "stop", strlen("stop")) == 0)
    {
        W2ST_OFF_CONNECTION(W2ST_CONNECT_AR);
        stopProc(process_type);
    }
    else
    {
        sprintf(pcWriteBuffer, "Valid parameters are 'start' and 'stop'.\r\n");
    }

    if (semRun) {
      osSemaphoreRelease(semRun);
    }

    return 0;
}


static BaseType_t prvAscCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    const char *pcParameter;
    BaseType_t lParameterStringLength;

    /* Clear write buffer if there is nothing to return */
    sprintf(pcWriteBuffer, "\r\n");

    /* Obtain the parameter string. */
    pcParameter = FreeRTOS_CLIGetParameter(
        pcCommandString,        /* The command string itself. */
        1,                      /* Return the first parameter. */
        &lParameterStringLength /* Store the parameter string length. */
    );

    if (strncmp(pcParameter, "start", strlen("start")) == 0)
    {
        W2ST_ON_CONNECTION(W2ST_CONNECT_ASC_EVENT);
        startProc(AUDIO_SC, 0 /* Not Used for Audio */);
    }
    else if (strncmp(pcParameter, "stop", strlen("stop")) == 0)
    {
        stopProc(AUDIO_SC);
        W2ST_OFF_CONNECTION(W2ST_CONNECT_ASC_EVENT);
    }
    else
    {
        sprintf(pcWriteBuffer, "Valid parameters are 'start' and 'stop'.\r\n");
    }

    if (semRun) {
      osSemaphoreRelease(semRun);
    }

    return 0;
}

static BaseType_t prvMultiCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    const char *pcParameter;
    BaseType_t lParameterStringLength;

    /* Clear write buffer if there is nothing to return */
    sprintf(pcWriteBuffer, "\r\n");

    /* Obtain the parameter string. */
    pcParameter = FreeRTOS_CLIGetParameter(
        pcCommandString,        /* The command string itself. */
        1,                      /* Return the first parameter. */
        &lParameterStringLength /* Store the parameter string length. */
    );

    if (strncmp(pcParameter, "start", strlen("start")) == 0)
    {
        MultiNN = 1;
        W2ST_ON_CONNECTION(W2ST_CONNECT_ASC_EVENT);
        startProc(AUDIO_SC, 0 /* Not Used for Audio */);
        W2ST_ON_CONNECTION(W2ST_CONNECT_AR);
        startProc(ACTIVITY_IGN, INERTIAL_ACQ_ACTIVITY_IGN_MS);
    }
    else if (strncmp(pcParameter, "stop", strlen("stop")) == 0)
    {
        stopProc(AUDIO_SC);
        W2ST_OFF_CONNECTION(W2ST_CONNECT_ASC_EVENT);
        stopProc(ACTIVITY_IGN_WSDM);
        HarAlgo = HAR_ALGO_IDX_NONE;
        W2ST_OFF_CONNECTION(W2ST_CONNECT_AR);
        MultiNN = 0;
    }
    else
    {
        sprintf(pcWriteBuffer, "Valid parameters are 'start' and 'stop'.\r\n");
    }

    if (semRun) {
      osSemaphoreRelease(semRun);
    }

    return 0;
}

static BaseType_t prvGetAllAIAlgoCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    /* return all available AI algorithms */
    sprintf(pcWriteBuffer, "1-ASC+GMP ,2-ASC+IGN,3-ASC+IGN_WSDM\n");

    return 0;
}

static BaseType_t prvSetAIAlgoCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{

  const char *pcParameter;
  BaseType_t lParameterStringLength;
  
  
  /* Clear write buffer if there is nothing to return */
  sprintf(pcWriteBuffer, "\r\n");

  /* Obtain the parameter string. */
  pcParameter = FreeRTOS_CLIGetParameter(
      pcCommandString,        /* The command string itself. */
      1,                      /* Return the first parameter. */
      &lParameterStringLength /* Store the parameter string length. */
  );

  if (strncmp(pcParameter, "1", 1) == 0)
    HarAlgo = HAR_GMP_IDX ;
  else if (strncmp(pcParameter, "2", 1) == 0)
    HarAlgo = HAR_IGN_IDX ;
  else if (strncmp(pcParameter, "3", 1) == 0)
    HarAlgo = HAR_IGN_WSDM_IDX ;
  else 
    HarAlgo = HAR_ALGO_IDX_NONE ;
    

  return 0;
}

static BaseType_t prvGetAIAlgoCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
  if (HAR_GMP_IDX==HarAlgo)
    sprintf(pcWriteBuffer, "1\r\n");
  else if (HAR_IGN_IDX==HarAlgo)
    sprintf(pcWriteBuffer, "2\r\n");
  else if (HAR_IGN_WSDM_IDX==HarAlgo)
    sprintf(pcWriteBuffer, "3\r\n");
  else
    sprintf(pcWriteBuffer, "0\r\n");
  return 0;
}

#if SENSING1_USE_DATALOG

static BaseType_t prvDatalogCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    const char *pcParameter1;
    const char *pcParameter2;
    BaseType_t lParameterStringLength;

    sprintf(pcWriteBuffer, "\r\n");

    pcParameter1 = FreeRTOS_CLIGetParameter(
        pcCommandString,        /* The command string itself. */
        1,                      /* Return the second parameter. */
        &lParameterStringLength /* Store the parameter string length. */
    );
    pcParameter2 = FreeRTOS_CLIGetParameter(
        pcCommandString,        /* The command string itself. */
        2,                      /* Return the second parameter. */
        &lParameterStringLength /* Store the parameter string length. */
    );

    /* Sanity check something was returned. */
    configASSERT(pcParameter1);
    configASSERT(pcParameter2);

    // TODO: Audio datalog needs to be fixed!
    // TODO: Start datalog in Process thread
    // TODO: Select sensors for mems datalog

    if (strncmp(pcParameter1, "start", strlen("start")) == 0)
    {
        if (strncmp(pcParameter2, "audio", strlen("audio")) == 0)
        {
            SD_CardLoggingAudioStart();
            sprintf(pcWriteBuffer, "Audio datalog started\r\n");
        }
        else if (strncmp(pcParameter2, "mems", strlen("mems")) == 0)
        {
            // TODO: Check when OnlyForAnnotation needs to be set
            SD_CardLoggingMemsStart(0);
            sprintf(pcWriteBuffer, "Mems datalog started\r\n");
        }
        else
        {
            sprintf(pcWriteBuffer, "Valid parameters are 'audio' and 'mems'.\r\n");
        }
    }
    else if (strncmp(pcParameter1, "stop", strlen("stop")) == 0)
    {
        if (strncmp(pcParameter2, "audio", strlen("audio")) == 0)
        {
            SD_CardLoggingAudioStop();
            sprintf(pcWriteBuffer, "Audio datalog stopped\r\n");
        }
        else if (strncmp(pcParameter2, "mems", strlen("mems")) == 0)
        {
            SD_CardLoggingMemsStop();
            sprintf(pcWriteBuffer, "Mems datalog stopped\r\n");
        }
        else
        {
            sprintf(pcWriteBuffer, "Valid parameters are 'audio' and 'mems'.\r\n");
        }
    }
    else
    {
        sprintf(pcWriteBuffer, "Valid parameters are 'start' and 'stop'.\r\n");
    }

    return 0;
}

static BaseType_t prvSdnameCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    const char *pcParameter;
    BaseType_t xParameterStringLength;

    (void) pcCommandString;
    (void) xWriteBufferLen;
    configASSERT(pcWriteBuffer);

    /* Obtain the parameter string. */
    pcParameter = FreeRTOS_CLIGetParameter(
        pcCommandString,        /* The command string itself. */
        1,                      /* Return the first parameter. */
        &xParameterStringLength /* Store the parameter string length. */
    );

    if (pcParameter != NULL)
    {
        /* Set datalog filename prefix (up to the first 7 chars) */
        strncpy(DefaultDataFileName, pcParameter, 11);
        DefaultDataFileName[11] = '\0';

        /* No output */
        sprintf(pcWriteBuffer, "\r\n");
    }
    else
    {
        /* Display datalog filename prefix */
        sprintf(pcWriteBuffer, "%s\r\n", DefaultDataFileName);
    }

    return 0;
}

static BaseType_t prvLSCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    static FILINFO Finfo;
    static DIR Dir;
    static BaseType_t xReturn = 0;
    FRESULT res;
    char path[] = "/";

    /* This assumes pcWriteBuffer is long enough. */
    (void) pcCommandString;

    if (xReturn == 0)
    {
        /* Command execution init */
        res = f_opendir(&Dir, path);
        if (res != FR_OK)
        {
            sprintf(pcWriteBuffer, "Failed to open directory: error %d.\r\n", res);
            xReturn = 0;
            return xReturn;
        }
    }

    res = f_readdir(&Dir, &Finfo);
    if ((res != FR_OK) || !Finfo.fname[0])
    {
        /* Break on error or end of dir */
        f_closedir(&Dir);
        sprintf(pcWriteBuffer, "\r\n");
        xReturn = 0; /* Command execution is complete */
    }
    else
    {
        sprintf(pcWriteBuffer,
                "%c%c%c%c%c %u/%02u/%02u %02u:%02u %9lu  %s\r\n",
                (Finfo.fattrib & AM_DIR) ? 'D' : '-', /* Directory */
                (Finfo.fattrib & AM_RDO) ? 'R' : '-', /* Read-only */
                (Finfo.fattrib & AM_HID) ? 'H' : '-', /* Hidden */
                (Finfo.fattrib & AM_SYS) ? 'S' : '-', /* System */
                (Finfo.fattrib & AM_ARC) ? 'A' : '-', /* Archive */
                (Finfo.fdate >> 9) + 1980, (Finfo.fdate >> 5) & 15, Finfo.fdate & 31,
                (Finfo.ftime >> 11), (Finfo.ftime >> 5) & 63,
                Finfo.fsize, Finfo.fname);
        xReturn = 1; /* Command execution incomplete */
    }

    return xReturn;
}

static BaseType_t prvFORMATCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
#ifndef STM32_SENSORTILEBOX
    volumeInit();
#endif /* STM32_SENSORTILEBOX */
    sprintf(pcWriteBuffer, "Volume format complete.\r\n");

    return 0;
}

static BaseType_t prvCATCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    const char *pcParameter ="";
    BaseType_t xParameterStringLength;
    static BaseType_t xReturn = 0;
    extern FIL MyFileDummy;
    FRESULT res;

    /* Ensure there is always a null terminator after each character written. */
    memset(pcWriteBuffer, 0x00, xWriteBufferLen);

    // /* Ensure the buffer leaves space for the \r\n. */
    // configASSERT(xWriteBufferLen > ( strlen("\r\n") * 2 ));
    // xWriteBufferLen -= strlen("\r\n");

    if (xReturn == 0)
    {
        /* The file has not been opened yet.  Find the file name. */
        pcParameter = FreeRTOS_CLIGetParameter(
            pcCommandString,        /* The command string itself. */
            1,                      /* Return the first parameter. */
            &xParameterStringLength /* Store the parameter string length. */
        );

        /* Sanity check something was returned. */
        configASSERT(pcParameter);

        // TODO: Make sure it is not a directory

        res = f_open(&MyFileDummy, pcParameter, FA_READ);
        if (res != FR_OK)
        {
            sprintf(pcWriteBuffer, "Cannot open \'%s\': error %d.\r\n", pcParameter, res);
        }
    }

    /* Read every line and display it */
    // *br should be checked to detect end of the file.
    // In case of *br < btr, it means the read/write pointer reached end of the file during read operation.
    if (f_gets(pcWriteBuffer, xWriteBufferLen, &MyFileDummy) != NULL)
    {
        if (f_error(&MyFileDummy) != 0)
        {
            sprintf(pcWriteBuffer, "Cannot read \'%s\'\r\n", pcParameter);
        }
        xReturn = 1;
    }
    else
    {
        /* Break when there are no characters to read or any error occurred during read operation. */
        f_close(&MyFileDummy);
        sprintf(pcWriteBuffer, "\r\n");
        xReturn = 0; /* Command execution is complete */
    }

    return xReturn;
}

static BaseType_t prvRMCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    const char *pcParameter;
    BaseType_t lParameterStringLength;
    FRESULT res;

    /* Obtain the parameter string. */
    pcParameter = FreeRTOS_CLIGetParameter(
        pcCommandString,        /* The command string itself. */
        1,                      /* Return the first parameter. */
        &lParameterStringLength /* Store the parameter string length. */
    );

    res = f_unlink(pcParameter);
    if (res != FR_OK)
    {
        sprintf(pcWriteBuffer, "Cannot remove \'%s\': error %d.\r\n", pcParameter, res);
    }
    else
    {
        sprintf(pcWriteBuffer, "\r\n");
    }

    return 0;
}
#endif /* SENSING1_USE_DATALOG */

#if SENSING1_USE_USB_MSC
static BaseType_t prvUsbCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    const char *pcParameter;
    BaseType_t lParameterStringLength;


    /* Obtain the parameter string. */
    pcParameter = FreeRTOS_CLIGetParameter(
        pcCommandString,        /* The command string itself. */
        1,                      /* Return the first parameter. */
        &lParameterStringLength /* Store the parameter string length. */
    );

    if (strncmp(pcParameter, "start", strlen("start")) == 0)
    {
        /* First unmount the volume */
        DATALOG_SD_DeInit();

        InitUSBMSC();
        sprintf(pcWriteBuffer, "USB MSC started.\r\n");
    }
    else if (strncmp(pcParameter, "stop", strlen("stop")) == 0)
    {
        /* Re-mount the volume */
        DATALOG_SD_Init();

        DeInitUSBMSC();
        sprintf(pcWriteBuffer, "USB MSC stopped.\r\n");
    }
    else
    {
        sprintf(pcWriteBuffer, "Valid parameters are 'start' and 'stop'.\r\n");
    }

    return 0;
}
#endif /* SENSING1_USE_USB_MSC */

static BaseType_t prvResetCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    /* System Reset */
    HAL_NVIC_SystemReset();

    return 0;
}

#ifdef STM32_SENSORTILEBOX
static BaseType_t prvOffCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    int32_t ret;

    ret = BSP_BC_CmdSend(SHIPPING_MODE_ON);

    if (ret != BSP_ERROR_NONE)
    {
        sprintf(pcWriteBuffer, "Failed to turn off device.\r\n");
    }
    else
    {
        /* No output */
        sprintf(pcWriteBuffer, "\r\n");
    }

    return 0;
}
#endif /* STM32_SENSORTILEBOX */
