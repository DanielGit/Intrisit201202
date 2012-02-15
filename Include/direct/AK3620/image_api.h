#ifndef __IMAGE_API_H__
#define __IMAGE_API_H__

/**
* @FILENAME image_api.h
* @BRIEF image lib api  
* Copyright (C) 2009 Anyka (Guangzhou) Software Technology Co., LTD
* @AUTHOR xie_zhishan
* @DATE 2009-05-22
* @UPDATE 2009-05-22
* @VERSION 0.1.0 (migrated from ImageLib V2.10.9)
* @REF jpeg codec design spec.doc & AK3223/AK3224 programmer guide
*/

#ifdef __cplusplus
extern "C" {
#endif

#define IMAGE_LIB_VERSION "ImageLib_for_noah V0.1.0"

/** ͼ���ʹ��˵����
 >> ͼ����ʼ��
    ��ʹ��ͼ���ĺ����ӿ�֮ǰ������Ҫ�������·������г�ʼ����
    (1)ʹ��SetImageChipID()����������ʹ�õ�оƬ����(�μ����ļ��еĶ���)
    (2)ʹ��Img_SetCallbackFuns)()�������ûص�����
    (3)ʹ��Img_SetFlushCacheFunc()��������Cacheˢ�º���
    ʾ�����룺
    CB_FUNS cb_funs;
    SetImageChipID(AK3224_JPEG_VER);
    memset(&cb_funs, 0, sizeof(cb_funs));
    cb_funs.malloc = NewMalloc;
    cb_funs.free   = NewFree;
    cb_funs.printf = DebugOutput;
    Img_SetCallbackFuns(&cb_funs);
    Img_SetFlushCacheFunc(Cache_FlushFunc);

 >> ��ȡͼ���汾��
    ʹ��Img_GetVersionInfo()�������Ի�ȡͼ���İ汾�ţ�����ֵ��һ���ַ�����
 
 >> ��ȡ�������
    ʹ��Img_GetLastError()�������Ի�ȡ���һ�γ���ĳ�����룬�������Ķ���μ�
    ���ļ��е�T_IMAGE_ERRORö�����Ͷ��塣

 >> ͼ������
    �μ����ļ��еĺ����������֣� ���У�<in>��ʾ���������������<out>��ʾ��������
��������<in,out>��ʾ�����������������������ϸ����Ϣ�����ͼ���ӿ�˵���ĵ���

 >> JPEGͼ�����ʱ��EXIF��Ϣ����
    (1)ͨ��Img_EnableExif()����ʹ��/��ֹJPEG����ʱǶ��EXIF��Ϣ��
    (2)ͨ��Img_GetExifInfo()������ȡJPEG����ʱǶ���EXIF��Ϣ��
    (3)ͨ��Img_SetExifInfo()��������JPEG����ʱǶ���EXIF��Ϣ��
    ʾ�����룺
    T_ExifInfo exifInfo;                        // ���ݽṹ�������
    char MyDateTime[20] = "2008:10:22 16:18:54";

    Img_EnableExif(AK_TRUE);                    // ����JPEG����ʱǶ��EXIF��Ϣ
    Img_GetExifInfo(&exifInfo);                 // ��ȡJPEG����ʱǶ���EXIF��Ϣ

    exifInfo.ImageDescription[0] = '\0';        // ��ֹǶ�� ImageDescription ��Ϣ��
    strcpy((char*)exifInfo.Make, "Anyka New");  // ע���ַ�����Ҫ����
    strcpy((char*)exifInfo.Model, "AK3224M");
    // ���޸�ԭ�е� Artist �� Software ��Ϣ��
    strcpy((char*)exifInfo.DateTime, MyDateTime);
    exifInfo.bThumbnail = AK_TRUE;              // �������800*600��ͼƬǶ������ͼ
    Img_SetExifInfo(&exifInfo);                 // ����JPEG����ʱǶ���EXIF��Ϣ
 */

typedef unsigned long        IMG_T_U32;
typedef signed long          IMG_T_S32;
typedef unsigned short       IMG_T_U16;
typedef unsigned char        IMG_T_U8;
typedef signed char          IMG_T_S8;
typedef unsigned char        IMG_T_BOOL;
typedef const signed char *  IMG_T_pCSTR;
typedef void                 IMG_T_VOID;


#if 1
typedef IMG_T_U32 (*CALLBACK_FUN_FREAD)
 (unsigned int hFile, void *pBuffer, unsigned int count); 

typedef IMG_T_U32 (*CALLBACK_FUN_FWRITE)
 (unsigned int hFile, void *pBuffer, unsigned int count); 

typedef IMG_T_U32 (*CALLBACK_FUN_FSEEK)
 (unsigned int hFile, int offset, unsigned char origin); 

typedef IMG_T_U32 (*CALLBACK_FUN_FGETLEN)(unsigned int hFile); 

typedef IMG_T_U32 (*CALLBACK_FUN_FTELL)(unsigned int hFile); 

typedef struct {
    IMG_T_U16    ResourceID;            
    IMG_T_U8    *Buff;      //The Pointer to save the resource.
                            // (Application should malloc the memory for it) 
    IMG_T_U32    Resource_len;    
} T_LOADRESOURCE_CB_PARA;
//if Buff is NULL, and Resource_len is 0, a failure have occurred

typedef void (*CALLBACK_FUN_LOADRESOURCE)(T_LOADRESOURCE_CB_PARA *pPara); 

typedef void (*CALLBACK_FUN_RELEASERESOURCE)(IMG_T_U8 *Buff); 

typedef void* (*CALLBACK_FUN_MALLOC)
 (IMG_T_U32 size, char *filename, IMG_T_U32 line); 

typedef void* (*CALLBACK_FUN_FREE)(void* mem); 

typedef void* (*CALLBACK_FUN_REMALLOC)(void* mem, IMG_T_U32 size); 

typedef void* (*CALLBACK_FUN_DMAMEMCOPY)
 (void* dst, void* src, IMG_T_U32 count); 

typedef void (*CALLBACK_FUN_SHOWFRAME)
 (void* srcImg, IMG_T_U32 src_width, IMG_T_U32 src_height); 

typedef void (*CALLBACK_FUN_CAMERASHOWFRAME)
 (void* srcImg, IMG_T_U32 src_width, IMG_T_U32 src_height); 

typedef void (*CALLBACK_FUN_CAPSTART)(void); 

typedef IMG_T_BOOL (*CALLBACK_FUN_CAPCOMPLETE)(void); 

typedef void* (*CALLBACK_FUN_CAPGETDATA)(void); 

typedef IMG_T_U32 (*CALLBACK_FUN_GETTICKCOUNT)(void); 

typedef IMG_T_S32 (*CALLBACK_FUN_PRINTF)(IMG_T_pCSTR format, ...); 


typedef struct{
    CALLBACK_FUN_FREAD               fread;
    CALLBACK_FUN_FWRITE              fwrite;
    CALLBACK_FUN_FSEEK               fseek;
    CALLBACK_FUN_FGETLEN             fgetlen;
    CALLBACK_FUN_FTELL               ftell;
    CALLBACK_FUN_LOADRESOURCE        LoadResource;
    CALLBACK_FUN_RELEASERESOURCE     ReleaseResource;
    CALLBACK_FUN_MALLOC              malloc;
    CALLBACK_FUN_FREE                free;
    CALLBACK_FUN_REMALLOC            remalloc;
    CALLBACK_FUN_DMAMEMCOPY          DMAMemcpy;
    CALLBACK_FUN_SHOWFRAME           ShowFrame;
    CALLBACK_FUN_CAMERASHOWFRAME     CameraShowFrame;
    CALLBACK_FUN_CAPSTART            CapStart;
    CALLBACK_FUN_CAPCOMPLETE         CapComplete;
    CALLBACK_FUN_CAPGETDATA          CapGetData;
    CALLBACK_FUN_GETTICKCOUNT        GetTickCount;
    CALLBACK_FUN_PRINTF              printf;
} CB_FUNS;
#endif

typedef IMG_T_VOID (*FlushCacheFunc)(void);

typedef IMG_T_VOID (*SaveBuffFunc)(IMG_T_U8 *buff, IMG_T_U32 size);

typedef IMG_T_VOID (*JPEGTaskFunc)(void);

#define IMG_INVALID_HANDLE 0

typedef enum tagIMAGETYPE
{
    IMG_IMAGE_UNKNOWN,
    IMG_IMAGE_BMP,
    IMG_IMAGE_WBMP,
    IMG_IMAGE_JPG,
    IMG_IMAGE_GIF,
    IMG_IMAGE_PNG
}T_IMAGE_TYPE;


// gif decoder mode
typedef enum
{
    MIN_SPACE,
    MAX_SPEED
}GIF_DEC_MODE;

// error type
typedef enum
{
    IMG_NO_ERROR,
    IMG_INPUT_NULL_POINTER,
    IMG_PARAMETER_ERROR,
    IMG_STREAM_ERROR,
    IMG_NOT_ENOUGH_MOMORY,
    IMG_NOT_SUPPORT_FORMAT
}T_IMAGE_ERROR;

typedef enum
{
    JPEG_HW_VER1,
    JPEG_HW_VER2,
    JPEG_HW_VER3    
} JPEG_HW_VERSION;

#define     AK3221_JPEG_VER                  JPEG_HW_VER1
#define     AK3223_JPEG_VER                  JPEG_HW_VER1
#define     AK3224_JPEG_VER                  JPEG_HW_VER1
#define     AK3225_JPEG_VER                  JPEG_HW_VER1
#define     AK3610_JPEG_VER                  JPEG_HW_VER1
#define     AK3620_JPEG_VER                  JPEG_HW_VER1
#define     AK322L_JPEG_VER                  JPEG_HW_VER2
#define     AK3225L_JPEG_VER                 JPEG_HW_VER2
#define     AK3631L_JPEG_VER                 JPEG_HW_VER2
#define     AK3810_JPEG_VER                  JPEG_HW_VER3
#define     AK7801_JPEG_VER                  JPEG_HW_VER3
#define     AK7802_JPEG_VER                  JPEG_HW_VER3


typedef struct
{
    IMG_T_U8    ImageDescription[80];
    IMG_T_U8    Make[30];
    IMG_T_U8    Model[20];
    IMG_T_U8    Artist[20];
    IMG_T_U8    Software[40];
    IMG_T_U8    DateTime[20];           // "YYYY:MM:DD hh:mm:ss"
    IMG_T_BOOL  bThumbnail;
} T_ExifInfo;


/******************************************************************************
 * Miscellaneous Function
 ******************************************************************************/

/** ���õ�ǰоƬ��ʹ�õ�JPEGӲ������汾
* @PARAM ver <in>JPEGӲ������汾(ȡֵ�����ļ�)
*/
IMG_T_VOID SetImageChipID(JPEG_HW_VERSION ver);

/** �趨�ص�����
* @PARAM pCBFuns <in>����ָ��ṹ���ָ��
*/
IMG_T_VOID Img_SetCallbackFuns(const CB_FUNS *pCBFuns);

/** ����Cacheˢ�º���
* @PARAM FlushCacheFunc <in>Cacheˢ�º�����ָ��
*/
IMG_T_VOID Img_SetFlushCacheFunc(const FlushCacheFunc func);

/** ����JPEG���й���������
* @PARAM JPEGTaskFunc <in>��JPEGģ�鲢�й����ĺ�����ָ��
*/
IMG_T_VOID Img_SetJPEGTaskFunc(const JPEGTaskFunc func);

/** ��ȡͼ���汾��Ϣ
* @RETURN ͼ���汾��Ϣ�ַ���
*/
IMG_T_S8 *Img_GetVersionInfo(void);

/** ������һ�γ���ĳ������
* @RETURN ��������ö��ֵ(�μ����ļ��е�T_IMAGE_ERRORö�����Ͷ���)
*/
T_IMAGE_ERROR Img_GetLastError(void);

/** ȡ��ͼƬ���ݵ�ͼƬ��ʽ����
* @PARAM imgBuf <in>ͼƬ���ݵĻ�����ָ��
* @RETURN ͼƬ����ö��ֵ(�μ����ļ��е�T_IMAGE_TYPEö�����Ͷ���)
*/
T_IMAGE_TYPE Img_ImageType(const IMG_T_U8 *imgBuf);

/** ��ͼƬ�����BMP��ʽ
* @PARAM imgBuf <in>ͼƬ���ݵĻ�����ָ��
* @PARAM bufLen <in>ͼƬ���ݵĻ���������
* @PARAM outLen <out>���BMP���ݵ��ֽڳ���
* @RETURN ����ɹ�, ����BMP��ʽͼ�����ݣ�����ʧ��, ����AK_NULL, outLenֵ��Ч
*/
IMG_T_U8 *Img_ImageToBmp(const IMG_T_U8 *imgBuf, IMG_T_U32 bufLen, IMG_T_U32 *outLen);

/** ��YUV����ת��ΪRGB����
* @PARAM srcY <in>Y����ָ��
* @PARAM srcU <in>U����ָ��
* @PARAM srcV <in>V����ָ��
* @PARAM RGB <out>RGB����ָ��
* @PARAM srcWidth <in>Դͼ��Ŀ��
* @PARAM srcHeight <in>Դͼ��ĸ߶�
* @PARAM dstWidth <in>Ŀ��ͼ��Ŀ��
* @PARAM dstHeight <in>Ŀ��ͼ��ĸ߶�
* @PARAM timeout <in>�趨�ĳ�ʱֵ��3210оƬ��Ч��
* @RETURN 0��ת����ȷ����ֵ��ת��ʧ��
* @COMMENT ����YUV��ʽ��H211��AK3810��AK78xxоƬ��ΪYUV 420��
*          Դͼ��ߴ�������Ŀ��ͼ��ߴ�
*/
IMG_T_S32 Img_YUV2RGB(IMG_T_U8 *srcY, IMG_T_U8 *srcU, IMG_T_U8 *srcV, IMG_T_U8 *RGB,
                      IMG_T_S32 srcWidth, IMG_T_S32 srcHeight , IMG_T_S32 dstWidth,
                      IMG_T_S32 dstHeight, IMG_T_S32 timeout);


/******************************************************************************
 * WBMP Decode Function
 ******************************************************************************/

/** ȡ��WBMP��ʽͼƬ�Ŀ�Ⱥ͸߶���Ϣ
* @PARAM buffer <in>WBMP��ʽͼƬ�Ļ�����ָ��
* @PARAM width <out>WBMP��ʽͼƬ���
* @PARAM height <out>WBMP��ʽͼƬ�߶�
* @RETURN AK_TRUE���ɹ���AK_FALSE��ʧ�ܣ�width��height�е�ֵ��Ч
*/
IMG_T_BOOL Img_WBMPInfo(const IMG_T_U8 *buffer, IMG_T_U16 *width, IMG_T_U16 *height);

/** ��WBMP��ʽͼƬ�����BMP��ʽͼƬ
* @PARAM buffer <in>WBMP��ʽ��Դ���ݻ�����ָ��
* @PARAM length <in>WBMP��ʽ��Դ�����ֽڳ���
* @PARAM outLen <out>���BMP���ݵ��ֽڳ���
* @RETURN ���ؽ��BMP���ݻ�����ָ�룬����ʧ���򷵻�AK_NULL
*/
IMG_T_U8 *Img_WBMP2BMP(const IMG_T_U8 *buffer, IMG_T_U32 length, IMG_T_U32 *outLen);


/******************************************************************************
 * JPEG Decode Function
 ******************************************************************************/

/** ȡ�ö�ӦJpegͼƬ�Ŀ�Ⱥ͸߶���Ϣ�����ȵĴ�ֱ��ˮƽ������
* @PARAM JpegData <in>Jpeg��Դ����ָ��
* @PARAM size <in>Jpeg���ݳ���
* @PARAM width <out>����JpegԴ���ݶ�ӦͼƬ�Ŀ��
* @PARAM height <out>����JpegԴ���ݶ�ӦͼƬ�ĸ߶�
* @PARAM y_h_sam <out>�������ȵ�ˮƽ������
* @PARAM y_v_samp <out>�������ȵĴ�ֱ������
* @RETURN AK_TRUE���ɹ���AK_FALSE��ʧ�ܣ�width��height��y_h_sam��y_v_sam�е�ֵ��Ч
* @COMMENT �������Ҫ��ȡ�����ʵ���Ϣ���ɽ��������y_h_samp��y_v_samp��ΪAK_NULL
*/
IMG_T_BOOL Img_JpegInfo(const IMG_T_U8 *jpegData, IMG_T_U32 size, IMG_T_U16 *width,
                        IMG_T_U16 *height, IMG_T_U8 *y_h_samp, IMG_T_U8 *y_v_samp);

/** ��JpegͼƬ�����BMP��ʽ��Ӳ�����룩
* @PARAM jpegData <in>Jpeg��Դ����ָ��
* @PARAM size <in,out>���룺Jpeg���ݳ��ȣ���������ؽ�����BMP���ݳ���
* @RETURN ���ؽ��BMP���ݻ�����ָ�룬����ʧ���򷵻�AK_NULL
* @COMMENT ���ص�BMP����ָ���ڲ���ʱ���뽫���ͷţ���������ڴ�й©
*/
IMG_T_U8 *Img_Jpeg2BMP(const IMG_T_U8 *jpegData, IMG_T_U32 *size);

/** ��JpegͼƬ�����BMP��ʽ��������룩
* @PARAM jpegData <in>Jpeg��Դ����ָ��
* @PARAM size <in,out>���룺Jpeg���ݳ��ȣ���������ؽ�����BMP���ݳ���
* @RETURN ���ؽ��BMP���ݻ�����ָ�룬����ʧ���򷵻�AK_NULL
* @COMMENT ���ص�BMP����ָ���ڲ���ʱ���뽫���ͷţ���������ڴ�й©
*/
IMG_T_U8 *Img_Jpeg2BMPSoft(const IMG_T_U8 *jpegData, IMG_T_U32 *size);

/** ��JpegJPEGͼƬ�����BMP��ʽ�����������С��Ӳ�����룩
* @PARAM jpegData <in>Jpeg��Դ����ָ��
* @PARAM dstWidth <in>ָ��������BMPͼƬ���
* @PARAM dstHeight <in>ָ��������BMPͼƬ�߶�
* @PARAM size <in,out>���룺Jpeg���ݳ��ȣ���������ؽ�����BMP���ݳ���
* @RETURN ���ؽ��BMP���ݻ�����ָ�룬����ʧ���򷵻�AK_NULL
* @COMMENT ���ص�BMP����ָ���ڲ���ʱ���뽫���ͷţ���������ڴ�й©
*/
IMG_T_U8 *Img_Jpeg2BMPEx(const IMG_T_U8 *jpegData, IMG_T_U16 dstWidth,
                         IMG_T_U16 dstHeight, IMG_T_U32 *size);

/** ��JpegͼƬ�����BMP��ʽ�����������С��������룩
* @PARAM jpegData <in>Jpeg��Դ����ָ��
* @PARAM dstWidth <in>ָ��������BMPͼƬ���
* @PARAM dstHeight <in>ָ��������BMPͼƬ�߶�
* @PARAM size <in,out>���룺Jpeg���ݳ��ȣ���������ؽ�����BMP���ݳ���
* @RETURN ���ؽ��BMP���ݻ�����ָ�룬����ʧ���򷵻�AK_NULL
* @COMMENT ���ص�BMP����ָ���ڲ���ʱ���뽫���ͷţ���������ڴ�й©
*/
IMG_T_U8 *Img_Jpeg2BMPExSoft(const IMG_T_U8 *jpegData, IMG_T_U16 dstWidth,
                             IMG_T_U16 dstHeight, IMG_T_U32 *size);


/** ��JpegͼƬ���ݽ����YUV���ݣ������ͼƬ��ߣ�Ӳ�����룩,������YUV��ʽ��JPEG�ļ��ж���ĸ�ʽһ��
* @PARAM srcJPEG <in>Jpeg��Դ����ָ��
* @PARAM size <in>Jpeg���ݳ���
* @PARAM dstYUV <out>������YUV���ݵ�ַ
* @PARAM width <out>JpegͼƬ���
* @PARAM height <out>JpegͼƬ�߶�
* @RETURN AK_TRUE���ɹ���AK_FALSE��ʧ��
* @COMMENT dstYUVָ���YUV���ݻ������������ⲿԤ�ȷ����
*/
IMG_T_BOOL Img_JPEG2YUV(IMG_T_U8 *srcJPEG, IMG_T_U32 size, IMG_T_U8 *dstYUV,
                        IMG_T_S32 *width, IMG_T_S32*height);

/** ����Դ��MJPEG��Ƶ�е�Jpeg���ݽ����YUV���ݣ������ͼƬ��ߣ�Ӳ�����룩��������YUV��ʽ��ƽ̨����ĸ�ʽһ��
* @PARAM srcJPEG <in>Jpeg��Դ����ָ��
* @PARAM size <in>Jpeg���ݳ���
* @PARAM dstYUV <out>������YUV���ݵ�ַ
* @PARAM width <out>JpegͼƬ���
* @PARAM height <out>JpegͼƬ�߶�
* @RETURN AK_TRUE���ɹ���AK_FALSE��ʧ��
* @COMMENT dstYUVָ���YUV���ݻ������������ⲿԤ�ȷ����
*/
IMG_T_BOOL Img_VideoJPEG2YUV(IMG_T_U8 *srcJPEG, IMG_T_U32 size, IMG_T_U8 *dstYUV,
                        IMG_T_S32 *width, IMG_T_S32*height);

/** ��JpegͼƬ���ݽ����YUV���ݣ������ͼƬ��ߣ�������룩
* @PARAM srcJPEG <in>Jpeg��Դ����ָ��
* @PARAM size <in>Jpeg���ݳ���
* @PARAM dstYUV <out>������YUV���ݵ�ַ
* @PARAM width <out>JpegͼƬ���
* @PARAM height <out>JpegͼƬ�߶�
* @RETURN AK_TRUE���ɹ���AK_FALSE��ʧ��
* @COMMENT dstYUVָ���YUV���ݻ������������ⲿԤ�ȷ����
*/
IMG_T_BOOL Img_JPEG2YUVSoft(IMG_T_U8 *srcJPEG, IMG_T_U32 size, IMG_T_U8 *dstYUV,
                            IMG_T_S32 *width,IMG_T_S32*height);

/** ��JpegͼƬ���ݽ����YUV���ݣ���߸���С1/2���������С���ͼƬ��ߣ�Ӳ�����룩
* @PARAM srcJPEG <in>Jpeg��Դ����ָ��
* @PARAM size <in>Jpeg���ݳ���
* @PARAM dstYUV <out>������YUV���ݵ�ַ
* @PARAM width <out>JpegͼƬ���
* @PARAM height <out>JpegͼƬ�߶�
* @RETURN AK_TRUE���ɹ���AK_FALSE��ʧ��
* @COMMENT dstYUVָ���YUV���ݻ������������ⲿԤ�ȷ����
*/
IMG_T_BOOL Img_JPEG2YUV4x(IMG_T_U8 *srcJPEG, IMG_T_U32 size, IMG_T_U8 *dstYUV,
                          IMG_T_S32 *width, IMG_T_S32*height);

/** ��JpegͼƬ���ݽ����YUV���ݣ����������С��Ӳ�����룩
* @PARAM srcJPEG <in>Jpeg��Դ����ָ��
* @PARAM size <in>Jpeg���ݳ���
* @PARAM dstYUV <out>������YUV���ݵ�ַ
* @PARAM dstWidth <in>ָ��������JpegͼƬ���
* @PARAM dstHeight <in>ָ��������JpegͼƬ�߶�
* @RETURN AK_TRUE���ɹ���AK_FALSE��ʧ��
* @COMMENT dstYUVָ���YUV���ݻ������������ⲿԤ�ȷ����
*/
IMG_T_BOOL Img_JPEG2YUVEx(IMG_T_U8 *srcJPEG, IMG_T_U32 size, IMG_T_U8 *dstYUV,
                          IMG_T_U16 dstWidth, IMG_T_U16 dstHeight);

/** ��JpegͼƬ����ɲ����ļ�ͷ��BMP��ʽ��BGR24����4�ֽ���䣬Bottom-Up����Ӳ�����룩
* @PARAM jpegData <in>Jpeg��Դ����ָ��
* @PARAM width <out>JpegͼƬ���
* @PARAM height <out>JpegͼƬ�߶�
* @PARAM size <in,out>���룺Jpeg���ݳ��ȣ���������ؽ�����BMP���ݳ���
* @RETURN ���ؽ��BMP���ݻ�����ָ�룬����ʧ���򷵻�AK_NULL
* @COMMENT dstRGBָ���RGB���ݻ������������ⲿԤ�ȷ����
*/
IMG_T_BOOL Img_Jpeg2RGB(const IMG_T_U8 *srcJPEG, IMG_T_U8 *dstRGB, IMG_T_U32 *width,
                        IMG_T_U32 *height, IMG_T_U32 size);

/** ����JPEG�ļ��е�����ͼ��Ӳ�����룩
* @PARAM jpegData <in>Jpeg��Դ����ָ��
* @PARAM size <in,out>���룺Jpeg���ݳ��ȣ���������ؽ�����BMP����ͼ�����ݳ���
* @PARAM width <out>����ͼ�Ŀ��
* @PARAM height <out>����ͼ�ĸ߶�
* @RETURN ���ؽ��BMP���ݻ�����ָ�룬����ʧ��(������ͼ������)�򷵻�AK_NULL
* @COMMENT ���ص�BMP����ָ���ڲ���ʱ���뽫���ͷţ���������ڴ�й©
*/
IMG_T_U8 *Img_JpegThumbnail(const IMG_T_U8 *jpegData, IMG_T_U32 *size,
                            IMG_T_U16 *width, IMG_T_U16 *height);

/** ��JPEG�ļ���Ƕ������ͼ��Ӳ������룩
* @PARAM jpegData <in>Jpeg��Դ����ָ��
* @PARAM size <in,out>���룺Jpeg���ݳ��ȣ���������ؽ�����BMP����ͼ�����ݳ���
* @RETURN ���ؽ��JPEG���ݻ�����ָ�룬ʧ���򷵻�AK_NULL
* @COMMENT ���ص�Jpeg����ָ���ڲ���ʱ���뽫���ͷţ���������ڴ�й©
*/
IMG_T_U8 *Img_EmbedThumbnail(const IMG_T_U8 *jpegData, IMG_T_U32 *size);

/** ʹ�� / ��ֹ JPEG ����ʱǶ��EXIF��Ϣ
* @PRAM enable <in> JPEG����ʱ�Ƿ����EXIF��Ϣ
* @RETURN ��
*/
IMG_T_VOID Img_EnableExif(IMG_T_BOOL enable);

/** ��ȡJPEG����ʱǶ���EXIF��Ϣ
* @PARAM exifInfo: ָ��T_ExifInfo���͵Ľṹ���ָ��
* @RETURN none
*/
IMG_T_VOID Img_GetExifInfo(T_ExifInfo* exifInfo);

/** ����JPEG����ʱǶ���EXIF��Ϣ
* @PARAM exifInfo: ָ��T_ExifInfo���͵Ľṹ���ָ��
* @RETURN none
*/
IMG_T_VOID Img_SetExifInfo(T_ExifInfo* exifInfo);


/******************************************************************************
 * JPEG Encode Function
 ******************************************************************************/

/** ��YUV���ݱ����JpegͼƬ���ݣ�Ӳ�����룩
* @PARAM srcY <in>Y����ָ��
* @PARAM srcU <in>U����ָ��
* @PARAM srcV <in>V����ָ��
* @PARAM dstJPEG <out>Jpeg����ָ��
* @PARAM size <in,out>���룺dstJPEG�����С�������Jpeg����
* @PARAM width <in>ͼ����
* @PARAM height <in>ͼ��߶�
* @PARAM quality <in>����������ȡֵ��Χ0-200
* @RETURN AK_TRUE���ɹ���AK_FALSE��ʧ��
* @COMMENT ����YUV��ʽ��H211��AK3810��AK78xxоƬ��ΪYUV 420��
*/
IMG_T_BOOL Img_YUV2JPEG(IMG_T_U8 *srcY, IMG_T_U8 *srcU, IMG_T_U8 *srcV,
                        IMG_T_U8 *dstJPEG, IMG_T_U32 *size, IMG_T_U32 width,
                        IMG_T_U32 height, IMG_T_U8 quality);

/** ��YUV���ݱ����JpegͼƬ���ݣ�������룩
* @PARAM srcY <in>Y����ָ��
* @PARAM srcU <in>U����ָ��
* @PARAM srcV <in>V����ָ��
* @PARAM dstJPEG <out>Jpeg����ָ��
* @PARAM size <in,out>���룺dstJPEG�����С�������Jpeg����
* @PARAM width <in>ͼ����
* @PARAM height <in>ͼ��߶�
* @PARAM quality <in>����������ȡֵ��Χ0-200
* @RETURN AK_TRUE���ɹ���AK_FALSE��ʧ��
* @COMMENT ����YUV��ʽ��H211��AK3810��AK78xxоƬ��ΪYUV 420��
*/
IMG_T_BOOL Img_YUV2JPEGSoft(IMG_T_U8 *srcY, IMG_T_U8 *srcU, IMG_T_U8 *srcV,
                            IMG_T_U8 *dstJPEG, IMG_T_U32 *size, IMG_T_U32 width,
                            IMG_T_U32 height, IMG_T_U8 quality);

/** ��YUV���ݱ����JpegͼƬ���ݣ�ˮƽ��ֱ������Ŵ�2����Ӳ�����룩
* @PARAM srcY <in>YUV����ָ��
* @PARAM dstJPEG <out>Jpeg����ָ��
* @PARAM size <in,out>���룺dstJPEG�����С�������Jpeg����
* @PARAM width <in>Դͼ����
* @PARAM height <in>Դͼ��߶�
* @PARAM quality <in>����������ȡֵ��Χ0-200
* @RETURN AK_TRUE���ɹ���AK_FALSE��ʧ��
* @COMMENT ����YUV��ʽ��H211��AK3810��AK78xxоƬ��ΪYUV 420��
*/
IMG_T_BOOL Img_YUV2JPEG4x(IMG_T_U8 *srcYUV, IMG_T_U8 *dstJPEG,
                    IMG_T_U32 *size, IMG_T_U32 width, IMG_T_U32 height,
                    IMG_T_U8 quality);

/** ��YUV���ݱ����JpegͼƬ���ݣ�����������ţ�Ӳ�����룩
* @PARAM srcY <in>YUV����ָ��
* @PARAM dstJPEG <out>Jpeg����ָ��
* @PARAM size <in,out>���룺dstJPEG�����С�������Jpeg����
* @PARAM srcWidth <in>Դͼ����
* @PARAM srcHeight <in>Դͼ��߶�
* @PARAM dstWidth <in>Ŀ��ͼ����
* @PARAM dstHeight <in>Ŀ��ͼ��߶�
* @PARAM quality <in>����������ȡֵ��Χ0-200
* @RETURN AK_TRUE���ɹ���AK_FALSE��ʧ��
* @COMMENT ����YUV��ʽ��H211��AK3810��AK78xxоƬ��ΪYUV 420��
*/
IMG_T_BOOL Img_YUV2JPEGEx(IMG_T_U8 *srcYUV, IMG_T_U8 *dstJPEG, IMG_T_U32 *size,
                          IMG_T_U32 srcWidth, IMG_T_U32 srcHeight, IMG_T_U32 dstWidth,
                          IMG_T_U32 dstHeight, IMG_T_U8 quality);

/** ��YUV���ݱ����JpegͼƬ���ݣ�����������ţ�Ӳ�����룩
* @PARAM srcY <in>Y����ָ��
* @PARAM srcU <in>U����ָ��
* @PARAM srcV <in>V����ָ��
* @PARAM dstJPEG <out>Jpeg����ָ��
* @PARAM size <in,out>���룺dstJPEG�����С�������Jpeg����
* @PARAM srcWidth <in>Դͼ����
* @PARAM srcHeight <in>Դͼ��߶�
* @PARAM dstWidth <in>Ŀ��ͼ����
* @PARAM dstHeight <in>Ŀ��ͼ��߶�
* @PARAM quality <in>����������ȡֵ��Χ0-200
* @RETURN AK_TRUE���ɹ���AK_FALSE��ʧ��
* @COMMENT ����YUV��ʽ��H211��AK3810��AK78xxоƬ��ΪYUV 420��
*/
IMG_T_BOOL Img_YUV2JPEGExs(IMG_T_U8 *srcY, IMG_T_U8 *srcU, IMG_T_U8 *srcV,
                           IMG_T_U8 *dstJPEG, IMG_T_U32 *size,
                           IMG_T_U32 srcWidth, IMG_T_U32 srcHeight, IMG_T_U32 dstWidth,
                           IMG_T_U32 dstHeight, IMG_T_U8 quality);

IMG_T_BOOL Img_YUV2JPEGSave(IMG_T_U8 *srcY, IMG_T_U8 *srcU, IMG_T_U8 *srcV,
                            SaveBuffFunc saveFunc, IMG_T_U32 width, IMG_T_U32 height,
                            IMG_T_U8 quality);


/******************************************************************************
 * GIF Decode Function
 ******************************************************************************/
typedef IMG_T_S32 T_hGIFENUM;


/** ����GIF����ģʽ��MAX_SPEED��Ĭ�ϣ���MIN_SPACE
* @PARAM mode <in>���õ�GIF����ģʽ
*/
IMG_T_VOID GIFEnum_SetDecMode(GIF_DEC_MODE mode);

/** ��ȡGIF��Ϣ
* @PARAM GIFbuf <in>GIFͼ������
* @PARAM width <out>GIFͼ����
* @PARAM height <out>GIFͼ��߶�
* @PARAM bitCount <out>GIFͼ����ɫ���
* @RETURN AK_TRUE���ɹ���AK_FALSE��ʧ�ܣ�width��height��bitCount�е�ֵ��Ч
*/
IMG_T_BOOL Img_GIFInfo(const IMG_T_U8 *GIFbuf, IMG_T_U16 *width, IMG_T_U16 *height,
                       IMG_T_U8 *bitCount);

/** ����GIFͼƬ����HANDLE
* @PARAM GIFbuf <in>GIF��ʽͼƬ��Դ������ָ��
* @PARAM buflength <in>GIF��ʽͼƬ��Դ����������
* @RETURN �ɹ��򷵻���Ч��HANDLE��ʧ���򷵻�IMG_INVALID_HANDLE
* @COMMENT ����HANDLE�ɹ�, GIFbuf��ָ���ڴ��һֱ��HANDLE����ֱ��HANDLE��Close��
*          �����HANDLE��Close֮ǰGIFbuf��ָ����ڴ治�ܱ��ͷ�
*/
T_hGIFENUM GIFEnum_New(const IMG_T_U8 *GIFbuf, IMG_T_S32 buflength);

/** �ر�GIF����HANDLE
* @PARAM gifEnum <in>��GIFEnum_New������HANDLE
*/
IMG_T_VOID GIFEnum_Close(T_hGIFENUM gifEnum);

/** ȡ��HANDLE��ӦGIFͼƬ����֡��
* @PARAM gifEnum <in>��GIFEnum_New������HANDLE
* @RETURN GIFͼƬ����֡��
*/
IMG_T_U16  GIFEnum_GetFrameCount(T_hGIFENUM gifEnum);

/** ����GIF��һ֡ͼƬ
* @PARAM gifEnum <in>��GIFEnum_New������HANDLE
* @RETURN AK_TRUE���ɹ���AK_FALSE��ʧ��
*/
IMG_T_BOOL GIFEnum_FirstFrame(T_hGIFENUM gifEnum);

/** ����GIF��һ֡ͼƬ
* @PARAM gifEnum <in>��GIFEnum_New������HANDLE
* @RETURN AK_TRUE���ɹ���AK_FALSE��ʧ��
*/
IMG_T_BOOL GIFEnum_NextFrame(T_hGIFENUM gifEnum);

/** ȡ��GIF����HANDLE��ǰ֡��BMP
* @PARAM gifEnum <in>��GIFEnum_New������HANDLE
* @PARAM dataLen <out>��ǰ֡��BMP���ݳ���
* @PARAM bitsPerPix <out>��ǰ֡��BMPɫ��ֵ
* @RETURN ���ص�ǰ֡��BMP����ָ�룬����ʧ���򷵻�AK_NULL
*/
const IMG_T_U8 *GIFEnum_GetCurBMP(T_hGIFENUM gifEnum, IMG_T_U32 *dataLen,
                                  IMG_T_U8 *bitsPerPix);

/** ����HANDLE��ӦGIF����ָ��֡��BMP������
* @PARAM gifEnum <in>��GIFEnum_New������HANDLE
* @PARAM packetIdx <in>ָ����֡��
* @PARAM outLen <out>ָ��֡��BMP���ݳ���
* @PARAM bitsPerPix <out>ָ��֡��BMPɫ��ֵ
* @RETURN ���ص�ָ��֡��BMP����ָ�룬����ʧ���򷵻�AK_NULL
*/
const IMG_T_U8 *GIFEnum_GetFrameBMP(T_hGIFENUM gifEnum, IMG_T_U16 packetIdx,
                                    IMG_T_U32 *outLen, IMG_T_U8 *bitsPerPix);

/** ȡ��GIF����HANDLE��ǰ֡��֡���
* @PARAM gifEnum <in>��GIFEnum_New������HANDLE
* @RETURN ��ǰ֡��֡���
*/
IMG_T_U16  GIFEnum_GetCurPacket(T_hGIFENUM gifEnum);

/** ȡ��GIF����HANDLE��ǰ֡����ʱʱ��(ms)
* @PARAM gifEnum <in>��GIFEnum_New������HANDLE
* @RETURN ��ǰ֡�Ķ�����ʱ
*/
IMG_T_S32  GIFEnum_GetCurDelay(T_hGIFENUM gifEnum);

/** ���ĵ�ǰ֡����ʱʱ��(ms)
* @PARAM gifEnum <in>��GIFEnum_New������HANDLE
* @PARAM curDelay <in>��ʱʱ��, ��λΪms
* @COMMENT ���Ƽ�ʹ�øýӿ�������GIF֡�Ķ�����ʱ
*/
IMG_T_VOID GIFEnum_SetCurDelay(T_hGIFENUM gifEnum, IMG_T_S32 curDelay);

/** ����GIF����֡��BMP�ܴ�С
* @PARAM gifEnum <in>��GIFEnum_New������HANDLE
* @RETURN GIF����֡��BMP�ܴ�С
*/
IMG_T_U32  GIFEnum_GetTotalSize(T_hGIFENUM gifEnum);


/******************************************************************************
 * PNG Function
 ******************************************************************************/

/** ��ȡPNG��Ϣ
* @PARAM pngbuf <in>PNG��ʽ��Դ���ݻ�����ָ��
* @PARAM width <out>PNGͼƬ���
* @PARAM height <out>PNGͼƬ�߶�
* @PARAM bitCount <out>PNGͼƬ��ɫ���
* @RETURN AK_TRUE���ɹ���AK_FALSE��ʧ�ܣ�width��height��bitCount�е�ֵ��Ч
*/
IMG_T_BOOL Img_PNGInfo(const IMG_T_U8 *pngbuf, IMG_T_U16 *width, IMG_T_U16 *height,
                       IMG_T_U8 *bitCount);

/** ��PNG��ʽͼƬ�����BMP��ʽͼƬ
* @PARAM pngbuf	<in>PNG��ʽ��Դ���ݻ�����ָ��
* @PARAM outLen	<out>����ɹ����ؽ��BMP�����ֽڳ���
* @RETURN ���ؽ��BMP���ݻ�����ָ�룬����ʧ���򷵻�AK_NULL
* @COMMENT ���ص�BMP����ָ���ڲ���ʱ���뽫���ͷţ���������ڴ�й©
*/
IMG_T_U8 *Img_PNG2BMP(const IMG_T_U8 *pngbuf, IMG_T_U32 *outLen);

/** ��BMP��ʽͼƬ�����PNG��ʽͼƬ
* @PARAM png_buff	<out>PNG��ʽ�����ݻ�����ָ��
* @PARAM png_len <in,out>���룺PNG�����С�������������PNG���ݳ���
* @PARAM bmp_buff <in>BMP��ʽ��Դ���ݻ���ָ��
* @RETURN AK_TRUE���ɹ���AK_FALSE��ʧ��
*/
IMG_T_BOOL Img_BMP2PNG(IMG_T_U8 *png_buff, IMG_T_U32 *png_len, const IMG_T_U8 *bmp_buff);


#ifdef __cplusplus
}
#endif

#endif    // #ifndef __IMAGE_API_H__
