/*
                    УДАЛИТЬ В ФИНАЛЬНОЙ ВЕРСИИ
Задание: разработать программу терпеливой сортировки чисел из файла, которые считываются в стеке на базе связного списка. 
Результат записывается в красно-чёрное дерево. 

*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#define GREEN_TEXT "\033[0;32m"
#define RED_TEXT "\033[0;31m"
#define RESET_TEXT "\033[0m"
#define BLACK_TEXT "\033[0;30m"
#define MAX_SIZE 1000
#define MAX_SIZE_OF_PILE 1000

/*
        Создание стека на базе связаного списка
*/

//Создание узла стека
typedef struct Node{
    double data;
    struct Node* next;
} Node;

//Данная структура служит для адекватной работы с вершиной стека
typedef struct{
    Node* top;
} Stack;
/*
        Конец создания стека
*/

/*
        Создание функций, которые работают со стеком
*/

//Данная функция отвечает за создание стека
Stack* createStack(){
    Stack* stack = (Stack*)malloc(sizeof(Stack));  //Выделение памяти под стек
    stack->top = NULL; //Пока что вершина стека пуста
    return stack; 
}

//Данная функция добавляет элемент в конец стека 
void pushTop(Stack* stack, double value){
    Node* newNode = (Node*)malloc(sizeof(Node));  //Выделение памяти под новый узел
    newNode->data = value;
    newNode->next = stack->top;  //Новый узел указывает на старую вершину
    stack->top = newNode;  //Новый узел становится вершиной стека
}

//Данная функция удаляет элемент с вершины стека 
double delFromStack(Stack* stack){
    Node* temp = stack->top;  //Временный указатель на верхний узел
    double value = temp->data;
    stack->top = stack->top->next;  //Перемещаем вершину на следующий узел
    free(temp);  //Освобождаем память
    return value;
}

//Данная функция показывает последний элемент стека
double peekFromStack(Stack* stack){
    return stack->top->data;
}

/*
        Конец создания функций, работающих со стеком
*/

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

//функция, которая читает числа из файла и добавляет их в стек
void readFromFile(Stack* stack, int* n){
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
						pushTop(stack, num); // добавляем число в стек
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
	}
}

/*
        Создание красно-чёрного дерева
*/

// Определение цвета узла
typedef enum { RED, BLACK } Color;

typedef struct RBNode {
    double data;
    Color color;
    struct RBNode *left, *right, *parent;
} RBNode;

// Функция создания нового узла (красного по умолчанию)
RBNode* createNode(double data) {
    RBNode* node = (RBNode*)malloc(sizeof(RBNode));
    node->data = data;
    node->color = RED;
    node->left = node->right = node->parent = NULL;
    return node;
}

// Левый поворот вокруг pt
void rotateLeft(RBNode** root, RBNode* pt) {
    RBNode* pt_right = pt->right;
    pt->right = pt_right->left;

    if (pt->right != NULL)
        pt->right->parent = pt;

    pt_right->parent = pt->parent;

    if (pt->parent == NULL)
        *root = pt_right;
    else if (pt == pt->parent->left)
        pt->parent->left = pt_right;
    else
        pt->parent->right = pt_right;

    pt_right->left = pt;
    pt->parent = pt_right;
}

// Правый поворот вокруг pt
void rotateRight(RBNode** root, RBNode* pt) {
    RBNode* pt_left = pt->left;
    pt->left = pt_left->right;

    if (pt->left != NULL)
        pt->left->parent = pt;

    pt_left->parent = pt->parent;

    if (pt->parent == NULL)
        *root = pt_left;
    else if (pt == pt->parent->left)
        pt->parent->left = pt_left;
    else
        pt->parent->right = pt_left;

    pt_left->right = pt;
    pt->parent = pt_left;
}

// Исправление нарушений после вставки
void fixViolation(RBNode** root, RBNode* pt) {
    RBNode *parent_pt = NULL, *grand_parent_pt = NULL;

    while ((pt != *root) && (pt->color == RED) && (pt->parent->color == RED)) {
        parent_pt = pt->parent;
        grand_parent_pt = pt->parent->parent;

        // Родитель - левый потомок дедушки
        if (parent_pt == grand_parent_pt->left) {
            RBNode* uncle_pt = grand_parent_pt->right;

            if (uncle_pt != NULL && uncle_pt->color == RED) {
                grand_parent_pt->color = RED;
                parent_pt->color = BLACK;
                uncle_pt->color = BLACK;
                pt = grand_parent_pt;
            } else {
                if (pt == parent_pt->right) {
                    rotateLeft(root, parent_pt);
                    pt = parent_pt;
                    parent_pt = pt->parent;
                }
                rotateRight(root, grand_parent_pt);
                Color tmp = parent_pt->color;
                parent_pt->color = grand_parent_pt->color;
                grand_parent_pt->color = tmp;
                pt = parent_pt;
            }
        }
        // Родитель - правый потомок дедушки
        else {
            RBNode* uncle_pt = grand_parent_pt->left;

            if (uncle_pt != NULL && uncle_pt->color == RED) {
                grand_parent_pt->color = RED;
                parent_pt->color = BLACK;
                uncle_pt->color = BLACK;
                pt = grand_parent_pt;
            } else {
                if (pt == parent_pt->left) {
                    rotateRight(root, parent_pt);
                    pt = parent_pt;
                    parent_pt = pt->parent;
                }
                rotateLeft(root, grand_parent_pt);
                Color tmp = parent_pt->color;
                parent_pt->color = grand_parent_pt->color;
                grand_parent_pt->color = tmp;
                pt = parent_pt;
            }
        }
    }
    (*root)->color = BLACK;
}

// Вставка в BST и возвращение нового корня
RBNode* BSTInsert(RBNode* root, RBNode* pt) {
    if (root == NULL)
        return pt;

    if (pt->data < root->data) {
        root->left = BSTInsert(root->left, pt);
        root->left->parent = root;
    } else if (pt->data > root->data) {
        root->right = BSTInsert(root->right, pt);
        root->right->parent = root;
    }
    // Если равны, игнорируем вставку (или можно менять логику)

    return root;
}

// Вставка нового значения в RB-дерево
void insert(RBNode** root, double data) {
    RBNode* pt = createNode(data);

    *root = BSTInsert(*root, pt);

    fixViolation(root, pt);
}

// Симметричный обход (inorder)
void inorderHelper(RBNode* root) {
    if (root == NULL)
        return;

    inorderHelper(root->left);
    printf("%.2f ", root->data);
    inorderHelper(root->right);
}

void printTreeRotated(RBNode* node, int space) {
    if (node == NULL)
        return;

    // Увеличиваем отступ для текущего уровня
    int indent = 5;
    space += indent;

    // Сначала печатаем правое поддерево (будет сверху)
    printTreeRotated(node->right, space);

    // Печатаем текущий узел с отступом
    printf("\n");
    for (int i = indent; i < space; i++)
        printf(" ");
    printf("%.2f(%s)\n", node->data, node->color == RED ? "RED" : "BLACK");

    // Затем печатаем левое поддерево (будет снизу)
    printTreeRotated(node->left, space);
}


// Освобождение памяти
void freeTree(RBNode* root) {
    if (root == NULL) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

/*
        Конец инициализации красно-чёрного дерева и создания функций,
        работающих с ним
*/

/*
        Реализация терпеливой сортировки
*/

//Данная функция конвертирует стек в массив
int stackToArray(Stack* stack, double* arr, int maxSize) {
    int i = 0;
    Node* current = stack->top;
    while (current != NULL && i < maxSize) {
        arr[i++] = current->data;
        current = current->next;
    }
    // Разворачиваем массив, так как стек LIFO, а мы читаем сверху вниз
    for (int j = 0; j < i / 2; ++j) {
        double temp = arr[j];
        arr[j] = arr[i - j - 1];
        arr[i - j - 1] = temp;
    }
    return i; // возвращаем количество элементов
}

typedef struct { //структура для одной стопки
	double *data;
	int size;
} Pile;

void init_pile(Pile *pile){ //функция для инициализации одной стопки
	pile->data = (double *)malloc(MAX_SIZE_OF_PILE * sizeof(double));
	pile->size = 0;
}

void push_to_pile(Pile *pile, double value){ //функция для добавления элементов в стек 
	pile->data[pile->size] = value;
	pile->size++;
}

double peek_from_pile(const Pile *pile){ //функция для взятия вершины стопки 
	return pile->data[pile->size-1];
}

void pop_from_pile(Pile *pile){ //функция для удаления элемента с верхушки стопки 
	if (pile->size > 0) {
		pile->size--;
	}
}

int binary_search(Pile *piles, int n, double value){ //классический бинарный поиск
	int left = 0, right = n - 1;
	while (left <= right){
		int mid = left + (right - left) / 2;
		if (peek_from_pile(&piles[mid]) <= value){
			left = mid + 1;
		} else {
			right = mid - 1;
		}
	}
	return left;
}

void patience_sort(double *array_for_sorting, int n, double *output){
	Pile piles[MAX_SIZE_OF_PILE]; //задаём массив стопок
	int pile_count = 0; //количество стопок 
	
	for (int i = 0; i < n; ++i){
		double current = array_for_sorting[i];
		int pos = binary_search(piles, pile_count, current);
		if (pos < pile_count) { //если стопка найдена, то добавим элемент в стопку 
			push_to_pile(&piles[pos], current);
		} else { //если стопка не найдена, то создаём новую 
			init_pile(&piles[pile_count]); 
			push_to_pile(&piles[pile_count], current);
			pile_count++;
		}
	}
	int index = 0;
	while (pile_count > 0){
		double min_value = 1e+100;
		int min_pile = -1; 
		//найдём стопку с минимальным элементом 
		for (int i = 0; i < pile_count; ++i){
			if (peek_from_pile(&piles[i]) < min_value){
				min_value = peek_from_pile(&piles[i]);
				min_pile = i;
			}
		}
		output[index++] = min_value;
		pop_from_pile(&piles[min_pile]); //удалим этот элемент из стопки 
		if (piles[min_pile].size == 0) { //удалим всю стопку целиком 
			free(piles[min_pile].data);
			for (int i = min_pile; i < pile_count - 1; ++i){
				piles[i] = piles[i + 1];
			}
			pile_count--;
		}
	}
	
}

int main(){
    Stack* stack = createStack();
    int counter = 0; 
	readFromFile(stack, &counter); //Читаем данные из файла
    
	double arr[MAX_SIZE];
    int arrSize = stackToArray(stack, arr, MAX_SIZE);

    // Инициализация красно-чёрного дерева
    RBNode* root = NULL;

    // Выполнение терпеливой сортировки с вставкой в красно-чёрное дерево
    double sorted_arr[MAX_SIZE];
    patience_sort(arr, arrSize, sorted_arr);

    for (int i = arrSize-1; i >= 0; i--) {
        insert(&root, sorted_arr[i]);
    }

    // Печать отсортированного дерева
    printf("Красно-чёрное дерево после терпеливой сортировки:\n");
    printTreeRotated(root, 0);

    // Освобождение памяти для стека
    while (stack->top != NULL) {
        delFromStack(stack);
    }
    free(stack);

    return 0;
}