#include "jr.h"

jr_status_t jr_tlv_parse(void *data, int length, int big_endian, int type_size, int length_size, jr_tlv_cb chunk_cb) {
    if (type_size < 1 || type_size > 4 || length_size < 1 || length_size > 4) {
        return JR_INVALID_ARG;
    }

    uint8_t *bytes = (uint8_t*)data;

    const int header_size = type_size + length_size;
    const int type_shift = big_endian ? 0 : ((type_size - 1) * 8);
    const int length_shift = big_endian ? 0 : ((length_size - 1) * 8);

    int phase = 0;
    int i = 0;

loop:
    while (i < length) {
        if ((i+header_size) > length) {
            return JR_PARSE_ERROR;
        }
        
        uint32_t chunk_type = 0;
        for (int j = 0; j < type_size; ++j) {
            if (big_endian) {
                chunk_type <<= 8;
            } else {
                chunk_type >>= 8;
            }
            chunk_type |= ((uint32_t)bytes[i++]) << type_shift;
        }
        
        uint32_t chunk_length = 0;
        for (int j = 0; j < length_size; ++j) {
            if (big_endian) {
                chunk_length <<= 8;
            } else {
                chunk_length >>= 8;
            }
            chunk_length |= ((uint32_t)bytes[i++]) << length_shift;
        }
        
        if ((i+chunk_length) > length) {
            return JR_PARSE_ERROR;
        }
        
        if (phase == 1) {
            jr_status_t ret = chunk_cb(chunk_type, chunk_length, &bytes[i]);
            if (ret < 0) {
                return ret;
            }
        }
        
        i += chunk_length;
    }

    if (phase == 0 && chunk_cb) {
        phase = 1;
        i = 0;
        goto loop;
    }

    return JR_OK;
}