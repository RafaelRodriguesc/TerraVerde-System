#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <time.h>
#include <inttypes.h>
#if defined __unix__
#include <unistd.h>
#define CLEAR_SCREEN() system("clear")
#elif defined _WIN32
#include <windows.h>
#define sleep(x) Sleep(1000 * (x))
#define CLEAR_SCREEN() system("cls")
#endif

struct usuario
{
    char *nome;
    char *senha;
};
struct empresa
{
    char *nome_do_responsavel;
    char *cpf;
    char *nome_da_empresa;
    char *cnpj;
    char *razao_social;
    char *nome_fantasia;
    char *endereco;
    char *email;
    char *abertura;
};

struct relatorio
{
    char *cnpj;
    unsigned int total_insumos_semestrais;
    unsigned int total_gastos_semestrais;
};
int empresaExistente(char *cnpj);
struct relatorio carregaRelatorio(char *cnpj);
void telaSolicitaCnpjRelatorio();

void telaInicial(), telaRegistraUsuario(), telaLoginUsuario(), telaSistema();
struct empresa carregaEmpresa(char *cnpj);
int conectaUsuario(char *nome, char *senha), salvaUsuario(struct usuario u), salvaEmpresa(struct empresa e);
struct usuario registraUsuario(char *nome, char *senha);
struct empresa registraEmpresa(char *nome_do_responsavel, char *cpf, char *nome_da_empresa, char *cnpj, char *razao_social, char *nome_fantasia, char *endereco, char *email, char *abertura);

int salvaRelatorio(struct relatorio r)
{
    char nome_arquivo[64];
    char linha[128];
    char *sigla = "relatorio";
    char *extensao = "txt";
    FILE *f;

    sprintf(nome_arquivo, "%s-%s.%s", sigla, r.cnpj, extensao);
    f = fopen(nome_arquivo, "w");

    sprintf(linha, "%s;%i;%i\n", r.cnpj, r.total_insumos_semestrais, r.total_gastos_semestrais);
    fprintf(f, "%s", linha);

    fclose(f);
    return 1;
}

struct relatorio registraRelatorio(char *cnpj, unsigned int total_insumos_semestrais, unsigned int total_gastos_semestrais)
{
    struct relatorio r;

    r.cnpj = cnpj;
    r.total_insumos_semestrais = total_insumos_semestrais;
    r.total_gastos_semestrais = total_gastos_semestrais;

    return r;
}

int main(void)
{
    setlocale(LC_ALL, "Portuguese_Brazil");
    telaInicial();
    return 0;
}

void escreveFrase(char *frase)
{
    printf("%s\n\n", frase);
}

int salvaEmpresa(struct empresa e)
{
    char *linha = malloc(256);
    FILE *f;

    f = fopen("empresas-cadastradas.txt", "a");
    sprintf(linha, "%s;%s;%s;%s;%s;%s;%s;%s;%s\n", e.cnpj, e.cpf, e.nome_do_responsavel, e.nome_da_empresa, e.abertura, e.email, e.endereco, e.nome_fantasia, e.razao_social);

    fprintf(f, "%s", linha);

    fclose(f);
    free(linha);

    return 1;
}

struct empresa registraEmpresa(char *nome_do_responsavel, char *cpf, char *nome_da_empresa, char *cnpj, char *razao_social, char *nome_fantasia, char *endereco, char *email, char *abertura)
{
    struct empresa e;

    e.nome_do_responsavel = nome_do_responsavel;
    e.cpf = cpf;
    e.nome_da_empresa = nome_da_empresa;
    e.cnpj = cnpj;
    e.razao_social = razao_social;
    e.nome_fantasia = nome_fantasia;
    e.endereco = endereco;
    e.email = email;
    e.abertura = abertura;

    return e;
}

int usuarioExistente(char *nome, char *linha)
{
    char *token = strtok(linha, ";");
    if (strcmp(nome, token) == 0)
    {
        return 1;
    }

    return 0;
}

int verificaSenhaUsuario(char *nome, char *senha, char *linha)
{
    char entrada[128];
    sprintf(entrada, "%s;%s", nome, senha);

    if (strcmp(linha, entrada) == 0)
    {
        return 1;
    }

    return 0;
}

int conectaUsuario(char *nome, char *senha)
{
    FILE *f;
    char linha[128];

    f = fopen("funcionarios.txt", "r");
    if (f == 0)
    {
        return 0;
    }

    while (fscanf(f, "%s\n", linha) > 0)
    {
        char copia[128];
        strcpy(copia, linha);

        if (!usuarioExistente(nome, copia))
        {
            continue;
        }

        if (verificaSenhaUsuario(nome, senha, linha))
        {
            return 1;
        }
    }
    fclose(f);

    return 0;
}

int salvaUsuario(struct usuario u)
{
    char *linha = malloc(sizeof(u));
    FILE *arquivo = fopen("funcionarios.txt", "a");

    sprintf(linha, "%s;%s\n", u.nome, u.senha);
    fprintf(arquivo, "%s", linha);

    free(linha);
    fclose(arquivo);

    return 0;
}

struct usuario registraUsuario(char *nome, char *senha)
{
    struct usuario fabricado;

    fabricado.nome = nome;
    fabricado.senha = senha;

    return fabricado;
}

void escreveInstrucao(char *instrucao[], unsigned int tamanho)
{
    int i;
    CLEAR_SCREEN();

    escreveFrase("================================================== TerraVerde System ==================================================");

    for (i = 0; i < tamanho; i++)
    {
        if (i == 0)
        {
            printf("[!] %s\n\n", instrucao[i]);
            continue;
        }

        escreveFrase(instrucao[i]);
    }
}

void telaInicial()
{
    char *instrucao[] = {"--------------------------------------------- Bem-vindo a TerraVerde! ------------------------------------------", "Você possui um login?", "[1] SIM", "[2] NÃO", "[3] Encerrar o programa"};
    size_t tamanho_da_instrucao = sizeof(instrucao) / sizeof(instrucao[0]);
    uint8_t escolha;

    escreveInstrucao(instrucao, tamanho_da_instrucao);
    scanf("%hhu", &escolha);

    switch (escolha)
    {
    case 1:
        telaLoginUsuario();
        break;
    case 2:
        telaRegistraUsuario();
        break;
    case 3:
        printf("\nEncerrando o programa...\n");
        exit(0);
        break;
    default:
        puts("Opção inválida!");
        break;
    }
}

void telaRegistroEmpresaSucesso()
{
    char *instrucao[] = {"Empresa registrada com sucesso!.", "Agora é possível gerenciar os dados", "Redirecionando para o menu principal..."};
    size_t tamanho_da_instrucao = sizeof(instrucao) / sizeof(instrucao[0]);
    escreveInstrucao(instrucao, tamanho_da_instrucao);

    sleep(4);
    telaSistema();
}

void telaRegistroEmpresa()
{
    char nome_do_responsavel[64], cpf[32], nome_da_empresa[64], cnpj[32], razao_social[64], nome_fantasia[64], endereco[256], email[64], abertura[64];
    struct empresa e;
    struct relatorio r;
    char *instrucao[] = {"------------------------------------------- REGISTRE UMA NOVA EMPRESA -------------------------------------------", "Preencha o formulário de cadastro de acordo com as solicitações do sistema."};
    size_t tamanho_da_instrucao = sizeof(instrucao) / sizeof(instrucao[0]);

    escreveInstrucao(instrucao, tamanho_da_instrucao);

    escreveFrase("Resposável pelas operações: ");
    getchar();
    scanf("%63[0-9a-zA-Z ]", nome_do_responsavel);

    escreveFrase("CPF do Responsável (somente números): ");
    getchar();
    scanf("%31[0-9a-zA-Z ]", cpf);

    escreveFrase("Empresa:");
    getchar();
    scanf("%63[0-9a-zA-Z ]", nome_da_empresa);

    escreveFrase("CNPJ da Empresa (somente numeros): ");
    getchar();
    scanf("%31[0-9a-zA-Z ]", cnpj);

    escreveFrase("Razão Social: ");
    getchar();
    scanf("%63[0-9a-zA-Z ]", razao_social);

    escreveFrase("Nome Fantasia: ");
    getchar();
    scanf("%63[0-9a-zA-Z ]", nome_fantasia);

    escreveFrase("Endereço da Empresa: ");
    getchar();
    fgets(endereco, sizeof(endereco), stdin);

    escreveFrase("E-mail: ");
    getchar();
    scanf("%63s", email);

    escreveFrase("Data de abertura: ");
    getchar();
    scanf("%63s", abertura);

    e = registraEmpresa(nome_do_responsavel, cpf, nome_da_empresa, cnpj, razao_social, nome_fantasia, endereco, email, abertura);
    r = registraRelatorio(e.cnpj, 0, 0);

    salvaEmpresa(e);
    salvaRelatorio(r);
    telaRegistroEmpresaSucesso();
}

int empresaExistente(char *cnpj)
{
    char nome_arquivo[64];
    char linha[128];
    FILE *f;
    char *byte;
    char *sigla = "relatorio";
    char *extensao = "txt";

    sprintf(nome_arquivo, "%s-%s.%s", sigla, cnpj, extensao);

    f = fopen(nome_arquivo, "r");

    if (f == 0)
    {
        return 0;
    }

    fscanf(f, "%s\n", linha);
    byte = strtok(linha, ";");

    if (strcmp(cnpj, byte) == 0)
    {
        fclose(f);
        return 1;
    }

    fclose(f);

    return 0;
}

void telaEmpresaNaoExiste()
{
    char *instrucao[] = {"CNPJ inválido, tente novamente ou rigistre a empresa", "Redirecionando para o menu principal..."};
    size_t tamanho_da_instrucao = sizeof(instrucao) / sizeof(instrucao[0]);
    escreveInstrucao(instrucao, tamanho_da_instrucao);

    sleep(4);
    telaSistema();
}

char **cortarLinha(char *linha, unsigned int tamanho)
{
    char **linhas = malloc(sizeof(char *) * tamanho);
    int index = 0;
    char *token = strtok(linha, ";");

    while (token != NULL)
    {
        linhas[index++] = token;
        token = strtok(NULL, ";");
    }

    return linhas;
}

struct relatorio carregaRelatorio(char *cnpj)
{
    struct relatorio r;
    char caminho[64];
    char linha[128];
    FILE *f;
    char **linhas;
    char *insumo_semestral;
    char *gasto_semestral;
    char *sigla = "relatorio";
    char *extensao = "txt";

    if (!empresaExistente(cnpj))
    {
        telaEmpresaNaoExiste();
    };

    sprintf(caminho, "%s-%s.%s", sigla, cnpj, extensao);
    f = fopen(caminho, "r");

    fscanf(f, "%s\n", linha);

    linhas = cortarLinha(linha, 3);

    insumo_semestral = linhas[1];
    gasto_semestral = linhas[2];

    r.cnpj = cnpj;
    r.total_insumos_semestrais = atoi(insumo_semestral);
    r.total_gastos_semestrais = atoi(gasto_semestral);

    free(linhas);

    return r;
}

char *capturarLinha(FILE *f, char *cnpj)
{
    char linha[256];
    while (fgets(linha, sizeof(linha), f))
    {
        char *copia = malloc(sizeof(linha));
        char **tokens;
        strcpy(copia, linha);

        tokens = cortarLinha(linha, 9);
        if (strcmp(tokens[0], cnpj) == 0)
        {
            return copia;
        }
        else
        {
            free(copia);
        }
    }
    return "";
}

struct empresa carregaEmpresa(char *cnpj)
{
    struct empresa e;
    char *caminho = "empresas-cadastradas.txt";
    FILE *f;
    char *linha;
    char **tokens;

    if (!empresaExistente(cnpj))
    {
        telaEmpresaNaoExiste();
    }

    f = fopen(caminho, "r");

    linha = capturarLinha(f, cnpj);

    tokens = cortarLinha(linha, 9);

    e.cnpj = tokens[0];
    e.cpf = tokens[1];
    e.nome_do_responsavel = tokens[2];
    e.nome_da_empresa = tokens[3];
    e.abertura = tokens[4];
    e.email = tokens[5];
    e.endereco = tokens[6];
    e.nome_fantasia = tokens[7];
    e.razao_social = tokens[8];

    return e;
}

void telaRelatorio(struct empresa e, struct relatorio r)
{
    char *instrucao[] = {"------------------------------- RELATÓRIO SEMESTRAL ----------------------------------", "Aqui estão os dados semestrais da empresa que você solicitou: "};
    size_t tamanho_da_instrucao = sizeof(instrucao) / sizeof(instrucao[0]);
    char insumos[64];
    char gastos[64];
    char nome_da_empresa[64];
    char nome_do_responsavel[64];
    char endereco[128];
    const char *insumosPrefix = "Kg de insumos produzidos neste semestre: ";
    const char *relacao_gasto_semestral = "Despesas em reais da Empresa: ";
    const char *ralacao_nome_resposavel = "Nome do reponsável pelas operações: ";
    const char *relacao_nome_empresa = "Empresa: ";
    const char *relacao_endereco = "Endereço: ";
    escreveInstrucao(instrucao, tamanho_da_instrucao);

    sprintf(insumos, "%s: %i", insumosPrefix, r.total_insumos_semestrais);
    sprintf(gastos, "%s: %i", relacao_gasto_semestral, r.total_gastos_semestrais);
    sprintf(nome_do_responsavel, "%s: %s", ralacao_nome_resposavel, e.nome_do_responsavel);
    sprintf(nome_da_empresa, "%s: %s", relacao_nome_empresa, e.nome_da_empresa);
    sprintf(endereco, "%s: %s", relacao_endereco, e.endereco);

    escreveFrase(nome_da_empresa);
    escreveFrase(nome_do_responsavel);
    escreveFrase(endereco);
    escreveFrase(insumos);
    escreveFrase(gastos);

    escreveFrase("Para continuar pressione a tecla [ENTER]...");
    getchar();
    getchar();
}

void telaSolicitaCnpjRelatorio()
{
    char *instrucao[] = {"------------------------------------ RELATORIO SEMESTRAL -----------------------------------"};
    size_t tamanho_da_instrucao = sizeof(instrucao) / sizeof(instrucao[0]);
    char cnpj[32];
    struct empresa e;
    struct relatorio r;
    escreveInstrucao(instrucao, tamanho_da_instrucao);

    escreveFrase("Digite o CNPJ da empresa desejada para exibir o relatório completo: ");
    scanf("%s", cnpj);

    if (!empresaExistente(cnpj))
    {
        telaEmpresaNaoExiste();
    }

    e = carregaEmpresa(cnpj);
    r = carregaRelatorio(cnpj);

    telaRelatorio(e, r);
}

void telaRelatorioEditadoComSucesso()
{
    char *instrucao[] = {"O reltório foi alterado e exportado com sucesso!", "Os relátorios seguem o formato relatório-{CNPJ}.txt", "Redirecionando para o menu principal..."};
    size_t tamanho_da_instrucao = sizeof(instrucao) / sizeof(instrucao[0]);
    escreveInstrucao(instrucao, tamanho_da_instrucao);

    sleep(4);
    telaSistema();
}

void telaEditarRelatorio()
{
    char *instrucao[] = {"------------------------------ GERENCIAMENTO DE RELATÓRIOS ------------------------------", "Os relatórios serão exportados automaticamente!"};
    size_t tamanho_da_instrucao = sizeof(instrucao) / sizeof(instrucao[0]);
    char cnpj[24];
    unsigned int insumo_semestral, gastosMensais;
    struct relatorio r;
    escreveInstrucao(instrucao, tamanho_da_instrucao);

    escreveFrase("Digite o CNPJ da empresa desejada:");
    scanf("%23s", cnpj);

    if (!empresaExistente(cnpj))
    {
        telaEmpresaNaoExiste();
    }

    escreveFrase("Digite a quantidade em KG de insumos produzidos no semestre: ");
    scanf("%i", &insumo_semestral);

    escreveFrase("Digite o valor em reais das despesas semestrais: ");
    scanf("%i", &gastosMensais);

    r = registraRelatorio(cnpj, insumo_semestral, gastosMensais);

    salvaRelatorio(r);

    telaRelatorioEditadoComSucesso();
}

void telaSistema()
{
    char *instrucao[] = {"------------------------------------------------ MENU PRINCIPAL ------------------------------------------------", "Navegue pelo sistema utilizando as teclas indicadas com [] ao lado de cada funcionalidade", "[1] Registre uma nova empresa", "[2] Edite e exporte relatórios das empresas", "[3] Exibir relátorio das empresas", "[4] Encerrar sistema"};
    size_t tamanho_da_instrucao = sizeof(instrucao) / sizeof(instrucao[0]);
    uint8_t escolha;
    escreveInstrucao(instrucao, tamanho_da_instrucao);

    getchar();
    scanf("%hhu", &escolha);

    switch (escolha)
    {
    case 1:
        telaRegistroEmpresa();
        break;
    case 2:
        telaEditarRelatorio();
        break;
    case 3:
        telaSolicitaCnpjRelatorio();
        break;
    case 4:
        exit(0);
        break;
    default:
        puts("Opção invalida...");
    }
}

void telaRegistroSucesso()
{
    char *instrucao[] = {"Usuário registrado com sucesso!", "Redirecionando para o menu inicial..."};
    size_t tamanho_da_instrucao = sizeof(instrucao) / sizeof(instrucao[0]);

    escreveInstrucao(instrucao, tamanho_da_instrucao);

    sleep(3);

    telaInicial();
}

void telaRegistraUsuario()
{
    char nome[48], senha[64];
    struct usuario u;
    char *instrucao[] = {"Para efetuar um login o usuário precisa estar previamente cadastrado.", "Cadastre um novo usuário: "};
    size_t tamanho_da_instrucao = sizeof(instrucao) / sizeof(instrucao[0]);
    escreveInstrucao(instrucao, tamanho_da_instrucao);

    escreveFrase("Digite um nome de usuário: ");
    scanf("%s", nome);

    escreveFrase("Digite uma senha: ");
    scanf("%s", senha);

    u = registraUsuario(nome, senha);
    salvaUsuario(u);

    telaRegistroSucesso();
}

void telaLoginSucesso()
{
    char *instrucao[] = {"Login realizado com sucesso!", "Bem-vindo!", "Redirecionando para o menu..."};
    size_t tamanho_da_instrucao = sizeof(instrucao) / sizeof(instrucao[0]);
    escreveInstrucao(instrucao, tamanho_da_instrucao);

    sleep(6);
    telaSistema();
}

void telaLoginIncorreto()
{
    char *instrucao[] = {"Desculpe! Usuário ou senha inválidos.", "Por favor, tente novamente", "Redirecionando para o menu principal..."};
    size_t tamanho_da_instrucao = sizeof(instrucao) / sizeof(instrucao[0]);
    escreveInstrucao(instrucao, tamanho_da_instrucao);

    sleep(6);
    telaInicial();
}

void telaLoginUsuario()
{
    char nome[48], senha[64];
    char *instrucao[] = {"Efetue o login"};
    size_t tamanho_da_instrucao = sizeof(instrucao) / sizeof(instrucao[0]);
    escreveInstrucao(instrucao, tamanho_da_instrucao);

    escreveFrase("Digite o nome de usuário: ");
    scanf("%s", nome);

    escreveFrase("Digite a senha: ");
    scanf("%s", senha);

    if (!conectaUsuario(nome, senha))
    {
        telaLoginIncorreto();
    }

    telaLoginSucesso();
}
