LOGIN ?= $(shell whoami)
DATA_PATH = /home/$(LOGIN)/data
DOMAIN = $(LOGIN).42.fr

all:
	@if ! grep -q "$(DOMAIN)" /etc/hosts; then \
		echo "127.0.0.1 $(DOMAIN)" >> /etc/hosts; \
	fi
	@if ! grep -q "www.$(DOMAIN)" /etc/hosts; then \
		echo "127.0.0.1 www.$(DOMAIN)" >> /etc/hosts; \
	fi
	mkdir -p $(DATA_PATH)/wordpress $(DATA_PATH)/mariadb
	@chmod -R 755 $(DATA_PATH)
	docker-compose -f srcs/docker-compose.yml build
	docker-compose -f srcs/docker-compose.yml up -d
	
clean:
	docker-compose -f srcs/docker-compose.yml down --rmi all -v

fclean: clean
	sudo rm -rf $(DATA_PATH)/wordpress $(DATA_PATH)/mariadb
	docker system prune -f

re: fclean all

up:
	docker-compose -f srcs/docker-compose.yml up -d

down:
	docker-compose -f srcs/docker-compose.yml down

.PHONY: all clean fclean re up down