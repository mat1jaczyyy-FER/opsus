define compile
	gcc $(1).c -pthread -o build/$(1)
endef

all:
	mkdir -p build
	$(call compile,dretve)
	$(call compile,procesi)
	$(call compile,Lamport)

clean:
	rm -rf build