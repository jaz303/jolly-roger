#include "jr.h"

jr_status_t jr_tlv_parse(const uint8_t *bytes, int length, int big_endian, int type_size, int length_size, jr_tlv_cb chunk_cb, jr_userdata_t userdata) {
    const int header_size = type_size + length_size;

    int phase = 0;
    int i = 0;

loop:
    while (i < length) {
        if ((i+header_size) > length) {
            return JR_PARSE_ERROR;
        }
        
        uint32_t chunk_type = jr_mem_read_uint(bytes + i, type_size, big_endian);
        i += type_size;

        uint32_t chunk_length = jr_mem_read_uint(bytes + i, length_size, big_endian);
        i += length_size;
        
        if ((i+chunk_length) > length) {
            return JR_PARSE_ERROR;
        }
        
        if (phase == 1) {
            jr_status_t ret = chunk_cb(chunk_type, &bytes[i], chunk_length, userdata);
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
