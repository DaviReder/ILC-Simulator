#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <pthread.h>
#include "../include/log.h"

pthread_mutex_t trava_log = PTHREAD_MUTEX_INITIALIZER;

void log_evento(char *tipo, char *formato, ...) {
    pthread_mutex_lock(&trava_log);

    time_t tempoBruto = time(NULL);
    struct tm *t = localtime(&tempoBruto);
    char h[30];
    strftime(h, sizeof(h), "[%d/%m %H:%M:%S]", t);

    FILE *file = fopen("log.txt", "a");
    if(file != NULL){
        fprintf(file, "%s [%s] ", h, tipo);
        va_list args;
        va_start(args, formato);
        vfprintf(file, formato, args);
        va_end(args);
        fprintf(file, "\n");
        fclose(file);
    } else {
        printf("\nErro ao abrir o arquivo de log.");
    }

    pthread_mutex_unlock(&trava_log);
}
