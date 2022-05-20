/*
 * log.h
 *
 *  Created on: 2021. 6. 2.
 *      Author: GP01
 */

#ifndef HEADER_LOG_H_
#define HEADER_LOG_H_

#define Lprintf(lev, fmt, ...) L_printf(__FUNCTION__, __LINE__, lev, fmt, ##__VA_ARGS__)

extern void L_printf(const char* func, char line, char _lev, const char *fmt, ...);

extern void log_write(void);

extern char* arg_cat(int argc, char *argv[]);

#endif /* HEADER_LOG_H_ */
