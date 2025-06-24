//CAJERO AUTOM�TICO DOLPHINBANK
#include <iostream>     // Entrada y salida
#include <cstdio>       // Archivos binarios
#include <cstring>      // Cadenas char[]
#include <cctype>       // Validaci�n de caracteres
#include <cstdlib>      // system(), exit()
#include <locale.h>     // Tildes
#include <limits>       // cin.ignore
#include <sstream>      // Validaciones de entrada

#define MAX_TITULAR 100 //Longitud m�xima del nombre del titular
//Estructura de los datos de cada cuenta
struct Cuenta{
	char titular[MAX_TITULAR];//Nombre del titular de la cuenta
	int numCuenta;//N�mero de cuenta la cu�l ser� de 6 d�gitos
	int clave;//Clave de cuenta la cu�l ser� de 4 d�gitos
	int saldo;//Saldo de la cuenta
};

// Elimina espacios de una cadena
string limpiar_espacios(const string& s) {
    string resultado = s;
    resultado.erase(remove_if(resultado.begin(), resultado.end(), ::isspace), resultado.end());
    return resultado;
}

// Validar entrada entera
long long int val(const string& mensaje) {
    string entrada;
    long long int num;
    while (true) {
        cout << mensaje;
        getline(cin, entrada);
        if (limpiar_espacios(entrada).empty()) {
            cout << "No se permite dejar el campo vac�o o con solo espacios.\n";
            continue;
        }
        stringstream ss(entrada);
        if (ss >> num && ss.eof()) break;
        cout << "Entrada no v�lida. Solo se permite un n�mero entero.\n";
    }
    return num;
}

// Validar entrada de texto no vac�a
string val_str(const string& mensaje) {
    string entrada;
    while (true) {
        cout << mensaje;
        getline(cin, entrada);
        if (limpiar_espacios(entrada).empty()) {
            cout << "No se permite dejar el campo vac�o.\n";
        } else break;
    }
    return entrada;
}

void leerTitular(char* destino, int maxTam) {
    string temp = val_str("Nombre del titular: ");
    strncpy(destino, temp.c_str(), maxTam);
    destino[maxTam - 1] = '\0'; // Asegurar que termina en '\0'
}


//Validaci�n para evitar cuentas duplicadas
bool existeCuenta(int numero){
	FILE* fichero= fopen("cuentas.dat", "rb");//Abre el archivo en modo lectura
	if(fichero == NULL) return false;
	Cuenta c;
	//Mientras siga habiendo alguna cuenta en el archivo, verifique si los n�meros coinciden
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
		numCuenta=val("N�mero de cuenta: ");
		
		if(!validarLongitud(numCuenta, 6)){
			cout<<"ERROR. El n�mero de cuenta debe tener exactamente 6 d�gitos positivos";
		}else if(existeCuenta(numCuenta)){
			cout<<"ERROR. El n�mero de cuenta ya existe";
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
			cout<<"ERROR. La clave debe tener exactamente 4 d�gitos positivos";
		}
	}while(!validarLongitud(clave, 4));
	return clave;
}

void crearCuenta(){
	Cuenta nueva;
	leerTitular(nueva.titular, MAX_TITULAR);
	nueva.numCuenta=validarNumCuenta();
	nueva.clave=validarClave();
	do{
		nueva.saldo=val("Saldo inicial: ");
		if(nueva.saldo<0){
			cout<<"ERROR. El saldo inicial no puede ser negativo.";
		}	
	}while(nueva.saldo<0);
	FILE* fichero=fopen("cuentas.dat", "ab");
	if(fichero!= NULL){
		fwrite(&nueva, sizeof(Cuenta), 1, fichero);
		fclose(fichero);
		cout << "\nCuenta creada exitosamente.\n";
	}else{
		cout << "\n? Error al abrir el archivo para guardar la cuenta.\n";
	}
}
