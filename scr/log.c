#include <stdio.h>
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
    pthread_mutex_lock(&trava_log);

    time_t tempoBruto = time(NULL);
    struct tm *t = localtime(&tempoBruto);
    char carimbo_tempo[30];
    strftime(carimbo_tempo, sizeof(carimbo_tempo), "[%d/%m %H:%M:%S]", t);

    const char *caminho_log = "data/log.txt";
    FILE *arquivo = fopen(caminho_log, "a");

    if (arquivo) {
        fprintf(arquivo, "%s [%s] ", carimbo_tempo, nivel);
        va_list args;
        va_start(args, formato);
        vfprintf(arquivo, formato, args);
        va_end(args);
        fprintf(arquivo, "\n");
        fclose(arquivo);

        // --- VERIFICAÇÃO DE ROTAÇÃO E COMPACTAÇÃO ---
        struct stat st;
        if (stat(caminho_log, &st) == 0 && st.st_size >= LIMITE_LOG_BYTES) {

            // Gerar nome do arquivo compactado: LOG_HHMMSS.dc
            char nome_compactado[50];
            char hora_compactada[20];
            strftime(hora_compactada, sizeof(hora_compactada), "%H%M%S", t);
            sprintf(nome_compactado, "data/LOG_%s.dc", hora_compactada);

            // Renomeia o log atual para um arquivo temporário para processamento
            // Isso evita que novas escritas ocorram no arquivo enquanto ele é lido
            const char *caminho_temp = "data/log_temp.txt";
            if (rename(caminho_log, caminho_temp) == 0) {

                // Executa a lógica de Huffman no arquivo temporário
                executar_compactacao_autonoma(caminho_temp, nome_compactado);

                // EXCLUSÃO: Remove o arquivo temporário de texto após compactar
                remove(caminho_temp);

                // Opcional: Registra no novo log que houve uma rotação
                printf("\n[SISTEMA] Log rotacionado para %s\n", nome_compactado);
            }
        }
    } else {
        fprintf(stderr, "ERRO CRITICO: Nao foi possivel gravar no log.txt\n");
    }

    pthread_mutex_unlock(&trava_log);
}
