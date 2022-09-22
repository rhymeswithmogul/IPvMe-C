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
#include <string.h>		/* strlen(), strcpy(), strtok(), strcmp(), strcat() */
#include <netdb.h>		/* struct addrinfo */
#include <unistd.h>		/* close() */
#include <sys/socket.h> /* send(), recv(), socket() */
#include <getopt.h>		/* getopt_long() */
#include "ipvme.h"		/* internal functions */
#include "help.h"		/* internal functions */

#ifndef WITHOUT_THREADS
#include <pthread.h>	/* Threading support */
#endif

static char	retval[BUFSIZE] = {0};
static char	retformat[5];

int
main(const int argc, char* const argv[])
{
	char	v4flag = 0;
	char	v6flag = 0;

	#ifndef WITHOUT_THREADS
		pthread_t	threads[2];
		size_t		activeThreads = 0;
	#endif

	/* "Text" is the default format if not specified. */
	strcpy(retformat, "text");

	/* Getopt */
	signed char					c;
	int							option_index = 0;
	const char* const			short_options = "46f:V?";
	static const struct option	long_options[] = {
		#ifndef WITHOUT_IPV4
			{"ipv4", no_argument, 0, '4'},
		#endif
		#ifndef WITHOUT_IPV6
			{"ipv6", no_argument, 0, '6'},
		#endif
			{"format",  required_argument, 0, 'f'},
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

			case 'f':
				if (strcmp(optarg,"text") == 0)
				{
					strcpy(retformat,optarg);
				}
				
				#ifndef WITHOUT_XML
				else if (strcmp(optarg,"xml") == 0)
				{
					strcpy(retformat,optarg);
					strcpy(retval,"<?xml version=\"1.0\" standalone=\"yes\"?>\r\n<IPAddresses>\r\n");
				}
				#endif

				#ifndef WITHOUT_JSON
				else if (strcmp(optarg,"json") == 0)
				{
					strcpy(retformat,optarg);
					strcpy(retval,"{\r\n\t\"IPAddresses\": [");
				}
				#endif

				else
				{
					fprintf(stderr, "Unknown format \"%s\" specified!  Falling back to text.\n", optarg);
					strcpy(retval,"text");
				}
				break;

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

	/* Parse the output based on the selected return format. */
	#ifndef WITHOUT_XML
	if (strcmp(retformat,"xml") == 0)
	{
		strcat(retval,"</IPAddresses>\r\n");
	}
	#endif

	#ifndef WITHOUT_JSON
	if (strcmp(retformat,"json") == 0)
	{
		/* We have left underscores where commas should go.
		   Replace all except the last one with a comma. */
		size_t i = 0, lastUnderscore = 0;
		for (; i < strlen(retval); i++)
		{
			if (retval[i] == '_')
			{
				retval[i] = ',';
				lastUnderscore = i;
			}
		}
		retval[lastUnderscore] = ' ';
		strcat(retval,"\r\n\t]\r\n}\r\n");
	}
	#endif

	puts(retval);
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
	struct addrinfo	*servinfo, *i, hints;
	char			buf[BUFSIZE] = {0};
	char			hostname[] = "ip_only.me";

	/* Select an IP version for this function. */
	hostname[2] = version;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = (version == '6' ? AF_INET6 : AF_INET);

#ifdef DEBUG
	printf("Connecting to %s\n", hostname);
#endif
	if ((error = getaddrinfo(hostname, "80", &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
		return;
	}

	/* In case multiple A/AAAA records are returned, attempt to connect
		to them all in order.  (i is our iterator.) */
	for (i = servinfo; i != NULL; i = i->ai_next)
	{
		if ((sockfd = socket(i->ai_family, i->ai_socktype, i->ai_protocol)) == -1)
		{
			#ifdef DEBUG
				perror("client: socket");
			#endif
			continue;
		}

		if (connect(sockfd, i->ai_addr, i->ai_addrlen) == -1)
		{
			close(sockfd);
			#ifdef DEBUG
				perror("client: connect");
			#endif
			continue;
		}

		break;
	}
	freeaddrinfo(servinfo); // all done with this structure
	if (i == NULL)
	{
		sprintf(buf, "IPv%c test failed", version);
		perror(buf);
		return;
	}

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
	char  appendToRetVal[BUFSIZE] = {0};
	char* IPVersion = strtok(strstr(buffer, "IPv"), ",");
	char* IPAddress = strtok(NULL, ",");

	if (strcmp(retformat,"text") == 0)
	{
		sprintf(appendToRetVal, "%s address = %s\n", IPVersion, IPAddress);
	}
#ifndef WITHOUT_XML
	else if (strcmp(retformat,"xml") == 0)
	{
		sprintf(appendToRetVal, "\t<IPAddress family=\"%s\" address=\"%s\"/>\r\n", IPVersion, IPAddress);
	}
#endif
#ifndef WITHOUT_JSON
	else if (strcmp(retformat,"json") == 0)
	{
		sprintf(appendToRetVal, "\r\n\t\t{\r\n\t\t\t\"family\": \"%s\",\r\n\t\t\t\"address\": \"%s\"\r\n\t\t}_", IPVersion, IPAddress);
	}
#endif
	strcat(retval, appendToRetVal);
	return;
}

char*
getUserAgent ()
{
	static char ret[100];
	sprintf(ret, "%s/%s (C/%ld)", PACKAGE, VERSION, __STDC_VERSION__);
	return ret;
}