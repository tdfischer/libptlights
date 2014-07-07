/*
 * This file is part of PtLights.
 * Copyright (C) 2013-2014 Torrie Fischer <tdfischer@hackerbots.net>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include "ptlights.h"

#include <assert.h>
#include <memory.h>
#include <malloc.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

typedef struct _PtLightsContext {
  char *address;
  int port;
  int light_socket;
  struct sockaddr_in dest_addr;
  PtLightsPixel pixels[PTLIGHTS_MAX_PIXELS];
  int autocommit;
  int refcount;
} PtLightsContext;

PtLightsContext *ptlights_context_new (const char *hostname, int port)
{
  PtLightsContext *ret;

  ret = malloc (sizeof(PtLightsContext));
  memset (ret, 0, sizeof(PtLightsContext));

  ret->refcount = 1;

  ret->address = strdup (hostname);
  ret->port = port;

  return ret;
}

void ptlights_context_ref (PtLightsContext *cxt)
{
  __sync_fetch_and_add (&cxt->refcount, 1);
}

void ptlights_context_unref (PtLightsContext *cxt)
{
  if (__sync_fetch_and_sub (&cxt->refcount, 1) == 0) {

    if (cxt->light_socket)
      close (cxt->light_socket);

    free (cxt->address);
    free (cxt);
  }
}

PtLightsPixelError
ptlights_context_set_pixel (PtLightsContext *cxt, int idx, PtLightsPixel p)
{
  assert (idx >= 0);
  assert (idx < PTLIGHTS_MAX_PIXELS);

  cxt->pixels[idx] = p;

  if (cxt->autocommit)
    return ptlights_context_commit (cxt);
}

PtLightsPixelError
ptlights_context_set_pixel_colors (PtLightsContext *cxt, int idx, uint16_t r, uint16_t g, uint16_t b)
{
  assert (idx >= 0);
  assert (idx < PTLIGHTS_MAX_PIXELS);

  cxt->pixels[idx].r = r;
  cxt->pixels[idx].g = g;
  cxt->pixels[idx].b = b;

  if (cxt->autocommit)
    return ptlights_context_commit (cxt);
}

PtLightsPixelError
ptlights_setup_socket (PtLightsContext *cxt)
{
  struct sockaddr_in dest_addr;
  struct hostent *hp;

  if (cxt->light_socket == 0) {
    cxt->light_socket = socket (AF_INET, SOCK_DGRAM, 0);

    memset (&cxt->dest_addr, 0, sizeof (cxt->dest_addr));
    cxt->dest_addr.sin_family = AF_INET;
    cxt->dest_addr.sin_port = htons (cxt->port);

    hp = gethostbyname (cxt->address);

    if (hp) {
      memcpy (&cxt->dest_addr.sin_addr, hp->h_addr_list[0], hp->h_length);
      return PTLIGHTS_ERROR_SUCCESS;
    } else {
      return PTLIGHTS_ERROR_RESOLVE;
    }
  }
}

PtLightsPixelError
ptlights_context_commit (PtLightsContext *cxt)
{
  int err;
  ptlights_setup_socket (cxt);
  err = sendto (cxt->light_socket,
                &cxt->pixels,
                sizeof (cxt->pixels),
                0,
                (struct sockaddr*)&cxt->dest_addr,
                sizeof(cxt->dest_addr));
  if (err != 0)
    return PTLIGHTS_ERROR_SEND;
  return PTLIGHTS_ERROR_SUCCESS;
}

void ptlights_set_autocommit (PtLightsContext *cxt, int enable_autocommit)
{
  cxt->autocommit = enable_autocommit;
}
