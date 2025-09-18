FROM gcc:latest

WORKDIR /app

# Criar diretório include e copiar headers primeiro
RUN mkdir -p include

# Instalar wget para baixar os headers
RUN apt-get update && apt-get install -y wget

# Baixar os headers necessários
RUN wget https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h -O include/httplib.h

# Copiar os arquivos do projeto
COPY CMakeLists.txt .
COPY src/ src/

# Compilar
RUN apt-get update && \
    apt-get install -y cmake && \
    cmake . && \
    make

EXPOSE 8081 8082
CMD ["./slave"]
