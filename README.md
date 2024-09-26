# MOCHILADB


## Valgrind command

valgrind --tool=helgrind --read-var-info=yes ./bin/mochiladb 2> helgrind_output.txt


valgrind --tool=memcheck --leak-check=full --track-origins=yes ./bin/mochiladb

valgrind --tool=memcheck --leak-check=full --track-origins=yes --show-reachable=yes ./bin/mochiladb

## Dockerization

docker build -t mochila-i .
docker run --name mochila-i-con -d -p 7986:7986 mochila-i:latest
