/*-
 ***********************************************************************
 *
 * $Id: setuc.c,v 1.6 2017/04/20 02:19:49 klm Exp $
 *
 ***********************************************************************
 *
 * Copyright 2014-2017 The PathWell Project, All Rights Reserved.
 *
 * This software, having been partly or wholly developed and/or
 * sponsored by KoreLogic, Inc., is hereby released under the terms
 * and conditions set forth in the project's "README.LICENSE" file.
 * For a list of all contributors and sponsors, please refer to the
 * project's "README.CREDITS" file.
 *
 ***********************************************************************
 */
#include "all-includes.h"

/*-
 ***********************************************************************
 *
 * Main
 *
 ***********************************************************************
 */
int
main(int iArgumentCount, char *ppcArgumentVector[])
{
  char acLine[PATHWELL_MAX_LINE_SIZE] = "";
  char c = 0;
  char *pcValue = NULL;
  int iErrors = 0;
  int iIndex = 0;
  int iValueIndex = 0;
  int iValueLength = 0;
  PROPERTIES *psProperties = NULL;

  /*-
   *********************************************************************
   *
   * Punch in and go to work.
   *
   *********************************************************************
   */
  opterr = 0;

  psProperties = AppNewProperties();
  if (psProperties == NULL)
  {
    fprintf(stderr, "%s: Error='Program failed to initialize.'\n", PROGRAM_NAME);
    return XER_Bootstrap;
  }

  /*-
   *********************************************************************
   *
   * Process command line arguments.
   *
   *********************************************************************
   */
  if (iArgumentCount < 2)
  {
    AppUsage();
  }

  while ((c = getopt(iArgumentCount, ppcArgumentVector, "c:d:f:ps:t:")) != -1)
  {
    switch (c)
    {
    case 'c':
      pcValue = optarg;
      iValueLength = strlen(pcValue);
      for (iValueIndex = 0; iValueIndex < iValueLength; iValueIndex++)
      {
        if (!isdigit((int)pcValue[iValueIndex]))
        {
          break;
        }
      }
      if (iValueLength < 1 || iValueIndex != iValueLength)
      {
        fprintf(stderr, "%s: Error='The \"use-count\" option must be an integer greater than or equal to zero.'\n", PROGRAM_NAME);
        exit(XER_Setup);
      }
      psProperties->iUseCount = atoi(pcValue);
      break;
    case 'd':
      psProperties->pcDbFile = optarg;
      if (psProperties->pcDbFile == NULL)
      {
        AppUsage();
      }
      break;
    case 'f':
      psProperties->pcInputFile = optarg;
      if (psProperties->pcInputFile == NULL)
      {
        AppUsage();
      }
      break;
    case 'p':
      psProperties->iPasswords = PATHWELL_TRUE;
      break;
    case 's':
      pcValue = optarg;
      iValueLength = strlen(pcValue);
      for (iValueIndex = 0; iValueIndex < iValueLength; iValueIndex++)
      {
        if (!isdigit((int)pcValue[iValueIndex]))
        {
          break;
        }
      }
      if (iValueLength < 1 || iValueIndex != iValueLength)
      {
        fprintf(stderr, "%s: Error='The \"token-set\" option must be an integer in the range [1-4].'\n", PROGRAM_NAME);
        exit(XER_Setup);
      }
      psProperties->iTokenSet = atoi(pcValue);
      if (psProperties->iTokenSet < PATHWELL_MIN_TOKEN_SET_ID || psProperties->iTokenSet > PATHWELL_MAX_TOKEN_SET_ID)
      {
        fprintf(stderr, "%s: Error='The \"token-set\" option must be an integer in the range [1-4].'\n", PROGRAM_NAME);
        exit(XER_Setup);
      }
      break;
    case 't':
      psProperties->pcEncoding = optarg;
      if (strcasecmp(psProperties->pcEncoding, "baseN+1") == 0)
      {
        psProperties->iEncoding = PATHWELL_ENCODING_BASENP1;
      }
      else if (strcasecmp(psProperties->pcEncoding, "bitmask") == 0)
      {
        psProperties->iEncoding = PATHWELL_ENCODING_BITMASK;
      }
      else
      {
        fprintf(stderr, "%s: Error='The \"encoding-type\" option must be one of \"baseN+1\" or \"bitmask\".'\n", PROGRAM_NAME);
        exit(XER_Setup);
      }
      break;
    default:
      AppUsage();
      break;
    }
  }

  /*
   *********************************************************************
   *
   * It's an error unless there is at least one argument or an input
   * file was specified.
   *
   *********************************************************************
   */
  if (iArgumentCount - optind < 1 && psProperties->pcInputFile == NULL)
  {
    AppUsage();
  }

  /*-
   *********************************************************************
   *
   * Make sure we have the required inputs.
   *
   *********************************************************************
   */
  if (psProperties->pcDbFile == NULL)
  {
    AppUsage();
  }

  if (psProperties->iUseCount == -1)
  {
    AppUsage();
  }

  /*-
   *********************************************************************
   *
   * Conditionally open and/or obtain a handle to the specified file.
   *
   *********************************************************************
   */
  if (psProperties->pcInputFile != NULL)
  {
    if (strcmp(psProperties->pcInputFile, "-") == 0)
    {
      psProperties->pInputFile = stdin;
    }
    else
    {
      psProperties->pInputFile = fopen(psProperties->pcInputFile, "r");
      if (psProperties->pInputFile == NULL)
      {
        fprintf(stderr,"%s: Error='Unable to open %s (No such file or directory).'\n", PROGRAM_NAME, psProperties->pcInputFile);
        AppCleanProperties(psProperties);
        exit(XER_Setup);
      }
    }
  }

  /*
   *********************************************************************
   *
   * Establish a database context, and make sure the database exists.
   *
   *********************************************************************
   */
  psProperties->psPwDContext = PwDNewContextFromParameters(psProperties->pcDbFile, SQLITE_OPEN_READWRITE, NULL, 0);
  if (!PwDContextIsValid(psProperties->psPwDContext))
  {
    fprintf(stderr, "%s: Error='%s'\n", PROGRAM_NAME, PwDGetError(psProperties->psPwDContext));
    exit(XER_Setup);
  }
  if (!PwDDbFileExists(psProperties->pcDbFile))
  {
    fprintf(stderr, "%s: Error='The specified database does not exist.'\n", PROGRAM_NAME);
    exit(XER_Setup);
  }

  /*
   *********************************************************************
   *
   * Set the encoding and token set.
   *
   *********************************************************************
   */
  PwTSetEncoding(psProperties->psPwTContext, psProperties->iEncoding);
  PwTSetTokenSet(psProperties->psPwTContext, psProperties->iTokenSet);

  /*
   *********************************************************************
   *
   * Conditionally update use counts for items specified via command line.
   *
   *********************************************************************
   */
  for (iIndex = optind; iIndex < iArgumentCount; iIndex++)
  {
    if (psProperties->iPasswords == PATHWELL_TRUE)
    {
      PwTSetPassword(psProperties->psPwTContext, ppcArgumentVector[iIndex]);
      PwTClearError(psProperties->psPwTContext);
      if (PwTPasswordToTopology(psProperties->psPwTContext) != ER_OK)
      {
        printf("Password='%s'; Topology='undef'; Error='%s';\n", PwTGetPassword(psProperties->psPwTContext), PwTGetError(psProperties->psPwTContext));
        iErrors++;
        continue;
      }
      if (PwTTopologyToId(psProperties->psPwTContext) != ER_OK)
      {
        printf("Password='%s'; Topology='undef'; Error='%s';\n", PwTGetPassword(psProperties->psPwTContext), PwTGetError(psProperties->psPwTContext));
        iErrors++;
        continue;
      }
      PwDClearError(psProperties->psPwDContext);
      if (PwDSetUseCount(psProperties->psPwDContext, psProperties->psPwTContext, psProperties->iUseCount) != ER_OK)
      {
        printf("Password='%s'; Topology='%s'; Error='%s';\n", PwTGetPassword(psProperties->psPwTContext), PwTGetTopology(psProperties->psPwTContext), PwDGetError(psProperties->psPwDContext));
        iErrors++;
        continue;
      }
      printf("Password='%s'; Topology='%s'; ID='%"PRId64"';\n", PwTGetPassword(psProperties->psPwTContext), PwTGetTopology(psProperties->psPwTContext), PwTGetId(psProperties->psPwTContext));
    }
    else
    {
      PwTSetTopology(psProperties->psPwTContext, ppcArgumentVector[iIndex]);
      PwTClearError(psProperties->psPwTContext);
      if (PwTTopologyToId(psProperties->psPwTContext) != ER_OK)
      {
        printf("Topology='%s'; Error='%s';\n", PwTGetTopology(psProperties->psPwTContext), PwTGetError(psProperties->psPwTContext));
        iErrors++;
        continue;
      }
      PwDClearError(psProperties->psPwDContext);
      if (PwDSetUseCount(psProperties->psPwDContext, psProperties->psPwTContext, psProperties->iUseCount) != ER_OK)
      {
        printf("Topology='%s'; Error='%s';\n", PwTGetTopology(psProperties->psPwTContext), PwDGetError(psProperties->psPwDContext));
        iErrors++;
        continue;
      }
      printf("Topology='%s'; ID='%"PRId64"';\n", PwTGetTopology(psProperties->psPwTContext), PwTGetId(psProperties->psPwTContext));
    }
  }

  /*-
   *********************************************************************
   *
   * Conditionally update use counts for items specified via file.
   *
   *********************************************************************
   */
  if (psProperties->pcInputFile != NULL)
  {
    while (fgets(acLine, PATHWELL_MAX_LINE_SIZE, psProperties->pInputFile) != NULL)
    {
      char *pcItem = PwSChomp(acLine, "\n\r", PW_S_CHOMP_RIGHT);
      if (psProperties->iPasswords == PATHWELL_TRUE)
      {
        PwTSetPassword(psProperties->psPwTContext, pcItem);
      }
      else
      {
        PwTSetTopology(psProperties->psPwTContext, pcItem);
      }
      if (pcItem != NULL)
      {
        free(pcItem);
      }
      if (psProperties->iPasswords == PATHWELL_TRUE)
      {
        PwTClearError(psProperties->psPwTContext);
        if (PwTPasswordToTopology(psProperties->psPwTContext) != ER_OK)
        {
          printf("Password='%s'; Topology='undef'; Error='%s';\n", PwTGetPassword(psProperties->psPwTContext), PwTGetError(psProperties->psPwTContext));
          iErrors++;
          continue;
        }
        if (PwTTopologyToId(psProperties->psPwTContext) != ER_OK)
        {
          printf("Password='%s'; Topology='undef'; Error='%s';\n", PwTGetPassword(psProperties->psPwTContext), PwTGetError(psProperties->psPwTContext));
          iErrors++;
          continue;
        }
        PwDClearError(psProperties->psPwDContext);
        if (PwDSetUseCount(psProperties->psPwDContext, psProperties->psPwTContext, psProperties->iUseCount) != ER_OK)
        {
          printf("Password='%s'; Topology='%s'; Error='%s';\n", PwTGetPassword(psProperties->psPwTContext), PwTGetTopology(psProperties->psPwTContext), PwDGetError(psProperties->psPwDContext));
          iErrors++;
          continue;
        }
        printf("Password='%s'; Topology='%s'; ID='%"PRId64"';\n", PwTGetPassword(psProperties->psPwTContext), PwTGetTopology(psProperties->psPwTContext), PwTGetId(psProperties->psPwTContext));
      }
      else
      {
        PwTClearError(psProperties->psPwTContext);
        if (PwTTopologyToId(psProperties->psPwTContext) != ER_OK)
        {
          printf("Topology='%s'; Error='%s';\n", PwTGetTopology(psProperties->psPwTContext), PwTGetError(psProperties->psPwTContext));
          iErrors++;
          continue;
        }
        PwDClearError(psProperties->psPwDContext);
        if (PwDSetUseCount(psProperties->psPwDContext, psProperties->psPwTContext, psProperties->iUseCount) != ER_OK)
        {
          printf("Topology='%s'; Error='%s';\n", PwTGetTopology(psProperties->psPwTContext), PwDGetError(psProperties->psPwDContext));
          iErrors++;
          continue;
        }
        printf("Topology='%s'; ID='%"PRId64"';\n", PwTGetTopology(psProperties->psPwTContext), PwTGetId(psProperties->psPwTContext));
      }
    }
  }

  /*-
   *********************************************************************
   *
   * Shut down and go home.
   *
   *********************************************************************
   */
  AppCleanProperties(psProperties);

  if (iErrors > 0)
  {
    return XER_Update;
  }

  return XER_OK;
}


/*-
 ***********************************************************************
 *
 * AppCleanProperties
 *
 ***********************************************************************
 */
void
AppCleanProperties(PROPERTIES *psProperties)
{
  if (psProperties != NULL)
  {
    if (psProperties->pInputFile != NULL)
    {
      fclose(psProperties->pInputFile);
    }
    PwDFreeContext(&psProperties->psPwDContext);
    PwTFreeContext(&psProperties->psPwTContext);
    free(psProperties);
  }
  return;
}


/*-
 ***********************************************************************
 *
 * AppNewProperties
 *
 ***********************************************************************
 */
PROPERTIES *
AppNewProperties(void)
{
  PROPERTIES *psProperties = (PROPERTIES *)calloc(1, sizeof(PROPERTIES));

  if (psProperties == NULL)
  {
    return NULL;
  }
  psProperties->iEncoding = PATHWELL_DEFAULT_ENCODING;
  psProperties->iTokenSet = PATHWELL_DEFAULT_TOKEN_SET_ID;
  psProperties->iUseCount = -1;
  psProperties->psPwTContext = PwTNewContext();
  if (psProperties->psPwTContext == NULL)
  {
    AppCleanProperties(psProperties);
    return NULL;
  }

  return psProperties;
}


/*-
 ***********************************************************************
 *
 * AppUsage
 *
 ***********************************************************************
 */
void
AppUsage(void)
{
  fprintf(stderr, "\n");
  fprintf(stderr, "Usage: %s [-p] [-f {file|-}] [-s token-set] [-t encoding-type] -c count -d db [{password|topology} [...]]\n", PROGRAM_NAME);
  fprintf(stderr, "\n");
  fprintf(stderr, "Where options can be any of the following:\n");
  fprintf
  (
    stderr,
    "\n\
  -c <count>\n\
                        New use count that is to be assigned to all supplied\n\
                        passwords/topologies.\n\
  -d <db>\n\
                        Path and name of the database that is to be updated.\n\
  -f <file>\n\
                        File containing items, one per line, to be processed.\n\
                        A value of '-' causes the program to read from stdin.\n\
  -p\n\
                        Specifies that the items being supplied are passwords\n\
                        rather than topologies.\n\
  -s <token-set>\n\
                        ID of the token set to use for performing conversions.\n\
                        The following token sets are supported:\n\
                          1) [dlus] (default)\n\
                          2) [dlusf]\n\
                          3) [dlusfx]\n\
                          4) [dlusfxH]\n\
  -t <encoding-type>\n\
                        Type of encoding to use for conversions. Must be one of\n\
                        'baseN+1' or 'bitmask'. The default value is 'baseN+1'.\n\
\n"
  );
  fprintf(stderr, "\n");
  exit(XER_Usage);
}
