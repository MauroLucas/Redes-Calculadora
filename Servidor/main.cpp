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
    char buffer[4000];
    char mensaje[4000];
    int puerto = 5000;
    bool clienteConectado;
    Server()
    {
        //Limpieza de datos
        memset(buffer, 0, sizeof(buffer));
        memset(mensaje, 0, sizeof(mensaje));
        clienteConectado = false;
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
      cout<<"Escuchando"<<endl;
      int resultado = recv(client, buffer, sizeof(buffer), 0);
      if(resultado == SOCKET_ERROR){
        cout<<"Error al intentar escuchar al cliente"<<endl;
        clienteConectado = false;
        //ArchivoLog("Error al intentar escuchar al cliente");
        //exit(1);
      }
      cout << "El cliente dice: " << buffer << endl;
      opcion = buffer[0];
      string mensaje;
      mensaje.assign(buffer);
      cout<<opcion<<endl;
      cout<<"Switch"<<endl;
      switch(opcion){
          case 'a': RealizarCalculo(mensaje);
          break;
          case 'b': EnviarArchivoLog();
          break;
          case 'c': CerrarSocket();
          break;
          default: break;
      }
      memset(buffer, 0, sizeof(buffer));
    }
    void EnviarMensaje(string m)
    {
        cout<<"Enviar Mensaje"<<endl;
        for(int i=0;i<(int)strlen(m.c_str());i++){
            this->mensaje[i] = m[i];
        }
        send(client, mensaje, sizeof(mensaje), 0);
        memset(mensaje, 0, sizeof(mensaje));

    }
    void Enviar()
    {
        send(client, mensaje, sizeof(mensaje), 0);
        memset(mensaje, 0, sizeof(mensaje));
        cout << "Mensaje enviado!" << endl;
    }
    void EnviarArchivoLog(){
        string linea;
        ifstream archivo("server.log");
        if(archivo){
            cout<<"archivo encontrado"<<endl;
            while(getline(archivo,linea)){
                    //cout<<linea<<endl;
                    for(int i =0; i<(int)strlen(linea.c_str())+1;i++){
                        buffer[i] = linea[i];
                    }
                send(client,buffer,sizeof(buffer),0);
                for(int i=0;i<sizeof(buffer);i++){
                    cout<<buffer[i];
                }
                cout<<endl;
                memset(buffer, 0, sizeof(buffer));

            }
            archivo.close();
            linea = "EOF";
            send(client,linea.c_str(),(int)strlen(linea.c_str())+1,0);
            memset(mensaje, 0, sizeof(mensaje));
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

        if(c == '+' || c == '-' || c == '*' || c == '/' || c == '!' || c== '^'){
            esValido = true;
        }
        if(c == '0' || c == '1' || c == '2' || c == '3' || c == '4' || c == '5' || c == '6' || c == '7' || c == '8' || c == '9'){
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
        bool operacionEncontrada = false;
        char operacion,a,b,c;
        int posicionOperacion = 0,cantCaracteres = 0,totCaracteres = 0, num1, num2;
        int i;

        //Busco la operacion
        for(i=1;mensaje[i]!='\0';i++){
                if(esOperacion(mensaje[i])){
                    operacionEncontrada = true;
                    operacion = mensaje[i];
                    posicionOperacion = i;
                }

        }
        totCaracteres = i-1;
        num1 = convertirAEntero(mensaje.substr(1,mensaje.find(operacion)-1));
        num2 = convertirAEntero(mensaje.substr(mensaje.find(operacion) + 1,totCaracteres - 1));
        cout<<"Validacion de *"<<endl;
        cout<<"Operacion : "<<operacion<<endl;

        if(esOperacion(mensaje[posicionOperacion - 1]) || esOperacion(mensaje[posicionOperacion + 1])){
            //esValido = false;
            EnviarMensaje("No se pudo realizar la operacion, la operacion esta mal formada: " + string(1,mensaje[posicionOperacion -1]) + string(1,mensaje[posicionOperacion]) + string(1,mensaje[posicionOperacion + 1]));
            return false;
        }

        if(operacion == '+'){
            if(esNumero(mensaje[posicionOperacion -1]) && esNumero(mensaje[posicionOperacion +1 ])){
                cout<<"La suma esta correcta"<<endl;
            }
        }

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
        if(operacion == '*'){
                if(esNumero(mensaje[posicionOperacion - 1]) && esNumero(mensaje[posicionOperacion + 1])){
                    cout<<"La operacion es correcta"<<endl;
                }
                else
                {
                    cout<<"La operacion es incorrecta"<<endl;
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




        return esValido;

    }



    void RealizarCalculo(string calculo){
        char operacion,a,b,c,caracterInvalido = true,operacionInvalida = true;
        string mensajeAux = "";
        bool caracterEncontrado = false;
        int i,num1,num2,posOperacion,totCaracteres=0, tipoError = 0;

        //tipoError = tipoErrorCalculo(calculo);
        //cout<<"tipo error "<<tipoError<<endl;
        //Se encontro un caracter invalido
        //Validar Caracteres Invalidos
        if(!ValidarCaracteresCalculo(calculo))
        {
            cout<<"Se encontraron caracteres invalidos"<<endl;
            caracterInvalido = false;
        }
        if(!ValidarOperacionCalculo(calculo))
        {
           cout<<"La operacion esta mal formada"<<endl;
           operacionInvalida = false;
        }


        //No se encontraron errores en la operacion
        if(caracterInvalido == true && operacionInvalida == true)
        {
            cout<<"No se encontraron errores"<<endl;
            for(i=1;calculo[i]!='\0';i++){
            if(calculo[i] == '+' || calculo[i] == '-' || calculo[i] == '/' || calculo[i] == '*' || calculo[i] == '!' || calculo[i] == '^'){
               operacion = calculo[i];
               posOperacion = i;
            }
        }
        totCaracteres = i - 1 ;
        cout<<"Total Caracteres " + to_string(totCaracteres)<<endl;
        //Primera Operador
        num1 = convertirAEntero(calculo.substr(1,calculo.find(operacion)-1));
        //Segundo Operador
        num2 = convertirAEntero(calculo.substr(calculo.find(operacion) + 1,totCaracteres - 1));
        cout<<"numero 1 " + to_string(num1)<<endl;
        cout<<"numero 2 " + to_string(num2)<<endl;
        switch(operacion){
            case '+': mensajeAux = to_string(num1+num2);

            break;
            case '-': mensajeAux = to_string(num1-num2);

            break;
            case '*': mensajeAux = to_string(num1*num2);

            break;
            case '/': mensajeAux = to_string(num1/num2);

            break;
            case '!': mensajeAux = to_string(factorial(num1));

            break;
            case '^': mensajeAux = to_string((int)pow(num1,num2));
            break;

            default:  mensajeAux = to_string(num1);
        }



         EnviarMensaje(mensajeAux);
        }


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
            exit(1);
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
