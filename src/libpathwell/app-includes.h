/*-
 ***********************************************************************
 *
 * $Id: app-includes.h,v 1.8 2017/04/20 13:23:39 klm Exp $
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
#ifndef _APP_INCLUDES_H_INCLUDED
#define _APP_INCLUDES_H_INCLUDED

#include <gmp.h>
#ifdef USE_JSON
#include <jansson.h>
#endif
#include <pathwell.h>
#include <pcre.h>

#include "api.h"

#ifndef PCRE_OVECTOR_ARRAY_SIZE
#define PCRE_OVECTOR_ARRAY_SIZE 30
#endif

#endif /* !_APP_INCLUDES_H_INCLUDED */
