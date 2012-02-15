/*
 * HTTP Cookies
 * Reads Netscape and Mozilla cookies.txt files
 * 
 * by Dave Lambley <mplayer@davel.me.uk>
 */

#ifndef COOKIES_H
#define COOKIES_H

#include "http.h"

extern void cookies_set(HTTP_header_t * http_hdr, const char *hostname,
			const char *url);

#endif
