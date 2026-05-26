#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <random>

using Edificio = std::tuple<std::string, int, int, int>;
using Individuo = std::vector<bool>;
using Poblacion = std::vector<Individuo>;

const int COSTO_MAXIMO = 85000;
const size_t TAMANIO_POBLACION = 10;
const int GENERACIONES_MAXIMAS = 5;
const double PROBABILIDAD_CRUCE = 0.8;
const double PROBABILIDAD_MUTACION = 0.02;

std::random_device rd;
std::mt19937 gen(rd());

const std::vector<Edificio> edificios = {
    {"Hospital_Goyeneche", 12000, 950, 10},
    {"UNSA_Pabellon_A", 10000, 850, 9},
    {"Colegio_Independencia", 8000, 700, 8},
    {"Municipalidad_Cayma", 7000, 500, 6},
    {"Centro_Salud_Yanahuara", 6000, 450, 7},
    {"Biblioteca_Regional", 5000, 400, 5},
    {"Terminal_Terrestre", 9000, 750, 8},
    {"Mercado_San_Camilo", 6500, 480, 6},
    {"Estadio_Melgar", 15000, 1200, 10},
    {"Instituto_Tecnologico", 4000, 300, 4},
    {"UNSA_Pabellon_B", 9500, 820, 8},
    {"Colegio_Militar", 8500, 680, 7},
    {"Hospital_Honorio_Delgado", 14000, 1100, 10},
    {"Municipalidad_Cerro_Colorado", 7500, 520, 6},
    {"Aeropuerto_Rodriguez_Ballon", 16000, 1300, 10},
    {"Parque_Industrial", 11000, 900, 9},
    {"Centro_Comercial_Lambramani", 10500, 870, 8},
};

double probabilidad()
{
    std::uniform_real_distribution<double> distribucion(0.0, 1.0);
    return distribucion(gen);
}

int numero_aleatorio(int minimo, int maximo)
{
    std::uniform_int_distribution<int> distribucion(minimo, maximo);
    return distribucion(gen);
}

int calcular_costo_total(const Individuo &individuo)
{
    int costo_total = 0;

    for (size_t i = 0; i < individuo.size(); ++i)
        if (individuo[i])
            costo_total += std::get<1>(edificios[i]);

    return costo_total;
}

bool es_valido(const Individuo &individuo)
{
    return calcular_costo_total(individuo) <= COSTO_MAXIMO;
}

int calcular_fitness(const Individuo &individuo)
{
    if (!es_valido(individuo))
        return 0;

    int fitness_total = 0;

    for (size_t i = 0; i < individuo.size(); ++i)
        if (individuo[i])
            fitness_total += std::get<3>(edificios[i]) * std::get<2>(edificios[i]);

    return fitness_total;
}

std::string obtener_cromosoma(const Individuo &individuo)
{
    std::string cromosoma;

    for (bool gen : individuo)
        cromosoma += gen ? '1' : '0';

    return cromosoma;
}

Individuo obtener_mejor_individuo(const Poblacion &poblacion)
{
    Individuo mejor_individuo = poblacion[0];
    int mejor_fitness = calcular_fitness(mejor_individuo);

    for (size_t i = 1; i < poblacion.size(); ++i)
    {
        int fitness_actual = calcular_fitness(poblacion[i]);

        if (fitness_actual > mejor_fitness)
        {
            mejor_individuo = poblacion[i];
            mejor_fitness = fitness_actual;
        }
    }

    return mejor_individuo;
}

Individuo seleccionar_padre(const Poblacion &poblacion, const std::vector<int> &fitness, int suma_fitness)
{
    if (suma_fitness == 0)
        return poblacion[numero_aleatorio(0, poblacion.size() - 1)];

    double punto = probabilidad() * suma_fitness;
    double acumulado = 0;

    for (size_t i = 0; i < poblacion.size(); ++i)
    {
        acumulado += fitness[i];

        if (acumulado >= punto)
            return poblacion[i];
    }

    return poblacion.back();
}

Poblacion seleccionar_padres(const Poblacion &poblacion)
{
    Poblacion padres;
    std::vector<int> fitness(poblacion.size());
    int suma_fitness = 0;

    for (size_t i = 0; i < poblacion.size(); ++i)
    {
        fitness[i] = calcular_fitness(poblacion[i]);
        suma_fitness += fitness[i];
    }

    while (padres.size() < poblacion.size())
        padres.push_back(seleccionar_padre(poblacion, fitness, suma_fitness));

    return padres;
}

void cruzar(Individuo &hijo1, Individuo &hijo2)
{
    if (hijo1.size() < 2 || probabilidad() > PROBABILIDAD_CRUCE)
        return;

    int punto_cruce = numero_aleatorio(1, hijo1.size() - 1);

    for (size_t i = punto_cruce; i < hijo1.size(); ++i)
    {
        bool gen_temporal = hijo1[i];
        hijo1[i] = hijo2[i];
        hijo2[i] = gen_temporal;
    }
}

void mutar(Individuo &individuo)
{
    for (size_t i = 0; i < individuo.size(); ++i)
        if (probabilidad() <= PROBABILIDAD_MUTACION)
            individuo[i] = !individuo[i];
}

Poblacion generar_siguiente_poblacion(const Poblacion &poblacion_actual)
{
    Poblacion padres = seleccionar_padres(poblacion_actual);
    Poblacion nueva_poblacion;

    nueva_poblacion.push_back(obtener_mejor_individuo(poblacion_actual));

    while (nueva_poblacion.size() < poblacion_actual.size())
    {
        Individuo hijo1 = padres[numero_aleatorio(0, padres.size() - 1)];
        Individuo hijo2 = padres[numero_aleatorio(0, padres.size() - 1)];

        cruzar(hijo1, hijo2);
        mutar(hijo1);
        mutar(hijo2);

        nueva_poblacion.push_back(hijo1);

        if (nueva_poblacion.size() < poblacion_actual.size())
            nueva_poblacion.push_back(hijo2);
    }

    return nueva_poblacion;
}

Poblacion generar_poblacion_inicial(size_t tamanio_poblacion, size_t cantidad_edificios)
{
    Poblacion poblacion(tamanio_poblacion, Individuo(cantidad_edificios, false));

    for (size_t i = 0; i < tamanio_poblacion; ++i)
        for (size_t j = 0; j < cantidad_edificios; ++j)
            poblacion[i][j] = probabilidad() > 0.5;

    return poblacion;
}

void imprimir_edificios_seleccionados(std::ostream &salida, const Individuo &individuo)
{
    bool hay_edificios = false;

    for (size_t i = 0; i < individuo.size(); ++i)
    {
        if (individuo[i])
        {
            salida << "- " << std::get<0>(edificios[i]) << "\n";
            hay_edificios = true;
        }
    }

    if (!hay_edificios)
        salida << "- Ninguno\n";
}

void imprimir_individuo(std::ostream &salida, const Individuo &individuo)
{
    salida << "Cromosoma: " << obtener_cromosoma(individuo) << "\n";
    salida << "Costo total: " << calcular_costo_total(individuo) << "\n";
    salida << "Fitness total: " << calcular_fitness(individuo) << "\n";
    salida << "Estado: " << (es_valido(individuo) ? "valido" : "invalido") << "\n";
}

void imprimir_resumen_final(std::ostream &salida, const Individuo &mejor_individuo)
{
    int presupuesto_utilizado = calcular_costo_total(mejor_individuo);

    salida << "\nMejor solucion encontrada:\n";
    imprimir_individuo(salida, mejor_individuo);
    salida << "Presupuesto maximo: " << COSTO_MAXIMO << "\n";
    salida << "Presupuesto utilizado: " << presupuesto_utilizado << "\n";
    salida << "Presupuesto restante: " << COSTO_MAXIMO - presupuesto_utilizado << "\n";
    salida << "Edificios seleccionados:\n";
    imprimir_edificios_seleccionados(salida, mejor_individuo);
}

void imprimir_informacion_estatica(std::ostream &salida)
{
    salida << "Algoritmo Genetico:\n";
    salida << "Cantidad de generaciones: " << GENERACIONES_MAXIMAS << "\n";
    salida << "Cantidad de individuos: " << TAMANIO_POBLACION << "\n";
    salida << "Cantidad de edificios: " << edificios.size() << "\n";
    salida << "Presupuesto maximo: " << COSTO_MAXIMO << "\n";
    salida << "Probabilidad de cruzamiento: " << PROBABILIDAD_CRUCE << "\n";
    salida << "Probabilidad de mutacion: " << PROBABILIDAD_MUTACION << "\n";
    salida << "Representacion: binaria\n";
    salida << "Seleccion: ruleta\n";
    salida << "Cruzamiento: un punto\n";
    salida << "Mutacion: por gen\n";
    salida << "----------------------------------------\n\n";
}

int main()
{
    const size_t cantidad_edificios = edificios.size();
    Poblacion poblacion = generar_poblacion_inicial(TAMANIO_POBLACION, cantidad_edificios);

    Individuo mejor_global = obtener_mejor_individuo(poblacion);
    int mejor_fitness_global = calcular_fitness(mejor_global);

    imprimir_informacion_estatica(std::cout);

    for (int generacion = 1; generacion <= GENERACIONES_MAXIMAS; ++generacion)
    {
        std::cout << "Generacion " << generacion << ":\n";

        for (size_t i = 0; i < poblacion.size(); ++i)
        {
            std::cout << "Individuo " << i + 1 << ":\n";
            imprimir_individuo(std::cout, poblacion[i]);
            std::cout << "\n";
        }

        Individuo mejor_generacion = obtener_mejor_individuo(poblacion);
        int fitness_generacion = calcular_fitness(mejor_generacion);

        if (fitness_generacion > mejor_fitness_global)
        {
            mejor_global = mejor_generacion;
            mejor_fitness_global = fitness_generacion;
        }

        poblacion = generar_siguiente_poblacion(poblacion);

        std::cout << "----------------------------------------\n\n";
    }

    imprimir_resumen_final(std::cout, mejor_global);

    return 0;
}
