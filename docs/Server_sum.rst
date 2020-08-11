***************
Server_sum 
***************
* Communication between server and client is realise by TCP protocol. Client sends lines that contains numbers, for each line server return line that holds one number(sum) or error statement.
* Each line ends with lineterminator (\r\n)
* Line can only contains digits, whitespaces. whitespace is nothing else then delimiter.
* Between two numbers there can only be one whitespace.
* If we cannot correctly define sum, server sends error statement.
* You can test server with (netcat)[https://nmap.org/ncat/]

