#include <httplib.h>
#include <cctype>
#include <string>
#include <iostream>

int contarLetras(const std::string& texto) {
    return std::count_if(texto.begin(), texto.end(), [](char c) { 
        return std::isalpha(c); 
    });
}

int contarNumeros(const std::string& texto) {
    return std::count_if(texto.begin(), texto.end(), [](char c) { 
        return std::isdigit(c); 
    });
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Uso: " << argv[0] << " <tipo>" << std::endl;
        return 1;
    }

    std::string tipo = argv[1];
    httplib::Server svr;
    int porta;
    std::string funcao;

    svr.Get("/health", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("OK", "text/plain");
    });

    if (tipo == "letras") {
        porta = 8081;
        funcao = "Contador de Letras";
        svr.Post("/letras", [](const httplib::Request& req, httplib::Response& res) {
            int resultado = contarLetras(req.body);
            std::cout << "Contando letras: " << resultado << std::endl;
            res.set_content(std::to_string(resultado), "text/plain");
        });
    } else {
        porta = 8082;
        funcao = "Contador de Números";
        svr.Post("/numeros", [](const httplib::Request& req, httplib::Response& res) {
            int resultado = contarNumeros(req.body);
            std::cout << "Contando números: " << resultado << std::endl;
            res.set_content(std::to_string(resultado), "text/plain");
        });
    }

    std::cout << "==================================" << std::endl;
    std::cout << "⚡ Slave " << tipo << " iniciado!" << std::endl;
    std::cout << "🔧 Função: " << funcao << std::endl;
    std::cout << "📍 Endereço: 0.0.0.0" << std::endl;
    std::cout << "🔌 Porta: " << porta << std::endl;
    std::cout << "==================================" << std::endl;

    svr.listen("0.0.0.0", porta);
    return 0;
}