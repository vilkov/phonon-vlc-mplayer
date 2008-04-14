/*****************************************************************************
 * control_structures.h: global header for mediacontrol
 *****************************************************************************
 * Copyright (C) 2005 the VideoLAN team
 * $Id: 8d7f9b850212b682e880b39256ce7348990f928d $
 *
 * Authors: Olivier Aubert <olivier.aubert@liris.univ-lyon1.fr>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/
/**
 * \defgroup mediacontrol_structures MediaControl Structures
 * Data structures used in the MediaControl API.
 *
 * @{
 */

#ifndef _VLC_CONTROL_STRUCTURES_H
#define _VLC_CONTROL_STRUCTURES_H 1

# ifdef __cplusplus
extern "C" {
# endif

#include <stdint.h>

/**
 * A position may have different origins:
 *  - absolute counts from the movie start
 *  - relative counts from the current position
 *  - modulo counts from the current position and wraps at the end of the movie
 */
typedef enum  {
    mediacontrol_AbsolutePosition,
    mediacontrol_RelativePosition,
    mediacontrol_ModuloPosition
} mediacontrol_PositionOrigin;

/**
 * Units available in mediacontrol Positions
 *  - ByteCount number of bytes
 *  - SampleCount number of frames
 *  - MediaTime time in milliseconds
 */
typedef enum {
    mediacontrol_ByteCount,
    mediacontrol_SampleCount,
    mediacontrol_MediaTime
} mediacontrol_PositionKey;

/**
 * Possible player status
 */
typedef enum {
    mediacontrol_PlayingStatus, mediacontrol_PauseStatus,
    mediacontrol_ForwardStatus, mediacontrol_BackwardStatus,
    mediacontrol_InitStatus,    mediacontrol_EndStatus,
    mediacontrol_UndefinedStatus
} mediacontrol_PlayerStatus;

/**
 * MediaControl Position
 */
typedef struct {
    mediacontrol_PositionOrigin origin;
    mediacontrol_PositionKey key;
    vlc_int64_t value;
} mediacontrol_Position;

/**
 * RGBPicture structure
 * This generic structure holds a picture in an encoding specified by type.
 */
typedef struct {
    int  width;
    int  height;
    uint32_t type;
    vlc_int64_t date;
    int  size;
    char *data;
} mediacontrol_RGBPicture;

/**
 * Playlist sequence
 * A simple list of strings.
 */
typedef struct {
    int size;
    char **data;
} mediacontrol_PlaylistSeq;

typedef struct {
    int code;
    char *message;
} mediacontrol_Exception;

/**
 * Exception codes
 */
#define mediacontrol_PositionKeyNotSupported    1
#define mediacontrol_PositionOriginNotSupported 2
#define mediacontrol_InvalidPosition            3
#define mediacontrol_PlaylistException          4
#define mediacontrol_InternalException          5

/**
 * Stream information
 * This structure allows to quickly get various informations about the stream.
 */
typedef struct {
    mediacontrol_PlayerStatus streamstatus;
    char *url;         /* The URL of the current media stream */
    vlc_int64_t position;  /* actual location in the stream (in ms) */
    vlc_int64_t length;    /* total length of the stream (in ms) */
} mediacontrol_StreamInformation;


# ifdef __cplusplus
}
# endif

#endif

/** @} */