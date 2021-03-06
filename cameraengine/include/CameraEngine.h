/******************************************************************************
//(C) Copyright [2010 - 2011] Marvell International Ltd.
//All Rights Reserved
******************************************************************************/

#ifndef _CAM_ENGINE_H_
#define _CAM_ENGINE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Camera Engine Version Info */
#define CAM_ENGINE_VERSION_MAJOR  3
#define CAM_ENGINE_VERSION_MINOR  0

typedef char                CAM_Int8s;
typedef unsigned char       CAM_Int8u;
typedef short               CAM_Int16s;
typedef unsigned short      CAM_Int16u;
typedef int                 CAM_Int32s;
typedef unsigned int        CAM_Int32u;
typedef long long           CAM_Int64s;
typedef unsigned long long  CAM_Int64u;
typedef long long           CAM_Tick;
typedef int                 CAM_Bool;

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif

#define CAM_TRUE                    (CAM_Bool)(1)
#define CAM_FALSE                   (CAM_Bool)(0)

#define CAM_STATISTIC_DISABLE       ((void*)(0xffffffff))

#define CAM_PORT_NONE               (-1)
#define CAM_PORT_PREVIEW            (0)
#define CAM_PORT_VIDEO              (1)
#define CAM_PORT_STILL              (2)
#define CAM_PORT_SNAPSHOT           (3)
#define CAM_PORT_ANY                (-2)

#define CAM_MAX_PORT_BUF_CNT             20
#define CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT 10
#define CAM_MAX_SUPPORT_IMAGE_SIZE_CNT   10

#define CAM_MAX_SUPPORT_CAMERA           4

/* normalized image size, in usage models likes face detection where needs to report facing cooridinate in the frame
 * we will normalize ROI according to CAM_NORMALIZED_FRAME_WIDTH and CAM_NORMALIZED_FRAME_HEIGHT.
 * the frame rectangle will be from top-left ( -CAM_NORMALIZED_FRAME_WIDTH / 2, -CAM_NORMALIZED_FRAME_HEIGHT / 2 )
 * to right-bottom ( CAM_NORMALIZED_FRAME_WIDTH / 2, CAM_NORMALIZED_FRAME_HEIGHT / 2 ).
 * eg, if we use a 2000 * 2000 reference frame, the top-left coordinate should be ( -1000, -1000 ),
 * and the bottom-right coordinate should be ( 1000, 1000 ).
 */
#define CAM_NORMALIZED_FRAME_WIDTH       2000
#define CAM_NORMALIZED_FRAME_HEIGHT      2000

typedef enum
{
	CAM_CAPTURESTATE_NULL            = 1,  // camera engine handle is created but no sensor is connected
	CAM_CAPTURESTATE_STANDBY         = 2,  // reserved for low power state
	CAM_CAPTURESTATE_IDLE            = 4,  // sensor is connected
	CAM_CAPTURESTATE_PREVIEW         = 8,
	CAM_CAPTURESTATE_VIDEO           = 16,
	CAM_CAPTURESTATE_STILL           = 32,
	CAM_CAPTURESTATE_ANY             = 0x3f,

	CAM_CAPTURESTATE_LIMIT           = 0x7fffffff,
} CAM_CaptureState;

typedef enum
{
	CAM_ERROR_NONE                 = 0,

	/* warnings */
	CAM_WARN_STARTCODE             = 1000, // start code of warnings, only a indicator

	CAM_WARN_DUPLICATEBUFFER       = 1001, // enqueue a same buffer

	/* errors */
	// user input error
	CAM_ERROR_STARTCODE            = 3000, // start code of errors, only a indicator

	CAM_ERROR_BADPOINTER           = 3001, // the input pointer is bad( NULL )

	CAM_ERROR_BADARGUMENT          = 3002, // the input argument is bad

	CAM_ERROR_BADSENSORID          = 3003, // the input sensor id is invalid

	CAM_ERROR_BADPORTID            = 3004, // the input port id is bad

	CAM_ERROR_PORTNOTVALID         = 3005, // the port's parameters are not valid / consistent

	CAM_ERROR_PORTNOTCONFIGUREABLE = 3006, // the port is not configurable (due to not inactive or reside with buffers)

	CAM_ERROR_BADBUFFER            = 3007, // the buffer doesn't meet the buffer requirement of the port

	CAM_ERROR_BADSTATETRANSITION   = 3008, // the requested state transition is not allowed

	CAM_ERROR_STATELIMIT           = 3009, // operation not allowed due to the limitation of the current camera state

	CAM_ERROR_SHOTMODELIMIT        = 3010, // operation not allowed due to the limitation of the current photo mode

	// implementation limitation
	CAM_ERROR_NOTSUPPORTEDCMD      = 3011, // the command is not supported

	CAM_ERROR_NOTSUPPORTEDARG      = 3012, // the argument is not supported

	CAM_ERROR_NOTIMPLEMENTED       = 3013, // the feature is planned to be supported, but not yet implemented

	// execution error
	CAM_ERROR_NOTENOUGHBUFFERS     = 3014, // no enough number of buffers has been enqueued

	CAM_ERROR_BUFFERUNAVAILABLE    = 3015, // dequeue can't return a valid buffer because pre-emptied by another command

	CAM_ERROR_PORTNOTACTIVE        = 3016, // dequeue from an inactive port

	CAM_ERROR_OUTOFMEMORY          = 3017, // memory allocation failed

	CAM_ERROR_OUTOFRESOURCE        = 3018, // usually caused by too much eneuque

	CAM_ERROR_TIMEOUT              = 3019, // usually happened in dequeue

	CAM_ERROR_DRIVEROPFAILED       = 3020, // open/close/ioctl return error

	CAM_ERROR_PPUFAILED            = 3021, // post-processing failed

	CAM_ERROR_FATALERROR           = 3022, // when fatal happened, camera engine must be set back to idle to clear this error.
	                                       // FATAL error may be caused by an unrecoverable device failure.

	CAM_ERROR_LIMIT                = 0x7fffffff,

} CAM_Error;


/*------------------------------------------------------------------------------
--  Commands for Camera Engine (the application adaptation layer)
------------------------------------------------------------------------------*/
typedef enum
{
	////////////////////////////////////////////////////////////////////////////////
	//  Command                                 Parameter
	//                                          Comments
	////////////////////////////////////////////////////////////////////////////////
	// camera initializtion
	CAM_CMD_ENUM_SENSORS,                // [OUT] CAM_Int32s     *pCameraCount
	                                     // [OUT] CAM_DeviceProp stCameraProp[CAM_MAX_SUPPORT_CAMERA]

	CAM_CMD_QUERY_CAMERA_CAP,            // [IN]  CAM_Int32s iSensorID
	                                     // [OUT] CAM_CameraCapability *pCapability

	CAM_CMD_SET_EVENTHANDLER,            // [IN]  CAM_EventHandler fnEventHandler
	                                     // [OUT] void *pUserData

	CAM_CMD_SET_SENSOR_ID,               // [IN]  CAM_Int32s iSensorID
	                                     // [UNUSED]

	CAM_CMD_GET_SENSOR_ID,               // [OUT] CAM_Int32s *pSensorID
	                                     // [UNUSED]

	CAM_CMD_SET_SENSOR_ORIENTATION,      // [IN]  CAM_FlipRotate eFlipRotate
	                                     // [UNUSED]

	CAM_CMD_GET_SENSOR_ORIENTATION,      // [OUT] CAM_FlipRotate *pFlipRotate
	                                     // [UNUSED]

	// state machine
	CAM_CMD_SET_STATE,                   // [IN]  CAM_CaptureState eState
	                                     // [UNUSED]

	CAM_CMD_GET_STATE,                   // [OUT] CAM_CaptureState *pState
	                                     // [UNUSED]

	// buffer management
	CAM_CMD_PORT_GET_BUFREQ,             // [OUT CAM_ImageBufferReq *pBufReq
	                                     // [UNUSED]
	                                     // get buffer requirement for streaming

	CAM_CMD_PORT_ENQUEUE_BUF,            // [IN] CAM_Int32s iPortId
	                                     // [IN] CAM_ImageBuffer *pImgBuf
	                                     // enqueue to empty buffer queue.
	                                     // and implicitly register the buffer if
	                                     // the buffer is enqueued for the first time

	CAM_CMD_PORT_DEQUEUE_BUF,           // [IN/OUT] CAM_Int32s *pPortId
	                                    // [OUT] CAM_ImageBuffer **ppImgBuf
	                                    // dequeue from filled buffer queue.
	                                    // Note that this is a blocking call.

	CAM_CMD_PORT_FLUSH_BUF,             // [IN]  CAM_Int32s iPortId
	                                    // release currently holded buffers and
	                                    // implicitly unregister all buffers that has
	                                    // ever been enqueued to the port

	// port configuration
	CAM_CMD_PORT_SET_SIZE,              // [IN]  CAM_Int32s iPortId
	                                    // [IN]  CAM_Size *sPreviewSize

	CAM_CMD_PORT_GET_SIZE,              // [IN]  CAM_Int32s iPortId
	                                    // [OUT] CAM_Size *sPreviewSize

	CAM_CMD_PORT_SET_FORMAT,            // [IN]  CAM_Int32s iPortId
	                                    // [IN]  CAM_ImageFormat eFormat

	CAM_CMD_PORT_GET_FORMAT,            // [IN]  CAM_Int32s iPortId
	                                    // [OUT] CAM_ImageFormat *pFormat

	CAM_CMD_PORT_SET_ROTATION,          // [IN]  CAM_Int32s iPortId
	                                    // [IN]  CAM_FlipRotate eFlipRotate

	CAM_CMD_PORT_GET_ROTATION,          // [IN]  CAM_Int32s iPortId
	                                    // [OUT] CAM_FlipRotate *pFlipRotate

	// JPEG parameters
	CAM_CMD_SET_JPEGPARAM,              // [IN]  CAM_JpegParam *pJpegparam
	                                    // [UNUSED]

	CAM_CMD_GET_JPEGPARAM,              // [OUT] CAM_JpegParam *pJpegParam
	                                    // [UNUSED]

	// frame rate
	CAM_CMD_SET_FRAMERATE,              // [IN]  CAM_Int32s iFpsQ16
	                                    // [UNUSED]

	CAM_CMD_GET_FRAMERATE,              // [OUT] CAM_Int32s *pFpsQ16
	                                    // [UNUSED]

	// preview aspect ratio
	CAM_CMD_SET_PREVIEW_RESIZEFAVOR,    // [IN]  CAM_Bool bAspectRatioOverFOV
	                                    // [UNUSED]

	CAM_CMD_GET_PREVIEW_RESIZEFAVOR,    // [IN]  CAM_Bool *pAspectRatioOverFOV
	                                    // [UNUSED]


	// shooting parameter: restricted by shot mode
	// shot mode
	CAM_CMD_QUERY_SHOTMODE_CAP,         // [IN]  CAM_ShotMode eShotMode
	                                    // [OUT] CAM_ShotModeCapability *pCapability

	CAM_CMD_SET_SHOTMODE,               // [IN]  CAM_ShotMode eShotMode
	                                    // [UNUSED]

	CAM_CMD_GET_SHOTMODE,               // [OUT] CAM_ShotMode *pShotMode
	                                    // [UNUSED]

	// video sub-mode
	CAM_CMD_SET_VIDEO_SUBMODE,          // [IN]  CAM_VideoSubMode eVideoSubMode
	                                    // [IN]  void*, sub mode parameters

	CAM_CMD_GET_VIDEO_SUBMODE,          // [OUT] CAM_VideoSubMode *pVideoSubMode
	                                    // [UNUSED]

	// still sub-mode
	CAM_CMD_SET_STILL_SUBMODE,          // [IN]  CAM_StillSubMode eStillSubMode
	                                    // [IN]  CAM_StillSubModeParam*, sub mode parameters
	                                    // if set SIMPLE mode, no need to input the
	                                    // second parameter

	CAM_CMD_GET_STILL_SUBMODE,          // [OUT] CAM_VideoSubMode *pStillSubMode
	                                    // [OUT] CAM_StillSubModeParam *pStillSubModeParam
	                                    // if given the second parameter, will output still
	                                    // sub mode parameters; otherwise only the sub mode
	                                    // will be delivered

	// exposure mode
	CAM_CMD_SET_EXPOSUREMODE,           // [IN]  CAM_ExposureMode eExposureMode
	                                    // [UNUSED]

	CAM_CMD_GET_EXPOSUREMODE,           // [OUT] CAM_ExposureMode *pExposureMode
	                                    // [UNUSED]

	// exposure metering mode
	CAM_CMD_SET_EXPOSUREMETERMODE,      // [IN]  CAM_ExposureMeterMode eExposureMeter
	                                    // [IN]  CAM_MultiROI*, ROI coordinate.
	                                    // second parameter is valid only under CAM_EXPOSUREMETERMODE_MANUAL mode,
	                                    // otherwise no need to input second parameter.

	CAM_CMD_GET_EXPOSUREMETERMODE,      // [OUT] CAM_ExposureMeterMode *pExposureMeter
	                                    // [UNUSED]

	// EV compensation
	CAM_CMD_SET_EVCOMPENSATION,         // [IN]  CAM_Int32s iEvCompensationQ16
	                                    // [UNUSED]

	CAM_CMD_GET_EVCOMPENSATION,         // [OUT] CAM_Int32s *pEvCompensationQ16
	                                    // [UNUSED]

	// ISO
	CAM_CMD_SET_ISO,                    // [IN]  CAM_ISOMode eISO
	                                    // [UNUSED]

	CAM_CMD_GET_ISO,                    // [OUT] CAM_ISOMode *pISO
	                                    // [UNUSED]

	CAM_CMD_SET_SHUTTERSPEED,           // [IN]  CAM_Int32s iShutterSpeedQ16
	                                    // [UNUSED]

	CAM_CMD_GET_SHUTTERSPEED,           // [OUT] CAM_Int32s *pShutterSpeedQ16
	                                    // [UNUSED]

	CAM_CMD_SET_FNUM,                   // [IN] CAM_Int32s iFNumQ16
	                                    // [UNUSED]

	CAM_CMD_GET_FNUM,                   // [OUT] CAM_Int32s *pFNumQ16
	                                    // [UNUSED]

	CAM_CMD_SET_BANDFILTER,             // [IN]  CAM_BandFilterMode eBandFilter
	                                    // [UNUSED]

	CAM_CMD_GET_BANDFILTER,             // [OUT] CAM_BandFilterMode *pBandFilter
	                                    // [UNUSED]

	CAM_CMD_SET_FLASHMODE,              // [IN]  CAM_FlashMode eFlashMode
	                                    // [UNUSED]

	CAM_CMD_GET_FLASHMODE,              // [OUT] CAM_FlashMode *pFlashMode
	                                    // [UNUSED]

	// white balance
	CAM_CMD_SET_WHITEBALANCEMODE,       // [IN]  CAM_WhiteBalanceMode eWhiteBalance
	                                    // [UNUSED]

	CAM_CMD_GET_WHITEBALANCEMODE,       // [OUT] CAM_WhiteBalanceMode *pWhiteBalance
	                                    // [UNUSED]

	// focus
	CAM_CMD_SET_FOCUSMODE,              // [IN]  CAM_FocusMode eFocusMode
	                                    // [UNUSED]

	CAM_CMD_GET_FOCUSMODE,              // [OUT] CAM_FocusMode *pFocusMode
	                                    // [UNUSED]

	CAM_CMD_SET_FOCUSZONE,              // [IN] CAM_FocusZoneMode eFocusZoneMode
	                                    // [IN] CAM_MultiROI      *pFocusROI, only valid while CAM_FOCUSZONE_MANUAL, otherwise please set as NULL

	CAM_CMD_GET_FOCUSZONE,              // [OUT] CAM_FocusZoneMode *pFocusZoneMode
	                                    // [OUT] CAM_MultiROI      *pFocusROI, only valid while CAM_FOCUSZONE_MANUAL, otherwise we not fill this parameter

	CAM_CMD_START_FOCUS,                // [IN]  UNUSED
	                                    // [IN]  UNUSED

	CAM_CMD_CANCEL_FOCUS,               // [UNUSED]
	                                    // [UNUSED]

	// zoom
	CAM_CMD_SET_OPTZOOM,                // [IN]  CAM_Int32s iZoomFactorQ16
	                                    // [UNUSED]

	CAM_CMD_GET_OPTZOOM,                // [IN]  CAM_Int32s *pZoomFactorQ16
	                                    // [UNUSED]

	CAM_CMD_SET_DIGZOOM,                // [IN]  CAM_Int32s iZoomFactorQ16
	                                    // [UNUSED]

	CAM_CMD_GET_DIGZOOM,                // [IN]  CAM_Int32s *pZoomFactorQ16
	                                    // [UNUSED]

	// special effect
	CAM_CMD_SET_COLOREFFECT,            // [IN]  CAM_ColorEffect eColorEffect
	                                    // [UNUSED]

	CAM_CMD_GET_COLOREFFECT,            // [OUT] CAM_ColorEffect *pColorEffect
	                                    // [UNUSED]

	// tone effect
	CAM_CMD_SET_BRIGHTNESS,             // [IN]  CAM_Int32s iBrightness
	                                    // [UNUSED]

	CAM_CMD_GET_BRIGHTNESS,             // [OUT] CAM_Int32s *pBrightness
	                                    // [UNUSED]

	CAM_CMD_SET_CONTRAST,               // [IN]  CAM_Int32s iContrast
	                                    // [UNUSED]

	CAM_CMD_GET_CONTRAST,               // [OUT] CAM_Int32s *pContrast
	                                    // [UNUSED]

	CAM_CMD_SET_SATURATION,             // [IN]  CAM_Int32s iSaturation
	                                    // [UNUSED]

	CAM_CMD_GET_SATURATION,             // [OUT] CAM_Int32s *pSaturation
	                                    // [UNUSED]

	CAM_CMD_SET_SHARPNESS,              // [IN]  CAM_Int32s iSharpness
	                                    // [UNUSED]

	CAM_CMD_GET_SHARPNESS,              // [OUT] CAM_Int32s *pSharpness
	                                    // [UNUSED]

	// face detection
	CAM_CMD_START_FACEDETECTION,        // [UNUSED]
	                                    // [UNUSED]

	CAM_CMD_CANCEL_FACEDETECTION,       // [UNUSED]
	                                    // [UNUSED]

	// robustness interface
	CAM_CMD_RESET_CAMERA,               // [IN] CAM_ResetType eResetType
	                                    // [UNUSED]

	CAM_CMD_LIMIT         = 0x7fffffff,
} CAM_Command;

// camera reset type
typedef enum
{
	CAM_RESET_FAST     = 1,
	CAM_RESET_COMPLETE = 2,

	CAM_RESET_LIMIT    = 0x7fffffff,
} CAM_ResetType;

typedef enum
{
	CAM_IMGFMT_RGGB8      = 1000,
	CAM_IMGFMT_BGGR8      = 1001,
	CAM_IMGFMT_GRBG8      = 1002,
	CAM_IMGFMT_GBRG8      = 1003,
	CAM_IMGFMT_RGGB10     = 1004,
	CAM_IMGFMT_BGGR10     = 1005,
	CAM_IMGFMT_GRBG10     = 1006,
	CAM_IMGFMT_GBRG10     = 1007,

	// low->high  R,G,B
	CAM_IMGFMT_RGB888     = 2000,
	CAM_IMGFMT_RGB444     = 2001,
	CAM_IMGFMT_RGB555     = 2002,
	CAM_IMGFMT_RGB565     = 2003,
	CAM_IMGFMT_RGB666     = 2004,
	CAM_IMGFMT_BGR888     = 2005,
	CAM_IMGFMT_BGR444     = 2006,
	CAM_IMGFMT_BGR555     = 2007,
	CAM_IMGFMT_BGR565     = 2008,
	CAM_IMGFMT_BGR666     = 2009,
	// with alpha channel
	CAM_IMGFMT_BGRA8888   = 2010,
	CAM_IMGFMT_RGBA8888   = 2011,

	CAM_IMGFMT_YCbCr444P  = 3000,
	CAM_IMGFMT_YCbCr444I  = 3001,
	CAM_IMGFMT_YCbCr422P  = 3002,
	CAM_IMGFMT_YCbYCr     = 3003,
	CAM_IMGFMT_CbYCrY     = 3004,
	CAM_IMGFMT_YCrYCb     = 3005,
	CAM_IMGFMT_CrYCbY     = 3006,
	CAM_IMGFMT_YCrCb420P  = 3007, // fourcc: YV12
	CAM_IMGFMT_YCbCr420P  = 3008, // fourcc: IYUV, I420
	CAM_IMGFMT_YCbCr420SP = 3009, // fourcc: NV12
	CAM_IMGFMT_YCrCb420SP = 3010, // fourcc: NV21

	CAM_IMGFMT_JPEG       = 4000,

	CAM_IMGFMT_LIMIT      = 0x7fffffff,
} CAM_ImageFormat;

// flip/rotate
typedef enum
{
	CAM_FLIPROTATE_NORMAL  = 0,
	CAM_FLIPROTATE_90L     = 1,
	CAM_FLIPROTATE_90R     = 2,
	CAM_FLIPROTATE_180     = 3,
	CAM_FLIPROTATE_HFLIP   = 4,  // horizontal flip
	CAM_FLIPROTATE_VFLIP   = 5,  // vertical flip
	CAM_FLIPROTATE_DFLIP   = 6,  // diagonal flip
	CAM_FLIPROTATE_ADFLIP  = 7,  // anti-diagonal flip

	CAM_FLIPROTATE_NUM,

	CAM_FLIPROTATE_LIMIT   = 0x7fffffff,
} CAM_FlipRotate;

// shot mode
typedef enum
{
	CAM_SHOTMODE_AUTO          = 0,
	CAM_SHOTMODE_MANUAL        = 1,

	CAM_SHOTMODE_PORTRAIT      = 2,
	CAM_SHOTMODE_LANDSCAPE     = 3,
	CAM_SHOTMODE_NIGHTPORTRAIT = 4,
	CAM_SHOTMODE_NIGHTSCENE    = 5,
	CAM_SHOTMODE_CHILD         = 6,
	CAM_SHOTMODE_INDOOR        = 7,
	CAM_SHOTMODE_PLANTS        = 8,
	CAM_SHOTMODE_SNOW          = 9,
	CAM_SHOTMODE_BEACH         = 10,
	CAM_SHOTMODE_FIREWORKS     = 11,
	CAM_SHOTMODE_SUBMARINE     = 12,
	CAM_SHOTMODE_WHITEBOARD    = 13,
	CAM_SHOTMODE_SPORTS        = 14,

	CAM_SHOTMODE_NUM,

	CAM_SHOTMODE_LIMIT         = 0x7fffffff,
} CAM_ShotMode;

// video capture sub-mode
typedef enum
{
	CAM_VIDEOSUBMODE_SIMPLE        = 0,
	CAM_VIDEOSUBMODE_STABILIZER    = 1, // Video Stabilizer
	CAM_VIDEOSUBMODE_TNR           = 2, // Temporal Noise Reduction
	CAM_VIDEOSUBMODE_STABILIZEDTNR = 3, // Video Stabilizer + Temporal Noise Reduction

	CAM_VIDEOSUBMODE_NUM,

	CAM_VIDEOSUBMODE_LIMIT         = 0x7fffffff,
} CAM_VideoSubMode;

// still capture sub-mode
typedef enum
{
	CAM_STILLSUBMODE_SIMPLE    = 0,
	CAM_STILLSUBMODE_BURST     = 1, // Burst Capture
	CAM_STILLSUBMODE_ZSL       = 2, // Zero Shutter Lag
	CAM_STILLSUBMODE_BURSTZSL  = 3, // Burst + ZSL
	CAM_STILLSUBMODE_HDR       = 4, // High Dynamic Range

	CAM_STILLSUBMODE_NUM,

	CAM_STILLSUBMODE_LIMIT     = 0x7fffffff,
} CAM_StillSubMode;

// white balance mode
typedef enum
{
	CAM_WHITEBALANCEMODE_AUTO                  = 0,
	CAM_WHITEBALANCEMODE_INCANDESCENT          = 1,
	CAM_WHITEBALANCEMODE_DAYLIGHT_FLUORESCENT  = 2,
	CAM_WHITEBALANCEMODE_DAYWHITE_FLUORESCENT  = 3,
	CAM_WHITEBALANCEMODE_COOLWHITE_FLUORESCENT = 4,
	CAM_WHITEBALANCEMODE_DAYLIGHT              = 5,
	CAM_WHITEBALANCEMODE_CLOUDY                = 6,
	CAM_WHITEBALANCEMODE_SHADOW                = 7,
	CAM_WHITEBALANCEMODE_LOCK                  = 8,

	CAM_WHITEBALANCEMODE_NUM,

	CAM_WHITEBALANCEMODE_LIMIT                 = 0x7fffffff,
} CAM_WhiteBalanceMode;

// color effect
typedef enum
{
	CAM_COLOREFFECT_OFF        = 0,
	CAM_COLOREFFECT_VIVID,
	CAM_COLOREFFECT_SEPIA,
	CAM_COLOREFFECT_GRAYSCALE,
	CAM_COLOREFFECT_NEGATIVE,
	CAM_COLOREFFECT_SOLARIZE,
	CAM_COLOREFFECT_POSTERIZE,
	CAM_COLOREFFECT_AQUA,

	CAM_COLOREFFECT_NUM,

	CAM_COLOREFFECT_LIMIT      = 0x7fffffff,
} CAM_ColorEffect;

// exposure mode
typedef enum
{
	CAM_EXPOSUREMODE_AUTO              = 0,
	CAM_EXPOSUREMODE_APERTUREPRIOR,
	CAM_EXPOSUREMODE_SHUTTERPRIOR,
	CAM_EXPOSUREMODE_PROGRAM,
	CAM_EXPOSUREMODE_MANUAL,

	CAM_EXPOSUREMODE_NUM,

	CAM_EXPOSUREMODE_LIMIT             = 0x7fffffff,
}CAM_ExposureMode;

// exposure metering mode
typedef enum
{
	CAM_EXPOSUREMETERMODE_AUTO            = 0,
	CAM_EXPOSUREMETERMODE_MEAN,
	CAM_EXPOSUREMETERMODE_CENTRALWEIGHTED,
	CAM_EXPOSUREMETERMODE_SPOT,
	CAM_EXPOSUREMETERMODE_MATRIX,
	CAM_EXPOSUREMETERMODE_MANUAL,

	CAM_EXPOSUREMETERMODE_NUM,

	CAM_EXPOSUREMETERMODE_LIMIT           = 0x7fffffff,
} CAM_ExposureMeterMode;

// ISO speed
typedef enum
{
	CAM_ISO_AUTO  = 0,
	CAM_ISO_50    = 1,
	CAM_ISO_100   = 2,
	CAM_ISO_200   = 3,
	CAM_ISO_400   = 4,
	CAM_ISO_800   = 5,
	CAM_ISO_1600  = 6,
	CAM_ISO_3200  = 7,

	CAM_ISO_NUM,

	CAM_ISO_LIMIT = 0x7fffffff,
} CAM_ISOMode;

// shutter type
typedef enum
{
	CAM_SHUTTERTYPE_ROLLING    = 0,
	CAM_SHUTTERTYPE_MECHANICAL = 1,

	CAM_SHUTTERTYPE_NUM,

	CAM_SHUTTERTYPE_LIMIT      = 0x7fffffff,
} CAM_ShutterType;

// only electronic rolling shutter needs anti-flicker
typedef enum
{
	CAM_BANDFILTER_AUTO  = 0,
	CAM_BANDFILTER_OFF   = 1,
	CAM_BANDFILTER_50HZ  = 2,
	CAM_BANDFILTER_60HZ  = 3,

	CAM_BANDFILTER_NUM,

	CAM_BANDFILTER_LIMIT = 0x7fffffff,
} CAM_BandFilterMode;

// actuator type
typedef enum
{
	CAM_ACTUATOR_FIXED,
	CAM_ACTUATOR_VCM,
	CAM_ACTUATOR_PIEZO,
	CAM_ACTUATOR_LIQUID,
	CAM_ACTUATOR_EDOF,

	CAM_ACTUATOR_NUM,

	CAM_ACTUATOR_LIMIT = 0x7fffffff,
} CAM_ActuatorType;

// focus mode
typedef enum
{
	CAM_FOCUS_AUTO_ONESHOT            = 0x0,  // need CAM_CMD_START_FOCUS trigger focus
	CAM_FOCUS_AUTO_CONTINUOUS_VIDEO   = 0x2,
	CAM_FOCUS_AUTO_CONTINUOUS_PICTURE = 0x3,  // use CAM_CMD_START_FOCUS to pasue CAF, CAM_CMD_CANCEL_FOCUS to resume CAF
	CAM_FOCUS_SUPERMACRO              = 0x4,
	CAM_FOCUS_MACRO                   = 0x5,  // need CAM_CMD_START_FOCUS trigger focus
	CAM_FOCUS_HYPERFOCAL              = 0x6,
	CAM_FOCUS_INFINITY                = 0x7,
	CAM_FOCUS_MANUAL                  = 0x8,

	CAM_FOCUS_NUM,

	CAM_FOCUS_LIMIT                  = 0x7fffffff,
} CAM_FocusMode;

// focus zone
typedef enum
{
	CAM_FOCUSZONEMODE_CENTER     = 0x0,
	CAM_FOCUSZONEMODE_MULTIZONE1 = 0x1,
	CAM_FOCUSZONEMODE_MULTIZONE2 = 0x2,
	CAM_FOCUSZONEMODE_MULTIZONE3 = 0x3,
	CAM_FOCUSZONEMODE_MANUAL     = 0x4,

	CAM_FOCUSZONEMODE_NUM,

	CAM_FOCUSZONEMODE_LIMIT      = 0x7fffffff,
} CAM_FocusZoneMode;

// focus state
typedef enum
{
	CAM_FOCUSSTATE_PERSIST    = 0x1,
	CAM_FOCUSSTATE_INFOCUS    = 0x2,
	CAM_FOCUSSTATE_OUTOFFOCUS = 0x3,
	CAM_FOCUSSTATE_FAIL       = 0x4,

	CAM_FOCUSSTATE_NUM,

	CAM_FOCUSSTATE_LIMIT      = 0x7fffffff,
}CAM_FocusState;

// flash type
typedef enum
{
	CAM_FLASHTYPE_NONE  = 0x0,
	CAM_FLASHTYPE_LED   = 0x1,
	CAM_FLASHTYPE_XENON = 0x2,

	CAM_FLASHTYPE_NUM,

	CAM_FLASHTYPE_LIMIT = 0x7fffffff,
} CAM_FlashType;

// flash mode
typedef enum
{
	CAM_FLASH_AUTO   = 0x0,
	CAM_FLASH_OFF    = 0x1,
	CAM_FLASH_ON     = 0x2,
	CAM_FLASH_TORCH  = 0x3,
	CAM_FLASH_REDEYE = 0x4,

	CAM_FLASH_NUM,

	CAM_FLASH_LIMIT  = 0x7fffffff,
} CAM_FlashMode;


// Events
typedef enum
{
	CAM_EVENT_FRAME_DROPPED        = 0x0,  // param - port id

	CAM_EVENT_FRAME_READY          = 0x1,  // param - port id

	CAM_EVENT_IN_FOCUS             = 0x2,

	CAM_EVENT_OUTOF_FOCUS          = 0x3,

	CAM_EVENT_FOCUS_AUTO_STOP      = 0x4,  // param - pointer to focus status (CAM_Bool), which indicate if the auto focus succeeded

	CAM_EVENT_STILL_SHUTTERCLOSED  = 0x5,

	CAM_EVENT_STILL_ALLPROCESSED   = 0x6,

	CAM_EVENT_FACE_UPDATE          = 0x7,

	CAM_EVENT_FATALERROR           = 0x8,

	CAM_EVENT_NUM,

	CAM_EVENT_LIMIT                = 0x7fffffff,
} CAM_EventId;

#define CAM_SENSOR_FACING_BACK     0
#define CAM_SENSOR_FACING_FRONT    1

#define CAM_OUTPUT_SINGLESTREAM    0
#define CAM_OUTPUT_DUALSTREAM      1

typedef struct
{
	CAM_Int8s       sName[32];
	CAM_FlipRotate  eInstallOrientation;  // means the sensor scan sequenace compared to the LCD scan sequence
	CAM_Int32s      iFacing;
	CAM_Int32s      iOutputProp;
} CAM_DeviceProp;

typedef struct
{
	CAM_Int32s  iWidth;
	CAM_Int32s  iHeight;
} CAM_Size;

typedef struct
{
	CAM_Int32s  iLeft;              // left boundary, the boundary itself is included in the rectangle
	CAM_Int32s  iTop;               // top boundary, the boundary itself is included in the rectangle
	CAM_Int32s  iWidth;             // width of the rect
	CAM_Int32s  iHeight;            // height of the rect
} CAM_Rect;

typedef struct
{
	CAM_Rect    stRect;
	CAM_Int32s  iWeight;
} CAM_WeightedRect;

#define CAM_MAX_ROI_CNT   5
typedef struct
{
	CAM_Int32s       iROICnt;
	CAM_WeightedRect stWeiRect[CAM_MAX_ROI_CNT];
} CAM_MultiROI;

// JPEG Parameter
typedef struct
{
	CAM_Int32s iSampling;      // 0 - 420, 1 - 422, 2 - 444
	CAM_Int32s iQualityFactor;
	CAM_Bool   bEnableExif;
	CAM_Bool   bEnableThumb;
	CAM_Int32s iThumbWidth;
	CAM_Int32s iThumbHeight;
} CAM_JpegParam;

#define BUF_FLAG_PHYSICALCONTIGUOUS             0x01
#define BUF_FLAG_NONPHYSICALCONTIGUOUS          0x02
#define BUF_FLAG_L1CACHEABLE                    0x04
#define BUF_FLAG_L1NONCACHEABLE                 0x08
#define BUF_FLAG_L2CACHEABLE                    0x10
#define BUF_FLAG_L2NONCACHEABLE                 0x20
#define BUF_FLAG_BUFFERABLE                     0x40
#define BUF_FLAG_UNBUFFERABLE                   0x80

#define BUF_FLAG_YUVPLANER                      0x100
#define BUF_FLAG_YUVBACKTOBACK                  0x200
#define BUF_FLAG_YVUBACKTOBACK                  0x400

#define BUF_FLAG_ALLOWPADDING                   0x800
#define BUF_FLAG_FORBIDPADDING                  0x1000

#define BUF_FLAG_ALL                            0x1fff

// shooting informations of frame, currently for EXIF
typedef struct
{
	CAM_Int32s              iExposureTimeQ16;   // uint: s
	CAM_Int32s              iFNumQ16;
	CAM_ExposureMode        eExposureMode;
	CAM_ExposureMeterMode   eExpMeterMode;
	CAM_Int32s              iISOSpeed;
	CAM_Int32s              iSubjectDistQ16;   // unit: m
	CAM_Bool                bIsFlashOn;        // flag indicates that flash is fired or not
	CAM_Int32s              iFocalLenQ16;
	CAM_Int32s              iFocalLen35mm;
} CAM_ShotInfo;

// image buffer header
typedef struct
{
	CAM_Int16u              usAGain[4];
	CAM_Int16u              usDGain[4];
	CAM_Int16u              usIspGain;
	CAM_Int8u               ucAWBPreset;
	CAM_Int16u              usWbRedScale;
	CAM_Int16u              usWbBlueScale;
} CAM_RawShotInfo;

// image buffer header
typedef struct
{
	CAM_ImageFormat         eFormat;
	CAM_Int32s              iWidth;
	CAM_Int32s              iHeight;
	CAM_Int32s              iStep[3];       // given by external before enqueue
	CAM_Int32s              iAllocLen[3];   // given by external before enqueue
	CAM_Int32s              iFilledLen[3];
	CAM_Int32s              iOffset[3];     // given by external before enqueue
	CAM_Int8u               *pBuffer[3];    // given by external before enqueue, virtual address
	CAM_Int32u              iPhyAddr[3];    // given by external before enqueue, physical address of pBuffer[]
	CAM_Tick                tTick;          // the interval from device boot to this frame ready, unit: ns
	CAM_Int32u              iFlag;          // given by external before enqueue
	CAM_Int32s              iUserIndex;     // given by external before enqueue
	void                    *pUserData;     // given by external before enqueue
	CAM_Int32s              iPrivateIndex;
	void                    *pPrivateData;

	CAM_Bool                bEnableShotInfo;
	CAM_ShotInfo            stShotInfo;
	CAM_RawShotInfo         stRawShotInfo;
} CAM_ImageBuffer;

// buffer requirement
typedef struct
{
	CAM_ImageFormat         eFormat;
	CAM_Int32s              iWidth;         // image width in pixels
	CAM_Int32s              iHeight;        // image height in pixels
	CAM_Int32s              iMinStep[3];    // image buffer's row stride in bytes
	CAM_Int32s              iMinBufLen[3];
	CAM_Int32s              iMinBufCount;
	CAM_Int32s              iAlignment[3];  // alignment of each buffer start address
	CAM_Int32s              iRowAlign[3];   // alignment of each row

	CAM_Int32u              iFlagOptimal;
	CAM_Int32u              iFlagAcceptable;
} CAM_ImageBufferReq;

// still submode parameter, hdr parameter temporarily not included
typedef struct
{
	CAM_Int32s              iBurstCnt;
	CAM_Int32s              iZslDepth;
} CAM_StillSubModeParam;

// JPEG Encoder Capability
typedef struct
{
	CAM_Bool   bSupportJpeg;
	CAM_Int32s iMinQualityFactor;
	CAM_Int32s iMaxQualityFactor;
	CAM_Bool   bSupportExif;
	CAM_Int8s  sExifVersion[8];
	CAM_Bool   bSupportThumb;
	CAM_Size   stMinThumbSize;
	CAM_Size   stMaxThumbSize;
} CAM_JpegCapability;

// port capability
typedef struct
{
	CAM_Bool        bArbitrarySize;
	CAM_Size        stMin;
	CAM_Size        stMax;

	CAM_Int32s      iSupportedSizeCnt;
	CAM_Size        stSupportedSize[CAM_MAX_SUPPORT_IMAGE_SIZE_CNT];

	CAM_Int32s      iSupportedFormatCnt;
	CAM_ImageFormat eSupportedFormat[CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT];

	CAM_Int32s      iSupportedRotateCnt;
	CAM_FlipRotate  eSupportedRotate[CAM_FLIPROTATE_NUM];
} CAM_PortCapability;

// camera capability
typedef struct
{
	CAM_Int32s             iSupportedExpModeCnt;
	CAM_ExposureMode       eSupportedExpMode[CAM_EXPOSUREMODE_NUM];

	CAM_Int32s             iSupportedExpMeterCnt;
	CAM_ExposureMeterMode  eSupportedExpMeter[CAM_EXPOSUREMETERMODE_NUM];

	CAM_Int32s             iMinEVCompQ16;
	CAM_Int32s             iMaxEVCompQ16;
	CAM_Int32s             iEVCompStepQ16;  // if support continuous ev compensation, pls set "iEVCompStepQ16" to zero

	CAM_Int32s             iSupportedIsoModeCnt;
	CAM_ISOMode            eSupportedIsoMode[CAM_ISO_NUM];

	CAM_Int32s             iMinShutSpdQ16;
	CAM_Int32s             iMaxShutSpdQ16;

	CAM_Int32s             iMinFNumQ16;
	CAM_Int32s             iMaxFNumQ16;

	CAM_Int32s             iSupportedBdFltModeCnt;
	CAM_BandFilterMode     eSupportedBdFltMode[CAM_BANDFILTER_NUM];

	CAM_Int32s             iSupportedFlashModeCnt;
	CAM_FlashMode          eSupportedFlashMode[CAM_FLASH_NUM];

	CAM_Int32s             iSupportedWBModeCnt;
	CAM_WhiteBalanceMode   eSupportedWBMode[CAM_WHITEBALANCEMODE_NUM];

	CAM_Int32s             iSupportedFocusModeCnt;
	CAM_FocusMode          eSupportedFocusMode[CAM_FOCUS_NUM];

	CAM_Int32s             iSupportedFocusZoneModeCnt;
	CAM_FocusZoneMode      eSupportedFocusZoneMode[CAM_FOCUSZONEMODE_NUM];

	CAM_Int32s             iSupportedColorEffectCnt;
	CAM_ColorEffect        eSupportedColorEffect[CAM_COLOREFFECT_NUM];

	CAM_Int32s             iSupportedVideoSubModeCnt;
	CAM_VideoSubMode       eSupportedVideoSubMode[CAM_VIDEOSUBMODE_NUM];

	CAM_Int32s             iSupportedStillSubModeCnt;
	CAM_StillSubMode       eSupportedStillSubMode[CAM_STILLSUBMODE_NUM];

	CAM_Int32s             iMinOptZoomQ16;
	CAM_Int32s             iMaxOptZoomQ16;

	CAM_Int32s             iMinDigZoomQ16;
	CAM_Int32s             iMaxDigZoomQ16;
	CAM_Int32s             iDigZoomStepQ16;
	CAM_Int32s             bSupportSmoothDigZoom;

	CAM_Int32s             iMinFpsQ16;
	CAM_Int32s             iMaxFpsQ16;

	CAM_Int32s             iMinBrightness;
	CAM_Int32s             iMaxBrightness;

	CAM_Int32s             iMinContrast;
	CAM_Int32s             iMaxContrast;

	CAM_Int32s             iMinSaturation;
	CAM_Int32s             iMaxSaturation;

	CAM_Int32s             iMinSharpness;
	CAM_Int32s             iMaxSharpness;

	CAM_Int32s             iMaxBurstCnt;

	CAM_Int32s             iMaxZslDepth;
	CAM_Int32s             iMaxExpMeterROINum; // max supported metering ROI number under CAM_EXPOSUREMETERMODE_MANUAL mode, if not support manual mode, please set to 0.
	CAM_Int32s             iMaxFocusROINum; // max supported focus ROI number under CAM_FOCUSZONEMODE_MANUAL mode, if not support manual mode, please set to 0.
} CAM_ShotModeCapability;

typedef struct
{
	CAM_PortCapability     stPortCapability[3];

	CAM_Int32s             iSupportedShotModeCnt;
	CAM_ShotMode           eSupportedShotMode[CAM_SHOTMODE_NUM];

	CAM_Int32s             iSupportedSensorRotateCnt;
	CAM_FlipRotate         eSupportedSensorRotate[CAM_FLIPROTATE_NUM];

	CAM_ShotModeCapability stSupportedShotParams;

	CAM_JpegCapability     stSupportedJPEGParams;

	CAM_Int32s             iMaxFaceNum;
} CAM_CameraCapability;

// Event handler table
typedef void (*CAM_EventHandler)( CAM_EventId eEventId, void *pParam, void *pUserData );

// Camera Engine entry API definitions
typedef void*   CAM_DeviceHandle;
typedef void*   CAM_Param;

#define UNUSED_PARAM  ((CAM_Param)NULL)

CAM_Error CAM_GetHandle( CAM_DeviceHandle *phHandle );

CAM_Error CAM_FreeHandle( CAM_DeviceHandle *phHandle );

CAM_Error CAM_SendCommand( CAM_DeviceHandle hHandle, CAM_Command cmd, CAM_Param param1, CAM_Param param2 );


// RESERVED: sensor image statstics
/*
typedef enum
{
	//Statistic Method                      // Param
	//                                      // Stat Result
	//                                      // Comments

	// IPP statistic method
	CAM_STATISTIC_ARBWINWEIGHTEDLUM,        // CAM_ArbWinWeightedParam *pParam
	// CAM_Int16u *pLumAvg
	// weighted luminance arverge

	CAM_STATISTIC_ARBWINWEIGHTEDRGBG,       // CAM_ArbWinWeightedParam *pParam
	// CAM_Int16u rg_bg_ratio[2]
	// weighted r/g, b/g ratios

	// Smart sensor statisic method
	CAM_STATISTIC_4X4WEIGHTEDLUMAVG,        // CAM_4X4WeightedLumParam *pParam
	// CAM_Int16u *pLumAvg
	// luminance average

	//
	CAM_STATISTIC_FRAMERGBHISTOGRAM,        // NULL
	// CAM_Int32s histogram[3][32]
	// 32 bins histograms for R, G, B
	// channels

	CAM_STATISTIC_FRAMELUMHISTOGRAM,        // NULL
	// CAM_Int32s histogram[32]
	// 32 bins histogram for luminance,
	// luminance can be defined by
	// statistic unit

	// Smart CI statistic method
	CAM_STATISTIC_8Z64P,                    // CAM_8Z64PParam
	// CAM_8Z64PResult *pResult
	// Statistic R/G/B average and Y
	// sharpness in 8 zones. Each zone
	// is composed of patches. The whole
	// frame are evenly divided into 8x8
	// totally 64 patches. Each patch
	// can be config to belong to one
	// zone. The statistic result is
	// updated every frame.
} CAM_StatisticMethod;

// Shutter limit
typedef struct
{
	CAM_Int32s  iResIndex;
	CAM_Int32s  iFPS;
	CAM_Int32s  iMin;
	CAM_Int32s  iMax;
	CAM_Int32s  iMaxFPSAutoDeduce;
} CAM_EShutterLimit;

// Parameter structure for arbitrary window weighted brightness
typedef struct
{
	CAM_Int32s iWindowNum;
	CAM_Rect   *pWindows;
	CAM_Int32s *pWeights;
} CAM_ArbWinWeightedParam;

// Parameter structure for window rgb/lum sum
typedef struct
{
	CAM_Int32s iLeft;
	CAM_Int32s iTop;
	CAM_Int32s iWidth;
	CAM_Int32s iHeight;
	CAM_Int32s iWeight[16];
} CAM_4X4WeightedLumParam;

// Parameter structure for 8Z64P statistic method
typedef struct
{
	CAM_Int8u   iRegionIndex[64];
	//  CAM_*****   ****************;           // filter for <r/g-b/g> color space
} CAM_8Z64PParam;

// Result structure for 8Z64P statistic method
typedef struct
{
	CAM_Int32s  iSumR[8];
	CAM_Int32s  iSumG[8];
	CAM_Int32s  iSumB[8];
	CAM_Int32s  iCount[8];
} CAM_8Z64PResult;

// Parameter structure for SET_STATISTIC_PARAMETER and GET_STATISTIC_RESULT
typedef struct
{
	CAM_StatisticMethod eMethod;
	void                *pData;     // content of pData depend on the eMethod
} CAM_StatisticData;
*/


#ifdef __cplusplus
}
#endif

#endif  // _CAM_ENGINE_H_
