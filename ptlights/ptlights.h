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

#include <stdint.h>

#define PTLIGHTS_MAX_PIXELS 198
#define PTLIGHTS_MAX_COLOR 65535

typedef struct _PtLightsContext PtLightsContext;

#pragma pack(push)
#pragma pack(1)
typedef struct _PtLightsPixel {
  uint16_t b;
  uint16_t r;
  uint16_t g;
} PtLightsPixel;
#pragma pack(pop)

typedef enum _PtLightsPixelError {
  PTLIGHTS_ERROR_SUCCESS,
  PTLIGHTS_ERROR_RESOLVE,
  PTLIGHTS_ERROR_SEND
} PtLightsPixelError;

PtLightsContext *ptlights_context_new (const char *hostname, int port);
void ptlights_context_ref (PtLightsContext *cst);
void ptlights_context_unref (PtLightsContext *cxt);
PtLightsPixelError ptlights_context_set_pixel (PtLightsContext *cxt, int idx, PtLightsPixel p);
PtLightsPixelError ptlights_context_set_pixel_colors (PtLightsContext *cxt, int idx, uint16_t r, uint16_t g, uint16_t b);
PtLightsPixelError ptlights_context_commit (PtLightsContext *cxt);
void ptlights_set_autocommit (PtLightsContext *cxt, int enable_autocommit);
