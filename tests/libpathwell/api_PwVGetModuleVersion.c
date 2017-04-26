/*-
 ***********************************************************************
 *
 * $Id: api_PwVGetModuleVersion.c,v 1.4 2017/04/18 22:12:52 klm Exp $
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
#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif
#include <pathwell.h>
#include <stdio.h>
#include <string.h>
#include <tap.h>

int
main(int iArgumentCount, char *ppcArgumentVector[])
{
  const char *pcRelease = NULL;

  plan_tests(1);
  pcRelease = PwVGetModuleVersion();
  ok(iArgumentCount == 2 && strcmp(pcRelease, ppcArgumentVector[1]) == 0, "%s", "module version");

  return exit_status();
}
