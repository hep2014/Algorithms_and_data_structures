#include <stdio.h> 
#include <stdlib.h>
#include <ctype.h> 
#include <string.h>
#include <float.h>
#include <errno.h> 
#include <time.h>

#define MAX_SIZE_OF_STACK 100
#define MAX_SIZE 5000
#define GREEN_TEXT "\033[0;32m"
#define RED_TEXT "\033[0;31m"
#define RESET_TEXT "\033[0m"


size_t total_sorting_memory = 0;

//------------------------------------------------------сама сортировка---------------------------------------------------
typedef struct { //структура для одной стопки
	double *data;
	int size;
} Stack;

void init_stack(Stack *stack){ //функция для инициализации одной стопки
	stack->data = (double *)malloc(MAX_SIZE_OF_STACK * sizeof(double));
	stack->size = 0;
	total_sorting_memory += MAX_SIZE_OF_STACK * sizeof(double);
}

void push_to_stack(Stack *stack, double value){ //функция для добавления элементов в стек 
	stack->data[stack->size] = value;
	stack->size++;
}

double peek_from_stack(const Stack *stack){ //функция для взятия вершины стопки 
	return stack->data[stack->size-1];
}

void pop_from_stack(Stack *stack){ //функция для удаления элемента с верхушки стопки 
	if (stack->size > 0) {
		stack->size--;
	}
}

int binary_search(Stack *stacks, int n, double value){ //классический бинарный поиск
	int left = 0, right = n - 1;
	while (left <= right){
		int mid = left + (right - left) / 2;
		if (peek_from_stack(&stacks[mid]) <= value){
			left = mid + 1;
		} else {
			right = mid - 1;
		}
	}
	return left;
}

void pacience_sort(double *array_for_sorting, int n){
	Stack stacks[MAX_SIZE_OF_STACK]; //задаём массив стопок
	int stack_count = 0; //количество стопок 
	
	for (int i = 0; i < n; ++i){
		double current = array_for_sorting[i];
		int pos = binary_search(stacks, stack_count, current);
		if (pos < stack_count) { //если стопка найдена, то добавим элемент в стопку 
			push_to_stack(&stacks[pos], current);
		} else { //если стопка не найдена, то создаём новую 
			init_stack(&stacks[stack_count]); 
			push_to_stack(&stacks[stack_count], current);
			stack_count++;
		}
	}
	int index = 0;
	while (stack_count > 0){
		double min_value = 1e+100;
		int min_stack = -1; 
		//найдём стопку с минимальным элементом 
		for (int i = 0; i < stack_count; ++i){
			if (peek_from_stack(&stacks[i]) < min_value){
				min_value = peek_from_stack(&stacks[i]);
				min_stack = i;
			}
		}
		array_for_sorting[index++] = min_value; //добавим элемент в отсортированный массив
		pop_from_stack(&stacks[min_stack]); //удалим этот элемент из стопки 
		if (stacks[min_stack].size == 0) { //удалим всю стопку целиком 
			free(stacks[min_stack].data);
			for (int i = min_stack; i < stack_count - 1; ++i){
				stacks[i] = stacks[i + 1];
			}
			stack_count--;
		}
	}
	
}
//--------------------------------------------------------здесь сортировка заканчивается------------------------------------------------------------
void trim(char *str){ //функция для удаления пробелов в строке (написана)
	char *end;
	while (isspace((unsigned char)*str)) str++;
	if (*str==0) return; 
	end = str+strlen(str)-1;
	while (end > str && isspace((unsigned char)*end)) end--;
	*(end+1) = '\0';
}

int is_number(const char  *input){ //функция для проверки, является ли введенное - числом (написана) 
	if (input == NULL) return 0;
	
	char str[128];
	strncpy(str, input, sizeof(str));
	str[sizeof(str) - 1] = '\0';
	
	trim(str);
	
	if (*str == '\0') return 0;
	
	char *endptr;
	errno = 0;
	double val = strtod(str, &endptr);
	
	if (*endptr != '\0') return 0;
	if (errno == ERANGE) return 0;
	return 1;
}

void parse_numbers_from_line(const char *line, double *array, int *count, int max_count) { //функция для чтения из потока
    char buffer[256];
    strncpy(buffer, line, sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';

    char *token = strtok(buffer, " \t\r\n");  // Разделители: пробел, таб, перевод строки, возврат каретки

    while (token != NULL) {
        if (*count >= max_count) {
            fprintf(stderr, RED_TEXT "Error: too many numbers provided (max = %d)\n" RESET_TEXT, max_count);
            exit(1);
        }

        if (!is_number(token)) {
            fprintf(stderr, RED_TEXT "Error: invalid number '%s'\n" RESET_TEXT, token);
            exit(1);
        }

        array[(*count)++] = strtod(token, NULL);
        token = strtok(NULL, " \t\r\n");
    }
}

void input_parameters(int *flag_f, int *flag_d, int *flag_s){
	char input[100];

    // Выбор типа массива
    printf("Please, choose the array type:\n1 - if you want static array\n2 - if you want dynamic array\n");
    if (fgets(input, sizeof(input), stdin) != NULL) {
        if (!is_number(input)) {
            fprintf(stderr, RED_TEXT "This is not a Number\n" RESET_TEXT);
            exit(1);
        }
        int choice = atoi(input);
        if (choice == 1) {
            *flag_s = 1;
        } else if (choice == 2) {
            *flag_d = 1;
        } else {
            fprintf(stderr, RED_TEXT "This is not a 1 or 2\n" RESET_TEXT);
            exit(1);
        }
    }

    // Выбор способа ввода данных
    printf("If you want to read the data from the file - input 1, and 0 if you don't want\n");
    if (fgets(input, sizeof(input), stdin) != NULL) {
        if (!is_number(input)) {
            fprintf(stderr, RED_TEXT "This is not a Number\n" RESET_TEXT);
            exit(1);
        }
        int read_from_file = atoi(input);
        if (read_from_file == 1) {
            *flag_f = 1;
        } else if (read_from_file == 0) {
            *flag_f = 0;
        } else {
            fprintf(stderr, RED_TEXT "This is not a 1 or 0\n" RESET_TEXT);
            exit(1);
        }
    }
}

void stat_arr(int flag_f, double array_for_sorting[], int *out_n){ //функция написана
	printf("Please, input the size of the static array but less than 5000\n");
	
	
	char buffer[100]; //это для чтения из стандартного потока, чтобы потом корректно обработать, число ли это
	int n;
	fgets(buffer, sizeof(buffer), stdin); 
	if (!is_number(buffer)){
		fprintf(stderr, RED_TEXT "Error: this is not a number\n" RESET_TEXT);
		exit(1);
	} else{
		n = atoi(buffer);
	}
	if (n <= 0 || n > MAX_SIZE) {
		fprintf(stderr, RED_TEXT "Error: number is too small or number is too large\n" RESET_TEXT);
		exit(1);
	}
	
	if (flag_f == 1){
		printf("Please, input the name of the file\n");
		char name_of_file[100]; 
		fgets(name_of_file, sizeof(name_of_file), stdin);
		name_of_file[strcspn(name_of_file, "\n")] = 0; 
		FILE *fp = fopen(name_of_file, "r");
		
		if (fp == NULL) {
			fprintf(stderr, RED_TEXT "Error: file is incorrect\n" RESET_TEXT);
			exit(1);
		}
		char line[256];
		int index = 0;

		while (fgets(line, sizeof(line), fp) != NULL) {
			parse_numbers_from_line(line, array_for_sorting, &index, n);
		}

		if (index < n) {
			fprintf(stderr, RED_TEXT "Error: not enough numbers in the file (expected %d, got %d)\n" RESET_TEXT, n, index);
			fclose(fp);
			exit(1);
		}

        fclose(fp);
	} else { 
		printf("Please, input the elements of the array\n");
		int index = 0;

		while (index < n) {
			char temp_line[256];
			if (!fgets(temp_line, sizeof(temp_line), stdin)) break;

			parse_numbers_from_line(temp_line, array_for_sorting, &index, n);
		}

		if (index < n) {
			fprintf(stderr, RED_TEXT "Error: not enough numbers provided (expected %d, got %d)\n" RESET_TEXT, n, index);
			exit(1);
		}

	}
	*out_n = n;
}

double* dynamic_arr(int flag_f, int *out_n){
	printf("Please, input the size of the dynamic array\n");
	char buffer[100];
	int n;
	fgets(buffer, sizeof(buffer), stdin); 
	if (!is_number(buffer)){
		fprintf(stderr, RED_TEXT "Error: this is not a number\n" RESET_TEXT);
		exit(1);
	} else{
		n = atoi(buffer);
	}
	if (n <= 0 || n > MAX_SIZE) {
		fprintf(stderr, RED_TEXT "Error: number is too small or number is too large\n" RESET_TEXT);
		exit(1);
	}
	
	double* array_for_sorting = malloc(n*sizeof(double));
	
	if (flag_f == 1){
		printf("Please, input the name of the file\n");
		char name_of_file[100]; 
		fgets(name_of_file, sizeof(name_of_file), stdin);
		name_of_file[strcspn(name_of_file, "\n")] = 0; 
		int index = 0;
		FILE *fp = fopen(name_of_file, "r");
		
		if (fp == NULL) {
			fprintf(stderr, RED_TEXT "Error: file is incorrect\n" RESET_TEXT);
			exit(1);
		}
		char line[256];

		while (fgets(line, sizeof(line), fp) != NULL) {
			parse_numbers_from_line(line, array_for_sorting, &index, n);
		}

        if (index < n) {  
            fprintf(stderr, RED_TEXT "Error: not enough numbers in the file. Expected %d, but got %d\n" RESET_TEXT, n, index);
            fclose(fp);
            exit(1);
        }

        fclose(fp);
	} else { 
		printf("Please, input the elements of the array (with enter)\n");
		int index = 0;

		while (index < n) {
			char temp_line[256];
			if (!fgets(temp_line, sizeof(temp_line), stdin)) break;

			parse_numbers_from_line(temp_line, array_for_sorting, &index, n);
		}

		if (index < n) {
			fprintf(stderr, RED_TEXT "Error: not enough numbers provided (expected %d, got %d)\n" RESET_TEXT, n, index);
			exit(1);
		}
	}
	*out_n = n;
	return array_for_sorting;
}



int main(int argc, char *argv[]){
		int flag_f = 0, flag_d = 0, flag_s = 0;
		
		input_parameters(&flag_f, &flag_d, &flag_s);
		//printf("Flag f = %d, flag s = %d, flag d = %d\n", flag_f, flag_s, flag_d);
		if (flag_s == 1){
			double array_for_sorting[MAX_SIZE];
			int n; 
			stat_arr(flag_f, array_for_sorting, &n);
			printf(GREEN_TEXT "Yours parameters:\n" RESET_TEXT);
			printf("Read from file = %d\nStatic array = %d\nDynamic array = %d\n", flag_f, flag_s, 1-flag_s);
			printf(GREEN_TEXT "Yours array:\n" RESET_TEXT);
			for (int i = 0; i < n; ++i){
				printf("%g ", array_for_sorting[i]);
			}
			printf(RED_TEXT "\nAre you sure want to sort this array?\n" RESET_TEXT);
			printf("Print ");
			printf(GREEN_TEXT"\"1\"" RESET_TEXT);
			printf(" if you want and ");
			printf(RED_TEXT "whatever"  RESET_TEXT);
			printf(" if you don't want\n");
			char buffer[100];
			int result;
			fgets(buffer, sizeof(buffer), stdin); 
			if (!is_number(buffer)){
				exit(1);
			} else{
				result = atoi(buffer);
			}
			if (result == 1){
				clock_t time_start = clock();
					
				pacience_sort(array_for_sorting, n); 
						
				clock_t end_time = clock();
				double time_taken = ((double)(end_time - time_start)) / CLOCKS_PER_SEC; 
				printf(GREEN_TEXT "We have sorted array, and now we have:\n" RESET_TEXT);
				for (int i = 0; i < n; ++i){
					printf("%g ", array_for_sorting[i]);
				}
				printf("\nTime taken to sort = %lf\nMemory allocated to pacience sort = %zu bytes\n", time_taken, total_sorting_memory);
			}
		} else { 
			if (flag_d == 1){
				int n; 
				double *array_for_sorting = dynamic_arr(flag_f, &n);
				printf(GREEN_TEXT "Yours parameters:\n" RESET_TEXT);
				printf("Read from file = %d\nStatic array = %d\nDynamic array = %d\n", flag_f, flag_s, 1-flag_s);
				printf(GREEN_TEXT "Yours array:\n" RESET_TEXT);
				for (int i = 0; i < n; ++i){
					printf("%g ", array_for_sorting[i]);
				}
				printf(RED_TEXT "\nAre you sure want to sort this array?\n" RESET_TEXT);
				printf("Print ");
				printf(GREEN_TEXT"\"1\"" RESET_TEXT);
				printf(" if you want and ");
				printf(RED_TEXT "whatever"  RESET_TEXT);
				printf(" if you don't want\n");
				char buffer[100];
				int result;
				fgets(buffer, sizeof(buffer), stdin); 
				if (!is_number(buffer)){
					exit(1);
				} else{
					result = atoi(buffer);
				}
				if (result == 1){
					clock_t time_start = clock();
						
					pacience_sort(array_for_sorting, n);
						
					clock_t end_time = clock();
					double time_taken = ((double)(end_time - time_start)) / CLOCKS_PER_SEC; 
					printf(GREEN_TEXT "We have sorted array, and now we have:\n" RESET_TEXT);
					for (int i = 0; i < n; ++i){
						printf("%g ", array_for_sorting[i]);
					}
					printf("\nTime taken to sort = %lf\nMemory allocated to pacience sort = %zu bytes\n", time_taken, total_sorting_memory);
				}
			}
		}
					
	return 0;
}
