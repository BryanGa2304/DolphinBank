//CAJERO AUTOMï¿½TICO DOLPHINBANK
#include <iostream>     // Entrada y salida
#include <cstdio>       // Archivos binarios
#include <cstring>      // Cadenas char[]
#include <cctype>       // Validaciï¿½n de caracteres
#include <cstdlib>      // system(), exit()
#include <locale.h>     // Tildes
#include <limits>       // cin.ignore
#include <sstream>      // Validaciones de entrada
#include <iomanip>      // Para setw y setfill


#define MAX_TITULAR 100 //Longitud mï¿½xima del nombre del titular
//Estructura de los datos de cada cuenta
struct Cuenta{
	char titular[MAX_TITULAR];//Nombre del titular de la cuenta
	int numCuenta;//Nï¿½mero de cuenta la cuï¿½l serï¿½ de 6 dï¿½gitos
	int clave;//Clave de cuenta la cuï¿½l serï¿½ de 4 dï¿½gitos
	float saldo;//Saldo de la cuenta
};

// Elimina espacios de una cadena
string limpiar_espacios(const string& s) {
    string resultado = s;
    resultado.erase(remove_if(resultado.begin(), resultado.end(), ::isspace), resultado.end());
    return resultado;
}

// Validar entrada decimal
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


// Validar entrada entera
long long int val(const string& mensaje) {
    string entrada;
    long long int num;
    while (true) {
        cout << mensaje;
        getline(cin, entrada);
        if (limpiar_espacios(entrada).empty()) {
            cout << "No se permite dejar el campo vacï¿½o o con solo espacios.\n";
            continue;
        }
        stringstream ss(entrada);
        if (ss >> num && ss.eof()) break;
        cout << "Entrada no vï¿½lida. Solo se permite un nï¿½mero entero.\n";
    }
    return num;
}

// Validar entrada de texto no vacï¿½a
string val_str(const string& mensaje) {
    string entrada;
    while (true) {
        cout << mensaje;
        getline(cin, entrada);
        if (limpiar_espacios(entrada).empty()) {
            cout << "No se permite dejar el campo vacï¿½o.\n";
        } else break;
    }
    return entrada;
}

void leerTitular(char* destino, int maxTam) {
    string temp = val_str("Nombre del titular: ");
    strncpy(destino, temp.c_str(), maxTam);
    destino[maxTam - 1] = '\0'; // Asegurar que termina en '\0'
}

int generarNumCuenta() {
    FILE* fichero = fopen("cuentas.dat", "rb");
    if (!fichero) return 1; // Primera cuenta si no hay archivo

    Cuenta c;
    int maxNum = 0;

    while (fread(&c, sizeof(Cuenta), 1, fichero) == 1) {
        if (c.numCuenta > maxNum)
            maxNum = c.numCuenta;
    }

    fclose(fichero);
    return maxNum + 1;
}


//Validaciï¿½n para evitar cuentas duplicadas
bool existeCuenta(int numero){
	FILE* fichero= fopen("cuentas.dat", "rb");//Abre el archivo en modo lectura
	if(fichero == NULL) return false;
	Cuenta c;
	//Mientras siga habiendo alguna cuenta en el archivo, verifique si los nï¿½meros coinciden
	while (fread(&c, sizeof(Cuenta), 1, fichero) == 1) {
		
        if (c.numCuenta == numero) {
        	fclose(fichero);
            return true;//Si coinciden, retorna true
        }
    }
    fclose(fichero);
	return false;//Si no coinciden, retorna false
}

bool validarLongitud(int numero, int longitud_deseada){
	int min=pow(10, longitud_deseada-1);
	int max=pow(10, longitud_deseada)-1;
	return numero>=min&&numero<=max;
}

int validarNumCuenta(){
	int numCuenta;
	bool valido=false;
	do{
		numCuenta=val("Nï¿½mero de cuenta: ");
		
		if(!validarLongitud(numCuenta, 6)){
			cout<<"ERROR. El nï¿½mero de cuenta debe tener exactamente 6 dï¿½gitos positivos";
		}else if(existeCuenta(numCuenta)){
			cout<<"ERROR. El nï¿½mero de cuenta ya existe";
    	}else{
			valido=true;
		}
	}while(!valido);
	return numCuenta;
}

int validarClave(){
	int clave;
	do{
		clave=val("Clave: ");
		if(!validarLongitud(clave, 4)){
			cout<<"ERROR. La clave debe tener exactamente 4 dï¿½gitos positivos";
		}
	}while(!validarLongitud(clave, 4));
	return clave;
}

void crearCuenta() {
    Cuenta nueva;

    leerTitular(nueva.titular, MAX_TITULAR);

    nueva.numCuenta = generarNumCuenta();
    cout << "\nNï¿½mero de cuenta asignado: ";
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
        cout << "\n? Cuenta creada exitosamente.\n";
    } else {
        cout << "\n? Error al abrir el archivo para guardar la cuenta.\n";
    }
}

bool autenticarCuenta(int numero, int clave, Cuenta &cuentaAutentificada){
	FILE* fichero=fopen("cuentas.dat","rb");
	if (!fichero) {
        cout << "Error al abrir el archivo.\n";
        return false;
    }
    
    Cuenta temp;
    while(fread(&temp, sizeof(Cuenta), 1, fichero)==1){
    	if(temp.numCuenta=numero&&temp.clave=clave){
    		cuentaAutentificada=temp;
    		fclose(fichero);
    		return true;
		}
	}
	fclose(fichero);
	return false;
}

void consultarSaldo(const Cuenta &cuenta) {
    cout << "\n========================================\n";
    cout << " TITULAR: " << cuenta.titular << endl;
    cout << " NÚMERO DE CUENTA: " << cuenta.numCuenta << endl;
    cout << " SALDO DISPONIBLE: $" << cuenta.saldo << endl;
    cout << "========================================\n";
}

void retirarDinero(Cuenta &cuentaAutentificada) {
    float montoRetiro = val_float("MONTO A RETIRAR: $");

    while (montoRetiro < 0) {
        montoRetiro = val_float("ERROR, EL MONTO NO PUEDE SER NEGATIVO. MONTO A RETIRAR: $");
    }

    while (montoRetiro > cuentaAutentificada.saldo) {
        montoRetiro = val_float("ERROR. SALDO INSUFICIENTE. VUELVA A INTRODUCIR EL MONTO: $");
    }

    // Se abre el archivo en modo lectura/escritura binaria
    FILE* fichero = fopen("cuentas.dat", "rb+");
    if (!fichero) {
        cout << "Error al abrir el archivo.\n";
        return;
    }

    Cuenta temp;
    while (fread(&temp, sizeof(Cuenta), 1, fichero) == 1) {
        // Encontrar la cuenta que coincide
        if (temp.numCuenta == cuentaAutentificada.numCuenta && temp.clave == cuentaAutentificada.clave) {
            // Actualizar saldo
            cuentaAutentificada.saldo -= montoRetiro;
            temp.saldo = cuentaAutentificada.saldo;

            // Ir hacia atrás para reescribir en la misma posición
            fseek(fichero, -sizeof(Cuenta), SEEK_CUR);
            fwrite(&temp, sizeof(Cuenta), 1, fichero);

            cout << "Retiro exitoso. Saldo actual: $" << fixed << setprecision(2) << cuentaAutentificada.saldo << endl;
            break;
        }
    }

    fclose(fichero);
}

void consignarDinero(Cuenta &cuentaAutentificada) {
    float montoConsignacion = val_float("MONTO A CONSIGNAR: $");

    while (montoConsignacion < 0) {
        montoConsignacion = val_float("ERROR, EL MONTO NO PUEDE SER NEGATIVO. MONTO A CONSIGNAR: $");
    }

    // Se abre el archivo en modo lectura/escritura binaria
    FILE* fichero = fopen("cuentas.dat", "rb+");
    if (!fichero) {
        cout << "Error al abrir el archivo.\n";
        return;
    }

    Cuenta temp;
    while (fread(&temp, sizeof(Cuenta), 1, fichero) == 1) {
        // Encontrar la cuenta que coincide
        if (temp.numCuenta == cuentaAutentificada.numCuenta && temp.clave == cuentaAutentificada.clave) {
            // Actualizar saldo
            cuentaAutentificada.saldo += montoConsignacion;
            temp.saldo = cuentaAutentificada.saldo;

            // Ir hacia atrás para reescribir en la misma posición
            fseek(fichero, -sizeof(Cuenta), SEEK_CUR);
            fwrite(&temp, sizeof(Cuenta), 1, fichero);

            cout << "\n=============================\n";
			cout << "Consignación exitosa. Saldo actual: $" << fixed << setprecision(2) << cuentaAutentificada.saldo << endl;
			cout << "=============================\n";
			break;
        }
    }

    fclose(fichero);
}
