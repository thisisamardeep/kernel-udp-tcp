
#include "uuid.h"

#include <uuid/uuid.h>
#include "memory.h"


namespace AsioUtils {

    std::string UUID::string() const {
        const char *digits = "0123456789abcdef";

        std::string result(36, '0');

        int index = 0;
        for (auto value: _data) {
            result[index++] = digits[(value >> 4) & 0x0F];
            result[index++] = digits[(value >> 0) & 0x0F];
            if ((index == 8) || (index == 13) || (index == 18) || (index == 23))
                result[index++] = '-';
        }

        return result;
    }

    UUID UUID::Sequential() {
        UUID result;
        uuid_t uuid;
        uuid_generate_time(uuid);

        result._data[0] = uuid[0];
        result._data[1] = uuid[1];
        result._data[2] = uuid[2];
        result._data[3] = uuid[3];

        result._data[4] = uuid[4];
        result._data[5] = uuid[5];

        result._data[6] = uuid[6];
        result._data[7] = uuid[7];

        result._data[8] = uuid[8];
        result._data[9] = uuid[9];

        result._data[10] = uuid[10];
        result._data[11] = uuid[11];
        result._data[12] = uuid[12];
        result._data[13] = uuid[13];
        result._data[14] = uuid[14];
        result._data[15] = uuid[15];

        return result;
    }

    UUID UUID::Random() {
        UUID result;
        uuid_t uuid;
        uuid_generate_random(uuid);

        result._data[0] = uuid[0];
        result._data[1] = uuid[1];
        result._data[2] = uuid[2];
        result._data[3] = uuid[3];

        result._data[4] = uuid[4];
        result._data[5] = uuid[5];

        result._data[6] = uuid[6];
        result._data[7] = uuid[7];

        result._data[8] = uuid[8];
        result._data[9] = uuid[9];

        result._data[10] = uuid[10];
        result._data[11] = uuid[11];
        result._data[12] = uuid[12];
        result._data[13] = uuid[13];
        result._data[14] = uuid[14];
        result._data[15] = uuid[15];

        return result;
    }



}
