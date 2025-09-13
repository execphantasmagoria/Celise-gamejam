#include "Logger.h"

void LOGGER_INFO(const char* message)
{
	printf("[DEBUG INFO] %s\n", message);
}

void LOGGER_ERROR(const char* message)
{
	printf("[DEBUG ERROR] %s\n", message);
}

void LOGGER_WARNING(const char* message)
{
	printf("[DEBUG WARNING] %s\n", message);
}