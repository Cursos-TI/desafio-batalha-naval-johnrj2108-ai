#include <stdio.h>

#define TAM 10        // tamanho do tabuleiro
#define NAVIO 3       // valor que representa um navio
#define AREA 5        // valor para área de habilidade sobre água
#define NAVIO_AREA 8  // valor para indicar navio + área afetada
#define SKILL_N 5     // tamanho das matrizes de habilidade (5x5)
#define SKILL_CENTER (SKILL_N/2)
#define SKILL_CONE 0
#define SKILL_CROSS 1
#define SKILL_OCTAHEDRON 2

// Função para inicializar o tabuleiro com 0
void inicializa_tabuleiro(int tab[TAM][TAM]) {
    for (int i = 0; i < TAM; i++)
        for (int j = 0; j < TAM; j++)
            tab[i][j] = 0;
}

// Funções de verificação/posicionamento de navios (do programa anterior)
// Aqui colocamos 4 navios tamanho 3: dois retos e dois diagonais
int pode_posicionar_reto(int tab[TAM][TAM], int linha, int coluna, int direcao) {
    for (int i = 0; i < 3; i++) {
        int r = linha + (direcao == 1 ? i : 0);
        int c = coluna + (direcao == 0 ? i : 0);
        if (r >= TAM || c >= TAM) return 0;
        if (tab[r][c] != 0) return 0;
    }
    return 1;
}
void posicionar_reto(int tab[TAM][TAM], int linha, int coluna, int direcao) {
    for (int i = 0; i < 3; i++) {
        int r = linha + (direcao == 1 ? i : 0);
        int c = coluna + (direcao == 0 ? i : 0);
        tab[r][c] = NAVIO;
    }
}

int pode_posicionar_diagonal(int tab[TAM][TAM], int linha, int coluna, int sentido) {
    for (int i = 0; i < 3; i++) {
        int r = linha + i;
        int c = coluna + (sentido == 0 ? i : -i);
        if (r >= TAM || c < 0 || c >= TAM) return 0;
        if (tab[r][c] != 0) return 0;
    }
    return 1;
}
void posicionar_diagonal(int tab[TAM][TAM], int linha, int coluna, int sentido) {
    for (int i = 0; i < 3; i++) {
        int r = linha + i;
        int c = coluna + (sentido == 0 ? i : -i);
        tab[r][c] = NAVIO;
    }
}

// Constrói a matriz da habilidade "cone" dinamicamente (5x5). Topo no topo, se expandindo para baixo.
void construir_cone(int cone[SKILL_N][SKILL_N]) {
    int center = SKILL_CENTER; // 2 para SKILL_N=5
    for (int i = 0; i < SKILL_N; i++) {
        for (int j = 0; j < SKILL_N; j++) {
            // Inicializa com 0
            cone[i][j] = 0;
            // Condicional para formar um "cone" que parte do topo e se expande pra baixo
            // Só desenhamos até a linha center (i <= center) para criar um cone apontando para baixo
            if (i <= center) {
                // se a distância horizontal até o centro for <= i, então faz parte do cone
                if ( (j >= center - i) && (j <= center + i) ) {
                    cone[i][j] = 1;
                }
            }
        }
    }
}

// Constrói matrix da habilidade "cruz" (5x5) com origem no centro
void construir_cruz(int cruz[SKILL_N][SKILL_N]) {
    int center = SKILL_CENTER;
    for (int i = 0; i < SKILL_N; i++) {
        for (int j = 0; j < SKILL_N; j++) {
            // Padrão cruz: linha do meio e coluna do meio são 1
            if (i == center || j == center) cruz[i][j] = 1;
            else cruz[i][j] = 0;
        }
    }
}

// Constrói matrix da habilidade "octaedro" (vista frontal -> losango / diamante) (5x5)
void construir_octaedro(int octa[SKILL_N][SKILL_N]) {
    int center = SKILL_CENTER;
    for (int i = 0; i < SKILL_N; i++) {
        for (int j = 0; j < SKILL_N; j++) {
            // Losango: pontos com distância de Manhattan até o centro <= center
            int manhattan = (i > center ? i - center : center - i) + (j > center ? j - center : center - j);
            if (manhattan <= center) octa[i][j] = 1;
            else octa[i][j] = 0;
        }
    }
}

// Aplica uma matriz de habilidade (skillMat) ao tabuleiro centrando-a em (origemR, origemC).
// Se a célula do tabuleiro for água (0) e a skill afetar -> marca AREA.
// Se a célula do tabuleiro já tiver NAVIO (3) e skill afetar -> marca NAVIO_AREA (8).
void aplicar_habilidade(int tab[TAM][TAM], int skillMat[SKILL_N][SKILL_N], int origemR, int origemC) {
    int center = SKILL_CENTER;
    for (int i = 0; i < SKILL_N; i++) {
        for (int j = 0; j < SKILL_N; j++) {
            if (skillMat[i][j] == 1) {
                // calcula coordenada no tabuleiro ao centralizar a matriz no ponto de origem
                int r = origemR - center + i;
                int c = origemC - center + j;
                // valida limites do tabuleiro
                if (r >= 0 && r < TAM && c >= 0 && c < TAM) {
                    if (tab[r][c] == 0) {
                        tab[r][c] = AREA; // marca área sobre água
                    } else if (tab[r][c] == NAVIO) {
                        tab[r][c] = NAVIO_AREA; // marca navio + área
                    } else {
                        // se já for AREA ou NAVIO_AREA, mantém (evita sobrescrever com outros códigos)
                        // neste projeto simples, não é necessário mais ação
                    }
                }
            }
        }
    }
}

// Exibe o tabuleiro no console. Valores: 0 (água), 3 (navio), 5 (área), 8 (navio+área)
void exibir_tabuleiro(int tab[TAM][TAM]) {
    printf("TABULEIRO (0=agua,3=navio,5=area,8=navio+area):\n\n");
    for (int i = 0; i < TAM; i++) {
        for (int j = 0; j < TAM; j++) {
            printf("%d ", tab[i][j]);
        }
        printf("\n");
    }
}

int main() {
    int tab[TAM][TAM];
    inicializa_tabuleiro(tab);

    // ============= Posiciona 4 navios (do exemplo anterior) ============
    // Navio reto horizontal (tamanho 3)
    int l1 = 2, c1 = 1;
    if (pode_posicionar_reto(tab, l1, c1, 0)) posicionar_reto(tab, l1, c1, 0);

    // Navio reto vertical
    int l2 = 5, c2 = 7;
    if (pode_posicionar_reto(tab, l2, c2, 1)) posicionar_reto(tab, l2, c2, 1);

    // Navio diagonal principal (↘)
    int l3 = 0, c3 = 0;
    if (pode_posicionar_diagonal(tab, l3, c3, 0)) posicionar_diagonal(tab, l3, c3, 0);

    // Navio diagonal secundária (↙)
    int l4 = 0, c4 = 9;
    if (pode_posicionar_diagonal(tab, l4, c4, 1)) posicionar_diagonal(tab, l4, c4, 1);

    // ============= Construir matrizes de habilidade (5x5) ============
    int cone[SKILL_N][SKILL_N];
    int cruz[SKILL_N][SKILL_N];
    int octa[SKILL_N][SKILL_N];

    construir_cone(cone);
    construir_cruz(cruz);
    construir_octaedro(octa);

    // (Opcional) -- imprimir as matrizes de habilidade no console para verificação
    printf("MATRIZ CONE (1=afetado,0=nao):\n");
    for (int i = 0; i < SKILL_N; i++) { for (int j = 0; j < SKILL_N; j++) printf("%d ", cone[i][j]); printf("\n"); }
    printf("\nMATRIZ CRUZ (1=afetado,0=nao):\n");
    for (int i = 0; i < SKILL_N; i++) { for (int j = 0; j < SKILL_N; j++) printf("%d ", cruz[i][j]); printf("\n"); }
    printf("\nMATRIZ OCTAEDRO (1=afetado,0=nao):\n");
    for (int i = 0; i < SKILL_N; i++) { for (int j = 0; j < SKILL_N; j++) printf("%d ", octa[i][j]); printf("\n"); }
    printf("\n");

    // ============= Definir pontos de origem no tabuleiro ============
    // Coordenadas definidas diretamente no código (linha, coluna)
    int origemConeR = 3, origemConeC = 3;    // exemplo: cone com origem em (3,3)
    int origemCruzR = 6, origemCruzC = 2;    // cruz em (6,2)
    int origemOctaR = 4, origemOctaC = 7;    // octaedro em (4,7)

    // ============= Aplicar habilidades ao tabuleiro (com validação de limites) ============
    aplicar_habilidade(tab, cone, origemConeR, origemConeC);
    aplicar_habilidade(tab, cruz, origemCruzR, origemCruzC);
    aplicar_habilidade(tab, octa, origemOctaR, origemOctaC);

    // ============= Exibir tabuleiro final ============
    exibir_tabuleiro(tab);

    return 0;
}

