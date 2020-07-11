////////////////////////////////
/// Inclus�o das bibliotecas ///
////////////////////////////////
#include <string>
#include <iostream>

// Se estiver rodando no windows
// precisamos incluir a biblioteca
// de modo diferente
#ifdef __WIN32
	#include <mysql.h>
#else
	// Em outros sistemas, creio eu,
	// � desse outro jeito
	#include <mysql/mysql.h>
#endif

///////////////////
/// Prot�tipos ////
///////////////////
void exibeMenu(); 
void desconectar();
void verCompromissos();
void mostrarErroDoMysql();
void adicionarCompromisso();

/////////////////////////////
/// Vari�veis globais X<( ///
/////////////////////////////

/**
 * Cont�m a conex�o com o banco de dados
 */

 *connexao;
bool conectado = false;

//////////////////
/// Constantes ///
//////////////////

const unsigned int ADICIONAR_COMPROMISSO = 0;
const unsigned int REMOVER_COMPROMISSO = 1;
const unsigned int MOSTRAR_COMPROMISSO = 2;
const unsigned int EDITAR_COMPROMISSO = 3;
const unsigned int SAIR = 4;

/**
 * Fun��o principal
 */
int main(int argc, char **argv) {

	// Exibe a mensagem de boas vindas
	std::cout << "Boas vindas da agenda!" << std::endl;

	exibeMenu();

	// Guarda a op��o escolhida
	int opcao;

	// Entra em um loop infinito perguntando
	// o que o usu�rio deseja fazer
	while (true) {

		// Pergunta a op��o do usu�rio
		std::cin >> opcao;

		// Executa a a��o escolhida
		switch (opcao) {
			case ADICIONAR_COMPROMISSO:
				adicionarCompromisso();
				break;
			case REMOVER_COMPROMISSO:
				exlcuiCompromisso();
				break;
			case MOSTRAR_COMPROMISSO:
				verCompromissos();
				break;
			case EDITAR_COMPROMISSO:
				editaCompromisso();
				break;
		}

		// Exibe o menu novamente
		exibeMenu();
	}

	desconectar();
	return 0;
}

/**
 * Exibe um menu para o usu�rio
 */
void exibeMenu() {
	std::cout << "Escolha uma op��o:" << std::endl;
	std::cout << ADICIONAR_COMPROMISSO << " - Adicionar compromisso" << std::endl;
	std::cout << REMOVER_COMPROMISSO << " - Remover compromisso" << std::endl;
	std::cout << MOSTRAR_COMPROMISSO << " - Mostrar compromisso" << std::endl;
	std::cout << EDITAR_COMPROMISSO << " - Editar compromisso" << std::endl;
	std::cout << SAIR << " - SAIR" << std::endl;
}

/**
 * Mostra os erros que podem ocorrer na conex�o
 * @param mysql
 */
void mostrarErroDoMysql(MYSQL *mysql) {
	printf("Erro: (%d) [%s] \"%s\"", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
	mysql_close(mysql);
	exit(-1);
}

/**
 * Conecta no banco de dados
 * @return - true: Conectou, false: Falha ao conectar
 */
bool conectar() {

	if (conectado) return true;

	connexao = mysql_init(NULL);
	if (!mysql_real_connect(connexao, "127.0.0.1", "root", "1234", "Agenda", 3306, NULL, 0)) {
		mostrarErroDoMysql(connexao);

		conectado = false;
		return false;
	}

	conectado = true;
	return true;
}

/**
 * Desconecta do banco de dados
 */
void desconectar() {
	mysql_close(connexao);
	conectado = false;
}

/**
 * Adiciona um compromisso
 */
void adicionarCompromisso() {

	// Verifica se a conexão foi realizada com sucesso
	if (!conectar()) {
		std::cout << "Falha ao conectar ao banco de dados!" << std::endl;
		return;
	}

	// Variáveis usadas para montar o query
	std::string dia, mes, ano, descricao;

	// Solicita o dia do compromisso
	std::cout << "Informe o dia:";
	std::cin >> dia;

	// Solicita o m�s do compromisso
	std::cout << "Informe o m�s:";
	std::cin >> mes;

	// Solicita o ano do compromisso
	std::cout << "Informe o ano:";
	std::cin >> ano;

	// Solicita a descri��o do compromisso
	std::cout << "Descreva o compromisso:";
	std::cin >> descricao;

	// Monta a query
	std::string sql = "insert into Compromisso (data, descricao)values('" + ano + "-" + mes + "-" + dia + "','" + descricao + "')";

	// Executa a query
	int statusDeExecucao = mysql_query(connexao, sql.data());

	// Verifica se deu tudo certo
	if (statusDeExecucao == 0) {
		// Deu certo!
		std::cout << "Compromisso inserido" << std::endl;
	} else {
		// Algo deu errado!
		mostrarErroDoMysql(connexao);
		std::cout << "Falha ao inserir compromisso!" << std::endl;
	}

}

void verCompromissos() {

	// Verifica se a conex�o foi realizada com sucesso
	if (!conectar()) {
		std::cout << "Falha ao conectar ao banco de dados!" << std::endl;
		return;
	}

	// Guarda um registro
	MYSQL_ROW registro;

	// Guarda todos os registros retornados
	MYSQL_RES *resultados;

	// Executa a query
	int statusDeExecucao = mysql_query(connexao, "SELECT cod, data, descricao FROM Compromisso");

	// Verifica se deu tudo certo
	if (statusDeExecucao == 0) {

		// Deu certo! Recupera os registros retornados;
		resultados = mysql_store_result(connexao);

		// Mostra os registros retornados
		while ((registro = mysql_fetch_row(resultados))) {
			std::cout << registro[0] << "\t" << registro[1] << "\t" << registro[2] << std::endl;
		}
	} else {
		// Algo deu errado!
		mostrarErroDoMysql(connexao);
		std::cout << "Falha ao recuperar os registros!" << std::endl;
	}
}

void editaCompromisso() {

	// Variáveis usadas
	std::string codigo, dia, mes, ano, descricao;

	// pergunta qual data deseja alterar
	std::cout << "Informe o dia do compromisso:";
	std::cin >> dia;
	std::cout << "Informe o mês do compromisso:";
	std::cin >> mes;
	std::cout << "Informe o ano do compromisso:";
	std::cin >> ano;

	// Executa a query
	int statusDeExecucao = mysql_query(connexao, "SELECT cod, data, descricao FROM Compromisso where data = " + ano + "-" + mes + "-" + dia);

	// Verifica se deu tudo certo, mostra o registro e pede para informar novos valores
	if (statusDeExecucao == 0) {

		// Deu certo! Recupera os registros retornados;
		resultados = mysql_store_result(connexao);

		// Mostra os registros retornados
		while ((registro = mysql_fetch_row(resultados))) {
			std::cout << registro[0] << "\t" << registro[1] << "\t" << registro[2] << std::endl;
		}

		//pergunta qual registro deseja alterar
		std::cout << "Informe qual registro deseja alterar:";
		std::cin >> codigo;

		// Solicita novo dia do compromisso
		std::cout << "Informe novo dia do compromisso:";
		std::cin >> dia;

		// Solicita novo mês do compromisso
		std::cout << "Informe novo mês do compromisso:";
		std::cin >> mes;

		// Solicita novo ano do compromisso
		std::cout << "Informe novo ano do compromisso:";
		std::cin >> ano;

		// Solicita nova descrição do compromisso
		std::cout << "Descreva novamente o compromisso:";
		std::cin >> descricao;

		// Monta a query
		std::string sql = "update Compromisso set data = " + ano + "-" + mes + "-" + dia + ", descricao = " + descricao + " where cod = " + codigo;

		// Executa a query
		int statusDeExecucao = mysql_query(connexao, sql.data());

	} else {
		// Algo deu errado!
		mostrarErroDoMysql(connexao);
		std::cout << "Falha ao recuperar os registros!" << std::endl;
	}
}

void excluiCompromisso() {

	// Variáveis usadas
	std::string codigo, dia, mes, ano, descricao;

	// pergunta qual data deseja excluir
	std::cout << "Informe o dia do compromisso:";
	std::cin >> dia;
	std::cout << "Informe o mês do compromisso:";
	std::cin >> mes;
	std::cout << "Informe o ano do compromisso:";
	std::cin >> ano;

	// Executa a query
	int statusDeExecucao = mysql_query(connexao, "SELECT cod, data, descricao FROM Compromisso where data = " + ano + "-" + mes + "-" + dia);

	// Verifica se deu tudo certo, mostra o registro e pede para informar novos valores
	if (statusDeExecucao == 0) {

		// Deu certo! Recupera os registros retornados;
		resultados = mysql_store_result(connexao);

		// Mostra os registros retornados
		while ((registro = mysql_fetch_row(resultados))) {
			std::cout << registro[0] << "\t" << registro[1] << "\t" << registro[2] << std::endl;
		}

		//pergunta qual registro deseja excluir
		std::cout << "Informe qual registro deseja excluir:";
		std::cin >> codigo;

		// Monta a query
		std::string sql = "delete from Compromisso where cod = " + codigo;

		// Executa a query
		int statusDeExecucao = mysql_query(connexao, sql.data());

	} else {
		// Algo deu errado!
		mostrarErroDoMysql(connexao);
		std::cout << "Falha ao recuperar os registros!" << std::endl;
	}
}
