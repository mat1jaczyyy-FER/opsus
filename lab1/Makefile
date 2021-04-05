define compile
	gcc $(1).c -lm -o build/$(1)
endef

all:
	mkdir -p build
	$(call compile,prosti_brojevi)
	$(call compile,obrada)
	$(call compile,generator_prekida)

clean:
	rm -rf build