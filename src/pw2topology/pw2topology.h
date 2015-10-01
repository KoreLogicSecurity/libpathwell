/*-
 ***********************************************************************
 *
 * $Id: pw2topology.h,v 1.18.2.1 2015/09/30 16:05:54 klm Exp $
 *
 ***********************************************************************
 *
 * Copyright 2013-2015 The PathWell Project, All Rights Reserved.
 *
 * This software, having been partly or wholly developed and/or
 * sponsored by KoreLogic, Inc., is hereby released under the terms
 * and conditions set forth in the project's "README.LICENSE" file.
 * For a list of all contributors and sponsors, please refer to the
 * project's "README.CREDITS" file.
 *
 ***********************************************************************
 */
#ifndef _PW2TOPOLOGY_H_INCLUDED
#define _PW2TOPOLOGY_H_INCLUDED

/*-
 ***********************************************************************
 *
 * Macros
 *
 ***********************************************************************
 */
#define PROGRAM_NAME "pathwell-pw2topology"

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
  char               *pcInputFile;
  char               *pcTokenSet;
  FILE               *pInputFile;
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

#endif /* !_PW2TOPOLOGY_H_INCLUDED */
