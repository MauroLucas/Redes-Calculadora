#include <iostream>
#include <winsock2.h>
#include <string>
#include <thread>
using namespace std;

//Varibles Globales
int tiempo = 0;
bool timeOut = false;

//Funciones Globales
void temporizador(int segundos);

//Clases
class Client{
public:
    WSADATA WSAData;
    SOCKET server;
    SOCKADDR_IN addr;
    char buffer[1024];
    char ip[1024];
    int puerto, tiempoMaximo;
    bool servidorConectado;

    Client()
    {
        memset(ip, 0, sizeof(ip));
        memset(buffer, 0, sizeof(buffer));
        servidorConectado = false;
        timeOut = false;
        tiempoMaximo = 180;

    }
    void IniciarCliente()
    {

        //Inicio el contador
        thread th(temporizador,tiempoMaximo);

        while(true){
        system("cls");
        cout<<"Conectando al servidor..."<<endl<<endl;
        WSAStartup(MAKEWORD(2,0), &WSAData);

        //Creacion del Socket
        server = socket(AF_INET, SOCK_STREAM, 0);
        if(server == INVALID_SOCKET)
        {
            cout<<"No se pudo crear el socket"<<endl;
            system("pause");
            WSACleanup();
            exit(1);
        }

        cout<<"Ingrese la direccion IP a la que desea conectarse"<<endl;
        cin>>this->ip;
        cout<<"Ingrese el puerto"<<endl;
        cin>>this->puerto;
        addr.sin_addr.s_addr = inet_addr(ip);
        addr.sin_family = AF_INET;
        addr.sin_port = htons(puerto);

        //Conectar al servidor
        int connResult = connect(server, (SOCKADDR *)&addr, sizeof(addr));
        if(connResult == SOCKET_ERROR)
        {
            closesocket(server);
            WSACleanup();
            cout<<"No se pudo conectar al servidor"<<endl;
            system("pause");
        }
        else{
            servidorConectado = true;
            tiempo = 0;

            while(servidorConectado)
            {
                MenuPrincipal();

            }
        }
        //Limpiar Winsock
        WSACleanup();
        }

    }

    void MenuPrincipal()
    {
        system("cls");
        int opcion = 0;
        cout<<"Que desea realizar?"<<endl;
        cout<<"1 - Realizar Calculo"<<endl;
        cout<<"2 - Ver registro de actividades"<<endl;
        cout<<"3 - Cerrar Sesion"<<endl;
        cin>>opcion;
        switch(opcion){
            case 1: RealizarCalculo();
            break;

            case 2: VerRegistroDeActividades();
            break;

            case 3: CerrarSocket();
            break;
            default: break;
        }

    }

    void Recibir()
    {
        recv(server, buffer, sizeof(buffer), 0);
        cout<<"Respuesta :"<< buffer << endl;
        memset(buffer, 0, sizeof(buffer));
    }

    bool SuperoElTiempo()
    {
        bool superoElTiempo = false;
        if(tiempo>this->tiempoMaximo){
            superoElTiempo = true;
        }
        return superoElTiempo;
    }
    void RealizarCalculo(){
        if(!SuperoElTiempo()){


        string input = "";
        int i,cantCaracteres = 0;
        bool mensajeValido = true;
        cout<<"Ingrese el calculo que desea realizar: "<<endl;
        getline(cin,input);
        while( input != "volver")
        {
            mensajeValido = true;
           getline(cin,input);
           if(strlen(input.c_str())>20 || input == ""){
            mensajeValido = false;
            cout<<"La operacion debe tener entre 1 y 20 caracteres"<<endl;
           }
           if(input != "volver" && mensajeValido){
            buffer[0] = 'a';
            for(i = 1; i<1024;i++){
                this->buffer[i] = input[i-1];
            }
            send(server, buffer, sizeof(buffer), 0);
            memset(buffer, 0, sizeof(buffer));
            tiempo = 0;
            Recibir();
        }
        }

        }
        else{
            cout<<"Cliente Desconectado por Inactividad"<<endl;
            buffer[0] = 'd';
            send(server,buffer,sizeof(buffer),0);
            memset(buffer, 0, sizeof(buffer));
            CerrarSocket();
        }
    }

    void VerRegistroDeActividades()
    {
        string linea="";
        if(!SuperoElTiempo())
        {
            this->buffer[0] = 'b';
            send(server,buffer,sizeof(buffer),0);
            memset(buffer, 0, sizeof(buffer));
            tiempo = 0;
            while(buffer[0] != 'E' && buffer[1] != 'O' && buffer[2] != 'F')
                {
                    recv(server, buffer, sizeof(buffer), 0);
                    if(buffer[0] != 'E' && buffer[1] != 'O' && buffer[2] != 'F')
                        {
                            cout<<buffer<<endl;
                        }

                }
             memset(buffer, 0, sizeof(buffer));

        }
        else
        {
            cout<<"Cliente Desconectado por Inactividad"<<endl;
            buffer[0] = 'd';
            send(server,buffer,sizeof(buffer),0);
            memset(buffer, 0, sizeof(buffer));
            CerrarSocket();

        }


      system("pause");
    }

    void CerrarSocket()
    {
       buffer[0] = 'c';
       send(server,buffer,sizeof(buffer),0);
       memset(buffer, 0, sizeof(buffer));
       servidorConectado = false;
       tiempo = 0;
       closesocket(server);
       cout<<"Se cerro la conexion con el servidor"<<endl;
       system("pause");

    }

};

 void temporizador(int segundos)
    {

        while(true)
        {
            Sleep(1000); //1 segundo
            tiempo++;

        }
    }

int main()
{
    Client *Cliente = new Client();
    Cliente->IniciarCliente();

}
