```
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

#define MAX_TITULAR 100 // Longitud máxima del nombre del titular

using namespace std;

// Estructura que representa los datos de cada cuenta
struct Cuenta {
    char titular[MAX_TITULAR]; // Nombre del titular
    int numCuenta;             // Número de cuenta (6 dígitos)
    int clave;                 // Clave de la cuenta (4 dígitos)
    float saldo;               // Saldo actual de la cuenta
};

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
        cout << mensaje;
        getline(cin, entrada);
        if (limpiar_espacios(entrada).empty()) {
            cout << "No se permite dejar el campo vacío o con solo espacios.\n";
            continue;
        }
        stringstream ss(entrada);
        if (ss >> num && ss.eof()) break;
        cout << "Entrada no válida. Solo se permite un número decimal.\n";
    }
    return num;
}

// Valida entrada entera (int o long)
long long int val(const string& mensaje) {
    string entrada;
    long long int num;
    while (true) {
        cout << mensaje;
        getline(cin, entrada);
        if (limpiar_espacios(entrada).empty()) {
            cout << "No se permite dejar el campo vacío.\n";
            continue;
        }
        stringstream ss(entrada);
        if (ss >> num && ss.eof()) break;
        cout << "Entrada no válida. Solo se permite un número entero.\n";
    }
    return num;
}

// Valida entrada de texto no vacía
string val_str(const string& mensaje) {
    string entrada;
    while (true) {
        cout << mensaje;
        getline(cin, entrada);
        if (limpiar_espacios(entrada).empty()) {
            cout << "No se permite dejar el campo vacío.\n";
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
            cout << "ERROR. La clave debe tener exactamente 4 dígitos positivos\n";
        }
    } while (!validarLongitud(clave, 4));
    return clave;
}

// Crea una nueva cuenta bancaria y la guarda en el archivo binario
void crearCuenta() {
    Cuenta nueva;
    leerTitular(nueva.titular, MAX_TITULAR);

    nueva.numCuenta = generarNumCuenta();
    cout << "\nNúmero de cuenta asignado: ";
    cout << setw(6) << setfill('0') << nueva.numCuenta << endl;

    nueva.clave = validarClave();

    do {
        nueva.saldo = val_float("Saldo inicial: ");
        if (nueva.saldo < 0) {
            cout << "ERROR. El saldo inicial no puede ser negativo.\n";
        }
    } while (nueva.saldo < 0);

    FILE* fichero = fopen("cuentas.dat", "ab");
    if (fichero != NULL) {
        fwrite(&nueva, sizeof(Cuenta), 1, fichero);
        fclose(fichero);
        cout << "\nCuenta creada exitosamente.\n";
    } else {
        cout << "\nError al abrir el archivo para guardar la cuenta.\n";
    }
}

// Autentica una cuenta a partir del número y clave
bool autenticarCuenta(int numero, int clave, Cuenta &cuentaAutentificada) {
    FILE* fichero = fopen("cuentas.dat", "rb");
    if (!fichero) {
        cout << "Error al abrir el archivo.\n";
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
    cout << "\n========================================\n";
    cout << " TITULAR: " << cuenta.titular << endl;
    cout << " NÚMERO DE CUENTA: " << cuenta.numCuenta << endl;
    cout << " SALDO DISPONIBLE: $" << fixed << setprecision(2) << cuenta.saldo << endl;
    cout << "========================================\n";
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
        cout << "Error al abrir el archivo.\n";
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
        cout << "Error al abrir el archivo.\n";
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
