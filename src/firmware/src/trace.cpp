
#if defined(CFG_DEBUG) && (CFG_DEBUG == 1)

#include <Arduino.h>
#include <sys/time.h>

#include "trace.h"

int trace_init(void)
{
    Serial.begin(115200);
    return 0;
}

/** Trace current systime */
const char *trace_systime(void)
{
  static char strtm[48];
  int msec;
  struct timeval curtime;
  struct tm *tm;

  gettimeofday(&curtime, NULL);
  tm = localtime(&curtime.tv_sec);
  msec = (millis() % 1000);

  snprintf(strtm, sizeof(strtm), "%02d/%02d/%d %02d:%02d:%02d.%03d", tm->tm_mday, tm->tm_mon+1, tm->tm_year+1900, tm->tm_hour, tm->tm_min, tm->tm_sec, msec);

  return strtm;
}

/** trace current uptime */
const char *trace_uptime(void)
{
   static char buf[48];

   uint32_t duration = millis();
   uint32_t msec = duration % 1000;
   uint32_t seconds = duration / 1000;
   uint32_t minutes = seconds / 60;
   uint32_t hours = minutes / 60;
   uint32_t days = hours / 24;

  if (days > 0)
    snprintf(buf, sizeof(buf), "%dd %02d:%02d:%02d.%03d", days, hours % 24, minutes % 60, seconds % 60, msec);
   else
    snprintf(buf,  sizeof(buf), "%02d:%02d:%02d.%03d", hours % 24, minutes % 60, seconds % 60, msec);

    return buf;
}


/** Dump memory to trace */
void trace_dump(const void *buffer, int buff_len)
{
#define BYTES_PER_LINE  16
    int i;
    
    if ( buff_len == 0 ) 
        return;

    const char *pbuffer = (const char *)buffer;
    const char *ptr_line;
    //format: field[length]
    // ADDR[10]+"   "+DATA_HEX[8*3]+" "+DATA_HEX[8*3]+"  |"+DATA_CHAR[8]+"|"
    char hd_buffer[10+3+BYTES_PER_LINE*3+3+BYTES_PER_LINE+1+1];
    char *ptr_hd;
    int bytes_cur_line;


    do {
        if ( buff_len > BYTES_PER_LINE ) {
            bytes_cur_line = BYTES_PER_LINE;
        } else {
            bytes_cur_line = buff_len;
        }

        ptr_line = pbuffer;
        ptr_hd = hd_buffer;

        ptr_hd += sprintf( ptr_hd, "%p ", pbuffer );
        for(i = 0; i < BYTES_PER_LINE; i ++ ) {
            if ( (i&7)==0 ) {
                ptr_hd += sprintf( ptr_hd, " " );
            }
            if ( i < bytes_cur_line ) {
                ptr_hd += sprintf( ptr_hd, " %02X", (uint8_t )ptr_line[i] );
            } else {
                ptr_hd += sprintf( ptr_hd, "   " );
            }
        }
        ptr_hd += sprintf( ptr_hd, "  |" );
        for(i = 0; i < bytes_cur_line; i ++ ) {
            if ( isprint((int)ptr_line[i]) ) {
                ptr_hd += sprintf( ptr_hd, "%c", ptr_line[i] );
            } else {
                ptr_hd += sprintf( ptr_hd, "." );
            }
        }
       ptr_hd += sprintf( ptr_hd, "|" );

        trace_printf("%s\r\n", hd_buffer);

        pbuffer += bytes_cur_line;
        buff_len -= bytes_cur_line;

    } while( buff_len );
}

#endif    /* CFG_DEBUG */
