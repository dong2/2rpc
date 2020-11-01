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
       
        free(buf);
        free(req.payload_desc);
        free(req.manufacturer);
        free(req.token);
        free(req.passwd);
        free(req.user_name);
    }

    /* rsp unpack */
    {
        Rsp *rrsp;
        unsigned char *buf; 
        unsigned int len;
        rrsp = rsp__unpack (NULL, len, buf);
        if (rrsp == NULL)
        { 
          return 1;
        }
        printf("reading checksuma:%d checksumb:%d\n",rrsp->checksuma, rrsp->checksumb); 
        printf("reading manufacturer:%s payload_desc:%s\n",rrsp->manufacturer, rrsp->payload_desc);
        rsp__free_unpacked(rrsp,NULL);
    }

    return 0;
}
