all: rsatest oaeptest rsa_genkey

CXX = g++
CXXFLAGS = -std=c++11 -pipe -Wall -O3 -march=native
LDFLAGS = -lgmp -lgmpxx

rsatest: rsatest.o util.o bigint.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

rsatest.o: rsatest.cc include/rsa.hpp include/bigint.hpp include/config.hh
	$(CXX) $(CXXFLAGS) -c -o $@ $<

oaeptest: oaeptest.o util.o bigint.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

oaeptest.o: oaeptest.cc include/oaep.hpp include/rsa.hpp include/bigint.hpp include/config.hh
	$(CXX) $(CXXFLAGS) -c -o $@ $<

util.o: util.cc include/util.hpp include/config.hh
	$(CXX) $(CXXFLAGS) -c -o $@ $<

bigint.o: bigint.cc include/bigint.hpp include/config.hh
	$(CXX) $(CXXFLAGS) -c -o $@ $<

rsa_genkey: rsa_genkey.o util.o bigint.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

rsa_genkey.o: rsa_genkey.cc include/rsa.hpp include/bigint.hpp include/config.hh
	$(CXX) $(CXXFLAGS) -c -o $@ $<

.PHONY: clean

clean:
	rm -rf *.o
