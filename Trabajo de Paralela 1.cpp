#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <vector>
#include <map>
#include <omp.h> 
#include <cmath>
#include <algorithm>

using namespace std;

int Fecha = 2025;
long cantidad = 100000000;

vector <int> estrato(10);
float total_personas=0;
map <pair <string, string> , vector<short>> edad;
map <int, int> viajes;

string limpiar_linea(string linea){
    string limpio = "";
    for (char c : linea)
        if (c != '"')
            limpio += c;
    return limpio;
}

void minuscula(string& palabra) {
    transform(palabra.begin(), palabra.end(), palabra.begin(),
                   [](unsigned char c){ return tolower(c); });
}

void parametros(string linea){
    linea = limpiar_linea(linea);
    vector<string> datos; 
    stringstream ss(linea);    
    string token;              
    while (getline(ss, token, ';')) {
        datos.push_back(token);
    }
    short estra = int(datos[6][0])-'0';
    string espe = datos[1];
    string gen = datos[2];
    short ed = Fecha - stoi(datos[5].substr(0,4));
    int via = stoi(datos[7].substr(1,datos[7].size()));

    #pragma omp critical
    {
        estrato[estra]++;
        total_personas++;
        edad[{espe,gen}].push_back(ed);
        viajes[via]++;
    }
}

void segmentar_archivo(string nombre_archivo,long long inicio,long long fin){
    
    ifstream archivo(nombre_archivo, ios::binary);
    if (!archivo){
        cerr<<"Error al abrir el archivo"<<nombre_archivo<<endl;
        return;
    }
    
    archivo.seekg(inicio);
    string linea;
    getline(archivo, linea);

    while(getline(archivo, linea)){
        inicio = archivo.tellg();
        if (fin < inicio){
            if (!linea.empty())
                parametros(linea);
            return;
        }
        parametros(linea);
    }
    
    archivo.close();
}

void leer_archivo(string nombre_archivo){
    ifstream archivo(nombre_archivo, ios::binary | ios::ate);
    if (!archivo) {
        cerr << "No se pudo abrir el archivo: " << nombre_archivo << endl;
        return;
    }
    size_t tamano_archivo =  static_cast<size_t>(archivo.tellg());
    archivo.close();

    int total_hilos = omp_get_max_threads();
    size_t tamano_lectura = tamano_archivo/total_hilos;

    #pragma omp parallel
    {
        int N_hilo = omp_get_thread_num();
        long long segmento = tamano_lectura * N_hilo;
        long long maximo = tamano_lectura * (N_hilo + 1);
        segmentar_archivo(nombre_archivo, segmento, maximo);

    }

}

void respuesta1(){
    for (int i = 0; i < estrato.size(); i++)
        cout<<"La cantidad de personas en el estrato ["<<i<<"] = "<<estrato[i]<<endl; 
}

void respuesta2(){
    float porcentaje;

    for (int i = 0; i < estrato.size(); i++){
        porcentaje = estrato[i]/total_personas;
        cout<<"El porcentaje de personas en el estrato ["<<i<<"] = "<<porcentaje<<endl;
    }
}

void respuesta3(){
    for (auto& datos : edad){
        string especie = datos.first.first;
        string genero = datos.first.second;
        vector<short> anios = datos.second;
        float suma = 0;
        for(int i = 0; i < anios.size(); i++)
            suma = suma + anios[i];
        float promedio = suma/anios.size();
            cout<<"La edad promedio para las especies ["<<especie<<", "<<genero<<"]= "<<suma<<endl;
    }
}

void respuesta4(){
    for (auto& datos : edad){
        string especie = datos.first.first;
        string genero = datos.first.second;
        vector<short> anios = datos.second;
        sort(anios.begin(), anios.end());
        int mediana = anios[(anios.size())/2];

        cout<<"La edad mediana de la especie ["<<especie<<","<<genero<<"] = "<< mediana <<endl;
    }
}

void respuesta5(){
        for (auto& datos : edad){
        string especie = datos.first.first;
        string genero = datos.first.second;
        vector<short> anios = datos.second;
        int menor1 = 0;
        int menor3 = 0;
        int mayor6 = 0;
        float promedio = 0;
        for (int i = 0; i < anios.size() ; i++)
        {
            if (anios[i] < 18){
                menor1++;
            }else if(anios[i] < 38){
                menor3++;
            }else{
                mayor6++;
            }
        }
        promedio = menor1/anios.size();
        cout<<"La edad promedio menor de 18 de la especie ["<<especie<<","<<genero<<"] = "<< promedio <<" porciento"<<endl;
        promedio = menor3/anios.size();
        cout<<"La edad promedio menor de 38 de la especie ["<<especie<<","<<genero<<"] = "<< promedio <<" porciento"<<endl;
        promedio = mayor6/anios.size();
        cout<<"La edad promedio mayor de 60 de la especie ["<<especie<<","<<genero<<"] = "<< promedio <<" porciento"<<endl;
    }
}

void respuesta7(){
    for (auto& datos : edad){
        string especie = datos.first.first;
        string genero = datos.first.second;
        vector<short> anios = datos.second;
        int menor1 = 0;
        int mayor6 = 0;
        float promedio = 0;
        for (int i = 0; i < anios.size() ; i++)
        {
            if (anios[i] < 18){
                menor1++;
            }else{
                mayor6++;
            }
        }
        float total = menor1 + mayor6;
        promedio = total/anios.size();
        cout<<"La dependencia es del "<<promedio<<" porciento"<<endl;
    }
}

int main(){
    leer_archivo("eldoria.csv");
    respuesta1();
    cout<<endl;
    respuesta2();
    cout<<endl;
    respuesta3();
    cout<<endl;
    respuesta4();
    cout<<endl;
    respuesta5();
    cout<<endl;
    respuesta7();
    return 0;
}