//
// Created by mojo on 17/02/22.
//

#ifndef COMPONENT_DRIVERS_USBCAMDRV_U3V_STREAMING_IF_H
#define COMPONENT_DRIVERS_USBCAMDRV_U3V_STREAMING_IF_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "UsbCamDrv_DeviceClassSpec_U3V.h"
#include "UsbCamDrv_Config.h"

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
} T_U3VSiImageLeader;


// typedef T_U3VSiImageLeader T_U3VSiImageExtChunkLeader;  /* payloadType = 0x4001 for Image Extended Chunk */

// typedef struct
// {
//     uint32_t        magicKey;           /* "U3VL" for Leader */
// 	uint16_t        reserved0;          /* Set 0 on Tx, ignore on Rx */
// 	uint16_t        leaderSize;
// 	uint64_t        blockID;
// 	uint16_t        reserved1;          /* Set 0 on Tx, ignore on Rx */
// 	uint16_t        payloadType;        /* 0x4000 for Chunk */
//     uint64_t        timestamp;
// } T_U3VSiChunkLeader;


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
} T_U3VSiImageTrailer;

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

typedef struct 
{
	uint64_t imageSize;
	uint64_t chunkDataSize;
	uint32_t maxBlockSize;
	uint32_t blockPadding;
	uint64_t blockSize;
	uint32_t maxLeaderSize;
	uint32_t maxTrailerSize;
} T_U3VStreamConfig;

typedef enum
{
    IMG_PLD_BFR_READY_TO_WRITE,
    IMG_PLD_BFR_WRITE_STATE_ACTV,
    IMG_PLD_BFR_READY_TO_READ,
    IMG_PLD_BFR_READ_STATE_ACTV,
} T_U3VImgPayldBfrState;

typedef enum
{
    SI_IMG_TRANSF_STATE_START,
    SI_IMG_TRANSF_STATE_LEADER_COMPLETE,
    SI_IMG_TRANSF_STATE_PAYLOAD_BLOCKS_COMPLETE,
    SI_IMG_TRANSF_STATE_TRAILER_COMPLETE,
    SI_IMG_TRANSF_STATE_ERROR
} T_U3VImgPayldTransfState;

typedef struct
{
    T_U3VImgPayldTransfState    imgPldTransfSt;
    // T_U3VSiImageLeader          siLeader;
    // T_U3VSiImageTrailer         siTrailer;
    uint8_t                     imgPldBfr1[U3V_IN_BUFFER_MAX_SIZE];
    T_U3VImgPayldBfrState       imgPldBfr1St;
    // uint8_t                     imgPldBfr2[U3V_IN_BUFFER_MAX_SIZE];
    // T_U3VImgPayldBfrState       imgPldBfr2St;
} T_U3VImgPayloadContainer;


/********************************************************
* Constant declarations
*********************************************************/


/********************************************************
* Variable declarations
*********************************************************/


/********************************************************
* Function declarations
*********************************************************/



#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_USBCAMDRV_U3V_STREAMING_IF_H
