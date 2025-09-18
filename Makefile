.PHONY: all clean setup-client setup-master setup-slave build run stop

all: setup-client setup-master setup-slave build

# Configuração do Cliente
setup-client:
	mkdir -p cliente/include
	mkdir -p cliente/build
	wget -nc -P cliente/include https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h
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

# Gerenciar rede
clean-network:
	docker network prune -f
	docker compose down --remove-orphans
	docker network rm trabalho_app-network 2>/dev/null || true

# Construir containers
build: clean-network
	docker compose build

# Iniciar sistema
run: clean-network
	docker compose up

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
