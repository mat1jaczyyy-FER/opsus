define compile
	gcc $(1).c -pthread -o build/$(1)
endef

all:
	mkdir -p build
	$(call compile,bozicnjak)
	$(call compile,filozofi)

clean:
	rm -rf build