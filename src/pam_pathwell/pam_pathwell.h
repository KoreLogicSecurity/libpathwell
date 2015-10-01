/*-
 ***********************************************************************
 *
 * $Id: pam_pathwell.h,v 1.8.2.1 2015/09/30 16:05:54 klm Exp $
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
#ifndef _PAM_PATHWELL_H_INCLUDED
#define _PAM_PATHWELL_H_INCLUDED

/*-
 ***********************************************************************
 *
 * Macros
 *
 ***********************************************************************
 */
#define MODULE_NAME "pam_pathwell"

#define PW_PAM_MODE_NOT_SET 0
#define PW_PAM_MODE_ENFORCE 1
#define PW_PAM_MODE_MONITOR 2

#define PW_PAM_OPT_BLACKLIST "blacklist"
#define PW_PAM_OPT_DB "db="
#define PW_PAM_OPT_DEBUG "debug"
#define PW_PAM_OPT_ENCODING "encoding="
#define PW_PAM_OPT_MAXUSE "maxuse="
#define PW_PAM_OPT_MINLEN "minlen="
#define PW_PAM_OPT_MINLEV "minlev="
#define PW_PAM_OPT_MODE "mode="
#define PW_PAM_OPT_TOKENSET "tokenset="
#define PW_PAM_OPT_USE_AUTHTOK "use_authtok"

#endif /* !_PAM_PATHWELL_H_INCLUDED */
