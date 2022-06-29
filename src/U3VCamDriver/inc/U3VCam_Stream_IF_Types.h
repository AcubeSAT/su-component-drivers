//
// Created by fomarko on 17/02/22.
//

#ifndef COMPONENT_DRIVERS_U3VCAM_STREAM_IF_TYPES_H
#define COMPONENT_DRIVERS_U3VCAM_STREAM_IF_TYPES_H


#include "stdint.h"


#ifdef __cplusplus
extern "C" {
#endif



/********************************************************
* Macro definitions
*********************************************************/

#define U3V_STREAM_PLD_TYPE_IMAGE                   0x0001
#define U3V_STREAM_PLD_TYPE_IMAGE_EXTENDED_CHUNK    0x4001
#define U3V_STREAM_PLD_TYPE_CHUNK                   0x4000


/********************************************************
 * Type definitions
 *********************************************************/

#pragma pack(push, 1)

typedef struct 
{
    uint32_t        magicKey;           /* "U3VL" for Leader / "U3VT" for Trailer */
	uint16_t        reserved0;          /* Set 0 on Tx, ignore on Rx */
	uint16_t        size;
	uint64_t        blockID;
    void            *data;
} T_U3VSiGenericPacket;

typedef struct
{
    uint32_t        magicKey;           /* "U3VL" for Leader */
	uint16_t        reserved0;          /* Set 0 on Tx, ignore on Rx */
	uint16_t        leaderSize;
	uint64_t        blockID;
	uint16_t        reserved1;          /* Set 0 on Tx, ignore on Rx */
	uint16_t        payloadType;        /* 0x0001 for Image */
    uint64_t        timestamp;
    uint32_t        pixelFormat;
    uint32_t        sizeX;
    uint32_t        sizeY;
    uint32_t        offsetX;
    uint32_t        offsetY;
    uint16_t        paddingX;
    uint16_t        reserved2;          /* Set 0 on Tx, ignore on Rx */
} T_U3VStrmIfImageLeader;

// typedef T_U3VStrmIfImageLeader T_U3VSiImageExtChunkLeader;  /* payloadType = 0x4001 for Image Extended Chunk */

typedef struct
{
    uint32_t        magicKey;           /* "U3VL" for Leader */
	uint16_t        reserved0;          /* Set 0 on Tx, ignore on Rx */
	uint16_t        leaderSize;
	uint64_t        blockID;
	uint16_t        reserved1;          /* Set 0 on Tx, ignore on Rx */
	uint16_t        payloadType;        /* 0x4000 for Chunk */
    uint64_t        timestamp;
} T_U3VSiChunkLeader;

typedef struct
{
    uint32_t        magicKey;           /* "U3VT" for Trailer */
    uint16_t        reserved0;          /* Set 0 on Tx, ignore on Rx */
    uint16_t        trailerSize;
    uint64_t        blockID;
    uint16_t        status;
    uint16_t        reserved1;          /* Set 0 on Tx, ignore on Rx */
    uint64_t        validPayloadSize;
    uint32_t        sizeY;
} T_U3VStrmIfImageTrailer;

// typedef struct
// {
//     uint32_t        magicKey;           /* "U3VT" for Trailer */
//     uint16_t        reserved0;          /* Set 0 on Tx, ignore on Rx */
//     uint16_t        trailerSize;
//     uint64_t        blockID;
//     uint16_t        status;
//     uint16_t        reserved1;          /* Set 0 on Tx, ignore on Rx */
//     uint64_t        validPayloadSize;
//     uint32_t        sizeY;
//     uint32_t        chunkLayoutID;
// } T_U3VSiImageExtChunkTrailer;

// typedef struct
// {
//     uint32_t        magicKey;           /* "U3VT" for Trailer */
//     uint16_t        reserved0;          /* Set 0 on Tx, ignore on Rx */
//     uint16_t        trailerSize;
//     uint64_t        blockID;
//     uint16_t        status;
//     uint16_t        reserved1;          /* Set 0 on Tx, ignore on Rx */
//     uint64_t        validPayloadSize;
//     uint32_t        chunkLayoutID;
// } T_U3VSiChunkTrailer;

#pragma pack(pop)



#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_U3VCAM_STREAM_IF_TYPES_H
