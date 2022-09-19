/*
 IPvMe
 Copyright (c) 2022 Colin Cogle <colin@colincogle.name>
 <https://github.com/rhymeswithmogul/ipvme-stdc>
 
This program is free software: you can redistribute it and/or modify it under
the terms of the GNU Affero General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
details.

You should have received a copy of the GNU Affero General Public License along
with this program.  If not, see <https://www.gnu.org/licenses/agpl-3.0.html>.
 */

#include <stdio.h>		/* printf(), puts() */
#include <string.h>		/* strcat(), strlen() */
#include "ipvme.h"		/* PACKAGE, VERSION, BUFSIZE */
#include "help.h"

void
version ()
{
	char compileOptions[BUFSIZE] = "";
	#ifdef DEBUG
	strcat(compileOptions, "+debug ");
	#endif

	#ifdef WITHOUT_IPV4
	strcat(compileOptions, "-IPv4 ");
	#endif

	#ifdef WITHOUT_IPV6
	strcat(compileOptions, "-IPv6 ");
	#endif

	#ifdef WITHOUT_THREADS
	strcat(compileOptions, "-threads");
	#endif

	printf("%s, version %s", PACKAGE, VERSION);
	if (strlen(compileOptions) > 0)
	{
		printf(" -- customized build: %s", compileOptions);
	}
	puts("\nCopyright © 2022 Colin Cogle.\n\
This program comes with ABSOLUTELY NO WARRANTY. This is free software, and\n\
you are welcome to redistribute it under certain conditions. See the GNU Affero\n\
General Public License (version 3.0 or any later version) for more details.");
	return;
}

/**
 * usage() -- show some help.
 *
 * @author Colin Cogle
 * @since  0.1
 */
void
usage()
{
	char compileOptions[BUFSIZE] = " ";

	#ifndef WITHOUT_IPV4
	strcat(compileOptions, "[-4] ");
	#endif

	#ifndef WITHOUT_IPV6
	strcat(compileOptions, "[-6] ");
	#endif

	printf("Usage: %s%s[-V] [-?]\n", PACKAGE, compileOptions);
	return;
}

/**
 * help() -- show even more help.
 *
 * @author Colin Cogle
 * @since  0.1
 */
void
help ()
{
	version();
	puts("");
	usage();
#ifndef WITHOUT_IPV4
	puts("\t-4, --ipv4     Only get the WAN IPv4 address.");
#endif
#ifndef WITHOUT_IPV6
	puts("\t-6, --ipv6     Only get the WAN IPv6 address.");
#endif
	puts("\t-V, --version  Show version and compile information.\n\
	-?, --help     Show this help.\n\
\n\
Find this project online at https://github.com/rhymeswithmogul/IPvMe_C/\n");
	return;
}
