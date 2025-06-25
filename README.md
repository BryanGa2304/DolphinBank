```
//CAJERO AUTOMÁTICO DOLPHINBANK
//Juan Quintero y Bryan Garcés

#include <iostream>     // Entrada y salida
#include <cstdio>       // Archivos binarios
#include <cstring>      // Cadenas char[]
#include <cctype>       // Validación de caracteres
#include <cstdlib>      // system(), exit()
#include <locale.h>     // Tildes
#include <limits>       // cin.ignore
#include <sstream>      // Validaciones de entrada

#define MAX_TITULAR 100 //Longitud máxima del nombre del titular
//Estructura de los datos de cada cuenta
struct Cuenta{
	char titular[MAX_TITULAR];//Nombre del titular de la cuenta
	int numCuenta;//Número de cuenta la cuál será de 6 dígitos
	int clave;//Clave de cuenta la cuál será de 4 dígitos
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
            cout << "No se permite dejar el campo vacío o con solo espacios.\n";
            continue;
        }
        stringstream ss(entrada);
        if (ss >> num && ss.eof()) break;
        cout << "Entrada no válida. Solo se permite un número entero.\n";
    }
    return num;
}

// Validar entrada de texto no vacía
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

void leerTitular(char* destino, int maxTam) {
    string temp = val_str("Nombre del titular: ");
    strncpy(destino, temp.c_str(), maxTam);
    destino[maxTam - 1] = '\0'; // Asegurar que termina en '\0'
}


//Validación para evitar cuentas duplicadas

bool existeCuenta(int numero){
	FILE* fichero= fopen("cuentas.dat", "rb"); //Abre el archivo en modo lectura
	if (fichero == NULL) return false;
	Cuenta c;
	//Mientras siga habiendo alguna cuenta en el archivo, verifique si los números coinciden
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
		numCuenta=val("Número de cuenta: ");
		
		if(!validarLongitud(numCuenta, 6)){
			cout<<"ERROR. El número de cuenta debe tener exactamente 6 dígitos positivos";
		}else if(existeCuenta(numCuenta)){
			cout<<"ERROR. El número de cuenta ya existe";
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
			cout<<"ERROR. La clave debe tener exactamente 4 dígitos positivos";
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
