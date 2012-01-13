/*
 * Copyright (c) 2001 Softweyr LLC.  All rights reserved.
 *
 * liedentd: a simple server for RFC1413 Identification Service
 * that refuses to give away critical system information.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notices, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notices, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY SOFTWEYR LLC AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL SOFTWEYR LLC OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id: liedentd.c,v 1.4 2001/03/12 02:07:10 wes Exp $
 */

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <md5.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#define IDENT_PORT 113
#define NOBODY 32767

char *pname;

const int STARS_SHINE = 1;

static void
usage()
{
    fprintf(stderr, "usage: %s [ -m message] [-s system] [-t timeout]\n", pname);
    fprintf(stderr, "\
\t-m\tSpecify ident message (username).  If -m is not specified, a\n\
\t\treadable random username will be generated for each response.\n\
\t-s\tSpecify system name, default: \"UNIX\".\n\
\t-t\tSpecify connection timeout in seconds, default: 15.\n\n");
    exit(1);
}


int
main(int argc, char *argv[])
{
    int serv_sock, client_sock, cli_len;
    struct sockaddr_in cli_addr, serv_addr;
    fd_set infds, testfds;
    int nfds = 0, minfds = 0, maxfds = 0, nready;
    struct timeval tv = { 15, 0 };

    char inbuf[BUFSIZ], outbuf[BUFSIZ * 2], *inptr;
    size_t inlen;

    int ch;
    extern char *optarg;
    extern int optind;

    int daemonize = 1;

    char * sysname = "UNIX";

    int randomUser = 1;
    char * message[64];
    char digest[64] = "This is not really a random name, but plays one on TV.";
    int msgLen = strlen(digest);
    time_t now;

    MD5_CTX ctx;

    pname = argv[0];

    while ((ch = getopt(argc, argv, "dm:s:t:")) != -1)
    {
	switch (ch)
	{
	case 'd':		/* Undocumented -d flag for debugging */
	    daemonize = 0;
	    break;

	case 'm':
	    strncpy((char *) message, optarg, 64);
	    randomUser = 0;
	    break;

	case 's':
	    sysname = optarg;
	    break;

	case 't':
	    tv.tv_sec = atoi(optarg);
	    break;

	case '?':
	default:
	    usage();
	}
    }

    argc -= optind;
    argv += optind;

    openlog(pname, LOG_AUTH, LOG_CONS);

    /*
     * Create a TCP socket and bind it to the ident port.
     */
    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (serv_sock < 0)
    {
	syslog(LOG_ERR, "creating server socket: %m");
	return -1;
    }
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &serv_sock, sizeof serv_sock);

    bzero((char *) &serv_addr, sizeof (serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(IDENT_PORT);

    if (bind(serv_sock, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0)
    {
	syslog(LOG_ERR, "binding server socket to ident port: %m");
	return -1;
    }

    listen(serv_sock, 5);

    /*
     * Create a pid file for use in stopping this thing.
     */
    if ((client_sock = open("/var/run/liedentd.pid", O_WRONLY | O_CREAT, 0644)) < 0)
    {
	syslog(LOG_ERR, "creating pid file /var/run/liedentd.pid: %m");
	return -2;
    }

    /*
     * Drop our root privileges so we are not an attack vector.
     */
    if (daemonize)
    {
	setuid(NOBODY);
	chdir("/tmp");
	daemon(1, 0);
    }

    /*
     * Finish writing the pid file now, after the daemon call, so the
     * pid will be correct.
     */
    asprintf(&optarg, "%d\n", getpid());
    write(client_sock, optarg, strlen(optarg));
    free(optarg);
    close(client_sock);

    /*
     * Setup for selecting I/O.
     */
    FD_ZERO(&infds);
    FD_SET(serv_sock, &infds);

    maxfds = serv_sock;

    while (STARS_SHINE)
    {
	testfds = infds;

	/*
	 * Wait for something to happen, or for timeout.
	 */
	if ((nready = select(maxfds + 1, &testfds, NULL, NULL, &tv)) == 0)
	{
	    /*
	     * The select timed out.  Close/kill any outstanding connections
	     * and try again.  If we haven't received any connections yet,
	     * minfds will be zero and we can skip this.
	     */
	    for (client_sock = minfds; client_sock && client_sock <= maxfds; client_sock++)
	    {
		/*
		 * Don't zap the listen socket!  Ordinarily this shouldn't
		 * happen, since serv_sock is created before any potential
		 * version of client_sock, but careful is better than
		 * crashed.
		 */
		if (client_sock == serv_sock)
		    continue;

		if (FD_ISSET(client_sock, &infds))
		{
		    FD_CLR(client_sock, &infds);
		    close(client_sock);
		}
	    }
	    continue;
	}
	else if (nready > 0)
	{
	    /*
	     * We have nready sockets ready for processing.  First check to
	     * see if we have a connection attempt to fulfill.
	     */
	    if (FD_ISSET(serv_sock, &testfds))
	    {
		/*
		 * Accept the connection and add to the current set.
		 */
		cli_len = sizeof (cli_addr);
		bzero((char *) &cli_addr, sizeof (cli_addr));
	
		client_sock = accept(serv_sock, (struct sockaddr *) &cli_addr, &cli_len);
		if (client_sock < 0)
		{
		    syslog(LOG_WARNING, "accepting connection on server socket: %m");
		    continue;
		}

		FD_SET(client_sock, &infds);
		maxfds = MAX(maxfds, client_sock);
		minfds = (minfds == 0) ? client_sock : MIN(minfds, client_sock);

		nready--;
	    }
	
	    /*
	     * Roll through the remaining sockets in the fdset, answering each
	     * that has a request available.
	     */
	    for (client_sock = minfds; nready && client_sock <= maxfds; client_sock++)
	    {
		if (FD_ISSET(client_sock, &testfds))
		{
		    if ((inlen = read(client_sock, inbuf, BUFSIZ - 1)) < 0)
		    {
			syslog(LOG_WARNING, "reading from client: %m");
		    }
		    else
		    {
			inbuf[inlen] = '\0';

			inptr = strchr(inbuf, '\r');
			if (inptr == NULL)
			{
			    syslog(LOG_WARNING, "invalid client request: %s", inbuf);
			}
			else
			{
			    *inptr = '\0';

			    /*
			     * Randomize the "name" string unless the user specified one.
			     */
			    if (randomUser)
			    {
				MD5Init(&ctx);
				MD5Update(&ctx, digest, msgLen);
				MD5End(&ctx, (char *) message);
				
				/*
				 * Seed the next "random" name.  This just sort of relies
				 * on message being preallocated to at least sizeof time_t
				 * + MD5_DIGEST_LENGTH (or 20 bytes) in length.
				 */
				now = time(0);
				bcopy(&now, digest, sizeof now);
				msgLen = strlen((char *) message);
				bcopy(message, digest + sizeof now, msgLen);
				msgLen += sizeof now;
			    }

			    snprintf(outbuf, 2 * BUFSIZ, "%s : USERID : %s : %s\r\n", inbuf, sysname, message);
			    if (write(client_sock, outbuf, strlen(outbuf)) < strlen(outbuf))
			    {
				syslog(LOG_WARNING, "writing response to client: %m");
			    }
			}
		    }

		    close(client_sock);
		    FD_CLR(client_sock, &infds);
		    nready--;
		}
	    }
	}
	else
	{
	    /*
	     * The select returned an error.  I wonder what that means?
	     */
	    syslog(LOG_ERR, "select failed: %m");
	}
    }

    /*
     * Not normally reached, but those stars may go out some day...
     */
    syslog(LOG_CRIT, "exiting");
    closelog();

    close(serv_sock);

    return 1;
}
