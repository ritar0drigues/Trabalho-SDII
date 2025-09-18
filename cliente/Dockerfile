FROM ubuntu:20.04

# Evitar prompts interativos durante a instalação
ENV DEBIAN_FRONTEND=noninteractive

WORKDIR /app

# Instalar dependências
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    libgtk-3-dev \
    wget \
    && rm -rf /var/lib/apt/lists/*

# Criar estrutura de diretórios
RUN mkdir -p include/nlohmann

# Baixar headers necessários
RUN wget https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h -O include/httplib.h && \
    wget https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp -O include/nlohmann/json.hpp

# Copiar arquivos do projeto
COPY CMakeLists.txt .
COPY src/ src/

# Compilar
RUN cmake . && make

# Configurar display para X11 forwarding
ENV DISPLAY=:0

CMD ["./cliente"]