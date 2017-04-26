/*-
 ***********************************************************************
 *
 * $Id: setuc.h,v 1.3 2017/04/20 02:16:46 klm Exp $
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
#ifndef _SETUC_H_INCLUDED
#define _SETUC_H_INCLUDED

/*-
 ***********************************************************************
 *
 * Macros
 *
 ***********************************************************************
 */
#define PROGRAM_NAME "pathwell-setuc"

#define XER_OK 0
#define XER_Usage 1
#define XER_Bootstrap 2
#define XER_Setup 3
#define XER_Update 4

/*-
 ***********************************************************************
 *
 * Typedefs
 *
 ***********************************************************************
 */
typedef struct _PROPERTIES
{
  char               *pcDbFile;
  char               *pcEncoding;
  char               *pcInputFile;
  FILE               *pInputFile;
  int                 iEncoding;
  int                 iPasswords;
  int                 iTokenSet;
  int                 iUseCount;
  PW_D_CONTEXT       *psPwDContext;
  PW_T_CONTEXT       *psPwTContext;
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

#endif /* !_SETUC_H_INCLUDED */
