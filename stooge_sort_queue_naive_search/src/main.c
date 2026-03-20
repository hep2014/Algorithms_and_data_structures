#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h> 
#include <ctype.h>
#include <time.h>

#define GREEN_TEXT "\033[0;32m"
#define RED_TEXT "\033[0;31m"
#define RESET_TEXT "\033[0m"
/*
			Создание очереди на базе связного списка
*/

//Создание узла очереди
typedef struct Node {
	double data;
	struct Node* next;
} Node;

//Данная структура служит для адекватного отслеживания начала и конца очереди
typedef struct {
	Node* front; //узел, указывающий на начало очереди
	Node* rear; //узел, указывающий на конец очереди 
} Queue;

/*
			Конец создания очереди
*/

/*
			Создание функций, работающих с очередью
*/

//Данная функция отвечает за создание пустой очереди
Queue* createQueue() {
    Queue* q = (Queue*)malloc(sizeof(Queue)); 
    q->front = q->rear = NULL; //указатели получают значение NULL, так как пока очередь не содержит элементов
    return q;
}

//Данная функция добавляет элемент в очередь
void enqueue(Queue* q, double x){
	Node* newNode = (Node*)malloc(sizeof(Node)); //выделяется память под новый узел 
	newNode->data = x;
    newNode->next = NULL;
    if (q->rear == NULL) { //Проверка очереди на пустоту 
        q->front = q->rear = newNode; //Если очередь пуста, то новый эл-т становится и началом, и концом
    } else {
        q->rear->next = newNode; // текущий последний элемент теперь ссылается на newNode
        q->rear = newNode; // newNode становится новым концом очереди
    }
}

//Данная функция удаляет элемент из очереди
double dequeue(Queue* q){
	Node* temp = q->front; 
	double data = temp->data; //Сохраняем возвращаемое значение
	q->front = q->front->next;
	if (q->front == NULL) { //Проверка очереди на пустоту после удаления эл-та
        q->rear = NULL;
    }
    free(temp); //Освобождение памяти 
    return data;
}

//Данная функция меняет элементы местами в очереди
void swapElements(Queue* q, double x, double y){
	//Предполагается, что элементы всегда будут найдены, а очередь не является пустой
	//при вызове данной функции (это убирает часть проверок)
	Node *nodeX = NULL, *nodeY = NULL;
    Node *current = q->front;
	while (current != NULL && (nodeX == NULL || nodeY == NULL)) {
        if (nodeX == NULL && current->data == x) {
            nodeX = current;
        }
        if (nodeY == NULL && current->data == y) {
            nodeY = current;
        }
        current = current->next;
    }
	double temp = nodeX->data;
	nodeX->data = nodeY->data;
	nodeY->data = temp;
}

//Данная функция подсчитывает количество элементов в очереди
int queueSize(Queue* q) {
    int count = 0;
    Node* current = q->front;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

//Данная функция выводит все элементы кольцевой очереди
void printQueue(Queue* q) {
    Node* current = q->front;
    while (current != NULL) {
        printf("%g ", current->data);  
        current = current->next;
    }
    printf("\n");
}

//Данная функция конвертирует очередь в строку
void queueToString(Queue* q, char* source){
	Node* current = q->front;
	source[0] = '\0';  //Инициализация строки как пустой
	while (current != NULL) {
        //Добавляем число в строку, используя sprintf
        char temp[50]; //Временный буфер для хранения строки, представляющей число
        sprintf(temp, "%g ", current->data); //Преобразуем число в строку
        strcat(source, temp); //Добавляем число в итоговую строку
        current = current->next;
    }
}
/*
				Конец создания функций, работающих с очередью
*/

//Вспомогательная функция, в которой реализована придурковатая сортировка на массивах
void stoogeArraySort(double* a, int i, int j) {
    if (a[j] < a[i]) { //Первый шаг - сравниваем первый и последний элементы
        double tmp = a[i]; //Если необходимо - меняем первый и последний элементы местами
        a[i] = a[j];
        a[j] = tmp;
    }
    if (j - i + 1 > 2) { //Если размер массива больше 2, то рекурсивно сортируем две трети массива
        int k = (j - i + 1) / 3; //Выделяем треть массива
        stoogeArraySort(a, i, j - k); //Рекурсивно вызываем сортировку для первых двух третьих массива
        stoogeArraySort(a, i + k, j); //Рекурсивно вызываем сортировку для последних двух третьих массива
        stoogeArraySort(a, i, j - k); //Снова рекурсивно вызываем сортировку для первых двух третьих массива
    }
}

//Создание придурковатой сортировки
void stoogeSort(Queue* q) {
    int size = queueSize(q);

    //Копируем очередь в массив
    double* arr = (double*)malloc(size * sizeof(double));
    Node* current = q->front;
    for (int i = 0; i < size; i++) {
        arr[i] = current->data;
        current = current->next;
    }
    
    stoogeArraySort(arr, 0, size - 1); //Сортируем
    
    //Обновляем очередь отсортированными значениями
    current = q->front;
    for (int i = 0; i < size; i++) {
        current->data = arr[i];
        current = current->next;
    }

    free(arr);
}
/*
				Создание функций, которые обрабатывают вводимые числа
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
/*
				Конец создания функций, обрабатывающих вводимые числа
*/

//функция, которая читает числа из файла и добавляет их в очередь
void readFromFile(Queue* q, int* n){
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
						enqueue(q, num); // добавляем число в очередь
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
	if (*n == 1){
		fprintf(stderr, RED_TEXT "В файле одно число. Этого мало для сортировки.\n" RESET_TEXT);
		exit(1);
	}
}

//Данная функция отвечает за вывод чисел в стандартный поток вывода
void printNumbers(Queue* q, int flag){ 
	char choose[20]; //Буфер, в который запишется либо "до", либо "после" 
	if (flag == 1){
		strcpy(choose, "до");
	} else {
		strcpy(choose, "после");
	}
	fprintf(stdout, GREEN_TEXT "Числа %s сортировки:\n" RESET_TEXT, choose); 
	printQueue(q); //Вывод чисел
}

//Создание алгоритма наивного поиска подстроки pattern в строке source 
int* naiveStringMatcher(char* pattern, char* source, int* matchCount){
	//Функция вернёт массив позиций, откуда начинается вхождение подстроки
	int patternLength = strlen(pattern);
	int sourceLength = strlen(source);
	int* positions = (int*)malloc(sourceLength * sizeof(int)); //Инициализация массива позиций
    int* matches = NULL;
    *matchCount = 0;  //Счётчик для числа найденных позиций
	for (int i = 0; i <= sourceLength - patternLength; i++) { //Проходим по всей строке source, проверяя каждый возможный сдвиг подстроки pattern
        if (strncmp(&source[i], pattern, patternLength) == 0) {
            //Убедимся, что перед и после совпадения — границы слова (пробелы или начало/конец строки)
            if ((i == 0 || source[i - 1] == ' ') &&
                (source[i + patternLength] == ' ' || source[i + patternLength] == '\0')) {
                	matches = realloc(matches, (*matchCount + 1) * sizeof(int));
                	matches[*matchCount] = i; //Сохраняем индекс начала найденного вхождения
                	(*matchCount)++; //Увеличиваем счётчик найденных совпадений
            }
        }
    }
    return matches;
}

//В данной функции запрашивается подстрока для наивного поиска 
void inputParametr(char* pattern){
	fprintf(stdout, "Если вы хотите произвести наивный поиск, введите \033[0;32m\"Да\"\033[0m. Или введите \033[0;31m\"Нет\"\033[0m в противном случае.\n"); 
	char buffer[100];
	fgets(buffer, sizeof(buffer), stdin);
	buffer[strcspn(buffer, "\n")] = 0;
	if (strcmp(buffer, "Да") == 0){ //Если пользователь согласился
		fprintf(stdout, "Введите подстроку. Учтите, что числа должны быть разделены пробелами:\n");
        fgets(pattern, 256, stdin); //В pattern передаётся подстрока
        pattern[strcspn(pattern, "\n")] = 0; // убрать \n
        // Копия строки для безопасной токенизации
        char temp[256];
        strncpy(temp, pattern, sizeof(temp));
        temp[sizeof(temp)-1] = '\0';
        char* token = strtok(temp, " \t");
        while (token != NULL) {
            if (!is_number(token)) {
                fprintf(stderr, RED_TEXT "Ошибка: '%s' не является числом.\n" RESET_TEXT, token);
                exit(1);
            }
            token = strtok(NULL, " \t");
        }
	} else if (strcmp(buffer, "Нет") == 0){ //Если пользователь отказался
		fprintf(stdout, GREEN_TEXT "Вы отказались. Программа успешно завершена.\n" RESET_TEXT);
		exit(0);
	} else { //Если пользователь ввёл неверное значение
		fprintf(stderr, RED_TEXT "Вы ввели неверное значение. Программа завершена с ошибкой.\n" RESET_TEXT);
		exit(1);
	}

}

int main(int argc, char* argv[]){
	Queue* q = createQueue(); //Создание очереди
	int counter = 0; 
	readFromFile(q, &counter); //Читаем данные из файла
	printNumbers(q, 1);
	clock_t timeBefore = clock();
	stoogeSort(q);
	clock_t timeAfter = clock();
	double time_taken = ((double)(timeAfter - timeBefore)) / CLOCKS_PER_SEC; //Подсчитываем время, затраченное на сортировку
	printNumbers(q, 2);
	fprintf(stdout, "Время, затраченное на сортировку: \033[0;32m %lf \033[0m секунд.\n", time_taken);
	char pattern[1000]; //Создание шаблона подстроки для последующего поиска
	inputParametr(pattern); //На этом этапе программа может либо завершиться, либо получить шаблон подстроки
	/*
					Данная часть кода выполнится только при согласии пользователя
	*/
	char source[4096]; //Создание строки, в которую запишутся элементы очереди
	source[0] = '\0';
	queueToString(q, source); //Вызов функции конвертации очереди в строку
	int matchCount = 0;
	int* matches = naiveStringMatcher(pattern, source, &matchCount); //Вызов функции наивного поиска
	if (matchCount == 0) {
    	fprintf(stdout, RED_TEXT "Подстрока не найдена.\n" RESET_TEXT);
	} else { //Красивый вывод с помощью выделения цветом
		fprintf(stdout, GREEN_TEXT "Подстрока найдена:\n" RESET_TEXT);
		int pos = 0;
		int patternLength = strlen(pattern);
		for (int i = 0; i < matchCount; i++) {
			int matchPos = matches[i];
			// Печатаем всё до совпадения
			while (pos < matchPos) {
				putchar(source[pos++]);
			}
			// Выделяем совпадение цветом
			printf(GREEN_TEXT);
			for (int j = 0; j < patternLength && source[pos] != '\0'; j++) {
				putchar(source[pos++]);
			}
			printf(RESET_TEXT);
		}
		// Печатаем остаток строки, если есть
		while (source[pos] != '\0') {
			putchar(source[pos++]);
		}
		printf("\n");
		free(matches);
	}
	return 0;
}




