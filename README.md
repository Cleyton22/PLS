# PLS
PLS para o problema da árvore geradora biobjetiva

O arquivo de entrada da instância da árvore geradora biobjetiva (AG-BI) deve seguir o mesmo padrão que os arquivos .grasp para o algoritmo funcinar corretamente. Para escolher qual instância o algoritmo usará, é preciso mudar o nome do aquivo "20.grasp1.in" na linha 766 "FILE *file = fopen("20.grasp1.in", "r");", na função main, para a instância desejada.

Para modificar quantas soluções aleatórias seão criadas para selecionar as apenas soluções não dominadas entre sí, na qual serão usadas como entrada para o PLS é preciso alterar o valor da constante: POPULATION_SIZE

Para visualizar as arestas e o valor total dos objetivos de cada solução no terminal é preciso remover "/" e "/" das seguintes seções:

/*

printf("Solucoes nao dominadas encontradas na primeira populacao:\n\n");

for (Node *node = unexploredSolutions->head; node != NULL; node = node->next){

printSolution(node->solution);

}\*/

/*

printf("Todas solucoes nao dominadas encontradas apos a execucao completa:\n\n");

for (Node *node = allNonDominatedSolutions->head; node != NULL; node = node->next){

printSolution(node->solution);

}\*/

No momento os valores total de cada objetivo de cada solução não dominada na população inicial e no pareto front são escritos nos arquivos: ValoresDoObjetivo1DasSoluçõesIniciais, ValoresDoObjetivo2DasSoluçõesIniciais, ValoresDoObjetivo1DasSoluçõesFinais e ValoresDoObjetivo2DasSoluçõesFinais. Esse metódo foi escolhido para não transbordar o terminal, além de facilitar a armazenação dos resultados em algo como uma planilha.
