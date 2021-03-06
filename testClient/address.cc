/*- Simple example of using getaddrinfo(3) function.
 *
 *  * Michal Ludvig <michal@logix.cz> (c) 2002, 2003
 *   * http://www.logix.cz/michal/devel/
 *    *
 *     * License: public domain.
 *
*/
#include "address.hh"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int host_lookup (const char *host, char* addr)
{
  struct addrinfo hints, *res;
  int errcode;
  char addrstr[100];
  void *ptr;

  memset (&hints, 0, sizeof (hints));
  hints.ai_family = PF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags |= AI_CANONNAME;

  errcode = getaddrinfo (host, NULL, &hints, &res);
  if (errcode != 0)
    {
      perror ("getaddrinfo");
      return -1;
    }

  printf ("Host: %s\n", host);
  while (res)
    {
  //    inet_ntop (res->ai_family, res->ai_addr->sa_data, addrstr, 100);
//        printf("%s\n",addrstr);
      switch (res->ai_family)
        {
        case AF_INET:
          ptr = &((struct sockaddr_in *) res->ai_addr)->sin_addr;
          break;
        case AF_INET6:
          ptr = &((struct sockaddr_in6 *) res->ai_addr)->sin6_addr;
          break;
        }
      inet_ntop (res->ai_family, ptr, addrstr, 100);
      printf ("IPv%d address: %s (%s)\n", res->ai_family == PF_INET6 ? 6 : 4,
              addrstr, res->ai_canonname);
      res = res->ai_next;
    }
    strcpy(addr,addrstr);
  return 0;
}

//int main(){
//    char address[100];
//
//    host_lookup("mip4.kyla.fi", address);
//
//    printf("%s\n",address);
//
//    return 0;
//}
