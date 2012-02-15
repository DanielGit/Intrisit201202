/* vf_blackframe.c - detect frames that are (almost) black
 *
 * $Id: vf_blackframe.c,v 1.3 2008/12/27 08:47:57 xyzhang Exp $
 *
 * search for black frames to detect scene transitions
 * (c) 2006 Julian Hall
 *
 * based on code designed for skipping commercials
 * (c) 2002-2003 Brian J. Murrell
 *
 * cleanup, simplify, speedup (c) 2006 by Ivo van Poorten
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifdef __LINUX__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#else
#include <uclib.h>
#include <uclib.h>
#include <uclib.h>
#endif

#include "config.h"
#include "mp_msg.h"

#include "img_format.h"
#include "mp_image.h"
#include "vf.h"

struct vf_priv_s {
    unsigned int bamount, bthresh, frame, lastkeyframe;
};

static int config(struct vf_instance_s* vf, int width, int height, int d_width,
                    int d_height, unsigned int flags, unsigned int outfmt) {
    return vf_next_config(vf,width,height,d_width,d_height,flags,outfmt);
}

static int query_format(struct vf_instance_s *vf, unsigned fmt) {
    switch(fmt) {
    case IMGFMT_YVU9:
    case IMGFMT_IF09:
    case IMGFMT_YV12:
    case IMGFMT_I420:
    case IMGFMT_IYUV:
    case IMGFMT_CLPL:
    case IMGFMT_Y800:
    case IMGFMT_Y8:
    case IMGFMT_NV12:
    case IMGFMT_NV21:
    case IMGFMT_444P:
    case IMGFMT_422P:
    case IMGFMT_411P:
    case IMGFMT_HM12:
        return vf_next_query_format(vf, fmt);
    }
    return 0;
}

static int put_image(struct vf_instance_s* vf, mp_image_t *mpi, double pts){
    mp_image_t *dmpi;
    int x, y;
    int nblack=0, pblack=0;
    unsigned char *yplane = mpi->planes[0];
    unsigned int ystride = mpi->stride[0];
    int pict_type = mpi->pict_type;
    int w = mpi->w, h = mpi->h;
    int bthresh = vf->priv->bthresh;
    int bamount = vf->priv->bamount;
    static const char *picttypes[4] = { "unknown", "I", "P", "B" };

    for (y=1; y<=h; y++) {
	    for (x=0; x<w; x++)
            nblack += yplane[x] < bthresh;
	    pblack = nblack*100/(w*y);
	    if (pblack < bamount) break;
        yplane += ystride;
    }

    if (pict_type > 3 || pict_type < 0) pict_type = 0;
    if (pict_type == 1) vf->priv->lastkeyframe = vf->priv->frame;

    if (pblack >= bamount)
	    mp_msg(MSGT_VFILTER, MSGL_INFO,"vf_blackframe: %u, %i%%, %s (I:%u)\n",
                                vf->priv->frame, pblack, picttypes[pict_type],
                                vf->priv->lastkeyframe);

    vf->priv->frame++;

    dmpi = vf_get_image(vf->next, mpi->imgfmt, MP_IMGTYPE_EXPORT, 0,
                                                    mpi->width, mpi->height);
    dmpi->planes[0] = mpi->planes[0];
    dmpi->stride[0] = mpi->stride[0];
    dmpi->planes[1] = mpi->planes[1];
    dmpi->stride[1] = mpi->stride[1];
    dmpi->planes[2] = mpi->planes[2];
    dmpi->stride[2] = mpi->stride[2];

    vf_clone_mpi_attributes(dmpi, mpi);

    return vf_next_put_image(vf, dmpi, pts);
}

static int control(struct vf_instance_s* vf, int request, void* data){
    return vf_next_control(vf,request,data);
}

static void uninit(struct vf_instance_s *vf) {
    if (vf->priv) free(vf->priv);
}

static int vf_open(vf_instance_t *vf, char* args){
    vf->priv = malloc(sizeof(struct vf_priv_s));
    if (!vf->priv) return 0;

    vf->config = config;
    vf->put_image = put_image;
    vf->control = control;
    vf->uninit = uninit;
    vf->query_format = query_format;

    vf->priv->bamount = 98;
    vf->priv->bthresh = 0x20;
    vf->priv->frame = 0;
    vf->priv->lastkeyframe = 0;

    if (args)
	    sscanf(args, "%u:%u", &vf->priv->bamount, &vf->priv->bthresh);
    return 1;
}

vf_info_t vf_info_blackframe = {
    "detects black frames",
    "blackframe",
    "Brian J. Murrell, Julian Hall, Ivo van Poorten",
    "Useful for detecting scene transitions",
    vf_open,
    NULL
};
