#include <httplib.h>
#include <thread>
#include <future>
#include <iostream>
#include <nlohmann/json.hpp>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>

using json = nlohmann::json;

// Função para obter IP local (não loopback)
std::string obterIPLocal() {
    struct ifaddrs *ifaddr, *ifa;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return "0.0.0.0";
    }

    std::string ipLocal = "0.0.0.0";

    for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr)
            continue;

        int family = ifa->ifa_addr->sa_family;
        if (family == AF_INET) { // IPv4
            if (strcmp(ifa->ifa_name, "lo") != 0) { // Ignorar loopback
                if (getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
                                host, NI_MAXHOST, nullptr, 0, NI_NUMERICHOST) == 0) {
                    ipLocal = host;
                    break;
                }
            }
        }
    }

    freeifaddrs(ifaddr);
    return ipLocal;
}

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
    const int porta = 8080;
    const std::string endereco = "0.0.0.0";
    std::string ipLocal = obterIPLocal(); // Obtém IP real da rede

    svr.Post("/processar", [](const httplib::Request& req, httplib::Response& res) {
        std::cout << "Processando requisição..." << std::endl;
        
        if (!verificarSaude("slave1", 8081) || !verificarSaude("slave2", 8082)) {
            std::cout << "Erro: Slaves não disponíveis" << std::endl;
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
        
        std::cout << "Resultado: " << resultado.dump() << std::endl;
        res.set_content(resultado.dump(), "application/json");
    });

    std::cout << "==================================" << std::endl;
    std::cout << "🚀 Servidor Master iniciado!" << std::endl;
    std::cout << "📍 Endereço: " << endereco << std::endl;
    std::cout << "🔌 Porta: " << porta << std::endl;
    std::cout << "🌐 IP local: " << ipLocal << std::endl; // Mostra IP real
    std::cout << "🌐 URL local: http://" << ipLocal << ":" << porta << std::endl;
    std::cout << "==================================" << std::endl;

    svr.listen(endereco.c_str(), porta);
    return 0;
}
