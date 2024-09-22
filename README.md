# MOCHILADB


## Valgrind command

valgrind --tool=helgrind --read-var-info=yes ./bin/mochiladb 2> helgrind_output.txt


valgrind --tool=memcheck --leak-check=full --track-origins=yes ./bin/mochiladb

valgrind --tool=memcheck --leak-check=full --track-origins=yes --show-reachable=yes ./bin/mochiladb