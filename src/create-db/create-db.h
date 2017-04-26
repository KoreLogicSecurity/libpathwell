/*-
 ***********************************************************************
 *
 * $Id: create-db.h,v 1.3 2017/04/18 22:12:50 klm Exp $
 *
 ***********************************************************************
 *
 * Copyright 2013-2017 The PathWell Project, All Rights Reserved.
 *
 * This software, having been partly or wholly developed and/or
 * sponsored by KoreLogic, Inc., is hereby released under the terms
 * and conditions set forth in the project's "README.LICENSE" file.
 * For a list of all contributors and sponsors, please refer to the
 * project's "README.CREDITS" file.
 *
 ***********************************************************************
 */
#ifndef _CREATE_DB_H_INCLUDED
#define _CREATE_DB_H_INCLUDED

/*-
 ***********************************************************************
 *
 * Macros
 *
 ***********************************************************************
 */
#define PROGRAM_NAME "pathwell-create-db"

#define XER_OK 0
#define XER_Usage 1
#define XER_Bootstrap 2
#define XER_Setup 3
#define XER_Create 4

/*-
 ***********************************************************************
 *
 * Typedefs
 *
 ***********************************************************************
 */
typedef struct _PROPERTIES
{
  char               *pcDbDirectory;
  char               *pcDbFile;
  char               *pcBlacklistedTopologiesFile;
  PW_D_CONTEXT       *psPwDContext;
} PROPERTIES;

/*-
 ***********************************************************************
 *
 * Function Prototypes
 *
 ***********************************************************************
 */
void                AppCleanProperties(PROPERTIES *psProperties);
PROPERTIES         *AppNewProperties(void);
void                AppUsage(void);

#endif /* !_CREATE_DB_H_INCLUDED */
