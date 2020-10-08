# bounded-buffer


#### How to run it...
- gcc -c ip_checksum.c
- gcc producer.c -o producer ip_checksum.o
- gcc consumer.c -o consumer ip_checksum.o
- ./producer
- ./consumer
