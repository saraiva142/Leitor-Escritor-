# 📦 Problema Leitor/Escritor com Memória Compartilhada e Semáforos em C

Este projeto implementa a comunicação entre processos usando memória compartilhada e semáforos, resolvendo o problema clássico de concorrência **Leitor-Escritor** com **preferência para leitores**.

## 🧠 Estrutura

A comunicação ocorre através de uma estrutura compartilhada:

```c
struct shared_use_st {
    int written_by_you;
    int nl;
    char some_text[2048];
};
```

- `written_by_you`: indica se o produtor já escreveu.
- `nl`: número de leitores lendo simultaneamente.
- `some_text`: área de texto compartilhada.

## 🛠️ Compilação

Compile os dois programas separadamente:

```bash
gcc escritor.c -o escritor
gcc leitor.c -o leitor
```

## ▶️ Execução

1. Execute o **escritor** em um terminal:

```bash
./escritor
```

2. Execute o **leitor** (um ou mais) em outros terminais:

```bash
./leitor
```

## 🔁 Lógica de Concorrência

Dois semáforos controlam o acesso:

- `db`: garante exclusividade para escritores.
- `mutex`: protege o contador de leitores (`nl`).

### 🧵 Regras:

- Leitores podem acessar simultaneamente.
- Escritor só entra quando não há leitores.
- Escritor espera até que `written_by_you == 0` para escrever novamente.

## 🧪 Exemplo de uso

```bash
Terminal 1 (Produtor):
Enter some text: Hello from writer!

Terminal 2 (Leitor):
You wrote: Hello from writer!
```

## 📄 Arquivos

- `escritor.c`: código do processo escritor.
- `leitor.c`: código do processo leitor.
- `shm_com.h`: definição da estrutura de memória compartilhada.
- `semun.h`: união necessária para usar `semctl()`.

## ✅ Requisitos

- Sistema Unix/Linux
- Compilador C (`gcc`)
- Familiaridade com IPC (Interprocess Communication)

## 📚 Referência

Implementa o modelo clássico de concorrência usando:
- Memória Compartilhada (`shmget`, `shmat`, `shmdt`)
- Semáforos (`semget`, `semop`, `semctl`)

---

Desenvolvido com 🧠 e 💻 para trabalho de Sistemas Operacionais 2 na Pontifícia Universidade Católica de Goiás.
