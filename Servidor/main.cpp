#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <fstream>
#include <winsock2.h>
#include <string>
#include <math.h>

using namespace std;

class Server{
public:
    WSADATA WSAData;
    SOCKET server, client;
    SOCKADDR_IN serverAddr, clientAddr;
    char buffer[1024];
    char mensaje[1024];
    int puerto = 5000;
    bool clienteConectado;
    string ip;
    Server()
    {
        //Limpieza de datos
        memset(buffer, 0, sizeof(buffer));
        memset(mensaje, 0, sizeof(mensaje));
        clienteConectado = false;
        ip = "127.0.0.1";
    }

    void IniciarServidor()
    {
        //Blucle Infinito del servidor
        while(true){
        cout<<"Iniciar Servidor"<<endl;
        ArchivoLog("================================");
        ArchivoLog("==========Inicia Servidor=======");
        ArchivoLog("================================");
        WSAStartup(MAKEWORD(2,0), &WSAData);
        server = socket(AF_INET, SOCK_STREAM, 0);

        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(puerto);
        serverAddr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());

        bind(server, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
        cout << "Socket creado. Puerto de escucha " + to_string(puerto) << endl;
        ArchivoLog("Socket creado. Puerto de escucha " + to_string(puerto));
        listen(server, 0);


        int clientAddrSize = sizeof(clientAddr);
        if((client = accept(server, (SOCKADDR *)&clientAddr, &clientAddrSize)) != INVALID_SOCKET)
        {
            cout << "Cliente conectado!" << endl;
            ArchivoLog("Conexion Aceptada");
            clienteConectado = true;
            while(clienteConectado)
            {
                Recibir();

            }

        }
        //Limpiar Winsock
        WSACleanup();

        }
    }

    void Recibir()
    {
      char opcion;
      int resultado = recv(client, buffer, sizeof(buffer), 0);
      if(resultado == SOCKET_ERROR){
        cout<<"Error al intentar escuchar al cliente"<<endl;
        clienteConectado = false;

      }

      opcion = buffer[0];
      string mensaje;
      mensaje.assign(buffer);
      cout<<"mensaje :"<<mensaje<<endl;
      switch(opcion){
          case 'a': RealizarCalculo(mensaje);
          break;
          case 'b': EnviarArchivoLog();
          break;
          case 'c': CerrarSocket();
          break;
          case 'd': TiempoInactividad();
          default: break;
      }
      memset(buffer, 0, sizeof(buffer));
    }
    void EnviarMensaje(string m)
    {
        for(int i=0;i<(int)strlen(m.c_str());i++){
            this->mensaje[i] = m[i];
        }
        send(client, mensaje, sizeof(mensaje), 0);
        memset(mensaje, 0, sizeof(mensaje));

    }
    void EnviarMensaje()
    {
        send(client, mensaje, sizeof(mensaje), 0);
        memset(mensaje, 0, sizeof(mensaje));
        cout << "Mensaje enviado!" << endl;
    }
    void EnviarArchivoLog(){
        string linea;
        ifstream archivo("server.log");
        if(archivo){

            while(getline(archivo,linea)){

                    EnviarMensaje(linea);
            }
            archivo.close();
            linea = "EOF";
            EnviarMensaje(linea);

            }else{
                cout<<"error al intentar abrir el archivo server.log"<<endl;
            }

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
    int factorial(int x){
        int i, resultado = 1;
        for(i = 1; i<=x;i++){
            resultado = resultado * i;
        }
        return resultado;
    }
    bool validarCaracter(char c){
        bool esValido = false;

        if(esOperacion(c)){
            esValido = true;
        }
        if(esNumero(c)){
            esValido = true;
        }
        return esValido;
    }
    bool esOperacion(char c)
    {
        bool operacion = false;
        if(c == '+' || c == '-' || c == '*' || c == '/' || c == '!' || c== '^'){
            operacion = true;
        }
        return operacion;
    }
    bool esNumero(char c)
    {
        bool numero = false;
        if(c == '0' || c == '1' || c == '2' || c == '3' || c == '4' || c == '5' || c == '6' || c == '7' || c == '8' || c == '9'){
            numero = true;
        }
        return numero;
    }
    bool ValidarCaracteresCalculo(string mensaje)
    {
        bool esValido = true;
        char caracterInvalido;
        for(int i=1;mensaje[i]!='\0' && esValido == true ;i++){
            if(!validarCaracter(mensaje[i])){
                esValido = false;
                caracterInvalido = mensaje[i];
            }
        }
        if(!esValido)
        {
           EnviarMensaje("No se pudo realizar la operacion, se encontro un caracter no contemplado :" + string(1,caracterInvalido));
        }

        return esValido;
    }
    bool ValidarOperacionCalculo(string mensaje)
    {
        bool esValido = true;
        char operacion;
        int posicionOperacion = 0,totCaracteres = 0,num1, num2;
        int i;

        //Busco la operacion
        for(i=1;mensaje[i]!='\0';i++){
                if(esOperacion(mensaje[i])){
                    operacion = mensaje[i];
                    posicionOperacion = i;
                }

        }
        totCaracteres = i-1;
        num1 = convertirAEntero(mensaje.substr(1,mensaje.find(operacion)-1));
        num2 = convertirAEntero(mensaje.substr(mensaje.find(operacion) + 1,totCaracteres - 1));
        //Validaciones generales
        if(esOperacion(mensaje[posicionOperacion - 1]) || esOperacion(mensaje[posicionOperacion + 1])){
            //esValido = false;
            EnviarMensaje("No se pudo realizar la operacion, la operacion esta mal formada: " + string(1,mensaje[posicionOperacion -1]) + string(1,mensaje[posicionOperacion]) + string(1,mensaje[posicionOperacion + 1]));
            return false;
        }

        //Validaciones de operacion suma
        if(operacion == '+'){
            if(!esNumero(mensaje[posicionOperacion -1])  || !esNumero(mensaje[posicionOperacion + 1 ])){
                if(mensaje[posicionOperacion -1] == 'a')
                {
                     EnviarMensaje("No se pudo realizar la operacion, la operacion esta mal formada: " + string(1,mensaje[posicionOperacion]) + string(1,mensaje[posicionOperacion + 1]));
                }
                else
                {
                    EnviarMensaje("No se pudo realizar la operacion, la operacion esta mal formada: " + string(1,mensaje[posicionOperacion -1]) + string(1,mensaje[posicionOperacion]) + string(1,mensaje[posicionOperacion + 1]));
                }
                return false;
            }
        }

        //Validaciones de operacion resta
        if(operacion == '-'){
            if(!esNumero(mensaje[posicionOperacion -1])  || !esNumero(mensaje[posicionOperacion + 1 ])){
                if(mensaje[posicionOperacion -1] == 'a')
                {
                     EnviarMensaje("No se pudo realizar la operacion, la operacion esta mal formada: " + string(1,mensaje[posicionOperacion]) + string(1,mensaje[posicionOperacion + 1]));
                }
                else
                {
                    EnviarMensaje("No se pudo realizar la operacion, la operacion esta mal formada: " + string(1,mensaje[posicionOperacion -1]) + string(1,mensaje[posicionOperacion]) + string(1,mensaje[posicionOperacion + 1]));
                }
                return false;
            }
        }

        //Validaciones de tipo factorial
        if(operacion == '!'){
            if(esNumero(mensaje[posicionOperacion + 1])){
                esValido = false;
                //En caso de que el error se encuentre al principio
                if(mensaje[posicionOperacion - 1] == 'a')
                {
                    EnviarMensaje("No se pudo realizar la operacion, la operacion esta mal formada: " + string(1,mensaje[posicionOperacion]) + string(1,mensaje[posicionOperacion + 1]));
                }
                else
                {
                    EnviarMensaje("No se pudo realizar la operacion, la operacion esta mal formada: " + string(1,mensaje[posicionOperacion -1]) + string(1,mensaje[posicionOperacion]) + string(1,mensaje[posicionOperacion + 1]));
                }

            }
        }

        //Validaciones de tipo multiplicacion
        if(operacion == '*'){
                if(esNumero(mensaje[posicionOperacion - 1]) && esNumero(mensaje[posicionOperacion + 1])){
                    //la operacion es correcta
                }
                else
                {

                    if(mensaje[posicionOperacion - 1] == 'a')
                    {
                        EnviarMensaje("No se pudo realizar la operacion, la operacion esta mal formada: " + string(1,mensaje[posicionOperacion]) + string(1,mensaje[posicionOperacion + 1]));
                    }
                    else
                    {
                        EnviarMensaje("No se pudo realizar la operacion, la operacion esta mal formada: " + string(1,mensaje[posicionOperacion -1]) + string(1,mensaje[posicionOperacion]) + string(1,mensaje[posicionOperacion + 1]));
                    }
                    return false;

                }
        }

        //Validaciones de tipo division
        if(operacion == '/'){
            if(!esNumero(mensaje[posicionOperacion - 1]) || !esNumero(mensaje[posicionOperacion + 1])){
                if(mensaje[posicionOperacion - 1] == 'a')
                {
                    EnviarMensaje("No se pudo realizar la operacion, la operacion esta mal formada: " + string(1,mensaje[posicionOperacion]) + string(1,mensaje[posicionOperacion + 1]));

                }
                else
                {
                    EnviarMensaje("No se pudo realizar la operacion, la operacion esta mal formada: " + string(1,mensaje[posicionOperacion -1]) + string(1,mensaje[posicionOperacion]) + string(1,mensaje[posicionOperacion + 1]));
                }
                return false;
            }
            if(num2 == 0)
            {
                EnviarMensaje("No se puedo realizar la operacion, no se puede dividir por 0");
                return false;

            }
        }

        //Validaciones de tipo potencia
        if(operacion == '^'){
            if(!esNumero(mensaje[posicionOperacion -1])  || !esNumero(mensaje[posicionOperacion + 1 ])){
                if(mensaje[posicionOperacion -1] == 'a')
                {
                     EnviarMensaje("No se pudo realizar la operacion, la operacion esta mal formada: " + string(1,mensaje[posicionOperacion]) + string(1,mensaje[posicionOperacion + 1]));
                }
                else
                {
                    EnviarMensaje("No se pudo realizar la operacion, la operacion esta mal formada: " + string(1,mensaje[posicionOperacion -1]) + string(1,mensaje[posicionOperacion]) + string(1,mensaje[posicionOperacion + 1]));
                }
                return false;
            }
        }




        return esValido;

    }



    void RealizarCalculo(string calculo){
        char operacion,caracterInvalido = true,operacionInvalida = true;
        string respuesta = "";
        int i,num1,num2,totCaracteres=0;

        if(!ValidarCaracteresCalculo(calculo))
        {
            caracterInvalido = false;
        }
        if(!ValidarOperacionCalculo(calculo))
        {
           operacionInvalida = false;
        }


        //No se encontraron errores en la operacion
        if(caracterInvalido == true && operacionInvalida == true)
        {
            for(i=1;calculo[i]!='\0';i++){
            if(calculo[i] == '+' || calculo[i] == '-' || calculo[i] == '/' || calculo[i] == '*' || calculo[i] == '!' || calculo[i] == '^'){
               operacion = calculo[i];
            }
        }

        totCaracteres = i - 1 ;

        //Primera Operador
        num1 = convertirAEntero(calculo.substr(1,calculo.find(operacion)-1));

        //Segundo Operador
        num2 = convertirAEntero(calculo.substr(calculo.find(operacion) + 1,totCaracteres - 1));

        switch(operacion){
            case '+': respuesta = to_string(num1+num2);

            break;
            case '-': respuesta = to_string(num1-num2);

            break;
            case '*': respuesta = to_string(num1*num2);

            break;
            case '/': respuesta = to_string(num1/num2);

            break;
            case '!': respuesta = to_string(factorial(num1));

            break;
            case '^': respuesta = to_string((int)pow(num1,num2));
            break;

            default:  respuesta = to_string(num1);
        }



         EnviarMensaje(respuesta);
        }


    }

    void TiempoInactividad()
    {
        closesocket(client);
        clienteConectado = false;
        cout<<"Socket cerrado por inactividad del cliente"<<endl;
    }

    void CerrarSocket()
    {
        closesocket(client);
        clienteConectado = false;
        cout << "Socket cerrado, cliente desconectado." << endl;
    }

    string fechaHoraActual(){

        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);

        std::ostringstream oss;
        oss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
        auto str = oss.str();

        return str;

    }

    void ArchivoLog(string mensaje){
        fstream file;
        file.open("server.log", ios::app | ios::out);
        if(file.fail()){
            cout<<"Error al acceder al archivo server.log";

        }else{
            file <<fechaHoraActual()<<":"<< mensaje.c_str() << endl;
        }

    }
};


int main()
{
  Server *Servidor = new Server();
  Servidor->IniciarServidor();

}
