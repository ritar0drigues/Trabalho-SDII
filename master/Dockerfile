FROM gcc:latest

WORKDIR /app

# Criar estrutura de diretórios
RUN mkdir -p include/nlohmann

# Instalar wget
RUN apt-get update && apt-get install -y wget

# Baixar os headers necessários com a estrutura correta
RUN wget https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h -O include/httplib.h && \
    wget https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp -O include/nlohmann/json.hpp

# Copiar os arquivos do projeto
COPY CMakeLists.txt .
COPY src/ src/

# Compilar
RUN apt-get update && \
    apt-get install -y cmake && \
    cmake . && \
    make

EXPOSE 8080
CMD ["./master"]
