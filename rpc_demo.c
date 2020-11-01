/*
    Copyright 2016 Garrett D'Amore <garrett@damore.org>

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom
    the Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.

    "nanomsg" is a trademark of Martin Sustrik
*/

/*  This program serves as an example for how to write a simple RPC service,
    using the request/reply pattern.  The server is just a single threaded
    for loop, which processes each request.  The requests run quickly enough
    that there is no need for parallelism.

    Our demonstration application layer protocol is simple.  The client sends
    a name, and the server replies with a greeting based on the time of day.
    The messages are sent in ASCII, and are not zero terminated.

    To run this program, start the server as rpc_demo <url> -s
    Then connect to it with the client as rpc_client <url> <name>.
    The client will print a timezone appropriate greeting, based on
    the time at the server.  For example:

    % ./rpc_demo tcp://127.0.0.1:5555 -s &
    % ./rpc_demo tcp://127.0.0.1:5555 Garrett
    Good morning, Garrett.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <nanomsg/nn.h>
#include <nanomsg/reqrep.h>

#include "test.pb-c.h"

static void fletcherChecksum(unsigned char* buffer, int size, unsigned char* checkSumA, unsigned char* checkSumB)
{
    int i = 0;
    *checkSumA = 0;
    *checkSumB = 0;
    for(; i < size; i++)
    {
        *checkSumA += buffer[i];
        *checkSumB += *checkSumA;
    }
}

/*  The server runs forever. */
int server(const char *url)
{
    int fd; 

    /*  Create the socket. */
    fd = nn_socket (AF_SP, NN_REP);
    if (fd < 0) {
        fprintf (stderr, "nn_socket: %s\n", nn_strerror (nn_errno ()));
        return (-1);
    }

    /*  Bind to the URL.  This will bind to the address and listen
        synchronously; new clients will be accepted asynchronously
        without further action from the calling program. */

    if (nn_bind (fd, url) < 0) {
        fprintf (stderr, "nn_bind: %s\n", nn_strerror (nn_errno ()));
        nn_close (fd);
        return (-1);
    }

    /*  Now we can just process results.  Note that there is no explicit
        accept required.  We just receive a request, and reply to it.
        Its important to note that we must not issue two receives in a
        row without replying first, or the following receive(s) will
        cancel any unreplied requests. */

    for (;;) {
        uint8_t msg[1400];
        uint8_t rc;
         
        rc = nn_recv (fd, msg, sizeof (msg), 0);
        if (rc < 0) {
            /*  Any error here is unexpected. */
            fprintf (stderr, "nn_recv: %s\n", nn_strerror (nn_errno ()));
            break;
        }

        /* req unpack */
        {
            Req *rreq;
            rreq = req__unpack (NULL, rc, msg);
            if (rreq == NULL)
            { 
              return 1;
            }
		    printf("reading manufacturer:%s payload_desc:%s\n",rreq->manufacturer, rreq->payload_desc);
		    printf("reading a:%d b:%d a:%d b:%d e:%d\n",rreq->s->a, rreq->s->b, rreq->s->c, rreq->s->d, rreq->s->e);
            
            req__free_unpacked(rreq,NULL);
        }


        /* rsp pack */ 
        {   
            Rsp rsp = RSP__INIT;
            rsp.manufacturer = (char*)malloc(sizeof(char)*32);
            rsp.payload_desc = (char*)malloc(sizeof(char)*32);
            rsp.n_payload = 1400;
            rsp.payload = (uint32_t*)malloc (sizeof (uint32_t) * rsp.n_payload);
            rsp.checksuma = 56;
            rsp.checksumb = 78;
            snprintf(rsp.manufacturer, 6, "%s", "ublox");
            snprintf(rsp.payload_desc, 4, "%s", "ubx");

            unsigned char *buf; 
            unsigned int len;
            len = rsp__get_packed_size (&rsp);
            buf = malloc (sizeof (uint32_t) * len);
            rsp__pack (&rsp, buf);
            printf("Writing %d serialized bytes\n",len);

            if (nn_send (fd, buf, len, 0) < 0) {
                /*  There are several legitimate reasons this can fail.
                    We note them for debugging purposes, but then ignore
                    otherwise.  If the socket is closed or failing, we will
                    notice in recv above, and exit then. */
                fprintf (stderr, "nn_send: %s (ignoring)\n",
                    nn_strerror (nn_errno ()));
            }

            free(buf);
            free(rsp.payload);
            free(rsp.payload_desc);
            free(rsp.manufacturer);
        }
        
    }

    nn_close (fd);
    return (-1);
}

/*  The client runs just once, and then returns. */
int client (const char *url, const char *username)
{
    int fd;
    int rc;
    char *msg;

    fd = nn_socket (AF_SP, NN_REQ);
    if (fd < 0) {
        fprintf (stderr, "nn_socket: %s\n", nn_strerror (nn_errno ()));
        return (-1);
    }

    if (nn_connect (fd, url) < 0) {
        fprintf (stderr, "nn_socket: %s\n", nn_strerror (nn_errno ()));
        nn_close (fd);
        return (-1);        
    }

    /* req pack */
    {  
        Sub sub = SUB__INIT;
        sub.a = 1;
        sub.b = 2;
        sub.c = 3;
        sub.d = 4;
        sub.e = 5;      
        Req req = REQ__INIT;
        req.s = &sub;
        req.user_name = (char*)malloc(sizeof(char)*32);
        req.passwd = (char*)malloc(sizeof(char)*32);
        req.token = (char*)malloc(sizeof(char)*32);
        req.manufacturer = (char*)malloc(sizeof(char)*32);
        req.payload_desc = (char*)malloc(sizeof(char)*32);
        snprintf(req.manufacturer, 6, "%s", "ublox");
        snprintf(req.payload_desc, 4, "%s", "ubx");
      
        unsigned char *buf; 
        unsigned int len;
        len = req__get_packed_size (&req);
        buf = malloc (sizeof (unsigned char) * (len));
        req__pack (&req, buf);
        printf("Writing %d serialized bytes\n",len);

        if (nn_send (fd, buf, len, 0) < 0) {
            fprintf (stderr, "nn_send: %s\n", nn_strerror (nn_errno ()));
            nn_close (fd);
            return (-1);
        }
    
        free(buf);
        free(req.payload_desc);
        free(req.manufacturer);
        free(req.token);
        free(req.passwd);
        free(req.user_name);
    }

    /*  Here we ask the library to allocate response buffer for us (NN_MSG). */
    rc = nn_recv (fd, &msg, NN_MSG, 0);
    if (rc < 0) {
        fprintf (stderr, "nn_recv: %s\n", nn_strerror (nn_errno ()));
        nn_close (fd);
        return (-1);
    }

    /* rsp unpack */
    {
        Rsp *rrsp;
        rrsp = rsp__unpack (NULL, rc, msg);
        if (rrsp == NULL)
        { 
          return 1;
        }
        printf("reading checksuma:%d checksumb:%d\n",rrsp->checksuma, rrsp->checksumb); 
        printf("reading manufacturer:%s payload_desc:%s\n",rrsp->manufacturer, rrsp->payload_desc);
        rsp__free_unpacked(rrsp,NULL);
    }

    nn_close (fd);
    nn_freemsg (msg);

    return (0);
}

int main (int argc, char **argv)
{
    int rc;
    if (argc < 3) {
        fprintf (stderr, "Usage: %s <url> [-s|name]\n", argv[0]);
        exit (EXIT_FAILURE);
    }
    if (strcmp (argv[2], "-s") == 0) {
        rc = server (argv[1]);
    } else {
        rc = client (argv[1], argv[2]);
    }
    exit (rc == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}
