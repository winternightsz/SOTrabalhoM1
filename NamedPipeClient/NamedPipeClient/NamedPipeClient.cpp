#include <windows.h>
#include <iostream>
#include <string>
#include <thread>

using namespace std;

#define PIPE_NUMEROS TEXT("\\\\.\\pipe\\PipeNumeros")       
#define PIPE_STRINGS TEXT("\\\\.\\pipe\\PipeStrings")       

void solicitarNumero() {
    HANDLE hPipe;
    char buffer[1024];
    DWORD bytesLidos, bytesEscritos;

    if (!WaitNamedPipe(PIPE_NUMEROS, NMPWAIT_WAIT_FOREVER)) {
        cout << "pipe ocupado" << endl;
        return;
    }

    //vai conectar ao pipe criado pelo servidor
    hPipe = CreateFile(
        PIPE_NUMEROS,
        GENERIC_READ | GENERIC_WRITE, //pro cliente poder ler e escrever no pipe
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    if (hPipe == INVALID_HANDLE_VALUE) {
        cout << "erro pra conectar no pipe de numeros codigo: " << GetLastError() << endl;
        return;
    }

    //vai solicitar numero varias vezes ao servidor
    for (int i = 0; i < 20; ++i) {
        string entradaUsuario = "NUMERO";
        WriteFile(hPipe, entradaUsuario.c_str(), entradaUsuario.size(), &bytesEscritos, NULL);

        if (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesLidos, NULL)) {
            buffer[bytesLidos] = '\0';  
            cout << "Numero recebido: " << buffer << endl;
        }

        Sleep(1000);  //espera 1 segundo entre as solicitacoes que ta fazendo pro servidor
    }

    
    CloseHandle(hPipe); //fecha o pipe pra liberar os recursos dessa conexao
}

//entradaUsuario.c_str() vai converte a string para um ponteiro de caracteres (const char*) porque o WriteFile espera um buffer de dados
//buffer[bytesLidos] = '\0' adiciona o terminador no final dos dados lidos pra transformar em uma string C valida

void solicitarString() {
    HANDLE hPipe;
    char buffer[1024];
    DWORD bytesLidos, bytesEscritos;

    if (!WaitNamedPipe(PIPE_STRINGS, NMPWAIT_WAIT_FOREVER)) {
        cout << "pipe ocupado" << endl;
        return;
    }

    
    hPipe = CreateFile(
        PIPE_STRINGS,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    if (hPipe == INVALID_HANDLE_VALUE) {
        cout << "erro pra conectar no pipe de strings codigo: " << GetLastError() << endl;
        return;
    }

    
    for (int i = 0; i < 20; ++i) {

        string entradaUsuario = "STRING";

        WriteFile(hPipe, entradaUsuario.c_str(), entradaUsuario.size(), &bytesEscritos, NULL);

        if (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesLidos, NULL)) {
            buffer[bytesLidos] = '\0';  
            cout << "String recebida: " << buffer << endl;
        }

        Sleep(1000);  
    }

    CloseHandle(hPipe);
}

int main() {
    
    thread threadNumeros(solicitarNumero);
    thread threadStrings(solicitarString);

    
    threadNumeros.join();
    threadStrings.join();

    cout << "Clientes encerrados." << endl;
    return 0;
}
