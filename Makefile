.PHONY: all clean setup-client setup-master setup-slave build run stop run-client

all: setup-client setup-master setup-slave build

# Configuração do Cliente GTK
setup-client:
	mkdir -p cliente/include/nlohmann
	mkdir -p cliente/build
	wget -nc -P cliente/include https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h
	wget -nc -P cliente/include/nlohmann https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp
	cd cliente/build && cmake .. && make

# Configuração do Master
setup-master:
	mkdir -p master/include/nlohmann
	wget -nc -P master/include https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h
	wget -nc -P master/include/nlohmann https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp

# Configuração do Slave
setup-slave:
	mkdir -p slave/include
	wget -nc -P slave/include https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h

# Compilar cliente local (para desenvolvimento)
build-client-local:
	sudo apt-get update && sudo apt-get install -y libgtk-3-dev pkg-config
	cd cliente && mkdir -p build && cd build && cmake .. && make

# Executar cliente local
run-client: build-client-local
	cd cliente/build && ./cliente

# Gerenciar rede
clean-network:
	docker network prune -f
	docker compose down --remove-orphans
	docker network rm trabalho_app-network 2>/dev/null || true

# Construir containers
build: clean-network
	docker compose build

# Iniciar sistema (servidores)
run: clean-network
	docker compose up --build

# Executar cliente GTK com Docker (requer X11 forwarding)
run-client-docker:
	docker run -it --rm \
		-e DISPLAY=$$DISPLAY \
		-v /tmp/.X11-unix:/tmp/.X11-unix:rw \
		--network host \
		cliente_gtk

# Parar sistema
stop:
	docker compose down

# Limpar tudo
clean: clean-network
	rm -rf cliente/build
	rm -rf cliente/include
	rm -rf master/include
	rm -rf slave/include
	docker compose down --rmi all

# Preparar ambiente para desenvolvimento
dev-setup:
	sudo apt-get update && sudo apt-get install -y \
		build-essential \
		cmake \
		pkg-config \
		libgtk-3-dev \
		wget