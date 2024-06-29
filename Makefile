generate: client server

client: client/basic_math_program.c client/is_prime_rpc_client.c 
	@gcc client/basic_math_program.c client/is_prime_rpc_client.c -Wall -Wextra -o client/basic_math_program_distributed 

server: server/is_prime_rpc_server.c is_prime.c 
	@gcc server/is_prime_rpc_server.c is_prime.c -Wall -Wextra -o server/server

clean:
	@echo "Cleaning up..."
	@rm -f server/server
	@rm -f client/basic_math_program_distributed

.DEFAULT_GOAL := generate