#include <httplib.h>
#include <cctype>
#include <string>

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
        return 1;
    }

    std::string tipo = argv[1];
    httplib::Server svr;

    svr.Get("/health", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("OK", "text/plain");
    });

    if (tipo == "letras") {
        svr.Post("/letras", [](const httplib::Request& req, httplib::Response& res) {
            res.set_content(std::to_string(contarLetras(req.body)), "text/plain");
        });
        svr.listen("0.0.0.0", 8081);
    } else {
        svr.Post("/numeros", [](const httplib::Request& req, httplib::Response& res) {
            res.set_content(std::to_string(contarNumeros(req.body)), "text/plain");
        });
        svr.listen("0.0.0.0", 8082);
    }

    return 0;
}
