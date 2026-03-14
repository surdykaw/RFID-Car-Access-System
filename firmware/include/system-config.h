#pragma once

#define NO_ACCESS	0
#define ACCESS	    1
#define UNLOCK 	    PC5
#define LOCK	    PC4
#define INFO_LED	PC1

uint8_t TAGS_UID_TAB[][4] = {
    {0x00, 0x00, 0x00, 0x00}, // TAG1 UID
    {0x00, 0x00, 0x00, 0x00}  // TAG2 UID
    // ... YOU CAN ADD OTHER TAGS HERE
};

#define TAG_COUNT (sizeof(TAGS_UID_TAB) / sizeof(TAGS_UID_TAB[0]))