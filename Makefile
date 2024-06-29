generate:
	@gcc is_prime.c basic_math_program.c client/is_prime_rpc_client.c -Wall -Wextra -o basic_math_program

clean:
	@echo "Cleaning up..."
	@rm -f basic_math_program

.DEFAULT_GOAL := generate