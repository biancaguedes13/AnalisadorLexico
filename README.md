# Analisador Lexico
Analisador Léxico. UFSJ, Compiladores - 2025/1

Este projeto implementa um analisador léxico em C, que identifica os principais tokens de um código-fonte em C, além de detectar erros léxicos comuns. Até o momento, o analisador léxico é capaz de reconhecer os seguintes erros:
- **Delimitadores não balanceados** (ex: abre `{` mas não fecha)
- **Literais de string não fechadas**
- **Delimitadores fechados sem abertura correspondente**

makefile incluso. Para executar, use o comando make e depois ./main
