#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define NUM_OBJECTIVES 2
#define POPULATION_SIZE 200

typedef struct {
    int vertex1; // Vértice 1 da aresta
    int vertex2; // Vértice 2 da aresta
    int objectives[NUM_OBJECTIVES]; // Array dos valores dos objetivos da aresta
} Edge;




typedef struct {
    Edge *edges;  // Array de arestas da solução
    int numEdges;  // Número de arestas na solução
    int totalObjective1; // Total do objetivo 1 da solução
    int totalObjective2; // Total do objetivo 2 da solução
    bool exploration; // Indicador de vizinhança explorada
} Solution;




typedef struct {
    Solution *solutions;  // Array de soluções da população
    int numSolutions;  // Número de soluções na população
} Population;




typedef struct Node {
    Solution *solution;  // Solução armazenada no nó da lista
    struct Node *next;  // Ponteiro para o próximo nó na lista
} Node;




typedef struct {
    Node *head;  // Ponteiro para o primeiro nó na lista
    int size;  // Número de nós na lista
} List;




// Função para receber uma solução e retornar a cópia dela
Solution* copySolution(Solution *originalSolution){
    Solution *copiedSolution = malloc(sizeof(Solution));
    copiedSolution->exploration = originalSolution->exploration;
    copiedSolution->totalObjective1 = originalSolution->totalObjective1;
    copiedSolution->totalObjective2 = originalSolution->totalObjective2;

    copiedSolution->numEdges = originalSolution->numEdges;
    copiedSolution->edges = malloc(copiedSolution->numEdges * sizeof(Edge));
    for (int i = 0; i < copiedSolution->numEdges; i++){
        copiedSolution->edges[i] = originalSolution->edges[i];
    }

    return copiedSolution;
}




// Função para criar uma lista
List *createList() {
    List *list = malloc(sizeof(List));
    list->head = NULL;
    list->size = 0;
    return list;
}




// Função para verificar se uma lista está vazia
int isListEmpty(List *list) {
    return list->size == 0;
}




// Função para adicionar uma solução a uma lista
Node* addToList(List *list, Solution *solution, Node *lastNode) {
    Node *node = malloc(sizeof(Node));
    node->solution = solution;
    node->next = NULL;

    if (list->head == NULL) {
        list->head = node;
    } else {
        lastNode->next = node;
    }

    list->size++;
    return node;
}




// Função para remover uma solução de uma lista
void removeFromList(List *list, Solution *solution) {
    Node *current = list->head;
    Node *previous = NULL;

    while (current != NULL && current->solution != solution) {
        previous = current;
        current = current->next;
    }

    if (current == NULL) {
        return;
    }

    if (previous == NULL) {
        list->head = current->next;
    } else {
        previous->next = current->next;
    }

    free(current);
    list->size--;
}




// Função para liberar a memória de uma população de soluções
void freePopulation(Population *population) {
    for (int i = 0; i < population->numSolutions; i++) {
        free(population->solutions[i].edges);
    }
    free(population->solutions);
    free(population);
}




// Função para liberar a memória de uma lista
void freeList(List *list) {
    Node *current = list->head;
    Node *next;
    while (current != NULL) {
        next = current->next;
        free(current->solution->edges);
        free(current->solution);
        free(current);
        current = next;
    }
    free(list);
}




// Função para imprimir os dados de uma solução
void printSolution(Solution *solution) {
    for (int i = 0; i < solution->numEdges; i++) {
        printf("Aresta %d: %d-%d [%d, %d]\n", i + 1, solution->edges[i].vertex1, solution->edges[i].vertex2,
               solution->edges[i].objectives[0], solution->edges[i].objectives[1]);
    }
    printf("\nObjetivo 1: %d | Objetivo 2: %d\n", solution->totalObjective1, solution->totalObjective2);
    printf("\n");
}




// Função para imprimir os dados de uma populção
void printPopulation(Population *population) {
    for (int p = 0; p < population->numSolutions; p++) {
        printf("Solucao %d:\n\n", p + 1);
        for (int i = 0; i < population->solutions[p].numEdges; i++) {
            printf("Aresta %d: %d-%d [%d, %d]\n", i + 1, population->solutions[p].edges[i].vertex1, population->solutions[p].edges[i].vertex2,
                   population->solutions[p].edges[i].objectives[0], population->solutions[p].edges[i].objectives[1]);
        }
        printf("\nObjetivo 1: %d | Objetivo 2: %d\n", population->solutions[p].totalObjective1, population->solutions[p].totalObjective2);
        printf("\n");
    }
}




// Função para imprimir os dados de todas as aresta do grafo original
void printInstanceData(Edge *allEdges, int amountOfAllEdges) {
    printf("Grafo original:\n\n");
    for (int i = 0; i < amountOfAllEdges; i++) {
        printf("%d %d %d %d\n", allEdges[i].vertex1, allEdges[i].vertex2,
               allEdges[i].objectives[0], allEdges[i].objectives[1]);
    }
}




// Função para encontrar o conjunto de um elemento i (usando compressão de caminho)
int find(int parent[], int i) {
    if (parent[i] == -1)
        return i;
    return find(parent, parent[i]);
}



// Função para unir dois subconjuntos em um único conjunto
void unionEdge(int parent[], int x, int y) {
    int xset = find(parent, x);
    int yset = find(parent, y);
    parent[xset] = yset;
}



// Função para verificar se a adição de uma nova aresta cria um ciclo
int doesFormCycle(int parent[], int vertex1, int vertex2) {
    int x = find(parent, vertex1);
    int y = find(parent, vertex2);

    if (x == y)
        return 1;

    unionEdge(parent, x, y);
    return 0;
}




// Função para criar uma solução válida
Solution createInitialSolution(Edge *allEdges, int amountOfAllEdges, int numVertices) {
    Solution solution;
    solution.edges = malloc((numVertices - 1) * sizeof(Edge));
    solution.numEdges = 0;
    solution.totalObjective1 = 0;
    solution.totalObjective2 = 0;

    int *parent = malloc(numVertices * sizeof(int));

    for (int v = 0; v < numVertices; ++v){
        parent[v] = -1;
    }

    while (solution.numEdges < numVertices - 1) {
        int randomIndex = rand() % amountOfAllEdges;
        Edge randomEdge = allEdges[randomIndex];

        if (!doesFormCycle(parent, randomEdge.vertex1 - 1, randomEdge.vertex2 - 1)) {
            solution.edges[solution.numEdges] = randomEdge;
            solution.numEdges++;
            solution.totalObjective1 += randomEdge.objectives[0];
            solution.totalObjective2 += randomEdge.objectives[1];
        }
    }

    free(parent);
    return solution;
}




// Função para criar uma população de soluções válidas
Population* createInitialPopulation(Edge *allEdges, int amountOfAllEdges, int numVertices) {
    Population *population = malloc(sizeof(Population));
    population->solutions = malloc(POPULATION_SIZE * sizeof(Solution));
    population->numSolutions = POPULATION_SIZE;

    for (int i = 0; i < POPULATION_SIZE; i++) {
        population->solutions[i] = createInitialSolution(allEdges, amountOfAllEdges, numVertices);
    }

    return population;
}




// Função para verificar se uma solução 1 domina uma solução 2
int dominates(Solution *solution1, Solution *solution2) {
    return (solution1->totalObjective1 <= solution2->totalObjective1 && solution1->totalObjective2 < solution2->totalObjective2) ||
           (solution1->totalObjective1 < solution2->totalObjective1 && solution1->totalObjective2 <= solution2->totalObjective2);
}




// Função para escolher um número aleatório para qual solução vai ser explorado a vizinhança
int pickRandomSolution(int y) {
    return rand() % (y + 1);
}




// Função para verificar se um vértice foi removido da solução durante a busca de vizinhos
int findMissingVertice(int *array, int *size, int find){
    int found;
    for(int i = 0; i < size[0]; i++){
        if(array[i] == find){
            return 0;
        } else{
            found = -1;;
        }
    }
    return found;
}




// Função secundária para armazenar quais arestas estão conectadas entre sí em uma solução
void verticesConnecteds2(Solution *newSolution, int *part, int *sizeOfPart, int search){
    for(int i = 0; i < newSolution->numEdges; i++){
        if(newSolution->edges[i].vertex1 == search){
            part[sizeOfPart[0]] = newSolution->edges[i].vertex2;
            sizeOfPart[0]++;
            int searchAgain = newSolution->edges[i].vertex2;
            newSolution->edges[i].vertex1 = 0;
            newSolution->edges[i].vertex2 = 0;
            verticesConnecteds2(newSolution, part, sizeOfPart, searchAgain);

        } else if(newSolution->edges[i].vertex2 == search){
            part[sizeOfPart[0]] = newSolution->edges[i].vertex1;
            sizeOfPart[0]++;
            int searchAgain = newSolution->edges[i].vertex1;
            newSolution->edges[i].vertex1 = 0;
            newSolution->edges[i].vertex2 = 0;
            verticesConnecteds2(newSolution, part, sizeOfPart, searchAgain);

        }
    }
}




// Função primária para armazenar quais arestas estão conectadas entre sí em uma solução
void verticesConnecteds(Solution *newSolution, int *part, int *sizeOfPart, int search, int *stop){
    for(int i = 0; i < newSolution->numEdges; i++){
        if(newSolution->edges[i].vertex1 == search){
            part[sizeOfPart[0]] = newSolution->edges[i].vertex2;
            sizeOfPart[0]++;
            if(stop[0] == 0){
                part[sizeOfPart[0]] = newSolution->edges[i].vertex1;
                sizeOfPart[0]++;
                stop[0] = 1;
            }
            int searchAgain = newSolution->edges[i].vertex2;
            newSolution->edges[i].vertex1 = 0;
            newSolution->edges[i].vertex2 = 0;
            verticesConnecteds2(newSolution, part, sizeOfPart, searchAgain);

        } else if(newSolution->edges[i].vertex2 == search){
            part[sizeOfPart[0]] = newSolution->edges[i].vertex1;
            sizeOfPart[0]++;
            if(stop[0] == 0){
                part[sizeOfPart[0]] = newSolution->edges[i].vertex2;
                sizeOfPart[0]++;
                stop[0] = 1;
            }
            int searchAgain = newSolution->edges[i].vertex1;
            newSolution->edges[i].vertex1 = 0;
            newSolution->edges[i].vertex2 = 0;
            verticesConnecteds2(newSolution, part, sizeOfPart, searchAgain);

        }
    }
}




// Função para criar uma população inicial de soluções não dominadas para usar como entrada para o PLS
List* createInitialNonDominatedPopulation(Edge *allEdges, int amountOfAllEdges, int numVertices) {
    Population *randomInitialPopulation = createInitialPopulation(allEdges, amountOfAllEdges, numVertices);

    List *nonDominatedSolutions = createList();
    Node *lastNode = NULL;

    int accepted = 0;
    for (int i = 0; i < randomInitialPopulation->numSolutions; i++){
        accepted = 0;
        for(int j = 0; j < randomInitialPopulation->numSolutions; j++){
            if(i != j){
                if (dominates(&randomInitialPopulation->solutions[j], &randomInitialPopulation->solutions[i])){
                    accepted = 1;
                }
            }
        }

        if(accepted != 1){
            Solution *acceptedSolution = copySolution(&randomInitialPopulation->solutions[i]);
            lastNode = addToList(nonDominatedSolutions, acceptedSolution, lastNode);
        }
    }

    freePopulation(randomInitialPopulation);
    return nonDominatedSolutions;
}




// Função para verificar, adicionar e remover soluções da lista de soluções não dominadas
void onlyAddNonDominatedSolutions(List *temporaryNeighborsSolutions, Solution *neighbor){
    if(isListEmpty(temporaryNeighborsSolutions)){
        Solution *newSolution = copySolution(neighbor);
        Node *lastNode = NULL;
        lastNode = addToList(temporaryNeighborsSolutions, newSolution, lastNode);
    } else{
        int proceed = 0;

        for (Node *current = temporaryNeighborsSolutions->head; current != NULL; current = current->next) {
            if(dominates(current->solution, neighbor) || (current->solution->totalObjective1 == neighbor->totalObjective1 && current->solution->totalObjective2 == neighbor->totalObjective2)){
                proceed = 1;
                break;
            }

            if(current->next == NULL){
                Solution *newSolution = copySolution(neighbor);
                Node *newNode = malloc(sizeof(Node));
                newNode->solution = newSolution;
                newNode->next = NULL;
                current->next = newNode;
                temporaryNeighborsSolutions->size++;
                break;
            }
        }

        if(proceed == 0){
            Node *current2 = temporaryNeighborsSolutions->head;
            Node *previous2 = NULL;

            while (current2 != NULL) {
                Node *nextNode = current2->next;
                if(dominates(neighbor, current2->solution)){
                    if(previous2 == NULL){
                        temporaryNeighborsSolutions->head = current2->next;
                    }else{
                        previous2->next = current2->next;
                    }
                    free(current2->solution->edges);
                    free(current2->solution);
                    free(current2);
                    temporaryNeighborsSolutions->size--;
                } else {
                    previous2 = current2;
                }
                current2 = nextNode;
            }
        }
    }
}




// Função para calcular qual a posição de uma determinada aresta no grafo original
int calculateEdgePositionOnAllEdges(int vertice1, int vertice2, int numVertices){
    int edgePosition = numVertices * (vertice1 - 1) - (vertice1 * (vertice1 - 1)) / 2 + (vertice2 - vertice1);
    return edgePosition;
}




// Função para buscar os vizinhos de uma solução
void neighborSearch(Solution *solutionToFindNeighbor, Edge *allEdges, int pickedEdge, List *temporaryNeighborsSolutions, int numVertices){
    int *stop = calloc(1, sizeof(int));

    int verticesCopy[NUM_OBJECTIVES];
    verticesCopy[0] = solutionToFindNeighbor->edges[pickedEdge].vertex1;
    verticesCopy[1] = solutionToFindNeighbor->edges[pickedEdge].vertex2;

    int objectivesCopy[NUM_OBJECTIVES];
    objectivesCopy[0] = solutionToFindNeighbor->edges[pickedEdge].objectives[0];
    objectivesCopy[1] = solutionToFindNeighbor->edges[pickedEdge].objectives[1];

    solutionToFindNeighbor->edges[pickedEdge].vertex1 = 0;
    solutionToFindNeighbor->edges[pickedEdge].vertex2 = 0;
    solutionToFindNeighbor->totalObjective1 -= objectivesCopy[0];
    solutionToFindNeighbor->totalObjective2 -= objectivesCopy[1];
    solutionToFindNeighbor->edges[pickedEdge].objectives[0] = 0;
    solutionToFindNeighbor->edges[pickedEdge].objectives[1] = 0;

    int startPosition;
    if (pickedEdge == 0){
        startPosition = 1;
    } else{
        startPosition = 0;
    }

    int search = solutionToFindNeighbor->edges[startPosition].vertex1;

    int *firstPart = calloc(solutionToFindNeighbor->numEdges, sizeof(int));
    int *sizeOfPart1 = calloc(1, sizeof(int));
    int *secondPart = calloc(solutionToFindNeighbor->numEdges, sizeof(int));
    int *sizeOfPart2 = calloc(1, sizeof(int));

    Solution *temporaryCopy = copySolution(solutionToFindNeighbor);

    verticesConnecteds(temporaryCopy, firstPart, sizeOfPart1, search, stop);
    stop[0] = 0;

    if(sizeOfPart1[0] == solutionToFindNeighbor->numEdges){
        int test = findMissingVertice(firstPart, sizeOfPart1, verticesCopy[0]);
        if (test == -1){
            for (int i = 0; i < sizeOfPart1[0]; i++){
                int verticeMissing = firstPart[i];
                if (firstPart[i] == verticesCopy[1]){
                    if ((i + 1) == sizeOfPart1[0]){
                        break;
                    } else{
                        verticeMissing = firstPart[i + 1];
                        i++;
                    }
                }

                if (verticeMissing < verticesCopy[0]){
                    Solution *neighbor = copySolution(solutionToFindNeighbor);
                    int positionInAllEdges = calculateEdgePositionOnAllEdges(verticeMissing, verticesCopy[0], numVertices);
                    neighbor->edges[pickedEdge] = allEdges[positionInAllEdges - 1];
                    neighbor->totalObjective1 += allEdges[positionInAllEdges - 1].objectives[0];
                    neighbor->totalObjective2 += allEdges[positionInAllEdges - 1].objectives[1];
                    onlyAddNonDominatedSolutions(temporaryNeighborsSolutions, neighbor);
                } else{
                    Solution *neighbor = copySolution(solutionToFindNeighbor);
                    int positionInAllEdges = calculateEdgePositionOnAllEdges(verticesCopy[0], verticeMissing, numVertices);
                    neighbor->edges[pickedEdge] = allEdges[positionInAllEdges - 1];
                    neighbor->totalObjective1 += allEdges[positionInAllEdges - 1].objectives[0];
                    neighbor->totalObjective2 += allEdges[positionInAllEdges - 1].objectives[1];
                    onlyAddNonDominatedSolutions(temporaryNeighborsSolutions, neighbor);
                }
            }
        } else{
            for (int i = 0; i < sizeOfPart1[0]; i++){
                int verticeMissing = firstPart[i];

                if (firstPart[i] == verticesCopy[0]){
                    if ((i + 1) == sizeOfPart1[0]){
                        break;
                    } else{
                        verticeMissing = firstPart[i + 1];
                        i++;
                    }
                }

                if (verticeMissing < verticesCopy[1]){
                    Solution *neighbor = copySolution(solutionToFindNeighbor);
                    int positionInAllEdges = calculateEdgePositionOnAllEdges(verticeMissing, verticesCopy[1], numVertices);
                    neighbor->edges[pickedEdge] = allEdges[positionInAllEdges - 1];
                    neighbor->totalObjective1 += allEdges[positionInAllEdges - 1].objectives[0];
                    neighbor->totalObjective2 += allEdges[positionInAllEdges - 1].objectives[1];
                    onlyAddNonDominatedSolutions(temporaryNeighborsSolutions, neighbor);
                } else{
                    Solution *neighbor = copySolution(solutionToFindNeighbor);
                    int positionInAllEdges = calculateEdgePositionOnAllEdges(verticesCopy[1], verticeMissing, numVertices);
                    neighbor->edges[pickedEdge] = allEdges[positionInAllEdges - 1];
                    neighbor->totalObjective1 += allEdges[positionInAllEdges - 1].objectives[0];
                    neighbor->totalObjective2 += allEdges[positionInAllEdges - 1].objectives[1];
                    onlyAddNonDominatedSolutions(temporaryNeighborsSolutions, neighbor);
                }
            }
        }
    } else{
        for(int i = 0; i < temporaryCopy->numEdges; i++){
            if (temporaryCopy->edges[i].vertex1 != 0){
                int search2 = temporaryCopy->edges[i].vertex1;
                verticesConnecteds(temporaryCopy, secondPart, sizeOfPart2, search2, stop);

                for (int i = 0; i < sizeOfPart1[0]; i++){
                    for (int h = 0; h < sizeOfPart2[0]; h++){
                        int vertice1 = firstPart[i];
                        int vertice2 = secondPart[h];

                        if ((vertice1 == verticesCopy[0] && vertice2 == verticesCopy[1]) || (vertice1 == verticesCopy[1] && vertice2 == verticesCopy[0])){
                            if ((h + 1) == sizeOfPart2[0]){
                                break;
                            } else{
                                vertice2 = secondPart[h + 1];
                                h++;
                            }
                        }

                        if (vertice1 < vertice2){
                            Solution *neighbor = copySolution(solutionToFindNeighbor);
                            int positionInAllEdges = calculateEdgePositionOnAllEdges(vertice1, vertice2, numVertices);
                            neighbor->edges[pickedEdge] = allEdges[positionInAllEdges - 1];
                            neighbor->totalObjective1 += allEdges[positionInAllEdges - 1].objectives[0];
                            neighbor->totalObjective2 += allEdges[positionInAllEdges - 1].objectives[1];
                            onlyAddNonDominatedSolutions(temporaryNeighborsSolutions, neighbor);
                        } else{
                            Solution *neighbor = copySolution(solutionToFindNeighbor);
                            int positionInAllEdges = calculateEdgePositionOnAllEdges(vertice2, vertice1, numVertices);
                            neighbor->edges[pickedEdge] = allEdges[positionInAllEdges - 1];
                            neighbor->totalObjective1 += allEdges[positionInAllEdges - 1].objectives[0];
                            neighbor->totalObjective2 += allEdges[positionInAllEdges - 1].objectives[1];
                            onlyAddNonDominatedSolutions(temporaryNeighborsSolutions, neighbor);
                        }
                    }
                }
                break;
            }
        }
    }

    solutionToFindNeighbor->edges[pickedEdge].vertex1 = verticesCopy[0];
    solutionToFindNeighbor->edges[pickedEdge].vertex2 = verticesCopy[1];
    solutionToFindNeighbor->totalObjective1 += objectivesCopy[0];
    solutionToFindNeighbor->totalObjective2 += objectivesCopy[1];
    solutionToFindNeighbor->edges[pickedEdge].objectives[0] = objectivesCopy[0];
    solutionToFindNeighbor->edges[pickedEdge].objectives[1] = objectivesCopy[1];

    free(stop);
    free(firstPart);
    free(sizeOfPart1);
    free(secondPart);
    free(sizeOfPart2);
    free(temporaryCopy->edges);
    free(temporaryCopy);
}




// Função para executar os passos principais do PLS até o encontrar o pareto local optimum
void plsMainLoop(Edge *allEdges, int amountOfAllEdges, int numVertices){
    clock_t start, end, pauseStart, pauseEnd;
    double cpuTimeUsed;

    start = clock();

    List *unexploredSolutions = createInitialNonDominatedPopulation(allEdges, amountOfAllEdges, numVertices);

    pauseStart = clock();
    /*
    printf("Solucoes nao dominadas encontradas na primeira populacao:\n\n");
    for (Node *node = unexploredSolutions->head; node != NULL; node = node->next){
        printSolution(node->solution);
    }*/

    FILE *outfile = fopen("ValoresDoObjetivo1DasSoluçõesIniciais.in", "w");
    if (outfile == NULL) {
        printf("Nao foi possivel abrir o arquivo para escrita.\n");
        return;
    }

    FILE *outfile2 = fopen("ValoresDoObjetivo2DasSoluçõesIniciais.in", "w");
    if (outfile2 == NULL) {
        printf("Nao foi possivel abrir o arquivo para escrita.\n");
        return;
    }

    // Escrevendo o objetivo 1 e 2 nos arquivos
    for (Node *node = unexploredSolutions->head; node != NULL; node = node->next) {
        fprintf(outfile, "%d\n", node->solution->totalObjective1);
        fprintf(outfile2, "%d\n", node->solution->totalObjective2);
    }

    fclose(outfile);
    fclose(outfile2);

    pauseEnd = clock();

    for(Node *node = unexploredSolutions->head; node != NULL; node = node->next){
        node->solution->exploration = false;
    }

    List *allNonDominatedSolutions = createList();
    Node *lastNode = NULL;
    for(Node *node = unexploredSolutions->head; node != NULL; node = node->next){
        lastNode = addToList(allNonDominatedSolutions, node->solution, lastNode);
    }

    do{
        int picked = pickRandomSolution(unexploredSolutions->size - 1);
        List *temporaryNeighborsSolutions = createList();

        Node *current = unexploredSolutions->head;
        for (int i = 0; i < picked; i++){
            current = current->next;
        }

        for (int j = 0; j < current->solution->numEdges; j++){
            neighborSearch(current->solution, allEdges, j, temporaryNeighborsSolutions, numVertices);
        }

        current->solution->exploration = true;

        for(Node *node = temporaryNeighborsSolutions->head; node != NULL; node = node->next){
            onlyAddNonDominatedSolutions(allNonDominatedSolutions, node->solution);
        }

        Node* currentEmptyer = unexploredSolutions->head;
        Node* nextEmptyer;
        while (currentEmptyer != NULL) {
            nextEmptyer = currentEmptyer->next;
            free(currentEmptyer);
            unexploredSolutions->size--;
            currentEmptyer = nextEmptyer;
        }
        unexploredSolutions->head = NULL;
        Node *lastNode2 = NULL;

        for (Node *node = allNonDominatedSolutions->head; node != NULL; node = node->next){
            if (node->solution->exploration == false){
                lastNode2 = addToList(unexploredSolutions, node->solution, lastNode2);
            }
        }

        freeList(temporaryNeighborsSolutions);

    } while(unexploredSolutions->size != 0);

    end = clock();

    FILE *outfile3 = fopen("ValoresDoObjetivo1DasSoluçõesFinais.in", "w");
    if (outfile3 == NULL) {
        printf("Nao foi possivel abrir o arquivo para escrita.\n");
        return;
    }

    FILE *outfile4 = fopen("ValoresDoObjetivo2DasSoluçõesFinais.in", "w");
    if (outfile4 == NULL) {
        printf("Nao foi possivel abrir o arquivo para escrita.\n");
        return;
    }

    // Escrevendo o objetivo 1 e 2 nos arquivos
    for (Node *node = allNonDominatedSolutions->head; node != NULL; node = node->next) {
        fprintf(outfile3, "%d\n", node->solution->totalObjective1);
        fprintf(outfile4, "%d\n", node->solution->totalObjective2);
    }

    fclose(outfile3);
    fclose(outfile4);
    /*
    printf("Todas solucoes nao dominadas encontradas apos a execucao completa:\n\n");
    for (Node *node = allNonDominatedSolutions->head; node != NULL; node = node->next){
        printSolution(node->solution);
    }*/

    printf("Quantidade de solucoes encontradas: %d\n\n", allNonDominatedSolutions->size);

    cpuTimeUsed = ((double) (end - start - (pauseEnd - pauseStart))) / CLOCKS_PER_SEC;
    printf("PLS levou %f segundos para executar \n", cpuTimeUsed);
}




int main() {
    srand(time(0));

    FILE *file = fopen("20.grasp1.in", "r");
    if (file == NULL) {
        printf("Nao foi possivel abrir o arquivo.\n");
        return 1;
    }

    int numVertices;
    fscanf(file, "%d", &numVertices);

    int amountOfAllEdges = (numVertices * (numVertices - 1)) / 2;

    Edge *allEdges = malloc(amountOfAllEdges * sizeof(Edge));
    for (int i = 0; i < amountOfAllEdges; i++) {
        fscanf(file, "%d %d %d %d", &allEdges[i].vertex1, &allEdges[i].vertex2, &allEdges[i].objectives[0], &allEdges[i].objectives[1]);
    }

    fclose(file);

    plsMainLoop(allEdges, amountOfAllEdges, numVertices);

    return 0;
}
