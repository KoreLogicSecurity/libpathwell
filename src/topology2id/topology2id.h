/*-
 ***********************************************************************
 *
 * $Id: topology2id.h,v 1.21 2013/10/26 14:40:32 klm Exp $
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
#ifndef _TOPOLOGY2ID_H_INCLUDED
#define _TOPOLOGY2ID_H_INCLUDED

/*-
 ***********************************************************************
 *
 * Macros
 *
 ***********************************************************************
 */
#define PROGRAM_NAME "pathwell-topology2id"

#define XER_OK 0
#define XER_Usage 1
#define XER_Bootstrap 2
#define XER_Setup 3
#define XER_Conversion 4

/*-
 ***********************************************************************
 *
 * Typedefs
 *
 ***********************************************************************
 */
typedef struct _PROPERTIES
{
  char               *pcEncoding;
  char               *pcInputFile;
  char               *pcTokenSet;
  FILE               *pInputFile;
  int                 iEncoding;
  int                 iTokenSet;
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

#endif /* !_TOPOLOGY2ID_H_INCLUDED */
