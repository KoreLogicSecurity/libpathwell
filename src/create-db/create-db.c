/*-
 ***********************************************************************
 *
 * $Id: create-db.c,v 1.5 2013/11/03 21:06:34 hlein Exp $
 *
 ***********************************************************************
 *
 * Copyright 2013-2013 The PathWell Project, All Rights Reserved.
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
  int iCFlag = PATHWELL_FALSE;
  int iDFlag = PATHWELL_FALSE;
  int iError = 0;
  int iFFlag = PATHWELL_FALSE;
  int iNumFlags = 0;
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

  while ((c = getopt(iArgumentCount, ppcArgumentVector, "cd:f:")) != -1)
  {
    switch (c)
    {
    case 'c':
      iCFlag = PATHWELL_TRUE;
      break;
    case 'd':
      iDFlag = PATHWELL_TRUE;
      psProperties->pcDbFile = optarg;
      if (psProperties->pcDbFile == NULL)
      {
        AppUsage();
      }
      break;
    case 'f':
      iFFlag = PATHWELL_TRUE;
      psProperties->pcBlacklistedTopologiesFile = optarg;
      if (psProperties->pcBlacklistedTopologiesFile == NULL)
      {
        AppUsage();
      }
      break;
    default:
      AppUsage();
      break;
    }
    iNumFlags++;
  }

  /*-
   *********************************************************************
   *
   * A blacklisted topologies file, '-f' is optional.
   *
   *********************************************************************
   */
  if (iFFlag == PATHWELL_TRUE)
  {
    if (!PwDDbFileExists(psProperties->pcBlacklistedTopologiesFile))
    {
      fprintf(stderr,"%s: Error='File \"%s\" does not exist.'\n", PROGRAM_NAME, psProperties->pcBlacklistedTopologiesFile);
      exit(XER_Setup);
    }
  }
  else
  {
    if (PwDDbFileExists(PATHWELL_CUSTOM_BLACKLIST_FILE))
    {
      psProperties->pcBlacklistedTopologiesFile = PATHWELL_CUSTOM_BLACKLIST_FILE;
    }
    else
    {
      if (PwDDbFileExists(PATHWELL_COMMON_BLACKLIST_FILE))
      {
        psProperties->pcBlacklistedTopologiesFile = PATHWELL_COMMON_BLACKLIST_FILE;
      }
      else
      {
        psProperties->pcBlacklistedTopologiesFile = NULL; // No custom/common file exists, but that's OK.
      }
    }
  }

  /*
   *********************************************************************
   *
   * It's an error if any arguments remain.
   *
   *********************************************************************
   */
  if (optind != iArgumentCount)
  {
    AppUsage();
  }

  /*
   *********************************************************************
   *
   * Allocate and initialize a new context structure.
   *
   *********************************************************************
   */
  psProperties->psPwDContext = PwDNewContextFromParameters(psProperties->pcDbFile, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL, 0);
  if (!PwDContextIsValid(psProperties->psPwDContext))
  {
    fprintf(stderr,"%s: Error='%s'\n", PROGRAM_NAME, PwDGetError(psProperties->psPwDContext));
    exit(XER_Setup);
  }

  /*
   *********************************************************************
   *
   * Conditionally create missing directories.
   *
   *********************************************************************
   */
  psProperties->pcDbDirectory = PwSDirname(psProperties->pcDbFile);
  if (psProperties->pcDbDirectory == NULL)
  {
    fprintf(stderr,"%s: Error='Failed to determine DB directory (%s).'\n", PROGRAM_NAME, strerror(errno));
    exit(XER_Create);
  }
  if (PwDDbDirectoryExists(psProperties->pcDbDirectory) != PATHWELL_TRUE)
  {
    if (iCFlag == PATHWELL_TRUE)
    {
      iError = PwSCreateDirectory(psProperties->pcDbDirectory, 00750);
      if (iError != ER_OK)
      {
        fprintf(stderr,"%s: Error='Failed to create DB directory (%s).'\n", PROGRAM_NAME, strerror(errno));
        exit(XER_Create);
      }
    }
    else
    {
      fprintf(stderr,"%s: Error='DB directory does not exist or is not accessible. Try using \"-c\" to create missing directories.'\n", PROGRAM_NAME);
      exit(XER_Create);
    }
  }

  /*
   *********************************************************************
   *
   * Create and initialize the specified database.
   *
   *********************************************************************
   */
  iError = PwDCreateSchema(psProperties->psPwDContext);
  if (iError != ER_OK)
  {
    fprintf(stderr,"%s: Error='%s'\n", PROGRAM_NAME, PwDGetError(psProperties->psPwDContext));
    exit(XER_Create);
  }
  if (psProperties->pcBlacklistedTopologiesFile != NULL)
  {
    iError = PwDLoadBlacklistedTopologies(psProperties->psPwDContext, psProperties->pcBlacklistedTopologiesFile);
    if (iError != ER_OK)
    {
      fprintf(stderr,"%s: Error='%s'\n", PROGRAM_NAME, PwDGetError(psProperties->psPwDContext));
      exit(XER_Create);
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
    if (psProperties->pcDbDirectory != NULL)
    {
      free(psProperties->pcDbDirectory);
    }
    PwDFreeContext(psProperties->psPwDContext);
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
  fprintf(stderr, "Usage: %s [-c] [-f <blacklist-file>] -d <db>\n", PROGRAM_NAME);
  fprintf(stderr, "\n");
  fprintf(stderr, "Where options can be any of the following:\n");
  fprintf
  (
    stderr,
    "\n\
  -c\n\
                        Specifies that missing directories are to be created\n\
                        as necessary.\n\
  -d <db>\n\
                        Path and name of the database that is to be created. A\n\
                        typical value is \"/var/lib/PathWell/PathWell.db\".\n\
  -f <blacklist-file>\n\
                        File containing blacklisted topologies, one per line,\n\
                        to be loaded into the database. The default value for\n\
                        this option is \"/etc/PathWell/topology_blacklist.local\"\n\
                        with \"/usr/share/PathWell/topology_blacklist.default\"\n\
                        as the fallback. Note: If this option isn't specified\n\
                        and neither of the default files exist, no topologies\n\
                        will be loaded into the blacklist.\n\
\n"
  );
  fprintf(stderr, "\n");
  exit(XER_Usage);
}
