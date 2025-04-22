#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define NUM_KEYW 34
#define TK_PALAVRA_RESERVADA 1
#define TK_NUMERO            2
#define TK_IDENTIFICADOR     3
#define TK_OPERADOR          4
#define TK_DELIMITADOR       5
#define TK_SEPARADOR         6
#define TK_LITERAL           7

int read_char(FILE *file, int *line) {
    int ch = fgetc(file);
    if (ch == '\n') (*line)++;
    return ch;
}

int main() {
    FILE *file = fopen("arquivo.txt", "r");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    int ch, line = 1;
    char word[100] = {0}, num[100] = {0}, literal[256] = {0}, delimitador_stack[256] = {0};
    int word_index = 0, num_index = 0, literal_index = 0, delimitador_top = -1;
    int delimitador_line[256] = {0};

read_next:
    ch = read_char(file, &line);
    if (ch == EOF) goto end;

    if (isspace(ch)) goto read_next;

    // Literais
    if (ch == '"') {
        literal_index = 0;
        literal[literal_index++] = (char)ch;
        int literal_start_line = line;
        int closed = 0;

        while ((ch = read_char(file, &line)) != EOF && literal_index < 255) {
            literal[literal_index++] = (char)ch;
            if (ch == '\\') {
                int escaped = read_char(file, &line);
                if (escaped == EOF) break;
                literal[literal_index++] = (char)escaped;
            } else if (ch == '"') {
                closed = 1;
                break;
            }
        }

        literal[literal_index] = '\0';

        if (closed) {
            printf("Literal: %s | ID: %d\n", literal, TK_LITERAL);
        } else {
            printf("Linha %d\nERRO: literal não fechada\n", literal_start_line);
            while (ch != '\n' && ch != EOF) ch = read_char(file, &line);
        }
        goto read_next;
    }

    // Delimitadores
    if (ch == '(' || ch == '{' || ch == '[') {
        delimitador_stack[++delimitador_top] = ch;
        delimitador_line[delimitador_top] = line;
        printf("Delimitador: %c | ID: %d\n", ch, TK_DELIMITADOR);
        goto read_next;
    }
    if (ch == ')' || ch == '}' || ch == ']') {
        if (delimitador_top < 0) {
            printf("Linha %d\nERRO: delimitador sem abertura correspondente\n", line);
        } else {
            char topo = delimitador_stack[delimitador_top];
            if ((topo == '(' && ch == ')') || (topo == '{' && ch == '}') || (topo == '[' && ch == ']')) {
                delimitador_top--;
                printf("Delimitador: %c | ID: %d\n", ch, TK_DELIMITADOR);
            } else {
                printf("Linha %d\nERRO: delimitador incompatível (aberto com '%c', fechado com '%c')\n", line, topo, ch);
            }
        }
        goto read_next;
    }

    // Separadores
    if (ch == ';' || ch == ',' || ch == '.') {
        printf("Separador: %c | ID: %d\n", ch, TK_SEPARADOR);
        goto read_next;
    }

    // Números
    if (isdigit(ch)) {
        num_index = 0;
read_number:
        num[num_index++] = (char)ch;
        ch = read_char(file, &line);
        if (isdigit(ch) || ch == '.') goto read_number;

        num[num_index] = '\0';
        printf("Número: %s | ID: %d\n", num, TK_NUMERO);
        ungetc(ch, file);
        goto read_next;
    }

    // Palavras-chave/Identificadores
    if (isalpha(ch) || ch == '_') {
        word_index = 0;
read_word:
        word[word_index++] = (char)ch;
        ch = read_char(file, &line);
        if (isalnum(ch) || ch == '_') goto read_word;

        word[word_index] = '\0';
        const char *keyWords[NUM_KEYW] = {
            "auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else",
            "enum", "extern", "float", "for", "goto", "if", "inline", "int", "long", "register",
            "restrict", "return", "short", "signed", "sizeof", "static", "struct", "switch", "typedef",
            "union", "unsigned", "void", "volatile", "while"
        };

        for (int i = 0; i < NUM_KEYW; i++) {
            if (strcmp(word, keyWords[i]) == 0) {
                printf("Palavra reservada: %s | ID: %d\n", word, TK_PALAVRA_RESERVADA);
                ungetc(ch, file);
                goto read_next;
            }
        }

        printf("Identificador: %s | ID: %d\n", word, TK_IDENTIFICADOR);
        ungetc(ch, file);
        goto read_next;
    }

    // Operadores
    if (strchr("=!<>+-*/%", ch)) {
        char op_str[3] = {(char)ch, '\0', '\0'};
        int next_ch = read_char(file, &line);

        if ((ch == '=' && next_ch == '=') || (ch == '!' && next_ch == '=') ||
            (ch == '<' && next_ch == '=') || (ch == '>' && next_ch == '=') ||
            (ch == '+' && next_ch == '+') || (ch == '-' && next_ch == '-')) {
            op_str[1] = (char)next_ch;
        } else {
            ungetc(next_ch, file);
        }

        printf("Operador: %s | ID: %d\n", op_str, TK_OPERADOR);
        goto read_next;
    }

    goto read_next;

end:
    while (delimitador_top >= 0) {
        printf("Linha %d\nERRO: delimitador '%c' não fechado\n",
               delimitador_line[delimitador_top], delimitador_stack[delimitador_top]);
        delimitador_top--;
    }

    fclose(file);
    return 0;
}
