#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

int main (int argc, const char * argv[])
{
    /* req unpack */
    {
        Req *rreq;
        unsigned char *buf; 
        unsigned int len;
        rreq = req__unpack (NULL, len, buf);
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

        free(buf);
        free(rsp.payload);
        free(rsp.payload_desc);
        free(rsp.manufacturer);
    }

    return 0;
}
