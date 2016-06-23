#include <iostream>
#include <cmath>
#include <list>
#include <sstream>
#include <map>

/** Fecha: 05/07/2013

Analiza una expressión en formato texto y resuelve las operaciones pertinentes

*/

const int MAX =5;

using namespace std;

inline void ResolverParentesis(string &Entrada,short &i);
double Ejecucion(string &Entrada);
double ConvertirNumero(string &Entrada,int &Index);
double RealizarOperacion(string &Entrada);
int ObtenerOperadorAnterior(string &Entrada,int Index);
double RealizarOperacion(string &Entrada);

double ConvertirAngulo(double Entrada)
{
    return Entrada *3.141592/180;
}

inline void ResolverFunciones(string& Entrada);

const char Operaciones[MAX] = {'^','/','*','+','-'};

typedef double (*Puntero_A_Funcion)(double);

map<string,Puntero_A_Funcion> Punteros;
map<string,double> Macros;

int main()
{
    Punteros["sin"] = sin;
    Punteros["cos"] = cos;
    Punteros["tan"] = tan;

    Punteros["sinh"] = sinh;
    Punteros["cosh"] = cosh;
    Punteros["tanh"] = tanh;

    Punteros["asin"] = asin;
    Punteros["acos"] = acos;
    Punteros["atan"] = atan;

    Punteros["asinh"] = asinh;
    Punteros["acosh"] = acosh;
    Punteros["atanh"] = atanh;

    Punteros["cbrt"] = cbrt;
    Punteros["sqrt"] = sqrt;

    Punteros["log"] = log;
    Punteros["log10"] = log10;

    Punteros["abs"] = abs;

    Punteros["rad"] = ConvertirAngulo;

    Macros["pi"] = 3.141592654;
    Macros["euler"] = 2.7182818;
    Macros["aureo"] = 1.61803398;
    Macros["ans"] = 0;

    string Expresion;
    cout<<"Calculadora parser por amchacon"<<endl<<endl;

    cout<<"Soporte para las siguientes funciones matematicas: "<<endl<<endl;

    short Contador = 0;

    for (auto it = Punteros.begin(); it != Punteros.end(); it++)
    {
        cout<<it->first<<"\t";
        Contador++;
        if (Contador == 3)
        {
            Contador = 0;
            cout<<endl;
        }
    }

    cout<<endl<<endl<<"Constantes definidas: "<<endl;

    Contador = 0;
    for (auto it = Macros.begin(); it != Macros.end(); it++)
    {
        cout<<it->first<<"\t";
        Contador++;
        if (Contador == 3)
        {
            Contador = 0;
            cout<<endl;
        }
    }

    cout<<endl<<endl;

    while(1)
    {

        cout << "Introduce tu expresion: " << endl;
        getline(cin,Expresion);

        try
        {
            cout<<endl<<"El resultado es: "<<RealizarOperacion(Expresion);
        }
        catch(const char* c)
        {
            cout<<endl<<c;
        }

        cout<<endl<<endl<<endl;

    }
    return 0;
}

double RealizarOperacion(string &Entrada)
{
    int Index;

    if (Entrada.empty())
        throw "Error, expresion vacia";

    // Macros

    for (auto it = Macros.begin();it != Macros.end();it++)
    {
        Index = Entrada.find(it->first);

        if (Index != -1)
        {
            Entrada.erase(Index,it->first.size());
            stringstream Cosita;
            Cosita<<it->second;
            Entrada.insert(Index,Cosita.str());
        }
    }

    // Funciones matemáticas

    ResolverFunciones(Entrada);
    Macros["ans"] =  Ejecucion(Entrada);

    return Macros["ans"];
}

/** Función principal que resuelve la operación **/

double Ejecucion(string &Entrada)
{
    // Comprobando paréntesis

    for (short i = 0; i < Entrada.size(); i++)
    {
        if (Entrada[i] == '(') // Si hay un parentesis
        {
            ResolverParentesis(Entrada,i);
        }
    }

    /** Empezamos a resolver las operaciones **/

    for (short j = 0; j < MAX; j++) // Recorremos las 4 operaciones
        for (short i = 0; i < Entrada.size(); i++)
        {
            if (Entrada[i] == Operaciones[j]) // Si encontramos nuestra operación
            {
                int Inicio = ObtenerOperadorAnterior(Entrada,i); // Buscamos el operador que le precede (o el 0)

                if (Inicio == -1)
                    continue;

                int Index = Inicio; // El index es nuestra posicion actual
                int Tam = Index; // El tamanyo de la expresion que contiene nuestra operacion
                double Operacion = ConvertirNumero(Entrada,Index); // Obtenemos el primer numero

                // Eliminando espacios en blanco...

                while(Entrada[Index] == ' ')
                {
                    Index++;
                }

                Index++; // Eliminamos el operador

                double Numerito = ConvertirNumero(Entrada,Index); // Obtenemos el segundo numero de la operacion

                switch (j) // Dependiendo de la operacion que estabamos buscando hacemos una cosa o otra
                {
                case 0:
                    Operacion = pow(Operacion,Numerito);
                    break;
                case 1:
                    if (!Numerito)
                        throw "Error, division por cero";
                    Operacion /= Numerito;
                    break;
                case 2:
                    Operacion *= Numerito;
                    break;
                case 3:
                    Operacion += Numerito;
                    break;
                case 4:
                    Operacion -= Numerito;
                }

                // El tamanyo de nuestra expresión es final (Index) menos inicio

                Tam = Index-Inicio;

                // Actualizamos nuestra expression

                Entrada.erase(Inicio,Tam); // Borramos la antigua

                // Obtenemos la expression númerica de la nueva

                stringstream Buffer;
                Buffer<<Operacion;

                // Y la insertamos

                Entrada.insert(Inicio,Buffer.str());

                // Volvemos al principio

                i = 0;
            }
        }

    /** Ahora unicamente nos queda una expresión númerica, la pasamos a número y la devolvemos **/

    int Index = 0;
    return ConvertirNumero(Entrada,Index);
}

inline void ResolverFunciones(string& Entrada)
{
    int Index;
    for (auto it = Punteros.begin(); it != Punteros.end(); it++)
    {
        Index = Entrada.find(it->first);
        if (Index != -1)
        {
            int Inicio = Index;
            Index = Inicio+it->first.size();
            while(Entrada[Index] == ' ')
            {
                Index++;
            }
            short aux = Index;
            if (Entrada[Index] == '(')
            {
                 ResolverParentesis(Entrada,aux);
            }


            if ((Entrada[Index] >= '0' && Entrada[Index] <= '9') || (Entrada[Index] == '-') || Entrada[Index] == '+')
            {
                stringstream Cosita;
                Cosita<<it->second(ConvertirNumero(Entrada,Index));
                Entrada.erase(Inicio,Index-Inicio);
                Entrada.insert(Inicio,Cosita.str());
            }
        }
    }
}

inline void ResolverParentesis(string &Entrada,short &i)
{
    short Toque = 1; // Buscamos el siguiente parentesis
    string Buffer; // Aquí guardaremos la expresión inscrita en el paréntesis
    bool Correcto = false; // Variable que comprueba si se llego al final del parentesis

//    for (auto it = Punteros.begin(); it != Punteros.end(); it++)
//    {
//        if (Entrada)
//    }

    for (short j = (i+1); j < Entrada.size(); j++)
    {
        Buffer+= Entrada[j]; // Vamos guardando la expresion...

        /** Si encontramos un paréntesis anidado, tendremos que encontrar el siguiente cierre de paréntesis **/

        if (Entrada[j] == '(')
            Toque++;
        if (Entrada[j] == ')') // Cierre de parentesis, un parentesis menos que buscar
            Toque--;

        if (Toque == 0) // Si ya hemos terminado
        {
            Buffer.erase(Buffer.size()-1,1); // Borramos el ')'

            ResolverFunciones(Buffer);

            /** Aplicamos recursividad, resolvemos la operación inscrita en el paréntesis **/

            Ejecucion(Buffer);

            /** Borramos nuestro paréntesis y lo sustituimos por nuestro nuevo valor **/

            Entrada.erase(i,j-i+1);
            Entrada.insert(i,Buffer);
            i = 0;
            Correcto = true; // Finalizo correctamente
            break;
        }
    }

    if (!Correcto) // Si no se encontró el final del paréntesis
    {
        throw "No se encontro el fin del parentesis \n";
    }
}

// Dada la posición de un operador, devuelve la posición contigua del operador anterior (o 0)

int ObtenerOperadorAnterior(string &Entrada,int Index)
{
    Index--; // nos saltamos nuestro operador
    if (Index < 0)
        return -1;

    // Vamos recorriendo nuestra entrada hasta que lo encontramos

    while (Entrada[Index] != '+' && Entrada[Index] != '-' && Entrada[Index] != '*' && Entrada[Index] != '/' && Index != 0)
    {
        Index--;
    }

    if (Index != 0)
        return Index+1; // Devolvemos la posición contigua
    else
        return 0;
}

/** El santo grial de todo, convierte una expressión de texto en un número **/

double ConvertirNumero(string &Entrada,int &Index)
{
    list<double> Enteros;
    list<double> Decimales;
    short aux;
    short signo = 1;

    // Nos saltamos los espacios en blanco

    while(Entrada[Index] == ' ')
    {
        Index++;
    }

    if (Entrada[Index] == '-')
    {
        signo = -1;
        Index++;
    }

    /** Ahora debería haber números, de lo contrario tenemos un error **/

    if (Entrada[Index] < '0' || Entrada[Index] > '9')
    {
        stringstream buffer;
        buffer<<"Error de sintaxis, desconocido elemento: '"<<Entrada[Index]<<"' en la posicion: "<<Index+1;
        throw buffer.str().c_str();
    }

    /** Añadimos los enteros **/

    while (Entrada[Index] >= '0' && Entrada[Index] <= '9' && Index < Entrada.size())
    {
        Enteros.push_back(Entrada[Index]-48);
        //cout<<Cosa[i]<<endl;
        Index++;
    }

    /** Si a continuación encontramos un punto o una coma, esque hay numeros a continuacion **/

    if (Entrada[Index] == '.' || Entrada[Index] == ',')
    {
        bool Activado = false;
        Index++;
        while (Entrada[Index] >= '0' && Entrada[Index] <= '9' && Index < Entrada.size())
        {
            Decimales.push_back(Entrada[Index]-48);
            Index++;
            Activado = true;
        }
        if (!Activado)
        {
            throw "Error de sintaxis, se insertó un punto pero no se añadió ningun decimal";
        }
    }
    double Numero = 0; // El valor que devolveremos
    aux = 0;

    /** Recorremos los enteros y lo pasamos a número. Como tenemos las cifras simplemente es sumarlas **/

    for (auto it = Enteros.begin(); it != Enteros.end(); it++)
    {
        Numero += *it*pow(10,Enteros.size()-aux-1);
        aux++;
    }
    aux = 0;

    /** Idem, pero para los decimales **/

    if (!Decimales.empty())
        for (auto it = Decimales.begin(); it != Decimales.end(); it++)
        {
            Numero += *it/(pow(10,(aux))*10);
            aux++;
        }

    Numero *= signo;
    return Numero;
}
