#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MUTATION_COEFFICIENT 1
#define MUTATION_VARIANCE 0.3
#define POPULATION_SIZE 10
#define NUMBER_OF_GENERATIONS 100
#define MAX_INITIALIZED_GENE_VALUE 100000
#define MUTATION_CONSTANT (MAX_INITIALIZED_GENE_VALUE * 0.005)

typedef float fitness_t;
typedef float gene_t;

struct individual {
    fitness_t fitness;
    gene_t gene;
};

typedef struct individual individual_t;

struct population {
    individual_t *individuals;
    int population_size;
    individual_t best_individual;
};

typedef struct population population_t;

population_t initialize_population(int population_size);
void evaluate_population_fitness(population_t *population, float (*function)(float));
void recombine_population(population_t *population);
void mutate_population(population_t *population);
population_t select_population(population_t *population, population_t (*evaluation_function)(population_t *));
int select_individual_index(population_t *population);

float function_1(float value);
float function_2(float value);
float function_3(float value);
float function_4(float value);

population_t tournament(population_t  *population);
population_t elitism(population_t *population);
population_t roulette(population_t *population);

int coin_toss();
int will_this_happen(float probability);
void printf_population(population_t *population);

float function_1(float value) {
    if(value < 10.0)
        return value;
    else
        return -value + 20;
}

float function_2(float value) {
    return -value * value + 12.0 * value + 8.0;
}

float function_3(float value) {
    return sin(value);
}

float function_4(float x) {
    float y = (2*cos(0.039*x) + 5*sin(0.05*x) + 0.5*cos(0.01*x) + 10*sin(0.07*x) + 5*sin(0.1*x) + 5*sin(0.035*x))*10+500;
    return y;
}

int coin_toss() {
    int random_number = rand();

    return random_number % 2;
}

int will_this_happen(float probability) {
    if(probability >= 1)
        return 1;

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
        population.individuals[i].gene = rand() % MAX_INITIALIZED_GENE_VALUE;
    }
    population.best_individual = population.individuals[0];

    return population;
}

void evaluate_population_fitness(population_t *population, float (*function)(float)) {
    population->best_individual.fitness = function(population->best_individual.gene);

    for(int i = 0; i < population->population_size; i++) {
        population->individuals[i].fitness = function(population->individuals[i].gene);

        if(population->individuals[i].fitness > population->best_individual.fitness) {
            population->best_individual = population->individuals[i];
        }
    }
}

void recombine_population(population_t *population) {

}

void mutate_population(population_t *population) {
    for(int i = 0; i < population->population_size; i++) {
        if(will_this_happen(MUTATION_COEFFICIENT)) {
            float *gene_value = &(population->individuals[i].gene);
            if(coin_toss() == 0) {
                *gene_value *= (1 + MUTATION_VARIANCE);
                //*gene_value += MUTATION_CONSTANT;
            } else {
                *gene_value *= (1 - MUTATION_VARIANCE);
                //*gene_value -= MUTATION_CONSTANT;
            }
        }
    }
}

population_t select_population(population_t *population, population_t (*evaluation_function)(population_t *)) {
    return evaluation_function(population);
}

population_t tournament(population_t  *population) {
    population_t new_population;
    new_population.population_size = population->population_size;
    new_population.individuals = malloc(population->population_size * sizeof(population->individuals));

    for(int i = 0; i < population->population_size; i++) {
        int first_individual = select_individual_index(population);
        int second_individual = select_individual_index(population);

        new_population.individuals[i].gene = (population->individuals[first_individual].gene + population->individuals[second_individual].gene) / 2;
    }
    new_population.best_individual = population->best_individual;

    return new_population;
}

population_t elitism(population_t *population) {
    population_t new_population;
    new_population.population_size = population->population_size;
    new_population.individuals = malloc(population->population_size * sizeof(population->individuals));
    new_population.best_individual = population->best_individual;

    for(int i = 0; i < population->population_size; i++) {
        int first_individual = select_individual_index(population);

        new_population.individuals[i].gene = (population->individuals[first_individual].gene + population->best_individual.gene) / 2;
    }

    return new_population;
}

population_t roulette(population_t *population) {
    float fitness_total = 0;
    float relative_fitness_sum = 0;
    float traversing_relative_fitness_sum = 0;
    float relative_fitness_vector[population->population_size];

    float random_number;

    int first_individual;
    int second_individual;
    int was_the_first_selected = 0;

    population_t new_population;
    new_population.population_size = population->population_size;
    new_population.individuals = malloc(population->population_size * sizeof(population->individuals));
    new_population.best_individual = population->best_individual;

    for(int i = 0; i < population->population_size; i++) {
        fitness_total += population->individuals[i].fitness;
    }

    for(int i = 0; i < population->population_size; i++) {
        relative_fitness_vector[i] = population->individuals[i].fitness / fitness_total;
        relative_fitness_sum  += relative_fitness_vector[i];
    }

    random_number = ((float)rand()/(float)(RAND_MAX)) * relative_fitness_sum;

    for(int j = 0; j < new_population.population_size; j++) {
        for(int i = 0; i < population->population_size; i++) {
            traversing_relative_fitness_sum += relative_fitness_vector[i];

            if(traversing_relative_fitness_sum > random_number && !was_the_first_selected) {
                first_individual = i;
                was_the_first_selected = 1;
                i = 0;
            } else if(traversing_relative_fitness_sum > random_number && was_the_first_selected) {
                second_individual = i;
                was_the_first_selected = 0;
                break;
            }
        }
        new_population.individuals[j].gene = (population->individuals[first_individual].gene + population->individuals[second_individual].gene) / 2;
    }

    return new_population;
}

int select_individual_index(population_t *population) {
    int first_individual = rand() % population->population_size;
    int second_individual = rand() % population->population_size;

    return population->individuals[first_individual].fitness > population->individuals[second_individual].fitness ? first_individual : second_individual;
}

void printf_population(population_t *population) {
    for(int i = 0; i < population->population_size; i++) {
        printf("%d. Gene: %f Fitness: %f \n", i + 1, population->individuals[i].gene, population->individuals[i].fitness);
    }
}

int main() {
    srand(time(NULL));

    int generation = 0;
    population_t population = initialize_population(POPULATION_SIZE);

    while(generation < NUMBER_OF_GENERATIONS) {
        population_t new_population;
        new_population.individuals = NULL;

        //recombine_population(&population);
        mutate_population(&population);
        evaluate_population_fitness(&population, function_4);

        population.individuals[rand() % POPULATION_SIZE] = population.best_individual;

        new_population = select_population(&population, elitism);

        free(population.individuals);
        population.individuals = new_population.individuals;
        population.population_size = new_population.population_size;

        //printf("%d,%f,%f\n", generation, population.best_individual.fitness, population.best_individual.gene);
        printf_population(&population);

        generation++;
    }
    /*
    printf("Last Generation: \n");
    for(int i = 0; i < population.population_size; i++) {
        printf("individual: %f fitness: %f\n", population.individuals[i], population_fitness.fitness[i]);
    }
    printf("\n");
    */
    printf("Best Individual: %f\n", population.best_individual.gene);
    printf("Most fitness: %f\n", population.best_individual.fitness);

    return 0;
}