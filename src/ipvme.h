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

#ifndef _ipvme_h
#define _ipvme_h

/* This macro is used to allocate various buffers throughout the app.
   The largest thing that will be stored in this buffer would be a complete
   response from ip6.me.  1 KB is probably overkill. */
#ifndef BUFSIZE
#define BUFSIZE 1024
#endif

/**
 * @brief	Looks up a WAN IP address.
 * 
 * This is a wrapper function for findIPv4Address() and findIPv6Address() that
 * exists to allow for threading.
 * 
 * @param[in] version   The IP version to query.  This should be either '4' or
 *                      '6'.  All other values will cause undefined behavior.
 * @return	The detected IP address of the system.
 * @since	1.0.0
 */
void
findIPAddress (const char version);


#ifndef WITHOUT_IPV4
/**
 * @brief	Looks up a WAN IPv4 address.
 * 
 * Connects to ip4only.me to find the system's WAN IPv4 address.
 * 
 * @param[in] nothing	This function does not process input;  this is only
 *                      present to create the specific function signature to
 *                      allow for threading.
 * @return	The detected IPv4 address of the system.
 * @since	1.0.0
 */
void*
findIPv4Address (void* nothing);
#endif


#ifndef WITHOUT_IPV6
/**
 * @brief	Looks up a WAN IPv6 address.
 * 
 * Connects to ip6only.me to find the system's WAN IPv4 address.
 * 
 * @param[in] nothing	This function does not process input;  this is only
 *                      present to create the specific function signature to
 *                      allow for threading.
 * @return	The detected IPv6 address of the system.
 * @since	1.0.0
 */
void*
findIPv6Address (void* nothing);
#endif


/**
 * @brief	Parse the response from ip6.me.
 * 
 * This function takes the full response from ip6.me (including HTTP headers)
 * and deconstructs it, extracting only the IPv4/IPv6 address.
 * 
 * @param[in,out] buffer	The buffer that will contain the IP address.
 * @return	Nothing is returned;  only the parameter buffer is modified.
 * @since	1.0.0
 */
void
parseResponse (const char* const buffer);


/**
 * @brief	Return a user agent.
 * 
 * This function generates a user agent string that is included with requests
 * to ip6.me.  They ask that a non-obfuscated user agent be included with all
 * requests.
 * 
 * @return	A user agent string.
 * @since	1.0.0
 */
char*
getUserAgent ();

/* The PACKAGE and VERSION macros should be created by autogen.sh.  If either
   one does not exist for some reason, create it here.  (This will also make
   your IDE happy.) */
#ifndef PACKAGE
#define PACKAGE "IPvMe"
#endif

#ifndef VERSION
#define VERSION "1.0.0"
#endif
#endif /* _ipvme_h */