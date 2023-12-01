all:
	g++ tensor_test.cpp -std=c++17 -I ./include  -o tensor_test

tensorToJson:
	g++ tensorToJson.cpp -std=c++17 -I ./include  -o tensorToJson

test:
	g++ tensor_test.cpp -std=c++17 -I ./include  -o tensor_test
