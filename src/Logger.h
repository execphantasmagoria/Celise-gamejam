#include <stdio.h>

#ifdef LOGGER_DEBUG

void LOGGER_INFO(const char* message);
void LOGGER_ERROR(const char* message);
void LOGGER_WARNING(const char* message);

#endif