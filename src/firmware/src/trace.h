
#ifndef __TRACE_H
#define __TRACE_H

#if defined(CFG_DEBUG) && (CFG_DEBUG == 1)

#define trace_printf    Serial.printf

int trace_init(void);
const char *trace_systime(void);
const char *trace_uptime(void);
void trace_dump(const void *buffer, int buff_len);

#if defined(CFG_TRACE_TIMESTAMP_SYSTIME) && (CFG_TRACE_TIMESTAMP_SYSTIME == 1)
 #define trace_timestamp  trace_systime
#else
 #define trace_timestamp  trace_uptime
#endif

typedef struct
{
   const char *name;
   uint8_t enabled;

} trace_tag_t;

/** Tag name definition */
#define TRACE_TAG(_name) \
  trace_tag_t trace_tag_##_name = {.name = #_name, .enabled = 1}; \
  static const trace_tag_t *trace_tag = &trace_tag_##_name; 

/** Tag reference by name definition */
#define TRACE_GROUP(_name) \
  extern trace_tag_t trace_tag_##_name;   \
  static const trace_tag_t *trace_tag = &trace_tag_##_name;

/** Tag reference */
#define TRACE_TAG_NAME(_name) \
  extern trace_tag_t trace_tag_##_name;  

#define TRACE_PRINTF(_format, ...) {              \
   if (trace_tag->enabled)                         \
      trace_printf(_format, ## __VA_ARGS__); \
}

#define TRACE(_format, ...) TRACE_PRINTF("%s %-14.14s     " _format "\r\n", trace_timestamp(), trace_tag->name, ## __VA_ARGS__)
#define TRACE_PRINTFF(_format, ...) TRACE_PRINTF("%s %-14.14s     " _format, trace_timestamp(), trace_tag->name, ## __VA_ARGS__)
#define TRACE_ERROR(_format, ...) TRACE_PRINTF("%s %-14.14s *E  " _format "   %s:%d\r\n" ), trace_timestamp(), trace_tag->name, ## __VA_ARGS__, __FILE__, __LINE__)

#define TRACE_DUMP trace_dump

#ifndef ASSERT
   #define ASSERT(EX) {                                                             \
      if (!(EX))                                                                    \
      {                                                                             \
         TRACE_ERROR("(%s) Assert failed at %s:%d", #EX, __FUNCTION__, __LINE__);   \
         while(1);                                                                  \
      }                                                                             \
   }
#endif   // ASSERT

#ifndef VERIFY
   #define VERIFY(EX) {                                                             \
      if (!(EX))                                                                    \
      {                                                                             \
         TRACE_ERROR("(%s) Verify failed at %s:%d", #EX, __FUNCTION__, __LINE__);   \
      }                                                                             \
   }
#endif   // VERIFY

#ifndef VERIFY_FATAL
   #define VERIFY_FATAL(EX) {                                                      \
      if (!(EX))                                                                   \
      {                                                                            \
         TRACE_ERROR("(%s) Fatal error at %s:%d", #EX, __FUNCTION__, __LINE__);    \
            while(1) {/*hal_delay_ms(80); hal_led_toggle(LED_ERROR); */}           \
      }                                                                            \
   }
#endif   // VERIFY_FATAL

#else

    #define trace_init()

    #define TRACE_TAG(_name) 
    #define TRACE(...)
    #define TRACE_PRINTFF(...)
    #define TRACE_PRINTF(...)
    #define TRACE_ERROR(...)
    #ifndef ASSERT
    #define ASSERT(...)
    #endif
    #define VERIFY(EX)  (EX)
    #define TRACE_DUMP

    #define VERIFY_FATAL(EX) {                                                 \
    if (!(EX))                                                               \
    {                                                                        \
        while(1) { /*hal_delay_ms(80);  hal_led_toggle(LED_ERROR); */}        \
    }                                                                        \
    }

#endif   // CFG_DEBUG

#endif   // __TRACE_H
