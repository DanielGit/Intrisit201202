/*
 * CD Info 
 * by Bertrand Baudet <bertrand_baudet@yahoo.com>
 * (C) 2002, MPlayer team.
*/

#include "config.h"

#ifdef __LINUX__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#else
#include <mplaylib.h>
#include <mplaylib.h>
#include <mplaylib.h>
#endif
#include "mp_msg.h"
#include "help_mp.h"
#include "cdd.h"

/*******************************************************************************************************************
 *
 * xmcd parser, cd info list
 *
 *******************************************************************************************************************/

cd_info_t*
cd_info_new() {
	cd_info_t *cd_info = NULL;
	
	cd_info = malloc(sizeof(cd_info_t));
	if( cd_info==NULL ) {
		mp_msg(MSGT_DEMUX, MSGL_ERR, MSGTR_MemAllocFailed);
		return NULL;
	}
	
	memset(cd_info, 0, sizeof(cd_info_t));
	
	return cd_info;
}

void
cd_info_free(cd_info_t *cd_info) {
	cd_track_t *cd_track, *cd_track_next;
	if( cd_info==NULL ) return;
	if( cd_info->artist!=NULL ) free(cd_info->artist);
	if( cd_info->album!=NULL ) free(cd_info->album);
	if( cd_info->genre!=NULL ) free(cd_info->genre);

	cd_track_next = cd_info->first;
	while( cd_track_next!=NULL ) {
		cd_track = cd_track_next;
		cd_track_next = cd_track->next;
		if( cd_track->name!=NULL ) free(cd_track->name);
		free(cd_track);
	}
}

cd_track_t*
cd_info_add_track(cd_info_t *cd_info, char *track_name, unsigned int track_nb, unsigned int min, unsigned int sec, unsigned int msec, unsigned long frame_begin, unsigned long frame_length) {
	cd_track_t *cd_track;
	
	if( cd_info==NULL || track_name==NULL ) return NULL;
	
	cd_track = malloc(sizeof(cd_track_t));
	if( cd_track==NULL ) {
		mp_msg(MSGT_DEMUX, MSGL_ERR, MSGTR_MemAllocFailed);
		return NULL;
	}
	memset(cd_track, 0, sizeof(cd_track_t));
	
	cd_track->name = malloc(strlen(track_name)+1);
	if( cd_track->name==NULL ) {
		mp_msg(MSGT_DEMUX, MSGL_ERR, MSGTR_MemAllocFailed);
		free(cd_track);
		return NULL;
	}
	strcpy(cd_track->name, track_name);
	cd_track->track_nb = track_nb;
	cd_track->min = min;
	cd_track->sec = sec;
	cd_track->msec = msec;
	cd_track->frame_begin = frame_begin;
	cd_track->frame_length = frame_length;

	if( cd_info->first==NULL ) {
		cd_info->first = cd_track;
	}
	if( cd_info->last!=NULL ) {
		cd_info->last->next = cd_track;
	}

	cd_track->prev = cd_info->last;
	
	cd_info->last = cd_track;
	cd_info->current = cd_track;

	cd_info->nb_tracks++;
	
	return cd_track;
}

cd_track_t*
cd_info_get_track(cd_info_t *cd_info, unsigned int track_nb) {
	cd_track_t *cd_track=NULL;

	if( cd_info==NULL ) return NULL;

	cd_track = cd_info->first;
	while( cd_track!=NULL ) {
		if( cd_track->track_nb==track_nb ) {
			return cd_track;
		}
		cd_track = cd_track->next;
	}
	return NULL;
}

void
cd_info_debug(cd_info_t *cd_info) {
	cd_track_t *current_track;
	printf("================ CD INFO === start =========\n");
	if( cd_info==NULL ) {
		printf("cd_info is NULL\n");
		return;
	}
	printf(" artist=[%s]\n", cd_info->artist);
	printf(" album=[%s]\n", cd_info->album);
	printf(" genre=[%s]\n", cd_info->genre);
	printf(" nb_tracks=%d\n", cd_info->nb_tracks);
	printf(" length= %2d:%02d.%02d\n", cd_info->min, cd_info->sec, cd_info->msec);
	current_track = cd_info->first;
	while( current_track!=NULL ) {
		printf("  #%2d %2d:%02d.%02d @ %7ld\t[%s] \n", current_track->track_nb, current_track->min, current_track->sec, current_track->msec, current_track->frame_begin, current_track->name);
		current_track = current_track->next;
	}
	printf("================ CD INFO ===  end  =========\n");
}
