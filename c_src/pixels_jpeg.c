#include <string.h>
#ifndef max
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif

#include "erl_nif.h"
#include "ext/picojpeg.h"

typedef unsigned char uint8;

static FILE *g_pInFile;
static uint g_nInFileSize;
static uint g_nInFileOfs;

unsigned char pjpeg_need_bytes_callback(unsigned char* pBuf, unsigned char buf_size, unsigned char *pBytes_actually_read, void *pCallback_data)
{
    uint n;
    pCallback_data;

    n = min(g_nInFileSize - g_nInFileOfs, buf_size);
    if (n && (fread(pBuf, 1, n, g_pInFile) != n))
        return PJPG_STREAM_READ_ERROR;
    *pBytes_actually_read = (unsigned char)(n);
    g_nInFileOfs += n;
    return 0;
}

//------------------------------------------------------------------------------
// Loads JPEG image from specified file. Returns NULL on failure.
// On success, the malloc()'d image's width/height is written to *x and *y, and
// the number of components (1 or 3) is written to *comps.
// pScan_type can be NULL, if not it'll be set to the image's pjpeg_scan_type_t.
// Not thread safe.
// If reduce is non-zero, the image will be more quickly decoded at approximately
// 1/8 resolution (the actual returned resolution will depend on the JPEG
// subsampling factor).

uint8 *pjpeg_load_from_file(const char *pFilename, int *x, int *y, int *comps, pjpeg_scan_type_t *pScan_type)
{
    pjpeg_image_info_t image_info;
    int mcu_x = 0;
    int mcu_y = 0;
    uint row_pitch;
    uint8 *pImage;
    uint8 status;
    uint decoded_width, decoded_height;
    uint row_blocks_per_mcu, col_blocks_per_mcu;

    *x = 0;
    *y = 0;
    *comps = 0;
    if (pScan_type) *pScan_type = PJPG_GRAYSCALE;

    g_pInFile = fopen(pFilename, "rb");
    if (!g_pInFile)
        return NULL;

    g_nInFileOfs = 0;

    fseek(g_pInFile, 0, SEEK_END);
    g_nInFileSize = ftell(g_pInFile);
    fseek(g_pInFile, 0, SEEK_SET);

    status = pjpeg_decode_init(&image_info, pjpeg_need_bytes_callback, NULL, 0);

    if (status)
    {
        printf("pjpeg_decode_init() failed with status %u\n", status);
        if (status == PJPG_UNSUPPORTED_MODE)
        {
            printf("Progressive JPEG files are not supported.\n");
        }

        fclose(g_pInFile);
        return NULL;
    }

    if (pScan_type)
        *pScan_type = image_info.m_scanType;

    // In reduce mode output 1 pixel per 8x8 block.
    decoded_width = image_info.m_width;
    decoded_height = image_info.m_height;

    row_pitch = decoded_width * image_info.m_comps;
    pImage = (uint8 *)malloc(row_pitch * decoded_height);
    if (!pImage)
    {
        fclose(g_pInFile);
        return NULL;
    }

    row_blocks_per_mcu = image_info.m_MCUWidth >> 3;
    col_blocks_per_mcu = image_info.m_MCUHeight >> 3;

    for ( ; ; )
    {
        int y, x;
        uint8 *pDst_row;

        status = pjpeg_decode_mcu();

        if (status)
        {
            if (status != PJPG_NO_MORE_BLOCKS)
            {
                printf("pjpeg_decode_mcu() failed with status %u\n", status);

                free(pImage);
                fclose(g_pInFile);
                return NULL;
            }

            break;
        }

        if (mcu_y >= image_info.m_MCUSPerCol)
        {
            free(pImage);
            fclose(g_pInFile);
            return NULL;
        }

        // Copy MCU's pixel blocks into the destination bitmap.
        pDst_row = pImage + (mcu_y * image_info.m_MCUHeight) * row_pitch + (mcu_x * image_info.m_MCUWidth * image_info.m_comps);

        for (y = 0; y < image_info.m_MCUHeight; y += 8)
        {
            const int by_limit = min(8, image_info.m_height - (mcu_y * image_info.m_MCUHeight + y));

            for (x = 0; x < image_info.m_MCUWidth; x += 8)
            {
                uint8 *pDst_block = pDst_row + x * image_info.m_comps;

                // Compute source byte offset of the block in the decoder's MCU buffer.
                uint src_ofs = (x * 8U) + (y * 16U);
                const uint8 *pSrcR = image_info.m_pMCUBufR + src_ofs;
                const uint8 *pSrcG = image_info.m_pMCUBufG + src_ofs;
                const uint8 *pSrcB = image_info.m_pMCUBufB + src_ofs;

                const int bx_limit = min(8, image_info.m_width - (mcu_x * image_info.m_MCUWidth + x));

                if (image_info.m_scanType == PJPG_GRAYSCALE)
                {
                    int bx, by;
                    for (by = 0; by < by_limit; by++)
                    {
                        uint8 *pDst = pDst_block;

                        for (bx = 0; bx < bx_limit; bx++)
                            *pDst++ = *pSrcR++;

                        pSrcR += (8 - bx_limit);

                        pDst_block += row_pitch;
                    }
                }
                else
                {
                    int bx, by;
                    for (by = 0; by < by_limit; by++)
                    {
                        uint8 *pDst = pDst_block;

                        for (bx = 0; bx < bx_limit; bx++)
                        {
                            pDst[0] = *pSrcR++;
                            pDst[1] = *pSrcG++;
                            pDst[2] = *pSrcB++;
                            pDst += 3;
                        }

                        pSrcR += (8 - bx_limit);
                        pSrcG += (8 - bx_limit);
                        pSrcB += (8 - bx_limit);

                        pDst_block += row_pitch;
                    }
                }
            }

            pDst_row += (row_pitch * 8);
        }

        mcu_x++;
        if (mcu_x == image_info.m_MCUSPerRow)
        {
            mcu_x = 0;
            mcu_y++;
        }
    }

    fclose(g_pInFile);

    *x = decoded_width;
    *y = decoded_height;
    *comps = image_info.m_comps;

    return pImage;
}

static void get_pixel(int* pDst, const uint8 *pSrc, int luma_only, int num_comps)
{
    int r, g, b;
    if (num_comps == 1)
    {
        r = g = b = pSrc[0];
    }
    else if (luma_only)
    {
        const int YR = 19595, YG = 38470, YB = 7471;
        r = g = b = (pSrc[0] * YR + pSrc[1] * YG + pSrc[2] * YB + 32768) / 65536;
    }
    else
    {
        r = pSrc[0]; g = pSrc[1]; b = pSrc[2];
    }
    pDst[0] = r; pDst[1] = g; pDst[2] = b;
}

/// /// ///

static ERL_NIF_TERM _jpeg_result(ErlNifEnv *env, uint8 *image_data, int width, int height, int comps, pjpeg_scan_type_t scan_type) {
    if (!image_data) {
        return enif_make_atom(env, "error");
    }

    ERL_NIF_TERM bindata_term;
    unsigned char * bindata_buf;

    bindata_buf = enif_make_new_binary(env, width * height * 4, &bindata_term);
    memset(bindata_buf, 0, width * height * 4);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            uint c;
            int a[3];
            get_pixel(a, image_data + (y * width + x) * comps, 0, comps);
            bindata_buf[y * width + 4 * x + 0] = a[0];
            bindata_buf[y * width + 4 * x + 1] = a[1];
            bindata_buf[y * width + 4 * x + 2] = a[2];
            bindata_buf[y * width + 4 * x + 3] = 255;
        }
    }

    free(image_data);

    return enif_make_tuple3(
        env,
        enif_make_int(env, width),
        enif_make_int(env, height),
        bindata_term
        );
}

ERL_NIF_TERM read_jpeg_file(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    ErlNifBinary binary;

    if (!enif_inspect_iolist_as_binary(env, argv[0], &binary)) {
        return enif_make_badarg(env);
    }

    char filename[1000];

    strncpy(filename, binary.data, binary.size);
    filename[binary.size] = 0;

    int width, height, comps;
    pjpeg_scan_type_t scan_type;
    uint8 *image_data;

    image_data = pjpeg_load_from_file(filename, &width, &height, &comps, &scan_type);
    return _jpeg_result(env, image_data, width, height, comps, scan_type);
}
