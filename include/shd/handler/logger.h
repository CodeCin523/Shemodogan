#ifndef __SHD_LOGGER_H__
#define __SHD_LOGGER_H__

#include "../type.h"

/* Registration metadata */
extern shd_handler_meta_t SHD_HND_LOGGER_META;
static constexpr shd_hid16_t SHD_HND_LOGGER_ID = SHD_HID16_LOGGER;

/* Runtime data struct */
typedef struct shd_hnd_logger {
    shd_basehnd_t base;
    char *pBuf;
    u32 bufIndex;
    u32 bufSize;
} shd_hnd_logger_t;

/* Creator struct */
typedef struct shd_crt_logger {
    shd_basecrt_t base;
    u32 bufferSize;
} shd_crt_logger_t;

/* Interface */
void shd_log_flush();
void shd_log(const char *msg, u8 level);
void shd_logh(const char *hnd_name, const char *msg, u8 level);

inline void shd_log_info(const char *msg) {
    shd_log(msg, 0);
}
inline void shd_log_debug(const char *msg) {
    shd_log(msg, 1);
}
inline void shd_log_warning(const char *msg) {
    shd_log(msg, 2);
}
inline void shd_log_error(const char *msg) {
    shd_log(msg, 3);
}

inline void shd_logh_info(const char *hnd_name, const char *msg) {
    shd_logh(hnd_name, msg, 0);
}
inline void shd_logh_debug(const char *hnd_name, const char *msg) {
    shd_logh(hnd_name, msg, 1);
}
inline void shd_logh_warning(const char *hnd_name, const char *msg) {
    shd_logh(hnd_name, msg, 2);
}
inline void shd_logh_error(const char *hnd_name, const char *msg) {
    shd_logh(hnd_name, msg, 3);
}

#endif /* __SHD_LOGGER_H__ */