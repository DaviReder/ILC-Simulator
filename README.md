# 💻 SoftPLC (Simulador de PLC)

**Simulador de PLC** é um núcleo de processamento lógico de baixo nível desenvolvido em C, projetado para emular o comportamento rigoroso de um **SoftPLC (Software Programmable Logic Controller)**.  O foco principal é demonstrar alta competência em linguagem C, estruturas de dados complexas e sistemas concorrentes aplicados à automação.

Este projeto não é apenas uma simulação de variáveis; é um motor que traduz a jornada física do dado: desde a leitura de corrente (**4-20mA**) de sensores simulados, passando por uma conversão Analógico-Digital (ADC), até a manipulação de registradores de 32 bits para o acionamento de atuadores via lógica de bits **(*bitwise*)**.

---

<p align="center">
  <img src="assets/fluxograma.png" alt="Fluxograma do Sistema">
  <br>
  <em>O diagrama acima detalha a separação entre as threads de interface e a lógica de scan do PLC</em>
</p>

---

###  Tecnologias e Arquitetura

*   📜 **Log de Eventos & Huffman:** Registro histórico de todas as decisões tomadas pelo controlador e sua compressão ao atingir 100kb com o algoritmo de Huffman.
*   🔄 **Ciclo de Scan Industrial (Scan Cycle):** Execução ininterrupta da lógica de controle em malha fechada, com leitura de entradas, processamento de interrupções e atualização de saídas.
*   🌳 **Árvore AVL**: Utilização de uma **Árvore AVL** balanceada para registro e busca de sensores/atuadores, garantindo tempos de resposta constantes.
*   🔢 **Lógica Bitwise Multivariável**: Processamento de decisões através de registradores de 32 bits, permitindo que múltiplos atuadores respondam simultaneamente a diferentes condições de sensores.
*   🧵 **Operação Multithreading**: Separação total entre a camada de controle (Kernel) e a camada de visualização (IHM) através de Mutexes e POSIX Threads, evitando o bloqueio do processamento industrial por interrupções de usuário.
*   🖥️ **Interface estática:** Ao receber informações continua impriminto a mesma interface, facilitando perceber as alterações nos valores com o tempo.

---

### 🚀 Roadmap & Futuras Melhorias

Melhorias lineares que sugiro ao projeto:
* **Refatoração para Clean Code**: Revisão profunda da arquitetura para garantir que o projeto seja um exemplo de legibilidade e manutenibilidade. Além de uma busca profunda de vazamentos com Dr. Memory ou ferramentas auxiliares.
* **Evolução da Interface**: Transição do terminal para uma interface visual mais próxima de sistemas SCADA modernos.
* **PID Controller**: Implementação de controle Proporcional-Integral-Derivativo para substituir a lógica de histerese.

---

### 🛠️ Instalação e Execução

#### Pré-requisitos
* Compilador **GCC** (MinGW recomendado para usuários Windows).
* Suporte a **pthreads** (nativo no MinGW).
* Testado no Windows 10.

#### Passo a Passo
1.  **Clone o repositório:**
    ```bash
    git clone https://github.com/DaviReder/softplc.git
    cd softplc
    ```
2.  **Compile o projeto:**
    ```bash
    gcc main.c src/*.c -I include -lpthread -o softplc
    ```
3.  **Execute o simulador:**
    ```bash
    ./softplc
    ```

---

> 🎓 Sobre o autor: Estudante da PUC/MG.

> Este projeto é a intersecção entre o rigor da engenharia de hardware e a otimização de software de alto desempenho. Seu intuito é puramente didático, servindo como base para fortificar o aprendizado em estruturas de dados complexas e otimização de software. Sinta-se à vontade para explorar e sugerir melhorias!
