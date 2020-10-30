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
#if 1    
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

    /* pack */
    unsigned char *buf; 
    unsigned int len;
    len = req__get_packed_size (&req);
    buf = malloc (sizeof (unsigned char) * (len));
    req__pack (&req, buf);
    printf("Writing %d serialized bytes\n",len);

    /* unpack */
    Req *rreq;
    rreq = req__unpack (NULL, len, buf);
    if (rreq == NULL)
    { 
      return 1;
    }
    printf("reading manufacturer:%s payload_desc:%s\n",rreq->manufacturer, rreq->payload_desc);
    printf("reading a:%d b:%d a:%d b:%d e:%d\n",rreq->s->a, rreq->s->b, rreq->s->c, rreq->s->d, rreq->s->e);
    req__free_unpacked(rreq,NULL);
    
    free(buf);
    free(req.payload_desc);
    free(req.manufacturer);
    free(req.token);
    free(req.passwd);
    free(req.user_name);
#else
    Rsp rsp = RSP__INIT;
    rsp.manufacturer = (char*)malloc(sizeof(char)*32);
    rsp.payload_desc = (char*)malloc(sizeof(char)*32);
    rsp.n_payload = 1400;
    rsp.payload = (uint32_t*)malloc (sizeof (uint32_t) * rsp.n_payload);
    rsp.checksuma = 55;
    rsp.checksumb = 66;
    snprintf(rsp.manufacturer, 6, "%s", "ublox");
    snprintf(rsp.payload_desc, 4, "%s", "ubx");

    /* pack */
    unsigned char *buf; 
    unsigned int len;
    len = rsp__get_packed_size (&rsp);
    buf = malloc (sizeof (uint32_t) * len);
    rsp__pack (&rsp, buf);
    printf("Writing %d serialized bytes\n",len);

    /* unpack */   
    Rsp *rrsp;
    rrsp = rsp__unpack (NULL, len, buf);
    if (rrsp == NULL)
    { 
      return 1;
    }
    printf("reading checksuma:%d checksumb:%d\n",rrsp->checksuma, rrsp->checksumb); 
    printf("reading manufacturer:%s payload_desc:%s\n",rrsp->manufacturer, rrsp->payload_desc);
    rsp__free_unpacked(rrsp,NULL);

    free(buf);
    free(rsp.payload);
    free(rsp.payload_desc);
    free(rsp.manufacturer);

#endif

    return 0;
}
