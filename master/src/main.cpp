#include <httplib.h>
#include <thread>
#include <future>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

bool verificarSaude(const std::string& host, int porta) {
    httplib::Client cli(host, porta);
    auto res = cli.Get("/health");
    return res && res->status == 200;
}

std::string processarEscravo(const std::string& host, int porta, 
                            const std::string& endpoint, const std::string& texto) {
    httplib::Client cli(host, porta);
    auto res = cli.Post(endpoint, texto, "text/plain");
    return res ? res->body : "erro";
}

int main() {
    httplib::Server svr;

    svr.Post("/processar", [](const httplib::Request& req, httplib::Response& res) {
        if (!verificarSaude("slave1", 8081) || !verificarSaude("slave2", 8082)) {
            res.status = 503;
            return;
        }

        auto futureLetras = std::async(std::launch::async, processarEscravo, 
            "slave1", 8081, "/letras", req.body);
        auto futureNumeros = std::async(std::launch::async, processarEscravo, 
            "slave2", 8082, "/numeros", req.body);

        json resultado;
        resultado["letras"] = futureLetras.get();
        resultado["numeros"] = futureNumeros.get();
        
        res.set_content(resultado.dump(), "application/json");
    });

    svr.listen("0.0.0.0", 8080);
    return 0;
}
