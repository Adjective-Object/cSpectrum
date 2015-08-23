#ifndef RELPATH
#define RELPATH
/*
 * Copyright (c) 1999 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * "Portions Copyright (c) 1999 Apple Computer, Inc.  All Rights
 * Reserved.  This file contains Original Code and/or Modifications of
 * Original Code as defined in and that are subject to the Apple Public
 * Source License Version 1.0 (the 'License').  You may not use this file
 * except in compliance with the License.  Please obtain a copy of the
 * License at http://www.apple.com/publicsource and read it before using
 * this file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License."
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
/*
 * relpath [-d DIR] START_DIR END_PATH
 *
 * Find a relative path from START_DIR to END_PATH.
 * Prints the relative path on standard out.
 *
 * If -d DIR, then only emit a relative path if both
 * START_DIR and END_PATH are sub-directories of DIR;
 * otherwise, emit an absolute path to END_PATH.
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <strings.h>
#include <libc.h>

static char *abspath(const char *opath, char *absbuf);

const char *progname;

std::string abspath(string opath, )

static char *
c_abspath(const char *opath, char *absbuf)
{
	struct stat st;
	char curdir[MAXPATHLEN+1];
	char symlink[MAXPATHLEN+1];
	char path[MAXPATHLEN+1];
	char file[MAXPATHLEN+1];
	char *cp;
	int cc;

	strcpy(path, opath);
	/*
	 * resolve last element of path until we know it's not
	 * a symbolic link
	 */
	while (lstat(path, &st) >= 0
	    && (st.st_mode & S_IFMT) == S_IFLNK
	    && (cc = readlink(path, symlink, sizeof(symlink)-1)) > 0) {
		symlink[cc] = '\0';
		if ((cp = rindex(path, '/')) != NULL && symlink[0] != '/')
			*++cp = '\0';
		else
			path[0] = '\0';
		strcat(path, symlink);
	}
	/*
	 * We cheat a little bit here and let getwd() do the
	 * dirty work of resolving everything before the last
	 * element of the path
	 */
	if (getwd(curdir) == NULL) {
		fprintf(stderr, "%s: %s\n", progname, curdir);
		exit(1);
	}
	if ((st.st_mode & S_IFMT) == S_IFDIR) {
		if (chdir(path) < 0) {
			fprintf(stderr, "%s: ", progname);
			perror(path);
			exit(1);
		}
		if (getwd(absbuf) == NULL) {
			fprintf(stderr, "%s: %s\n", progname, absbuf);
			exit(1);
		}
		if (chdir(curdir) < 0) {
			fprintf(stderr, "%s: ", progname);
			perror(path);
			exit(1);
		}
		return absbuf;
	}
	if ((cp = rindex(path, '/')) == NULL) {
		/*
		 * last element of path is only element and it
		 * now not a symbolic link, so we're done
		 */
		strcpy(absbuf, curdir);
		if (absbuf[strlen(absbuf) - 1] != '/')
			strcat(absbuf, "/");
		return strcat(absbuf, path);
	}
	*cp++ = 0;
	strcpy(file, cp); /* save last element */

	if (chdir(path) < 0) {
		fprintf(stderr, "%s: ", progname);
		perror(path);
		exit(1);
	}
	if (getwd(absbuf) == NULL) {
		fprintf(stderr, "%s: %s\n", progname, absbuf);
		exit(1);
	}

	if (chdir(curdir) < 0) {
		fprintf(stderr, "%s: ", progname);
		perror(path);
		exit(1);
	}
	if (absbuf[strlen(absbuf)-1] != '/')
		strcat(absbuf, "/");
	return strcat(absbuf, file);
}
#endif