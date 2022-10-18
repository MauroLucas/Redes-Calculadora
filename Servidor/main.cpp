#include <iostream>
#include <winsock2.h>

using namespace std;

class Server{
public:
    WSADATA WSAData;
    SOCKET server, client;
    SOCKADDR_IN serverAddr, clientAddr;
    char buffer[1024];
    Server()
    {
        WSAStartup(MAKEWORD(2,0), &WSAData);
        server = socket(AF_INET, SOCK_STREAM, 0);

        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(5555);

        bind(server, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
        listen(server, 0);

        cout << "Escuchando para conexiones entrantes." << endl;
        int clientAddrSize = sizeof(clientAddr);
        if((client = accept(server, (SOCKADDR *)&clientAddr, &clientAddrSize)) != INVALID_SOCKET)
        {
            cout << "Cliente conectado!" << endl;
        }
    }

    string Recibir()
    {
      char opcion;
      recv(client, buffer, sizeof(buffer), 0);
      cout << "El cliente dice: " << buffer << endl;
      opcion = buffer[0];
      string mensaje;
      mensaje.assign(buffer);
      cout<<opcion<<endl;
      switch(opcion){
          case 'a': RealizarCalculo(mensaje);
          break;
          default: break;
      }
      memset(buffer, 0, sizeof(buffer));
    }
    void Enviar()
    {
        cout<<"Escribe el mensaje a enviar: ";
        cin>>this->buffer;
        send(client, buffer, sizeof(buffer), 0);
        memset(buffer, 0, sizeof(buffer));
        cout << "Mensaje enviado!" << endl;
    }
    int convertirAEntero(string a){
        int i = 0;
        int num = 0;
        while (a[i] !=0){
            num = (a[i] - '0') + (num *10);
            i++;
        }
        return num;
    }
    void RealizarCalculo(string calculo){
        char operacion;
        int i,num1,num2,posOperacion,totCaracteres=0;
        //Tipo de Operacion
        for(i=1;calculo[i]!='\0';i++){
            if(calculo[i] == '+' || calculo[i] == '-' || calculo[i] == '/' || calculo[i] == '*' || calculo[i] == '!' || calculo[i] == '^'){
               operacion = calculo[i];
               posOperacion = i;
            }
        }
        totCaracteres = i - 1 ;
        //Primera Operador
        num1 = convertirAEntero(calculo.substr(1,calculo.find(operacion)-1));
        //Segundo Operador
        num2 = convertirAEntero(calculo.substr(calculo.find(operacion) + 1,totCaracteres - 1));
        switch(operacion){
            case '+': cout<<"La suma es ";
            cout<<num1+num2<<endl;
            break;
        }
        cout<<num1<<endl;
        cout<<num2<<endl;
        cout<<operacion<<endl;
    }

    void CerrarSocket()
    {
        closesocket(client);
        cout << "Socket cerrado, cliente desconectado." << endl;
    }
};


int main()
{
  Server *Servidor = new Server();
  while(true)
  {
     Servidor->Recibir();
     Servidor->Enviar();
  }
}
