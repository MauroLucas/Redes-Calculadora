#include <iostream>
#include <winsock2.h>

using namespace std;

class Client{
public:
    WSADATA WSAData;
    SOCKET server;
    SOCKADDR_IN addr;
    char buffer[1024];
    char mensaje[4000];
    char ip[1024];
    int puerto;
    Client()
    {
        cout<<"Conectando al servidor..."<<endl<<endl;
        WSAStartup(MAKEWORD(2,0), &WSAData);
        server = socket(AF_INET, SOCK_STREAM, 0);
        cout<<"Ingrese la direccion IP a la que desea conectarse"<<endl;
        cin>>this->ip;
        cout<<"Ingrese el puerto"<<endl;
        cin>>this->puerto;
        addr.sin_addr.s_addr = inet_addr(ip);
        addr.sin_family = AF_INET;
        addr.sin_port = htons(puerto);
        connect(server, (SOCKADDR *)&addr, sizeof(addr));
        cout << "Conectado al Servidor!" << endl;
    }
    void Enviar()
    {
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
        recv(server, mensaje, sizeof(mensaje), 0);
        cout << mensaje << endl;
        memset(mensaje, 0, sizeof(mensaje));
    }

    void RealizarCalculo(){
        string bufferAux = "";
        int i,cantCaracteres = 0;
        bool mensajeValido = true;
        cout<<"Ingrese el calculo que desea realizar: ";
        do{
           cin>>bufferAux;
           if(strlen(bufferAux.c_str())>24){
            mensajeValido = false;
            cout<<"La operacion debe tener entre 1 y 20 caracteres"<<endl;
           }
           if(buffer != "volver" && mensajeValido){
            buffer[0] = 'a';
            for(i = 1; i<1024;i++){
                this->buffer[i] = bufferAux[i-1];
            }
            send(server, buffer, sizeof(buffer), 0);
            memset(buffer, 0, sizeof(buffer));
            //cout << "Mensaje enviado!" << endl;
            Recibir();
           }

        }while(bufferAux != "volver");
        //cin>>bufferAux;
        //cout<<strlen(bufferAux.c_str())<<endl;
        //if(bufferAux == "volver"){
          //  cout<<bufferAux<<endl;
        //}
        //if(strlen(bufferAux.c_str()) >24 ){
         //   cout<<"La operacion debe tener entre 1 y 20 Caracteres"<<endl;
       // }

        //*buffer[0] = 'a';
        //for(int i=1;i<1024;i++){
          //  this->buffer[i] = bufferAux[i-1];
        //}
        //send(server, buffer, sizeof(buffer), 0);
        //memset(buffer, 0, sizeof(buffer));
        //cout << "Mensaje enviado!" << endl;
       // Recibir();
    }
    void VerRegistroDeActividades(){
    }

    void CerrarSocket()
    {
       closesocket(server);
       WSACleanup();
       cout << "Socket cerrado." << endl << endl;
    }
};

int main()
{
    Client *Cliente = new Client();
    while(true)
    {
        Cliente->Enviar();
        //Cliente->Recibir();
    }
}
