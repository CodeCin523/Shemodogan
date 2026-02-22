#include <shd/handler/logger.h>
#include <shd/shd.h>

#include <stdlib.h>
#include <string.h>
#include <time.h>

// #ifdef onxrt_OS_LINUX
    #include <unistd.h>
// #elifdef onxrt_OS_WIN
//     #include <windows.h>
//     #include <fileapi.h>
// #endif


static shd_hnd_logger_t *logger_dt = 0;
static const char LOG_LEVEL[][3] = { // use array to remove null terminator that will never be used.
    "MSG", "DBG", "WRN", "ERR"
};          


/* #region Logger # */
/* ################ */
void shd_log_flush() {
    if(!logger_dt || !logger_dt->pBuf)
        return;

    write(STDOUT_FILENO, logger_dt->pBuf, logger_dt->bufIndex);
    logger_dt->bufIndex = 0;
}

inline static size_t allowed_len(size_t len_msg, size_t len_required, size_t bufSize) {
    size_t allowed = bufSize - (len_required - len_msg);
    return len_msg > allowed? allowed : len_msg;
}

inline static u32 format_date(char *buf, u32 index, struct tm *local) {
    u16 tYear = local->tm_year + 1900,
        tDay  = local->tm_yday + 1;
    u8  tSec  = local->tm_sec,
        tMin  = local->tm_min,
        tHour = local->tm_hour;
        
    buf[index++] = '[';
    buf[index++] = (tYear / 1000) % 10 + '0';
    buf[index++] = (tYear / 100) % 10 + '0';
    buf[index++] = (tYear / 10) % 10 + '0';
    buf[index++] = (tYear) % 10 + '0';
    buf[index++] = '-';
    buf[index++] = (tDay / 100) % 10 + '0';
    buf[index++] = (tDay / 10) % 10 + '0';
    buf[index++] = (tDay) % 10 + '0';
    buf[index++] = ' ';
    buf[index++] = (tHour / 10) % 10 + '0';
    buf[index++] = (tHour) % 10 + '0';
    buf[index++] = ':';
    buf[index++] = (tMin / 10) % 10 + '0';
    buf[index++] = (tMin) % 10 + '0';
    buf[index++] = ':';
    buf[index++] = (tSec / 10) % 10 + '0';
    buf[index++] = (tSec) % 10 + '0';
    buf[index++] = ']';

    return index;
}
inline static u32 format_level(char *buf, u32 index, u8 level) {
    const char *level_str = LOG_LEVEL[level % 4];

    buf[index++] = '[';
    buf[index++] = level_str[0];
    buf[index++] = level_str[1];
    buf[index++] = level_str[2];
    buf[index++] = ']';

    return index;
}

// [YYYY-DDD HH:MM:SS] [LogLevel] - Message
void shd_log(const char *msg, u8 level) {
    if(!logger_dt)
        return;
    if(!msg)
        return;

    size_t len_msg = strlen(msg);
    size_t len_required =  
        19          /* timestamp */ +
        6           /* level     */ +
        len_msg + 3 /* message   */ +
        1           /* \n        */;
    if(logger_dt->bufIndex + len_required > logger_dt->bufSize)
        shd_log_flush();
    
    char *buf = logger_dt->pBuf;
    u32 index = logger_dt->bufIndex;

    len_msg = allowed_len(len_msg, len_required, logger_dt->bufSize - index);

    { // time
        time_t now;
        time(&now);
        struct tm *local = localtime(&now);

        index = format_date(buf, index, local);
    }
    // level
    buf[index++] = ' ';
    index = format_level(buf, index, level);

    // message
    buf[index++] = ' ';
    buf[index++] = '-';
    buf[index++] = ' ';
    memcpy(&buf[index], msg, len_msg);
    index += len_msg;
    buf[index++] = '\n';

    logger_dt->bufIndex = index;
}

// [YYYY-DDD HH:MM:SS] [LogLevel] [HandlerName] - Message
void shd_logh(const char *hnd_name, const char *msg, u8 level) {
    if(!logger_dt)
        return;
    if(!hnd_name || !msg)
        return;

    size_t len_msg = strlen(msg);
    size_t len_hnd = strlen(hnd_name);
    size_t len_required =  
        19          /* timestamp */ +
        6           /* level     */ +
        len_hnd + 3 /* hnd_name  */ +
        len_msg + 3 /* message   */ +
        1           /* \n        */;
    if(logger_dt->bufIndex + len_required > logger_dt->bufSize)
        shd_log_flush();
    
    char *buf = logger_dt->pBuf;
    u32 index = logger_dt->bufIndex;

    len_msg = allowed_len(len_msg, len_required, logger_dt->bufSize - index);

    { // time
        time_t now;
        time(&now);
        struct tm *local = localtime(&now);

        index = format_date(buf, index, local);
    }
    // level
    buf[index++] = ' ';
    index = format_level(buf, index, level);

    buf[index++] = ' ';
    buf[index++] = '[';
    memcpy(&buf[index], hnd_name, len_hnd);
    index += len_hnd;
    buf[index++] = ']';

    // message
    buf[index++] = ' ';
    buf[index++] = '-';
    buf[index++] = ' ';
    memcpy(&buf[index], msg, len_msg);
    index += len_msg;
    buf[index++] = '\n';

    logger_dt->bufIndex = index;
}
/* #endregion     # */
/* ################ */

/* #region Handler# */
/* ################ */
static shd_status_t logger_init(shd_basecrt_t *_creator) {
    if(!_creator)
        return SHD_STATUS_HANDLER_MISSING_CREATOR;
    if(_creator->type != SHD_CRTTYPE_LOGGER)
        return SHD_STATUS_HANDLER_INVALID_CREATOR;
    
    shd_crt_logger_t creator = {
        {0, 0}, 8192
    };
    if((_creator->flags & SHD_CRTFLAG_DEFAULT_CREATOR) == 0) {
        shd_crt_logger_t *_crt = (shd_crt_logger_t *) _creator;
        creator = *_crt;
    }

    shd_hnd_logger_t *dt = (shd_hnd_logger_t *) shd_handler_get(
        SHD_HND_LOGGER_ID, 
        &(shd_basegtr_t){ 0, SHD_GTRFLAG_DIRECT_INSTANCE }
    );
    if(!dt)
        return SHD_STATUS_FAILED;

    dt->pBuf = malloc(creator.bufferSize);
    if(!dt->pBuf)
        return SHD_STATUS_FAILED_EXTERN_ALLOC;

    dt->bufIndex = 0;
    dt->bufSize = creator.bufferSize;
    logger_dt = dt;

    return SHD_STATUS_SUCCESS;
}
static shd_status_t logger_term() {
    if(!logger_dt)
        return SHD_STATUS_SUCCESS;

    if(logger_dt->pBuf != 0) {
        if(logger_dt->bufIndex)
            shd_log_flush();
        free(logger_dt->pBuf);
    }

    logger_dt->pBuf = 0;
    logger_dt->bufIndex = 0;
    logger_dt->bufSize = 0;

    logger_dt = 0;

    return SHD_STATUS_SUCCESS;
}
static shd_basehnd_t *logger_get(shd_basegtr_t *_getter) {
    return 0;
}

shd_handler_meta_t SHD_HND_LOGGER_META = {
    logger_init,
    logger_term,
    logger_get,
    "shd_hndlogger",
    0,
    0,
    sizeof(shd_hnd_logger_t)
};
/* #endregion     # */
/* ################ */
