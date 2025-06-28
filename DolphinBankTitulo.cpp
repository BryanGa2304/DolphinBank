// CAJERO AUTOMÁTICO DOLPHINBANK
#include <iostream>     // Entrada y salida
#include <cstdio>       // Archivos binarios
#include <cstring>      // Cadenas char[]
#include <cctype>       // Validación de caracteres
#include <cstdlib>      // system(), exit()
#include <locale.h>     // Para soportar tildes
#include <limits>       // Para cin.ignore()
#include <sstream>      // Para validaciones con stringstream
#include <iomanip>      // Para setprecision y setw
#include <algorithm>    //remove_if
#include <math.h>       //Operaciones matemáticas   
#define MAX_TITULAR 100 // Longitud máxima del nombre del titular
#include <windows.h>    // Necesario para GetConsoleScreenBufferInfo

using namespace std;

// Estructura que representa los datos de cada cuenta
struct Cuenta {
    char titular[MAX_TITULAR]; // Nombre del titular
    int numCuenta;             // Número de cuenta (6 dígitos)
    int clave;                 // Clave de la cuenta (4 dígitos)
    float saldo;               // Saldo actual de la cuenta
};

#include <windows.h> // Necesario para GetConsoleScreenBufferInfo

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
    cout << texto << "\n";
}

// Elimina todos los espacios de una cadena
string limpiar_espacios(const string& s) {
    string resultado = s;
    resultado.erase(remove_if(resultado.begin(), resultado.end(), ::isspace), resultado.end());
    return resultado;
}

// Valida entrada decimal (float)
float val_float(const string& mensaje) {
    string entrada;
    float num;
    while (true) {
        centrarTexto(mensaje);
        getline(cin, entrada);
        if (limpiar_espacios(entrada).empty()) {
            centrarTexto("No se permite dejar el campo vacío o con solo espacios.\n");
            continue;
        }
        stringstream ss(entrada);
        if (ss >> num && ss.eof()) break;
        centrarTexto("Entrada no válida. Solo se permite un número decimal.\n");
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
            centrarTexto("No se permite dejar el campo vacío.\n");
            continue;
        }
        stringstream ss(entrada);
        if (ss >> num && ss.eof()) break;
        centrarTexto("Entrada no válida. Solo se permite un número entero.\n");
    }
    return num;
}

// Valida entrada de texto no vacía
string val_str(const string& mensaje) {
    string entrada;
    while (true) {
        centrarTexto(mensaje);
        getline(cin, entrada);
        if (limpiar_espacios(entrada).empty()) {
            centrarTexto("No se permite dejar el campo vacío.\n");
        } else break;
    }
    return entrada;
}

// Lee el nombre del titular y lo convierte a char[]
void leerTitular(char* destino, int maxTam) {
    string temp = val_str("Nombre del titular: ");
    strncpy(destino, temp.c_str(), maxTam);
    destino[maxTam - 1] = '\0'; // Final seguro
}

// Genera automáticamente el siguiente número de cuenta
int generarNumCuenta() {
    FILE* fichero = fopen("cuentas.dat", "rb");
    if (!fichero) return 1; // Si no hay archivo, comenzar desde 1

    Cuenta c;
    int maxNum = 0;

    while (fread(&c, sizeof(Cuenta), 1, fichero) == 1) {
        if (c.numCuenta > maxNum)
            maxNum = c.numCuenta;
    }

    fclose(fichero);
    return maxNum + 1;
}

// Verifica si ya existe una cuenta con ese número
bool existeCuenta(int numero) {
    FILE* fichero = fopen("cuentas.dat", "rb");
    if (!fichero) return false;

    Cuenta c;
    while (fread(&c, sizeof(Cuenta), 1, fichero) == 1) {
        if (c.numCuenta == numero) {
            fclose(fichero);
            return true;
        }
    }

    fclose(fichero);
    return false;
}

// Verifica si el número tiene la longitud deseada (en dígitos)
bool validarLongitud(int numero, int longitud_deseada) {
    int min = pow(10, longitud_deseada - 1);
    int max = pow(10, longitud_deseada) - 1;
    return numero >= min && numero <= max;
}

// Solicita y valida una clave de 4 dígitos
int validarClave() {
    int clave;
    do {
        clave = val("Clave: ");
        if (!validarLongitud(clave, 4)) {
            centrarTexto("ERROR. La clave debe tener exactamente 4 dígitos positivos\n");
        }
    } while (!validarLongitud(clave, 4));
    return clave;
}

// Crea una nueva cuenta bancaria y la guarda en el archivo binario
void crearCuenta() {
    Cuenta nueva;
    leerTitular(nueva.titular, MAX_TITULAR);

    nueva.numCuenta = generarNumCuenta();
    centrarTexto("\nNúmero de cuenta asignado: ");
    cout << setw(6) << setfill('0') << nueva.numCuenta << endl;

    nueva.clave = validarClave();

    do {
        nueva.saldo = val_float("Saldo inicial: ");
        if (nueva.saldo < 0) {
            centrarTexto("ERROR. El saldo inicial no puede ser negativo.\n");
        }
    } while (nueva.saldo < 0);

    FILE* fichero = fopen("cuentas.dat", "ab");
    if (fichero != NULL) {
        fwrite(&nueva, sizeof(Cuenta), 1, fichero);
        fclose(fichero);
        centrarTexto("\nCuenta creada exitosamente.\n");
    } else {
        centrarTexto("\nError al abrir el archivo para guardar la cuenta.\n");
    }
}

// Autentica una cuenta a partir del número y clave
bool autenticarCuenta(int numero, int clave, Cuenta &cuentaAutentificada) {
    FILE* fichero = fopen("cuentas.dat", "rb");
    if (!fichero) {
        centrarTexto("Error al abrir el archivo.\n");
        return false;
    }

    Cuenta temp;
    while (fread(&temp, sizeof(Cuenta), 1, fichero) == 1) {
        if (temp.numCuenta == numero && temp.clave == clave) {
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
    centrarTexto("\n========================================\n");
    cout << " TITULAR: " << cuenta.titular << endl;
    cout << " NÚMERO DE CUENTA: " << cuenta.numCuenta << endl;
    cout << " SALDO DISPONIBLE: $" << fixed << setprecision(2) << cuenta.saldo << endl;
    centrarTexto("========================================\n");
}

// Permite retirar dinero si hay saldo suficiente
void retirarDinero(Cuenta &cuentaAutentificada) {
    float montoRetiro = val_float("MONTO A RETIRAR: $");

    while (montoRetiro < 0) {
        montoRetiro = val_float("ERROR, EL MONTO NO PUEDE SER NEGATIVO. MONTO A RETIRAR: $");
    }

    while (montoRetiro > cuentaAutentificada.saldo) {
        montoRetiro = val_float("ERROR. SALDO INSUFICIENTE. VUELVA A INTRODUCIR EL MONTO: $");
    }

    FILE* fichero = fopen("cuentas.dat", "rb+");
    if (!fichero) {
        centrarTexto("Error al abrir el archivo.\n");
        return;
    }

    Cuenta temp;
    while (fread(&temp, sizeof(Cuenta), 1, fichero) == 1) {
        if (temp.numCuenta == cuentaAutentificada.numCuenta && temp.clave == cuentaAutentificada.clave) {
            cuentaAutentificada.saldo -= montoRetiro;
            temp.saldo = cuentaAutentificada.saldo;

            fseek(fichero, -sizeof(Cuenta), SEEK_CUR);
            fwrite(&temp, sizeof(Cuenta), 1, fichero);

            cout << "\nRetiro exitoso. Saldo actual: $" << fixed << setprecision(2) << cuentaAutentificada.saldo << endl;
            break;
        }
    }

    fclose(fichero);
}

// Permite consignar dinero a la cuenta
void consignarDinero(Cuenta &cuentaAutentificada) {
    float montoConsignacion = val_float("MONTO A CONSIGNAR: $");

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
        if (temp.numCuenta == cuentaAutentificada.numCuenta && temp.clave == cuentaAutentificada.clave) {
            cuentaAutentificada.saldo += montoConsignacion;
            temp.saldo = cuentaAutentificada.saldo;

            fseek(fichero, -sizeof(Cuenta), SEEK_CUR);
            fwrite(&temp, sizeof(Cuenta), 1, fichero);

            cout << "\nConsignación exitosa. Saldo actual: $" << fixed << setprecision(2) << cuentaAutentificada.saldo << endl;
            break;
        }
    }

    fclose(fichero);
}

void transferirDinero(Cuenta &cuentaAutentificada) {
    int receptor = val("DIGITE EL NÚMERO DE CUENTA A DONDE LLEGARÁ LA TRANSFERENCIA: ");

    // Validación: longitud y que no sea la misma cuenta
    while (!validarLongitud(receptor, 6) || receptor == cuentaAutentificada.numCuenta) {
        centrarTexto("El número de cuenta debe tener 6 dígitos y no puede ser el mismo que el suyo.\n");
        receptor = val("VUELVA A INGRESAR EL NÚMERO DE CUENTA DEL DESTINATARIO: ");
    }

    float monto = val_float("VALOR DE LA TRANSFERENCIA: $");
    while (monto <= 0 || monto > cuentaAutentificada.saldo) {
        centrarTexto("Error, el valor de la transferencia no puede ser menor a cero ni superar el saldo total de la cuenta.\n");
        monto = val_float("VUELVA A INGRESAR VALOR DE LA TRANSFERENCIA: $");
    }

    FILE* fichero = fopen("cuentas.dat", "rb+");
    if (!fichero) {
        centrarTexto("Error al abrir el archivo.\n");
        return;
    }

    Cuenta temp;
    bool destinoEncontrado = false;

    // Buscar y actualizar cuenta destino
    while (fread(&temp, sizeof(Cuenta), 1, fichero) == 1) {
        if (temp.numCuenta == receptor) {
            temp.saldo += monto;
            fseek(fichero, -sizeof(Cuenta), SEEK_CUR);
            fwrite(&temp, sizeof(Cuenta), 1, fichero);
            destinoEncontrado = true;
            break;
        }
    }

    if (!destinoEncontrado) {
        centrarTexto("Error. La cuenta destinataria no existe.\n");
        fclose(fichero);
        return; // ? Importante: salir sin modificar saldo del emisor
    }

    // Buscar y actualizar cuenta origen (emisor)
    rewind(fichero); // Volver al inicio
    while (fread(&temp, sizeof(Cuenta), 1, fichero) == 1) {
        if (temp.numCuenta == cuentaAutentificada.numCuenta) {
            temp.saldo -= monto;
            cuentaAutentificada.saldo = temp.saldo; // Actualizar también en RAM
            fseek(fichero, -sizeof(Cuenta), SEEK_CUR);
            fwrite(&temp, sizeof(Cuenta), 1, fichero);
            centrarTexto("\nTransferencia realizada exitosamente.\n");
            cout<< "Nuevo saldo: $" << fixed << setprecision(2) << cuentaAutentificada.saldo << "\n";
            break;
        }
    }

    fclose(fichero);
}

void menuOperaciones(Cuenta &cuentaAutentificada) {
    int opcion;
    do {
        centrarTexto("========= MENÚ DE OPERACIONES =========\n");
        centrarTexto("1. Consultar saldo\n");
        centrarTexto("2. Retirar dinero\n");
        centrarTexto("3. Consignar dinero\n");
        centrarTexto("4. Transferir dinero\n");
        centrarTexto("5. Cerrar sesión\n");
        centrarTexto("=======================================\n");
        opcion = val("Seleccione una opción: ");

        switch (opcion) {
            case 1:
                consultarSaldo(cuentaAutentificada);
                break;
            case 2:
                retirarDinero(cuentaAutentificada);
                break;
            case 3:
                consignarDinero(cuentaAutentificada);
                break;
            case 4:
                transferirDinero(cuentaAutentificada);
                break;
            case 5:
                centrarTexto("\nSesión cerrada correctamente.\n");
                break;
            default:
                centrarTexto("Opción inválida.\n");
        }

    } while (opcion != 5); // Cierra sesión terminando el bucle
}
void menuPrincipal() {
    int opcion;
    do {
        centrarTexto("========== MENÚ PRINCIPAL ===========\n");
        centrarTexto("1. Crear cuenta\n");
        centrarTexto("2. Iniciar sesión\n");
        centrarTexto("3. Salir\n");
        centrarTexto("===================================\n");
        opcion = val("Seleccione una opción: ");

        switch (opcion) {
            case 1:
                crearCuenta();
                break;

            case 2: {
                int numero = val("Ingrese su número de cuenta: ");
                int clave = val("Ingrese su clave: ");
                Cuenta cuentaAutentificada;

                if (autenticarCuenta(numero, clave, cuentaAutentificada)) {
                    centrarTexto("\nInicio de sesión exitoso.\n");
                    menuOperaciones(cuentaAutentificada);
                } else {
                    centrarTexto("Número de cuenta o clave incorrecta.\n");
                }
                break;
            }

            case 3:
                centrarTexto("Gracias por usar DolphinBank.\n");
                break;

            default:
                centrarTexto("Opción inválida.\n");
        }

    } while (opcion != 3); // Cierra sesión terminando el bucle
}

 main(){
 	system("COLOR 70");
 	
 	setlocale(LC_ALL, ""); // Habilitar tildes
 	cout<<"\t   _____    _____   __      ____   __   __   __   __    __      _____    _______   __    __    __  __ \n";
	cout<<"\t  |  _  \\  |  _  | |  |    |  _ | |  | |  | |  | |  \\  |  |    |  _  \\  |   _   | |  \\  |  |  |  |/  |\n";
	cout<<"\t  | | |  | | | | | |  |    |  __| |  |_|  | |  | |   \\ |  |    |  ___/  |  |_|  | |   \\ |  |  |     /\n";
	cout<<"\t  | |_|  | | |_| | |  |__  | |    |   _   | |  | |    \\|  |    |  _  \\  |   _   | |    \\|  |  |     \\\n";
	cout<<"\t  |_____/  |_____| |_____| |_|    |__| |__| |__| |__|\\____|    |_____/  |__| |__| |__|\\____|  |__|\\__|\n\n";
	menuPrincipal();
 }
