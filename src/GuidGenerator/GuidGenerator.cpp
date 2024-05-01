#include "GuidGenerator.h"
#include <uuid/uuid.h>

std::string GuidGenerator::GenerateGuid() {
    uuid_t uuid;
    uuid_generate_random(uuid);
    char s[37];
    uuid_unparse(uuid, s);
    std::string uuidAsString  = s;
    // Keeping just the first section because max length of spread usernames is 10
    return uuidAsString.substr(0, 8);
}

