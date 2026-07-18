#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <pthread.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../include/log.h"
#include "../include/codificar.h"

#define LIMITE_LOG_BYTES 102400 // 100 KB para teste

static pthread_mutex_t trava_log = PTHREAD_MUTEX_INITIALIZER;

void log_evento(const char *nivel, const char *formato, ...) {
    int disparar_compactacao = 0;
    char nome_compactado[50];
    const char *caminho_temp = "data/log_temp.txt";
    const char *caminho_log = "data/log.txt";

    // LOCK: Garante exclusão mútua estrita apenas para a operação no arquivo texto de log
    pthread_mutex_lock(&trava_log);

    time_t tempoBruto = time(NULL);
    struct tm *t = localtime(&tempoBruto);
    char carimbo_tempo[30];
    strftime(carimbo_tempo, sizeof(carimbo_tempo), "[%d/%m %H:%M:%S]", t);

    FILE *arquivo = fopen(caminho_log, "a");

    if (arquivo) {
        fprintf(arquivo, "%s [%s] ", carimbo_tempo, nivel);
        va_list args;
        va_start(args, formato);
        vfprintf(arquivo, formato, args);
        va_end(args);
        fprintf(arquivo, "\n");
        fclose(arquivo);

        // --- VERIFICAÇÃO DE ROTAÇÃO ---
        struct stat st;
        if (stat(caminho_log, &st) == 0 && st.st_size >= LIMITE_LOG_BYTES) {
            // Gerar nome do arquivo compactado: LOG_HHMMSS.dc
            char hora_compactada[20];
            strftime(hora_compactada, sizeof(hora_compactada), "%H%M%S", t);
            sprintf(nome_compactado, "data/LOG_%s.dc", hora_compactada);

            // Renomeia o log atual para libertar o caminho original imediatamente
            if (rename(caminho_log, caminho_temp) == 0) {
                disparar_compactacao = 1;
            }
        }
    } else {
        fprintf(stderr, "ERRO CRITICO: Nao foi possivel gravar no log.txt\n");
    }

    // UNLOCK: Libera o mutex AGORA. A thread do controlador ou da main já podem voltar a logar normalmente
    pthread_mutex_unlock(&trava_log);

    if (disparar_compactacao) {
        executar_compactacao_autonoma(caminho_temp, nome_compactado);

        // Remove o arquivo temporário após concluir a operação de I/O massiva
        remove(caminho_temp);

        printf("\n[SISTEMA] Log rotacionado com sucesso para %s\n", nome_compactado);
    }
}
