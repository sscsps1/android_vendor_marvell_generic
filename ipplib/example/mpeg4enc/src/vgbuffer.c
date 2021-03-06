/***************************************************************************************** 
Copyright (c) 2009, Marvell International Ltd. 
All Rights Reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Marvell nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY MARVELL ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL MARVELL BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************************/

#include "vguser.h"
#include "ippIP.h"

/***************************************************************************
// Name:             videoInitBuffer
// Description:      Initialize the input bitstream buffer for MPEG4 decoder
//
// Input Arguments:
//      pBufInfo  Pointer to decoder input bitstream buffer
//
// Output Arguments:
//      pBufInfo  Pointer to updated decoder bitstream buffer
//
// Returns:
//     IPP_STATUS_NOERR		--		No Error
*****************************************************************************/
IppCodecStatus videoInitBuffer (IppBitstream *pBufInfo)
{
	/*
	// Initialize IppBitstream
	// at least big enough to store 2 frame data for less reload 
	*/
    IPP_MemCalloc(&pBufInfo->pBsBuffer, DATA_BUFFER_SIZE, 4);

	if (pBufInfo->pBsBuffer == NULL) {
		return IPP_STATUS_NOMEM_ERR;
	}

	/*
	//no read data at beginning
	//set current pointer to the end of buffer
	*/

	pBufInfo->pBsCurByte = pBufInfo->pBsBuffer + DATA_BUFFER_SIZE;
	pBufInfo->bsCurBitOffset = 0;
    pBufInfo->bsByteLen = DATA_BUFFER_SIZE;

	return IPP_STATUS_NOERR;
}

/***************************************************************************
// Name:             videoReloadBuffer
// Description:      Reload the input bitstream buffer for MPEG4 decoder
//
// Input Arguments:
//      pBufInfo  Pointer to decoder input bitstream buffer
//		stream    Pointer to the input file stream buffer
//
// Output Arguments:
//      pBufInfo  Pointer to updated decoder bitstream buffer
//
// Returns:
//		IPP_STATUS_NOERR		--		No Error
//		IPP_STATUS_ERR			--      IPP_Fread return 0  		
*****************************************************************************/
IppCodecStatus videoReloadBuffer (IppBitstream *pBufInfo, FILE *stream)
{
	int    offset;	
	int	remain_data;
	
	offset = pBufInfo->pBsCurByte - pBufInfo->pBsBuffer;
    remain_data = pBufInfo->bsByteLen - offset;

	if ( remain_data != 0 ) {
		memcpy ( pBufInfo->pBsBuffer, pBufInfo->pBsCurByte, remain_data );
	}
	
	pBufInfo->bsByteLen = remain_data + IPP_Fread ( pBufInfo->pBsBuffer + remain_data, 1,
		DATA_BUFFER_SIZE - remain_data, stream ); 

	if ( pBufInfo->bsByteLen == remain_data ) {
		return IPP_STATUS_ERR;
	}
	
	pBufInfo->pBsCurByte = pBufInfo->pBsBuffer;	
	
	return IPP_STATUS_NOERR;
}

	
/***************************************************************************
// Name:             videoFreeBuffer
// Description:      Free input bitstream buffer for MPEG4 decoder
//
// Input Arguments:
//      pBufInfo  Pointer to decoder input bitstream buffer
//
// Output Arguments:
//      pBufInfo  Pointer to updated decoder input bitstream buffer
//
// Returns:
//     IPP_STATUS_NOERR		--		No Error
*****************************************************************************/
IppCodecStatus videoFreeBuffer (IppBitstream *pBufInfo)
{
	if ( pBufInfo->pBsBuffer ) {
		IPP_MemFree (&pBufInfo->pBsBuffer);
		pBufInfo->pBsBuffer = NULL;
	}
		
	return IPP_STATUS_NOERR;
}


/***************************************************************************
// Name:             init_cmp_output_buf
// Description:      Initialize the output bit-stream buffer for MPEG4 encoder
//
// Input Arguments:
//      pBufInfo	Pointer to the output bitstream buffer structure of MPEG-4 encoder
//		buffersize  The output bitstream buffer size for MPEG-4 encoder.
//	
// Output Arguments:
//      pBufInfo	Pointer to updated output bitstream buffer structure of 
//					MPEG-4 encoder
// Return: 
//		1: Okay
//		0: Fail
*****************************************************************************/
int init_cmp_output_buf (IppBitstream *pBufInfo, int buffersize)
{
	/*
	// Initialize IppBitstream
	// at least big enough to store 2 frame data for less buffer output number
	*/

    IPP_MemCalloc(&pBufInfo->pBsBuffer, buffersize, 4);

	/*
	//no output data at beginning
	//set current pointer to the start of buffer
	*/
	pBufInfo->pBsCurByte = pBufInfo->pBsBuffer;
	pBufInfo->bsCurBitOffset = 0;
	pBufInfo->bsByteLen  = buffersize;
	
	return (pBufInfo->pBsBuffer != NULL);
}

/***************************************************************************
// Name:             free_cmp_output_buf
// Description:      Free encoder output bit-stream buffer
//
// Input Arguments:
//      pBufInfo	Pointer to output bitstream buffer structure of MPEG-4 encoder
//
// Output Arguments:
//      pBufInfo	Pointer to updated output bitstream buffer structure of MPEG-4 encoder
//
// Return: 
//		0: Okay
//		1: Fail
*****************************************************************************/
int free_cmp_output_buf (IppBitstream *pBufInfo)
{
	if ( pBufInfo->pBsBuffer ) {
		IPP_MemFree (&pBufInfo->pBsBuffer);
		return 0;
	} else {
		return 1;
	}
}


/***************************************************************************
// Name:             output_bitstream
// Description:      Output encoded bitstream
//
// Input Arguments:
//      pBufInfo  Pointer to output bitstream buffer structure of MPEG-4 encoder
//
// Output Arguments:
//      pBufInfo  Pointer to updated output bitstream buffer structure of MPEG-4 encoder
//		stream    Pointer to output file stream buffer
//
// Return: 
//		1: Okay
//		0: Fail
*****************************************************************************/
int output_bitstream (IppBitstream *pBufInfo, FILE *stream)
{
	int    offset;	
	int    realwrote = 1;
	
	offset = pBufInfo->pBsCurByte - pBufInfo->pBsBuffer;

	if(offset !=0){
		realwrote = IPP_Fwrite(pBufInfo->pBsBuffer, 1, offset, stream);
	} else {
		realwrote = 0;
	}

	pBufInfo->pBsCurByte = pBufInfo->pBsBuffer;	
	
	return realwrote;
}

/***************************************************************************
// Name:             LoadExtendPlane
// Description:      Load YUV420 raw data to input buffer
//
// Input Arguments:
//		infile		Pointer to input file stream buffer
//      pPicture	Pointer to input bitstream buffer structure of MPEG-4 encoder
//
// Output Arguments:
//      pPicture	Pointer to updated input bitstream buffer structure of MPEG-4 encoder
//
// Return:  None
*****************************************************************************/
IPPCODECFUN(IppCodecStatus, LoadExtendPlane) (FILE* infile, IppPicture * pPicture)
{
	Ipp8u	*tempPtr1;	
	int j;

	tempPtr1 = pPicture->ppPicPlane[0];
	/* Reload YUV plane */
	for(j = 0; j < pPicture->picHeight; j++) {
		if ( (pPicture->picWidth) != IPP_Fread(tempPtr1,1,pPicture->picWidth,infile) ) {
			return IPP_STATUS_INPUT_ERR;
		}
		tempPtr1 += pPicture->picPlaneStep[0];
	}

	tempPtr1 = pPicture->ppPicPlane[1];
	for(j = 0; j < pPicture->picHeight/2; j++) {
		if ( (pPicture->picWidth/2) != IPP_Fread(tempPtr1,1,pPicture->picWidth/2, infile) ) {
			return IPP_STATUS_INPUT_ERR;
		}		
		tempPtr1 += pPicture->picPlaneStep[1];
	}

	tempPtr1 = pPicture->ppPicPlane[2];

	for(j = 0; j < pPicture->picHeight/2; j++) {
		if ((pPicture->picWidth/2) != IPP_Fread(tempPtr1,1,pPicture->picWidth/2, infile) ) {
			return IPP_STATUS_INPUT_ERR;
		}		
		tempPtr1 += pPicture->picPlaneStep[2];
	}	

	return IPP_STATUS_NOERR;

}

/***************************************************************************
// Name:             insert_eos
// Description:      insert EOS at the end of stream
//
// Input Arguments:
//      pBufInfo  Pointer to output bitstream buffer structure of MPEG-4 decoder
// Output Arguments:
//      pBufInfo  Pointer to updated output bitstream buffer structure of MPEG-4 decoder
//
*****************************************************************************/
void insert_eos (IppBitstream *pBufInfo)
{	
	unsigned int remain_data;

	if ( pBufInfo->bsByteLen + EOS_LEN > DATA_BUFFER_SIZE ) {
		/* no enough space to insert EOS at current position */
		/* reload first */
		remain_data = pBufInfo->pBsBuffer + pBufInfo->bsByteLen - pBufInfo->pBsCurByte;

		if ( remain_data + EOS_LEN > DATA_BUFFER_SIZE ) {
			pBufInfo->pBsBuffer[DATA_BUFFER_SIZE - 4] = EOS1;
			pBufInfo->pBsBuffer[DATA_BUFFER_SIZE - 3] = EOS2;
			pBufInfo->pBsBuffer[DATA_BUFFER_SIZE - 2] = EOS3;
			pBufInfo->pBsBuffer[DATA_BUFFER_SIZE - 1] = (unsigned char)EOS4;
			return;
		}

		memcpy ( pBufInfo->pBsBuffer, pBufInfo->pBsCurByte, remain_data );		
		pBufInfo->pBsCurByte = pBufInfo->pBsBuffer;
		pBufInfo->bsByteLen = remain_data;
	}

	/* insert EOS at end of buffer */
	pBufInfo->pBsBuffer[pBufInfo->bsByteLen + 0] = EOS1;
	pBufInfo->pBsBuffer[pBufInfo->bsByteLen + 1] = EOS2;
	pBufInfo->pBsBuffer[pBufInfo->bsByteLen + 2] = EOS3;
	pBufInfo->pBsBuffer[pBufInfo->bsByteLen + 3] = (unsigned char)EOS4;
		
	pBufInfo->bsByteLen += EOS_LEN;
}


/***********************************************************
// Name:			outfile_frame
// Description:		Write the decoded yuv plane to file, while the outfile is appointed.
//
// Input Parameters:
//		pPicture	Pointer to the output bitstream buffer structure of MPEG-4 decoder
// Output Parameters:
//		outfile		The updated output file stream pointer.
//
// Note:
//   the outfile pointer shouldn't be NULL.
//
*************************************************************/
void outfile_frame (FILE *outfile,IppPicture *pPicture) 
{
	int i,step;
	Ipp8u *pResultY, *pResultU, *pResultV;

	pResultY = pPicture->ppPicPlane[0];
	pResultU = pPicture->ppPicPlane[1];
	pResultV = pPicture->ppPicPlane[2];
	

	step = pPicture->picPlaneStep[0];
	for(i = 0; i<pPicture->picHeight; i++) {
		IPP_Fwrite(pResultY, sizeof( char ), pPicture->picWidth, outfile);
		pResultY += step;			
	}
	
	step = pPicture->picPlaneStep[1];
	for(i = 0; i<pPicture->picHeight/2; i++) {
		IPP_Fwrite(pResultU, sizeof( char ),pPicture->picWidth/2, outfile);
		pResultU += step;
	}
	
	step = pPicture->picPlaneStep[2];
	for(i = 0; i<pPicture->picHeight/2; i++) {
		IPP_Fwrite(pResultV, sizeof( char ), pPicture->picWidth/2, outfile);
		pResultV += step;
	}
	
}
