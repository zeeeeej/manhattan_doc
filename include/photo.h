#ifndef _PHOTO_H
#define _PHOTO_H

#ifdef __cplusplus
extern "C" {
#endif


int qjy_photo_init();
int qjy_photo_deinit();
int qjy_take_photo(uint8_t type, uint8_t *id);

#ifdef __cplusplus
}
#endif

#endif