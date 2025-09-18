#include <httplib.h>
#include <iostream>
#include <fstream>
#include <string>

std::string lerArquivo(const std::string& caminho) {
    std::ifstream arquivo(caminho);
    return std::string((std::istreambuf_iterator<char>(arquivo)),
                       std::istreambuf_iterator<char>());
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Uso: " << argv[0] << " <arquivo.txt>" << std::endl;
        return 1;
    }

    httplib::Client cli("http://localhost:8080");
    
    auto conteudo = lerArquivo(argv[1]);
    auto res = cli.Post("/processar", conteudo, "text/plain");
    
    if (res && res->status == 200) {
        std::cout << "Resultado: " << res->body << std::endl;
    } else {
        std::cout << "Erro na requisição" << std::endl;
    }

    return 0;
}
