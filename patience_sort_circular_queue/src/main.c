#include <stdio.h>
#include <stdlib.h>
#include <errno.h> 
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_SIZE_OF_STACK 8096
#define GREEN_TEXT "\033[0;32m"
#define RED_TEXT "\033[0;31m"
#define RESET_TEXT "\033[0m"

/*
				создание структур
 */
//создание узла очереди
typedef struct Node {
	double data;
	struct Node* next;
} Node;

//создание кольцевой очереди
typedef struct CircularQueue{
	Node* rear; //указатель на последний элемент очереди (хвост)
} CircularQueue;
//--------------конец создания структур----------------

/*
		создание функций для работы с кольцевой очередью
*/
//функция, которая создаёт кольцевую очередь
CircularQueue* createQueue(){
	CircularQueue* queue = (CircularQueue*)malloc(sizeof(CircularQueue));
	queue->rear = NULL;
	return queue;
}

//функция, которая проверяет очередь на пустоту
int isEmpty(CircularQueue* queue){ //возвращает псевдобулево значение
	return queue->rear == NULL; //логическое выражение, которое вернёт или 0, или 1
}

//функция, которая добавляет элемент в очередь
void enqueue(CircularQueue* queue, double value){
	Node* newNode = malloc(sizeof(Node));
	newNode->data = value;
	
	if (isEmpty(queue)){
		newNode->next = newNode; //указывает сам на себя
		queue->rear = newNode;
	} else {
		newNode->next = queue->rear->next;
		queue->rear->next = newNode;
		queue->rear = newNode;
	}
}

//функция, которая удаляет элемент из очереди
double dequeue(CircularQueue* queue){
	Node* front = queue->rear->next; //указатель на первый элемент очереди
	double value = front->data; 
	
	if (queue->rear == front){ //проверка: если в очереди только один элемент
		free(front); //освобождаем память под единственный узел
		queue->rear = NULL; //обнуляем указатель, так как теперь очередь пуста
	} else {
		queue->rear->next = front->next; //удаляем "голову"
		free(front);
	}
	
	return value; //возвращаем значение удалённого элемента
}

//функция, которая позволяет посмотреть первый элемент очереди
double front(CircularQueue *queue){
	return queue->rear->next->data;
}

//функция, которая освобождает память после использования очереди 
void freeQueue(CircularQueue* queue){
	if (!isEmpty(queue)){ //проверка: если очередь не пустая
		Node* current = queue->rear->next;
		Node* nextNode;
		
		do {
			nextNode = current->next;
			free(current);
			current = nextNode;
		} while (current != queue->rear->next);
	}
	free(queue);
}

//функция, которая позволяет посмотреть элемент очереди, который был добавлен последним
double back(CircularQueue* queue){
	return queue->rear->data;
}

//функция, которая выводит все элементы кольцевой очереди 
void printQueue(CircularQueue* queue) {
    Node* current = queue->rear->next;  // начинаем с головы
    do {
        printf("%g ", current->data);
        current = current->next;
    } while (current != queue->rear->next);  // пока не вернулись к началу

    printf("\n");
}

//--------------------конец создания функций для работы с кольцевой очередью------------------

/*
				создание функций, которые обрабатывают вводимые числа
 */

//функция удаления пробелов в строке
void trim(char *str){ 
	char *end;
	while (isspace((unsigned char)*str)) str++;
	if (*str==0) return; 
	end = str+strlen(str)-1;
	while (end > str && isspace((unsigned char)*end)) end--;
	*(end+1) = '\0';
}

//функция для проверки на число 
int is_number(const char  *input){ 
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
//--------------------конец создания функций, обрабатывающих вводимые числа

//функция, которая читает числа из файла и добавляет их в кольцевую очередь
void readFromFile(CircularQueue* queue, int* n){
	*n = 0; //создание счётчика успешно прочитанных чисел
	fprintf(stdout, GREEN_TEXT "Введите название файла с числами\n" RESET_TEXT);
	char name_of_file[100]; 
	fgets(name_of_file, sizeof(name_of_file), stdin);
	name_of_file[strcspn(name_of_file, "\n")] = 0; 
	FILE *fp = fopen(name_of_file, "r");
		
	if (fp == NULL) {
		fprintf(stderr, RED_TEXT "Ошибка: невозможно прочеcть данный файл\n" RESET_TEXT);
		exit(1);
	} else {
		char buffer[1024]; // буфер для чтения строки из файла
        while (fgets(buffer, sizeof(buffer), fp) != NULL) {
			char *token = strtok(buffer, " \t\n"); // разделители: пробел, табуляция, перевод строки
			while (token != NULL) {
					if (is_number(token)) { // проверяем, является ли токен числом
						char *endptr; 
						double num = strtod(token, &endptr); // преобразуем строку в число
						enqueue(queue, num); // добавляем число в очередь
						(*n)++;
					} else {
						fprintf(stderr, RED_TEXT "Ошибка: '%s' невозможно обработать. Удалите данный элемент и перезапустите программу\n" RESET_TEXT, token);
						exit(1);
					}
					token = strtok(NULL, " \t\n"); // Следующий токен
				}
			}
	}
	fclose(fp);
	if (*n == 0){
		fprintf(stderr, RED_TEXT "В файле отсутствуют числа\n" RESET_TEXT);
		exit(1);
	}
}
/*
				Реализация терпеливой сортировки на кольцевой очереди
 */
//структура для одной стопки
typedef struct { 
	double *data;
	int size;
} Stack;

//функция для инициализации одной стопки
void init_stack(Stack *stack){ 
	stack->data = (double *)malloc(MAX_SIZE_OF_STACK * sizeof(double));
	stack->size = 0;
}

//функция для добавления элементов в стек
void push_to_stack(Stack *stack, double value){  
	stack->data[stack->size] = value;
	stack->size++;
}

//функция для взятия вершины стопки
double peek_from_stack(const Stack *stack){  
	return stack->data[stack->size-1];
}

//функция для удаления элемента с верхушки стопки 
void pop_from_stack(Stack *stack){ 
	if (stack->size > 0) {
		stack->size--;
	}
}

//классический бинарный поиск
int binary_search(Stack *stacks, int n, double value){ 
	int left = 0, right = n - 1;
	while (left <= right){
		int mid = left + (right - left) / 2; //постоянно проверяем средний элемент
		if (peek_from_stack(&stacks[mid]) <= value){
			left = mid + 1;
		} else {
			right = mid - 1;
		}
	}
	return left;
}

//реализация самой сортировки
void patience_sort_queue(CircularQueue* queue) {
    // Подсчёт размера очереди
    int n = 0;
    Node* current = queue->rear->next;
    do {
        n++;
        current = current->next;
    } while (current != queue->rear->next);

    // Создаём временный массив значений из очереди
    double* temp = (double*)malloc(n * sizeof(double));
    current = queue->rear->next;
    for (int i = 0; i < n; ++i) {
        temp[i] = current->data;
        current = current->next;
    }

    Stack stacks[MAX_SIZE_OF_STACK];
    int stack_count = 0;

    for (int i = 0; i < n; ++i) {
        double current_value = temp[i];
        int pos = binary_search(stacks, stack_count, current_value);
        if (pos < stack_count) {
            push_to_stack(&stacks[pos], current_value);
        } else {
            init_stack(&stacks[stack_count]);
            push_to_stack(&stacks[stack_count], current_value);
            stack_count++;
        }
    }

    // Очистим очередь
    while (!isEmpty(queue)) {
        dequeue(queue);
    }

    // Снова наполняем очередь отсортированными значениями
    while (stack_count > 0) {
        double min_value = 1e100;
        int min_stack = -1;
        for (int i = 0; i < stack_count; ++i) {
            if (peek_from_stack(&stacks[i]) < min_value) {
                min_value = peek_from_stack(&stacks[i]);
                min_stack = i;
            }
        }
        enqueue(queue, min_value);
        pop_from_stack(&stacks[min_stack]);

        if (stacks[min_stack].size == 0) {
            free(stacks[min_stack].data);
            for (int i = min_stack; i < stack_count - 1; ++i) {
                stacks[i] = stacks[i + 1];
            }
            stack_count--;
        }
    }
    free(temp);
}
//-------------------------------здесь сортировка заканчивается----

//данная функция отвечает за вывод чисел в стандартный поток вывода
void printNumbers(CircularQueue* queue, int flag){ 
	char choose[20]; //буфер, в который запишется либо "до", либо "после" 
	if (flag == 1){
		strcpy(choose, "до");
	} else {
		strcpy(choose, "после");
	}
	fprintf(stdout, GREEN_TEXT "Числа %s сортировки:\n" RESET_TEXT, choose); 
	printQueue(queue); //вывод чисел
}

//данная функция отвечает за запись количества элементов и времени, затраченного на сортировку, в csv файл
void writeToCsv(int counter, double time_taken){
	FILE *fp = fopen("result.csv", "a");
	fprintf(fp, "%d, %lf\n", counter, time_taken);
}

int main(int argc, char *argv[]){
	CircularQueue* queue = createQueue(); //создаём кольцевую очередь
	int counter; //создаём переменную, которая отвечает за количество прочитанных чисел
	readFromFile(queue, &counter); //читаем числа из файла
	fprintf(stdout,"Количество успешно прочитанных чисел = %d\n", counter);
	printNumbers(queue, 1);
	clock_t timeBefore = clock(); //переменная типа clock_t, которая хранит время начала сортировки в клоках 
	patience_sort_queue(queue);
	clock_t timeAfter = clock(); //переменная типа clock_t, которая хранит время окончания сортировки в клоках 
	double time_taken = ((double)(timeAfter - timeBefore)) / CLOCKS_PER_SEC; 
	printNumbers(queue, 2);
	fprintf(stdout, "Время, затраченное на сортировку: \033[0;32m %lf \033[0m секунд\n", time_taken);
	fprintf(stdout, "Количество элементов и затраченное время записаны в файл \033[0;32m result.csv \033[0m\n");
	writeToCsv(counter, time_taken);
	return 0;
}
