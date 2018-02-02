/*
 * Copyright (c) 2017 Fastly, Kazuho Oku
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#ifndef quicly_frame_h
#define quicly_frame_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include "picotls.h"
#include "quicly/constants.h"
#include "quicly/ranges.h"

#define QUICLY_FRAME_TYPE_PADDING 0
#define QUICLY_FRAME_TYPE_RST_STREAM 1
#define QUICLY_FRAME_TYPE_CONNECTION_CLOSE 2
#define QUICLY_FRAME_TYPE_APPLICATION_CLOSE 3
#define QUICLY_FRAME_TYPE_MAX_DATA 4
#define QUICLY_FRAME_TYPE_MAX_STREAM_DATA 5
#define QUICLY_FRAME_TYPE_MAX_STREAM_ID 6
#define QUICLY_FRAME_TYPE_PING 7
#define QUICLY_FRAME_TYPE_BLOCKED 8
#define QUICLY_FRAME_TYPE_STREAM_BLOCKED 9
#define QUICLY_FRAME_TYPE_STREAM_ID_BLOCKED 10
#define QUICLY_FRAME_TYPE_NEW_CONNECTION_ID 11
#define QUICLY_FRAME_TYPE_STOP_SENDING 12
#define QUICLY_FRAME_TYPE_PONG 13
#define QUICLY_FRAME_TYPE_ACK 14
#define QUICLY_FRAME_TYPE_STREAM_BASE 0x10
#define QUICLY_FRAME_TYPE_STREAM_BITS 0x7
#define QUICLY_FRAME_TYPE_STREAM_BIT_OFF 0x4
#define QUICLY_FRAME_TYPE_STREAM_BIT_LEN 0x2
#define QUICLY_FRAME_TYPE_STREAM_BIT_FIN 0x1

#define QUICLY_MAX_DATA_FRAME_CAPACITY (1 + 8)
#define QUICLY_MAX_STREAM_DATA_FRAME_CAPACITY (1 + 8 + 8)
#define QUICLY_MAX_STREAM_ID_FRAME_CAPACITY (1 + 8)
#define QUICLY_RST_FRAME_CAPACITY (1 + 8 + 2 + 8)
#define QUICLY_STREAM_ID_BLOCKED_FRAME_CAPACITY (1 + 8)
#define QUICLY_STOP_SENDING_FRAME_CAPACITY (1 + 8 + 2)
#define QUICLY_ACK_FRAME_CAPACITY (1 + 8 + 8 + 1 + 8)
#define QUICLY_STREAM_FRAME_CAPACITY (1 + 8 + 8 + 1)

static uint16_t quicly_decode16(const uint8_t **src);
static uint32_t quicly_decode32(const uint8_t **src);
static uint64_t quicly_decode64(const uint8_t **src);
static uint64_t quicly_decodev(const uint8_t **src, const uint8_t *end);
static uint8_t *quicly_encode16(uint8_t *p, uint16_t v);
static uint8_t *quicly_encode32(uint8_t *p, uint32_t v);
static uint8_t *quicly_encode64(uint8_t *p, uint64_t v);
static uint8_t *quicly_encodev(uint8_t *p, uint64_t v);
static unsigned quicly_clz32(uint32_t v);
static unsigned quicly_clz64(uint64_t v);

static uint8_t *quicly_encode_stream_frame_header(uint8_t *dst, uint8_t *dst_end, uint64_t stream_id, int is_fin, uint64_t offset,
                                                  size_t *data_len, int *must_pad);

typedef struct st_quicly_stream_frame_t {
    uint64_t stream_id;
    unsigned is_fin : 1;
    uint64_t offset;
    ptls_iovec_t data;
} quicly_stream_frame_t;

static int quicly_decode_stream_frame(uint8_t type_flags, const uint8_t **src, const uint8_t *end, quicly_stream_frame_t *frame);

static uint8_t *quicly_encode_rst_stream_frame(uint8_t *dst, uint64_t stream_id, uint16_t app_error_code, uint64_t final_offset);

typedef struct st_quicly_rst_stream_frame_t {
    uint64_t stream_id;
    uint16_t app_error_code;
    uint64_t final_offset;
} quicly_rst_stream_frame_t;

static int quicly_decode_rst_stream_frame(const uint8_t **src, const uint8_t *end, quicly_rst_stream_frame_t *frame);

typedef struct st_quicly_close_frame_t {
    uint16_t error_code;
    ptls_iovec_t reason_phrase;
} quicly_close_frame_t;

static int quicly_decode_close_frame(const uint8_t **src, const uint8_t *end, quicly_close_frame_t *frame);

static uint8_t *quicly_encode_max_data_frame(uint8_t *dst, uint64_t max_data);

typedef struct st_quicly_max_data_frame_t {
    uint64_t max_data;
} quicly_max_data_frame_t;

static int quicly_decode_max_data_frame(const uint8_t **src, const uint8_t *end, quicly_max_data_frame_t *frame);

static uint8_t *quicly_encode_max_stream_data_frame(uint8_t *dst, uint64_t stream_id, uint64_t max_stream_data);

typedef struct st_quicly_max_stream_data_frame_t {
    uint64_t stream_id;
    uint64_t max_stream_data;
} quicly_max_stream_data_frame_t;

static int quicly_decode_max_stream_data_frame(const uint8_t **src, const uint8_t *end, quicly_max_stream_data_frame_t *frame);

static uint8_t *quicly_encode_max_stream_id_frame(uint8_t *dst, uint64_t max_stream_id);

typedef struct st_quicly_max_stream_id_frame_t {
    uint64_t max_stream_id;
} quicly_max_stream_id_frame_t;

static int quicly_decode_max_stream_id_frame(const uint8_t **src, const uint8_t *end, quicly_max_stream_id_frame_t *frame);

static size_t quicly_calc_ping_frame_size(size_t data_len);
uint8_t *quicly_encode_ping_frame(uint8_t *dst, ptls_iovec_t data);

typedef struct st_quicly_ping_frame_t {
    ptls_iovec_t data;
} quicly_ping_frame_t;

static int quicly_decode_ping_frame(const uint8_t **src, const uint8_t *end, quicly_ping_frame_t *frame);

typedef struct st_quicly_stream_blocked_frame_t {
    uint64_t stream_id;
    uint64_t offset;
} quicly_stream_blocked_frame_t;

static int quicly_decode_stream_blocked_frame(const uint8_t **src, const uint8_t *end, quicly_stream_blocked_frame_t *frame);

static uint8_t *quicly_encode_stream_id_blocked_frame(uint8_t *dst, uint64_t stream_id);

typedef struct st_quicly_stream_id_blocked_frame_t {
    uint64_t stream_id;
} quicly_stream_id_blocked_frame_t;

static int quicly_decode_stream_id_blocked_frame(const uint8_t **src, const uint8_t *end, quicly_stream_id_blocked_frame_t *frame);

static uint8_t *quicly_encode_stop_sending_frame(uint8_t *dst, uint64_t stream_id, uint16_t app_error_code);

typedef struct st_quicly_stop_sending_frame_t {
    uint64_t stream_id;
    uint16_t app_error_code;
} quicly_stop_sending_frame_t;

static int quicly_decode_stop_sending_frame(const uint8_t **src, const uint8_t *end, quicly_stop_sending_frame_t *frame);

uint8_t *quicly_encode_ack_frame(uint8_t *dst, uint8_t *dst_end, quicly_ranges_t *ranges, size_t *range_index);

typedef struct st_quicly_ack_frame_t {
    uint64_t largest_acknowledged;
    uint64_t smallest_acknowledged;
    uint64_t num_gaps;
    uint64_t ack_block_lengths[257];
    uint64_t gaps[256];
} quicly_ack_frame_t;

int quicly_decode_ack_frame(uint8_t type_flags, const uint8_t **src, const uint8_t *end, quicly_ack_frame_t *frame);

/* inline definitions */

inline uint16_t quicly_decode16(const uint8_t **src)
{
    uint16_t v = (uint16_t)(*src)[0] << 8 | (*src)[1];
    *src += 2;
    return v;
}

inline uint32_t quicly_decode32(const uint8_t **src)
{
    uint32_t v = (uint32_t)(*src)[0] << 24 | (uint32_t)(*src)[1] << 16 | (uint32_t)(*src)[2] << 8 | (*src)[3];
    *src += 4;
    return v;
}

inline uint64_t quicly_decode64(const uint8_t **src)
{
    uint64_t v = (uint64_t)(*src)[0] << 56 | (uint64_t)(*src)[1] << 48 | (uint64_t)(*src)[2] << 40 | (uint64_t)(*src)[3] << 32 |
                 (uint64_t)(*src)[4] << 24 | (uint64_t)(*src)[5] << 16 | (uint64_t)(*src)[6] << 8 | (*src)[7];
    *src += 8;
    return v;
}

inline uint64_t quicly_decodev(const uint8_t **src, const uint8_t *end)
{
    if (*src == end)
        return UINT64_MAX;
    if (**src >> 6 == 0)
        return *(*src)++;

    /* multi-byte */
    size_t len = 1 << (**src >> 6);
    if (end - *src < len)
        return UINT64_MAX;
    uint64_t v = *(*src)++ & 0x3f;
    --len;
    do {
        v = (v << 8) | *(*src)++;
    } while (--len != 0);
    return v;
}

inline uint8_t *quicly_encode16(uint8_t *p, uint16_t v)
{
    *p++ = (uint8_t)(v >> 8);
    *p++ = (uint8_t)v;
    return p;
}

inline uint8_t *quicly_encode32(uint8_t *p, uint32_t v)
{
    *p++ = (uint8_t)(v >> 24);
    *p++ = (uint8_t)(v >> 16);
    *p++ = (uint8_t)(v >> 8);
    *p++ = (uint8_t)v;
    return p;
}

inline uint8_t *quicly_encode64(uint8_t *p, uint64_t v)
{
    *p++ = (uint8_t)(v >> 56);
    *p++ = (uint8_t)(v >> 48);
    *p++ = (uint8_t)(v >> 40);
    *p++ = (uint8_t)(v >> 32);
    *p++ = (uint8_t)(v >> 24);
    *p++ = (uint8_t)(v >> 16);
    *p++ = (uint8_t)(v >> 8);
    *p++ = (uint8_t)v;
    return p;
}

inline uint8_t *quicly_encodev(uint8_t *p, uint64_t v)
{
    if (v > 63) {
        if (v > 16383) {
            if (v > 1073741823) {
                assert(v <= 4611686018427387903);
                *p++ = 0xc0 | (uint8_t)(v >> 56);
                *p++ = (uint8_t)(v >> 48);
                *p++ = (uint8_t)(v >> 40);
                *p++ = (uint8_t)(v >> 32);
                *p++ = (uint8_t)(v >> 24);
            } else {
                *p++ = 0x80 | (uint8_t)(v >> 24);
            }
            *p++ = (uint8_t)(v >> 16);
            *p++ = (uint8_t)(v >> 8);
        } else {
            *p++ = 0x40 | (v >> 8);
        }
    }
    *p++ = (uint8_t)v;
    return p;
}

inline unsigned quicly_clz32(uint32_t v)
{
    QUICLY_BUILD_ASSERT(sizeof(unsigned) == 4);
    return v != 0 ? __builtin_clz(v) : 32;
}

inline unsigned quicly_clz64(uint64_t v)
{
    QUICLY_BUILD_ASSERT(sizeof(long long) == 8);
    return v != 0 ? __builtin_clzll(v) : 64;
}

inline uint8_t *quicly_encode_stream_frame_header(uint8_t *dst, uint8_t *dst_end, uint64_t stream_id, int is_fin, uint64_t offset,
                                                  size_t *data_len, int *must_pad)
{
    uint8_t type = QUICLY_FRAME_TYPE_STREAM_BASE, *type_at = dst++;
    size_t copysize;

    /* emit stream id and offset */
    dst = quicly_encodev(dst, stream_id);
    if (offset != 0) {
        type |= QUICLY_FRAME_TYPE_STREAM_BIT_OFF;
        dst = quicly_encodev(dst, offset);
    }

    /* determine the amount to write as well as adjusting the flags */
    copysize = dst_end - dst;
    *must_pad = 1;
    if (copysize >= *data_len) {
        /* can emit entire data */
        if (copysize >= *data_len + 2) {
            /* emit with length to allow succeeding frames */
            dst = quicly_encodev(dst, *data_len);
            type |= QUICLY_FRAME_TYPE_STREAM_BIT_LEN;
            *must_pad = 0;
        }
        if (is_fin)
            type |= QUICLY_FRAME_TYPE_STREAM_BIT_FIN;
    } else {
        /* partial write */
        *data_len = copysize;
    }

    *type_at = type;
    return dst;
}

inline int quicly_decode_stream_frame(uint8_t type_flags, const uint8_t **src, const uint8_t *end, quicly_stream_frame_t *frame)
{
    /* obtain stream id */
    if ((frame->stream_id = quicly_decodev(src, end)) == UINT64_MAX)
        goto Error;

    /* obtain offset */
    if ((type_flags & QUICLY_FRAME_TYPE_STREAM_BIT_OFF) != 0) {
        if ((frame->offset = quicly_decodev(src, end)) == UINT64_MAX)
            goto Error;
    } else {
        frame->offset = 0;
    }

    /* obtain data */
    if ((type_flags & QUICLY_FRAME_TYPE_STREAM_BIT_LEN) != 0) {
        uint64_t len;
        if ((len = quicly_decodev(src, end)) == UINT64_MAX)
            goto Error;
        if (end - *src < len)
            goto Error;
        frame->data = ptls_iovec_init(*src, len);
        *src += len;
    } else {
        frame->data = ptls_iovec_init(*src, end - *src);
        *src = end;
    }

    /* fin bit */
    frame->is_fin = (type_flags & QUICLY_FRAME_TYPE_STREAM_BIT_FIN) != 0;
    if (!frame->is_fin && frame->data.len == 0)
        goto Error;

    return 0;
Error:
    return QUICLY_ERROR_FRAME_ERROR(QUICLY_FRAME_TYPE_STREAM_BASE);
}

inline uint8_t *quicly_encode_rst_stream_frame(uint8_t *dst, uint64_t stream_id, uint16_t app_error_code, uint64_t final_offset)
{
    *dst++ = QUICLY_FRAME_TYPE_RST_STREAM;
    dst = quicly_encodev(dst, stream_id);
    dst = quicly_encode16(dst, app_error_code);
    dst = quicly_encodev(dst, final_offset);
    return dst;
}

inline int quicly_decode_rst_stream_frame(const uint8_t **src, const uint8_t *end, quicly_rst_stream_frame_t *frame)
{
    if ((frame->stream_id = quicly_decodev(src, end)) == UINT64_MAX)
        goto Error;
    if (end - *src < 2)
        goto Error;
    frame->app_error_code = quicly_decode16(src);
    frame->final_offset = quicly_decodev(src, end);
    return 0;
Error:
    return QUICLY_ERROR_FRAME_ERROR(QUICLY_FRAME_TYPE_RST_STREAM);
}

inline int quicly_decode_close_frame(const uint8_t **src, const uint8_t *end, quicly_close_frame_t *frame)
{
    uint64_t reason_len;

    if (end - *src < 2)
        goto Error;
    frame->error_code = quicly_decode16(src);
    if ((reason_len = quicly_decodev(src, end)) == UINT64_MAX)
        goto Error;
    if (end - *src < reason_len)
        goto Error;
    frame->reason_phrase = ptls_iovec_init(*src, reason_len);
    *src += reason_len;
    return 0;
Error:
    return QUICLY_ERROR_FRAME_ERROR(QUICLY_FRAME_TYPE_CONNECTION_CLOSE);
}

inline uint8_t *quicly_encode_max_data_frame(uint8_t *dst, uint64_t max_data)
{
    *dst++ = QUICLY_FRAME_TYPE_MAX_DATA;
    dst = quicly_encodev(dst, max_data);
    return dst;
}

inline int quicly_decode_max_data_frame(const uint8_t **src, const uint8_t *end, quicly_max_data_frame_t *frame)
{
    if ((frame->max_data = quicly_decodev(src, end)) == UINT64_MAX)
        return QUICLY_ERROR_FRAME_ERROR(QUICLY_FRAME_TYPE_MAX_DATA);
    return 0;
}

inline uint8_t *quicly_encode_max_stream_data_frame(uint8_t *dst, uint64_t stream_id, uint64_t max_stream_data)
{
    *dst++ = QUICLY_FRAME_TYPE_MAX_STREAM_DATA;
    dst = quicly_encodev(dst, stream_id);
    dst = quicly_encodev(dst, max_stream_data);
    return dst;
}

inline int quicly_decode_max_stream_data_frame(const uint8_t **src, const uint8_t *end, quicly_max_stream_data_frame_t *frame)
{
    if ((frame->stream_id = quicly_decodev(src, end)) == UINT64_MAX)
        goto Error;
    if ((frame->max_stream_data = quicly_decodev(src, end)) == UINT64_MAX)
        goto Error;
    return 0;
Error:
    return QUICLY_ERROR_FRAME_ERROR(QUICLY_FRAME_TYPE_MAX_STREAM_DATA);
}

inline uint8_t *quicly_encode_max_stream_id_frame(uint8_t *dst, uint64_t max_stream_id)
{
    *dst++ = QUICLY_FRAME_TYPE_MAX_STREAM_ID;
    dst = quicly_encodev(dst, max_stream_id);
    return dst;
}

inline size_t quicly_calc_ping_frame_size(size_t data_len)
{
    assert(data_len < 256);
    return 2 + data_len;
}

inline int quicly_decode_max_stream_id_frame(const uint8_t **src, const uint8_t *end, quicly_max_stream_id_frame_t *frame)
{
    if ((frame->max_stream_id = quicly_decodev(src, end)) == UINT64_MAX)
        return QUICLY_ERROR_FRAME_ERROR(QUICLY_FRAME_TYPE_MAX_STREAM_ID);
    return 0;
}

inline int quicly_decode_ping_frame(const uint8_t **src, const uint8_t *end, quicly_ping_frame_t *frame)
{
    if (end - *src < 1)
        goto Error;
    frame->data.len = *(*src)++;
    if (end - *src < frame->data.len)
        goto Error;
    frame->data.base = (void *)src;
    src += frame->data.len;
    return 0;
Error:
    return QUICLY_ERROR_FRAME_ERROR(QUICLY_FRAME_TYPE_PING);
}

inline int quicly_decode_stream_blocked_frame(const uint8_t **src, const uint8_t *end, quicly_stream_blocked_frame_t *frame)
{
    if ((frame->stream_id = quicly_decodev(src, end)) == UINT64_MAX)
        goto Error;
    if ((frame->offset = quicly_decodev(src, end)) == UINT64_MAX)
        goto Error;
    return 0;
Error:
    return QUICLY_ERROR_FRAME_ERROR(QUICLY_FRAME_TYPE_STREAM_BLOCKED);
}

inline uint8_t *quicly_encode_stream_id_blocked_frame(uint8_t *dst, uint64_t stream_id)
{
    *dst++ = QUICLY_FRAME_TYPE_STREAM_ID_BLOCKED;
    dst = quicly_encodev(dst, stream_id);
    return dst;
}

inline int quicly_decode_stream_id_blocked_frame(const uint8_t **src, const uint8_t *end, quicly_stream_id_blocked_frame_t *frame)
{
    if ((frame->stream_id = quicly_decodev(src, end)) == UINT64_MAX)
        return QUICLY_ERROR_FRAME_ERROR(QUICLY_FRAME_TYPE_STREAM_ID_BLOCKED);
    return 0;
}

inline uint8_t *quicly_encode_stop_sending_frame(uint8_t *dst, uint64_t stream_id, uint16_t app_error_code)
{
    *dst++ = QUICLY_FRAME_TYPE_STOP_SENDING;
    dst = quicly_encodev(dst, stream_id);
    dst = quicly_encode16(dst, app_error_code);
    return dst;
}

inline int quicly_decode_stop_sending_frame(const uint8_t **src, const uint8_t *end, quicly_stop_sending_frame_t *frame)
{
    if ((frame->stream_id = quicly_decodev(src, end)) == UINT64_MAX)
        goto Error;
    if (end - *src < 2)
        goto Error;
    frame->app_error_code = quicly_decode16(src);
    return 0;
Error:
    return QUICLY_ERROR_FRAME_ERROR(QUICLY_FRAME_TYPE_STOP_SENDING);
}

#ifdef __cplusplus
}
#endif

#endif
