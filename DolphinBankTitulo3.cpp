// CAJERO AUTOM�TICO DOLPHINBANK
#include <iostream>     // Entrada y salida
#include <cstdio>       // Archivos binarios
#include <cstring>      // Cadenas char[]
#include <cctype>       // Validaci�n de caracteres
#include <cstdlib>      // system(), exit()
#include <locale.h>     // Para soportar tildes
#include <limits>       // Para cin.ignore()
#include <sstream>      // Para validaciones con stringstream
#include <iomanip>      // Para setprecision y setw
#include <algorithm>    //remove_if
#include <math.h>       //Operaciones matem�ticas   
#define MAX_TITULAR 100 // Longitud m�xima del nombre del titular
#include <windows.h>    // Necesario para GetConsoleScreenBufferInfo

using namespace std;

// Estructura que representa los datos de cada cuenta
struct Cuenta {
    char titular[MAX_TITULAR]; // Nombre del titular
    char numCuenta[7];             // N�mero de cuenta (6 d�gitos)
    char clave[5];                 // Clave de la cuenta (4 d�gitos)
    double saldo;               // Saldo actual de la cuenta
};
void color(int color){
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),color);
}

int anchoConsola() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
        return csbi.srWindow.Right - csbi.srWindow.Left + 1;
    else
        return 80; // Por defecto si no se puede obtener
}

void centrarTexto(const string& texto) {
    int ancho = anchoConsola();
    int espacios = (ancho - texto.length()) / 2;
    if (espacios > 0) cout<< string(espacios, ' ');
    cout <<texto ;
}
void titulo(){
	cout<<"\n\n\n\n\n\n";
	color(3);
 	centrarTexto("  _____    _____   __      ____   __   __   __   __    __      _____    _______   __    __    __  __ \n");
	centrarTexto(" |  _  \\  |  _  | |  |    |  _ | |  | |  | |  | |  \\  |  |    |  _  \\  |   _   | |  \\  |  |  |  |/  |\n");
	centrarTexto("| | |  | | | | | |  |    |  __| |  |_|  | |  | |   \\ |  |    |  ___/  |  |_|  | |   \\ |  |  |     /\n");
	centrarTexto("| |_|  | | |_| | |  |__  | |    |   _   | |  | |    \\|  |    |  _  \\  |   _   | |    \\|  |  |     \\\n");
	centrarTexto("  |_____/  |_____| |_____| |_|    |__| |__| |__| |__|\\____|    |_____/  |__| |__| |__|\\____|  |__|\\__|\n\n");
	color(15);
}
// Elimina todos los espacios de una cadena
string limpiar_espacios(const string& s) {
    string resultado = s;
    resultado.erase(remove_if(resultado.begin(), resultado.end(), ::isspace), resultado.end());
    return resultado;
}

// Valida entrada decimal (double)
double val_float(const string& mensaje) {
    string entrada;
    double num;
    while (true) {
        centrarTexto(mensaje);
        getline(cin, entrada);
        if (limpiar_espacios(entrada).empty()) {
            centrarTexto("No se permite dejar el campo vac�o o con solo espacios.\n");
            continue;
        }
        stringstream ss(entrada);
        if (ss >> num && ss.eof()) break;
        centrarTexto("Entrada no v�lida. Solo se permite un n�mero decimal.\n");
    }
    return num;
}

// Valida entrada entera (int o long)
long long int val(const string& mensaje) {
    string entrada;
    long long int num;
    while (true) {
        centrarTexto(mensaje);
        getline(cin, entrada);
        if (limpiar_espacios(entrada).empty()) {
            centrarTexto("No se permite dejar el campo vac�o.\n");
            continue;
        }
        stringstream ss(entrada);
        if (ss >> num && ss.eof()) break;
        centrarTexto("Entrada no v�lida. Solo se permite un n�mero entero.\n");
    }
    return num;
}

// Valida entrada de texto no vac�a
string val_str(const string& mensaje) {
    string entrada;
    while (true) {
        centrarTexto(mensaje);
        getline(cin, entrada);
        if (limpiar_espacios(entrada).empty()) {
            centrarTexto("No se permite dejar el campo vac�o.\n");
        } else break;
    }
    return entrada;
}

void barraCarga(const string& mensaje){
    cout<<mensaje;
    cout << "\n\n";
    cout<<"[";
    for(int i = 0; i < 30; ++i){
        cout <<"|" << flush;
        Sleep(30); // velocidad de carga
    }
    cout << "]\n\n";
}



// Lee el nombre del titular y lo convierte a char[]
void leerTitular(char* destino, int maxTam) {
    string temp = val_str("Nombre del titular: ");
    strncpy(destino, temp.c_str(), maxTam);
    destino[maxTam - 1] = '\0'; // Final seguro
}

// Genera autom�ticamente el siguiente n�mero de cuenta
void generarNumCuenta(char* destino) {
    FILE* fichero = fopen("cuentas.dat", "rb");
    int maxNum = 0;
    Cuenta c;
    while (fread(&c, sizeof(Cuenta), 1, fichero) == 1) {
        int num = atoi(c.numCuenta); // convertir de char[] a int
        if (num > maxNum) maxNum = num;
    }
    fclose(fichero);
    sprintf(destino, "%06d", maxNum + 1); // rellena con ceros a la izquierda
}


// Verifica si ya existe una cuenta con ese n�mero
bool existeCuenta(const char* numero) {
    FILE* fichero = fopen("cuentas.dat", "rb");
    if (!fichero) return false;

    Cuenta c;
    while (fread(&c, sizeof(Cuenta), 1, fichero) == 1) {
        if (strcmp(c.numCuenta, numero) == 0) {
            fclose(fichero);
            return true;
        }
    }

    fclose(fichero);
    return false;
}


// Verifica si el n�mero tiene la longitud deseada (en d�gitos)
bool validarLongitud(const char* numero, int longitud_deseada) {
    return strlen(numero) == longitud_deseada && all_of(numero, numero + longitud_deseada, ::isdigit);
}


// Solicita y valida una clave de 4 d�gitos
void validarClave(char* destino) {
    string clave;
    do {
        clave = val_str("Clave (4 d�gitos): ");
        if (clave.length() != 4 || !all_of(clave.begin(), clave.end(), ::isdigit)) {
            centrarTexto("ERROR. La clave debe tener exactamente 4 d�gitos num�ricos.\n");
        }
    } while (clave.length() != 4 || !all_of(clave.begin(), clave.end(), ::isdigit));
    strncpy(destino, clave.c_str(), 5);
    destino[4] = '\0';
}


// Crea una nueva cuenta bancaria y la guarda en el archivo binario
void crearCuenta() {
    Cuenta nueva;
    leerTitular(nueva.titular, MAX_TITULAR);

    generarNumCuenta(nueva.numCuenta);
	cout<<"\n";
    centrarTexto("N�mero de cuenta asignado: ");
    cout << setw(6) << setfill('0') << nueva.numCuenta << "\n\n";
	
    validarClave(nueva.clave);

    do {
    	cout<<"\n";
        nueva.saldo = val_float("Saldo inicial: ");
        if (nueva.saldo < 0) {
            centrarTexto("ERROR. El saldo inicial no puede ser negativo.\n");
        }
    } while (nueva.saldo < 0);

    FILE* fichero = fopen("cuentas.dat", "ab");
    if (fichero != NULL) {
        fwrite(&nueva, sizeof(Cuenta), 1, fichero);
        fclose(fichero);
        cout<<"\n";
        color(10);
        centrarTexto("Cuenta creada exitosamente.\n");
        color(15);
    } else {
        centrarTexto("\nError al abrir el archivo para guardar la cuenta.\n");
    }
}

// Autentica una cuenta a partir del n�mero y clave
bool autenticarCuenta(const char* numero, const char* clave, Cuenta &cuentaAutentificada){
    FILE* fichero = fopen("cuentas.dat", "rb");
    if (!fichero) {
        centrarTexto("Error al abrir el archivo.\n");
        return false;
    }

    Cuenta temp;
    while (fread(&temp, sizeof(Cuenta), 1, fichero) == 1) {
        if (strcmp(temp.numCuenta, numero) == 0 && strcmp(temp.clave, clave) == 0) {
            cuentaAutentificada = temp;
            fclose(fichero);
            return true;
        }
    }

    fclose(fichero);
    return false;
}

// Muestra los datos de la cuenta autenticada
void consultarSaldo(const Cuenta &cuenta) {
    centrarTexto("========================================\n");
    stringstream ss1,ss2,ss3;
    ss1<<" TITULAR: " << cuenta.titular << endl;
    ss2<<" N�MERO DE CUENTA: " << setw(6) << setfill('0') << cuenta.numCuenta<<endl;
    ss3<<" SALDO DISPONIBLE: $" << fixed << setprecision(2) << cuenta.saldo << endl;
    cout<<"\n";
    centrarTexto(ss1.str());
    cout<<"\n";
    centrarTexto(ss2.str());
    cout<<"\n";
    centrarTexto(ss3.str());
    cout<<"\n";
    centrarTexto("========================================\n");
}

// Permite retirar dinero si hay saldo suficiente
void retirarDinero(Cuenta &cuentaAutentificada) {
	stringstream ss;
	cout<<"\n";
    double montoRetiro = val_float("MONTO A RETIRAR: $");

    while (montoRetiro < 0) {
    	cout<<"\n";
        montoRetiro = val_float("ERROR, EL MONTO NO PUEDE SER NEGATIVO. MONTO A RETIRAR: $");
    }

    while (montoRetiro > cuentaAutentificada.saldo) {
    	cout<<"\n";
        montoRetiro = val_float("ERROR. SALDO INSUFICIENTE. VUELVA A INTRODUCIR EL MONTO: $");
    }

    FILE* fichero = fopen("cuentas.dat", "rb+");
    if (!fichero) {
        centrarTexto("Error al abrir el archivo.\n");
        return;
    }

    Cuenta temp;
    while (fread(&temp, sizeof(Cuenta), 1, fichero) == 1) {
        if (strcmp(temp.numCuenta, cuentaAutentificada.numCuenta) == 0 && strcmp(temp.clave, cuentaAutentificada.clave) == 0) {
            cuentaAutentificada.saldo -= montoRetiro;
            temp.saldo = cuentaAutentificada.saldo;

            fseek(fichero, -sizeof(Cuenta), SEEK_CUR);
            fwrite(&temp, sizeof(Cuenta), 1, fichero);
            cout<<"\n";
			color(10);
            ss << "Retiro exitoso. Saldo actual: $" << fixed << setprecision(2) << cuentaAutentificada.saldo << endl;
            centrarTexto(ss.str());
            cout<<"\n";
            color(15);
            break;
        }
    }

    fclose(fichero);
}

// Permite consignar dinero a la cuenta
void consignarDinero(Cuenta &cuentaAutentificada) {
    double montoConsignacion = val_float("MONTO A CONSIGNAR: $");
	stringstream ss;
    while (montoConsignacion < 0) {
        montoConsignacion = val_float("ERROR, EL MONTO NO PUEDE SER NEGATIVO. MONTO A CONSIGNAR: $");
    }

    FILE* fichero = fopen("cuentas.dat", "rb+");
    if (!fichero) {
        centrarTexto("Error al abrir el archivo.\n");
        return;
    }

    Cuenta temp;
    while (fread(&temp, sizeof(Cuenta), 1, fichero) == 1) {
        if (strcmp(temp.numCuenta, cuentaAutentificada.numCuenta) == 0 && strcmp(temp.clave, cuentaAutentificada.clave) == 0){
            cuentaAutentificada.saldo += montoConsignacion;
            temp.saldo = cuentaAutentificada.saldo;

            fseek(fichero, -sizeof(Cuenta), SEEK_CUR);
            fwrite(&temp, sizeof(Cuenta), 1, fichero);
			color(10);
	        ss << "Consignaci�n exitosa. Saldo actual: $" << fixed << setprecision(2) << cuentaAutentificada.saldo << endl;
            centrarTexto(ss.str());
	        color(15);
            break;
        }
    }

    fclose(fichero);
}

void transferirDinero(Cuenta &cuentaAutentificada) {
    char receptor[7];
    string temp;

    // Validar entrada del n�mero de cuenta destino
    do {
        temp = val_str("DIGITE EL N�MERO DE CUENTA A DONDE LLEGAR� LA TRANSFERENCIA: ");
    } while (temp.length() != 6 || !all_of(temp.begin(), temp.end(), ::isdigit) || strcmp(temp.c_str(), cuentaAutentificada.numCuenta) == 0);
    strncpy(receptor, temp.c_str(), 7);

    double monto = val_float("VALOR DE LA TRANSFERENCIA: $");
    while (monto <= 0 || monto > cuentaAutentificada.saldo) {
        centrarTexto("Error, el valor de la transferencia no puede ser menor a cero ni superar el saldo total de la cuenta.\n");
        monto = val_float("VUELVA A INGRESAR VALOR DE LA TRANSFERENCIA: $");
    }

    FILE* fichero = fopen("cuentas.dat", "rb+");
    if (!fichero) {
        centrarTexto("Error al abrir el archivo.\n");
        return;
    }

    Cuenta tempDestino;
    bool destinoEncontrado = false;

    // Buscar cuenta destino y actualizar su saldo
    while (fread(&tempDestino, sizeof(Cuenta), 1, fichero) == 1) {
        if (strcmp(tempDestino.numCuenta, receptor) == 0) {
            tempDestino.saldo += monto;
            fseek(fichero, -sizeof(Cuenta), SEEK_CUR);
            fwrite(&tempDestino, sizeof(Cuenta), 1, fichero);
            destinoEncontrado = true;
            break;
        }
    }

    if (!destinoEncontrado) {
        cout << "\n";
        color(4);
        centrarTexto("Error. La cuenta destinataria no existe.\n");
        color(15);
        fclose(fichero);
        return;
    }

    // Buscar cuenta origen (emisor) y restar el monto
    rewind(fichero);
    Cuenta tempOrigen;
    while (fread(&tempOrigen, sizeof(Cuenta), 1, fichero) == 1) {
        if (strcmp(tempOrigen.numCuenta, cuentaAutentificada.numCuenta) == 0) {
            tempOrigen.saldo -= monto;
            cuentaAutentificada.saldo = tempOrigen.saldo; // Actualizar en RAM
            fseek(fichero, -sizeof(Cuenta), SEEK_CUR);
            fwrite(&tempOrigen, sizeof(Cuenta), 1, fichero);

            cout << "\n";
            color(10);
            centrarTexto("Transferencia realizada exitosamente.\n");
            cout << "Nuevo saldo: $" << fixed << setprecision(2) << cuentaAutentificada.saldo << "\n";
            color(15);
            break;
        }
    }

    fclose(fichero);
}


void menuOperaciones(Cuenta &cuentaAutentificada) {
    int opcion;
    do {
        centrarTexto("========= MEN� DE OPERACIONES =========\n\n");
        centrarTexto("1. Consultar saldo\n\n");
        centrarTexto("2. Retirar dinero\n\n");
        centrarTexto("3. Consignar dinero\n\n");
        centrarTexto("4. Transferir dinero\n\n");
        centrarTexto("5. Cerrar sesi�n\n\n");
        centrarTexto("=======================================\n\n");
        opcion = val("Seleccione una opci�n: ");
		cout<<"\n";
        switch (opcion) {
            case 1:
            	system("cls");
                titulo();
            	consultarSaldo(cuentaAutentificada);
            	system("PAUSE");
                system("cls");
                titulo();
                
                break;
            case 2:
            	system("PAUSE");
                system("cls");
                titulo();
                retirarDinero(cuentaAutentificada);
                break;
            case 3:
            	system("PAUSE");
                system("cls");
                titulo();
                consignarDinero(cuentaAutentificada);
                break;
            case 4:
            	system("PAUSE");
                system("cls");
                titulo();
                transferirDinero(cuentaAutentificada);
                break;
            case 5:
                centrarTexto("\nSesi�n cerrada correctamente.\n");
                break;
            default:
            	color(4);
                centrarTexto("Opci�n inv�lida.\n");
                color(15);
                system("PAUSE");
                system("cls");
                titulo();
        }

    } while (opcion != 5); // Cierra sesi�n terminando el bucle
}
void menuPrincipal() {
    int opcion;
    do {
        centrarTexto("========== MEN� PRINCIPAL ===========\n\n");
        centrarTexto("1. Crear cuenta\n\n");
        centrarTexto("2. Iniciar sesi�n\n\n");
        centrarTexto("3. Salir\n\n");
        centrarTexto("===================================\n\n");
        opcion = val("Seleccione una opci�n: ");
		cout<<"\n";
        switch (opcion) {
            case 1:
                crearCuenta();
                break;

            case 2: {
                char numero[7];
				char clave[5];
				string temp;

				// Ingreso de n�mero de cuenta
				do {
    				temp = val_str("Ingrese su n�mero de cuenta (6 d�gitos): ");
				} while (temp.length() != 6 || !all_of(temp.begin(), temp.end(), ::isdigit));
				strncpy(numero, temp.c_str(), 7);

				// Ingreso de clave
				do {
					cout<<"\n";
    				temp = val_str("Ingrese su clave (4 d�gitos): ");
				} while (temp.length() != 4 || !all_of(temp.begin(), temp.end(), ::isdigit));
				strncpy(clave, temp.c_str(), 5);

                Cuenta cuentaAutentificada;

                if (autenticarCuenta(numero, clave, cuentaAutentificada)) {
                	color(10);
                	cout<<"\n";
                    barraCarga("Iniciando sesi�n...");
                    color(15);
                	system("cls");
                	titulo();
                	
                    menuOperaciones(cuentaAutentificada);
                } else {
                	color(4);
                    centrarTexto("N�mero de cuenta o clave incorrecta.\n\n");
                    color(15);
                    system("PAUSE");
                	system("cls");
                	titulo();
                }
                break;
            }

            case 3:
                centrarTexto("Gracias por usar DolphinBank.\n");
                break;

            default:
            	color(4);
                centrarTexto("Opci�n inv�lida.\n");
                color(15);
                system("PAUSE");
                system("cls");
                titulo();
        }

    } while (opcion != 3); // Cierra sesi�n terminando el bucle
}

int main(){
 	
 	system("PAUSE");
 	system("cls");
 	setlocale(LC_ALL, ""); // Habilitar tildes
 	titulo();
	menuPrincipal();
	centrarTexto("\nGracias por confiar en DolphinBank. �Hasta pronto!\n");

	return 0;
}
