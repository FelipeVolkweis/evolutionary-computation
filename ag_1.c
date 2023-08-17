#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MUTATION_COEFFICIENT 0.1
#define MUTATION_VARIANCE 0.1
#define POPULATION_SIZE 10
#define NUMBER_OF_GENERATIONS 500
#define MAX_INITIALIZED_GENE_VALUE 1000

typedef float individual_t;
typedef float fitness_t;

struct population {
    individual_t *individuals;
    int population_size;
    individual_t best_individual;
    float max_fitness_value;
    float min_fitness_value;
};

struct fitness {
    fitness_t *fitness;
    int population_size;
};

typedef struct population population_t;
typedef struct fitness population_fitness_t;

population_t initialize_population(int population_size);
population_fitness_t evaluate_population_fitness(population_t *population);
void recombine_population(population_t *population);
void mutate_population(population_t *population);
population_t select_population(population_t population, population_fitness_t population_fitness);
int select_individual_index(population_fitness_t population_fitness);

float function(float value);
float function_2(float value);
int coin_toss();
int will_this_happen(float probability);

float function(float value) {
    if(value < 10.0)
        return value;
    else
        return -value + 20;
}

float function_2(float value) {
    return -value * value + 12.0 * value + 8.0;
}

int coin_toss() {
    int random_number = rand();

    return random_number % 2;
}

int will_this_happen(float probability) {
    int random_number = rand();

    float MAX_CHANCE = RAND_MAX * probability;
    int my_chance = random_number;

    return my_chance <= MAX_CHANCE;
}

population_t initialize_population(int population_size) {
    population_t population;
    population.population_size = population_size;
    population.individuals = malloc(population.population_size * sizeof(population.individuals));

    for(int i = 0; i < population.population_size; i++) {
        population.individuals[i] = rand() % MAX_INITIALIZED_GENE_VALUE;
    }
    population.best_individual = population.individuals[0];

    return population;
}

population_fitness_t evaluate_population_fitness(population_t *population) {
    population_fitness_t population_fitness;

    population_fitness.population_size = population->population_size;
    population_fitness.fitness = malloc(population_fitness.population_size * sizeof(population_fitness.fitness));

    population->max_fitness_value = function_2(population->individuals[0]);
    population->min_fitness_value = population->max_fitness_value;

    for(int i = 0; i < population_fitness.population_size; i++) {
        population_fitness.fitness[i] = function_2(population->individuals[i]);

        if(population_fitness.fitness[i] > population->max_fitness_value) {
            population->best_individual = population->individuals[i];
            population->max_fitness_value = population_fitness.fitness[i];
        } else if(population_fitness.fitness[i] < population->min_fitness_value) {
            population->min_fitness_value = population_fitness.fitness[i];
        }
    }

    return population_fitness;
}

void recombine_population(population_t *population) {

}

void mutate_population(population_t *population) {
    for(int i = 0; i < population->population_size; i++) {
        if(will_this_happen(MUTATION_COEFFICIENT)) {
            float *gene_value = &(population->individuals[i]);
            if(coin_toss() == 0) {
                *gene_value *= (1 + MUTATION_VARIANCE);
            } else {
                *gene_value *= (1 - MUTATION_VARIANCE);
            }
        }
    }
}

population_t select_population(population_t population, population_fitness_t population_fitness) {
    population_t new_population;
    new_population.population_size = population.population_size;
    new_population.individuals = malloc(population.population_size * sizeof(population.individuals));
    for(int i = 0; i < population.population_size; i++) {
        int first_individual = select_individual_index(population_fitness);
        int second_individual = select_individual_index(population_fitness);

        new_population.individuals[i] = (population.individuals[first_individual] + population.individuals[second_individual]) / 2;
    }

    return new_population;
}

int select_individual_index(population_fitness_t population_fitness) {
    int first_individual = rand() % population_fitness.population_size;
    int second_individual = rand() % population_fitness.population_size;

    return population_fitness.fitness[first_individual] > population_fitness.fitness[second_individual] ? first_individual : second_individual;
}

int main() {
    srand(time(NULL));

    int generation = 0;
    population_t population = initialize_population(POPULATION_SIZE);
    population_fitness_t population_fitness;

    while(generation < NUMBER_OF_GENERATIONS) {
        population_t new_population;
        new_population.individuals = NULL;

        //recombine_population(&population);
        mutate_population(&population);
        population_fitness = evaluate_population_fitness(&population);
        new_population = select_population(population, population_fitness);

        free(population.individuals);
        population.individuals = new_population.individuals;
        population.population_size = new_population.population_size;

        generation++;

        free(population_fitness.fitness);
        population_fitness.fitness = NULL;
    }
    /*
    printf("Last Generation: \n");
    for(int i = 0; i < population.population_size; i++) {
        printf("individual: %f fitness: %f\n", population.individuals[i], population_fitness.fitness[i]);
    }
    printf("\n");
    */
    printf("Best Individual: %f\n", population.best_individual);

    return 0;
}