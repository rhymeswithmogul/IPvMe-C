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

#include <stdio.h>		/* fprintf() */
#include <string.h>		/* strlen(), strtok() */
#include <netdb.h>		/* struct addrinfo */
#include <unistd.h>		/* close() */
#include <sys/socket.h> /* send(), recv(), socket() */
#include <getopt.h>		/* getopt_long() */
#include "ipvme.h"		/* internal functions */
#include "help.h"		/* internal functions */

#ifndef WITHOUT_THREADS
#include <pthread.h>	/* Threading support */
#endif


int
main(const int argc, char* const argv[])
{
	char	v4flag = 0;
	char	v6flag = 0;

	#ifndef WITHOUT_THREADS
		pthread_t	threads[2];
		size_t		activeThreads = 0;
	#endif

	/* Getopt */
	signed char					c;
	int							option_index = 0;
	const char* const			short_options = "46V?";
	static const struct option	long_options[] = {
		#ifndef WITHOUT_IPV4
			{"ipv4", no_argument, 0, '4'},
		#endif
		#ifndef WITHOUT_IPV6
			{"ipv6", no_argument, 0, '6'},
		#endif
		{"help",    no_argument, 0, '?'},
		{"version", no_argument, 0, 'V'},
		{0,0,0,0}
	};
	while ((c = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1)
	{
		switch (c)
		{
			#ifndef WITHOUT_IPV4
			case '4':
				v4flag = 1;
				break;
			#endif

			#ifndef WITHOUT_IPV6
			case '6':
				v6flag = 1;
				break;
			#endif

			case 'V':
				version();
				return 0;

			case '?':
				help();
				return 0;
			
			default:
				usage();
				return 1;
		}
	}

	/* In the following blocks, we're going to check and see which IP versions
	   to check. If the user specified -4, only show the IPv4 address. Likewise,
	   if the user specified -6, only show the IPv6 address.  However, in case
	   the user specified both or none, look up them both.
	*/
	#ifndef WITHOUT_IPV4
	if (v4flag || v4flag == v6flag)
	{
		#ifdef WITHOUT_THREADS
			findIPv4Address(NULL);
		#else
			if ((pthread_create(&threads[activeThreads], NULL, findIPv4Address, NULL)) == 0)
			{
				activeThreads++;
			}
			else
			{
				fprintf(stderr, "Could not check the IPv4 address.\n");
			}
		#endif
	}
	#endif /* WITHOUT_IPV4 */

	#ifndef WITHOUT_IPV6
	if (v6flag || v4flag == v6flag)
	{
		#ifdef WITHOUT_THREADS
			findIPv6Address(NULL);
		#else
			if ((pthread_create(&threads[activeThreads], NULL, findIPv6Address, NULL)) == 0)
			{
				activeThreads++;
			}
			else
			{
				fprintf(stderr, "Could not check the IPv6 address.\n");
			}
		#endif
	}
	#endif /* WITHOUT_IPV6 */

	#ifndef WITHOUT_THREADS
	for (; activeThreads > 0; activeThreads--)
	{
		pthread_join(threads[activeThreads - 1], NULL);
	}
	#endif

	return 0;
}


/* These next two functions are used by the pthread library, and are required
   to have a signature of void* (void*).  Thus, each one of the following two
   have an aptly-named parameter, and a #pragma statement to tell GCC not to
   stop when running in -Wall -Werror mode. */
#pragma GCC diagnostic ignored "-Wunused-parameter"
#ifndef WITHOUT_IPV4
void*
findIPv4Address (void* nothing)
{
	findIPAddress('4');
	return NULL;
}
#endif

#ifndef WITHOUT_IPV6
void*
findIPv6Address (void* nothing) 
{
	findIPAddress('6');
	return NULL;
}
#endif
#pragma GCC diagnostic pop

void
findIPAddress (const char version)
{
	int				sockfd, error;
	size_t			numbytes;
	struct addrinfo	*servinfo, *j;
	char			buf[BUFSIZE];
	char			hostname[] = "ip_only.me";

	hostname[2] = version;

#ifdef DEBUG
	printf("Connecting to %s\n", hostname);
#endif
	if ((error = getaddrinfo(hostname, "80", NULL, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
		return;
	}

	/* In case multiple A/AAAA records are returned, attempt to connect
		to them all in order.  (j is our iterator.) */
	for (j = servinfo; j != NULL; j = j->ai_next)
	{
		if ((sockfd = socket(j->ai_family, j->ai_socktype, j->ai_protocol)) == -1)
		{
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, j->ai_addr, j->ai_addrlen) == -1)
		{
			close(sockfd);
			perror("client: connect");
			continue;
		}

		break;
	}
	if (j == NULL)
	{
		fprintf(stderr, "client: failed to connect\n");
		return;
	}
	freeaddrinfo(servinfo); // all done with this structure

	/* HTTP request headers */
	sprintf(buf, "GET /api/ HTTP/1.1\r\nHost: ip%conly.me\r\nUser-Agent: %s\r\n\r\n", version, getUserAgent());
	if ((numbytes = send(sockfd, buf, strlen(buf), 0)) == (size_t)(-1))
	{
		perror("HTTP request");
		return;
	}
#ifdef DEBUG
	else
	{
		fputs("\n=<request>================================================\n", stderr);
		fputs(buf, stderr);
		fputs("\n=</request>===============================================\n", stderr);
	}
#endif

	/* Get a response from the server, then parse it. */
	strcpy(buf, "");
	if ((numbytes = recv(sockfd, buf, sizeof(buf)-1, 0)) == (size_t)(-1))
	{
		perror("recv");
		return;
	}
	close(sockfd);
	buf[numbytes] = '\0';
#ifdef DEBUG
	fputs("\n=<response>===============================================\n", stderr);
	fputs(buf, stderr);
	fputs("\n=</response>==============================================\n", stderr);
#endif
	parseResponse(buf);
	return;
}

void
parseResponse (const char* const buffer)
{
	char* response  = strstr(buffer, "IPv");
	char* IPVersion = strtok(response, ",");
	char* IPAddress = strtok(NULL, ",");

	printf("%s address = %s\n", IPVersion, IPAddress);
	return;
}

char*
getUserAgent ()
{
	static char ret[100];
	sprintf(ret, "%s/%s (C/%ld)", PACKAGE, VERSION, __STDC_VERSION__);
	return ret;
}